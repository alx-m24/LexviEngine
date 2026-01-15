import sys
import requests

# generate enum class of keys matching GLFW keys

if len(sys.argv) < 2:
    print("Usage: python3 GetKeys.py <output_path>") 
    sys.exit(1)

outPath = sys.argv[1]

glfwURL = "https://raw.githubusercontent.com/glfw/glfw/master/include/GLFW/glfw3.h"
outGLFWPath = "temp/glfw3.h"

r = requests.get(glfwURL)
r.raise_for_status()

outString = """#pragma once

#include <cstdint>

namespace Lexvi {
     enum class Key : int32_t {
"""

keysNum:int = 0
MaxGLFWKey:int = 0
for line in r.text.splitlines():
    if line.startswith("#define GLFW_KEY_"):
        parts = line.split()
        if len(parts) >= 3:
            name = parts[1].replace("GLFW_KEY_", "")  # Strip prefix
            if name != "UNKNOWN" and name != "LAST":
                if name.isdigit():
                    name = "N" + name
                value = parts[2]
                outString += f"        {name} = {value},\n"
                keysNum += 1
                MaxGLFWKey = max(int(value), MaxGLFWKey)

outString += "    };\n\n"
outString += "    enum class Mouse : int32_t {\n"

mouseButtonNum = 0
MaxGLFWMouse = 0
aliases = {}  # name -> target

# First pass: real numeric buttons
for line in r.text.splitlines():
    if line.startswith("#define GLFW_MOUSE_BUTTON_"):
        parts = line.split()
        if len(parts) >= 3:
            name = parts[1].replace("GLFW_MOUSE_BUTTON_", "")  # Strip prefix
            value = parts[2]

            # Skip aliases
            if name in ("LEFT", "RIGHT", "MIDDLE", "LAST"):
                aliases[name] = value.replace("GLFW_MOUSE_BUTTON_", "")  # store for later
                continue

            if name != "UNKNOWN" and name != "LAST":
                if name.isdigit():
                    name = "B" + name  # B0, B1, etc.
                outString += f"        {name} = {value},\n"
                mouseButtonNum += 1
                try:
                    ivalue = int(value)
                    MaxGLFWMouse = max(ivalue, MaxGLFWMouse)
                except ValueError:
                    pass

# Second pass: add aliases without incrementing counts or max
for alias, target in aliases.items():
    # convert numeric targets if needed
    if target.isdigit():
        target = "B" + target
    outString += f"        {alias} = {target},\n"


outString += "    };\n\n"
outString += f"    inline constexpr size_t KeysNum = {keysNum};\n"
outString += f"    inline constexpr size_t MaxGLFWKey = {MaxGLFWKey};\n\n"
outString += f"    inline constexpr size_t MouseButtonNum = {mouseButtonNum};\n"
outString += f"    inline constexpr size_t MaxGLFWMouse = {MaxGLFWMouse};\n"
outString += "\n}"

with open(outPath, "w", encoding="utf-8") as f:
    f.write(outString)

print(f"Generated enum class at {outPath}")
