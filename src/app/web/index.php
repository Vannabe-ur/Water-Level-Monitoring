<?php
header('Content-Type: text/html');
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, viewport-fit=cover">
    <title>AquaSense IoT | Water Intelligence Platform</title>

    <script src="https://cdn.jsdelivr.net/npm/chart.js@4.4.0/dist/chart.umd.min.js"></script>
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0-beta3/css/all.min.css">
    <link rel="stylesheet" href="/style/style.css">
</head>
<body>

<div class="navbar">
    <div class="logo">
        <img src="img/water-logo.png" alt="logo" style="width:24px;height:32px;">
        <span>AquaSense<span style="font-weight:400"> | IoT Guard</span></span>
    </div>
    <div class="menu">
        <span><i class="fas fa-chart-line"></i> Dashboard</span>
        <span><i class="fas fa-history"></i> Analytics</span>
        <span><i class="fas fa-microchip"></i> Sensors</span>
        <span><i class="fas fa-headset"></i> Support</span>
    </div>
</div>

<div class="hero">
    <h1><i class="fas fa-droplet"></i> Water Guard IoT</h1>
    <p>Intelligent real-time water level monitoring • Smart alerts & predictive insights for residential & industrial tanks.</p>
    <button onclick="loadData()"><i class="fas fa-sync-alt"></i> Force refresh data</button>
</div>

<div class="kpi-grid">
    <div class="kpi-card">
        <div class="kpi-label"><i class="fas fa-fill-drip"></i> Current level</div>
        <div class="kpi-value" id="current">-- cm</div>
        <div class="trend-badge">real-time</div>
    </div>
    <div class="kpi-card">
        <div class="kpi-label"><i class="fas fa-shield-alt"></i> System status</div>
        <div class="kpi-value" id="status">---</div>
        <div class="trend-badge" id="statusHint">Operational</div>
    </div>
    <div class="kpi-card">
        <div class="kpi-label"><i class="fas fa-chart-line"></i> Daily peak</div>
        <div class="kpi-value" id="max">0 cm</div>
        <div class="trend-badge">last 24h</div>
    </div>
    <div class="kpi-card">
        <div class="kpi-label"><i class="fas fa-chart-simple"></i> 24h average</div>
        <div class="kpi-value" id="avg">0 cm</div>
        <div class="trend-badge">historical mean</div>
    </div>
</div>

<div class="dashboard-main">
    <div class="card tank-container">
        <div class="tank-wrapper">
            <h3><i class="fas fa-tint"></i> Storage tank</h3>
            <div class="tank">
                <div class="water" id="water"></div>
            </div>
            <div class="tank-stats">
                Fill level: <strong id="fillPercent">0%</strong> |
                <span id="levelCm">0</span> cm / 100 cm
            </div>
        </div>
    </div>

    <div class="card chart-card">
        <h3><i class="fas fa-chart-scatter"></i> Level trend</h3>
        <canvas id="waterChart"></canvas>
    </div>
</div>

<div class="card history-section">
    <div class="history-header">
        <h3><i class="fas fa-clock"></i> Recent telemetry log</h3>
    </div>
    <table>
        <thead>
            <tr>
                <th>Water level (cm)</th>
                <th>Status</th>
                <th>Timestamp</th>
            </tr>
        </thead>
        <tbody id="tableBody">
            <tr><td colspan="3">Loading sensor data...</td></tr>
        </tbody>
    </table>
</div>

<footer>
    AquaSense Edge • Real-time monitoring | Data refreshes every 3s
</footer>

<script src="/scripts/main.js"></script>
</body>
</html>