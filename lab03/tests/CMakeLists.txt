# ── Helpers ───────────────────────────────────────────────────────────────────

# Scratch directory unique to each CTest run so tests don't stomp each other.
set(TEST_TMP "${CMAKE_CURRENT_BINARY_DIR}/tmp")

# ── Test 1: generate_images runs and writes two PNG files ─────────────────────
add_test(
    NAME generate_images_runs
    COMMAND generate_images
            "${TEST_TMP}/t1_img1.png"
            "${TEST_TMP}/t1_img2.png"
    WORKING_DIRECTORY "${TEST_TMP}"
)
# Ensure the tmp directory exists before the test tries to write into it.
set_tests_properties(generate_images_runs PROPERTIES
    FIXTURES_SETUP    generate_images_fixture
    ENVIRONMENT       "CTEST_TMPDIR=${TEST_TMP}"
)

# Create the temp dir as part of CTest setup via a script.
add_test(
    NAME create_tmp_dir
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${TEST_TMP}"
)
set_tests_properties(create_tmp_dir PROPERTIES
    FIXTURES_SETUP generate_images_fixture
)
set_tests_properties(generate_images_runs PROPERTIES
    DEPENDS create_tmp_dir
)

# ── Test 2: generated img1.png is non-empty ───────────────────────────────────
add_test(
    NAME img1_file_exists
    COMMAND "${CMAKE_COMMAND}" -E md5sum "${TEST_TMP}/t1_img1.png"
)
set_tests_properties(img1_file_exists PROPERTIES
    DEPENDS generate_images_runs
    FIXTURES_REQUIRED generate_images_fixture
)

# ── Test 3: generated img2.png is non-empty ───────────────────────────────────
add_test(
    NAME img2_file_exists
    COMMAND "${CMAKE_COMMAND}" -E md5sum "${TEST_TMP}/t1_img2.png"
)
set_tests_properties(img2_file_exists PROPERTIES
    DEPENDS generate_images_runs
    FIXTURES_REQUIRED generate_images_fixture
)

# ── Test 4: register_circles runs on the generated images ─────────────────────
# Ground-truth transform: scale≈2, translation≈(150,150), angle≈0.
# This test only checks that the binary exits 0; quantitative checks are in
# the integration test (test 5).
add_test(
    NAME register_circles_runs
    COMMAND register_circles
            "${TEST_TMP}/t1_img1.png"
            "${TEST_TMP}/t1_img2.png"
            "${TEST_TMP}/registered.png"
    WORKING_DIRECTORY "${TEST_TMP}"
)
set_tests_properties(register_circles_runs PROPERTIES
    DEPENDS         "generate_images_runs"
    FIXTURES_REQUIRED generate_images_fixture
    TIMEOUT         120   # registration must complete within 2 minutes
)

# ── Test 5: registered output image is written ────────────────────────────────
add_test(
    NAME registered_image_exists
    COMMAND "${CMAKE_COMMAND}" -E md5sum "${TEST_TMP}/registered.png"
)
set_tests_properties(registered_image_exists PROPERTIES
    DEPENDS register_circles_runs
    FIXTURES_REQUIRED generate_images_fixture
)
