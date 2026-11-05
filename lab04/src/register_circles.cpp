// ITK-based 2-D image registration using Similarity2DTransform.
//
// Usage:
//   register_circles <fixed.nrrd> <moving.nrrd> [registered.nrrd [convergence.csv]]
//
// Fixed  = img1.nrrd (small, 30 mm diameter circle at (50,50) mm)
// Moving = img2.nrrd (large, 60 mm diameter circle at (200,200) mm)
//
// Expected result: scale≈2.0, angle≈0 rad, translation≈(150,150) mm.
//
// NRRD stores spacing, origin, and direction cosines so all metric distances
// are automatically in physical (mm) units.

#include <itkCenteredTransformInitializer.h>
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkImageRegistrationMethodv4.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkMeanSquaresImageToImageMetricv4.h>
#include <itkMultiThreaderBase.h>
#include <itkRegularStepGradientDescentOptimizerv4.h>
#include <itkRescaleIntensityImageFilter.h>
#include <itkResampleImageFilter.h>
#include <itkSimilarity2DTransform.h>

#include <cstdlib>
#include <fstream>
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
    std::ofstream csvStream;

    void SetCsvPath(const std::string& path)
    {
        if (!path.empty()) {
            csvStream.open(path);
            csvStream << "level,iteration,metric,step\n";
        }
    }

    void Execute(itk::Object* caller, const itk::EventObject& event) override
    {
        Execute(static_cast<const itk::Object*>(caller), event);
    }
    void Execute(const itk::Object* obj, const itk::EventObject& event) override
    {
        const auto* opt = dynamic_cast<const OptimizerType*>(obj);
        if (!opt || !itk::IterationEvent().CheckEvent(&event)) { return; }
        const double v    = opt->GetValue();
        const double step = opt->GetCurrentStepLength();
        const unsigned int iter = opt->GetCurrentIteration();
        if (v < bestMetric) { bestMetric = v; }
        std::cout << "  [L" << level << "] iter " << std::setw(4) << iter
                  << "  metric " << std::setw(12) << std::setprecision(6) << v
                  << "  step " << step << "\n";
        if (csvStream.is_open()) {
            csvStream << level << "," << iter << ","
                      << std::setprecision(8) << v << "," << step << "\n";
        }
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

// ── Normalize intensity to [0,1] ──────────────────────────────────────────────
// Smoothing is intentionally omitted: applying a fixed-sigma Gaussian before
// registration creates a scale-dependent metric floor when scale≠1.
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

int main(int argc, char* argv[])
{
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0]
                  << " <fixed.nrrd> <moving.nrrd> [registered.nrrd [convergence.csv]]\n";
        return EXIT_FAILURE;
    }

    const std::string fixedPath   = argv[1];
    const std::string movingPath  = argv[2];
    const std::string outputPath  = (argc > 3) ? argv[3] : "";
    const std::string csvPath     = (argc > 4) ? argv[4] : "";

    // Single-threaded: deterministic, reproducible results.
    itk::MultiThreaderBase::SetGlobalMaximumNumberOfThreads(1);

    try {
        // ── Load & normalize ──────────────────────────────────────────────────
        auto r1 = itk::ImageFileReader<ImageType>::New();
        r1->SetFileName(fixedPath);
        r1->Update();
        auto fixed = normalizeImage(r1->GetOutput());

        auto r2 = itk::ImageFileReader<ImageType>::New();
        r2->SetFileName(movingPath);
        r2->Update();
        auto moving = normalizeImage(r2->GetOutput());

        // ── Components ────────────────────────────────────────────────────────
        auto metric       = MetricType::New();
        auto optimizer    = OptimizerType::New();
        auto transform    = TransformType::New();
        auto registration = RegistrationType::New();

        // ── Moments-based initialisation ──────────────────────────────────────
        // Aligns the centre-of-mass of both images. For binary circles, CoM ==
        // circle centre, so this seeds translation near (150,150) mm and places
        // the rotation centre at the fixed centroid (50,50). Translation then
        // does not depend on scale, decoupling the two primary DOFs.
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
        // Similarity2DTransform parameters: [scale, angle, tx, ty].
        // Scale is the primary unknown (1→2). Angle is suppressed (circles are
        // isotropic). Translation is already near correct after moments init.
        {
            OptimizerType::ScalesType scales(transform->GetNumberOfParameters());
            scales[0] =     1.0;   // scale      — primary DOF
            scales[1] = 10000.0;   // angle      — suppress (isotropic circles)
            scales[2] = 50000.0;   // tx         — locked after moments init
            scales[3] = 50000.0;   // ty
            optimizer->SetScales(scales);
        }

        optimizer->SetLearningRate(0.5);
        optimizer->SetMinimumStepLength(1e-8);
        optimizer->SetNumberOfIterations(500);
        optimizer->SetRelaxationFactor(0.5);
        optimizer->SetGradientMagnitudeTolerance(1e-8);
        optimizer->ReturnBestParametersAndValueOn();

        // ── Convergence observer ──────────────────────────────────────────────
        auto iterObs = IterationObserver::New();
        iterObs->SetCsvPath(csvPath);
        optimizer->AddObserver(itk::IterationEvent(), iterObs);

        // ── Random metric sampling ────────────────────────────────────────────
        // 20% random sampling prevents the metric from returning max_value when
        // the similarity transform maps part of the fixed image outside the
        // moving image's physical domain.
        registration->SetMetricSamplingStrategy(
            RegistrationType::MetricSamplingStrategyEnum::RANDOM);
        registration->SetMetricSamplingPercentage(0.20);

        // ── Multi-resolution pyramid: 4 levels ───────────────────────────────
        // 4 levels: 3 levels were insufficient — the step was still decaying at
        // iteration 499. A 4th level restarts the step after coarse convergence.
        constexpr unsigned int levels = 4;
        registration->SetNumberOfLevels(levels);

        RegistrationType::ShrinkFactorsArrayType shrink(levels);
        shrink[0] = 8; shrink[1] = 4; shrink[2] = 2; shrink[3] = 1;

        RegistrationType::SmoothingSigmasArrayType sigmas(levels);
        sigmas[0] = 8.0; sigmas[1] = 4.0; sigmas[2] = 2.0; sigmas[3] = 0.0;

        registration->SetShrinkFactorsPerLevel(shrink);
        registration->SetSmoothingSigmasPerLevel(sigmas);
        registration->SmoothingSigmasAreSpecifiedInPhysicalUnitsOn();

        // ── Level observer ────────────────────────────────────────────────────
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

        std::cout << "\n=== Registration result ===\n"
                  << std::fixed << std::setprecision(4)
                  << "  Scale            : " << T->GetScale()
                  << "  (expected ~2.0)\n"
                  << "  Angle (rad)      : " << T->GetAngle()
                  << "  (expected ~0.0)\n"
                  << "  Translation      : (" << tx[0] << ", " << tx[1] << ") mm"
                  << "  (expected ~(150, 150))\n"
                  << "  Best metric      : " << iterObs->bestMetric << "\n";
        if (!csvPath.empty()) {
            std::cout << "  Convergence CSV  : " << csvPath << "\n";
        }

        // ── Resample and write ────────────────────────────────────────────────
        if (!outputPath.empty()) {
            auto resampler = itk::ResampleImageFilter<ImageType, ImageType>::New();
            resampler->SetInput(moving);
            resampler->SetTransform(T);
            resampler->SetUseReferenceImage(true);
            resampler->SetReferenceImage(fixed);
            resampler->SetDefaultPixelValue(0.0f);

            auto writer = itk::ImageFileWriter<ImageType>::New();
            writer->SetFileName(outputPath);
            writer->SetInput(resampler->GetOutput());
            writer->Update();
            std::cout << "  Registered image → " << outputPath << "\n";
        }

    } catch (const itk::ExceptionObject& e) {
        std::cerr << "ITK error: " << e << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
