#ifndef TEST_DRAW_HELPER_HXX
#define TEST_DRAW_HELPER_HXX

#include <Draw_Interpretor.hxx>
#include <gtest/gtest.h>
#include <iostream>
#include <vector>
#include <string>

// Helper class to capture Draw output if needed,
// or just provide a valid Interpretor instance.
class TestDrawInterpretor : public Draw_Interpretor
{
public:
  TestDrawInterpretor()
      : Draw_Interpretor()
  {
  }
};

// Macro to run a static Draw command function
// Func: The static function name (e.g., OCC31294)
// Args: A std::vector<std::string> or similar container of arguments
//       The first argument should be the command name itself.
#define RUN_DRAW_COMMAND(Func, Args)                                                               \
  do                                                                                               \
  {                                                                                                \
    TestDrawInterpretor      di;                                                                   \
    std::vector<const char*> argv_ptrs;                                                            \
    for (const auto& arg : Args)                                                                   \
    {                                                                                              \
      argv_ptrs.push_back(arg.c_str());                                                            \
    }                                                                                              \
    Standard_Integer res = Func(di, (Standard_Integer)argv_ptrs.size(), argv_ptrs.data());         \
    EXPECT_EQ(res, 0) << "Draw command " << Args[0] << " failed";                                  \
  } while (0)

#endif // TEST_DRAW_HELPER_HXX
