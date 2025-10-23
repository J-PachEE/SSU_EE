<?php
// Enable error reporting
ini_set('display_errors', 1);
error_reporting(E_ALL);

// Database connection
$host = '*********';
$dbname = '*******************';
$username = '*******************';
$password = '*****************';

$conn = new mysqli($host, $username, $password, $dbname);
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// Extract incoming parameters (if used for insert)
$nodeId = $_GET['node_name'] ?? $_GET['nodeId'] ?? null;
$nodeTemp = $_GET['temperature'] ?? $_GET['nodeTemp'] ?? null;
$humidity = $_GET['humidity'] ?? null;
$timeReceived = $_GET['time_received'] ?? $_GET['timeReceived'] ?? date("Y-m-d H:i:s");

// Validate inputs
$validNode = isset($nodeId) && strlen($nodeId) <= 10;
$validTemp = is_numeric($nodeTemp) && $nodeTemp >= -10 && $nodeTemp <= 100;
$validHumidity = is_numeric($humidity) && $humidity >= 0 && $humidity <= 100;

// Check if node is registered
$checkNode = $conn->prepare("SELECT node_name FROM sensor_register WHERE node_name = ?");
$checkNode->bind_param("s", $nodeId);
$checkNode->execute();
$checkNode->store_result();
$nodeExists = $checkNode->num_rows > 0;
$checkNode->close();

// Insert if valid
if ($validNode && $validTemp && $validHumidity && $nodeExists) {
    $stmt = $conn->prepare("INSERT INTO sensor_data (node_name, time_received, temperature, humidity) VALUES (?, ?, ?, ?)");
    $stmt->bind_param("ssii", $nodeId, $timeReceived, $nodeTemp, $humidity);
    if ($stmt->execute()) {
        echo "<p style='color:green;'>✅ Data inserted: $nodeId, $nodeTemp°C, $humidity%, $timeReceived</p>";
    } else {
        echo "<p style='color:red;'>❌ Insert failed: " . $stmt->error . "</p>";
    }
    $stmt->close();
} elseif ($nodeId || $nodeTemp || $humidity) {
    echo "<p style='color:red;'>❌ Invalid input:<br>";
    if (!$validNode) echo "- Missing or invalid node name.<br>";
    if (!$validTemp) echo "- Temperature must be -10–100°C.<br>";
    if (!$validHumidity) echo "- Humidity must be 0–100%.<br>";
    if (!$nodeExists) echo "- Node not registered.<br>";
    echo "</p>";
}

// Queries
$sensorDataQuery = "SELECT node_name, time_received, temperature, humidity FROM sensor_data ORDER BY node_name ASC, time_received ASC";
$sensorRegisterQuery = "SELECT node_name, manufacturer, longitude, latitude FROM sensor_register ORDER BY node_name ASC";

$sensorDataResult = $conn->query($sensorDataQuery);
$sensorRegisterResult = $conn->query($sensorRegisterQuery);

// Chart data for node-1
$nodeToPlot = 'node-1';
$plotQuery = "SELECT time_received, temperature FROM sensor_data WHERE node_name = ? ORDER BY time_received ASC";
$plotStmt = $conn->prepare($plotQuery);
$plotStmt->bind_param("s", $nodeToPlot);
$plotStmt->execute();
$plotResult = $plotStmt->get_result();

$timestamps = [];
$celsius = [];

while ($row = $plotResult->fetch_assoc()) {
    $timestamps[] = $row['time_received'];
    $celsius[] = $row['temperature'];
}
?>
<!DOCTYPE html>
<html>
<head>
    <title>Sensor Overview</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <style>
        h1 { color: #3b5e34; }
        body {
            font-family: Arial, sans-serif;
            background-color: #eef2f5;
            padding: 20px;
        }
        h2 { color: #3b5e34; }
        table {
            border-collapse: collapse;
            width: 100%;
            margin-bottom: 40px;
        }
        th, td {
            border: 1px solid #bbb;
            padding: 10px;
            text-align: left;
        }
        th {
            background-color: #345e3e;
            color: white;
        }
        tr:nth-child(even) {
            background-color: #f9fcff;
        }
    </style>
</head>
<body>
<h1>Jair's Sensor Data Tables</h1>

<h2>Sensor Data Table</h2>
<table>
    <tr>
        <th>Node Name</th>
        <th>Time Received</th>
        <th>Temperature (°C)</th>
        <th>Humidity (%)</th>
    </tr>
    <?php
    if ($sensorDataResult->num_rows > 0) {
        while ($row = $sensorDataResult->fetch_assoc()) {
            echo "<tr>";
            echo "<td>" . htmlspecialchars($row['node_name']) . "</td>";
            echo "<td>" . htmlspecialchars($row['time_received']) . "</td>";
            echo "<td>" . htmlspecialchars($row['temperature']) . "</td>";
            echo "<td>" . htmlspecialchars($row['humidity']) . "</td>";
            echo "</tr>";
        }
    } else {
        echo "<tr><td colspan='4'>No sensor data found.</td></tr>";
    }
    ?>
</table>

<?php
$tempquery = "SELECT AVG(temperature) AS avg_temp FROM sensor_data WHERE node_name = ?";
$tempstmt = $conn->prepare($tempquery);
$tempstmt->bind_param("s", $nodeToPlot);
$tempstmt->execute();
$tempresult = $tempstmt->get_result();
if ($tempresult->num_rows > 0) {
    $row = $tempresult->fetch_assoc();
    echo "Average temperature ($nodeToPlot): " . round($row['avg_temp'], 2) . " °C<br>";
}
$tempstmt->close();

$humquery = "SELECT AVG(humidity) AS avg_hum FROM sensor_data WHERE node_name = ?";
$humstmt = $conn->prepare($humquery);
$humstmt->bind_param("s", $nodeToPlot);
$humstmt->execute();
$humresult = $humstmt->get_result();
if ($humresult->num_rows > 0) {
    $row = $humresult->fetch_assoc();
    echo "Average humidity ($nodeToPlot): " . round($row['avg_hum'], 2) . " %<br>";
}
$humstmt->close();
?>

<h2>Sensor Register Table</h2>
<table>
    <tr>
        <th>Node Name</th>
        <th>Manufacturer</th>
        <th>Longitude</th>
        <th>Latitude</th>
    </tr>
    <?php
    if ($sensorRegisterResult->num_rows > 0) {
        while ($row = $sensorRegisterResult->fetch_assoc()) {
            echo "<tr>";
            echo "<td>" . htmlspecialchars($row['node_name']) . "</td>";
            echo "<td>" . htmlspecialchars($row['manufacturer']) . "</td>";
            echo "<td>" . htmlspecialchars($row['longitude']) . "</td>";
            echo "<td>" . htmlspecialchars($row['latitude']) . "</td>";
            echo "</tr>";
        }
    } else {
        echo "<tr><td colspan='4'>No sensor register data found.</td></tr>";
    }
    ?>
</table>

<h2>Sensor Node <?php echo $nodeToPlot; ?> Temperature Plot (°C)</h2>
<canvas id="sensorChart" width="800" height="400"></canvas>

<script>
const labels = <?php echo json_encode($timestamps); ?>;
const data = {
    labels: labels,
    datasets: [{
        label: 'Temperature (°C)',
        data: <?php echo json_encode($celsius); ?>,
        backgroundColor: 'rgba(22, 255, 69, 0.87)',
        borderColor: 'rgba(22, 255, 69, 0.87)',
        borderWidth: 1
    }]
};

const config = {
    type: 'bar',
    data: data,
    options: {
        responsive: true,
        plugins: {
            title: {
                display: true,
                text: 'Sensor Node <?php echo $nodeToPlot; ?>'
            }
        },
        scales: {
            x: {
                title: {
                    display: true,
                    text: 'Time'
                }
            },
            y: {
                title: {
                    display: true,
                    text: 'Temperature (°C)'
                },
                suggestedMin: -10,
                suggestedMax: 100
            }
        }
    }
};

new Chart(document.getElementById('sensorChart'), config);
</script>

</body>
</html>

<?php
$conn->close();
?>
