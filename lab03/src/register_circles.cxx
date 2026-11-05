#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkCastImageFilter.h>
#include <itkRescaleIntensityImageFilter.h>
#include <itkImageRegistrationMethodv4.h>
#include <itkSimilarity2DTransform.h>
#include <itkCenteredTransformInitializer.h>
#include <itkMeanSquaresImageToImageMetricv4.h>
#include <itkRegularStepGradientDescentOptimizerv4.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkResampleImageFilter.h>
#include <itkMultiThreaderBase.h>

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>

// ── Type aliases ──────────────────────────────────────────────────────────────
using PixelType        = float;
using ImageType        = itk::Image<PixelType, 2>;
using TransformType    = itk::Similarity2DTransform<double>;
using OptimizerType    = itk::RegularStepGradientDescentOptimizerv4<double>;
using MetricType       = itk::MeanSquaresImageToImageMetricv4<ImageType, ImageType>;
using InterpolatorType = itk::LinearInterpolateImageFunction<ImageType, double>;
using RegistrationType = itk::ImageRegistrationMethodv4<ImageType, ImageType, TransformType>;

// ── Per-iteration observer ────────────────────────────────────────────────────
class IterationObserver : public itk::Command
{
public:
    using Self       = IterationObserver;
    using Superclass = itk::Command;
    using Pointer    = itk::SmartPointer<Self>;
    itkNewMacro(Self);

    unsigned int level{ 0 };
    double bestMetric{ std::numeric_limits<double>::max() };

    void Execute(itk::Object* caller, const itk::EventObject& event) override
    {
        Execute(static_cast<const itk::Object*>(caller), event);
    }
    void Execute(const itk::Object* obj, const itk::EventObject& event) override
    {
        const auto* opt = dynamic_cast<const OptimizerType*>(obj);
        if (!opt || !itk::IterationEvent().CheckEvent(&event)) { return; }
        const double v = opt->GetValue();
        if (v < bestMetric) { bestMetric = v; }
        std::cout << "  [L" << level << "] iter " << std::setw(4)
                  << opt->GetCurrentIteration()
                  << "  metric " << std::setw(12) << std::setprecision(6) << v
                  << "  step "   << opt->GetCurrentStepLength() << "\n";
    }
};

// ── Per-pyramid-level observer ────────────────────────────────────────────────
class LevelObserver : public itk::Command
{
public:
    using Self       = LevelObserver;
    using Superclass = itk::Command;
    using Pointer    = itk::SmartPointer<Self>;
    itkNewMacro(Self);

    IterationObserver* iterObs{ nullptr };

    void Execute(itk::Object* c, const itk::EventObject& e) override
    {
        Execute(static_cast<const itk::Object*>(c), e);
    }
    void Execute(const itk::Object*, const itk::EventObject& event) override
    {
        if (!itk::MultiResolutionIterationEvent().CheckEvent(&event)) { return; }
        if (iterObs) { ++iterObs->level; }
        std::cout << "\n--- pyramid level " << (iterObs ? iterObs->level : 0u)
                  << " ---\n";
    }
};

// ── Normalize intensity to [0,1] — no Gaussian smoothing ─────────────────────
// Smoothing is intentionally omitted here. Applying a fixed physical-sigma
// Gaussian before registration creates a scale-dependent metric floor: after
// mapping with scale≈2, img1 (σ=2mm edge) is compared against img2 whose
// σ=2mm edge appears as σ=1mm in fixed-image space. The two edges never match,
// so MSE can never reach 0 at the correct transform.
// The multi-resolution pyramid's built-in smoothing handles the coarse-to-fine
// strategy without this artefact.
static ImageType::Pointer normalizeImage(ImageType::Pointer input)
{
    using RescaleType = itk::RescaleIntensityImageFilter<ImageType, ImageType>;
    auto rescale = RescaleType::New();
    rescale->SetInput(input);
    rescale->SetOutputMinimum(0.0f);
    rescale->SetOutputMaximum(1.0f);
    rescale->Update();
    return rescale->GetOutput();
}

// ── Load PNG as float ─────────────────────────────────────────────────────────
static ImageType::Pointer loadImage(const std::string& path)
{
    using ByteType   = itk::Image<unsigned char, 2>;
    using ReaderType = itk::ImageFileReader<ByteType>;
    using CastType   = itk::CastImageFilter<ByteType, ImageType>;

    auto reader = ReaderType::New();
    reader->SetFileName(path);

    auto cast = CastType::New();
    cast->SetInput(reader->GetOutput());
    cast->Update();
    return cast->GetOutput();
}

int main(int argc, char* argv[])
{
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0]
                  << " <fixed.png> <moving.png> [output.png]\n";
        return EXIT_FAILURE;
    }

    const std::string fixedPath  = argv[1];
    const std::string movingPath = argv[2];
    const std::string outputPath = (argc > 3) ? argv[3] : "registered.png";

    itk::MultiThreaderBase::SetGlobalMaximumNumberOfThreads(1);

    try {
        // ── Load & normalize ──────────────────────────────────────────────────
        auto fixed  = normalizeImage(loadImage(fixedPath));
        auto moving = normalizeImage(loadImage(movingPath));

        // ── Components ────────────────────────────────────────────────────────
        auto metric       = MetricType::New();
        auto optimizer    = OptimizerType::New();
        auto transform    = TransformType::New();
        auto registration = RegistrationType::New();

        // ── Moments-based initialisation ──────────────────────────────────────
        // The circles are 150 mm apart; with an identity transform the metric
        // is undefined. CenteredTransformInitializer::MomentsOn() aligns the
        // two centres of mass and places the rotation centre at the fixed
        // image's centre of mass.
        //
        // For these images (fixed centroid (50,50), moving centroid (200,200)):
        //   centre     = (50, 50)
        //   translation = (150, 150)   [valid for any scale when centre=(50,50)]
        //   scale      = 1.0           [to be optimised]
        //   angle      = 0.0           [to be optimised]
        transform->SetScale(1.0);
        transform->SetAngle(0.0);
        {
            using InitType =
                itk::CenteredTransformInitializer<TransformType, ImageType, ImageType>;
            auto init = InitType::New();
            init->SetTransform(transform);
            init->SetFixedImage(fixed);
            init->SetMovingImage(moving);
            init->MomentsOn();
            init->InitializeTransform();
        }

        // ── Optimizer scales ──────────────────────────────────────────────────
        // Similarity2DTransform parameter order: [scale, angle, tx, ty].
        //
        // After moments init, translation is already correct (150,150) and
        // needs only tiny corrections. Scale is the primary unknown (1→2).
        // Angle must stay near 0 (circles are rotation-symmetric, so the
        // metric landscape is flat in angle; a large step causes oscillation).
        //
        // Gradient update ∝ gradient / scale (element-wise), so:
        //   Small scale[0] → large effective step for scale (desired)
        //   Large scale[1] → tiny step for angle (desired: stay at 0)
        //   Large scale[2,3] → tiny step for translation (already correct)
        {
            OptimizerType::ScalesType scales(transform->GetNumberOfParameters());
            scales[0] =     1.0;   // scale      — primary DOF
            scales[1] = 10000.0;   // angle      — suppress (circles are isotropic)
            scales[2] = 50000.0;   // tx         — locked after moments init
            scales[3] = 50000.0;   // ty
            optimizer->SetScales(scales);
        }

        // LearningRate = maximum step in normalised scale space.
        // With scales[0]=1: step of 0.5 moves scale by 0.5/iteration.
        // RelaxationFactor halves the step on direction reversals, driving
        // fine-grained convergence once the coarse basin is found.
        optimizer->SetLearningRate(0.5);
        optimizer->SetMinimumStepLength(1e-8);
        optimizer->SetNumberOfIterations(500);
        optimizer->SetRelaxationFactor(0.5);
        optimizer->SetGradientMagnitudeTolerance(1e-8);
        optimizer->ReturnBestParametersAndValueOn();

        auto iterObs = IterationObserver::New();
        optimizer->AddObserver(itk::IterationEvent(), iterObs);

        // ── Random metric sampling ────────────────────────────────────────────
        // With scale≈1.5 the similarity transform maps a large portion of the
        // fixed image's upper-right pixels outside the 256×256 moving domain.
        // Dense sampling marks those pixels invalid, driving the metric to
        // max_value. Random sampling at 20% (≈13 k samples) ensures a stable
        // finite metric throughout the pyramid.
        registration->SetMetricSamplingStrategy(
            RegistrationType::MetricSamplingStrategyEnum::RANDOM);
        registration->SetMetricSamplingPercentage(0.20);

        // ── Multi-resolution pyramid ──────────────────────────────────────────
        // 4 levels: the scale step was still decaying at iteration 499 with
        // 3 levels, showing the optimizer needs more iterations at fine
        // resolution. A fourth level restarts the step at LearningRate after
        // the coarser levels have narrowed scale close to 2.0.
        constexpr unsigned int levels = 4;
        registration->SetNumberOfLevels(levels);

        RegistrationType::ShrinkFactorsArrayType shrink(levels);
        shrink[0] = 8; shrink[1] = 4; shrink[2] = 2; shrink[3] = 1;

        RegistrationType::SmoothingSigmasArrayType sigmas(levels);
        sigmas[0] = 8.0; sigmas[1] = 4.0; sigmas[2] = 2.0; sigmas[3] = 0.0;

        registration->SetShrinkFactorsPerLevel(shrink);
        registration->SetSmoothingSigmasPerLevel(sigmas);
        registration->SmoothingSigmasAreSpecifiedInPhysicalUnitsOn();

        // ── Observers ─────────────────────────────────────────────────────────
        auto levelObs = LevelObserver::New();
        levelObs->iterObs = iterObs;
        registration->AddObserver(itk::MultiResolutionIterationEvent(), levelObs);

        // ── Wire up ───────────────────────────────────────────────────────────
        registration->SetMetric(metric);
        registration->SetOptimizer(optimizer);
        registration->SetFixedImage(fixed);
        registration->SetMovingImage(moving);
        registration->SetInitialTransform(transform);
        registration->InPlaceOn();

        std::cout << "--- pyramid level 0 ---\n";
        registration->Update();

        // ── Results ───────────────────────────────────────────────────────────
        const auto* T  = dynamic_cast<const TransformType*>(registration->GetTransform());
        const auto& tx = T->GetTranslation();

        // Ground truth (centre=(50,50) from moments init):
        //   scale=2.0, angle=0.0, translation=(150, 150)
        std::cout << "\n=== Registration result ===\n"
                  << std::fixed << std::setprecision(4)
                  << "  Scale            : " << T->GetScale()
                  << "  (expected ~2.0)\n"
                  << "  Angle (rad)      : " << T->GetAngle()
                  << "  (expected ~0.0)\n"
                  << "  Translation      : (" << tx[0] << ", " << tx[1] << ") mm"
                  << "  (expected ~(150, 150))\n"
                  << "  Best metric      : " << iterObs->bestMetric << "\n";

        // ── Resample and write ────────────────────────────────────────────────
        using ResamplerType = itk::ResampleImageFilter<ImageType, ImageType>;
        auto resampler = ResamplerType::New();
        resampler->SetInput(moving);
        resampler->SetTransform(T);
        resampler->SetSize(fixed->GetLargestPossibleRegion().GetSize());
        resampler->SetOutputOrigin(fixed->GetOrigin());
        resampler->SetOutputSpacing(fixed->GetSpacing());
        resampler->SetOutputDirection(fixed->GetDirection());
        resampler->SetDefaultPixelValue(0.0f);

        using ByteType    = itk::Image<unsigned char, 2>;
        using RescaleType = itk::RescaleIntensityImageFilter<ImageType, ByteType>;
        auto rescale = RescaleType::New();
        rescale->SetInput(resampler->GetOutput());
        rescale->SetOutputMinimum(0);
        rescale->SetOutputMaximum(255);

        using WriterType = itk::ImageFileWriter<ByteType>;
        auto writer = WriterType::New();
        writer->SetFileName(outputPath);
        writer->SetInput(rescale->GetOutput());
        writer->Update();

        std::cout << "  Registered image → " << outputPath << "\n";

    } catch (const itk::ExceptionObject& e) {
        std::cerr << "ITK error: " << e << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
