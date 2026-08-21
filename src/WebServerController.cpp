#include "WebServerController.h"

// Professional Industrial Glassmorphism Dashboard UI with Motion Speed Control & 3D Canvas
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

        <!-- PANEL 1: DIRECT JOINT CONTROL & SPEED LIMITER -->
        <div class="panel">
            <div class="panel-header">
                <h2>Joint Kinematics</h2>
                <span class="value-tag" id="motion-status">READY</span>
            </div>

            <!-- JOINT 1 -->
            <div class="control-item">
                <div class="control-label">
                    <span>J1: BASE YAW (LEFT / RIGHT) <span class="limit-subtext" id="j1-limit-tag">(0° - 180°)</span></span>
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
                    <span>J2: SHOULDER BOOM (FRONT / BACK REACH) <span class="limit-subtext" id="j2-limit-tag">(15° - 165°)</span></span>
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
                    <span>J3: ELEVATOR BAR (HEIGHT UP / DOWN) <span class="limit-subtext" id="j3-limit-tag">(50° - 170°)</span></span>
                    <span class="value-tag" id="j3-val">90°</span>
                </div>
                <div class="range-wrapper">
                    <button class="step-btn" onclick="stepJoint('j3', -2)">-</button>
                    <input type="range" id="j3" min="50" max="170" value="90" oninput="onSliderChange()">
                    <button class="step-btn" onclick="stepJoint('j3', 2)">+</button>
                </div>
            </div>

            <!-- JOINT 4 -->
            <div class="control-item">
                <div class="control-label">
                    <span>J4: GRIPPER CLAW (OPEN / CLOSE) <span class="limit-subtext" id="j4-limit-tag">(0° - 17°)</span></span>
                    <span class="value-tag" id="j4-val">10°</span>
                </div>
                <div class="range-wrapper">
                    <button class="step-btn" onclick="stepJoint('j4', -2)">-</button>
                    <input type="range" id="j4" min="0" max="17" value="10" oninput="onSliderChange()">
                    <button class="step-btn" onclick="stepJoint('j4', 2)">+</button>
                </div>
            </div>

            <!-- 🚀 DYNAMIC MOTION SPEED SLIDER -->
            <div class="control-item" style="margin-top: 10px; border: 1px solid rgba(56, 189, 248, 0.3); background: rgba(56, 189, 248, 0.05);">
                <div class="control-label">
                    <span style="color:var(--accent-cyan); font-weight:700;">⚡ MOTION SPEED LIMIT</span>
                    <span class="value-tag" id="speed-val" style="background: rgba(56,189,248,0.2);">35 deg/s</span>
                </div>
                <div class="range-wrapper">
                    <span style="font-size:0.7rem; color:var(--text-muted); font-weight:600;">SLOW</span>
                    <input type="range" id="speed-slider" min="10" max="120" value="35" oninput="onSpeedChange()">
                    <span style="font-size:0.7rem; color:var(--text-muted); font-weight:600;">FAST</span>
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
                        <td><input type="number" id="l-j3-min" value="50" min="0" max="180"></td>
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
            const w = canvas3D.width, h = canvas3D.height;
            ctx3D.clearRect(0, 0, w, h);

            // ── Read sliders ──────────────────────────────────────────────
            const j1 = parseFloat(document.getElementById('j1').value || 90);
            const j2 = parseFloat(document.getElementById('j2').value || 90);
            const j3 = parseFloat(document.getElementById('j3').value || 90);
            const j4 = parseFloat(document.getElementById('j4').value || 5);

            // ── Angles ───────────────────────────────────────────────────
            const yaw  = (j1 - 90) * Math.PI / 180;   // J1 base rotation
            const a2   = (90 - j2) * Math.PI / 180;   // J2 boom: 0=vertical, +ve=forward
            const a3   = (90 - j3) * Math.PI / 180;   // J3 rear bar: same convention

            // ── Scale & dimensions (from GrabCAD model, ~160x155x170mm) ──
            const BP   = 52;   // base plate half-size (square ~105mm)
            const LEG  = 10;   // leg height
            const CW   = 16;   // chassis half-width
            const CD   = 10;   // chassis half-depth
            const CH   = 34;   // chassis height to pivot
            const L1   = 68;   // J2 lower boom length
            const L2   = 60;   // forearm / upper arm length
            const LR   = 68;   // J3 rear bar length (= L1 for parallelogram)
            const WP   = 16;   // wrist plate half-width
            const GR   = 30;   // wrist-to-claw distance
            const FL   = 22;   // finger length
            const SL   = 14;   // SG90 half-length
            const SW   = 6;    // SG90 half-width
            const SH   = 10;   // SG90 half-height

            // ── J1 yaw helper ─────────────────────────────────────────────
            function ry(x,z){ return { x: x*Math.cos(yaw)-z*Math.sin(yaw), z: x*Math.sin(yaw)+z*Math.cos(yaw) }; }
            function P(x,y,z){ const r=ry(x,z); return {x:r.x,y,z:r.z}; }

            // ── Grid ──────────────────────────────────────────────────────
            for(let i=-140;i<=140;i+=20){
                drawLine3D({x:i,y:0,z:-140},{x:i,y:0,z:140},'rgba(56,189,248,0.06)',1);
                drawLine3D({x:-140,y:0,z:i},{x:140,y:0,z:i},'rgba(56,189,248,0.06)',1);
            }

            // ══════════════════════════════════════════════════════════════
            // 1. BASE PLATE + 4 LEGS  (fixed, no J1 rotation on plate itself)
            // ══════════════════════════════════════════════════════════════
            const col_base = '#334155';
            drawLine3D({x:-BP,y:LEG,z:-BP},{x:BP,y:LEG,z:-BP},col_base,4);
            drawLine3D({x: BP,y:LEG,z:-BP},{x:BP,y:LEG,z: BP},col_base,4);
            drawLine3D({x: BP,y:LEG,z: BP},{x:-BP,y:LEG,z:BP},col_base,4);
            drawLine3D({x:-BP,y:LEG,z: BP},{x:-BP,y:LEG,z:-BP},col_base,4);
            // fill hatch
            for(let x=-BP+12;x<BP;x+=14)
                drawLine3D({x,y:LEG,z:-BP},{x,y:LEG,z:BP},'rgba(51,65,85,0.2)',1);
            // 4 corner legs
            for(const [lx,lz] of [[-BP+8,-BP+8],[BP-8,-BP+8],[BP-8,BP-8],[-BP+8,BP-8]]){
                drawLine3D({x:lx,y:0,z:lz},{x:lx,y:LEG,z:lz},'#475569',4);
            }

            // ══════════════════════════════════════════════════════════════
            // 2. J1 BASE SERVO (under plate, blue block, fixed)
            // ══════════════════════════════════════════════════════════════
            drawLine3D({x:-SL,y:0,z:-SW},{x:SL,y:0,z:-SW},'#2563eb',5);
            drawLine3D({x: SL,y:0,z:-SW},{x:SL,y:0,z: SW},'#2563eb',5);
            drawLine3D({x: SL,y:0,z: SW},{x:-SL,y:0,z:SW},'#2563eb',5);
            drawLine3D({x:-SL,y:0,z: SW},{x:-SL,y:0,z:-SW},'#2563eb',5);
            drawLine3D({x:-SL,y:0,z:-SW},{x:-SL,y:LEG,z:-SW},'#2563eb',3);
            drawLine3D({x: SL,y:0,z:-SW},{x: SL,y:LEG,z:-SW},'#2563eb',3);
            drawLine3D({x: SL,y:0,z: SW},{x: SL,y:LEG,z: SW},'#2563eb',3);
            drawLine3D({x:-SL,y:0,z: SW},{x:-SL,y:LEG,z: SW},'#2563eb',3);

            // ══════════════════════════════════════════════════════════════
            // 3. UPRIGHT CHASSIS BLOCK  (rotates with J1)
            //    Two side plates + cross braces
            // ══════════════════════════════════════════════════════════════
            const col_ch = '#64748b';
            // Left plate  (J2 servo side, x = -CW)
            drawLine3D(P(-CW,LEG,-CD), P(-CW,LEG+CH,-CD), col_ch, 6);
            drawLine3D(P(-CW,LEG, CD), P(-CW,LEG+CH, CD), col_ch, 6);
            drawLine3D(P(-CW,LEG,-CD), P(-CW,LEG,   CD), '#475569', 4);
            drawLine3D(P(-CW,LEG+CH,-CD), P(-CW,LEG+CH,CD), '#475569', 4);
            // Right plate (J3 servo side, x = +CW)
            drawLine3D(P( CW,LEG,-CD), P( CW,LEG+CH,-CD), col_ch, 6);
            drawLine3D(P( CW,LEG, CD), P( CW,LEG+CH, CD), col_ch, 6);
            drawLine3D(P( CW,LEG,-CD), P( CW,LEG,   CD), '#475569', 4);
            drawLine3D(P( CW,LEG+CH,-CD), P( CW,LEG+CH,CD), '#475569', 4);
            // Horizontal cross braces
            drawLine3D(P(-CW,LEG+CH*0.3,0), P(CW,LEG+CH*0.3,0), '#475569', 3);
            drawLine3D(P(-CW,LEG+CH*0.65,0),P(CW,LEG+CH*0.65,0),'#475569', 3);

            // ══════════════════════════════════════════════════════════════
            // 4. J2 SERVO (left side of chassis)  — blue SG90 box
            // ══════════════════════════════════════════════════════════════
            const sy = LEG + CH*0.45;   // vertical centre of servo
            // left face of servo is outside the left plate
            drawLine3D(P(-CW-SL*2,sy-SH,-SW), P(-CW,sy-SH,-SW), '#2563eb', 4);
            drawLine3D(P(-CW-SL*2,sy+SH,-SW), P(-CW,sy+SH,-SW), '#2563eb', 4);
            drawLine3D(P(-CW-SL*2,sy-SH,-SW), P(-CW-SL*2,sy+SH,-SW), '#2563eb', 4);
            drawLine3D(P(-CW-SL*2,sy-SH, SW), P(-CW,sy-SH, SW), '#2563eb', 4);
            drawLine3D(P(-CW-SL*2,sy+SH, SW), P(-CW,sy+SH, SW), '#2563eb', 4);
            drawLine3D(P(-CW-SL*2,sy-SH, SW), P(-CW-SL*2,sy+SH, SW), '#2563eb', 4);
            drawLine3D(P(-CW-SL*2,sy-SH,-SW), P(-CW-SL*2,sy-SH,SW), '#2563eb', 3);
            drawLine3D(P(-CW-SL*2,sy+SH,-SW), P(-CW-SL*2,sy+SH,SW), '#2563eb', 3);

            // ══════════════════════════════════════════════════════════════
            // 5. J3 SERVO (right side of chassis) — sky blue SG90 box
            // ══════════════════════════════════════════════════════════════
            drawLine3D(P(CW,sy-SH,-SW), P(CW+SL*2,sy-SH,-SW), '#0ea5e9', 4);
            drawLine3D(P(CW,sy+SH,-SW), P(CW+SL*2,sy+SH,-SW), '#0ea5e9', 4);
            drawLine3D(P(CW+SL*2,sy-SH,-SW), P(CW+SL*2,sy+SH,-SW), '#0ea5e9', 4);
            drawLine3D(P(CW,sy-SH, SW), P(CW+SL*2,sy-SH, SW), '#0ea5e9', 4);
            drawLine3D(P(CW,sy+SH, SW), P(CW+SL*2,sy+SH, SW), '#0ea5e9', 4);
            drawLine3D(P(CW+SL*2,sy-SH, SW), P(CW+SL*2,sy+SH, SW), '#0ea5e9', 4);
            drawLine3D(P(CW+SL*2,sy-SH,-SW), P(CW+SL*2,sy-SH,SW), '#0ea5e9', 3);
            drawLine3D(P(CW+SL*2,sy+SH,-SW), P(CW+SL*2,sy+SH,SW), '#0ea5e9', 3);

            // ══════════════════════════════════════════════════════════════
            // 6. FOUR-BAR PARALLELOGRAM KINEMATICS
            //
            //   Pivot heights: both J2 and J3 pivot at y = LEG + CH
            //   J2 (LEFT):  lower boom swings in sagittal plane → controls REACH
            //   J3 (RIGHT): rear bar swings same plane → controls HEIGHT
            //   Forearm:    parallel to rear bar (parallelogram), length = L2
            //   Top bar:    connects rodTop → elbowTop (closes the 4-bar loop)
            // ══════════════════════════════════════════════════════════════
            const pivY = LEG + CH;   // both pivots at top of chassis

            // J2 pivot (left side, x = -CW)
            const j2piv = P(-CW, pivY, 0);
            // boom tip (elbow)
            const bFwd = L1*Math.sin(a2), bUp = L1*Math.cos(a2);
            const elbow = P(-CW, pivY+bUp, bFwd);

            // J3 pivot (right side, x = +CW)
            const j3piv = P( CW, pivY, 0);
            // rear bar tip
            const rFwd = LR*Math.sin(a3), rUp = LR*Math.cos(a3);
            const rodTop = P(CW, pivY+rUp, rFwd);

            // Forearm: from elbow, parallel to rear bar, length L2
            const faFwd = L2*Math.sin(a3), faUp = L2*Math.cos(a3);
            const wristL = P(-CW, pivY+bUp+faUp, bFwd+faFwd);   // left wrist
            const wrist  = P(  0, pivY+bUp+faUp, bFwd+faFwd);   // centre wrist
            const wristR = P( CW, pivY+bUp+faUp, bFwd+faFwd);   // right wrist

            // Claw pivot extends forward
            const clawY = pivY + bUp + faUp;
            const clawZ = bFwd + faFwd + GR;
            const claw  = P(0, clawY, clawZ);

            // ══════════════════════════════════════════════════════════════
            // 7. DRAW LINKAGE (back-to-front painter's order)
            // ══════════════════════════════════════════════════════════════

            // Rear bar — J3 (amber)
            drawLine3D(j3piv, rodTop, '#f59e0b', 7);
            drawJoint3D(rodTop, '#f59e0b', 5);

            // Top closing bar: rodTop → wristR (completes parallelogram)
            drawLine3D(rodTop, wristR, '#94a3b8', 4);

            // Lower boom — J2 (cyan)
            drawLine3D(j2piv, elbow, '#38bdf8', 8);
            drawJoint3D(elbow, '#38bdf8', 5);

            // Forearm — driven by J3 angle (indigo)
            drawLine3D(elbow, wristL, '#818cf8', 7);

            // Wrist connector left→centre
            drawLine3D(wristL, wrist, '#a5b4fc', 4);

            // Wrist plate (horizontal bar spanning the width)
            drawLine3D(P(-WP, clawY, bFwd+faFwd), P(WP, clawY, bFwd+faFwd), '#e2e8f0', 7);
            drawJoint3D(wrist, '#818cf8', 5);

            // J4 servo on wrist plate (small blue box)
            drawLine3D(P(-8,clawY+14,bFwd+faFwd+4), P(8,clawY+14,bFwd+faFwd+4), '#3b82f6',4);
            drawLine3D(P(-8,clawY,   bFwd+faFwd+4), P(8,clawY,   bFwd+faFwd+4), '#3b82f6',4);
            drawLine3D(P(-8,clawY,   bFwd+faFwd+4), P(-8,clawY+14,bFwd+faFwd+4),'#3b82f6',4);
            drawLine3D(P( 8,clawY,   bFwd+faFwd+4), P( 8,clawY+14,bFwd+faFwd+4),'#3b82f6',4);

            // Gripper arm to claw centre
            drawLine3D(wrist, claw, '#fb7185', 6);
            drawJoint3D(claw, '#fb7185', 5);

            // Pincer fingers (open sideways — j4=0 closed, j4=17 open)
            const openAng = (j4/17.0) * 32 * Math.PI/180;
            // Left finger
            const lf1 = P(-FL*Math.sin(openAng), clawY, clawZ + FL*Math.cos(openAng));
            const lf2 = P(-FL*Math.sin(openAng)*0.4, clawY-5, clawZ + FL);
            drawLine3D(claw, lf1, '#fb7185', 5);
            drawLine3D(lf1, lf2, '#fb7185', 3);
            // Right finger
            const rf1 = P( FL*Math.sin(openAng), clawY, clawZ + FL*Math.cos(openAng));
            const rf2 = P( FL*Math.sin(openAng)*0.4, clawY-5, clawZ + FL);
            drawLine3D(claw, rf1, '#fb7185', 5);
            drawLine3D(rf1, rf2, '#fb7185', 3);

            // ══════════════════════════════════════════════════════════════
            // 8. PIVOT JOINTS
            // ══════════════════════════════════════════════════════════════
            drawJoint3D(j2piv, '#38bdf8', 6);
            drawJoint3D(j3piv, '#f59e0b', 6);
            drawJoint3D(P(0,LEG,0), '#ffffff', 5);   // J1 shaft

            // ══════════════════════════════════════════════════════════════
            // 9. LABELS
            // ══════════════════════════════════════════════════════════════
            ctx3D.font = 'bold 9px Inter,sans-serif';
            for(const [p,c,t] of [
                [j2piv,'#38bdf8','J2'],
                [j3piv,'#f59e0b','J3'],
                [wrist, '#818cf8','WRIST'],
                [claw,  '#fb7185','J4'],
            ]){
                const s = project3D(p.x,p.y,p.z);
                ctx3D.fillStyle = c;
                ctx3D.fillText(t, s.x+8, s.y-5);
            }
        }


        // -------------------------------------------------------------
        // CONTROL DISPATCHERS & SPEED SLIDER
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
                // Move with smooth trajectory interpolation speed control (direct=0)
                fetch(`/api/move?j1=${j1}&j2=${j2}&j3=${j3}&j4=${j4}&direct=0`);
            }
        }

        function onSpeedChange() {
            let spd = parseFloat(document.getElementById('speed-slider').value);
            document.getElementById('speed-val').innerText = Math.round(spd) + ' deg/s';
            fetch(`/api/speed?s=${spd}`);
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
            document.getElementById('l-j3-min').value = 50; document.getElementById('l-j3-max').value = 170; document.getElementById('l-j3-off').value = 0;
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

                if (data.speed !== undefined) {
                    document.getElementById('speed-slider').value = Math.round(data.speed);
                    document.getElementById('speed-val').innerText = Math.round(data.speed) + ' deg/s';
                }

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

const char MODEL_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
    <title>Robot Arm Model Control</title>
    <style>
        :root {
            --bg: #10131a;
            --panel: #171c25;
            --panel-2: #202633;
            --line: #323b4d;
            --text: #f4f7fb;
            --muted: #9ca8ba;
            --cyan: #36c5f0;
            --green: #4ade80;
            --red: #f87171;
            --amber: #fbbf24;
            --metal: #d9dde5;
            --servo: #1596c7;
        }

        * {
            box-sizing: border-box;
            margin: 0;
            padding: 0;
            font-family: Arial, Helvetica, sans-serif;
        }

        body {
            min-height: 100vh;
            background: var(--bg);
            color: var(--text);
            display: grid;
            grid-template-columns: minmax(0, 1fr) 340px;
            overflow: hidden;
        }

        .viewer {
            position: relative;
            min-width: 0;
            background: #eef1f5;
        }

        canvas {
            width: 100%;
            height: 100%;
            display: block;
            touch-action: none;
            cursor: grab;
        }

        canvas:active {
            cursor: grabbing;
        }

        .hud {
            position: absolute;
            left: 16px;
            top: 16px;
            display: flex;
            gap: 8px;
            align-items: center;
            color: #10131a;
            font-size: 13px;
            font-weight: 700;
        }

        .pill {
            border: 1px solid rgba(16, 19, 26, 0.12);
            background: rgba(255, 255, 255, 0.78);
            padding: 7px 10px;
            border-radius: 8px;
            box-shadow: 0 8px 24px rgba(15, 23, 42, 0.08);
        }

        aside {
            border-left: 1px solid var(--line);
            background: var(--panel);
            padding: 16px;
            overflow-y: auto;
        }

        h1 {
            font-size: 18px;
            font-weight: 800;
            margin-bottom: 4px;
        }

        .sub {
            color: var(--muted);
            font-size: 12px;
            line-height: 1.4;
            margin-bottom: 16px;
        }

        .status {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 8px;
            margin-bottom: 16px;
        }

        .metric {
            background: var(--panel-2);
            border: 1px solid var(--line);
            border-radius: 8px;
            padding: 10px;
        }

        .metric span {
            display: block;
            color: var(--muted);
            font-size: 11px;
            text-transform: uppercase;
            margin-bottom: 5px;
        }

        .metric strong {
            font-size: 15px;
        }

        .joint {
            border-top: 1px solid var(--line);
            padding: 14px 0;
        }

        .joint label {
            display: flex;
            justify-content: space-between;
            align-items: center;
            font-size: 13px;
            font-weight: 700;
            margin-bottom: 10px;
        }

        .joint output {
            color: var(--cyan);
            font-family: Consolas, monospace;
            font-size: 14px;
        }

        input[type=range] {
            width: 100%;
            accent-color: var(--cyan);
        }

        .row {
            display: grid;
            grid-template-columns: repeat(2, 1fr);
            gap: 8px;
            margin-top: 10px;
        }

        button {
            border: 1px solid var(--line);
            background: var(--panel-2);
            color: var(--text);
            min-height: 38px;
            border-radius: 8px;
            font-weight: 700;
            cursor: pointer;
        }

        button:hover {
            border-color: var(--cyan);
        }

        .primary {
            background: #075985;
            border-color: var(--cyan);
        }

        .danger {
            background: #7f1d1d;
            border-color: var(--red);
        }

        .toggle {
            display: flex;
            justify-content: space-between;
            align-items: center;
            background: var(--panel-2);
            border: 1px solid var(--line);
            border-radius: 8px;
            padding: 12px;
            margin-bottom: 14px;
            font-size: 13px;
            font-weight: 700;
        }

        .toggle input {
            width: 20px;
            height: 20px;
            accent-color: var(--green);
        }

        .limits {
            color: var(--muted);
            font-size: 12px;
            line-height: 1.6;
            border-top: 1px solid var(--line);
            padding-top: 14px;
            margin-top: 16px;
        }

        @media (max-width: 780px) {
            body {
                grid-template-columns: 1fr;
                grid-template-rows: 55vh 45vh;
                overflow: auto;
            }

            aside {
                border-left: 0;
                border-top: 1px solid var(--line);
            }
        }
    </style>
</head>
<body>
    <main class="viewer">
        <canvas id="scene"></canvas>
        <div class="hud">
            <div class="pill" id="link-state">Disconnected</div>
            <div class="pill" id="tip-state">X 0 | Y 0 | Z 0</div>
        </div>
    </main>

    <aside>
        <h1>Virtual Arm Control</h1>
        <p class="sub">Move the model. When live link is enabled, the physical arm receives the same joint targets through the ESP32 API.</p>

        <div class="toggle">
            <span>Live link to physical arm</span>
            <input id="live-link" type="checkbox" checked>
        </div>

        <div class="status">
            <div class="metric"><span>Motion</span><strong id="motion">READY</strong></div>
            <div class="metric"><span>E-stop</span><strong id="estop">CLEAR</strong></div>
            <div class="metric"><span>Speed</span><strong id="speed">35 deg/s</strong></div>
            <div class="metric"><span>Plate</span><strong>2.8 mm</strong></div>
        </div>

        <section class="joint">
            <label for="j1">J1 Base Yaw <output id="j1-out">90 deg</output></label>
            <input id="j1" type="range" min="0" max="180" value="90" step="1">
        </section>

        <section class="joint">
            <label for="j2">J2 Shoulder <output id="j2-out">90 deg</output></label>
            <input id="j2" type="range" min="15" max="165" value="90" step="1">
        </section>

        <section class="joint">
            <label for="j3">J3 Linkage <output id="j3-out">90 deg</output></label>
            <input id="j3" type="range" min="50" max="170" value="90" step="1">
        </section>

        <section class="joint">
            <label for="j4">J4 Gripper <output id="j4-out">10 deg</output></label>
            <input id="j4" type="range" min="0" max="17" value="10" step="1">
        </section>

        <div class="row">
            <button class="primary" onclick="preset(90,90,90,10)">Home</button>
            <button onclick="preset(90,30,50,0)">Rest</button>
            <button onclick="setJoint('j4',0)">Open</button>
            <button onclick="setJoint('j4',17)">Close</button>
            <button class="danger" onclick="cmd('ESTOP')">E-stop</button>
            <button onclick="cmd('RESET_ESTOP')">Reset</button>
        </div>

        <div class="limits">
            <strong>Model dimensions</strong><br>
            Base plate: 95 x 144 mm<br>
            Link model: L0 85 mm, L1 120 mm, L2 110 mm, L3 65 mm<br>
            Limits: J1 0-180, J2 15-165, J3 50-170, J4 0-17 deg<br>
            Coupled rule: near J2 15 deg, J3 should stay at least 85 deg.
        </div>
    </aside>

    <script>
        const canvas = document.getElementById('scene');
        const ctx = canvas.getContext('2d');
        const ids = ['j1', 'j2', 'j3', 'j4'];
        const state = { j1: 90, j2: 90, j3: 90, j4: 10, speed: 35, moving: false, estop: false };
        const limits = {
            j1: [0, 180],
            j2: [15, 165],
            j3: [50, 170],
            j4: [0, 17]
        };
        const dims = { baseW: 95, baseD: 144, l0: 85, l1: 120, l2: 110, l3: 65, halfGap: 12 };
        let yawView = -38 * Math.PI / 180;
        let pitchView = 24 * Math.PI / 180;
        let dragging = false;
        let lastPointer = null;
        let lastSend = 0;

        function deg(v) { return v * Math.PI / 180; }
        function clamp(v, lo, hi) { return Math.max(lo, Math.min(hi, v)); }

        function resize() {
            const rect = canvas.getBoundingClientRect();
            const dpr = window.devicePixelRatio || 1;
            canvas.width = Math.max(1, Math.round(rect.width * dpr));
            canvas.height = Math.max(1, Math.round(rect.height * dpr));
            ctx.setTransform(dpr, 0, 0, dpr, 0, 0);
            draw();
        }

        function project(p) {
            const cy = Math.cos(yawView), sy = Math.sin(yawView);
            const cp = Math.cos(pitchView), sp = Math.sin(pitchView);
            const x1 = p.x * cy - p.z * sy;
            const z1 = p.x * sy + p.z * cy;
            const y1 = p.y * cp - z1 * sp;
            const scale = Math.min(canvas.clientWidth / 280, canvas.clientHeight / 245);
            return {
                x: canvas.clientWidth * 0.52 + x1 * scale,
                y: canvas.clientHeight * 0.72 - y1 * scale
            };
        }

        function armPoints(sideOffset) {
            const baseYaw = deg(state.j1 - 90);
            const j2 = deg(state.j2);
            const j3 = deg(state.j3);
            const sx = sideOffset * Math.cos(baseYaw + Math.PI / 2);
            const sz = sideOffset * Math.sin(baseYaw + Math.PI / 2);
            const fx = Math.sin(baseYaw);
            const fz = Math.cos(baseYaw);
            const shoulder = { x: sx, y: dims.l0, z: sz };
            const elbow = {
                x: shoulder.x + fx * dims.l1 * Math.cos(j2),
                y: shoulder.y + dims.l1 * Math.sin(j2),
                z: shoulder.z + fz * dims.l1 * Math.cos(j2)
            };
            const wrist = {
                x: elbow.x + fx * dims.l2 * Math.cos(j3),
                y: elbow.y + dims.l2 * Math.sin(j3),
                z: elbow.z + fz * dims.l2 * Math.cos(j3)
            };
            const tip = {
                x: wrist.x + fx * dims.l3,
                y: wrist.y,
                z: wrist.z + fz * dims.l3
            };
            return { shoulder, elbow, wrist, tip };
        }

        function line(a, b, color, width) {
            const pa = project(a), pb = project(b);
            ctx.strokeStyle = color;
            ctx.lineWidth = width;
            ctx.lineCap = 'round';
            ctx.beginPath();
            ctx.moveTo(pa.x, pa.y);
            ctx.lineTo(pb.x, pb.y);
            ctx.stroke();
        }

        function node(p, color, r) {
            const q = project(p);
            ctx.fillStyle = color;
            ctx.beginPath();
            ctx.arc(q.x, q.y, r, 0, Math.PI * 2);
            ctx.fill();
            ctx.strokeStyle = '#ffffff';
            ctx.lineWidth = 2;
            ctx.stroke();
        }

        function plate() {
            const w = dims.baseW / 2, d = dims.baseD / 2;
            const pts = [
                {x:-w,y:0,z:-d}, {x:w,y:0,z:-d},
                {x:w,y:0,z:d}, {x:-w,y:0,z:d}
            ];
            const p = pts.map(project);
            ctx.fillStyle = '#d9dde5';
            ctx.strokeStyle = '#a8b0bd';
            ctx.lineWidth = 2;
            ctx.beginPath();
            ctx.moveTo(p[0].x, p[0].y);
            for (let i = 1; i < p.length; i++) ctx.lineTo(p[i].x, p[i].y);
            ctx.closePath();
            ctx.fill();
            ctx.stroke();
        }

        function drawServo(center, w, h, color) {
            const p = project(center);
            ctx.fillStyle = color;
            ctx.strokeStyle = '#0d6688';
            ctx.lineWidth = 2;
            ctx.fillRect(p.x - w / 2, p.y - h / 2, w, h);
            ctx.strokeRect(p.x - w / 2, p.y - h / 2, w, h);
        }

        function draw() {
            ctx.clearRect(0, 0, canvas.clientWidth, canvas.clientHeight);
            plate();
            const left = armPoints(-dims.halfGap);
            const right = armPoints(dims.halfGap);
            const center = armPoints(0);

            line({x:0,y:0,z:0}, {x:0,y:dims.l0,z:0}, '#6b7280', 8);
            drawServo({x:0,y:18,z:0}, 34, 28, '#1596c7');

            line(left.shoulder, left.elbow, '#d9dde5', 9);
            line(right.shoulder, right.elbow, '#d9dde5', 9);
            line(left.elbow, left.wrist, '#d9dde5', 8);
            line(right.elbow, right.wrist, '#d9dde5', 8);
            line(left.shoulder, right.shoulder, '#9ca3af', 4);
            line(left.elbow, right.elbow, '#9ca3af', 4);
            line(left.wrist, right.wrist, '#9ca3af', 4);

            const rearTop = { x: -dims.halfGap, y: dims.l0 + 70, z: -42 };
            const rearBottom = { x: -dims.halfGap, y: dims.l0 - 8, z: -38 };
            line(rearBottom, rearTop, '#cfd4dc', 7);
            line(rearTop, left.elbow, '#cfd4dc', 5);

            line(center.wrist, center.tip, '#f87171', 7);
            const open = state.j4 / 17;
            line(center.tip, {x:center.tip.x - 10 - open * 18, y:center.tip.y - 8, z:center.tip.z + 18}, '#f87171', 5);
            line(center.tip, {x:center.tip.x + 10 + open * 18, y:center.tip.y - 8, z:center.tip.z + 18}, '#f87171', 5);

            [left.shoulder, right.shoulder, left.elbow, right.elbow, left.wrist, right.wrist, center.tip].forEach(p => node(p, '#6b7280', 5));
            drawServo({x:-34,y:dims.l0-18,z:0}, 34, 24, '#1596c7');
            drawServo({x:34,y:dims.l0-18,z:0}, 34, 24, '#1596c7');
            drawServo(center.wrist, 25, 20, '#1596c7');

            document.getElementById('tip-state').textContent =
                'X ' + center.tip.x.toFixed(0) + ' | Y ' + center.tip.y.toFixed(0) + ' | Z ' + center.tip.z.toFixed(0);
        }

        function updateLabels() {
            ids.forEach(id => {
                document.getElementById(id).value = Math.round(state[id]);
                document.getElementById(id + '-out').textContent = Math.round(state[id]) + ' deg';
            });
            document.getElementById('motion').textContent = state.moving ? 'MOVING' : 'READY';
            document.getElementById('estop').textContent = state.estop ? 'ACTIVE' : 'CLEAR';
            document.getElementById('speed').textContent = Math.round(state.speed) + ' deg/s';
            document.getElementById('link-state').textContent =
                document.getElementById('live-link').checked ? 'Live link on' : 'Virtual only';
        }

        function applyCoupledLimit() {
            if (state.j2 <= 15.5 && state.j3 < 85) state.j3 = 85;
        }

        function sendPose(force) {
            if (!document.getElementById('live-link').checked && !force) return;
            const now = Date.now();
            if (!force && now - lastSend < 60) return;
            lastSend = now;
            const q = '?j1=' + state.j1 + '&j2=' + state.j2 + '&j3=' + state.j3 + '&j4=' + state.j4 + '&direct=0';
            fetch('/api/move' + q)
                .then(() => document.getElementById('link-state').textContent = 'Command sent')
                .catch(() => document.getElementById('link-state').textContent = 'Link failed');
        }

        function setJoint(id, value) {
            state[id] = clamp(Number(value), limits[id][0], limits[id][1]);
            applyCoupledLimit();
            updateLabels();
            draw();
            sendPose(true);
        }

        function preset(j1, j2, j3, j4) {
            state.j1 = j1;
            state.j2 = j2;
            state.j3 = j3;
            state.j4 = j4;
            applyCoupledLimit();
            updateLabels();
            draw();
            sendPose(true);
        }

        function cmd(name) {
            fetch('/api/cmd?c=' + encodeURIComponent(name)).then(fetchStatus);
        }

        function fetchStatus() {
            fetch('/api/status')
                .then(r => r.json())
                .then(data => {
                    state.j1 = data.j1;
                    state.j2 = data.j2;
                    state.j3 = data.j3;
                    state.j4 = data.j4;
                    state.speed = data.speed || state.speed;
                    state.moving = !!data.isMoving;
                    state.estop = !!data.isEStopped;
                    ids.forEach(id => {
                        const min = data[id + '_min'];
                        const max = data[id + '_max'];
                        if (min !== undefined && max !== undefined) {
                            limits[id] = [min, max];
                            document.getElementById(id).min = min;
                            document.getElementById(id).max = max;
                        }
                    });
                    updateLabels();
                    draw();
                })
                .catch(() => document.getElementById('link-state').textContent = 'Status offline');
        }

        ids.forEach(id => {
            document.getElementById(id).addEventListener('input', ev => {
                state[id] = Number(ev.target.value);
                applyCoupledLimit();
                updateLabels();
                draw();
                sendPose(false);
            });
            document.getElementById(id).addEventListener('change', () => sendPose(true));
        });

        canvas.addEventListener('pointerdown', ev => {
            dragging = true;
            lastPointer = { x: ev.clientX, y: ev.clientY };
            canvas.setPointerCapture(ev.pointerId);
        });

        canvas.addEventListener('pointermove', ev => {
            if (!dragging || !lastPointer) return;
            yawView += (ev.clientX - lastPointer.x) * 0.008;
            pitchView = clamp(pitchView + (ev.clientY - lastPointer.y) * 0.006, -0.35, 1.15);
            lastPointer = { x: ev.clientX, y: ev.clientY };
            draw();
        });

        canvas.addEventListener('pointerup', ev => {
            dragging = false;
            lastPointer = null;
            canvas.releasePointerCapture(ev.pointerId);
        });

        window.addEventListener('resize', resize);
        document.getElementById('live-link').addEventListener('change', updateLabels);
        resize();
        fetchStatus();
        setInterval(fetchStatus, 2000);
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
    server.on("/model", HTTP_GET, std::bind(&WebServerController::handleModelPage, this));
    server.on("/api/status", HTTP_GET, std::bind(&WebServerController::handleApiStatus, this));
    server.on("/api/move", HTTP_GET, std::bind(&WebServerController::handleApiMoveJoints, this));
    server.on("/api/ik", HTTP_GET, std::bind(&WebServerController::handleApiMoveIK, this));
    server.on("/api/teach", HTTP_GET, std::bind(&WebServerController::handleApiTeach, this));
    server.on("/api/limits", HTTP_GET, std::bind(&WebServerController::handleApiSetLimits, this));
    server.on("/api/speed", HTTP_GET, std::bind(&WebServerController::handleApiSetSpeed, this));
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

void WebServerController::handleModelPage() {
    server.send_P(200, "text/html", MODEL_HTML);
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
    doc["speed"] = sc.getSpeed();
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
        // Default direct=false to enforce smooth trajectory speed control!
        bool direct = server.hasArg("direct") ? (server.arg("direct") == "1" || server.arg("direct") == "true") : false;
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

void WebServerController::handleApiSetSpeed() {
    if (server.hasArg("s")) {
        float spd = server.arg("s").toFloat();
        robot.getServoController().setSpeed(spd);
        server.send(200, "application/json", "{\"success\":true}");
    } else {
        server.send(400, "application/json", "{\"success\":false,\"error\":\"Missing s parameter\"}");
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
