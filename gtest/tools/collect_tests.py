import os
import argparse
import re

# Commands that indicate a test requires interactive graphics/UI
INTERACTIVE_KEYWORDS = [
    "vinit", "vdisplay", "vset", "vfit", "vview", "look", "render", 
    "ivtk", "mesh", "ivdisplay", "AIS", "ViewerTest", "smallview", "donly", "fit"
]

# File extensions or metadata files to ignore
IGNORE_FILES = ["parse.rules", "parse.history", "begin", "end", ".rules", "grids.list"]

def scan_data_files(data_dir):
    """Recursively scan a data directory and return a map of filename -> relative path."""
    data_map = {}
    if not data_dir or not os.path.exists(data_dir):
        return data_map
    
    for root, _, files in os.walk(data_dir):
        for file in files:
            # Map basename to its full path for easy lookup
            data_map[file] = os.path.join(root, file)
    return data_map

def should_skip_content(content):
    """Check if the Tcl content contains interactive UI commands."""
    for kw in INTERACTIVE_KEYWORDS:
        # Use regex to avoid matching inside words
        if re.search(rf"\b{kw}\b", content):
            return True
    return False

def extract_dependencies(content):
    """Extract data file dependencies and complex commands."""
    deps = re.findall(r"locate_data_file\s+([\w\.-]+)", content)
    cmds = []
    if "filling" in content: cmds.append("BRepOffsetAPI_MakeFilling")
    if "fillcurves" in content: cmds.append("GeomFill_BSplineCurves / GeomFill_ConstrainedFilling")
    if "bsplinecurve" in content: cmds.append("Geom_BSplineCurve")
    return deps, cmds

def collect_tests(root_dir, batch_size=10, data_map=None):
    """Collect Tcl tests and group them into batches for AI processing."""
    batches = []
    current_batch = []
    
    # Normalize path to handle "tests/bugs/filling" correctly
    abs_base = os.path.abspath("tests") # The static root of OCCT tests
    
    for root, dirs, files in os.walk(root_dir):
        # Determine the Group and Grid relative to the main 'tests' directory
        try:
            rel_path = os.path.relpath(root, abs_base)
            parts = rel_path.split(os.sep)
            # parts will be something like ["bugs", "filling"]
            group = parts[0] if len(parts) > 0 else "unknown"
            grid = parts[1] if len(parts) > 1 else ""
        except ValueError:
            group = "unknown"
            grid = "unknown"
        
        for file in files:
            if any(file.endswith(ext) for ext in IGNORE_FILES) or file in ["begin", "end"]:
                continue
            
            file_path = os.path.join(root, file)
            try:
                with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                    content = f.read()
                
                if should_skip_content(content):
                    continue
                
                data_deps, cpp_hints = extract_dependencies(content)
                
                # Filtering logic: check if data dependencies exist
                missing_deps = []
                resolved_deps = {}
                if data_map is not None:
                    for dep in data_deps:
                        if dep in data_map:
                            resolved_deps[dep] = data_map[dep]
                        else:
                            missing_deps.append(dep)
                    
                    if missing_deps:
                        print(f"Skipping {file_path}: Missing data files {missing_deps}")
                        continue
                
                # Metadata for the AI
                test_info = {
                    "path": f"{group}/{grid}/{file}",
                    "suite": f"OCCT_{group.capitalize()}_{grid.replace('.', '_')}",
                    "case": file.replace(".", "_"),
                    "content": content.strip(),
                    "deps": data_deps,
                    "resolved_deps": resolved_deps,
                    "hints": cpp_hints
                }
                current_batch.append(test_info)
                
                if len(current_batch) >= batch_size:
                    batches.append(current_batch)
                    current_batch = []
                    
            except Exception as e:
                print(f"Error reading {file_path}: {e}")

    if current_batch:
        batches.append(current_batch)
    
    return batches

def format_markdown(batches):
    """Format collected batches into a Markdown file for AI copy-pasting."""
    output = "# OCCT Test Migration Batches\n\n"
    output += "Below are batches of Tcl scripts. Paste one batch at a time to the AI.\n\n"
    
    for i, batch in enumerate(batches):
        output += f"--- BATCH {i+1} START ---\n\n"
        output += "I am converting these tests to direct C++ API Gtest cases.\n\n"
        
        for test in batch:
            output += f"### Test: {test['path']}\n"
            output += f"- **Target Suite**: {test['suite']}\n"
            output += f"- **Target Case**: {test['case']}\n"
            if test['deps']:
                resolved_info = []
                for dep in test['deps']:
                    path = test['resolved_deps'].get(dep)
                    if path:
                        resolved_info.append(f"`{dep}` (Path: `{path}`)")
                    else:
                        resolved_info.append(f"`{dep}`")
                output += f"- **Data Files**: {', '.join(resolved_info)}\n"
            if test['hints']:
                output += f"- **Suggested C++**: {', '.join(test['hints'])}\n"
            
            output += "```tcl\n"
            output += test['content'] + "\n"
            output += "```\n\n"
        
        output += f"--- BATCH {i+1} END ---\n\n"
    
    return output

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Collect OCCT Tcl tests for AI conversion.")
    parser.add_argument("path", help="Path to the test folder (e.g., tests/bugs/filling)")
    parser.add_argument("--batch", type=int, default=5, help="Number of tests per batch (default: 5)")
    parser.add_argument("--out", help="Output filename (default: batches_[path_slug].md)")
    parser.add_argument("--data", help="Optional path to the data directory to filter tests by availability")
    
    args = parser.parse_args()
    
    # Auto-generate output filename if not provided
    if not args.out:
        # Normalize path and remove 'tests/' prefix if present
        clean_path = os.path.normpath(args.path)
        if clean_path.startswith("tests" + os.sep):
            clean_path = clean_path[len("tests" + os.sep):]
        elif clean_path == "tests":
            clean_path = "all"
            
        slug = clean_path.replace(os.sep, "_").replace(".", "_")
        args.out = f"batches_{slug}.md"
    
    print(f"Scanning {args.path}...")
    
    data_map = None
    if args.data:
        print(f"Indexing data files in {args.data}...")
        data_map = scan_data_files(args.data)
        print(f"Found {len(data_map)} data files.")
        
    batches = collect_tests(args.path, args.batch, data_map)
    
    if not batches:
        print("No suitable non-interactive tests found. (Checks skip keywords: vinit, vdisplay, AIS, etc.)")
    else:
        md_content = format_markdown(batches)
        with open(args.out, 'w', encoding='utf-8') as f:
            f.write(md_content)
        print(f"Total: {sum(len(b) for b in batches)} tests in {len(batches)} batches.")
        print(f"Result saved to: {args.out}")
