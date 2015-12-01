/*
 * This is using arduino IDE itself for directly programming the ESP8266.
 * I am using phant library here open sourced by sparfun.
 * Also using phant(on my local desktop) for a server , also open sourced by sparfun!!.
 * Let's get rolling.
 *
 */

#include <ESP8266WiFi.h>
// Include the SparkFun Phant library.
#include <Phant.h>
#define LED_PIN 2


//mere ghar ke liye
/*
const char* ssid     = "Chucha";
const char* password = "chuchaspeaking";

const char* ssid     = "PowerRangers";
const char* password = "dhawanisgreat";
*/

//for thinqubator
const char* ssid     = "Thinqubator";
const char* password = "C1sco12345";


const char* host = "10.142.123.75";
const char* streamId   = "doWk1AlVE8IrO716QEBEsVBzP4DJ";
const char* privateKey = "GMg64QZXJ1HPkL9pZx3xH3qpYx1Q";

void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(LED_PIN, OUTPUT);
  //ESP is not connected so led is off.
  digitalWrite(LED_PIN, LOW);
 // Serial.println("***********************hi*******************");
  // We start by connecting to a WiFi network , led will be blinking @2Hz while connecting.
  connectToWiFi();
  //ESP is connected to make led solid red.
  digitalWrite(LED_PIN, HIGH);

}

int value = 0;
void loop(){

	while(!sendData()){
	  Serial.println("Sending data failed, trying again in 5 sec");
	  delay(5000);
	}
	Serial.println("Data sent, waiting 10sec to send another data");
	delay(10000);

}



/*FUNCTION NAME:
  * connectToWiFi
  *
  * DESCRIPTION:
  * This is used to connect the ESP to the WIFI. Will be running in while untill not connected.
  *
  * PARAMETERS:
  * None.
  *
  * RETURN VALUES:
  * None
  */

void connectToWiFi(){
	byte ledStatus = LOW;
	Serial.println();
	Serial.println();
	Serial.print("Connecting to ");
    Serial.println(ssid);

	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED) {
	// Blink the LED every 30sec.
	   digitalWrite(LED_PIN, ledStatus); // Write LED high/low
	   ledStatus = (ledStatus == HIGH) ? LOW : HIGH;
	   delay(500);
	   Serial.print(".");
	}

	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());


}




/*FUNCTION NAME:
  * sendData()
  *
  * DESCRIPTION:
  * This is used to push data to local version of data.sparkfun.com
  *
  * PARAMETERS:
  * None.
  *
  * RETURN VALUES:
  * 1- if data sent successfully .
  * 0- if data not sent
  */

int sendData(){
	// LED turns on when we enter, it'll go off when we
	// successfully post.
	digitalWrite(LED_PIN, HIGH);
	++value;

	// Declare an object from the Phant library - phant
	Phant phant(host,streamId, privateKey);

	// Add the four field/value pairs defined by our stream:
	phant.add("value", value);


	Serial.print("connecting to ");
	Serial.println(host);

	// Use WiFiClient class to create TCP connections
	WiFiClient client;
	const int httpPort = 8080;
	if (!client.connect(host, httpPort)) {
	  Serial.println("connection failed");
	  return 0;
	}

/*
	// We now create a URI for the request
	 String url = "/input/";
	 url += streamId;
	 url += "?private_key=";
	 url += privateKey;
	 url += "&value=";
	 url += value;

	 Serial.print("Requesting URL: ");
	 Serial.println(url);

	 // This will send the request to the server
	 client.print(String("GET ") + url + " HTTP/1.1\r\n" +
	               "Host: " + host + "\r\n" +
	               "Connection: close\r\n\r\n");

*/
	 // If we successfully connected, print our Phant post:
	 client.print(phant.post());

	 delay(10);  //wait for response from server

	 // Read all the lines of the reply from server and print them to Serial
	 while(client.available()){
	   String line = client.readStringUntil('\r');
	   Serial.print(line);
	   //look for OK response in HTTP/1.1 200 OK
	   /*not working properly commenting out
	   if(line.substring(13) == "OK"){
		   Serial.println("Data sent successfully");
		   dataSentSuccessfully = 1;
		   value --;
	   }
	   hoping data will always be sent properly.
	   */
	 }

	 Serial.println();
	 Serial.println("closing connection");

	 // Before we exit, turn the LED off.
	 digitalWrite(LED_PIN, LOW);

	 return 1; // Return success
}
