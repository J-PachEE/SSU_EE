<?php
// Read current LED status
$ledStatus = file_exists("results.txt") ? trim(file_get_contents("results.txt")) : "UNKNOWN";
?>

<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <title>LED Control</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      text-align: center;
      margin-top: 50px;
    }
    button {
      font-size: 18px;
      padding: 10px 20px;
      margin: 10px;
      cursor: pointer;
    }
    #status {
      margin-top: 20px;
      font-size: 20px;
      color: <?php echo ($ledStatus === "ON") ? "green" : "red"; ?>;
    }
  </style>
</head>
<body>
  <h1>LED Control Panel</h1>
  <button onclick="setLED('ON')">Turn ON</button>
  <button onclick="setLED('OFF')">Turn OFF</button>

  <div id="status">
    Current LED Status: <strong><?php echo $ledStatus; ?></strong>
  </div>

  <script>
    function setLED(status) {
      fetch('https://jairpacheco.com/update.php', {
        method: 'PUT',
        headers: {
          'Content-Type': 'application/json'
        },
        body: JSON.stringify({ led: status })
      })
      .then(response => response.text())
      .then(data => {
        alert("Server response: " + data);
        location.reload(); // Refresh to show updated status
      })
      .catch(error => {
        alert("Error: " + error);
      });
    }
  </script>
</body>
</html>
