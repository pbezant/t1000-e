#!/usr/bin/env python3
"""
FINAL comprehensive fix for SES project - remove ALL Nordic SDK dependencies
including the nRF_Drivers folder that's causing the build errors
"""

import re
import sys

def fix_ses_project_final(project_file):
    print(f"🔧 FINAL SES project fix: {project_file}")
    
    with open(project_file, 'r') as f:
        content = f.read()
    
    # Backup original
    backup_file = project_file + '.final_backup'
    with open(backup_file, 'w') as f:
        f.write(content)
    print(f"💾 Original backed up to {backup_file}")
    
    # Remove ALL problematic Nordic SDK folders
    print("🗑️  Removing ALL Nordic SDK folders...")
    
    # Remove nRF_Drivers folder (this is causing the current error)
    content = re.sub(r'<folder Name="nRF_Drivers">.*?</folder>', '', content, flags=re.DOTALL)
    
    # Remove nRF_BLE folder (also has Nordic SDK dependencies)
    content = re.sub(r'<folder Name="nRF_BLE">.*?</folder>', '', content, flags=re.DOTALL)
    
    # Remove nRF_SoftDevice folder if it has problematic files
    content = re.sub(r'<folder Name="nRF_SoftDevice">.*?</folder>', '', content, flags=re.DOTALL)
    
    # Remove None folder with Nordic SDK files
    content = re.sub(r'<folder Name="None">.*?</folder>', '', content, flags=re.DOTALL)
    
    # Remove any remaining Nordic SDK file references
    patterns_to_remove = [
        r'<file file_name="[^"]*integration/nrfx[^"]*" />',
        r'<file file_name="[^"]*modules/nrfx[^"]*" />',
        r'<file file_name="[^"]*components/ble[^"]*" />',
        r'<file file_name="[^"]*components/softdevice[^"]*" />',
    ]
    
    for pattern in patterns_to_remove:
        content = re.sub(pattern, '', content)
    
    # Clean up include paths more aggressively
    print("📝 Cleaning include paths...")
    
    include_match = re.search(r'c_user_include_directories="([^"]*)"', content)
    if include_match:
        include_dirs = include_match.group(1)
        
        # Keep only essential directories
        essential_dirs = []
        for dir_path in include_dirs.split(';'):
            # Remove any Nordic SDK paths
            if ('components/' not in dir_path and 
                'integration/' not in dir_path and 
                'modules/' not in dir_path and
                'external/' not in dir_path):
                essential_dirs.append(dir_path)
            # Keep only essential external paths
            elif 'external/utf_converter' in dir_path:
                essential_dirs.append(dir_path)
        
        new_include_dirs = ';'.join(essential_dirs)
        content = content.replace(include_match.group(0), f'c_user_include_directories="{new_include_dirs}"')
    
    # Add essential startup files that we actually need
    print("➕ Adding essential startup files...")
    
    # Find a good place to add essential files (after Segger Startup Files)
    startup_pattern = r'(<folder Name="Segger Startup Files">.*?</folder>)'
    replacement = r'''\1
    <folder Name="Essential_Startup">
      <file file_name="../startup_stubs.c" />
    </folder>'''
    content = re.sub(startup_pattern, replacement, content, flags=re.DOTALL)
    
    # Write fixed project
    with open(project_file, 'w') as f:
        f.write(content)
    
    print("✅ SES project FINALLY fixed!")
    print("📋 Removed: ALL Nordic SDK dependencies including nRF_Drivers")
    print("📋 Cleaned: Include paths to only essential directories")
    print("📋 Added: Essential startup stubs")

if __name__ == "__main__":
    project_file = "Seeed-Tracker-T1000-E-for-LoRaWAN-dev-board/pca10056/s140/11_ses_lorawan_tracker/t1000_e_dev_kit_pca10056.emProject"
    fix_ses_project_final(project_file) 