import os
import re
import sys

import FreeCAD
import Mesh


ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
SRC_DIR = os.path.join(ROOT, "cad", "converted_viewer_meshes")
OUT_OBJ = os.path.join(SRC_DIR, "RobotArm_Single_Clean_Model.obj")
OUT_FCSTD = os.path.join(SRC_DIR, "RobotArm_Single_Clean_Model.FCStd")
REPORT = os.path.join(SRC_DIR, "single_clean_model_report.txt")

SKIP_PATTERNS = ("ANSI_", "Forged_")


def should_skip(filename):
    return any(pattern in filename for pattern in SKIP_PATTERNS)


def object_name(filename):
    name = os.path.splitext(filename)[0]
    return re.sub(r"[^A-Za-z0-9_]+", "_", name)


def write_combined_obj(obj_files):
    vertex_offset = 0
    lines = [
        "# Single clean robot arm mesh",
        "# Source: Autodesk Viewer extracted OBJ parts",
        "# Units: millimeters",
    ]

    for filename in obj_files:
        lines.append(f"o {object_name(filename)}")
        local_vertices = 0
        with open(os.path.join(SRC_DIR, filename), "r", encoding="utf-8") as handle:
            for raw in handle:
                line = raw.strip()
                if line.startswith("v "):
                    lines.append(line)
                    local_vertices += 1
                elif line.startswith("f "):
                    face = []
                    for token in line.split()[1:]:
                        face.append(str(int(token.split("/")[0]) + vertex_offset))
                    lines.append("f " + " ".join(face))
        vertex_offset += local_vertices

    with open(OUT_OBJ, "w", encoding="utf-8") as handle:
        handle.write("\n".join(lines))
        handle.write("\n")


def write_fcstd():
    doc = FreeCAD.newDocument("RobotArm_Single_Clean_Model")
    mesh = Mesh.Mesh(OUT_OBJ)
    obj = doc.addObject("Mesh::Feature", "RobotArm_Single_Clean_Model")
    obj.Mesh = mesh
    obj.Label = "RobotArm_Single_Clean_Model"
    obj.Visibility = True
    doc.recompute()
    doc.saveAs(OUT_FCSTD)
    FreeCAD.closeDocument(doc.Name)


def main():
    obj_files = sorted(
        item
        for item in os.listdir(SRC_DIR)
        if item.lower().endswith(".obj")
        and not item.startswith("RobotArm_")
        and not should_skip(item)
    )

    write_combined_obj(obj_files)
    write_fcstd()

    lines = [
        "Single clean robot model created",
        f"OBJ: {OUT_OBJ}",
        f"FreeCAD: {OUT_FCSTD}",
        f"Combined input parts: {len(obj_files)}",
        "Removed fastener/hardware files matching: ANSI_, Forged_",
    ]
    with open(REPORT, "w", encoding="utf-8") as handle:
        handle.write("\n".join(lines))
        handle.write("\n")
    print("\n".join(lines))
    return 0


if __name__ == "__main__":
    sys.exit(main())
