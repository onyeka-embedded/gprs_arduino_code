#include <SoftwareSerial.h>

// Define the SIM800L RX and TX pins
#define Rx 2
#define Tx 3

// Create a SoftwareSerial object to communicate with the SIM800L
SoftwareSerial gprsSerial(Rx, Tx);

#define base_url  "onyekainnov8.pythonanywhere.com"
#define post_endpoint  "onyekainnov8.pythonanywhere.com/api/device-update/"

// Replace this with your actual ThingSpeak API key
#define _ID   "GAS0001"

//Insert SIM Details
#define APN      "internet.ng.airtel.com" // "web.gprs.mtnnigeria.net"
#define PWD       ""
#define USER      ""

int co = 6;
int nox = 1;
int temp = 18;
int hum = 65;

char* sendAtCmd(String cmd, uint16_t period);
void gprsInit();
void senData();

unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;
unsigned long timee = 20000;  //the value is a number of milliseconds

void setup() {
  // Start the serial communication with SIM800L
  gprsSerial.begin(115200);               // the GPRS baud rate
  Serial.begin(9600);    // the GPRS baud rate
  delay(20000);
  sendAtCmd("AT+IPR=9600", 3000);
  gprsSerial.begin(9600);
  // Ensure the SIM800L is responsive
  sendAtCmd("AT", 1000);

  // Initialize GPRS connection (replace with your actual APN, username, and password)
  gprsInit();
  startMillis = millis();  //initial start time
}

void loop() {

  currentMillis = millis();
  if (currentMillis - startMillis >= timee) { //test whether the period has elapsed
    senData();
    startMillis = currentMillis;  //IMPORTANT to save the start time of the current LED state.
  }
}

void senData() {
  gprsInit();
  co = co + 0.1;
  nox = nox + 0.2;
  temp = temp + 1.2;
  hum = hum + 1;
  String payload = "device_id=" + String(_ID) + "&co=" + String(co + 0.1) + "&nox=" + String(nox + 0.2) + "&temp=" + String(temp + 1.2) + "&hum=" + String(hum + 1);
  //open GPRS context bearer
  sendAtCmd("AT+SAPBR=1,1", 2000);
  sendAtCmd("AT+SAPBR=2,1", 2000);
  // Send the HTTP POST request
  sendAtCmd("AT+HTTPINIT", 2000);
  sendAtCmd("AT+HTTPPARA=\"CID\",1", 2000);
  String URL = "AT+HTTPPARA=\"URL\",\"" + String(post_endpoint) + "\"";
  sendAtCmd(URL, 5000);
  sendAtCmd("AT+HTTPPARA=\"CONTENT\",\"application/x-www-form-urlencoded\"", 3000);
  sendAtCmd("AT+HTTPDATA=" + String(payload.length()) + ",10000", 3000);
  sendAtCmd(payload, 5000);
  sendAtCmd("AT+HTTPACTION=1", 10000);
  sendAtCmd("AT+HTTPREAD", 4000);
  sendAtCmd("AT+HTTPTERM", 1000);
  sendAtCmd("AT+CIPSHUT", 1000);
  sendAtCmd("AT+SAPBR=0,1", 1000);
}

void gprsInit() {
  sendAtCmd("AT", 1000);
  sendAtCmd("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"", 2000);
  String apn = "AT+SAPBR=3,1,\"APN\",\"" + String(APN) + "\"";
  sendAtCmd(apn, 2000);
  sendAtCmd("AT + SAPBR = 3, 1, \"USER\",\"\"", 2000);
  sendAtCmd("AT+SAPBR=3,1,\"PWD\",\"\"", 2000);
  sendAtCmd("AT+SAPBR=1,1", 2000);
}

char* sendAtCmd(String cmd, uint16_t period) {
  String response = "";
  char *result;
  gprsSerial.println(cmd);
  /*delay(period);
    ShowSerialData();
  */
  unsigned long currentMillis = millis();
  while ((currentMillis + period) > millis()) {
    while (gprsSerial.available()) {
      //Serial.println(gprsSerial.readString());
      char c = gprsSerial.read();
      response += c;
    }
  }
  result = response.c_str();
  Serial.print("RESPONSE: ");
  Serial.println(response);
  return result;
}
