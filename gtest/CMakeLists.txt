# Define the test executable
add_executable(my_tests
  test_main.cpp
  Bugs_Regression_GTest.cxx
)

# ============================================================
# 链接所有必需的 OCCT 库
# 根据链接错误，需要以下模块：
# ============================================================
target_link_libraries(my_tests PRIVATE
    # GoogleTest 库
    gtest
    gtest_main
    
    # ===== 核心基础库 =====
    TKernel          # 基础类型、集合、字符串
    TKMath           # 数学函数
    
    # ===== 几何建模库 =====
    TKG2d            # 2D 几何
    TKG3d            # 3D 几何
    TKGeomBase       # 几何基础
    TKGeomAlgo       # 几何算法
    TKBRep           # 边界表示
    
    # ===== 拓扑操作库 =====
    TKTopAlgo        # 拓扑算法
    TKPrim           # 基本图元
    TKBool           # 布尔运算
    TKBO             # 布尔运算（新版）
    TKFillet         # 倒角/圆角
    TKOffset         # 偏移操作
    TKFeat           # 特征建模
    TKShHealing      # 形状修复
    
    # ===== OCAF (应用框架) =====
    TKCAF            # CAF 核心
    TKLCAF           # CAF 轻量级持久化
    TKVCAF           # CAF 可视化
    TKBinL           # 二进制轻量级
    TKXmlL           # XML 轻量级
    TKStdL           # 标准轻量级
    TKBin            # 二进制持久化
    TKXml            # XML 持久化
    TKStd            # 标准持久化
    
    # ===== XCAF (扩展 CAF) =====
    TKXCAF           # XCAF 核心
    TKXSBase         # XS 基础（数据交换）
    #TKXDESTEP        # STEP 读取
    #TKXDEIGES        # IGES 读取
    #TKSTEP           # STEP 支持
    #TKSTEP209        # STEP AP209
    #TKSTEPBase       # STEP 基础
    #TKSTEPAttr       # STEP 属性
    #TKIGES           # IGES 支持
    
    # ===== CAF 控制器 =====
    
    #TKSTEPCAFControl # STEP CAF 控制
    #TKXCAFDoc        # XCAF 文档
    #TKBinXCAF        # 二进制 XCAF
    #TKXmlXCAF        # XML XCAF
    
    # ===== 表达式解析 =====
    #TKExpr           # 表达式引擎
    #TKExprIntrp      # 表达式解释器
    
    # ===== TObj (对象模型) =====
    TKTObj           # TObj 核心
    
    # ===== Draw 测试框架 =====
    TKDraw           # Draw 解释器
    TKDCAF           # Draw CAF 命令
    TKQADraw         # QA Draw 工具
    TKViewerTest     # 视图测试
    
    # ===== 可视化 (如果需要) =====
    # TKV3d          # 3D 视图
    # TKService      # 视图服务
    
    # ===== 网格化 (如果需要) =====
    # TKMesh         # 网格生成
)

# Include directories
target_include_directories(my_tests PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../src/QABugs
  # 如果需要，添加 OCCT 的其他头文件路径
  # ${OCCT_INCLUDE_DIR}
)

# Organize in VS Solution
set_target_properties(my_tests PROPERTIES FOLDER "Tests")

# ============================================================
# 可选：添加编译定义
# ============================================================
# target_compile_definitions(my_tests PRIVATE
#   _CRT_SECURE_NO_WARNINGS  # 禁用 MSVC 安全警告
# )

# ============================================================
# 可选：设置运行时库 (根据你的 OCCT 构建配置)
# ============================================================
# set_target_properties(my_tests PROPERTIES
#   MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
# )