# Windows + MSVC + Clangd 代码跳转配置总结

基于之前的分析和文档，以下是实现 OCCT 项目代码跳转（Go to Definition）的完整配置清单。

## 1. 核心原理
VS Code 的 Clangd 插件需要读取 `compile_commands.json` 文件来理解项目结构、头文件路径和宏定义。只要这个文件存在且路径正确，代码跳转就能工作。

## 2. 关键步骤清单

### 第一步：生成编译数据库 (compile_commands.json)
必须使用 `Ninja` 生成器，因为 Visual Studio 生成器不支持导出此文件。

1.  打开 **x64 Native Tools Command Prompt for VS 2022**。
2.  进入构建目录（例如 `E:\code\occtSrc\OCCT\Build`）。
3.  执行 CMake 命令：
    ```cmd
    cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -D3RDPARTY_DIR="E:/code/occtSrc/3rdparty-vc14-64" ..
    ```
4.  **验证**：确保 `Build` 目录下生成了 `compile_commands.json` 文件。

### 第三步：VS Code 插件设置
1.  安装插件：**clangd** (`llvm-vs-code-extensions.vscode-clangd`)。
2.  **重要**：如果插件提示 "clangd not found"，点击 **Install** 让它自动下载。
    *   *注意：不要在 settings.json 中手动指定 "clangd.path"，除非你非常确定路径是正确的。自动下载的版本通常最稳定。*

### 第三步：项目配置 (.vscode/settings.json)
在 `.vscode/settings.json` 中添加以下配置。这是最关键的一步，**必须告诉 clangd 去哪里找 compile_commands.json**。

```json
{
    // 1. 禁用微软 C/C++ 插件的智能提示，防止冲突
    "C_Cpp.intelliSenseEngine": "disabled",

    // 2. Clangd 核心配置
    "clangd.arguments": [
        // [关键] 指向包含 compile_commands.json 的文件夹（相对于项目根目录）
        "--compile-commands-dir=Build",
        
        // 启用后台索引，加快后续响应
        "--background-index",
        
        // 并发解析线程数
        "-j=12",
        
        // 启用详细补全
        "--completion-style=detailed",
        
        // 自动插入头文件策略
        "--header-insertion=iwyu"
    ]
}
```

### 第四步：MSVC 兼容性配置 (.clangd)
在项目根目录（`E:\code\occtSrc\OCCT\.clangd`）创建此文件，解决 Windows 头文件解析问题。

```yaml
CompileFlags:
  # 强制 clangd 模拟 cl.exe 模式，否则会报大量系统头文件错误
  Add: [--driver-mode=cl]

Diagnostics:
  # 屏蔽一些干扰视线的警告
  Suppress: 
    - "unknown-pragmas"
    - "unused-variable"
```

## 3. 常见问题排查 (Troubleshooting)

| 现象 | 原因 | 解决方法 |
| :--- | :--- | :--- |
| **无法跳转，提示 "Symbol not found"** | 找不到 `compile_commands.json` | 检查 `settings.json` 中的 `--compile-commands-dir=Build` 是否正确指向了 Build 目录。 |
| **提示 "clangd not found"** | 未安装 clangd 或路径错误 | 点击弹窗的 Install 按钮；检查 `settings.json` 是否有多余的 `"clangd.path"` 配置并删除它。 |
| **跳转到错误的系统头文件** | 环境变量缺失 | 尝试从 **x64 Native Tools Command Prompt** 中输入 `code .` 启动 VS Code，以继承 VS 的环境变量。 |
| **一直在 Indexing...** | 项目过大 | 正常现象。OCCT 首次索引可能需要 5-10 分钟。完成后速度会很快。 |

## 4. 验证方式
1.  重启 VS Code (Ctrl+Shift+P -> Reload Window)。
2.  打开任意 `.cxx` 源文件。
3.  看底部状态栏，应该显示 `clangd: indexing` (首次) 或 `clangd: idle`。
4.  将鼠标悬停在类名上，应该能看到详细类型信息；按 `F12` 应能立即跳转。
