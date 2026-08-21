import os
import re
import sys

import FreeCAD
import Mesh


ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
SRC_DIR = os.path.join(ROOT, "cad", "converted_viewer_meshes")
OUT_PATH = os.path.join(SRC_DIR, "RobotArm_Mesh_Assembly.FCStd")
REPORT_PATH = os.path.join(SRC_DIR, "freecad_assembly_report.txt")


def clean_label(filename):
    label = os.path.splitext(filename)[0]
    label = re.sub(r"^\d+_", "", label)
    return label[:80]


def main():
    if not os.path.isdir(SRC_DIR):
        raise RuntimeError(f"Missing mesh folder: {SRC_DIR}")

    obj_files = sorted(
        item
        for item in os.listdir(SRC_DIR)
        if item.lower().endswith(".obj")
    )

    doc = FreeCAD.newDocument("RobotArm_Mesh_Assembly")
    imported = []

    for filename in obj_files:
        path = os.path.join(SRC_DIR, filename)
        before = set(obj.Name for obj in doc.Objects)
        Mesh.insert(path, doc.Name)
        after = [obj for obj in doc.Objects if obj.Name not in before]
        for obj in after:
            obj.Label = clean_label(filename)
            obj.Visibility = True
        imported.append(filename)

    doc.recompute()
    doc.saveAs(OUT_PATH)

    lines = [
        "FreeCAD mesh assembly created",
        f"Output: {OUT_PATH}",
        f"Imported OBJ files: {len(imported)}",
        "",
        *imported,
    ]
    with open(REPORT_PATH, "w", encoding="utf-8") as handle:
        handle.write("\n".join(lines))
        handle.write("\n")

    print("\n".join(lines[:5]))
    print(f"... {len(imported)} files total")
    return 0


if __name__ == "__main__":
    sys.exit(main())
