import sys
import os
import glob
import re
import csv
import clang.cindex
from clang.cindex import CursorKind
from collections import defaultdict

# ================= 配置部分 =================
# 1. libclang 路径
CLANG_LIBRARY_FILE = r"E:\workFile\temp\analyzeHeaderDepend\classAnalysis\clang+llvm-21.1.8-x86_64-pc-windows-msvc\clang+llvm-21.1.8-x86_64-pc-windows-msvc\bin\libclang.dll"

# 2. OCCT 源码目录
OCCT_SRC_DIR = r"E:\code\occtSrc\OCCT\src"

# 3. OCCT 头文件目录
OCCT_INC_DIR = r"E:\code\occtSrc\Install\inc"

# 4. 输出目录
OUTPUT_DIR = "occt_entities_csv"
# ===========================================

if not os.path.exists(CLANG_LIBRARY_FILE):
    print(f"Error: {CLANG_LIBRARY_FILE} not found.")
    sys.exit(1)
clang.cindex.Config.set_library_file(CLANG_LIBRARY_FILE)

def build_package_to_toolkit_map(src_dir):
    """
    解析所有 PACKAGES 文件，构建 Package -> Toolkit 映射
    """
    package_map = {}
    packages_files = glob.glob(os.path.join(src_dir, "*", "PACKAGES"))
    
    for pkg_file in packages_files:
        toolkit_name = os.path.basename(os.path.dirname(pkg_file))
        
        with open(pkg_file, 'r', encoding='utf-8') as f:
            for line in f:
                package_name = line.strip()
                if package_name:
                    package_map[package_name] = toolkit_name
    
    print(f"已解析 {len(packages_files)} 个 PACKAGES 文件，共 {len(package_map)} 个包")
    return package_map

def infer_package_from_filename(filepath):
    """
    从文件名推断 Package 名称
    """
    basename = os.path.basename(filepath)
    name_without_ext = os.path.splitext(basename)[0]
    
    if '_' in name_without_ext:
        package_name = name_without_ext.split('_')[0]
        return package_name
    else:
        return name_without_ext

def get_file_base_names(src_dir, inc_dir):
    """
    获取所有文件基础名（头文件和源文件同名视为一个）
    """
    files_info = defaultdict(lambda: {'has_hxx': False, 'has_cxx': False, 'package': ''})
    
    # 扫描头文件
    for hxx_file in glob.glob(os.path.join(inc_dir, "*.hxx")):
        basename = os.path.splitext(os.path.basename(hxx_file))[0]
        files_info[basename]['has_hxx'] = True
        files_info[basename]['package'] = infer_package_from_filename(hxx_file)
    
    # 扫描源文件
    for cxx_file in glob.glob(os.path.join(src_dir, "*", "*.cxx")):
        basename = os.path.splitext(os.path.basename(cxx_file))[0]
        files_info[basename]['has_cxx'] = True
        if not files_info[basename]['package']:
            files_info[basename]['package'] = infer_package_from_filename(cxx_file)
    
    print(f"共找到 {len(files_info)} 个唯一文件名")
    return files_info

def extract_entities_from_file(filepath):
    """
    从单个头文件中提取类、结构体、枚举、命名空间等实体
    """
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
    except Exception as e:
        return []

    entities = []
    seen_entities = set()  # 用于去重
    target_filename = os.path.basename(filepath).lower()
    
    def visit(node, parent_namespace=""):
        # 只处理当前文件定义的实体
        if node.location.file:
            node_fname = os.path.basename(node.location.file.name).lower()
            if node_fname != target_filename:
                return
        
        # 类
        if node.kind == CursorKind.CLASS_DECL and node.is_definition():
            if node.spelling:
                full_name = f"{parent_namespace}::{node.spelling}" if parent_namespace else node.spelling
                entity_key = ('Class', full_name)
                if entity_key not in seen_entities:
                    seen_entities.add(entity_key)
                    entities.append({
                        'type': 'Class',
                        'name': node.spelling,
                        'full_name': full_name,
                        'namespace': parent_namespace
                    })
        
        # 结构体
        elif node.kind == CursorKind.STRUCT_DECL and node.is_definition():
            if node.spelling:
                full_name = f"{parent_namespace}::{node.spelling}" if parent_namespace else node.spelling
                entity_key = ('Struct', full_name)
                if entity_key not in seen_entities:
                    seen_entities.add(entity_key)
                    entities.append({
                        'type': 'Struct',
                        'name': node.spelling,
                        'full_name': full_name,
                        'namespace': parent_namespace
                    })
        
        # 枚举
        elif node.kind == CursorKind.ENUM_DECL and node.is_definition():
            if node.spelling:
                full_name = f"{parent_namespace}::{node.spelling}" if parent_namespace else node.spelling
                entity_key = ('Enum', full_name)
                if entity_key not in seen_entities:
                    seen_entities.add(entity_key)
                    entities.append({
                        'type': 'Enum',
                        'name': node.spelling,
                        'full_name': full_name,
                        'namespace': parent_namespace
                    })
        
        # 命名空间
        elif node.kind == CursorKind.NAMESPACE:
            if node.spelling:
                new_namespace = f"{parent_namespace}::{node.spelling}" if parent_namespace else node.spelling
                entity_key = ('Namespace', new_namespace)
                if entity_key not in seen_entities:
                    seen_entities.add(entity_key)
                    entities.append({
                        'type': 'Namespace',
                        'name': node.spelling,
                        'full_name': new_namespace,
                        'namespace': parent_namespace
                    })
                # 递归处理命名空间内的实体（即使已记录过命名空间，也要处理其内容）
                for child in node.get_children():
                    visit(child, new_namespace)
                return
        
        # 递归处理子节点
        for child in node.get_children():
            visit(child, parent_namespace)
    
    visit(tu.cursor)
    return entities

def save_to_csv(output_dir, package_map, files_info, all_entities):
    """
    保存所有数据到 CSV 文件
    """
    os.makedirs(output_dir, exist_ok=True)
    
    # ========== 1. Toolkits.csv ==========
    toolkit_packages = defaultdict(list)
    for package, toolkit in package_map.items():
        toolkit_packages[toolkit].append(package)
    
    with open(os.path.join(output_dir, "toolkits.csv"), 'w', newline='', encoding='utf-8-sig') as f:
        writer = csv.writer(f)
        writer.writerow(["original_name", "refactored_name", "type", "package_count"])
        
        for toolkit in sorted(toolkit_packages.keys()):
            writer.writerow([
                toolkit,
                "",  # 重构后的名称（待填写）
                "Toolkit",
                len(toolkit_packages[toolkit])
            ])
    
    print(f"已生成: {output_dir}/toolkits.csv")
    
    # ========== 2. Packages.csv ==========
    package_files = defaultdict(int)
    for basename, info in files_info.items():
        package = info['package']
        if package:
            package_files[package] += 1
    
    with open(os.path.join(output_dir, "packages.csv"), 'w', newline='', encoding='utf-8-sig') as f:
        writer = csv.writer(f)
        writer.writerow(["original_name", "refactored_name", "type", "toolkit", "file_count"])
        
        for package in sorted(package_map.keys()):
            writer.writerow([
                package,
                "",
                "Package",
                package_map[package],
                package_files.get(package, 0)
            ])
    
    print(f"已生成: {output_dir}/packages.csv")
    
    # ========== 3. Files.csv ==========
    with open(os.path.join(output_dir, "files.csv"), 'w', newline='', encoding='utf-8-sig') as f:
        writer = csv.writer(f)
        writer.writerow(["original_name", "refactored_name", "type", "package", "toolkit", "has_hxx", "has_cxx"])
        
        for basename in sorted(files_info.keys()):
            info = files_info[basename]
            package = info['package']
            toolkit = package_map.get(package, "Unknown")
            
            writer.writerow([
                basename,
                "",
                "File",
                package,
                toolkit,
                "Yes" if info['has_hxx'] else "No",
                "Yes" if info['has_cxx'] else "No"
            ])
    
    print(f"已生成: {output_dir}/files.csv")
    
    # ========== 4. Classes.csv ==========
    with open(os.path.join(output_dir, "classes.csv"), 'w', newline='', encoding='utf-8-sig') as f:
        writer = csv.writer(f)
        writer.writerow(["original_name", "refactored_name", "type", "full_name", "namespace", "package", "toolkit"])
        
        for entity in sorted(all_entities, key=lambda x: x['full_name']):
            if entity['type'] == 'Class':
                package = entity['name'].split('_')[0] if '_' in entity['name'] else entity['name']
                toolkit = package_map.get(package, "Unknown")
                
                writer.writerow([
                    entity['name'],
                    "",
                    entity['type'],
                    entity['full_name'],
                    entity['namespace'],
                    package,
                    toolkit
                ])
    
    print(f"已生成: {output_dir}/classes.csv")
    
    # ========== 5. Structs.csv ==========
    with open(os.path.join(output_dir, "structs.csv"), 'w', newline='', encoding='utf-8-sig') as f:
        writer = csv.writer(f)
        writer.writerow(["original_name", "refactored_name", "type", "full_name", "namespace", "package", "toolkit"])
        
        for entity in sorted(all_entities, key=lambda x: x['full_name']):
            if entity['type'] == 'Struct':
                package = entity['name'].split('_')[0] if '_' in entity['name'] else entity['name']
                toolkit = package_map.get(package, "Unknown")
                
                writer.writerow([
                    entity['name'],
                    "",
                    entity['type'],
                    entity['full_name'],
                    entity['namespace'],
                    package,
                    toolkit
                ])
    
    print(f"已生成: {output_dir}/structs.csv")
    
    # ========== 6. Enums.csv ==========
    with open(os.path.join(output_dir, "enums.csv"), 'w', newline='', encoding='utf-8-sig') as f:
        writer = csv.writer(f)
        writer.writerow(["original_name", "refactored_name", "type", "full_name", "namespace", "package", "toolkit"])
        
        for entity in sorted(all_entities, key=lambda x: x['full_name']):
            if entity['type'] == 'Enum':
                package = entity['name'].split('_')[0] if '_' in entity['name'] else entity['name']
                toolkit = package_map.get(package, "Unknown")
                
                writer.writerow([
                    entity['name'],
                    "",
                    entity['type'],
                    entity['full_name'],
                    entity['namespace'],
                    package,
                    toolkit
                ])
    
    print(f"已生成: {output_dir}/enums.csv")
    
    # ========== 7. Namespaces.csv ==========
    with open(os.path.join(output_dir, "namespaces.csv"), 'w', newline='', encoding='utf-8-sig') as f:
        writer = csv.writer(f)
        writer.writerow(["original_name", "refactored_name", "type", "full_name", "parent_namespace"])
        
        for entity in sorted(all_entities, key=lambda x: x['full_name']):
            if entity['type'] == 'Namespace':
                writer.writerow([
                    entity['name'],
                    "",
                    entity['type'],
                    entity['full_name'],
                    entity['namespace']
                ])
    
    print(f"已生成: {output_dir}/namespaces.csv")
    
    # ========== 8. ALL_ENTITIES.csv (合并所有实体) ==========
    with open(os.path.join(output_dir, "all_entities.csv"), 'w', newline='', encoding='utf-8-sig') as f:
        writer = csv.writer(f)
        writer.writerow(["entity_type", "original_name", "refactored_name", "full_name", "namespace", "package", "toolkit", "metadata"])
        
        # Toolkits
        for toolkit in sorted(toolkit_packages.keys()):
            writer.writerow([
                "Toolkit",
                toolkit,
                "",
                toolkit,
                "",
                "",
                "",
                f"packages={len(toolkit_packages[toolkit])}"
            ])
        
        # Packages
        for package in sorted(package_map.keys()):
            writer.writerow([
                "Package",
                package,
                "",
                package,
                "",
                package,
                package_map[package],
                f"files={package_files.get(package, 0)}"
            ])
        
        # Files
        for basename in sorted(files_info.keys()):
            info = files_info[basename]
            package = info['package']
            toolkit = package_map.get(package, "Unknown")
            metadata = f"hxx={'Yes' if info['has_hxx'] else 'No'},cxx={'Yes' if info['has_cxx'] else 'No'}"
            
            writer.writerow([
                "File",
                basename,
                "",
                basename,
                "",
                package,
                toolkit,
                metadata
            ])
        
        # Classes, Structs, Enums, Namespaces
        for entity in sorted(all_entities, key=lambda x: (x['type'], x['full_name'])):
            if entity['type'] in ['Class', 'Struct', 'Enum']:
                package = entity['name'].split('_')[0] if '_' in entity['name'] else entity['name']
                toolkit = package_map.get(package, "Unknown")
            else:
                package = ""
                toolkit = ""
            
            writer.writerow([
                entity['type'],
                entity['name'],
                "",
                entity['full_name'],
                entity['namespace'],
                package,
                toolkit,
                ""
            ])
    
    print(f"已生成: {output_dir}/all_entities.csv (合并文件)")

def main():
    print("开始提取 OCCT 实体信息 (CSV 格式)...")
    
    # 1. 构建 Package -> Toolkit 映射
    package_map = build_package_to_toolkit_map(OCCT_SRC_DIR)
    
    # 2. 获取所有文件信息
    files_info = get_file_base_names(OCCT_SRC_DIR, OCCT_INC_DIR)
    
    # 3. 从所有头文件中提取实体
    all_entities = []
    global_seen_entities = {}  # 全局去重: key=(type, full_name), value=entity_dict
    hxx_files = glob.glob(os.path.join(OCCT_INC_DIR, "*.hxx"))
    
    print(f"\n开始解析 {len(hxx_files)} 个头文件...")
    for i, hxx_file in enumerate(hxx_files, 1):
        if i % 500 == 0:
            print(f"进度: {i}/{len(hxx_files)} ({(i/len(hxx_files)*100):.1f}%)")
        
        entities = extract_entities_from_file(hxx_file)
        
        # 全局去重
        for entity in entities:
            entity_key = (entity['type'], entity['full_name'])
            if entity_key not in global_seen_entities:
                global_seen_entities[entity_key] = entity
                all_entities.append(entity)
    
    print(f"\n共提取 {len(all_entities)} 个实体:")
    entity_counts = defaultdict(int)
    for entity in all_entities:
        entity_counts[entity['type']] += 1
    
    for entity_type, count in sorted(entity_counts.items()):
        print(f"  - {entity_type}: {count}")
    
    # 4. 保存到 CSV 文件
    print(f"\n正在生成 CSV 文件到目录: {OUTPUT_DIR}/")
    save_to_csv(OUTPUT_DIR, package_map, files_info, all_entities)
    
    print(f"\n完成！所有 CSV 文件已保存到: {OUTPUT_DIR}/")
    print(f"  - 单独文件: toolkits.csv, packages.csv, files.csv, classes.csv, structs.csv, enums.csv, namespaces.csv")
    print(f"  - 合并文件: all_entities.csv")

if __name__ == "__main__":
    main()
