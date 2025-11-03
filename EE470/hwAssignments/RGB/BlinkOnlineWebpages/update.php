<?php

$json = file_get_contents("php://input");


if (json_decode($json) === null) {
    http_response_code(400);
    echo "Invalid JSON";
    exit;
}


file_put_contents("results.txt", $json);
echo "Saved: " . $json;
?>
