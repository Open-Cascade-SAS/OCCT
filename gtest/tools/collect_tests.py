import os
import argparse

# Commands that indicate a test requires interactive graphics/UI
INTERACTIVE_KEYWORDS = [
    "vinit", "vdisplay", "vset", "vfit", "vview", "look", "render", 
    "ivtk", "mesh", "ivdisplay", "AIS", "ViewerTest"
]

# File extensions or metadata files to ignore
IGNORE_FILES = ["parse.rules", "parse.history", "begin", "end", ".rules"]

def should_skip_content(content):
    """Check if the Tcl content contains interactive UI commands."""
    for kw in INTERACTIVE_KEYWORDS:
        if kw in content:
            return True
    return False

def collect_tests(root_dir, batch_size=10):
    """Collect Tcl tests and group them into batches for AI processing."""
    batches = []
    current_batch = []
    
    for root, dirs, files in os.walk(root_dir):
        # Determine the Group and Grid from the folder structure
        # Assume OCCT structure: tests/<group>/<grid>/<test>
        parts = os.path.relpath(root, root_dir).split(os.sep)
        group = parts[0] if len(parts) > 0 else "unknown"
        grid = parts[1] if len(parts) > 1 else ""
        
        for file in files:
            if any(file.endswith(ext) for ext in IGNORE_FILES) or file == "begin" or file == "end":
                continue
            
            file_path = os.path.join(root, file)
            try:
                with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                    content = f.read()
                
                if should_skip_content(content):
                    continue
                
                # Metadata for the AI
                test_info = {
                    "path": f"{group}/{grid}/{file}",
                    "suite": f"OCCT_{group.capitalize()}_{grid.replace('.', '_')}",
                    "case": file.replace(".", "_"),
                    "content": content.strip()
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
    output += "Each block below contains a batch of Tcl scripts. Copy them to the AI one by one.\n\n"
    
    for i, batch in enumerate(batches):
        output += f"## Batch {i+1} ({len(batch)} tests)\n\n"
        output += "```markdown\n"
        output += f"I have {len(batch)} Tcl scripts to convert to Gtest within the same domain.\n"
        output += "Please convert each into a separate TEST() case in a single C++ file.\n\n"
        
        for test in batch:
            output += f"### Test: {test['path']}\n"
            output += f"- Suite: {test['suite']}\n"
            output += f"- Case: {test['case']}\n"
            output += "```tcl\n"
            output += test['content'] + "\n"
            output += "```\n\n"
            output += "---\n\n"
        
        output += "```\n\n"
    
    return output

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Collect OCCT Tcl tests for AI conversion.")
    parser.add_argument("path", help="Path to the test folder (e.g., tests/bugs/filling)")
    parser.add_argument("--batch", type=int, default=5, help="Number of tests per batch (default: 5)")
    parser.add_argument("--out", default="test_batches.md", help="Output filename")
    
    args = parser.parse_args()
    
    print(f"Scanning {args.path}...")
    batches = collect_tests(args.path, args.batch)
    
    if not batches:
        print("No suitable non-interactive tests found.")
    else:
        md_content = format_markdown(batches)
        with open(args.out, 'w', encoding='utf-8') as f:
            f.write(md_content)
        print(f"Successfully collected {sum(len(b) for b in batches)} tests into {len(batches)} batches.")
        print(f"Output saved to: {args.out}")
