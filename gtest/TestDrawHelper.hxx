#ifndef TEST_DRAW_HELPER_HXX
#define TEST_DRAW_HELPER_HXX

#include <gtest/gtest.h>
#include <Draw_Interpretor.hxx>
#include <Standard_Integer.hxx>
#include <vector>
#include <string>
#include <iostream>

// 定义 Draw 命令函数的函数指针类型
typedef Standard_Integer (*DrawCommandFunc)(Draw_Interpretor&, Standard_Integer, const char**);

// ============================================================
// 全局单例获取函数
// 保证 Draw_Interpretor 只被初始化一次
// ============================================================
inline Draw_Interpretor& GetGlobalDrawInterpretor()
{
  static Draw_Interpretor* pDi = nullptr;
  if (pDi == nullptr)
  {
    pDi = new Draw_Interpretor();
    try
    {
      pDi->Init();
      // 可选：加载额外的命令模块
      // Draw::Commands(*pDi);
      // DBRep::BasicCommands(*pDi);
    }
    catch (...)
    {
      std::cerr << "[Critical Error] Failed to initialize Draw_Interpretor (Tcl)!" << std::endl;
      std::cerr << "Please check TCL_LIBRARY environment variable." << std::endl;
    }
  }
  return *pDi;
}

class DrawCommandRunner
{
public:
  static void Execute(DrawCommandFunc                 func,
                      const std::vector<std::string>& args,
                      const char*                     funcName,
                      const char*                     file,
                      int                             line)
  {
    // =======================================================
    // [修复] 使用全局单例，而不是局部变量
    // =======================================================
    Draw_Interpretor& di = GetGlobalDrawInterpretor();

    // 2. 准备参数
    std::vector<const char*> argvPtrs;
    argvPtrs.reserve(args.size());
    for (size_t i = 0; i < args.size(); ++i)
    {
      argvPtrs.push_back(args[i].c_str());
    }

    // 3. 执行前重置结果缓冲区，以免受到上一个测试的影响
    di.Reset();

    Standard_Integer res = 0;
    try
    {
      res = func(di, static_cast<Standard_Integer>(argvPtrs.size()), argvPtrs.data());
    }
    catch (const std::exception& e)
    {
      ADD_FAILURE_AT(file, line) << "C++ Exception in " << funcName << ": " << e.what();
      return;
    }
    catch (...)
    {
      ADD_FAILURE_AT(file, line) << "Unknown Exception in " << funcName;
      return;
    }

    // 4. 获取输出
    const char* outputStr = di.Result();
    std::string outputMsg = (outputStr ? outputStr : "");

    // 5. 验证结果
    if (res != 0)
    {
      ADD_FAILURE_AT(file, line) << "Draw command returned error code (" << res << ").\n"
                                 << "Command: " << funcName << "\n"
                                 << "Output: " << outputMsg;
    }
  }
};

#define RUN_DRAW_COMMAND(FuncName, ...)                                                            \
  {                                                                                                \
    std::vector<std::string> draw_cmd_args = {__VA_ARGS__};                                        \
    DrawCommandRunner::Execute(FuncName, draw_cmd_args, #FuncName, __FILE__, __LINE__);            \
  }

#endif // TEST_DRAW_HELPER_HXX