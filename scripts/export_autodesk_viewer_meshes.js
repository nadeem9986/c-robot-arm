const fs = require("fs");
const path = require("path");
const { chromium } = require("playwright");

const ROOT = path.resolve(__dirname, "..");
const OUT_DIR = path.join(ROOT, "cad", "converted_viewer_meshes");
const MODEL_URL = "https://autode.sk/4qnE031";

function sanitizeName(name) {
  return name
    .replace(/[<>:"/\\|?*]+/g, "_")
    .replace(/\s+/g, "_")
    .replace(/_+/g, "_")
    .replace(/^_|_$/g, "")
    .slice(0, 90);
}

async function main() {
  fs.mkdirSync(OUT_DIR, { recursive: true });

  const browser = await chromium.launch({ headless: false });
  const page = await browser.newPage({ viewport: { width: 1400, height: 900 } });

  await page.goto(MODEL_URL, { waitUntil: "domcontentloaded", timeout: 60000 });
  await page.waitForFunction(
    () =>
      window.NOP_VIEWER &&
      window.NOP_VIEWER.model &&
      window.NOP_VIEWER.model.isLoadDone &&
      window.NOP_VIEWER.model.isLoadDone(),
    null,
    { timeout: 60000 }
  );
  await page.waitForTimeout(5000);

  const result = await page.evaluate(() => {
    const viewer = window.NOP_VIEWER;
    const model = viewer.model;
    const tree = model.getData().instanceTree;
    const fragList = model.getFragmentList();
    const THREE = window.THREE;

    const parentById = {};
    function walk(id) {
      tree.enumNodeChildren(
        id,
        (child) => {
          parentById[child] = id;
          walk(child);
        },
        false
      );
    }
    walk(model.getRootId());

    function namedParent(dbid) {
      let current = dbid;
      while (current && current !== model.getRootId()) {
        const name = tree.getNodeName(current) || "";
        if (name && !/^Solid\d*$/i.test(name)) {
          return { id: current, name };
        }
        current = parentById[current];
      }
      return { id: dbid, name: tree.getNodeName(dbid) || `dbid_${dbid}` };
    }

    function transformPoint(matrix, x, y, z) {
      const e = matrix.elements;
      return [
        e[0] * x + e[4] * y + e[8] * z + e[12],
        e[1] * x + e[5] * y + e[9] * z + e[13],
        e[2] * x + e[6] * y + e[10] * z + e[14],
      ];
    }

    const parts = {};
    const fragCount = fragList.getCount();

    for (let fragId = 0; fragId < fragCount; fragId++) {
      const dbid = fragList.fragments.fragId2dbId[fragId];
      const part = namedParent(dbid);
      const key = `${part.id}_${part.name}`;

      if (!parts[key]) {
        parts[key] = {
          id: part.id,
          name: part.name,
          vertices: [],
          faces: [],
          fragmentCount: 0,
        };
      }

      const geometry = fragList.getGeometry(fragId);
      if (!geometry || !geometry.vb || !geometry.ib) continue;

      const matrix = new THREE.Matrix4();
      fragList.getWorldMatrix(fragId, matrix);

      const vb = geometry.vb;
      const ib = geometry.ib;
      const stride = geometry.vbstride || 3;
      const baseIndex = parts[key].vertices.length;

      for (let i = 0; i < vb.length; i += stride) {
        parts[key].vertices.push(transformPoint(matrix, vb[i], vb[i + 1], vb[i + 2]));
      }

      for (let i = 0; i + 2 < ib.length; i += 3) {
        parts[key].faces.push([baseIndex + ib[i] + 1, baseIndex + ib[i + 1] + 1, baseIndex + ib[i + 2] + 1]);
      }

      parts[key].fragmentCount += 1;
    }

    return {
      unit: model.getUnitString ? model.getUnitString() : "mm",
      parts: Object.values(parts).filter((part) => part.vertices.length > 0 && part.faces.length > 0),
    };
  });

  const report = [
    "Autodesk Viewer mesh export",
    `Source: ${MODEL_URL}`,
    `Units: ${result.unit}`,
    "Format: OBJ mesh, not parametric CAD solids",
    "",
  ];

  for (const part of result.parts) {
    const safe = sanitizeName(`${String(part.id).padStart(3, "0")}_${part.name}`);
    const objPath = path.join(OUT_DIR, `${safe}.obj`);
    const lines = [
      `# ${part.name}`,
      `# Autodesk dbId: ${part.id}`,
      `# Units: ${result.unit}`,
      `o ${safe}`,
    ];

    for (const vertex of part.vertices) {
      lines.push(`v ${vertex[0].toFixed(6)} ${vertex[1].toFixed(6)} ${vertex[2].toFixed(6)}`);
    }

    for (const face of part.faces) {
      lines.push(`f ${face[0]} ${face[1]} ${face[2]}`);
    }

    fs.writeFileSync(objPath, lines.join("\n"), "utf8");
    report.push(
      `OK ${path.basename(objPath)}: ${part.vertices.length} vertices, ${part.faces.length} faces, ${part.fragmentCount} fragments`
    );
  }

  report.push("");
  report.push(`Exported ${result.parts.length} OBJ mesh files.`);
  fs.writeFileSync(path.join(OUT_DIR, "mesh_export_report.txt"), report.join("\n"), "utf8");

  await browser.close();
  console.log(report.join("\n"));
}

main().catch((error) => {
  console.error(error);
  process.exit(1);
});
