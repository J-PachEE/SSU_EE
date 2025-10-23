<?php
// Enable error reporting
ini_set('display_errors', 1);
error_reporting(E_ALL);

// Database connection
$host = '**********';
$dbname = '***************';
$username = '***************';
$password = '*****************';

$conn = new mysqli($host, $username, $password, $dbname);
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// Extract and decode raw BASE64 query string
$params = [];
$query = $_SERVER['QUERY_STRING'] ?? '';

function isBase64($string) {
    $decoded = base64_decode($string, true);
    return $decoded !== false && base64_encode($decoded) === $string;
}

if (!empty($query) && isBase64($query)) {
    $decoded = urldecode(base64_decode($query));
    parse_str($decoded, $params);
} else {
    parse_str($query, $params); // fallback to plain query string
}

// Extract parameters
$nodeName = $params['node_name'] ?? null;
$temperature = $params['temperature'] ?? null;
$humidity = $params['humidity'] ?? null;
$timeReceived = $params['time_received'] ?? null;

// Set timezone and fallback time
date_default_timezone_set('America/Los_Angeles');
if (!$timeReceived) {
    $timeReceived = date("Y-m-d H:i:s");
}

// Optional debug
// echo "<pre>"; print_r($params); echo "</pre>";

// Validate required fields
if (!$nodeName || $temperature === null || $humidity === null) {
    die("Missing required parameters.");
}
if (strlen($nodeName) > 10) {
    die("Node name exceeds 10 characters.");
}
if (!is_numeric($temperature) || $temperature < -10 || $temperature > 100 || strlen((string)$temperature) > 6) {
    die("Invalid temperature. Must be -10 to 100°C.");
}
if (!is_numeric($humidity) || $humidity < 0 || $humidity > 100 || strlen((string)$humidity) > 6) {
    die("Invalid humidity. Must be 0 to 100%.");
}

// Check if node is registered
$checkNode = $conn->prepare("SELECT node_name FROM sensor_register WHERE node_name = ?");
$checkNode->bind_param("s", $nodeName);
$checkNode->execute();
$checkNode->store_result();
if ($checkNode->num_rows === 0) {
    die("Node not registered.");
}
$checkNode->close();

// Check for duplicate entry
$checkDuplicate = $conn->prepare("SELECT 1 FROM sensor_data WHERE node_name = ? AND time_received = ?");
$checkDuplicate->bind_param("ss", $nodeName, $timeReceived);
$checkDuplicate->execute();
$checkDuplicate->store_result();
if ($checkDuplicate->num_rows > 0) {
    die("Duplicate entry for this node and timestamp.");
}
$checkDuplicate->close();

// Insert data
$insert = $conn->prepare("INSERT INTO sensor_data (node_name, time_received, temperature, humidity) VALUES (?, ?, ?, ?)");
$insert->bind_param("ssii", $nodeName, $timeReceived, $temperature, $humidity);

if ($insert->execute()) {
    echo "Data inserted successfully: $nodeName at $timeReceived";
} else {
    echo "Insert failed: " . $conn->error;
}

$insert->close();
$conn->close();
?>
