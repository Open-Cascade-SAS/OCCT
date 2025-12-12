# Windows + MSVC + Clangd 开发环境配置指南

## 1. 核心工具安装
我们需要安装 Ninja 来生成编译数据库，安装 LLVM 来提供 clangd 服务。

请以**管理员身份**打开 PowerShell 并执行：

```powershell
# 1. 安装 Ninja (构建工具，速度快，支持导出 compile_commands.json)
choco install ninja

# 2. 安装 LLVM (包含 clangd 核心程序)
choco install llvm

# 3. (可选) 安装 CMake，如果你还没有的话
choco install cmake
```

---

## 2. 生成 compile_commands.json
由于 `Visual Studio` 生成器不支持导出编译数据库，必须使用 `Ninja` 生成器。

**重要步骤：**
1.  按 `Win` 键，搜索并打开 **"x64 Native Tools Command Prompt for VS 2022"** (不要使用普通 CMD 或 PowerShell)。
2.  进入你的构建目录并执行 CMake。

假设你的目录结构如下：
*   源码: `E:\code\occtSrc\OCCT`
*   Build: `E:\code\occtSrc\OCCT\Build`
*   第三方库: `E:\code\occtSrc\3rdparty-vc14-64`

**执行命令：**

```cmd
:: 1. 进入构建目录
cd /d E:\code\occtSrc\OCCT\Build

:: 2. 清理旧缓存（防止 VS 生成器和 Ninja 冲突）
del CMakeCache.txt

:: 3. 生成配置 (核心参数是 -G "Ninja" 和 -DCMAKE_EXPORT_COMPILE_COMMANDS=1)
cmake -G "Ninja" ^
 -DCMAKE_BUILD_TYPE=Release ^
 -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ^
 -D3RDPARTY_DIR="E:/code/occtSrc/3rdparty-vc14-64" ^
 ..

:: 执行完毕后，检查当前目录下是否生成了 compile_commands.json
```

---

## 3. VS Code 插件与项目配置

### 3.1 安装插件
在 VS Code 扩展商店搜索并安装：
*   **clangd** (`llvm-vs-code-extensions.vscode-clangd`)

### 3.2 项目配置文件 (.vscode/settings.json)
在源码根目录 `E:\code\occtSrc\OCCT` 下，创建文件夹 `.vscode`，并在其中创建 `settings.json`：

```json
{
    // 禁用微软官方 C/C++ 插件的 IntelliSense，避免冲突和卡顿
    "C_Cpp.intelliSenseEngine": "disabled",

    // Clangd 参数配置
    "clangd.arguments": [
        // 指向 build 目录，告诉它去哪里找 compile_commands.json
        "--compile-commands-dir=Build",
        
        // 启用后台索引
        "--background-index",
        
        // 启用多线程并发解析
        "-j=12",
        
        // 补全风格更详细
        "--completion-style=detailed",
        
        // 自动补充头文件
        "--header-insertion=iwyu",

        // 如果 clangd 找不到 cl.exe，解除下面这行的注释并修改为实际路径
        // "--query-driver=C:\\Program Files\\Microsoft Visual Studio\\2022\\Professional\\VC\\Tools\\MSVC\\**\\bin\\Hostx64\\x64\\cl.exe"
    ]
}
```

---

## 4. 解决 MSVC 兼容性问题 (.clangd)
为了让 Clangd 正确理解 Windows 的头文件和 `cl.exe` 的参数，必须在**源码根目录**创建配置文件。

**文件路径**：`E:\code\occtSrc\OCCT\.clangd` (注意文件名以点开头)
**文件内容**：

```yaml
CompileFlags:
  # 强制 clangd 模拟 cl.exe 的行为
  Add: [--driver-mode=cl]

Diagnostics:
  # 屏蔽一些非必要的警告（OCCT 代码量大，可能有很多旧风格代码）
  Suppress: 
    - "unknown-pragmas"
    - "unused-variable"
```

---

## 5. 启动与验证

为了确保 VS Code 能继承 `cl.exe` 的环境变量（防止找不到标准库），推荐通过以下方式启动：

1.  保持 **"x64 Native Tools Command Prompt"** 打开。
2.  进入源码目录：`cd /d E:\code\occtSrc\OCCT`
3.  输入命令启动 VS Code：
    ```cmd
    code .
    ```
4.  打开 VS Code 后：
    *   按 `Ctrl + Shift + P` -> `Developer: Reload Window`。
    *   打开任意 `.cxx` 文件。
    *   观察底部状态栏，应显示 **"clangd: indexing..."**。
    *   等待索引完成后，尝试 `F12` 跳转定义，应该可以瞬间跳转。