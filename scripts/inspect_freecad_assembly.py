import os
import sys

import FreeCAD


ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
DOC_PATH = os.path.join(ROOT, "cad", "converted_viewer_meshes", "RobotArm_Mesh_Assembly.FCStd")


def main():
    doc = FreeCAD.openDocument(DOC_PATH)
    boxes = []
    for obj in doc.Objects:
        if hasattr(obj, "Mesh"):
            box = obj.Mesh.BoundBox
            boxes.append((obj.Label, box.XLength, box.YLength, box.ZLength, box.XMin, box.YMin, box.ZMin, box.XMax, box.YMax, box.ZMax))

    print(f"Objects: {len(boxes)}")
    if boxes:
        xmin = min(item[4] for item in boxes)
        ymin = min(item[5] for item in boxes)
        zmin = min(item[6] for item in boxes)
        xmax = max(item[7] for item in boxes)
        ymax = max(item[8] for item in boxes)
        zmax = max(item[9] for item in boxes)
        print(f"Global bbox: X {xmin:.3f}..{xmax:.3f} ({xmax-xmin:.3f} mm)")
        print(f"Global bbox: Y {ymin:.3f}..{ymax:.3f} ({ymax-ymin:.3f} mm)")
        print(f"Global bbox: Z {zmin:.3f}..{zmax:.3f} ({zmax-zmin:.3f} mm)")
        for row in boxes[:12]:
            print(f"{row[0]}: {row[1]:.3f} x {row[2]:.3f} x {row[3]:.3f} mm")
    FreeCAD.closeDocument(doc.Name)
    return 0


if __name__ == "__main__":
    sys.exit(main())
