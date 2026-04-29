<?php
header('Content-Type: application/json');

/**
 * Database configuration
 */
$host = "db";
$port = 3306;
$user = "root";
$pass = "root";
$db   = "myapp";

/**
 * Connect to database
 */
$conn = new mysqli($host, $user, $pass, $db, $port);

if ($conn->connect_error) {
    die(json_encode([
        "error" => "DB CONNECTION FAILED",
        "message" => $conn->connect_error
    ]));
}

/**
 * Read ESP values
 */
$cm = isset($_GET['cm']) ? floatval($_GET['cm']) : 94.0;
$status = isset($_GET['status']) ? strtoupper($_GET['status']) : 'CAUTION';

/**
 * Log to file
 */
$log_file = __DIR__ . "/level.txt";
$log_entry = "Water CM: $cm | Status: $status\n";

if (file_put_contents($log_file, $log_entry, FILE_APPEND | LOCK_EX) === false) {
    error_log("Failed to write to log file: $log_file");
}

/**
 * Insert into database
 */
$stmt = $conn->prepare("INSERT INTO water_level (water_cm, status) VALUES (?, ?)");

if (!$stmt) {
    die(json_encode([
        "error" => "PREPARE FAILED",
        "message" => $conn->error
    ]));
}

$stmt->bind_param("ds", $cm, $status);

if (!$stmt->execute()) {
    die(json_encode([
        "error" => "INSERT FAILED",
        "message" => $stmt->error
    ]));
}

$stmt->close();

/**
 * Fetch latest 20 rows
 */
$result = $conn->query("
    SELECT * 
    FROM water_level 
    ORDER BY create_at DESC 
    LIMIT 20
");

if (!$result) {
    die(json_encode([
        "error" => "QUERY FAILED",
        "message" => $conn->error
    ]));
}

$data = [];

while ($row = $result->fetch_assoc()) {
    $data[] = $row;
}

/**
 * Return JSON response
 */
echo json_encode([
    "success" => true,
    "latest" => $data
]);

$conn->close();
?>