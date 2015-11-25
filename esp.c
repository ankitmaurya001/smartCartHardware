 /*
  * This is used for sending data through ESP8266
  * The ESP8266 is connected to the hardware serial Rx1 and Tx1 of the mega.
  * We are using local version of data.sparkfun.com for now.
  */

//SSID + KEY
//const char SSID_ESP[] = "Chucha";
//const char SSID_KEY[] = "chuchaspeaking";
const char SSID_ESP[] = "PowerRangers";
const char SSID_KEY[] = "dhawanisgreat";


//connect
const char connect[]="AT+CWJAP=\"Chucha\",\"chuchaspeaking\"";


// URLs
const char thingsSpeakURL[] = "GET /input/Zr8kAraW8KsPXmy4E8LPFqk7dDk?private_key=ZrJw3rLyJBsonqpmbk0ouMwOWNw&kaand=29.86&phonenumber=21.94&productname=10.59&productprice=7.08 HTTP/1.0\r\n\r\n";
//const char thingsSpeakURL[] = "GET /input/K5Vno8g7OAsw3722mkgQt2vDOXQ?private_key=BQ3bgaBVzJHolGKKQbD4T3Q2rXY&field1=34.21 HTTP/1.0\r\n\r\n";
const char URL_current_temp[] = "GET https://api.thingspeak.com/apps/thinghttp/send_request?api_key=3M27CDHR12NN52V1 HTTP/1.0\r\n\r\n";

//DEFINE KEYWORDS HERE
const char OK[] = "OK";
const char CARROT[] = ">";
const char SUCCESS[] = "success";

//MODES
const char CWMODE = '3';//CWMODE 1=STATION, 2=APMODE, 3=BOTH
const char CIPMUX = '0';//CWMODE 0=Single Connection, 1=Multiple Connections

//DEFINE ALL GLOBAL VAARIABLES HERE
char payload[150];
byte payloadSize=0, counter=0;



//declare all functions here
boolean setupESP();
boolean checkResponse(const char response[],int responseSize,int timeout);
void clearESPBuffer();
void printESPBuffer();
void sendData();
boolean connectESP();



//main() is only used so that stupid eclispe doesn't start throwing error
//int main() {

void setup(){
  Serial.begin(115200);   //will be used for logging the data from the ESP8266
  Serial1.begin(115200);  //wiil be connected to ESP8266 and will do the actual work.
  delay(5000);
  while(!setupESP()){
	  Serial.println("Setup of ESP failed trying again in 5 sec");
	  delay(5000);
  }

  Serial.println("Setup of ESP passed");
}

void loop(){
	sendData();


}


//main
//}


/*FUNCTION NAME:
  * setupESP
  *
  * DESCRIPTION:
  * This is used to setup the ESP , called only once
  *
  * PARAMETERS:
  * None.
  *
  * RETURN VALUES:
  * 0 - if any of AT commands failed
  * 1 - if all AT commands passed
  */


boolean setupESP(){
   //send AT+RST to reset
   Serial.println("Sending AT+RST");   //log the info
   Serial1.print("AT+RST\r\n");      //send AT+RST to ESP8266
   //check whether you got OK
   if(checkResponse(OK,sizeof(OK),5000)){
	   Serial.println("ESP Reset OK"); //log the info
   }else{
	   Serial.println("ESP Reset FAIL");  //log the info
	   return 0;
   }
   //get rid on any more Rx data and clear the buffer
   clearESPBuffer();

   //let the ESP rest
   delay(5000);

   //send AT command just see if module is alive.
   Serial.println("Sending AT");   //log the info
   Serial1.print("AT\r\n");        //send AT to ESP8266
    //check whether you got OK
   if(checkResponse(OK,sizeof(OK),5000)){
      Serial.println("ESP alive"); //log the info
   }else{
      Serial.println("ESP dead");  //log the info
      return 0;
   }
    //get rid on any more Rx data and clear the buffer
    clearESPBuffer();





    //set the CWMODE
    Serial.println("AT+CWMODE=3");  //log the info
    Serial1.print("AT+CWMODE=");
    Serial1.print(CWMODE);          //just send what is set in the constant
    Serial1.print("\r\n");
    //check whether you got OK
    if(checkResponse(OK,sizeof(OK),5000)){
       Serial.println("ESP CWMODE Set Passed"); //log the info
    }else{
        Serial.println("ESP CWMODE Set Failed");  //log the info
        return 0;
    }
     //get rid on any more Rx data and clear the buffer
    clearESPBuffer();


   //connect to a ssid
    Serial.println("Connecting to SSID: ");  //log the info
    Serial.println(SSID_ESP);        //log the info
    Serial1.print("AT+CWJAP=\"");
    Serial1.print(SSID_ESP);
    Serial1.print("\",\"");
    Serial1.print(SSID_KEY);
    Serial1.print("\"\r\n");
//    Serial1.print(connect);
//    Serial1.print("\r\n");
    //check whether you got OK
    if(checkResponse(OK,sizeof(OK),20000)){
        Serial.println("ESP Connected"); //log the info
    }else{
         Serial.println("ESP Connection failed");  //log the info
         return 0;
    }
    //get rid on any more Rx data and clear the buffer
    clearESPBuffer();


   //set no of connections
    Serial.println("Setting no. of connections");  //log the info
    Serial1.print("AT+CIPMUX=");
    Serial1.print(CIPMUX);
    Serial1.print("\r\n");
    //check whether you got OK
    if(checkResponse(OK,sizeof(OK),5000)){
         Serial.println("ESP CIPMUX set"); //log the info
    }else{
         Serial.println("ESP CIPMUX set failed");  //log the info
         return 0;
    }
    //get rid on any more Rx data and clear the buffer
    clearESPBuffer();

    return 1 ;  //everything worked

}




/*FUNCTION NAME:
  * checkResponse
  *
  * DESCRIPTION:
  * This checks for the given response from the ESP8266 .
  *
  * PARAMETERS:
  * 1.response[]  - the response to search
  * 2.responseSize  - the size of response ,sizeof(response)
  * 3.timeout     - the maximum time to lookfor
  *
  * RETURN VALUES:
  * 1 - if pattern found
  * 0 - if pattern not found or timeout
  */
boolean checkResponse(const char response[],int responseSize,int timeout){
   unsigned long timeoutStart = millis(); //for the timeout
   char data[20];       //this is the buffer - if response is longer than 20, then increase this
   responseSize--;      //since we're going to get an extra character from the sizeof()


   //fill the data buffer from Rx
   for(byte i=0; i<responseSize; i++){
     while(!Serial1.available()){//wait until a new byte is sent down from the ESP.
      if((millis()-timeoutStart)>timeout){//if nothing happens within the timeout period, get out of here
        Serial.println("timeout");     //timeout !! log the info
              return 0; //get out of here
              }
      }

      data[i]=Serial1.read();// save the byte to the buffer data
     }


   //the buffer if filled so start checking the response and refreshing the buffer
   //stay in here until the keyword found or a timeout occurs
    while(1){
        //run through the entire buffer and look for the keyword
        //this check is here, just in case the first thing out of the ESP was the response, meaning the buffer was actually filled with the response
        for(byte i=0; i<responseSize; i++){
          if(response[i]!=data[i]){//if it doesn't match, break out of the search now
            break;
          }
          if(i==(responseSize-1)){//we got all the way through the buffer without breaking, must be a match!
          return 1; //return a 1 and get outta here!
          }
        }

   //start rolling/refreshing the buffer
     for(byte i=0; i<(responseSize-1); i++){
        data[i]=data[i+1];// so the data at 0 becomes the data at 1, and so on.... the last value is where we'll put the new data
     }

    //timing control
      while(!Serial1.available()){
        if((millis()-timeoutStart)>timeout){
          Serial.println("timeout");  //timeout !! log the info
              return 0;
            }
       }
      data[responseSize-1]=Serial1.read();//save the new data in the last position in the buffer.
    }
}



/*FUNCTION NAME:
  * clearESPBuffer
  *
  * DESCRIPTION:
  * This is used to clear the rx buffer , which is filled by response from ESP.
  *
  * PARAMETERS:
  * None.
  *
  * RETURN VALUES:
  * None
  */

void clearESPBuffer(){
  char temp;
  while(Serial1.available()){
    temp = Serial1.read();
    delay(1);
   }
}


/*FUNCTION NAME:
  * printESPBuffer
  *
  * DESCRIPTION:
  * This is used to print the rx buffer , which is filled by response from ESP.
  * Will be helpful in debugging.
  *
  * PARAMETERS:
  * None.
  *
  * RETURN VALUES:
  * None
  */


void printESPBuffer(){
  while(Serial1.available()){
    Serial.println(Serial1.read());  //log all the info
  }

}



/*FUNCTION NAME:
  * sendData
  *
  * DESCRIPTION:
  * This is used to send data to the data.sparkfun.com(local version).
  * The data is sent from ESP.
  *
  *
  * PARAMETERS:
  * None.
  *
  * RETURN VALUES:
  * None
  */
void sendData(){
	counter=0;//keeps track of the payload size
	payloadSize=0;
	for(int i=0; i<(sizeof(thingsSpeakURL)-1); i++){
	   payload[payloadSize+i] = thingsSpeakURL[i];//build up the payload
	   counter++;
	}
	payloadSize = counter+payloadSize;

	//connect the ESP to server and proceed if successful
	if(connectESP()){
	//send the data
		Serial.println("Sending data of size: ");  //log the info
		Serial.print(payloadSize);                  //log the info
	//send data to the server
/*		for(int i=0; i<payloadSize; i++){
		   Serial1.print(payload[i]);
		   Serial.print(payload[i]);    //log the info
		}*/
		for(int i=0; i<150; i++){
				   Serial1.print(payload[i]);
				   Serial.print(payload[i]);    //log the info
				}
		Serial1.print("1.0\r\n\r\n");
		Serial.print("1.0\r\n\r\n");

	//debugging for now
/*	//check whether you got success
		if(checkResponse(SUCCESS,sizeof(SUCCESS),15000)){
		   Serial.println("Data sent to server"); //log the info
		}else{
		   Serial.println("Data not sent to server");  //log the info
		}
		//get rid on any more Rx data and clear the buffer
		clearESPBuffer();
*/
		delay(15000);
		printESPBuffer(); //print all what you are getting

	}

}



/*FUNCTION NAME:
  * connectESP
  *
  * DESCRIPTION:
  * This sets up a TCP connection to the local version of data.sparkfun.com.
  * Also tells the ESP how much char of data to send.
  *
  *
  * PARAMETERS:
  * None.
  *
  * RETURN VALUES:
  * 1 - if able to connect
  * 0 - if not able to connect
  */
boolean connectESP(){
	//get rid on any more Rx data and clear the buffer
    clearESPBuffer();
	Serial.println("Setting TCP connection");  //log the info
	Serial1.print("AT+CIPSTART=\"TCP\",\"192.168.1.102\",8080\r\n");   //set TCP connection to the server.
    //check whether you got OK
	if(checkResponse(OK,sizeof(OK),5000)){
	   Serial.println("TCP connection set with ESP"); //log the info
	}else{
	   Serial.println("TCP connection failed with ESP");  //log the info
	   return 0;
	}
	//get rid on any more Rx data and clear the buffer
	clearESPBuffer();

	//send the no of character to send
	Serial.println("Sending no of chars to send");   //log the info
    Serial1.print("AT+CIPSEND=");
    Serial1.print(payloadSize);
    Serial1.print("\r\n");
    //check whether you got >
	if(checkResponse(CARROT,sizeof(CARROT),5000)){
	   Serial.println("Got > ready to send data"); //log the info
	}else{
	   Serial.println("Didn't got > :(");  //log the info
	   return 0;
	}
	//get rid on any more Rx data and clear the buffer
	clearESPBuffer();
	return 1;


}








