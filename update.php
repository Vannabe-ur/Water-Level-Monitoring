<?php
// ================= DATABASE CONFIG =================
$host = "localhost";
$port = 3307;          
$user = "root";
$pass = "e20221650";
$db   = "water_monitor";

$conn = new mysqli($host, $user, $pass, $db, $port);
if ($conn->connect_error) {
    die("DB CONNECTION FAILED: " . $conn->connect_error);
}

// ================= GET DATA FROM ESP =================
// Instead of checking only cm and status, we accept all expected inputs
$cm = isset($_GET['cm']) ? floatval($_GET['cm']) : 94.0; // default to 94.0 if not provided
$status = isset($_GET['status']) ? strtoupper($_GET['status']) : 'CAUTION'; // default to CAUTION if not provided

// ================= REAL-TIME FILE =================
file_put_contents("level.txt", "Water CM: $cm | Status: $status\n", FILE_APPEND);

// ================= INSERT INTO DATABASE =================
$stmt = $conn->prepare(
    "INSERT INTO water_level (water_cm, status) VALUES (?, ?)"
);
$stmt->bind_param("ds", $cm, $status);
$stmt->execute();
$stmt->close();

// ================= FETCH ALL COLUMNS =================
$result = $conn->query("SELECT * FROM water_level ORDER BY create_at DESC LIMIT 20");
$data = [];
while ($row = $result->fetch_assoc()) {
    $data[] = $row; // each row contains id, water_cm, status, create_at
}

// Return JSON of last 20 entries
header('Content-Type: application/json');
echo json_encode($data);

$conn->close();
?>