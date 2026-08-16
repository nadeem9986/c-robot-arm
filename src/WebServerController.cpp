#include "WebServerController.h"

// Professional Industrial Glassmorphism Dashboard UI with 3D Canvas Visualizer
const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
    <title>MeArm 4-DOF Robot Console</title>
    <style>
        :root {
            --bg-dark: #090d16;
            --panel-bg: rgba(18, 26, 43, 0.85);
            --panel-border: rgba(56, 189, 248, 0.15);
            --accent-cyan: #38bdf8;
            --accent-glow: rgba(56, 189, 248, 0.3);
            --accent-indigo: #818cf8;
            --accent-rose: #fb7185;
            --accent-emerald: #34d399;
            --accent-amber: #fbbf24;
            --text-primary: #f8fafc;
            --text-secondary: #94a3b8;
            --text-muted: #64748b;
            --input-bg: #0f172a;
        }

        * { box-sizing: border-box; margin: 0; padding: 0; font-family: -apple-system, BlinkMacSystemFont, 'SF Pro Display', 'Segoe UI', Roboto, Helvetica, Arial, sans-serif; -webkit-tap-highlight-color: transparent; }

        body {
            background-color: var(--bg-dark);
            background-image: 
                radial-gradient(at 0% 0%, rgba(56, 189, 248, 0.08) 0px, transparent 50%),
                radial-gradient(at 100% 100%, rgba(129, 140, 248, 0.08) 0px, transparent 50%);
            color: var(--text-primary);
            min-height: 100vh;
            padding: 16px;
            display: flex;
            flex-direction: column;
            align-items: center;
        }

        /* TOP NAVIGATION TELEMETRY BAR */
        .top-bar {
            width: 100%;
            max-width: 1200px;
            display: flex;
            justify-content: space-between;
            align-items: center;
            background: var(--panel-bg);
            border: 1px solid var(--panel-border);
            backdrop-filter: blur(12px);
            border-radius: 12px;
            padding: 12px 20px;
            margin-bottom: 20px;
            box-shadow: 0 8px 32px rgba(0, 0, 0, 0.4);
        }

        .brand-title {
            display: flex;
            align-items: center;
            gap: 12px;
        }

        .brand-title h1 {
            font-size: 1.15rem;
            font-weight: 700;
            letter-spacing: 0.5px;
            background: linear-gradient(135deg, #38bdf8, #818cf8);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            text-transform: uppercase;
        }

        .brand-badge {
            background: rgba(56, 189, 248, 0.12);
            color: var(--accent-cyan);
            border: 1px solid rgba(56, 189, 248, 0.3);
            font-size: 0.7rem;
            padding: 2px 8px;
            border-radius: 4px;
            font-weight: 600;
            letter-spacing: 1px;
        }

        .telemetry-group {
            display: flex;
            align-items: center;
            gap: 16px;
            font-size: 0.8rem;
            color: var(--text-secondary);
        }

        .status-dot {
            width: 8px;
            height: 8px;
            border-radius: 50%;
            background: var(--accent-emerald);
            box-shadow: 0 0 10px var(--accent-emerald);
            animation: pulse-dot 2s infinite;
        }

        @keyframes pulse-dot {
            0%, 100% { transform: scale(1); opacity: 1; }
            50% { transform: scale(1.3); opacity: 0.6; }
        }

        /* MAIN DASHBOARD GRID */
        .dashboard-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(350px, 1fr));
            gap: 20px;
            width: 100%;
            max-width: 1200px;
        }

        .panel {
            background: var(--panel-bg);
            border: 1px solid var(--panel-border);
            backdrop-filter: blur(12px);
            border-radius: 16px;
            padding: 20px;
            box-shadow: 0 8px 32px rgba(0, 0, 0, 0.4);
            display: flex;
            flex-direction: column;
        }

        .panel-header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 18px;
            border-bottom: 1px solid rgba(255, 255, 255, 0.06);
            padding-bottom: 10px;
        }

        .panel-header h2 {
            font-size: 0.95rem;
            font-weight: 600;
            color: var(--text-primary);
            text-transform: uppercase;
            letter-spacing: 1px;
            display: flex;
            align-items: center;
            gap: 8px;
        }

        .panel-header h2::before {
            content: '';
            display: inline-block;
            width: 4px;
            height: 14px;
            background: var(--accent-cyan);
            border-radius: 2px;
        }

        /* SLIDER CONTROLS */
        .control-item {
            margin-bottom: 16px;
            background: rgba(15, 23, 42, 0.6);
            border: 1px solid rgba(255, 255, 255, 0.04);
            border-radius: 10px;
            padding: 12px 14px;
        }

        .control-label {
            display: flex;
            justify-content: space-between;
            align-items: center;
            font-size: 0.85rem;
            font-weight: 500;
            color: var(--text-secondary);
            margin-bottom: 8px;
        }

        .limit-subtext {
            font-size: 0.72rem;
            color: var(--text-muted);
            margin-left: 6px;
        }

        .value-tag {
            font-family: 'JetBrains Mono', monospace, monospace;
            font-weight: 700;
            color: var(--accent-cyan);
            font-size: 0.9rem;
            background: rgba(56, 189, 248, 0.1);
            padding: 2px 8px;
            border-radius: 4px;
            border: 1px solid rgba(56, 189, 248, 0.2);
        }

        .range-wrapper {
            display: flex;
            align-items: center;
            gap: 10px;
        }

        .step-btn {
            background: #1e293b;
            border: 1px solid rgba(255,255,255,0.08);
            color: var(--text-primary);
            width: 28px;
            height: 28px;
            border-radius: 6px;
            font-size: 1rem;
            font-weight: bold;
            cursor: pointer;
            display: flex;
            align-items: center;
            justify-content: center;
            transition: all 0.15s ease;
        }

        .step-btn:hover { background: #334155; color: var(--accent-cyan); }
        .step-btn:active { transform: scale(0.92); }

        input[type=range] {
            flex: 1;
            height: 6px;
            border-radius: 3px;
            background: #1e293b;
            outline: none;
            -webkit-appearance: none;
        }

        input[type=range]::-webkit-slider-thumb {
            -webkit-appearance: none;
            width: 18px;
            height: 18px;
            border-radius: 50%;
            background: var(--accent-cyan);
            cursor: pointer;
            border: 2px solid #ffffff;
            box-shadow: 0 0 10px var(--accent-cyan);
            transition: transform 0.1s ease;
        }

        input[type=range]::-webkit-slider-thumb:hover {
            transform: scale(1.15);
        }

        /* BUTTONS GRID */
        .btn-grid-4 {
            display: grid;
            grid-template-columns: repeat(2, 1fr);
            gap: 10px;
            margin-top: 10px;
        }

        .btn-modern {
            padding: 10px 14px;
            border: 1px solid rgba(255, 255, 255, 0.08);
            border-radius: 8px;
            background: rgba(30, 41, 59, 0.8);
            color: var(--text-primary);
            font-size: 0.82rem;
            font-weight: 600;
            letter-spacing: 0.5px;
            cursor: pointer;
            transition: all 0.2s ease;
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 6px;
        }

        .btn-modern:hover {
            background: rgba(56, 189, 248, 0.15);
            border-color: rgba(56, 189, 248, 0.4);
            color: var(--accent-cyan);
            transform: translateY(-1px);
        }

        .btn-modern:active { transform: translateY(0); }

        .btn-primary {
            background: linear-gradient(135deg, #0284c7, #0369a1);
            border-color: #38bdf8;
            color: #ffffff;
            box-shadow: 0 4px 14px rgba(2, 132, 199, 0.35);
        }
        .btn-primary:hover {
            background: linear-gradient(135deg, #38bdf8, #0284c7);
            color: #ffffff;
        }

        .btn-emerald {
            background: rgba(52, 211, 153, 0.12);
            border-color: rgba(52, 211, 153, 0.3);
            color: var(--accent-emerald);
        }
        .btn-emerald:hover {
            background: rgba(52, 211, 153, 0.25);
            border-color: var(--accent-emerald);
        }

        .btn-rose {
            background: rgba(251, 113, 133, 0.12);
            border-color: rgba(251, 113, 133, 0.3);
            color: var(--accent-rose);
        }
        .btn-rose:hover {
            background: rgba(251, 113, 133, 0.25);
            border-color: var(--accent-rose);
        }

        /* CARTESIAN IK INPUTS */
        .ik-grid {
            display: grid;
            grid-template-columns: repeat(3, 1fr);
            gap: 8px;
            margin-bottom: 12px;
        }

        .ik-box {
            display: flex;
            flex-direction: column;
            gap: 4px;
        }

        .ik-box label {
            font-size: 0.72rem;
            color: var(--text-muted);
            font-weight: 600;
            text-transform: uppercase;
        }

        .ik-box input {
            width: 100%;
            background: var(--input-bg);
            border: 1px solid rgba(255, 255, 255, 0.08);
            border-radius: 6px;
            color: var(--accent-cyan);
            font-family: 'JetBrains Mono', monospace, monospace;
            font-size: 0.9rem;
            font-weight: 600;
            padding: 8px;
            text-align: center;
            outline: none;
            transition: border-color 0.2s;
        }

        .ik-box input:focus {
            border-color: var(--accent-cyan);
            box-shadow: 0 0 8px var(--accent-glow);
        }

        /* SERVO LIMIT CONFIGURATION TABLE */
        .limits-table {
            width: 100%;
            border-collapse: collapse;
            margin-bottom: 12px;
            font-size: 0.8rem;
        }

        .limits-table th {
            text-align: center;
            color: var(--text-muted);
            font-weight: 600;
            padding: 6px;
            border-bottom: 1px solid rgba(255, 255, 255, 0.06);
            text-transform: uppercase;
            font-size: 0.7rem;
        }

        .limits-table td {
            padding: 6px 4px;
            text-align: center;
        }

        .limits-table input {
            width: 60px;
            background: var(--input-bg);
            border: 1px solid rgba(255, 255, 255, 0.08);
            border-radius: 4px;
            color: var(--accent-cyan);
            font-family: 'JetBrains Mono', monospace, monospace;
            font-size: 0.82rem;
            padding: 4px;
            text-align: center;
            outline: none;
        }

        .limits-table input:focus {
            border-color: var(--accent-cyan);
        }

        /* 3D CANVAS VISUALIZER CARD */
        .canvas-card {
            width: 100%;
            height: 240px;
            background: #060911;
            border: 1px solid rgba(56, 189, 248, 0.2);
            border-radius: 12px;
            position: relative;
            overflow: hidden;
            margin-bottom: 14px;
        }

        canvas#armCanvas3D {
            width: 100%;
            height: 100%;
            display: block;
            cursor: grab;
        }

        canvas#armCanvas3D:active {
            cursor: grabbing;
        }

        .canvas-overlay {
            position: absolute;
            top: 8px;
            left: 8px;
            font-size: 0.72rem;
            font-family: 'JetBrains Mono', monospace;
            color: var(--accent-cyan);
            background: rgba(15, 23, 42, 0.85);
            padding: 4px 8px;
            border-radius: 4px;
            border: 1px solid rgba(56, 189, 248, 0.2);
            pointer-events: none;
        }

        .canvas-hint {
            position: absolute;
            bottom: 6px;
            right: 8px;
            font-size: 0.65rem;
            color: var(--text-muted);
            pointer-events: none;
        }

        /* E-STOP BUTTON */
        .estop-container {
            margin-top: 15px;
            width: 100%;
        }

        .btn-estop {
            width: 100%;
            padding: 14px;
            border-radius: 10px;
            background: linear-gradient(135deg, #dc2626, #991b1b);
            border: 1px solid #ef4444;
            color: #ffffff;
            font-size: 0.95rem;
            font-weight: 800;
            letter-spacing: 1.5px;
            text-transform: uppercase;
            cursor: pointer;
            box-shadow: 0 4px 20px rgba(220, 38, 38, 0.4);
            transition: all 0.2s ease;
        }

        .btn-estop:hover {
            background: linear-gradient(135deg, #ef4444, #dc2626);
            box-shadow: 0 6px 25px rgba(239, 68, 68, 0.6);
        }

        .btn-estop.active {
            background: linear-gradient(135deg, #10b981, #059669);
            border-color: #34d399;
            box-shadow: 0 4px 20px rgba(16, 185, 129, 0.4);
        }

        /* RESPONSIVE FINE TUNING */
        @media (max-width: 480px) {
            body { padding: 8px; }
            .top-bar { flex-direction: column; gap: 8px; text-align: center; }
            .dashboard-grid { grid-template-columns: 1fr; }
        }
    </style>
</head>
<body>

    <!-- TOP NAVIGATION TELEMETRY BAR -->
    <div class="top-bar">
        <div class="brand-title">
            <div class="status-dot" id="status-dot"></div>
            <h1>MeArm 4-DOF PARALLEL CONSOLE</h1>
            <span class="brand-badge">INDUSTRIAL CONTROL</span>
        </div>
        <div class="telemetry-group">
            <span>IP: <strong id="net-ip" style="color:var(--accent-cyan);">192.168.31.174</strong></span>
            <span>|</span>
            <span>MODE: <strong id="net-mode" style="color:var(--accent-emerald);">STATION (AMANAT)</strong></span>
        </div>
    </div>

    <!-- MAIN CONTROL DASHBOARD GRID -->
    <div class="dashboard-grid">

        <!-- PANEL 1: DIRECT JOINT CONTROL -->
        <div class="panel">
            <div class="panel-header">
                <h2>Joint Kinematics</h2>
                <span class="value-tag" id="motion-status">READY</span>
            </div>

            <!-- JOINT 1 -->
            <div class="control-item">
                <div class="control-label">
                    <span>J1: BASE YAW <span class="limit-subtext" id="j1-limit-tag">(0° - 180°)</span></span>
                    <span class="value-tag" id="j1-val">90°</span>
                </div>
                <div class="range-wrapper">
                    <button class="step-btn" onclick="stepJoint('j1', -2)">-</button>
                    <input type="range" id="j1" min="0" max="180" value="90" oninput="onSliderChange()">
                    <button class="step-btn" onclick="stepJoint('j1', 2)">+</button>
                </div>
            </div>

            <!-- JOINT 2 -->
            <div class="control-item">
                <div class="control-label">
                    <span>J2: SHOULDER BOOM <span class="limit-subtext" id="j2-limit-tag">(15° - 165°)</span></span>
                    <span class="value-tag" id="j2-val">90°</span>
                </div>
                <div class="range-wrapper">
                    <button class="step-btn" onclick="stepJoint('j2', -2)">-</button>
                    <input type="range" id="j2" min="15" max="165" value="90" oninput="onSliderChange()">
                    <button class="step-btn" onclick="stepJoint('j2', 2)">+</button>
                </div>
            </div>

            <!-- JOINT 3 -->
            <div class="control-item">
                <div class="control-label">
                    <span>J3: FOREARM PARALLEL BAR <span class="limit-subtext" id="j3-limit-tag">(10° - 170°)</span></span>
                    <span class="value-tag" id="j3-val">90°</span>
                </div>
                <div class="range-wrapper">
                    <button class="step-btn" onclick="stepJoint('j3', -2)">-</button>
                    <input type="range" id="j3" min="10" max="170" value="90" oninput="onSliderChange()">
                    <button class="step-btn" onclick="stepJoint('j3', 2)">+</button>
                </div>
            </div>

            <!-- JOINT 4 -->
            <div class="control-item">
                <div class="control-label">
                    <span>J4: GRIPPER CLAW <span class="limit-subtext" id="j4-limit-tag">(0° - 17°)</span></span>
                    <span class="value-tag" id="j4-val">10°</span>
                </div>
                <div class="range-wrapper">
                    <button class="step-btn" onclick="stepJoint('j4', -2)">-</button>
                    <input type="range" id="j4" min="0" max="17" value="10" oninput="onSliderChange()">
                    <button class="step-btn" onclick="stepJoint('j4', 2)">+</button>
                </div>
            </div>

            <!-- PRESET POSITIONS -->
            <div class="btn-grid-4">
                <button class="btn-modern" onclick="sendPreset('HOME')">🏠 HOME</button>
                <button class="btn-modern" onclick="sendPreset('REST')">😴 REST</button>
            </div>
        </div>

        <!-- PANEL 2: DYNAMIC SERVO LIMITS & CALIBRATION -->
        <div class="panel">
            <div class="panel-header">
                <h2>⚙️ Servo Degree Limits</h2>
                <span class="value-tag">SAFETY CONSTRAINTS</span>
            </div>

            <table class="limits-table">
                <thead>
                    <tr>
                        <th>JOINT</th>
                        <th>MIN (°)</th>
                        <th>MAX (°)</th>
                        <th>TRIM OFFSET</th>
                    </tr>
                </thead>
                <tbody>
                    <tr>
                        <td style="color:var(--accent-cyan); font-weight:600;">J1 Base</td>
                        <td><input type="number" id="l-j1-min" value="0" min="0" max="180"></td>
                        <td><input type="number" id="l-j1-max" value="180" min="0" max="180"></td>
                        <td><input type="number" id="l-j1-off" value="0" min="-30" max="30"></td>
                    </tr>
                    <tr>
                        <td style="color:var(--accent-indigo); font-weight:600;">J2 Shoulder</td>
                        <td><input type="number" id="l-j2-min" value="15" min="0" max="180"></td>
                        <td><input type="number" id="l-j2-max" value="165" min="0" max="180"></td>
                        <td><input type="number" id="l-j2-off" value="0" min="-30" max="30"></td>
                    </tr>
                    <tr>
                        <td style="color:var(--accent-emerald); font-weight:600;">J3 Elbow Bar</td>
                        <td><input type="number" id="l-j3-min" value="10" min="0" max="180"></td>
                        <td><input type="number" id="l-j3-max" value="170" min="0" max="180"></td>
                        <td><input type="number" id="l-j3-off" value="0" min="-30" max="30"></td>
                    </tr>
                    <tr>
                        <td style="color:var(--accent-rose); font-weight:600;">J4 Gripper</td>
                        <td><input type="number" id="l-j4-min" value="0" min="0" max="17"></td>
                        <td><input type="number" id="l-j4-max" value="17" min="0" max="17"></td>
                        <td><input type="number" id="l-j4-off" value="0" min="-30" max="30"></td>
                    </tr>
                </tbody>
            </table>

            <div class="btn-grid-4" style="margin-bottom: 20px;">
                <button class="btn-modern btn-primary" onclick="saveServoLimits()">💾 SAVE LIMITS</button>
                <button class="btn-modern" onclick="resetServoLimits()">🔄 RESET DEFAULTS</button>
            </div>

            <div class="panel-header">
                <h2>Cartesian Solver (XYZ)</h2>
                <span class="value-tag">MEARM IK</span>
            </div>

            <div class="ik-grid">
                <div class="ik-box">
                    <label>X (mm)</label>
                    <input type="number" id="ik-x" value="120">
                </div>
                <div class="ik-box">
                    <label>Y (mm)</label>
                    <input type="number" id="ik-y" value="0">
                </div>
                <div class="ik-box">
                    <label>Z (mm)</label>
                    <input type="number" id="ik-z" value="100">
                </div>
            </div>
            <button class="btn-modern btn-primary" onclick="sendIK()">📍 EXECUTE CARTESIAN MOVE</button>
        </div>

        <!-- PANEL 3: 3D ROBOT MODEL & TEACH REPEAT ENGINE -->
        <div class="panel">
            <div class="panel-header">
                <h2>Real-Time 3D Robot Model</h2>
                <span class="value-tag" style="color:var(--accent-emerald)">3D CANVAS</span>
            </div>

            <!-- 3D VISUALIZER CANVAS -->
            <div class="canvas-card">
                <div class="canvas-overlay" id="coords-overlay">X: 120.0 | Y: 0.0 | Z: 100.0</div>
                <div class="canvas-hint">🖱️ Drag to rotate 3D view • Scroll to zoom</div>
                <canvas id="armCanvas3D"></canvas>
            </div>

            <div class="panel-header" style="margin-top: 10px;">
                <h2>Teach & Repeat Engine</h2>
                <span class="value-tag" id="teach-count">0 POSES</span>
            </div>

            <div class="btn-grid-4">
                <button class="btn-modern btn-emerald" onclick="sendTeach('REC')">📸 RECORD POSE</button>
                <button class="btn-modern" onclick="sendTeach('PLAY')">▶️ PLAY ONCE</button>
                <button class="btn-modern" onclick="sendTeach('LOOP')">🔄 LOOP PLAY</button>
                <button class="btn-modern btn-rose" onclick="sendTeach('CLEAR')">🗑️ CLEAR MEMORY</button>
            </div>

            <div class="estop-container">
                <button class="btn-estop" id="estop-btn" onclick="toggleEStop()">🚨 EMERGENCY STOP</button>
            </div>
        </div>

    </div>

    <script>
        let isEStopped = false;
        let lastSendTime = 0;

        // -------------------------------------------------------------
        // HIGH-PERFORMANCE 3D MEARM CANVAS VISUALIZER (PARALLEL LINKAGE)
        // -------------------------------------------------------------
        let camYaw = 0.6;
        let camPitch = 0.4;
        let camZoom = 1.0;
        let isDragging = false;
        let lastMouseX = 0, lastMouseY = 0;

        let canvas3D, ctx3D;

        function init3DCanvas() {
            canvas3D = document.getElementById('armCanvas3D');
            if (!canvas3D) return;
            ctx3D = canvas3D.getContext('2d');

            function resizeCanvas() {
                canvas3D.width = canvas3D.parentElement.clientWidth;
                canvas3D.height = canvas3D.parentElement.clientHeight;
                draw3DModel();
            }

            window.addEventListener('resize', resizeCanvas);
            resizeCanvas();

            canvas3D.onmousedown = (e) => { isDragging = true; lastMouseX = e.clientX; lastMouseY = e.clientY; };
            window.onmouseup = () => { isDragging = false; };
            window.onmousemove = (e) => {
                if (!isDragging) return;
                camYaw += (e.clientX - lastMouseX) * 0.01;
                camPitch = Math.max(0.1, Math.min(1.2, camPitch + (e.clientY - lastMouseY) * 0.01));
                lastMouseX = e.clientX; lastMouseY = e.clientY;
                draw3DModel();
            };
            canvas3D.onwheel = (e) => {
                e.preventDefault();
                camZoom = Math.max(0.6, Math.min(2.0, camZoom - e.deltaY * 0.001));
                draw3DModel();
            };
        }

        function project3D(x, y, z) {
            if (!canvas3D) return { x: 0, y: 0, z: 0, scale: 1 };
            const w = canvas3D.width;
            const h = canvas3D.height;

            const cosY = Math.cos(camYaw), sinY = Math.sin(camYaw);
            const x1 = x * cosY - z * sinY;
            const z1 = x * sinY + z * cosY;

            const cosP = Math.cos(camPitch), sinP = Math.sin(camPitch);
            const y2 = y * cosP - z1 * sinP;
            const z2 = y * sinP + z1 * cosP;

            const dist = 300;
            const scale = (dist / (dist + z2)) * 1.1 * camZoom;

            const screenX = w / 2 + x1 * scale;
            const screenY = h / 2 + 40 - y2 * scale;
            return { x: screenX, y: screenY, z: z2, scale: scale };
        }

        function drawLine3D(p1, p2, color, width=3, dash=[]) {
            if (!ctx3D) return;
            const sp1 = project3D(p1.x, p1.y, p1.z);
            const sp2 = project3D(p2.x, p2.y, p2.z);
            ctx3D.strokeStyle = color;
            ctx3D.lineWidth = width * ((sp1.scale + sp2.scale) / 2);
            ctx3D.setLineDash(dash);
            ctx3D.beginPath();
            ctx3D.moveTo(sp1.x, sp1.y);
            ctx3D.lineTo(sp2.x, sp2.y);
            ctx3D.stroke();
            ctx3D.setLineDash([]);
        }

        function drawJoint3D(p, color='#38bdf8', radius=5) {
            if (!ctx3D) return;
            const sp = project3D(p.x, p.y, p.z);
            ctx3D.fillStyle = color;
            ctx3D.beginPath();
            ctx3D.arc(sp.x, sp.y, radius * sp.scale, 0, Math.PI * 2);
            ctx3D.fill();
            ctx3D.strokeStyle = '#ffffff';
            ctx3D.lineWidth = 1;
            ctx3D.stroke();
        }

        function draw3DModel() {
            if (!canvas3D || !ctx3D) return;
            const w = canvas3D.width;
            const h = canvas3D.height;
            ctx3D.clearRect(0, 0, w, h);

            // Ground Grid
            const gridSize = 140;
            const step = 20;
            for (let i = -gridSize; i <= gridSize; i += step) {
                drawLine3D({x: i, y: 0, z: -gridSize}, {x: i, y: 0, z: gridSize}, 'rgba(56, 189, 248, 0.08)', 1);
                drawLine3D({x: -gridSize, y: 0, z: i}, {x: gridSize, y: 0, z: i}, 'rgba(56, 189, 248, 0.08)', 1);
            }

            let j1 = parseFloat(document.getElementById('j1').value || 90);
            let j2 = parseFloat(document.getElementById('j2').value || 90);
            let j3 = parseFloat(document.getElementById('j3').value || 90);
            let j4 = parseFloat(document.getElementById('j4').value || 10);

            const rad1 = (j1 - 90) * Math.PI / 180;
            const rad2 = j2 * Math.PI / 180;
            const rad3 = j3 * Math.PI / 180;

            const L0 = 40;  // Base height
            const L1 = 90;  // Boom
            const L2 = 80;  // Forearm
            const L3 = 45;  // Claw extension

            const bS = { x: 0, y: L0, z: 0 };

            function rotY(x, z) {
                return {
                    x: x * Math.cos(rad1) - z * Math.sin(rad1),
                    z: x * Math.sin(rad1) + z * Math.cos(rad1)
                };
            }

            const eR = L1 * Math.cos(rad2);
            const eY = L0 + L1 * Math.sin(rad2);
            const eXZ = rotY(eR, 0);
            const bE = { x: eXZ.x, y: eY, z: eXZ.z };

            const wR = eR + L2 * Math.cos(rad3);
            const wY = eY + L2 * Math.sin(rad3);
            const wXZ = rotY(wR, 0);
            const bW = { x: wXZ.x, y: wY, z: wXZ.z };

            const gR = wR + L3;
            const gXZ = rotY(gR, 0);
            const bG = { x: gXZ.x, y: wY, z: gXZ.z };

            const cR = 25 * Math.cos(rad3);
            const cY = L0 + 25 * Math.sin(rad3);
            const cXZ = rotY(cR, 12);
            const bC = { x: cXZ.x, y: cY, z: cXZ.z };

            const pOff = rotY(eR, 12);
            const bP = { x: pOff.x, y: eY, z: pOff.z };

            const tOff1 = rotY(eR, -8);
            const bT1 = { x: tOff1.x, y: eY + 10, z: tOff1.z };
            const tOff2 = rotY(wR, -8);
            const bT2 = { x: tOff2.x, y: wY + 10, z: tOff2.z };

            // Render Structure
            drawLine3D({x:-30, y:0, z:-30}, {x:30, y:0, z:-30}, '#334155', 4);
            drawLine3D({x:30, y:0, z:-30}, {x:30, y:0, z:30}, '#334155', 4);
            drawLine3D({x:30, y:0, z:30}, {x:-30, y:0, z:30}, '#334155', 4);
            drawLine3D({x:-30, y:0, z:30}, {x:-30, y:0, z:-30}, '#334155', 4);
            drawLine3D({x:0, y:0, z:0}, bS, '#475569', 8);

            const sL = rotY(0, -14);
            const sR = rotY(0, 14);
            drawLine3D({x:sL.x, y:L0, z:sL.z}, {x:sL.x, y:L0-20, z:sL.z}, '#0284c7', 8);
            drawLine3D({x:sR.x, y:L0, z:sR.z}, {x:sR.x, y:L0-20, z:sR.z}, '#0284c7', 8);

            const cAxis = rotY(0, 12);
            drawLine3D({x:cAxis.x, y:L0, z:cAxis.z}, bC, '#f8fafc', 3);

            drawLine3D(bC, bP, '#818cf8', 3, [4, 4]);

            const boomAxis = rotY(0, -6);
            const bS_off = { x: boomAxis.x, y: L0, z: boomAxis.z };
            drawLine3D(bS_off, bE, '#38bdf8', 6);

            drawLine3D(bE, bW, '#818cf8', 5);

            drawLine3D(bT1, bT2, 'rgba(56,189,248,0.5)', 3);

            drawLine3D(bW, bG, '#fb7185', 5);

            const clawWidth = (j4 / 17.0) * 10;
            const fL = rotY(gR, -clawWidth);
            const fR = rotY(gR, clawWidth);
            const fL_tip = rotY(gR + 15, -clawWidth);
            const fR_tip = rotY(gR + 15, clawWidth);

            drawLine3D({x:bG.x, y:bG.y, z:bG.z}, {x:fL.x, y:bG.y, z:fL.z}, '#fb7185', 3);
            drawLine3D({x:fL.x, y:bG.y, z:fL.z}, {x:fL_tip.x, y:bG.y, z:fL_tip.z}, '#fb7185', 3);

            drawLine3D({x:bG.x, y:bG.y, z:bG.z}, {x:fR.x, y:bG.y, z:fR.z}, '#fb7185', 3);
            drawLine3D({x:fR.x, y:bG.y, z:fR.z}, {x:fR_tip.x, y:bG.y, z:fR_tip.z}, '#fb7185', 3);

            drawJoint3D(bS, '#ffffff', 4);
            drawJoint3D(bE, '#38bdf8', 5);
            drawJoint3D(bW, '#818cf8', 5);
            drawJoint3D(bG, '#fb7185', 4);
        }

        // -------------------------------------------------------------
        // CONTROL DISPATCHERS
        // -------------------------------------------------------------
        function stepJoint(id, delta) {
            let el = document.getElementById(id);
            let val = parseInt(el.value) + delta;
            val = Math.max(parseInt(el.min), Math.min(parseInt(el.max), val));
            el.value = val;
            onSliderChange();
        }

        function onSliderChange() {
            let j1 = parseFloat(document.getElementById('j1').value);
            let j2 = parseFloat(document.getElementById('j2').value);
            let j3 = parseFloat(document.getElementById('j3').value);
            let j4 = parseFloat(document.getElementById('j4').value);

            document.getElementById('j1-val').innerText = Math.round(j1) + '°';
            document.getElementById('j2-val').innerText = Math.round(j2) + '°';
            document.getElementById('j3-val').innerText = Math.round(j3) + '°';
            document.getElementById('j4-val').innerText = Math.round(j4) + '°';

            draw3DModel();

            let now = Date.now();
            if (now - lastSendTime > 40) {
                lastSendTime = now;
                fetch(`/api/move?j1=${j1}&j2=${j2}&j3=${j3}&j4=${j4}&direct=1`);
            }
        }

        function applyLimitsToSliders(data) {
            document.getElementById('j1').min = data.j1_min; document.getElementById('j1').max = data.j1_max;
            document.getElementById('j1-limit-tag').innerText = `(${data.j1_min}° - ${data.j1_max}°)`;
            document.getElementById('l-j1-min').value = data.j1_min; document.getElementById('l-j1-max').value = data.j1_max; document.getElementById('l-j1-off').value = data.j1_off;

            document.getElementById('j2').min = data.j2_min; document.getElementById('j2').max = data.j2_max;
            document.getElementById('j2-limit-tag').innerText = `(${data.j2_min}° - ${data.j2_max}°)`;
            document.getElementById('l-j2-min').value = data.j2_min; document.getElementById('l-j2-max').value = data.j2_max; document.getElementById('l-j2-off').value = data.j2_off;

            document.getElementById('j3').min = data.j3_min; document.getElementById('j3').max = data.j3_max;
            document.getElementById('j3-limit-tag').innerText = `(${data.j3_min}° - ${data.j3_max}°)`;
            document.getElementById('l-j3-min').value = data.j3_min; document.getElementById('l-j3-max').value = data.j3_max; document.getElementById('l-j3-off').value = data.j3_off;

            document.getElementById('j4').min = data.j4_min; document.getElementById('j4').max = data.j4_max;
            document.getElementById('j4-limit-tag').innerText = `(${data.j4_min}° - ${data.j4_max}°)`;
            document.getElementById('l-j4-min').value = data.j4_min; document.getElementById('l-j4-max').value = data.j4_max; document.getElementById('l-j4-off').value = data.j4_off;
        }

        function saveServoLimits() {
            let url = `/api/limits?` +
                `j1_min=${document.getElementById('l-j1-min').value}&j1_max=${document.getElementById('l-j1-max').value}&j1_off=${document.getElementById('l-j1-off').value}&` +
                `j2_min=${document.getElementById('l-j2-min').value}&j2_max=${document.getElementById('l-j2-max').value}&j2_off=${document.getElementById('l-j2-off').value}&` +
                `j3_min=${document.getElementById('l-j3-min').value}&j3_max=${document.getElementById('l-j3-max').value}&j3_off=${document.getElementById('l-j3-off').value}&` +
                `j4_min=${document.getElementById('l-j4-min').value}&j4_max=${document.getElementById('l-j4-max').value}&j4_off=${document.getElementById('l-j4-off').value}`;

            fetch(url).then(res => res.json()).then(data => {
                if (data.success) {
                    alert("✅ Servo Degree Limits Saved Successfully!");
                    fetchStatus();
                } else {
                    alert("❌ Invalid limits! Ensure Min < Max.");
                }
            });
        }

        function resetServoLimits() {
            document.getElementById('l-j1-min').value = 0; document.getElementById('l-j1-max').value = 180; document.getElementById('l-j1-off').value = 0;
            document.getElementById('l-j2-min').value = 15; document.getElementById('l-j2-max').value = 165; document.getElementById('l-j2-off').value = 0;
            document.getElementById('l-j3-min').value = 10; document.getElementById('l-j3-max').value = 170; document.getElementById('l-j3-off').value = 0;
            document.getElementById('l-j4-min').value = 0; document.getElementById('l-j4-max').value = 17; document.getElementById('l-j4-off').value = 0;
            saveServoLimits();
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
                else alert("⚠️ Target Coordinate Unreachable by MeArm Geometry!");
            });
        }

        function sendTeach(action) {
            fetch(`/api/teach?action=${action}`).then(() => fetchStatus());
        }

        function toggleEStop() {
            let action = isEStopped ? 'RESET_ESTOP' : 'ESTOP';
            fetch(`/api/cmd?c=${action}`).then(() => {
                isEStopped = !isEStopped;
                let btn = document.getElementById('estop-btn');
                let dot = document.getElementById('status-dot');
                if (isEStopped) {
                    btn.innerText = "✅ RESET EMERGENCY LOCKOUT";
                    btn.classList.add("active");
                    dot.style.background = "#ef4444";
                    dot.style.boxShadow = "0 0 10px #ef4444";
                } else {
                    btn.innerText = "🚨 EMERGENCY STOP";
                    btn.classList.remove("active");
                    dot.style.background = "#34d399";
                    dot.style.boxShadow = "0 0 10px #34d399";
                }
            });
        }

        function fetchStatus() {
            fetch('/api/status').then(res => res.json()).then(data => {
                document.getElementById('j1').value = Math.round(data.j1);
                document.getElementById('j2').value = Math.round(data.j2);
                document.getElementById('j3').value = Math.round(data.j3);
                document.getElementById('j4').value = Math.round(data.j4);

                document.getElementById('j1-val').innerText = Math.round(data.j1) + '°';
                document.getElementById('j2-val').innerText = Math.round(data.j2) + '°';
                document.getElementById('j3-val').innerText = Math.round(data.j3) + '°';
                document.getElementById('j4-val').innerText = Math.round(data.j4) + '°';

                document.getElementById('ik-x').value = Math.round(data.x);
                document.getElementById('ik-y').value = Math.round(data.y);
                document.getElementById('ik-z').value = Math.round(data.z);

                document.getElementById('teach-count').innerText = data.teachCount + ' POSES';
                let overlay = document.getElementById('coords-overlay');
                if (overlay) overlay.innerText = `X: ${data.x.toFixed(1)} | Y: ${data.y.toFixed(1)} | Z: ${data.z.toFixed(1)}`;

                applyLimitsToSliders(data);
                draw3DModel();
            });
        }

        window.onload = () => {
            init3DCanvas();
            fetchStatus();
            setInterval(fetchStatus, 3000);
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
        while (WiFi.status() != WL_CONNECTED && attempts < 20) {
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
    server.on("/api/limits", HTTP_GET, std::bind(&WebServerController::handleApiSetLimits, this));
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
    ServoController& sc = robot.getServoController();

    JointState s1 = sc.getJointState(0);
    JointState s2 = sc.getJointState(1);
    JointState s3 = sc.getJointState(2);
    JointState s4 = sc.getJointState(3);

    JsonDocument doc;
    doc["j1"] = j.j1; doc["j1_min"] = s1.minAngle; doc["j1_max"] = s1.maxAngle; doc["j1_off"] = s1.offsetAngle;
    doc["j2"] = j.j2; doc["j2_min"] = s2.minAngle; doc["j2_max"] = s2.maxAngle; doc["j2_off"] = s2.offsetAngle;
    doc["j3"] = j.j3; doc["j3_min"] = s3.minAngle; doc["j3_max"] = s3.maxAngle; doc["j3_off"] = s3.offsetAngle;
    doc["j4"] = j.j4; doc["j4_min"] = s4.minAngle; doc["j4_max"] = s4.maxAngle; doc["j4_off"] = s4.offsetAngle;

    doc["x"] = xyz.x;
    doc["y"] = xyz.y;
    doc["z"] = xyz.z;
    doc["isMoving"] = sc.isMoving();
    doc["isEStopped"] = sc.isEStopped();
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
        bool direct = server.hasArg("direct") ? (server.arg("direct") == "1" || server.arg("direct") == "true") : true;
        robot.moveJoints(j1, j2, j3, j4, direct);
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

void WebServerController::handleApiSetLimits() {
    ServoController& sc = robot.getServoController();
    bool ok = true;

    if (server.hasArg("j1_min") && server.hasArg("j1_max")) {
        ok &= sc.setJointLimits(0, server.arg("j1_min").toFloat(), server.arg("j1_max").toFloat(), server.hasArg("j1_off") ? server.arg("j1_off").toFloat() : 0.0f);
    }
    if (server.hasArg("j2_min") && server.hasArg("j2_max")) {
        ok &= sc.setJointLimits(1, server.arg("j2_min").toFloat(), server.arg("j2_max").toFloat(), server.hasArg("j2_off") ? server.arg("j2_off").toFloat() : 0.0f);
    }
    if (server.hasArg("j3_min") && server.hasArg("j3_max")) {
        ok &= sc.setJointLimits(2, server.arg("j3_min").toFloat(), server.arg("j3_max").toFloat(), server.hasArg("j3_off") ? server.arg("j3_off").toFloat() : 0.0f);
    }
    if (server.hasArg("j4_min") && server.hasArg("j4_max")) {
        ok &= sc.setJointLimits(3, server.arg("j4_min").toFloat(), server.arg("j4_max").toFloat(), server.hasArg("j4_off") ? server.arg("j4_off").toFloat() : 0.0f);
    }

    if (ok) {
        server.send(200, "application/json", "{\"success\":true}");
    } else {
        server.send(400, "application/json", "{\"success\":false,\"error\":\"Invalid joint limit range\"}");
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
