<?php
// Database connection
$servername = "localhost";
$username   = "u997870193_db_jairpacheco";
$password   = "Jairpacheco6075";
$dbname     = "u997870193_jairpacheco";

$conn = new mysqli($servername, $username, $password, $dbname);
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// --- Potentiometer data ---
$sql = "SELECT timestamp, value FROM potentiometer_data ORDER BY timestamp ASC";
$result = $conn->query($sql);

$timestamps = [];
$values = [];

if ($result->num_rows > 0) {
    while($row = $result->fetch_assoc()) {
        $timestamps[] = $row["timestamp"];
        $values[] = (float)$row["value"]; // cast to float
    }
}

// --- LED state data ---
$sql_led = "SELECT timestamp, state FROM led_state ORDER BY timestamp ASC";
$result_led = $conn->query($sql_led);

$ledTimestamps = [];
$ledStates = [];

if ($result_led->num_rows > 0) {
    while($row = $result_led->fetch_assoc()) {
        $ledTimestamps[] = $row["timestamp"];
        $ledStates[] = (int)$row["state"]; // 0 or 1
    }
}

$conn->close();
?>

<!DOCTYPE html>
<html>
<head>
    <title>IoT Demo Dashboard</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
</head>
<body>
    <h2>Potentiometer Voltage Curve</h2>
    <canvas id="voltageChart"></canvas>

    

    <script>
    // Voltage chart
    const voltageCtx = document.getElementById('voltageChart').getContext('2d');
    const voltageChart = new Chart(voltageCtx, {
        type: 'line',
        data: {
            labels: <?php echo json_encode($timestamps); ?>,
            datasets: [{
                label: 'Voltage (V)',
                data: <?php echo json_encode($values); ?>,
                borderColor: 'blue',
                fill: false
            }]
        },
        options: {
            scales: {
                y: {
                    title: { display: true, text: 'Volts' }
                },
                x: {
                    title: { display: true, text: 'Timestamp' }
                }
            }
        }
    });

   
    </script>
</body>
</html>
