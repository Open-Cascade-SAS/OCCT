import os
import re
from bs4 import BeautifulSoup
from pathlib import Path
import glob

import os.path

def get_referenced_images(html_file):
    # Get the directory containing the HTML file
    html_dir = os.path.dirname(os.path.abspath(html_file))
    
    with open(html_file, 'r', encoding='utf-8') as f:
        soup = BeautifulSoup(f.read(), 'html.parser')
    
    images = set()
    
    # Extract direct image references
    for img in soup.find_all('img'):
        if src := img.get('src'):
            # Convert relative path to absolute
            abs_path = os.path.normpath(os.path.join(html_dir, src))
            images.add(abs_path)
    
    # Extract toggle references
    for elem in soup.find_all(attrs={'onclick': True}):
        onclick = elem['onclick']
        paths = re.findall(r'diffimage_toggle\(this,"([^"]+)","([^"]+)"\)', onclick)
        for src1, src2 in paths:
            # Convert relative paths to absolute
            abs_path1 = os.path.normpath(os.path.join(html_dir, src1))
            abs_path2 = os.path.normpath(os.path.join(html_dir, src2))
            images.add(abs_path1)
            images.add(abs_path2)
    
    return images

def cleanup_platform_images(results_dir, platform):
    html_file = f"{results_dir}/current/{platform}/diff-*.html"
    html_files = glob.glob(html_file)
    
    if not html_files:
        print(f"No diff HTML found for {platform}")
        return
        
    # Get referenced images from HTML
    referenced = set()
    for html in html_files:
        images = get_referenced_images(html)
        referenced.update(images)
    
    # Convert relative paths to absolute
    base_dir = Path(results_dir)
    current_dir = base_dir / "current" / platform
    master_dir = base_dir / "master" / platform
    
    # Find all PNGs
    png_files = set()
    for directory in [current_dir, master_dir]:
        for root, _, files in os.walk(directory):
            for file in files:
                if file.lower().endswith('.png'):
                    png_files.add(Path(root) / file)

    # Remove unreferenced PNGs
    for png in png_files:
        if str(png) not in referenced:
            try:
                png.unlink()
            except OSError as e:
                print(f"Error removing {png}: {e}")

def main():
    platforms = [
        "windows-x64",
        "windows-clang-x64", 
        "macos-x64",
        "macos-gcc-x64",
        "linux-clang-x64",
        "linux-gcc-x64"
    ]
    
    results_dir = Path("./").resolve()
    
    for platform in platforms:
        print(f"\nProcessing {platform}...")
        cleanup_platform_images(results_dir, platform)

if __name__ == "__main__":
    main()
