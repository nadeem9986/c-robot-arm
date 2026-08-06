#include "WebServerController.h"

// HTML UI embedded directly in Flash memory for standalone web interface
const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 4-DOF Robot Arm Controller</title>
    <style>
        :root {
            --bg-color: #0f172a;
            --card-bg: #1e293b;
            --accent-color: #38bdf8;
            --accent-hover: #0284c7;
            --danger-color: #ef4444;
            --success-color: #10b981;
            --text-main: #f8fafc;
            --text-muted: #94a3b8;
            --border-color: #334155;
        }
        * { box-sizing: border-box; margin: 0; padding: 0; font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Helvetica, Arial, sans-serif; }
        body { background: var(--bg-color); color: var(--text-main); padding: 15px; display: flex; flex-direction: column; align-items: center; min-height: 100vh; }
        .header { text-align: center; margin-bottom: 20px; }
        .header h1 { font-size: 1.8rem; color: var(--accent-color); margin-bottom: 5px; }
        .header p { color: var(--text-muted); font-size: 0.9rem; }
        
        .container { display: grid; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); gap: 15px; width: 100%; max-width: 1000px; }
        .card { background: var(--card-bg); border: 1px solid var(--border-color); border-radius: 12px; padding: 18px; box-shadow: 0 4px 15px rgba(0,0,0,0.3); }
        .card h2 { font-size: 1.1rem; color: var(--accent-color); margin-bottom: 15px; border-bottom: 1px solid var(--border-color); padding-bottom: 8px; }
        
        .slider-group { margin-bottom: 16px; }
        .slider-label { display: flex; justify-content: space-between; font-weight: 600; margin-bottom: 6px; font-size: 0.9rem; }
        .slider-value { color: var(--accent-color); }
        input[type=range] { width: 100%; height: 10px; border-radius: 5px; background: #334155; outline: none; -webkit-appearance: none; }
        input[type=range]::-webkit-slider-thumb { -webkit-appearance: none; width: 24px; height: 24px; border-radius: 50%; background: var(--accent-color); cursor: pointer; border: 2px solid #ffffff; box-shadow: 0 2px 5px rgba(0,0,0,0.5); }
        
        .btn-grid { display: grid; grid-template-columns: repeat(2, 1fr); gap: 10px; margin-top: 15px; }
        .btn { padding: 12px 15px; border: none; border-radius: 8px; font-weight: 600; cursor: pointer; background: var(--accent-color); color: #000; transition: all 0.2s ease; text-align: center; font-size: 0.95rem; }
        .btn:active { transform: scale(0.98); }
        .btn-danger { background: var(--danger-color); color: #fff; }
        .btn-danger:hover { background: #dc2626; }
        .btn-success { background: var(--success-color); color: #fff; }
        .btn-success:hover { background: #059669; }
        .btn-full { grid-column: span 2; }
        
        .status-badge { display: inline-block; padding: 4px 10px; border-radius: 20px; font-size: 0.8rem; font-weight: 600; background: var(--success-color); color: white; }
        .canvas-container { display: flex; justify-content: center; align-items: center; background: #090d16; border-radius: 8px; padding: 10px; height: 200px; }
        canvas { max-width: 100%; max-height: 100%; }
        
        .xyz-inputs { display: grid; grid-template-columns: repeat(3, 1fr); gap: 10px; margin-bottom: 15px; }
        .input-box { display: flex; flex-direction: column; }
        .input-box label { font-size: 0.8rem; color: var(--text-muted); margin-bottom: 4px; }
        .input-box input { background: #0f172a; border: 1px solid var(--border-color); color: white; padding: 10px; border-radius: 6px; font-size: 0.95rem; text-align: center; }
    </style>
</head>
<body>

    <div class="header">
        <h1>🤖 ESP32 Cantilever Robot Arm</h1>
        <p>4-DOF Servo Control • Phone Web Interface</p>
    </div>

    <div class="container">
        <!-- JOINT CONTROL CARD -->
        <div class="card">
            <h2>🕹️ Direct Servo Slider Control</h2>
            
            <div class="slider-group">
                <div class="slider-label"><span>J1: Base Yaw</span> <span id="j1-val" class="slider-value">90°</span></div>
                <input type="range" id="j1" min="0" max="180" value="90" oninput="updateJoints()">
            </div>

            <div class="slider-group">
                <div class="slider-label"><span>J2: Shoulder Pitch</span> <span id="j2-val" class="slider-value">90°</span></div>
                <input type="range" id="j2" min="15" max="165" value="90" oninput="updateJoints()">
            </div>

            <div class="slider-group">
                <div class="slider-label"><span>J3: Elbow Pitch</span> <span id="j3-val" class="slider-value">90°</span></div>
                <input type="range" id="j3" min="10" max="170" value="90" oninput="updateJoints()">
            </div>

            <div class="slider-group">
                <div class="slider-label"><span>J4: Wrist / Gripper</span> <span id="j4-val" class="slider-value">60°</span></div>
                <input type="range" id="j4" min="0" max="120" value="60" oninput="updateJoints()">
            </div>

            <div class="btn-grid">
                <button class="btn" onclick="sendPreset('HOME')">🏠 Home</button>
                <button class="btn" onclick="sendPreset('REST')">😴 Rest</button>
            </div>
        </div>

        <!-- CARTESIAN IK CONTROL CARD -->
        <div class="card">
            <h2>📐 Inverse Kinematics (XYZ)</h2>
            <div class="xyz-inputs">
                <div class="input-box">
                    <label>X (mm)</label>
                    <input type="number" id="ik-x" value="120" step="5">
                </div>
                <div class="input-box">
                    <label>Y (mm)</label>
                    <input type="number" id="ik-y" value="0" step="5">
                </div>
                <div class="input-box">
                    <label>Z (mm)</label>
                    <input type="number" id="ik-z" value="100" step="5">
                </div>
            </div>
            <button class="btn btn-full" onclick="sendIK()">📍 Move to Coordinate</button>

            <h2 style="margin-top: 20px;">🎬 Teach & Repeat Memory</h2>
            <div class="btn-grid">
                <button class="btn btn-success" onclick="sendTeach('REC')">📸 Record Pose</button>
                <button class="btn" onclick="sendTeach('PLAY')">▶️ Play Once</button>
                <button class="btn" onclick="sendTeach('LOOP')">🔄 Loop Play</button>
                <button class="btn btn-danger" onclick="sendTeach('CLEAR')">🗑️ Clear Memory</button>
            </div>
        </div>

        <!-- 2D KINEMATICS VISUALIZER & SYSTEM STATUS -->
        <div class="card">
            <h2>📊 Robot Arm Visualizer & Safety</h2>
            <div class="canvas-container">
                <canvas id="armCanvas" width="280" height="180"></canvas>
            </div>
            
            <div style="margin-top: 15px; display: flex; justify-content: space-between; align-items: center;">
                <span>Status: <span class="status-badge" id="sys-status">READY</span></span>
                <button class="btn btn-danger" onclick="toggleEStop()" id="estop-btn">🚨 EMERGENCY STOP</button>
            </div>
        </div>
    </div>

    <script>
        let isEStopped = false;
        let lastSendTime = 0;

        function updateJoints() {
            let j1 = document.getElementById('j1').value;
            let j2 = document.getElementById('j2').value;
            let j3 = document.getElementById('j3').value;
            let j4 = document.getElementById('j4').value;

            document.getElementById('j1-val').innerText = j1 + '°';
            document.getElementById('j2-val').innerText = j2 + '°';
            document.getElementById('j3-val').innerText = j3 + '°';
            document.getElementById('j4-val').innerText = j4 + '°';

            drawArm(parseFloat(j2), parseFloat(j3), parseFloat(j4));

            let now = Date.now();
            if (now - lastSendTime > 50) {
                lastSendTime = now;
                fetch(`/api/move?j1=${j1}&j2=${j2}&j3=${j3}&j4=${j4}`);
            }
        }

        function sendPreset(pose) {
            fetch(`/api/cmd?c=${pose}`).then(() => fetchStatus());
        }

        function sendIK() {
            let x = document.getElementById('ik-x').value;
            let y = document.getElementById('ik-y').value;
            let z = document.getElementById('ik-z').value;
            fetch(`/api/ik?x=${x}&y=${y}&z=${z}`).then(res => res.json()).then(data => {
                if(data.success) fetchStatus();
                else alert("Target Position Unreachable!");
            });
        }

        function sendTeach(action) {
            fetch(`/api/teach?action=${action}`);
        }

        function toggleEStop() {
            let action = isEStopped ? 'RESET_ESTOP' : 'ESTOP';
            fetch(`/api/cmd?c=${action}`).then(() => {
                isEStopped = !isEStopped;
                document.getElementById('estop-btn').innerText = isEStopped ? '✅ RESET E-STOP' : '🚨 EMERGENCY STOP';
                document.getElementById('sys-status').innerText = isEStopped ? 'HALTED' : 'READY';
                document.getElementById('sys-status').style.background = isEStopped ? '#ef4444' : '#10b981';
            });
        }

        function fetchStatus() {
            fetch('/api/status').then(res => res.json()).then(data => {
                document.getElementById('j1').value = data.j1;
                document.getElementById('j2').value = data.j2;
                document.getElementById('j3').value = data.j3;
                document.getElementById('j4').value = data.j4;

                document.getElementById('j1-val').innerText = Math.round(data.j1) + '°';
                document.getElementById('j2-val').innerText = Math.round(data.j2) + '°';
                document.getElementById('j3-val').innerText = Math.round(data.j3) + '°';
                document.getElementById('j4-val').innerText = Math.round(data.j4) + '°';

                drawArm(data.j2, data.j3, data.j4);
            });
        }

        function drawArm(j2, j3, j4) {
            const canvas = document.getElementById('armCanvas');
            const ctx = canvas.getContext('2d');
            ctx.clearRect(0, 0, canvas.width, canvas.height);

            const originX = 140;
            const originY = 150;
            const scale = 0.55;

            const rad2 = (j2) * Math.PI / 180;
            const rad3 = (j2 + j3 - 90) * Math.PI / 180;
            const rad4 = (j2 + j3 + j4 - 90) * Math.PI / 180;

            const l0 = 30 * scale;
            const l1 = 120 * scale;
            const l2 = 110 * scale;
            const l3 = 65 * scale;

            // Base
            ctx.fillStyle = '#475569';
            ctx.fillRect(originX - 25, originY, 50, 12);
            ctx.fillStyle = '#64748b';
            ctx.fillRect(originX - 8, originY - l0, 16, l0);

            // Shoulder Joint
            const sX = originX;
            const sY = originY - l0;

            // Elbow Joint
            const eX = sX + l1 * Math.cos(rad2);
            const eY = sY - l1 * Math.sin(rad2);

            // Wrist Joint
            const wX = eX + l2 * Math.cos(rad3);
            const wY = eY - l2 * Math.sin(rad3);

            // Gripper Tip
            const gX = wX + l3 * Math.cos(rad4);
            const gY = wY - l3 * Math.sin(rad4);

            // Draw Links
            ctx.strokeStyle = '#38bdf8';
            ctx.lineWidth = 5;
            ctx.lineCap = 'round';

            // Shoulder Link (L1)
            ctx.beginPath(); ctx.moveTo(sX, sY); ctx.lineTo(eX, eY); ctx.stroke();
            // Forearm Link (L2)
            ctx.strokeStyle = '#818cf8';
            ctx.beginPath(); ctx.moveTo(eX, eY); ctx.lineTo(wX, wY); ctx.stroke();
            // Gripper Link (L3)
            ctx.strokeStyle = '#f43f5e';
            ctx.beginPath(); ctx.moveTo(wX, wY); ctx.lineTo(gX, gY); ctx.stroke();

            // Draw Joint Circles
            ctx.fillStyle = '#ffffff';
            [ {x:sX, y:sY}, {x:eX, y:eY}, {x:wX, y:wY} ].forEach(pt => {
                ctx.beginPath(); ctx.arc(pt.x, pt.y, 4, 0, 2*Math.PI); ctx.fill();
            });
        }

        window.onload = () => {
            fetchStatus();
        };
    </script>
</body>
</html>
)rawliteral";

WebServerController::WebServerController(RobotArm& robotArm, uint16_t port)
    : server(port), robot(robotArm), apMode(true) {}

void WebServerController::begin(bool useStationMode, const char* staSSID, const char* staPass, const char* apSSID, const char* apPass) {
    bool connected = false;

    if (useStationMode && staSSID != NULL && strlen(staSSID) > 0 && strcmp(staSSID, "YOUR_WIFI_NAME") != 0) {
        WiFi.mode(WIFI_STA);
        WiFi.begin(staSSID, staPass);
        Serial.printf("[WIFI] Connecting to Wi-Fi Network '%s'", staSSID);

        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 20) { // Wait up to 10 seconds
            delay(500);
            Serial.print(".");
            attempts++;
        }

        if (WiFi.status() == WL_CONNECTED) {
            connected = true;
            apMode = false;
            Serial.println();
            Serial.println("=================================================");
            Serial.printf("  ✅ WI-FI CONNECTED TO: %s\n", staSSID);
            Serial.printf("  👉 OPEN YOUR PHONE BROWSER TO: http://%s\n", WiFi.localIP().toString().c_str());
            Serial.println("=================================================");
        } else {
            Serial.println("\n⚠️ Failed to connect to Wi-Fi station network. Falling back to Access Point Mode...");
        }
    }

    if (!connected) {
        WiFi.mode(WIFI_AP);
        WiFi.softAP(apSSID, apPass);
        apMode = true;
        IPAddress apIP = WiFi.softAPIP();
        Serial.println();
        Serial.println("=================================================");
        Serial.printf("  📡 ACCESS POINT MODE STARTED: %s\n", apSSID);
        Serial.printf("  👉 CONNECT PHONE WI-FI TO: %s (Pass: %s)\n", apSSID, apPass);
        Serial.printf("  👉 OPEN PHONE BROWSER TO: http://%s\n", apIP.toString().c_str());
        Serial.println("=================================================");
    }

    setupRoutes();
    server.begin();
    Serial.println("[WEB] HTTP Server listening on port 80");
}

void WebServerController::setupRoutes() {
    server.on("/", HTTP_GET, std::bind(&WebServerController::handleRoot, this));
    server.on("/api/status", HTTP_GET, std::bind(&WebServerController::handleApiStatus, this));
    server.on("/api/move", HTTP_GET, std::bind(&WebServerController::handleApiMoveJoints, this));
    server.on("/api/ik", HTTP_GET, std::bind(&WebServerController::handleApiMoveIK, this));
    server.on("/api/teach", HTTP_GET, std::bind(&WebServerController::handleApiTeach, this));
    server.on("/api/cmd", HTTP_GET, [this]() {
        if (server.hasArg("c")) {
            String res = robot.executeCommand(server.arg("c"));
            server.send(200, "text/plain", res);
        } else {
            server.send(400, "text/plain", "Missing c parameter");
        }
    });
}

void WebServerController::handleRoot() {
    server.send_P(200, "text/html", INDEX_HTML);
}

void WebServerController::handleApiStatus() {
    JointAngles j = robot.getCurrentJoints();
    Vector3D xyz = robot.getCurrentCartesian();

    JsonDocument doc;
    doc["j1"] = j.j1;
    doc["j2"] = j.j2;
    doc["j3"] = j.j3;
    doc["j4"] = j.j4;
    doc["x"] = xyz.x;
    doc["y"] = xyz.y;
    doc["z"] = xyz.z;
    doc["isMoving"] = robot.getServoController().isMoving();
    doc["isEStopped"] = robot.getServoController().isEStopped();
    doc["teachCount"] = robot.getTeachCount();

    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
}

void WebServerController::handleApiMoveJoints() {
    if (server.hasArg("j1") && server.hasArg("j2") && server.hasArg("j3") && server.hasArg("j4")) {
        float j1 = server.arg("j1").toFloat();
        float j2 = server.arg("j2").toFloat();
        float j3 = server.arg("j3").toFloat();
        float j4 = server.arg("j4").toFloat();
        robot.moveJoints(j1, j2, j3, j4);
        server.send(200, "application/json", "{\"success\":true}");
    } else {
        server.send(400, "application/json", "{\"success\":false,\"error\":\"Missing parameters\"}");
    }
}

void WebServerController::handleApiMoveIK() {
    if (server.hasArg("x") && server.hasArg("y") && server.hasArg("z")) {
        float x = server.arg("x").toFloat();
        float y = server.arg("y").toFloat();
        float z = server.arg("z").toFloat();
        float pitch = server.hasArg("pitch") ? server.arg("pitch").toFloat() : 0.0f;
        
        bool ok = robot.moveCartesian(x, y, z, pitch);
        if (ok) {
            server.send(200, "application/json", "{\"success\":true}");
        } else {
            server.send(422, "application/json", "{\"success\":false,\"error\":\"Unreachable coordinate\"}");
        }
    } else {
        server.send(400, "application/json", "{\"success\":false,\"error\":\"Missing XYZ parameters\"}");
    }
}

void WebServerController::handleApiTeach() {
    if (server.hasArg("action")) {
        String action = server.arg("action");
        String res = robot.executeCommand(action);
        server.send(200, "application/json", "{\"success\":true,\"result\":\"" + res + "\"}");
    } else {
        server.send(400, "application/json", "{\"success\":false}");
    }
}

void WebServerController::handleClient() {
    server.handleClient();
}
