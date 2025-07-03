#!/usr/bin/env python3
"""
Fix SES project file by removing problematic Nordic SDK dependencies
"""

import re
import sys

def fix_ses_project(project_file):
    print(f"🔧 Fixing SES project: {project_file}")
    
    with open(project_file, 'r') as f:
        content = f.read()
    
    # Remove problematic nRF_Log files
    print("📝 Removing nRF_Log dependencies...")
    content = re.sub(r'<folder Name="nRF_Log">.*?</folder>', '', content, flags=re.DOTALL)
    
    # Remove other problematic Nordic SDK files that might not exist
    problematic_files = [
        'nrf_log_frontend.c',
        'nrf_log_str_formatter.c',
        'app_error_handler_gcc.c',
        'app_util_platform.c',
    ]
    
    for file_name in problematic_files:
        print(f"🗑️  Removing references to {file_name}")
        pattern = rf'<file file_name="[^"]*{re.escape(file_name)}" />'
        content = re.sub(pattern, '', content)
    
    # Add simple replacements for essential functions
    print("➕ Adding simplified replacements...")
    
    # Find the Application folder and add our simple replacements
    app_folder_pattern = r'(<folder Name="Application">)'
    replacement = r'\1\n      <file file_name="../simple_log.c" />'
    content = re.sub(app_folder_pattern, replacement, content)
    
    # Write the fixed project
    backup_file = project_file + '.backup'
    print(f"💾 Backing up original to {backup_file}")
    
    with open(backup_file, 'w') as f:
        f.write(content)
    
    with open(project_file, 'w') as f:
        f.write(content)
    
    print("✅ SES project fixed!")

if __name__ == "__main__":
    project_file = "Seeed-Tracker-T1000-E-for-LoRaWAN-dev-board/pca10056/s140/11_ses_lorawan_tracker/t1000_e_dev_kit_pca10056.emProject"
    fix_ses_project(project_file) 