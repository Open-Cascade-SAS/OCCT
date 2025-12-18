#include <gtest/gtest.h>
#include <OSD.hxx>

/**
 * @brief Main entry point for Gtest-style OCCT unit tests.
 * This runner focuses on direct C++ API tests.
 */
int main(int argc, char** argv)
{
  // Disable OCCT signal handlers to let Gtest and the debugger handle crashes naturally.
  OSD::SetSignal(Standard_False);

  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}