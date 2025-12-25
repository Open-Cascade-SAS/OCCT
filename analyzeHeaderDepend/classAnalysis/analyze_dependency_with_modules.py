import json
import sys
import csv
import collections

# ================= 配置 =================
INPUT_FILE = "occt_class_graph_with_modules.json" # 确保此文件存在

# 输出文件
FILE_1_CLEAN   = "1_occt_build_step1_clean.csv"
FILE_2_CYCLES  = "2_occt_build_step2_cycles.csv"
FILE_3_VICTIMS = "3_occt_build_step3_victims.csv"

# 是否包含弱依赖? (建议 False 以获取物理编译顺序)
INCLUDE_WEAK_DEPS = False 
# =======================================

class DependencyGraphSorter:
    def __init__(self, json_file):
        try:
            with open(json_file, 'r', encoding='utf-8') as f:
                self.raw_data = json.load(f)
        except FileNotFoundError:
            print(f"错误: 找不到文件 {json_file}")
            sys.exit(1)
        
        # 1. 数据清洗：剔除 'Handle_' 开头的类
        self.all_classes = {cls for cls in self.raw_data.keys() if not cls.startswith("Handle_")}
        print(f"原始类: {len(self.raw_data)}, 清洗后: {len(self.all_classes)}")

        self.adj = collections.defaultdict(list) 
        self.in_degree = {u: 0 for u in self.all_classes} 
        self.deps_info = {} 
        self.metadata = {}

    def get_metadata(self, cls_name):
        """获取类的模块信息"""
        if cls_name in self.raw_data:
            return self.raw_data[cls_name].get('package', 'Unknown'), self.raw_data[cls_name].get('toolkit', 'Unknown')
        return "Unknown", "Unknown"

    def build_graph(self):
        """构建依赖图"""
        print(f"正在构建依赖图 (包含弱依赖: {INCLUDE_WEAK_DEPS})...")
        for cls_name in self.all_classes:
            details = self.raw_data[cls_name]
            
            # 存储元数据
            self.metadata[cls_name] = {
                'package': details.get('package', 'Unknown'),
                'toolkit': details.get('toolkit', 'Unknown')
            }
            
            dependencies = set()
            dependencies.update(details.get('inherits', []))
            dependencies.update(details.get('fields', []))
            if INCLUDE_WEAK_DEPS:
                dependencies.update(details.get('uses', []))
            
            # 过滤依赖
            valid_deps = [d for d in dependencies if d in self.all_classes and d != cls_name]
            
            self.deps_info[cls_name] = valid_deps
            self.in_degree[cls_name] = len(valid_deps)
            
            for dep in valid_deps:
                self.adj[dep].append(cls_name)

    def sort_main_graph(self):
        """阶段 1: 正常拓扑排序"""
        print(">>> [Step 1] 正在提取基础层 (Clean Layer)...")
        current_in_degree = self.in_degree.copy()
        queue = collections.deque([u for u, d in current_in_degree.items() if d == 0])
        
        sorted_list = []
        level = 0
        
        while queue:
            level_size = len(queue)
            for _ in range(level_size):
                u = queue.popleft()
                sorted_list.append({'level': level, 'class': u})
                
                for v in self.adj[u]:
                    current_in_degree[v] -= 1
                    if current_in_degree[v] == 0:
                        queue.append(v)
            level += 1
        return sorted_list

    def separate_cycles_and_victims(self, sorted_list):
        """阶段 2: 提取循环 (Root Cause) 和 受害者 (Victims)"""
        sorted_keys = set(item['class'] for item in sorted_list)
        residual_nodes = self.all_classes - sorted_keys
        
        if not residual_nodes: return [], [], []

        print(f">>> [Step 2] 发现 {len(residual_nodes)} 个未排序类，正在分析死锁环...")

        # 构建残留子图
        graph = collections.defaultdict(list)
        for u in residual_nodes:
            for v in self.deps_info[u]:
                if v in residual_nodes:
                    graph[u].append(v)

        # Tarjan 算法找 SCC
        visited = set()
        stack = []
        on_stack = set()
        ids = {}
        low = {}
        self.id_counter = 0
        sccs = []

        def dfs(at):
            stack.append(at)
            on_stack.add(at)
            visited.add(at)
            ids[at] = low[at] = self.id_counter
            self.id_counter += 1

            for to in graph[at]:
                if to not in visited:
                    dfs(to)
                    low[at] = min(low[at], low[to])
                elif to in on_stack:
                    low[at] = min(low[at], ids[to])

            if ids[at] == low[at]:
                current_scc = []
                while stack:
                    node = stack.pop()
                    on_stack.remove(node)
                    current_scc.append(node)
                    if node == at: break
                sccs.append(current_scc)

        for node in residual_nodes:
            if node not in visited:
                dfs(node)

        cycle_nodes_set = set()
        victim_nodes_list = []
        real_cycles_info = []
        
        for scc in sccs:
            is_cycle = False
            if len(scc) > 1:
                is_cycle = True
            elif len(scc) == 1:
                node = scc[0]
                if node in self.deps_info[node]: 
                    is_cycle = True
            
            if is_cycle:
                cycle_nodes_set.update(scc)
                real_cycles_info.append(scc)
            else:
                victim_nodes_list.extend(scc)
        
        return real_cycles_info, cycle_nodes_set, victim_nodes_list

    def sort_victims(self, victim_nodes, cycle_nodes_set):
        """阶段 3: 对受害者排序"""
        print(f">>> [Step 3] 正在对 {len(victim_nodes)} 个受害者排序...")
        v_adj = collections.defaultdict(list)
        v_in_degree = {u: 0 for u in victim_nodes}
        v_set = set(victim_nodes)
        
        for u in victim_nodes:
            for v in self.deps_info[u]:
                if v in v_set:
                    v_in_degree[u] += 1
                    v_adj[v].append(u)
        
        queue = collections.deque([u for u, d in v_in_degree.items() if d == 0])
        sorted_victims = []
        level = 0
        
        while queue:
            level_size = len(queue)
            for _ in range(level_size):
                u = queue.popleft()
                blocker_cycles = [d for d in self.deps_info[u] if d in cycle_nodes_set]
                
                blocker_toolkits = set()
                for bc in blocker_cycles:
                    _, tk = self.get_metadata(bc)
                    blocker_toolkits.add(tk)
                
                cycle_note = ""
                if blocker_cycles:
                    cycle_note = f"Blocked by Cycle in {list(blocker_toolkits)}: {blocker_cycles[0]}..."

                sorted_victims.append({
                    'level': level, 
                    'class': u,
                    'cycle_note': cycle_note
                })
                
                for v in v_adj[u]:
                    v_in_degree[v] -= 1
                    if v_in_degree[v] == 0:
                        queue.append(v)
            level += 1
        return sorted_victims

    def export_csv(self, clean_list, cycles_info, sorted_victims):
        """
        导出函数 (修改版): 在写入 CSV 之前对数据进行排序
        排序优先级: Level (升序) -> Toolkit (升序) -> Package (升序) -> ClassName (升序)
        """
        
        # --- 1. CLEAN LIST (基础层) ---
        print(f"正在导出 {FILE_1_CLEAN} (已按层级和模块排序)...")
        clean_rows = []
        for item in clean_list:
            pkg, tk = self.get_metadata(item['class'])
            # 构建一个元组列表，方便排序
            # 格式: (Level, Toolkit, Package, ClassName, Note)
            clean_rows.append( (item['level'], tk, pkg, item['class'], 'Ready') )
        
        # 执行排序: Python 的 sort 会依次比较元组中的元素
        clean_rows.sort(key=lambda x: (x[0], x[1], x[2], x[3]))

        with open(FILE_1_CLEAN, 'w', newline='', encoding='utf-8') as f:
            w = csv.writer(f)
            w.writerow(['Order', 'Level', 'Toolkit', 'Package', 'Class Name', 'Note'])
            for i, row in enumerate(clean_rows):
                # Order 列根据最终排序结果重新生成 (i+1)
                w.writerow([i+1, row[0], row[1], row[2], row[3], row[4]])


        # --- 2. CYCLES (循环层) ---
        print(f"正在导出 {FILE_2_CYCLES} (已按模块排序)...")
        # 循环没有 Level，所以我们主要按 Toolkit -> Package 排序
        cycle_rows = []
        
        # 先给循环排个序，方便分配 ID (保持按大小降序，为了先看大环)
        cycles_info.sort(key=len, reverse=True)
        
        for idx, scc in enumerate(cycles_info):
            cycle_id = idx + 1
            size = len(scc)
            scc_str = " <-> ".join(scc[:3])
            context = f"Cycle: {scc_str}..."
            
            for node in scc:
                pkg, tk = self.get_metadata(node)
                # 格式: (Toolkit, Package, CycleID, Size, ClassName, Context)
                cycle_rows.append( (tk, pkg, cycle_id, size, node, context) )
        
        # 执行排序: 按 Toolkit -> Package -> CycleID -> ClassName
        cycle_rows.sort(key=lambda x: (x[0], x[1], x[2], x[4]))

        with open(FILE_2_CYCLES, 'w', newline='', encoding='utf-8') as f:
            w = csv.writer(f)
            w.writerow(['Cycle ID', 'Size', 'Toolkit', 'Package', 'Class Name', 'Context'])
            for row in cycle_rows:
                # 注意：Cycle ID 在这里可能会打乱顺序显示（因为我们要把同一模块的类聚在一起）
                # 这是符合预期的，因为我们想看的是"这个模块里有哪些循环类"
                w.writerow([row[2], row[3], row[0], row[1], row[4], row[5]])


        # --- 3. VICTIMS (受害层) ---
        print(f"正在导出 {FILE_3_VICTIMS} (已按层级和模块排序)...")
        victim_rows = []
        for item in sorted_victims:
            pkg, tk = self.get_metadata(item['class'])
            note = item['cycle_note'] or "Depends on other Victims"
            # 格式: (RelativeLevel, Toolkit, Package, ClassName, Note)
            victim_rows.append( (item['level'], tk, pkg, item['class'], note) )
            
        # 执行排序: Level -> Toolkit -> Package -> ClassName
        victim_rows.sort(key=lambda x: (x[0], x[1], x[2], x[3]))

        with open(FILE_3_VICTIMS, 'w', newline='', encoding='utf-8') as f:
            w = csv.writer(f)
            w.writerow(['Order', 'Relative Level', 'Toolkit', 'Package', 'Class Name', 'Dependency Logic'])
            for i, row in enumerate(victim_rows):
                w.writerow([i+1, row[0], row[1], row[2], row[3], row[4]])

        print("全部完成！")

if __name__ == "__main__":
    sorter = DependencyGraphSorter(INPUT_FILE)
    sorter.build_graph()
    
    clean_list = sorter.sort_main_graph()
    cycles_info, cycle_set, victim_list = sorter.separate_cycles_and_victims(clean_list)
    sorted_victims = sorter.sort_victims(victim_list, cycle_set)
    
    sorter.export_csv(clean_list, cycles_info, sorted_victims)