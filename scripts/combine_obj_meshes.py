import os
import re
import sys


ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
SRC_DIR = os.path.join(ROOT, "cad", "converted_viewer_meshes")
OUT_PATH = os.path.join(SRC_DIR, "RobotArm_Full_Combined.obj")


def object_name(filename):
    name = os.path.splitext(filename)[0]
    name = re.sub(r"[^A-Za-z0-9_]+", "_", name)
    return name


def main():
    obj_files = sorted(
        item
        for item in os.listdir(SRC_DIR)
        if item.lower().endswith(".obj") and not item.startswith("RobotArm_Full_Combined")
    )

    vertex_offset = 0
    output = [
        "# Combined robot arm mesh exported from Autodesk Viewer parts",
        "# Units: millimeters",
    ]

    for filename in obj_files:
        output.append(f"o {object_name(filename)}")
        path = os.path.join(SRC_DIR, filename)
        local_vertices = 0
        with open(path, "r", encoding="utf-8") as handle:
            for raw in handle:
                line = raw.strip()
                if line.startswith("v "):
                    output.append(line)
                    local_vertices += 1
                elif line.startswith("f "):
                    indices = []
                    for token in line.split()[1:]:
                        base = token.split("/")[0]
                        indices.append(str(int(base) + vertex_offset))
                    output.append("f " + " ".join(indices))
        vertex_offset += local_vertices

    with open(OUT_PATH, "w", encoding="utf-8") as handle:
        handle.write("\n".join(output))
        handle.write("\n")

    print(f"Combined {len(obj_files)} OBJ files")
    print(OUT_PATH)
    return 0


if __name__ == "__main__":
    sys.exit(main())
