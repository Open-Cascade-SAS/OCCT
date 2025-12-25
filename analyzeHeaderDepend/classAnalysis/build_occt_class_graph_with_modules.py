import sys
import os
import glob
import json
import re
import time
import multiprocessing
import clang.cindex
from clang.cindex import CursorKind

# ================= 配置部分 =================
# 1. libclang 路径
CLANG_LIBRARY_FILE = r"E:\workFile\temp\analyzeHeaderDepend\classAnalysis\clang+llvm-21.1.8-x86_64-pc-windows-msvc\clang+llvm-21.1.8-x86_64-pc-windows-msvc\bin\libclang.dll"

# 2. OCCT 源码目录 (包含 src 子目录)
OCCT_SRC_DIR = r"E:\code\occtSrc\OCCT\src"

# 3. OCCT 头文件目录 (必须是包含所有 .hxx 的目录，通常是 inc)
OCCT_INC_DIR = r"E:\code\occtSrc\Install\inc"

NUM_PROCESSES = min(os.cpu_count(), 8)
# ===========================================

if not os.path.exists(CLANG_LIBRARY_FILE):
    print(f"Error: {CLANG_LIBRARY_FILE} not found.")
    sys.exit(1)
clang.cindex.Config.set_library_file(CLANG_LIBRARY_FILE)

def build_package_to_toolkit_map(src_dir):
    """
    解析所有 PACKAGES 文件，构建 Package -> Toolkit 映射
    返回: dict { 'PackageName': 'ToolkitName' }
    """
    package_map = {}
    packages_files = glob.glob(os.path.join(src_dir, "*", "PACKAGES"))
    
    for pkg_file in packages_files:
        toolkit_name = os.path.basename(os.path.dirname(pkg_file))
        
        with open(pkg_file, 'r', encoding='utf-8') as f:
            for line in f:
                package_name = line.strip()
                if package_name:  # 跳过空行
                    package_map[package_name] = toolkit_name
    
    print(f"已解析 {len(packages_files)} 个 PACKAGES 文件，共 {len(package_map)} 个包")
    return package_map

def infer_package_from_filename(filepath):
    """
    从文件名推断 Package 名称
    例如: BRepBuilderAPI_MakeShape.hxx -> BRepBuilderAPI
    """
    basename = os.path.basename(filepath)
    # 移除扩展名
    name_without_ext = os.path.splitext(basename)[0]
    
    # OCCT 的命名规范: PackageName_ClassName
    if '_' in name_without_ext:
        package_name = name_without_ext.split('_')[0]
        return package_name
    else:
        # 如果没有下划线，整个名字可能就是 Package 名 (如 gp.hxx)
        return name_without_ext

def clean_type_name(type_str):
    """
    清洗类型名称
    """
    if not type_str: return ""
    
    # 1. 移除 class/struct/enum 前缀 (针对 Forward Declaration 和 Elaborated Type)
    type_str = re.sub(r'\b(class|struct|enum)\s+', '', type_str)

    # 2. 处理 Handle 宏
    m_handle = re.search(r'handle<([^>]+)>', type_str, re.IGNORECASE)
    if m_handle: type_str = m_handle.group(1)
    
    m_handle_macro = re.search(r'Handle\(([^)]+)\)', type_str, re.IGNORECASE)
    if m_handle_macro: type_str = m_handle_macro.group(1)

    # 3. 移除 const, volatile, &, *
    clean = type_str.replace('const', '').replace('volatile', '').replace('&', '').replace('*', '')
    
    # 4. 移除命名空间
    if '::' in clean: clean = clean.split('::')[-1]
    
    return clean.strip()

def analyze_single_file(filepath, package_map):
    index = clang.cindex.Index.create()
    args = [
        '-x', 'c++', 
        f'-I{OCCT_INC_DIR}', 
        '-DWNT', 
        '-DStandard_EXPORT=', 
        '-DStandard_VkExport=',
        '-std=c++14'
    ]
    
    try:
        tu = index.parse(filepath, args=args, options=clang.cindex.TranslationUnit.PARSE_SKIP_FUNCTION_BODIES)
    except Exception:
        return None

    # 推断当前文件所属的 Package
    package_name = infer_package_from_filename(filepath)
    toolkit_name = package_map.get(package_name, "Unknown")

    # 结果容器
    file_result = {} 
    # 本文件定义的所有类型 (Class, Struct, Enum, Typedef)
    defined_types = set()

    current_context = None # 当前正在解析的类名
    target_filename = os.path.basename(filepath).lower()
    
    def visit(node):
        nonlocal current_context
        
        # 1. 允许根节点递归
        if node.kind == CursorKind.TRANSLATION_UNIT:
            for child in node.get_children():
                visit(child)
            return

        # 2. 检查节点归属文件 (只处理当前文件定义的类型)
        if node.location.file:
            node_fname = os.path.basename(node.location.file.name).lower()
            if node_fname != target_filename:
                return 
        else:
            if current_context is None: return

        # === 类型定义扫描 (Class, Struct, Enum, Typedef) ===
        is_definition_node = node.kind in [
            CursorKind.CLASS_DECL, 
            CursorKind.STRUCT_DECL, 
            CursorKind.ENUM_DECL, 
            CursorKind.TYPEDEF_DECL
        ]

        if is_definition_node:
            if node.is_definition():
                name = node.spelling
                if name:
                    defined_types.add(name) # 记录下来，作为全局白名单
                    
                    # 只有 Class 和 Struct 需要深入解析成员依赖
                    if node.kind in [CursorKind.CLASS_DECL, CursorKind.STRUCT_DECL]:
                        current_context = name
                        if name not in file_result:
                            file_result[name] = {
                                'inherits': set(), 
                                'fields': set(), 
                                'uses': set(),
                                'package': package_name,
                                'toolkit': toolkit_name
                            }
                        
                        for child in node.get_children():
                            visit(child)
                        current_context = None 
            return

        # === 类成员解析 ===
        if current_context:
            # A. 继承关系
            if node.kind == CursorKind.CXX_BASE_SPECIFIER:
                base_name = clean_type_name(node.type.spelling)
                if base_name:
                    file_result[current_context]['inherits'].add(base_name)

            # B. 成员变量 (Fields)
            elif node.kind == CursorKind.FIELD_DECL:
                t_name = clean_type_name(node.type.spelling)
                if t_name and (t_name[0].isupper() or t_name.startswith('Standard_')): 
                    file_result[current_context]['fields'].add(t_name)

            # C. 方法 (参数 + 返回值)
            elif node.kind == CursorKind.CXX_METHOD or node.kind == CursorKind.FUNCTION_DECL:
                
                # 1. 检查返回值类型
                ret_type = clean_type_name(node.result_type.spelling)
                if ret_type and (ret_type[0].isupper() or ret_type.startswith('Standard_')):
                    if ret_type != current_context and ret_type != "void":
                        file_result[current_context]['uses'].add(ret_type)

                # 2. 检查参数
                for arg in node.get_children():
                    if arg.kind == CursorKind.PARM_DECL:
                        t_name = clean_type_name(arg.type.spelling)
                        if t_name and (t_name[0].isupper() or t_name.startswith('Standard_')):
                            if t_name != current_context:
                                file_result[current_context]['uses'].add(t_name)
    
    visit(tu.cursor)
    
    # 格式化输出
    final_data = {}
    for c_name, data in file_result.items():
        real_uses = data['uses'] - data['fields'] - data['inherits']
        final_data[c_name] = {
            'package': data['package'],
            'toolkit': data['toolkit'],
            'inherits': list(data['inherits']),
            'fields': list(data['fields']),
            'uses': list(real_uses)
        }
    
    # 返回两个数据：依赖图片段，以及本文件定义了哪些类型(用于构建白名单)
    return final_data, list(defined_types)

def worker_wrapper(args):
    filepath, package_map = args
    try:
        return analyze_single_file(filepath, package_map)
    except:
        return None

def main():
    print(f"开始全量解析 OCCT 类依赖 (带模块信息)...")
    
    # 1. 构建 Package -> Toolkit 映射
    package_map = build_package_to_toolkit_map(OCCT_SRC_DIR)
    
    # 2. 获取所有头文件
    all_files = glob.glob(os.path.join(OCCT_INC_DIR, "*.hxx"))
    print(f"共找到 {len(all_files)} 个 .hxx 文件")

    full_graph = {}
    global_defined_types = set() # 全局白名单
    
    start_time = time.time()
    
    # 准备参数 (每个文件都需要 package_map)
    worker_args = [(f, package_map) for f in all_files]
    
    with multiprocessing.Pool(processes=NUM_PROCESSES) as pool:
        processed = 0
        total = len(all_files)
        for result_tuple in pool.imap_unordered(worker_wrapper, worker_args, chunksize=20):
            processed += 1
            if processed % 500 == 0:
                print(f"进度: {processed}/{total} ({(processed/total)*100:.1f}%)")
            
            if result_tuple:
                graph_data, types_list = result_tuple
                full_graph.update(graph_data)
                global_defined_types.update(types_list)

    print(f"解析耗时: {time.time() - start_time:.2f} 秒")
    print(f"全局共识别出 {len(global_defined_types)} 个类型定义 (Class/Enum/Typedef)。")

    print("正在清洗依赖关系并导出...")
    
    # 辅助白名单 (这些通常是基础类型，可能没有 .hxx 定义，或者定义在 .cxx 里)
    aux_whitelist = ("Standard_", "GeomAbs_", "TopAbs_", "Aspect_", "Quantity_", "StepData_", "StepGeom_")

    def is_valid_dep(name):
        if name in global_defined_types: return True
        # 宽容模式：只要是大写开头，且符合 OCCT 命名习惯的前缀，也保留
        if name.startswith(aux_whitelist): return True
        return False

    cleaned_graph = {}
    for cls, deps in full_graph.items():
        cleaned_graph[cls] = {
            'package': deps['package'],
            'toolkit': deps['toolkit'],
            'inherits': [x for x in deps['inherits'] if is_valid_dep(x)],
            'fields': [x for x in deps['fields'] if is_valid_dep(x)],
            'uses': [x for x in deps['uses'] if is_valid_dep(x)]
        }

    out_file = "occt_class_graph_with_modules.json"
    with open(out_file, 'w', encoding='utf-8') as f:
        json.dump(cleaned_graph, f, indent=2)
    print(f"结果已保存至 {out_file}")
    
    # 额外输出：统计每个 Toolkit 的类数量
    toolkit_stats = {}
    for cls, data in cleaned_graph.items():
        tk = data['toolkit']
        toolkit_stats[tk] = toolkit_stats.get(tk, 0) + 1
    
    print("\n=== Toolkit 统计 ===")
    for tk, count in sorted(toolkit_stats.items(), key=lambda x: x[1], reverse=True):
        print(f"{tk}: {count} 个类")

if __name__ == "__main__":
    multiprocessing.freeze_support()
    main()
