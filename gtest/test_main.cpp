#include <gtest/gtest.h>
#include <Draw_Interpretor.hxx>
#include <Draw.hxx>

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);

  // Initialize Draw if necessary (e.g. load basic commands)
  // Draw::Load(Draw::GetInterpretor());

  return RUN_ALL_TESTS();
}
