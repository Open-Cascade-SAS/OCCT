#include <gtest/gtest.h>
#include <Draw_Interpretor.hxx>
#include <DBRep.hxx>
#include <GeometryTest.hxx>
#include <BRepTest.hxx>
#include <Draw.hxx>
#include "../src/OSD/OSD.hxx"

// 全局初始化：OCCT Draw 环境
class OCCTEnvironment : public ::testing::Environment
{
public:
  virtual void SetUp() override
  {
    // // 创建解释器用于加载命令
    // Draw_Interpretor di;

    // di.Init();
    // // 不使用 Draw::Load()，而是直接加载需要的模块
    // // 这些函数会注册命令到全局命令表

    // // 加载基础 BRep 命令
    // try
    // {
    //   Draw::Commands(di); // Load basic Draw commands first
    //   DBRep::BasicCommands(di);
    //   GeometryTest::AllCommands(di);
    //   BRepTest::AllCommands(di);
    // }
    // catch (Standard_Failure const& theFailure)
    // {
    //   std::cerr << "OCCT Exception: " << theFailure.GetMessageString() << std::endl;
    // }
    // catch (std::exception const& e)
    // {
    //   std::cerr << "Std Exception: " << e.what() << std::endl;
    // }
    // catch (...)
    // {
    //   std::cerr << "Unknown Exception during command registration." << std::endl;
    // }

    // 如果还需要其他模块：
    // MeshTest::Commands(di);
    // HLRTest::Commands(di);
  }

  virtual void TearDown() override
  {
    // 清理工作（如果需要）
  }
};

int main(int argc, char** argv)
{

  // 【关键修复】禁用 OCCT 的信号捕获，让调试器能捕捉到崩溃
  // 如果设为 Standard_True，OCCT 会尝试把崩溃转为 C++ 异常，但在 GTest 里容易导致直接退出。
  // 开发调试阶段建议设为 False。
  OSD::SetSignal(Standard_False);

  ::testing::InitGoogleTest(&argc, argv);

  // 注册全局环境
  ::testing::AddGlobalTestEnvironment(new OCCTEnvironment);

  return RUN_ALL_TESTS();
}