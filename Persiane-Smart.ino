/*-------------------------------------------------------------
 15/01/2020
 Autore: Marco
 Piattaforma: ESP8266
 Linguaggio: C++ e Arduino
 File Persiane_Smart.ino
 -----------------------------------------------------------
 Descrizione:
 Codice da utilizzare per automatizzare le persiane gestisce due motori DC e 4 micro, tramite la motor shield dedicata alla NodeMCU,
 il video dove trovare tutti i riferimenti è su youtube al link: https://www.youtube.com/watch?v=GMPM3Aij41A&t=39s
 ------------------------------------------------------------------------------
 Licenza:
 Per cortesia guarda il file LICENSE.txt per dettagli.
 --------------------------------------------------------*/
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

ESP8266WebServer server(80);
#define LEDBLU  16  //D0 
#define MOTOR_A  5  //D1
#define MOTOR_B  4  //D2
#define DIRMOTOR_A  0  //D3
#define DIRMOTOR_B  2 //D4
#define MICRO_A_OPEN  14  //D5
#define MICRO_A_CLOSE 12  //D6
#define MICRO_B_OPEN 13  //D7
#define MICRO_B_CLOSE 15  //D8

MDNSResponder mdns;

char* ssid = "dammi i soldi";
char* password = "marcok625";

char webpage[] PROGMEM = R"=====(
<html>
<head>
</head>
<body>
<p> Apertura Ante: <span id="ante-state">__</span> </p>
<button onclick="myFunction()"> TOGGLE </button>
</body>
<script>
function myFunction()
{
  console.log("button was clicked!");
  var xhr = new XMLHttpRequest();
  var url = "/antestate";
  xhr.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("ante-state").innerHTML = this.responseText;
    }
  };
  xhr.open("GET", url, true);
  xhr.send();
};
document.addEventListener('DOMContentLoaded', myFunction, false);
</script>
</html>
)=====";

void setup()
{ 
  WiFi.begin(ssid,password);
  Serial.begin(115200);
  while(WiFi.status()!=WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  if (MDNS.begin("persianesmart", WiFi.localIP()))   // http://persianesmart.local/
    Serial.println("MDNS responder started");
  
   
  server.on("/",[](){server.send_P(200,"text/html", webpage);});
  server.on("/antestate",getLEDState);
  server.begin();

  MDNS.addService("http", "tcp", 80);
  pinMode ( LEDBLU, OUTPUT);
  pinMode ( MOTOR_A, OUTPUT);
  pinMode ( MOTOR_B, OUTPUT);
  pinMode ( DIRMOTOR_A, OUTPUT);
  pinMode ( DIRMOTOR_B, OUTPUT);
  pinMode ( MICRO_A_OPEN, INPUT);
  pinMode ( MICRO_A_CLOSE, INPUT);
  pinMode ( MICRO_B_OPEN, INPUT);
  pinMode ( MICRO_B_CLOSE, INPUT);
  digitalWrite( MICRO_A_OPEN, LOW);
  digitalWrite( MICRO_A_CLOSE, LOW);
  digitalWrite( MICRO_B_OPEN, LOW);
  digitalWrite( MICRO_B_CLOSE, LOW);
  digitalWrite( MOTOR_A, LOW);
  digitalWrite( MOTOR_B, LOW);
  digitalWrite( DIRMOTOR_A, LOW);
  digitalWrite( DIRMOTOR_B, LOW);
  digitalWrite( LEDBLU, LOW);
  
}
//GESTIONE ANOMALIE nell' loop
void loop(){  
  server.handleClient();
  MDNS.update();
  
  if (digitalRead(MOTOR_A)==HIGH && digitalRead(MICRO_A_CLOSE)==HIGH)
   blokA();

  if (digitalRead(MOTOR_A)==HIGH && digitalRead(MICRO_A_OPEN)==HIGH)
   blokA();

  if (digitalRead(MOTOR_B)==HIGH && digitalRead(MICRO_B_OPEN)==HIGH)
   blokB(); 

  if (digitalRead(MOTOR_B)==HIGH && digitalRead(MICRO_B_CLOSE) ==HIGH)
   blokB(); 
     
   if (digitalRead(MOTOR_B)==LOW && digitalRead(MOTOR_A)==LOW && digitalRead(MICRO_B_OPEN) ==HIGH && digitalRead(MICRO_A_OPEN) ==LOW && digitalRead(MICRO_A_CLOSE) ==HIGH)
   antechiudiB();
    } 

void toggleLED(){
  if (digitalRead(MOTOR_A)==LOW && digitalRead(MOTOR_B)==LOW && digitalRead(MICRO_A_CLOSE)==HIGH && digitalRead(MICRO_B_CLOSE)==HIGH) 
    anteapriB();
   
  else if (digitalRead(MOTOR_A)==LOW && digitalRead(MOTOR_B)==LOW && digitalRead(MICRO_A_OPEN)==HIGH && digitalRead(MICRO_B_OPEN)==HIGH) 
    antechiudiA();           
}

void getLEDState(){
  toggleLED();
  String Apertura_Ante = digitalRead(MICRO_A_OPEN) ? "APERTE" : "CHIUSE";
  server.send(200,"text/plain", Apertura_Ante);
}

void anteapriB(){
  bool Apri = true;
  while(Apri) {
    if(digitalRead(MICRO_B_OPEN)==HIGH){
      Apri = false;
      }
    digitalWrite(DIRMOTOR_B, HIGH);
    digitalWrite(MOTOR_B, HIGH);
    delay(500); 
   }
   digitalWrite(LEDBLU, LOW);
   digitalWrite(MOTOR_B, LOW);
   digitalWrite(DIRMOTOR_B, LOW);
   anteapriA();
 }

void anteapriA(){
  bool ApriA = true;
  while(ApriA) {
    if(digitalRead(MICRO_A_OPEN)==HIGH){
      ApriA = false;
      }
    digitalWrite(DIRMOTOR_A, LOW);
    digitalWrite(MOTOR_A, HIGH);
    delay(500); 
   }
   digitalWrite(LEDBLU, LOW);
   digitalWrite(MOTOR_A, LOW);
   delay(2000); 
 }
 
void antechiudiA(){
   bool Chiudi = true;
  while(Chiudi) {
    if(digitalRead(MICRO_A_CLOSE)==HIGH){
      Chiudi = false;
      }
    digitalWrite(DIRMOTOR_A, HIGH);
    digitalWrite(MOTOR_A, HIGH);
    delay(500); 
    }
   digitalWrite( LEDBLU, LOW);
   digitalWrite(MOTOR_A, LOW);
   digitalWrite(DIRMOTOR_A, LOW);
   antechiudiB();
 }

void antechiudiB(){
  bool ChiudiB = true;
  while(ChiudiB) {
    if(digitalRead(MICRO_B_CLOSE)==HIGH){
      ChiudiB = false;
      }
    digitalWrite(DIRMOTOR_B, LOW);
    digitalWrite(MOTOR_B, HIGH);
    delay(500); 
    }
   digitalWrite( LEDBLU, LOW);
   digitalWrite(MOTOR_B, LOW);
   delay(2000); 
 }
 
void blokA (){
  digitalWrite(MOTOR_A, LOW);
  delay(500);
  }

void blokB (){
  digitalWrite(MOTOR_B, LOW);
  delay(500);
  }
