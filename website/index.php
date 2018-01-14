
<?php
$servername = "localhost";
$username = "iot_read";
$password = "LetMeRead";
$dbname = "IoT_project";
     
$conn = new mysqli($servername, $username, $password, $dbname);
if ($conn->connect_error) {
	die("connection failed: " . @$conn->connect_error);
	}

$sql = "SELECT * FROM IoT_project.Corridor ORDER BY id DESC LIMIT 50";
$result = $conn->query($sql);

if ($result->num_rows > 0){
	while($row = $result->fetch_assoc()){
		echo "id: " . $row["id"]. " - Event " . $row["event"]. " " . $row["time"]. "<br>";
	}
} else {
	echo "0 results";
}
$conn->close();
?>
