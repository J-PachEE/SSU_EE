<?php
// Save slider value to rgb.txt when form is submitted
if ($_SERVER["REQUEST_METHOD"] == "POST" && isset($_POST["rgb"])) {
    $value = intval($_POST["rgb"]);
    file_put_contents("rgb.txt", $value);
}

// Read current RGB value
$current = file_exists("rgb.txt") ? file_get_contents("rgb.txt") : "0";
?>

<!DOCTYPE html>
<html>
<head>
  <title>RGB Control</title>
</head>
<body>
  <h1>RGB LED Control</h1>
  <form method="POST">
    <label for="rgb">Red Intensity (0–255):</label><br>
    <input type="range" id="rgb" name="rgb" min="0" max="255" value="<?php echo $current; ?>" oninput="output.value = rgb.value">
    <output id="output"><?php echo $current; ?></output><br><br>
    <input type="submit" value="Set RGB">
  </form>
  <p>Current RGB Value: <?php echo $current; ?></p>
</body>
</html>
