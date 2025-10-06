<?php
ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);

$servername = "mysql";  // Nome do serviço MySQL no docker-compose
$username = "root";
$password = "";         // Senha vazia, como no XAMPP
$dbname = "esp32_data";

// Tenta conectar ao MySQL com retry
$max_attempts = 5;
$attempt = 1;

while ($attempt <= $max_attempts) {
    try {
        $conn = new mysqli($servername, $username, $password, $dbname);
        if ($conn->connect_error) {
            throw new Exception("Connection failed: " . $conn->connect_error);
        }
        break; // Conexão bem-sucedida, sai do loop
    } catch (Exception $e) {
        if ($attempt == $max_attempts) {
            die("Erro após $max_attempts tentativas: " . $e->getMessage());
        }
        sleep(2); // Espera 2 segundos antes de tentar novamente
        $attempt++;
    }
}

// Verifica se os dados POST foram enviados
if ($_SERVER['REQUEST_METHOD'] !== 'POST' || !isset($_POST['temperature']) || !isset($_POST['humidity'])) {
    die("Erro: Requisição inválida ou dados de temperatura e umidade não recebidos.");
}

// Prepara a query para evitar SQL Injection
$stmt = $conn->prepare("INSERT INTO sensor_readings (temperature, humidity) VALUES (?, ?)");
if ($stmt === false) {
    die("Prepare failed: " . $conn->error);
}

// Recebe os dados do ESP32 e converte para float
$temperature = floatval($_POST['temperature']);
$humidity = floatval($_POST['humidity']);

// Vincula os parâmetros e executa
$stmt->bind_param("dd", $temperature, $humidity);
if ($stmt->execute()) {
    echo "Dados salvos com sucesso!";
} else {
    echo "Erro ao salvar: " . $stmt->error;
}

// Fecha a conexão
$stmt->close();
$conn->close();
?>
