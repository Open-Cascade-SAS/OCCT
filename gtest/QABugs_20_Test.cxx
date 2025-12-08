#include "TestDrawHelper.hxx"

// ==========================================================
// 关键步骤：包含源文件
// 这使得原本 static 的函数在当前编译单元变得可见
// ==========================================================

// 注意：你需要根据实际的文件路径修改这里的 include 路径
// 假设你的目录结构是：
// project/
//   src/
//     QABugs/
//       QABugs_20.cxx
//   tests/
//     QABugs_20_Test.cxx

// 为了防止重复定义 main 或其他冲突，如果源文件非常杂乱，
// 有时需要定义一些宏来屏蔽代码，但 QABugs_20.cxx 看起来很干净，直接包含即可。
#include "../src/QABugs/QABugs_20.cxx"

// ==========================================================
// 测试用例编写
// ==========================================================

TEST(QABugs_20, OCC31294_PrismGen)
{
  // 直接调用 static 函数 OCC31294
  // 参数列表第一个元素必须是命令名
  RUN_DRAW_COMMAND(OCC31294, "OCC31294");
}

TEST(QABugs_20, OCC26675_1_SurfaceGen)
{
  // 测试另一个 static 函数
  // 注意：这个命令原本需要参数 "result"，我们传入它
  RUN_DRAW_COMMAND(SurfaceGenOCC26675_1, "OCC26675_1", "my_result_surf");
}

TEST(QABugs_20, OCC28829_FPE)
{
  // 测试浮点异常处理
  RUN_DRAW_COMMAND(OCC28829, "OCC28829");
}

TEST(QABugs_20, CheckParabola_1)
{
  // 测试 static 函数 OCC26747_1
  RUN_DRAW_COMMAND(OCC26747_1, "OCC26747_1", "parabola_res");
}