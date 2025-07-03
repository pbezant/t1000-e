#!/usr/bin/env python3
"""
Comprehensive fix for SES project - remove ALL Nordic SDK dependencies
"""

import re
import sys

def fix_ses_project_complete(project_file):
    print(f"🔧 Comprehensive SES project fix: {project_file}")
    
    with open(project_file, 'r') as f:
        content = f.read()
    
    # Backup original
    backup_file = project_file + '.original'
    with open(backup_file, 'w') as f:
        f.write(content)
    print(f"💾 Original backed up to {backup_file}")
    
    # Remove ALL Nordic SDK library references
    print("🗑️  Removing ALL Nordic SDK library files...")
    
    # Remove entire nRF_Libraries folder
    content = re.sub(r'<folder Name="nRF_Libraries">.*?</folder>', '', content, flags=re.DOTALL)
    
    # Remove any remaining components/libraries references
    pattern = r'<file file_name="[^"]*components/libraries[^"]*" />'
    content = re.sub(pattern, '', content)
    
    # Remove components from include paths - keep only essential ones
    print("📝 Cleaning include paths...")
    
    # Extract the include directories line
    include_match = re.search(r'c_user_include_directories="([^"]*)"', content)
    if include_match:
        include_dirs = include_match.group(1)
        
        # Keep only essential directories (remove components/libraries)
        essential_dirs = []
        for dir_path in include_dirs.split(';'):
            if 'components/libraries' not in dir_path and 'components/ble' not in dir_path:
                essential_dirs.append(dir_path)
        
        new_include_dirs = ';'.join(essential_dirs)
        content = content.replace(include_match.group(0), f'c_user_include_directories="{new_include_dirs}"')
    
    # Add our simple replacements
    print("➕ Adding minimal replacements...")
    
    # Find Application folder and add minimal files
    app_folder_pattern = r'(<folder Name="Application">.*?)(</folder>)'
    replacement = r'''\1      <file file_name="../minimal_nordic.c" />
      <file file_name="../../../t1000_e/tracker/inc/app_ble_beacon.h" />
      <file file_name="../../../t1000_e/tracker/src/app_ble_beacon.c" />
    \2'''
    content = re.sub(app_folder_pattern, replacement, content, flags=re.DOTALL)
    
    # Write fixed project
    with open(project_file, 'w') as f:
        f.write(content)
    
    print("✅ SES project completely fixed!")
    print("📋 Removed: ALL Nordic SDK library dependencies")
    print("📋 Added: Minimal replacements + beacon files")

if __name__ == "__main__":
    project_file = "Seeed-Tracker-T1000-E-for-LoRaWAN-dev-board/pca10056/s140/11_ses_lorawan_tracker/t1000_e_dev_kit_pca10056.emProject"
    fix_ses_project_complete(project_file) 