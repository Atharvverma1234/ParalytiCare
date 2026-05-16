#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

// ================= WIFI =================
const char* ssid = "Redmi_12_5G";
const char* password = "123@Abcd";

// ================= TELEGRAM =================
#define BOT_TOKEN "YOUR_BOT_TOKEN"
#define CHAT_ID "YOUR_CHAT_ID"

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

// ================= SERVER =================
ESP8266WebServer server(80);

// ================= DATA =================
float T = 0;
float F = 0;
float HR = 0;
float SPO2 = 0;
int M = 0;

String buffer = "";

bool alertSent = false;

// ================= PARSE FUNCTION =================
void parseData(String d) {

  int result = sscanf(d.c_str(),
                      "T:%f,F:%f,M:%d,HR:%f,SPO2:%f",
                      &T, &F, &M, &HR, &SPO2);

  if (result == 5) {
    Serial.println("Parsed OK");
  } else {
    Serial.println("Parse Error");
  }
}

// ================= TELEGRAM ALERT =================
void sendTelegramAlert() {

  String msg = "🚨 PARALYSIS PATIENT ALERT 🚨\n\n";

  msg += "⚠ Motion Detected!\n\n";
  msg += "🌡 Temperature: " + String(T) + " °C\n";
  msg += "🦾 Flex Value: " + String(F) + "\n";
  msg += "❤ Heart Rate: " + String(HR) + " bpm\n";
  msg += "🩸 SpO2: " + String(SPO2) + " %\n";

  bot.sendMessage(CHAT_ID, msg, "");

  Serial.println("Telegram Alert Sent");
}

// ================= SETUP =================
void setup() {

  Serial.begin(115200);
  delay(2000);

  Serial.println("\nBooting ESP8266...");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");

  int attempts = 0;

  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {

    Serial.println("\nConnected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    client.setInsecure();

    bot.sendMessage(CHAT_ID,
                    "✅ Paralysis Patient Monitoring System Connected",
                    "");

  } else {

    Serial.println("\nWiFi Connection Failed!");
  }

  // ================= DASHBOARD =================
  server.on("/", []() {

    String ip = WiFi.localIP().toString();

    String html = R"====(
<!DOCTYPE html>
<html>

<head>
<meta name="viewport" content="width=device-width, initial-scale=1">

<title>Health Dashboard</title>

<style>

body{
  font-family:'Segoe UI',sans-serif;
  background:linear-gradient(135deg,#0f172a,#1e293b);
  color:white;
  margin:0;
  text-align:center;
}

h1{
  margin-top:20px;
}

.ip{
  background:rgba(0,255,150,0.1);
  padding:10px;
  width:fit-content;
  margin:auto;
  margin-bottom:20px;
  border-radius:10px;
  color:#00ffa6;
}

.container{
  display:flex;
  flex-wrap:wrap;
  justify-content:center;
}

.card{
  background:rgba(255,255,255,0.05);
  width:170px;
  margin:10px;
  padding:20px;
  border-radius:20px;
  box-shadow:0 0 10px rgba(0,0,0,0.5);
  transition:0.3s;
}

.card:hover{
  transform:scale(1.05);
}

.label{
  opacity:0.7;
}

.value{
  font-size:24px;
  margin-top:10px;
}

.alert{
  background:red;
  padding:15px;
  margin:20px;
  border-radius:10px;
  display:none;
  animation:blink 1s infinite;
}

@keyframes blink{
  0%{opacity:1;}
  50%{opacity:0.5;}
  100%{opacity:1;}
}

</style>
</head>

<body>

<h1>🏥 Health Dashboard</h1>

<div class="ip">
Device IP: )====" + ip + R"====(
</div>

<div class="container">

<div class="card">
<div class="label">Temperature</div>
<div id="T" class="value">--</div>
</div>

<div class="card">
<div class="label">Flex</div>
<div id="F" class="value">--</div>
</div>

<div class="card">
<div class="label">Motion</div>
<div id="M" class="value">--</div>
</div>

<div class="card">
<div class="label">Heart Rate</div>
<div id="HR" class="value">--</div>
</div>

<div class="card">
<div class="label">SpO2</div>
<div id="SPO2" class="value">--</div>
</div>

</div>

<div id="alertBox" class="alert">
🚨 MOTION DETECTED 🚨
</div>

<audio id="alarmSound">
<source src="https://actions.google.com/sounds/v1/alarms/alarm_clock.ogg">
</audio>

<script>

function update(){

fetch('/data')
.then(response => response.json())
.then(data => {

document.getElementById("T").innerHTML =
data.T + " °C";

document.getElementById("F").innerHTML =
data.F;

document.getElementById("M").innerHTML =
data.M ? "Moving" : "Still";

document.getElementById("HR").innerHTML =
data.HR + " bpm";

document.getElementById("SPO2").innerHTML =
data.SPO2 + " %";

if(data.M == 1){

document.getElementById("alertBox").style.display = "block";

document.getElementById("alarmSound").play();

}else{

document.getElementById("alertBox").style.display = "none";

}

});

}

setInterval(update,1000);

</script>

</body>
</html>
)====";

    server.send(200, "text/html", html);
  });

  // ================= JSON API =================
  server.on("/data", []() {

    String json = "{";

    json += "\"T\":" + String(T) + ",";
    json += "\"F\":" + String(F) + ",";
    json += "\"M\":" + String(M) + ",";
    json += "\"HR\":" + String(HR) + ",";
    json += "\"SPO2\":" + String(SPO2);

    json += "}";

    server.send(200, "application/json", json);
  });

  server.begin();

  Serial.println("HTTP Server Started");
}

// ================= LOOP =================
void loop() {

  server.handleClient();

  while (Serial.available()) {

    char c = Serial.read();

    if (c == '\n') {

      parseData(buffer);

      // ================= TELEGRAM ALERT CONDITION =================
      if (M == 1 && !alertSent) {

        sendTelegramAlert();

        alertSent = true;
      }

      if (M == 0) {

        alertSent = false;
      }

      buffer = "";

    } else {

      buffer += c;
    }
  }
}
