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
 * Fetch latest 20 water level records
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
    "data" => $data
]);

$conn->close();
?>