#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// ===== WIFI =====
const char* ssid = "";
const char* password = "";

// ===== SERVER =====
ESP8266WebServer server(80);

// ===== DATA =====
float T=0, F=0, HR=0, SPO2=0;
int M=0;

String buffer = "";

// ===== PARSE FUNCTION =====
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

// ===== SETUP =====
void setup() {
  Serial.begin(115200);
  delay(2000);

  Serial.println("\nBooting ESP...");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connecting");

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
  } else {
    Serial.println("\nWiFi Failed!");
  }

  // ===== DASHBOARD =====
  server.on("/", [](){

    String ip = WiFi.localIP().toString();

    String html = R"====(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Health Monitor</title>

<style>
body {
  font-family: 'Segoe UI', sans-serif;
  background: linear-gradient(135deg,#0f172a,#1e293b);
  color:white;
  text-align:center;
  margin:0;
}

h1 { margin:20px; }

.ip {
  background: rgba(0,255,150,0.1);
  padding:10px;
  margin:10px auto;
  width:fit-content;
  border-radius:10px;
  font-size:14px;
  color:#00ffa6;
}

.container {
  display:flex;
  flex-wrap:wrap;
  justify-content:center;
}

.card {
  background: rgba(255,255,255,0.05);
  padding:20px;
  margin:10px;
  border-radius:20px;
  width:150px;
  box-shadow:0 0 10px rgba(0,0,0,0.5);
  transition:0.3s;
}

.card:hover { transform:scale(1.05); }

.label { opacity:0.7; }
.value { font-size:24px; margin-top:10px; }

.alert {
  background:red;
  padding:15px;
  margin:20px;
  border-radius:10px;
  display:none;
  animation:blink 1s infinite;
}

@keyframes blink {
  0%{opacity:1;}50%{opacity:0.5;}100%{opacity:1;}
}
</style>
</head>

<body>

<h1> Health Dashboard</h1>

<div class="ip"> Device IP: )====" + ip + R"====(</div>

<div class="container">
  <div class="card"><div class="label">Temp</div><div id="T" class="value">--</div></div>
  <div class="card"><div class="label">Flex</div><div id="F" class="value">--</div></div>
  <div class="card"><div class="label">Motion</div><div id="M" class="value">--</div></div>
  <div class="card"><div class="label">Heart Rate</div><div id="HR" class="value">--</div></div>
  <div class="card"><div class="label">SpO2</div><div id="SPO2" class="value">--</div></div>
</div>

<div id="alertBox" class="alert"> MOTION DETECTED!</div>

<audio id="alarmSound">
<source src="https://actions.google.com/sounds/v1/alarms/alarm_clock.ogg">
</audio>

<script>
function update(){
 fetch('/data')
 .then(r=>r.json())
 .then(d=>{

  document.getElementById("T").innerHTML = d.T + "°C";
  document.getElementById("F").innerHTML = d.F;
  document.getElementById("M").innerHTML = d.M ? "Moving" : "Still";
  document.getElementById("HR").innerHTML = d.HR + " bpm";
  document.getElementById("SPO2").innerHTML = d.SPO2 + "%";

  if(d.M==1){
    document.getElementById("alertBox").style.display="block";
    document.getElementById("alarmSound").play();
  } else {
    document.getElementById("alertBox").style.display="none";
  }

 });
}
setInterval(update,1000);
</script>

</body>
</html>
)====";

    server.send(200,"text/html",html);
  });

  // ===== DATA API =====
  server.on("/data", [](){
    String json = "{";
    json += "\"T\":" + String(T) + ",";
    json += "\"F\":" + String(F) + ",";
    json += "\"M\":" + String(M) + ",";
    json += "\"HR\":" + String(HR) + ",";
    json += "\"SPO2\":" + String(SPO2);
    json += "}";

    server.send(200,"application/json",json);
  });

  server.begin();
  Serial.println("HTTP Server Started");
}

// ===== LOOP =====
void loop() {
  server.handleClient();

  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n') {
      parseData(buffer);
      buffer = "";
    } else {
      buffer += c;
    }
  }
}
