<?php
header('Content-Type: application/json'); // must be first line

$host = "localhost";
$port = 5432;
$user = "root";
$pass = "root";
$db   = "myapp";

$conn = new mysqli($host, $user, $pass, $db, $port);
if ($conn->connect_error) die(json_encode(["error"=>"DB CONNECTION FAILED"]));

$result = $conn->query("SELECT * FROM water_level ORDER BY create_at DESC LIMIT 20");

$data = [];
while($row = $result->fetch_assoc()) {
    $data[] = $row;
}

echo json_encode($data);

$conn->close();
?>