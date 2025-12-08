# Define the test executable
add_executable(my_tests
  test_main.cpp
  QABugs_GTest.cxx
)

# Link against gtest and OCCT libraries
# Note: We assume OCCT libraries (TK*) are available as targets from the main build
target_link_libraries(my_tests PRIVATE
  gtest
  gtest_main
  TKernel
  TKMath
  TKBRep
  TKTopAlgo
  TKPrim
  TKDraw
)

# Include directories
target_include_directories(my_tests PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

# Organize in VS Solution
set_target_properties(my_tests PROPERTIES FOLDER "Tests")

