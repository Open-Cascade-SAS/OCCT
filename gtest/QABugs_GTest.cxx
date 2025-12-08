#include "TestDrawHelper.hxx"

// ==========================================================
// 声明外部的遗留函数
// 这一步是必须的，因为 C++ 需要知道函数的签名
// ==========================================================

// 声明你在 QABugs_20.cxx 中定义的函数
// 记得去原文件把 "static" 关键字删掉！
extern Standard_Integer OCC31294(Draw_Interpretor&, Standard_Integer, const char**);
extern Standard_Integer OCC28829(Draw_Interpretor&, Standard_Integer, const char**);

// ==========================================================
// 测试用例编写
// ==========================================================

TEST(QABugs, OCC31294_PrismGen)
{
  // 一行代码运行测试
  // 第一个参数是命令名，这很重要，因为很多 Draw 命令内部会解析 argv[0]
  RUN_DRAW_COMMAND(OCC31294, "OCC31294");
}

TEST(QABugs, OCC28829_FPE)
{
  // 测试一个带参数的情况（假设它需要参数）
  // RUN_DRAW_COMMAND(OCC28829, "OCC28829", "arg1", "arg2");

  // 如果原函数不需要参数，只传命令名即可
  RUN_DRAW_COMMAND(OCC28829, "OCC28829");
}