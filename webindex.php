<?php
// webindex.php
header('Content-Type: text/html');
?>
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<title>Water Level Monitor</title>
<style>
* { box-sizing: border-box; }
body {
  margin: 0;
  height: 100vh;
  font-family: 'Arial', sans-serif;
  background: linear-gradient(135deg, #1e3c72, #2a5298);
  display: flex;
  align-items: center;
  justify-content: center;
  color: #fff;
}
.card {
  width: 360px;
  background: rgba(255,255,255,0.1);
  border-radius: 20px;
  padding: 30px;
  text-align: center;
  box-shadow: 0 20px 50px rgba(0,0,0,0.3);
}
h2 { margin: 0 0 15px 0; font-size: 28px; }
.tank {
  width: 120px;
  height: 300px;
  margin: 20px auto;
  border: 4px solid #fff;
  border-radius: 12px;
  position: relative;
  overflow: hidden;
  background: rgba(255,255,255,0.05);
}
.water {
  position: absolute;
  bottom: 0;
  width: 100%;
  height: 0%;
  transition: height 1s ease-in-out;
  border-radius: 0 0 12px 12px;
}
.safe { background: linear-gradient(#4dff88, #00cc66); }
.caution { background: linear-gradient(#ffd633, #ffb700); }
.full { background: linear-gradient(#ff4d4d, #cc0000); }
.value { font-size: 42px; font-weight: bold; margin-top: 10px; }
.status { margin-top: 8px; font-size: 20px; font-weight: bold; }
</style>
</head>
<body>

<div class="card">
  <h2>💧 Water Level Monitor</h2>
  <div class="tank">
    <div class="water safe" id="water"></div>
  </div>
  <div class="value"><span id="cm">0.0</span> cm</div>
  <div class="status" id="statusText">SAFE</div>
</div>

<script>
// ===== CONFIG =====
const tankHeight = 20;  // Max tank height in cm

// Smoothly animate water height
let currentHeight = 0; 
function animateWater(targetPercent) {
    const water = document.getElementById("water");
    const step = (targetPercent - currentHeight) / 20;
    let frame = 0;
    const interval = setInterval(() => {
        currentHeight += step;
        water.style.height = Math.max(0, Math.min(100, currentHeight)) + "%";
        frame++;
        if (frame >= 20) {
            clearInterval(interval);
            currentHeight = targetPercent;
        }
    }, 50);
}

// Update UI
function updateUI(cm, status) {
    const percent = (cm / tankHeight) * 100;
    animateWater(percent);

    document.getElementById("cm").innerText = cm.toFixed(1);

    const water = document.getElementById("water");
    const statusText = document.getElementById("statusText");

    water.className = "water"; // reset classes

    switch(status.toUpperCase()) {
        case "SAFE":
            water.classList.add("safe");
            statusText.innerText = "SAFE";
            statusText.style.color = "#4dff88";
            break;
        case "CAUTION":
            water.classList.add("caution");
            statusText.innerText = "CAUTION";
            statusText.style.color = "#ffd633";
            break;
        case "FULL":
            water.classList.add("full");
            statusText.innerText = "FULL";
            statusText.style.color = "#ff4d4d";
            break;
        default:
            water.classList.add("safe");
            statusText.innerText = status;
            statusText.style.color = "#ffffff";
    }
}

// Load latest water level from view.php (returns JSON)
function loadLevel() {
    fetch("view.php")
        .then(res => res.json())
        .then(data => {
            if (Array.isArray(data) && data.length > 0) {
                const latest = data[0];
                updateUI(parseFloat(latest.water_cm), latest.status);
            }
        })
        .catch(err => console.error("Error loading data:", err));
}

// Refresh every second
setInterval(loadLevel, 1000);
loadLevel();
</script>

</body>
</html>