#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

// =========================
// L298N PIN CONFIGURATION
// =========================

#define IN1 13
#define IN2 12
#define IN3 14
#define IN4 27

#define ENA 26
#define ENB 25

// =========================
// WIFI
// =========================

// Replace these with YOUR router's WiFi credentials
const char* ssid = "Room-601";
const char* password = "room601601";

WebServer server(80);

// Motor speed: 0 - 255
int speedValue = 200;


// =========================
// MOTOR FUNCTIONS
// =========================

void stopCar()
{
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);

    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);

    analogWrite(ENA, 0);
    analogWrite(ENB, 0);
}


void forward()
{
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);

    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);

    analogWrite(ENA, speedValue);
    analogWrite(ENB, speedValue);
}


void backward()
{
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);

    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);

    analogWrite(ENA, speedValue);
    analogWrite(ENB, speedValue);
}


void left()
{
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);

    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);

    analogWrite(ENA, speedValue);
    analogWrite(ENB, speedValue);
}


void right()
{
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);

    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);

    analogWrite(ENA, speedValue);
    analogWrite(ENB, speedValue);
}


// =========================
// WEB PAGE
// =========================

String webpage()
{
    String html = R"rawliteral(

<!DOCTYPE html>
<html>

<head>

<meta name="viewport"
content="width=device-width, initial-scale=1">

<title>ESP32 RC Car</title>

<style>

* {
    box-sizing: border-box;
}

body {
    background: #111;
    color: white;
    font-family: Arial, sans-serif;
    text-align: center;
    margin: 0;
    padding: 20px;
}

h1 {
    margin: 10px 0 25px;
    font-size: 28px;
}

/* D-pad container: 3x3 grid */
.dpad {
    display: grid;
    grid-template-columns: repeat(3, 80px);
    grid-template-rows: repeat(3, 80px);
    gap: 8px;
    justify-content: center;
    margin-bottom: 25px;
}

/* Base button style */
button {
    width: 80px;
    height: 80px;

    display: flex;
    align-items: center;
    justify-content: center;

    border: none;
    border-radius: 50%;

    background: #333;
    color: white;

    cursor: pointer;
    touch-action: none;  /* prevent scroll while pressing */
    user-select: none;
    -webkit-user-select: none;
}

button:active {
    background: #666;
    transform: scale(0.92);
}

/* Position each button in the grid */
.up    { grid-area: 1 / 2; }
.left  { grid-area: 2 / 1; }
.stop  { grid-area: 2 / 2; }
.right { grid-area: 2 / 3; }
.down  { grid-area: 3 / 2; }

/* Stop button styling */
button.stop {
    background: #b00020;
}

button.stop:active {
    background: #ff1744;
}

/* Arrow icons drawn with CSS borders (no fonts needed) */
button .arrow {
    display: inline-block;
    width: 0;
    height: 0;
}

/* Up arrow: triangle pointing up */
.arrow.up {
    border-left: 16px solid transparent;
    border-right: 16px solid transparent;
    border-bottom: 26px solid #fff;
}

/* Down arrow: triangle pointing down */
.arrow.down {
    border-left: 16px solid transparent;
    border-right: 16px solid transparent;
    border-top: 26px solid #fff;
}

/* Left arrow: triangle pointing left */
.arrow.left {
    border-top: 16px solid transparent;
    border-bottom: 16px solid transparent;
    border-right: 26px solid #fff;
}

/* Right arrow: triangle pointing right */
.arrow.right {
    border-top: 16px solid transparent;
    border-bottom: 16px solid transparent;
    border-left: 26px solid #fff;
}

/* Stop: a filled square */
.arrow.stop {
    width: 26px;
    height: 26px;
    background: #fff;
    border: none;
}

/* Speed slider */
.speed-section {
    margin-top: 20px;
}

.speed-section h3 {
    margin-bottom: 10px;
}

input[type="range"] {
    width: 250px;
}

.speed-value {
    display: block;
    margin-top: 10px;
    font-size: 22px;
    color: #4caf50;
}

</style>

</head>

<body>

<h1>ESP32 RC Car</h1>

<div class="dpad">

    <button class="up"
    ontouchstart="move('right')"
    ontouchend="move('stop')"
    onmousedown="move('right')"
    onmouseup="move('stop')">
    <span class="arrow up"></span>
    </button>

    <button class="left"
    ontouchstart="move('backward')"
    ontouchend="move('stop')"
    onmousedown="move('backward')"
    onmouseup="move('stop')">
    <span class="arrow left"></span>
    </button>

    <button class="stop"
    onclick="move('stop')">
    <span class="arrow stop"></span>
    </button>

    <button class="right"
    ontouchstart="move('forward')"
    ontouchend="move('stop')"
    onmousedown="move('forward')"
    onmouseup="move('stop')">
    <span class="arrow right"></span>
    </button>

    <button class="down"
    ontouchstart="move('left')"
    ontouchend="move('stop')"
    onmousedown="move('left')"
    onmouseup="move('stop')">
    <span class="arrow down"></span>
    </button>

</div>

<div class="speed-section">

    <h3>Speed</h3>

    <input
    type="range"
    id="speedSlider"
    min="0"
    max="255"
    value="200"
    oninput="setSpeed(this.value)">

    <span class="speed-value" id="speedLabel">200</span>

</div>

<script>

function move(direction)
{
    fetch("/" + direction);
}

function setSpeed(value)
{
    document.getElementById("speedLabel").textContent = value;
    fetch("/speed?value=" + value);
}

</script>

</body>

</html>

)rawliteral";

    return html;
}


// =========================
// SETUP
// =========================

void setup()
{
    Serial.begin(115200);

    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);

    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);

    pinMode(ENA, OUTPUT);
    pinMode(ENB, OUTPUT);

    stopCar();

    // Connect to your router's WiFi (station mode)
    Serial.println();
    Serial.println("================================");
    Serial.println("ESP32 RC CAR");
    Serial.println("================================");

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    Serial.print("Connecting to WiFi: ");
    Serial.println(ssid);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("WiFi connected!");

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());


    // Web routes

    server.on("/", []()
    {
        server.send(200, "text/html", webpage());
    });


    server.on("/forward", []()
    {
        forward();
        server.send(200, "text/plain", "FORWARD");
    });


    server.on("/backward", []()
    {
        backward();
        server.send(200, "text/plain", "BACKWARD");
    });


    server.on("/left", []()
    {
        left();
        server.send(200, "text/plain", "LEFT");
    });


    server.on("/right", []()
    {
        right();
        server.send(200, "text/plain", "RIGHT");
    });


    server.on("/stop", []()
    {
        stopCar();
        server.send(200, "text/plain", "STOP");
    });


    server.on("/speed", []()
    {
        if (server.hasArg("value"))
        {
            speedValue = server.arg("value").toInt();

            speedValue = constrain(speedValue, 0, 255);

            Serial.print("Speed: ");
            Serial.println(speedValue);
        }

        server.send(200, "text/plain", "OK");
    });


    server.begin();

    Serial.println("Web server started!");
}


// =========================
// LOOP
// =========================

void loop()
{
    server.handleClient();
}