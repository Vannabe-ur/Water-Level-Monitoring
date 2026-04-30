let waterChart = null;
const MAX_TANK_CM = 100;

function initChart() {
    const ctx = document.getElementById('waterChart').getContext('2d');
    waterChart = new Chart(ctx, {
        type: 'line',
        data: {
            labels: [],
            datasets: [{
                label: 'Water Level',
                data: [],
                borderColor: '#f97316',
                backgroundColor: 'rgba(249,115,22,0.08)',
                fill: true,
                tension: 0.3
            }]
        }
    });
}

function evaluateStatus(waterCm) {
    if (waterCm >= 90) return { text: "CRITICAL FULL", class: "status-critical", hint: "Overflow risk" };
    if (waterCm >= 75) return { text: "HIGH", class: "status-warning", hint: "Near capacity" };
    if (waterCm <= 20) return { text: "LOW", class: "status-critical", hint: "Refill needed" };
    return { text: "NOMINAL", class: "status-badge", hint: "Stable" };
}

async function fetchDataFromBackend() {
    try {
        const response = await fetch("view.php");
        const json = await response.json();
        console.log("API response:", json);
        return json.data || [];
    } catch (err) {
        console.error("Fetch error:", err);
        return [];
    }
}

async function loadData() {
    try {
        let data = await fetchDataFromBackend();
        if (!data.length) return;

        const latest = data[0];
        const waterCm = parseFloat(latest.water_cm);
        const fillPercent = (waterCm / MAX_TANK_CM) * 100;
        const status = evaluateStatus(waterCm);

        document.getElementById("current").innerHTML = waterCm + " cm";
        document.getElementById("status").innerHTML = status.text;
        document.getElementById("statusHint").innerHTML = status.hint;
        document.getElementById("water").style.height = fillPercent + "%";
        document.getElementById("fillPercent").innerText = fillPercent.toFixed(1) + "%";
        document.getElementById("levelCm").innerText = waterCm;

        let rows = "";
        let labels = [];
        let values = [];

        data.slice(0,10).forEach(row=>{
            rows += `
            <tr>
                <td>${row.water_cm} cm</td>
                <td>${row.status}</td>
                <td>${row.create_at}</td>
            </tr>`;
        });

        data.slice(0,20).reverse().forEach(row=>{
            labels.push(row.create_at);
            values.push(row.water_cm);
        });

        document.getElementById("tableBody").innerHTML = rows;

        waterChart.data.labels = labels;
        waterChart.data.datasets[0].data = values;
        waterChart.update();

    } catch(err){
        console.error(err);
    }
}

initChart();
loadData();
setInterval(loadData,3000);