import os
import re
import sys

import FreeCAD
import Mesh


ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
SRC_DIR = os.path.join(ROOT, "cad", "converted_viewer_meshes")
OUT_PATH = os.path.join(SRC_DIR, "RobotArm_Clean_Mesh_Assembly.FCStd")
REPORT_PATH = os.path.join(SRC_DIR, "clean_assembly_report.txt")


SKIP_PATTERNS = (
    "ANSI_",
    "Forged_",
)


def should_skip(filename):
    return any(pattern in filename for pattern in SKIP_PATTERNS)


def label_from_filename(filename):
    label = os.path.splitext(filename)[0]
    label = re.sub(r"[^A-Za-z0-9_]+", "_", label)
    return label[:80]


def main():
    os.makedirs(SRC_DIR, exist_ok=True)
    obj_files = sorted(
        item
        for item in os.listdir(SRC_DIR)
        if item.lower().endswith(".obj")
        and not item.startswith("RobotArm_Full_Combined")
        and not should_skip(item)
    )

    doc = FreeCAD.newDocument("RobotArm_Clean_Mesh_Assembly")
    imported = []

    for filename in obj_files:
        path = os.path.join(SRC_DIR, filename)
        before = set(obj.Name for obj in doc.Objects)
        Mesh.insert(path, doc.Name)
        added = [obj for obj in doc.Objects if obj.Name not in before]
        for obj in added:
            obj.Label = label_from_filename(filename)
            obj.Visibility = True
        imported.append(filename)

    doc.recompute()
    doc.saveAs(OUT_PATH)

    lines = [
        "Clean FreeCAD mesh assembly created",
        f"Output: {OUT_PATH}",
        "Skipped fasteners and duplicate hardware labels matching: ANSI_, Forged_",
        f"Imported OBJ files: {len(imported)}",
        "",
        *imported,
    ]
    with open(REPORT_PATH, "w", encoding="utf-8") as handle:
        handle.write("\n".join(lines))
        handle.write("\n")

    print("\n".join(lines[:6]))
    print(f"... {len(imported)} files total")
    return 0


if __name__ == "__main__":
    sys.exit(main())
