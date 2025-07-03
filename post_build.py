import os
from SCons.Script import DefaultEnvironment

env = DefaultEnvironment()

# Path to uf2conv.py
UF2CONV = os.path.join(
    env['PROJECT_DIR'],
    "Seeed-Tracker-T1000-E-for-LoRaWAN-dev-board",
    "firmware",
    "uf2conv.py"
)

# Output paths
hex_path = os.path.join(env['BUILD_DIR'], env['PIOENV'], "firmware.hex")
uf2_path = os.path.join(env['BUILD_DIR'], env['PIOENV'], "firmware.uf2")

def hex_to_uf2(source, target, env):
    cmd = f'python "{UF2CONV}" "{hex_path}" -c -f 0xADA52840 -o "{uf2_path}"'
    print(f"[post_build] Converting HEX to UF2: {cmd}")
    result = os.system(cmd)
    if result == 0:
        print(f"[post_build] UF2 file generated: {uf2_path}")
    else:
        print(f"[post_build] UF2 conversion failed!")
    return result

# Add post-action to run after .hex is built
env.AddPostAction(hex_path, hex_to_uf2) 