set(DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/data")
message(STATUS "Test data: ${DATA_DIR}")

set(TEST_SRCS
  assorted_tests.cpp
  diag_write_common.hpp
  diag_write_mixed_test.cpp
  diag_write_plural_test.cpp
  diag_write_singular_test.cpp
  diagnostic_test.cpp
  gt_main.cpp
  source_code_test.cpp
  source_line_test.cpp
  source_location_test.cpp
  streams_test.cpp
  support/diagnostic_str.cpp
  support/diagnostic_str.hpp
  support/mixed_types.hpp
  support/mock_strings.cpp
  support/outstrstream.hpp
  support/plural_types.hpp
  support/printer.cpp
  support/printer.hpp
  support/raven_text.hpp
  support/singular_types.hpp
  support/strings.cpp
  support/strings.hpp
  support/support.cpp
  support/support.hpp
  support/test_with_diagnostics.hpp
)

add_executable(${PROJECT_NAME}-test ${TEST_SRCS})
set_target_properties(${PROJECT_NAME}-test PROPERTIES FOLDER tests)
target_compile_options(${PROJECT_NAME}-test PRIVATE ${DIAGS_ADDITIONAL_WALL_FLAGS})
target_compile_features(${PROJECT_NAME}-test PRIVATE cxx_std_17)
target_link_libraries(${PROJECT_NAME}-test CONAN_PKG::gtest ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME}-test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME ${PROJECT_NAME}.assorted COMMAND ${PROJECT_NAME}-test --gtest_filter=Assorted.*)
add_test(NAME ${PROJECT_NAME}.source COMMAND ${PROJECT_NAME}-test --gtest_filter=source_line.*:source_location.*:streams.* --data_path=${DATA_DIR})
add_test(NAME ${PROJECT_NAME}.write-singular COMMAND ${PROJECT_NAME}-test --gtest_filter=*/diags_*_singular.*)
add_test(NAME ${PROJECT_NAME}.write-plural COMMAND ${PROJECT_NAME}-test --gtest_filter=*/diags_*_plural.*)
add_test(NAME ${PROJECT_NAME}.write-mixed COMMAND ${PROJECT_NAME}-test --gtest_filter=*/diags_*_mixed.*)
add_test(NAME ${PROJECT_NAME}.builder COMMAND ${PROJECT_NAME}-test --gtest_filter=*/diagnostic_builder.*)
add_test(NAME ${PROJECT_NAME}.source_code COMMAND ${PROJECT_NAME}-test --gtest_filter=source_code.* --data_path=${DATA_DIR})
