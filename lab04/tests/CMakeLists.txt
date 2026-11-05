# ── Helpers ───────────────────────────────────────────────────────────────────

# Scratch directory unique to each CTest run so tests don't stomp each other.
set(TEST_TMP "${CMAKE_CURRENT_BINARY_DIR}/tmp")

# ── Test 0: create scratch directory ─────────────────────────────────────────
add_test(
    NAME create_tmp_dir
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${TEST_TMP}"
)
set_tests_properties(create_tmp_dir PROPERTIES
    FIXTURES_SETUP generate_images_fixture
)

# ── Test 1: generate_images runs and writes two NRRD files ───────────────────
add_test(
    NAME generate_images_runs
    COMMAND generate_images
            "${TEST_TMP}/t1_img1.nrrd"
            "${TEST_TMP}/t1_img2.nrrd"
    WORKING_DIRECTORY "${TEST_TMP}"
)
set_tests_properties(generate_images_runs PROPERTIES
    DEPENDS             create_tmp_dir
    FIXTURES_SETUP      generate_images_fixture
)

# ── Test 2: img1.nrrd is non-empty ────────────────────────────────────────────
add_test(
    NAME img1_file_exists
    COMMAND "${CMAKE_COMMAND}" -E md5sum "${TEST_TMP}/t1_img1.nrrd"
)
set_tests_properties(img1_file_exists PROPERTIES
    DEPENDS           generate_images_runs
    FIXTURES_REQUIRED generate_images_fixture
)

# ── Test 3: img2.nrrd is non-empty ────────────────────────────────────────────
add_test(
    NAME img2_file_exists
    COMMAND "${CMAKE_COMMAND}" -E md5sum "${TEST_TMP}/t1_img2.nrrd"
)
set_tests_properties(img2_file_exists PROPERTIES
    DEPENDS           generate_images_runs
    FIXTURES_REQUIRED generate_images_fixture
)

# ── Test 4: register_circles exits 0 within timeout ──────────────────────────
add_test(
    NAME register_circles_runs
    COMMAND register_circles
            "${TEST_TMP}/t1_img1.nrrd"
            "${TEST_TMP}/t1_img2.nrrd"
            "${TEST_TMP}/registered.nrrd"
    WORKING_DIRECTORY "${TEST_TMP}"
)
set_tests_properties(register_circles_runs PROPERTIES
    DEPENDS           generate_images_runs
    FIXTURES_REQUIRED generate_images_fixture
    TIMEOUT           120
)

# ── Test 5: registered output is non-empty ────────────────────────────────────
add_test(
    NAME registered_image_exists
    COMMAND "${CMAKE_COMMAND}" -E md5sum "${TEST_TMP}/registered.nrrd"
)
set_tests_properties(registered_image_exists PROPERTIES
    DEPENDS           register_circles_runs
    FIXTURES_REQUIRED generate_images_fixture
)

# ── Test 6: scale accuracy — recovered scale must be within 5% of 2.0 ────────
add_test(
    NAME scale_accuracy
    COMMAND register_circles
            "${TEST_TMP}/t1_img1.nrrd"
            "${TEST_TMP}/t1_img2.nrrd"
    WORKING_DIRECTORY "${TEST_TMP}"
)
set_tests_properties(scale_accuracy PROPERTIES
    DEPENDS             generate_images_runs
    FIXTURES_REQUIRED   generate_images_fixture
    TIMEOUT             120
    PASS_REGULAR_EXPRESSION "Scale.*1\\.[89]|Scale.*2\\.[01]"
)
