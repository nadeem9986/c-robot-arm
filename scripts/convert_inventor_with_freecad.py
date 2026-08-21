import os
import sys

import FreeCAD

try:
    import Import
except Exception as exc:
    Import = None
    print(f"ERROR: FreeCAD Import module unavailable: {exc}")


ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
SRC_DIR = os.path.join(ROOT, "cad", "models")
OUT_DIR = os.path.join(ROOT, "cad", "converted")
LOG_PATH = os.path.join(OUT_DIR, "conversion_report.txt")


def write_log(lines):
    os.makedirs(OUT_DIR, exist_ok=True)
    with open(LOG_PATH, "w", encoding="utf-8") as handle:
        handle.write("\n".join(lines))
        handle.write("\n")


def convert_file(path, lines):
    name = os.path.basename(path)
    stem = os.path.splitext(name)[0]
    out_path = os.path.join(OUT_DIR, f"{stem}.step")

    if Import is None:
        lines.append(f"FAIL {name}: FreeCAD Import module is unavailable")
        return False

    doc = FreeCAD.newDocument(stem[:32])
    try:
        Import.insert(path, doc.Name)
        doc.recompute()
        objects = list(doc.Objects)
        if not objects:
            lines.append(f"FAIL {name}: imported no objects")
            return False
        Import.export(objects, out_path)
        lines.append(f"OK   {name} -> {os.path.relpath(out_path, ROOT)}")
        return True
    except Exception as exc:
        lines.append(f"FAIL {name}: {exc}")
        return False
    finally:
        FreeCAD.closeDocument(doc.Name)


def main():
    os.makedirs(OUT_DIR, exist_ok=True)
    files = [
        os.path.join(SRC_DIR, item)
        for item in sorted(os.listdir(SRC_DIR))
        if item.lower().endswith((".ipt", ".iam"))
    ]

    lines = [
        "Inventor to STEP conversion attempt using FreeCAD",
        f"Source: {SRC_DIR}",
        f"Output: {OUT_DIR}",
        "",
    ]

    ok_count = 0
    for path in files:
        if convert_file(path, lines):
            ok_count += 1

    lines.extend(["", f"Converted {ok_count} of {len(files)} files."])
    write_log(lines)
    print("\n".join(lines))
    return 0 if ok_count == len(files) else 2


if __name__ == "__main__":
    sys.exit(main())
