/**
 * This is used for reading the rfid values with the reader attached to hardwareSerial Rx2 of the mega.
 * The ESP8266 is connected to the hardware serial Rx1 and Tx1 of the mega.
 * As soon as we get the value into serial Rx2 i.e value from the rfid reader,we will send
 * it to data.sparkfun.com(local version).We will maintain a local array of structures(has to be on a server in the long run),
 * which we will look into and send the corresponding values to the data.sparkfun.com(local version).
 *
 *
 * Basically for now I am just integrating the VM5GA.c and esp.c together. Hope it works!!
 **/

//For ESP8266
//mere ghar ke liye
//SSID + KEY
//const char SSID_ESP[] = "Chucha";
//const char SSID_KEY[] = "chuchaspeaking";
//const char SSID_ESP[] = "PowerRangers";
//const char SSID_KEY[] = "dhawanisgreat";


//for thinqubator
const char SSID_ESP[] = "Thinqubator";
const char SSID_KEY[] = "C1sco12345";


//connect
const char connect[]="AT+CWJAP=\"Chucha\",\"chuchaspeaking\"";


// URLs
const char thingsSpeakURL[] = "GET /input/L87zZE13dQu1wB3e3aoeH73rM4ZR?private_key=QQNPMndZ5rIyLaNWNZ4WuLGdY3Z0&kaand=green&phonenumber=12345&productname=Levis&productprice=1999 HTTP/1.0\r\n\r\n";
//const char thingsSpeakURL[] = "GET /input/K5Vno8g7OAsw3722mkgQt2vDOXQ?private_key=BQ3bgaBVzJHolGKKQbD4T3Q2rXY&field1=34.21 HTTP/1.0\r\n\r\n";
const char URL_current_temp[] = "GET https://api.thingspeak.com/apps/thinghttp/send_request?api_key=3M27CDHR12NN52V1 HTTP/1.0\r\n\r\n";
//const char thigsSpeakURL[] = "GET /input/Zr8kAraW8KsPXmy4E8LPFqk7dDk?private_key=ZrJw3rLyJBsonqpmbk0ouMwOWNw&kaand=29.86&phonenumber=21.94&productname=10.59&productprice=7.08 HTTP/1.0\r\n\r\n";


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


//FOR VM5GA
//DEFINE KEYWORDS HERE
//const byte INFO[] = {0xBB , 0x00, 0x03, 0x00, 0x01, 0x02, 0x06, 0x7E};
//const char INFO_RESPONSE[] =
//const char SUCCESS[] = "success";

byte info[] = {0xBB , 0x00, 0x03, 0x00, 0x01, 0x02, 0x06, 0x7E};
byte china800MHZregion[] = {0xBB,	0x00,	0x07,	0x00,	0x01,	0x04,	0x0C,	0x7E};
byte freq840MHZ[] = {0xBB, 0x00, 0xAB, 0x00, 0x01, 0x00, 0xAC, 0x7E };
byte infoResponse[] = {0xBB, 0x01, 0x03, 0x00, 0x08, 0x02, 0x4D, 0x61, 0x67, 0x69, 0x63, 0x52, 0x66, 0xA7, 0X7E};
byte signalStrength20dm[] = {0xBB, 0x00, 0xB6, 0x00, 0x02, 0x07, 0xD0, 0x8F, 0x7E };
byte signalStrength26dbm[] ={0xBB, 0x00, 0xB6, 0x00, 0x02, 0x0A, 0x28, 0xEA, 0x7E };
byte singlePollingCmd[] = {0xBB, 0x00, 0x22, 0x00, 0x00, 0x22, 0x7E};
byte multiplePollingCmd[] = {0xBB, 0x00, 0x27, 0x00, 0x03, 0x22, 0x00, 0x28, 0x74, 0x7E }; // poll 40 times
byte stopMultiplePollingCmd[] = {0xBB, 0x00, 0x28, 0x00, 0x00, 0x28, 0x7E};
//String infoResponseString = "";
String readString = "";
char infoResponse1[] = {0xBB, 0x01, 0x03, 0x00, 0x08, 0x02, 0x4D, 0x61, 0x67, 0x69, 0x63, 0x52, 0x66, 0xA7, 0X7E};
char china800MHZregionResponse1[] = {0xBB,	0x01,	0x07,	0x00,	0x01,	0x00,	0x09,	0x7E};
char freq840MHZResponse1[] = {0xBB,	0x01,	0xAB,	0x00,	0x01,	0x00,	0xAD,	0x7E};
char signalStrength20dmResponse1[] = {0xBB,	0x01,	0xB6,	0x00,	0x01,	0x00,	0xB8,	0x7E};
char tagID1Response[] ={0xBB, 0x02, 0x22, 0x00, 0x11, 0xC3, 0x30, 0x00,
		0x34, 0x08, 0x33, 0xB2, 0xDD, 0xD9, 0x01, 0x40,
		0x00, 0x00, 0x00, 0x00, 0x34, 0x6F, 0xE3, 0x7E };
char stopMultiplePollingCmdResponse1[]={0xBB, 0x01,	0x28, 0x00,	0x01, 0x00,	0x2A, 0x7E};

//vahiyaat jugaad
int tag1Found =0;
int tag2Found =0;
int tag3Found =0;
int tag4Found =0;


//gloabal variable for the current tag.
int tagNo;


void setup() {
  // initialize both serial ports:
  Serial.begin(115200);
  Serial2.begin(115200);
  Serial1.begin(115200);  //wiil be connected to ESP8266 and will do the actual work.
  pinMode(8, OUTPUT);   //connected to EN pin
  digitalWrite(8,HIGH);  //make EN pin high
  setupVM5GA();          //setup VM5GA


  delay(5000);

  while(!setupESP()){
	  Serial.println("Setup of ESP failed trying again in 5 sec");
	  delay(5000);
  }

  Serial.println("Setup of ESP passed");

}


//This will be looped over .I will wait for a tag , and only if a valid tag is found,
//will ask ESP to send data to the server and will repeat.
void loop(){
	//going will multiple polling , because it looked better to me . Well it's 5:23 am so can't really say :P
	//try multiple polling and see what happens
	//keep polling untill you find a tag!!
	while(!(tagNo =multiplePolling()));
	//clear the VM5GA buffer.
	clearVM5GABuffer();
	//rest
	delay(10);

	//tag found now , request ESP8266 to push data to server.
	Serial.print("Requesting ESP to push tag:");   //log the info.
    Serial.println(tagNo);                         //log the info.
	sendData();
}






// VM5GA functions

/*FUNCTION NAME:
  * isVM5GAAlive
  *
  * DESCRIPTION:
  * This checks whether VM5GA is alive.
  *
  *
  * PARAMETERS:
  * None.
  *
  * RETURN VALUES:
  * 1  - if alive
  * 0  - if not alive
  */

int isVM5GAAlive(){
	//will try to get product info and check the response .
	//query product info.


	 //log the info
	 Serial.println("Querying product info");
     // clear VM5GA buffer first.
	 clearVM5GABuffer();
	 Serial2.write(info, sizeof(info));
     //check for the response.
	 if(checkResponseVM5GA(infoResponse1,sizeof(infoResponse),5000)){
		 Serial.println("Module found !!");
		 return 1;
	 }else{
		 Serial.println("Module not found");
		 return 0;
	 }

}


/*FUNCTION NAME:
  * checkResponseVM5GA
  *
  * DESCRIPTION:
  * This checks for the given response from VM5GA.
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
boolean checkResponseVM5GA(char response[],int responseSize,int timeout){
   unsigned long timeoutStart = millis(); //for the timeout
   char data[30];       //this is the buffer - if response is longer than 20, then increase this
   responseSize--;      //since we're going to get an extra character from the sizeof()


   //fill the data buffer from Rx
   for(byte i=0; i<responseSize; i++){
     while(!Serial2.available()){//wait until a new byte is sent down from the VM5GA.
      if((millis()-timeoutStart)>timeout){//if nothing happens within the timeout period, get out of here
        Serial.println("timeout1");     //timeout !! log the info
              return 0; //get out of here
              }
      }

      data[i]=Serial2.read();// save the byte to the buffer data
      readString += data[i];
     }

// really don't know why this is showing not a match.
//   Serial.println("Printing data");
//   Serial.println(readString);
//  readString = "";
//   if(readString == infoResponseString){
//	   Serial.println("Correct module info!!");
//   }else{
//	   Serial.println("Wrong module info!!");
//   }



   //the buffer if filled so start checking the response and refreshing the buffer
   //stay in here until the keyword found or a timeout occurs
    while(1){
        //run through the entire buffer and look for the keyword
        //this check is here, just in case the first thing out of the VM5GA was the response, meaning the buffer was actually filled with the response
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
        data[i]=data[i+1];// so the data at 0 becomes the data at 1, and so on.... the last value is where we'll put the new data.
     }

    //timing control
      while(!Serial2.available()){
        if((millis()-timeoutStart)>timeout){
          Serial.println("timeout2");  //timeout !! log the info
              return 0;
            }
       }
      data[responseSize-1]=Serial2.read();//save the new data in the last position in the buffer.
    }
}



/*FUNCTION NAME:
  * clearVM5GABuffer
  *
  * DESCRIPTION:
  * This is used to clear the rx buffer , which is filled by response from VM5GA.
  *
  * PARAMETERS:
  * None.
  *
  * RETURN VALUES:
  * None
  */

void clearVM5GABuffer(){
  char temp;
  while(Serial2.available()){
    temp = Serial2.read();
    delay(1);
   }
}




/*FUNCTION NAME:
  * setFrequencyVM5GA
  *
  * DESCRIPTION:
  * This is used to set the frequency of operation to CHINA 800MHz region.
  *
  * PARAMETERS:
  * None.
  *
  * RETURN VALUES:
  * None
  */

void setFrequencyVM5GA(){
  // first set region to CHINA 800MHZ.
  //	BB	00	07	00	01	(04) C	7E .. 04 for china 800Mhz.

  //log the info
	Serial.println("Setting region to CHINA 800MHZ");
  // clear VM5GA buffer first.
	clearVM5GABuffer();
	Serial2.write(china800MHZregion, sizeof(china800MHZregion));
  //check for the response.
    if(checkResponseVM5GA(china800MHZregionResponse1,sizeof(china800MHZregionResponse1),5000)){
		Serial.println("Region set to CHINA 800MHZ");   //log the info
	}else{
		Serial.println("Unable to set region to CHINA 800MHZ");  // log the info
	}

    //set the frequency to 840.125MHz
    //log the info
    Serial.println("Setting frequency to 840.125MHz");
    // clear VM5GA buffer first.
    clearVM5GABuffer();
    Serial2.write(freq840MHZ, sizeof(freq840MHZResponse1));
    //check for the response.
    if(checkResponseVM5GA(freq840MHZResponse1,sizeof(freq840MHZResponse1),5000)){
   		Serial.println("Frequency set to 840.125MHz");   //log the info
   	}else{
   		Serial.println("Unable to set frequency to 840.125MHz");  // log the info
   	}


}



/*FUNCTION NAME:
  * setSignalStrenthVM5GA
  *
  * DESCRIPTION:
  * This sets signal strength of VM5GA to 26dbm.
  *
  *
  * PARAMETERS:
  * None.
  *
  * RETURN VALUES:
  * None.
  *
  */

void setSignalStrengthVM5GA(){

	 //log the info
	 Serial.println("Setting signal strength to 26dbm");
     // clear VM5GA buffer first.
	 clearVM5GABuffer();
	 Serial2.write(signalStrength26dbm, sizeof(signalStrength26dbm));
     //check for the response.
	 if(checkResponseVM5GA(signalStrength20dmResponse1,sizeof(signalStrength20dmResponse1),5000)){
		 Serial.println("Signal strength set to 26dbm");
	 }else{
		 Serial.println("Unable to set signal strength to 26dbm");
	 }

}


/*FUNCTION NAME:
  * stopMultiplePollingVM5GA
  *
  * DESCRIPTION:
  * This function stops multiple polling of VM5GA.
  *
  *
  * PARAMETERS:
  * None.
  *
  * RETURN VALUES:
  * None.
  *
  */

void stopMultiplePollingVM5GA(){
	 //log the info
	 Serial.println("Stopping multiple polling");
	 // clear VM5GA buffer first.
	 clearVM5GABuffer();
	 Serial2.write(stopMultiplePollingCmd, sizeof(stopMultiplePollingCmd));
	 //check for the response.
	 if(checkResponseVM5GA(stopMultiplePollingCmdResponse1,sizeof(stopMultiplePollingCmdResponse1),5000)){
		Serial.println("Multiple polling stopped");
	 }else{
		Serial.println("Unable to stop multiple polling");
	}

}





/*FUNCTION NAME:
  * singlePolling
  *
  * DESCRIPTION:
  * This function polls the tag one single time.
  *
  *
  * PARAMETERS:
  * None.
  *
  * RETURN VALUES:
  * None.
  *
  */

void singlePolling(){

	 //log the info
	 Serial.println("Polling one single time");
     // clear VM5GA buffer first.
	 clearVM5GABuffer();
	 Serial2.write(singlePollingCmd, sizeof(singlePollingCmd));
	 delay(100);
	 //print the response.
	 //printResponseofVM5GA();
     //check for the response.
	// if(checkResponseVM5GA(tagID1Response,sizeof(tagID1Response),10000)){
	//	 Serial.println("Tag ID with 34 found");
	// }else{
	//	 Serial.println("Tag ID with 34 not found");
	// }
	 int tag =0;
	 tag = readTagVM5GA();
	 switch (tag) {
	     case 1:
	        Serial.println("Tag 0x31 found ");
	       break;
	     case 2:
	    	 Serial.println("Tag 0x32 found ");
	       break;
	     case 3:
	    	 Serial.println("Tag 0x33 found ");
	    	 break;
	     case 4:
	    	 Serial.println("Tag 0x34 found ");
	    	 break;
	     default:
	       // Serial.println("No tag found");
	     break;
	   }
}


/*FUNCTION NAME:
  * multiplePolling
  *
  * DESCRIPTION:
  * This function polls the tag multiple times. (40 here)
  *
  *
  * PARAMETERS:
  * None.
  *
  * RETURN VALUES:
  * tagNo - When a new tag is found.(valid values currently 1,2,3,4)
  * 0 - if no tag found.
  */

int multiplePolling(){

	 //log the info
	 Serial.println("Multiple polling 40 times");
     // clear VM5GA buffer first.
	 clearVM5GABuffer();
	 Serial2.write(multiplePollingCmd, sizeof(multiplePollingCmd));
	 delay(20);
	 //print the response.
	 //printResponseofVM5GA();
     //check for the response.
	// if(checkResponseVM5GA(tagID1Response,sizeof(tagID1Response),10000)){
	//	 Serial.println("Tag ID with 34 found");
	// }else{
	//	 Serial.println("Tag ID with 34 not found");
	// }
	 //read 40 times
	 for(int i=0;i<40;i++){
	 int tag =0;
	 tag = readTagVM5GA();
	 switch (tag) {
	     case 1:
	    	if(tag1Found ==0){  //come only once
	        Serial.println("Tag 0x31 found ");
	        tag1Found = 1;
	        stopMultiplePollingVM5GA();
	        return 1;
	    	}
	       break;
	     case 2:
	    	 if(tag2Found ==0){ //come only once
	    	 Serial.println("Tag 0x32 found ");
	    	 tag2Found = 1;
	    	 stopMultiplePollingVM5GA();
	    	 return 2;
	    	 }
	       break;
	     case 3:
	    	 if(tag3Found ==0){ //come only once
	    	 Serial.println("Tag 0x33 found ");
	    	 tag3Found = 1;
	    	 stopMultiplePollingVM5GA();
	    	 return 3;
	    	 }
	    	 break;
	     case 4:
	    	 if(tag4Found ==0){ //come only once
	    	 Serial.println("Tag 0x34 found ");
	    	 tag4Found =1;
	    	 stopMultiplePollingVM5GA();
	    	 return 4;
	    	 }
	    	 break;
	     default:
	       // Serial.println("No tag found");
	     break;
	   }
	 }
	 return 0; //nothing found
}

/*FUNCTION NAME:
  * printResponseOfVM5GA
  *
  * DESCRIPTION:
  * This function prints the response of VM5GA in readable format.
  *
  *
  * PARAMETERS:
  * None.
  *
  * RETURN VALUES:
  * None.
  *
  */


void printResponseofVM5GA(){
	 while (Serial2.available()) {
		  delay(10);  //small delay to allow input buffer to fill
		  char start[] = {0XBB};
		   char c = Serial2.read();  //gets one byte from serial buffer

		   readString += c;
		 } //makes the string readString

		 if (readString.length() >0) {
		   Serial.println(readString); //prints string to serial port out
		   readString=""; //clears variable for new input
		 }
}





/*FUNCTION NAME:
  * readTagValues
  *
  * DESCRIPTION:
  * This function reads tag value with some vahiyaat jugaad. Need to clean this up!!
  *
  *
  * PARAMETERS:
  * None.
  *
  * RETURN VALUES:
  * tag no.
  * 1 --- 0x31
  * 2 --- 0x32
  * 3 --- 0x33
  * 4 --- 0x34
  * 0 --- no valid tag.
  *
  */


int readTagVM5GA(){
	int i =0;
	int tagFound =0;
	while (Serial2.available()) {
		delay(5);  //small delay to allow input buffer to fill
		char badStart[] = {0XBB, 0X01 ,0XFF};
		char goodStart[] = {0X00}; //PC(LSB) should be zero
		char tags[] = {0x31, 0x32, 0x33, 0x34};
		char c = Serial2.read();  //gets one byte from serial buffer
        if(i==2 && c == badStart[2]){   //there should be a fucking better way to compare , no time hope this shit works.!!
            break;  //wrong response code from VM5GA.
        }
        if(i==7 && c == goodStart[0]){
        	char c = Serial2.read();
        	for(int j=0;j<4;j++){
        		if(c == tags[j]){
        		//tag found , save the id and break
        			tagFound = j+1;
        			break;
        		}
        	 }
        	break;  // breaking here as we have found some tag which is not in the inventory .
        }
		i++;

	}

	if(i==2 || i==7){
	   // read till end of this response i.e till 0x7E
	   while(Serial2.available()){
	    	delay(5); //small delay to allow input buffer to fill
	    	char end[] = {0X7E};
	    	char c = Serial2.read();
	    	if(c == end[0]){
	    		break;
	    	}
	    }
	}
	return tagFound;

}




/*FUNCTION NAME:
  * setupVM5GA
  *
  * DESCRIPTION:
  * This function initializes the VM5GA module.
  * 1)It checks if module is alive.
  * 2)Sets frequency region to CHINA 800MHz.
  * 3)Sets frequency to 840.125MHz.
  * 4)Sets signal strength to 26bdm.
  *
  * PARAMETERS:
  * None.
  *
  * RETURN VALUES:
  * None.
  *
  */

void setupVM5GA(){

	 while(!isVM5GAAlive());  //only go ahead if module is alive.
	 //clear the VM5GA buffer.
	 clearVM5GABuffer();
	 //rest
	 delay(10);

	 //set frequency to 840.125MHz. Not supported in India. (865-867 MHz) need to change !!
	 setFrequencyVM5GA();
     //clear the VM5GA buffer.
	 clearVM5GABuffer();
	 //rest
	 delay(10);

	 //set signal strength to 26dbm.
	 setSignalStrengthVM5GA();
     //clear the VM5GA buffer.
	 clearVM5GABuffer();
	 //rest
	 delay(10);

}



//ESP8266 functions
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
/*		for(int i=0; i<150; i++){
				   Serial1.print(payload[i]);
				   Serial.print(payload[i]);    //log the info
				}
		Serial1.print("1.0\r\n\r\n");
		Serial.print("1.0\r\n\r\n");
*/
	//now I have fucked up this code and I am going vahiyaat se bhi vahiyyat jugaad.
	//see the tag no. and decide what you want to send.
	//doing juggad for the app again , don't know why the fucking changes in the app are not working.
		// My mapping is
		// Levi's Jeans - oreo1
		// Wrogn shirt -  jam11
		// Arrow shirt - sauce
		// Leee shirt  - milk1
		//tag 1 Levi's Jeans.
		if(tagNo ==1){
		Serial.println("Sending Levi's Jeans");
		Serial.println("GET /input/L87zZE13dQu1wB3e3aoeH73rM4ZR?private_key=QQNPMndZ5rIyLaNWNZ4WuLGdY3Z0&kaand=green&phonenumber=12345&productname=oreo1&productprice=1999 HTTP/1.0\r\n\r\n");
		Serial1.println("GET /input/L87zZE13dQu1wB3e3aoeH73rM4ZR?private_key=QQNPMndZ5rIyLaNWNZ4WuLGdY3Z0&kaand=green&phonenumber=12345&productname=oreo1&productprice=1999 HTTP/1.0\r\n\r\n");
		}

		//tag 2 Wrogn Shirt
		if(tagNo ==2){
		Serial.println("Sending Wrogn shirt");
		Serial.println("GET /input/L87zZE13dQu1wB3e3aoeH73rM4ZR?private_key=QQNPMndZ5rIyLaNWNZ4WuLGdY3Z0&kaand=green&phonenumber=12345&productname=jam11&productprice=2499 HTTP/1.0\r\n\r\n");
	    Serial1.println("GET /input/L87zZE13dQu1wB3e3aoeH73rM4ZR?private_key=QQNPMndZ5rIyLaNWNZ4WuLGdY3Z0&kaand=green&phonenumber=12345&productname=jam11&productprice=2499 HTTP/1.0\r\n\r\n");
		}


	    //tag 3 Arrow Shirt
		if(tagNo ==3){
	    Serial.println("Sending Arrow shirt");
	    Serial.println("GET /input/L87zZE13dQu1wB3e3aoeH73rM4ZR?private_key=QQNPMndZ5rIyLaNWNZ4WuLGdY3Z0&kaand=green&phonenumber=12345&productname=sauce&productprice=3999 HTTP/1.0\r\n\r\n");
	    Serial1.println("GET /input/L87zZE13dQu1wB3e3aoeH73rM4ZR?private_key=QQNPMndZ5rIyLaNWNZ4WuLGdY3Z0&kaand=green&phonenumber=12345&productname=sauce&productprice=3999 HTTP/1.0\r\n\r\n");
		}


	    //tag 4 Leee Shirt
		if(tagNo ==4){
	    Serial.println("Sending Leeee shirt");
	    Serial.println("GET /input/L87zZE13dQu1wB3e3aoeH73rM4ZR?private_key=QQNPMndZ5rIyLaNWNZ4WuLGdY3Z0&kaand=green&phonenumber=12345&productname=milk1&productprice=1499 HTTP/1.0\r\n\r\n");
	    Serial1.println("GET /input/L87zZE13dQu1wB3e3aoeH73rM4ZR?private_key=QQNPMndZ5rIyLaNWNZ4WuLGdY3Z0&kaand=green&phonenumber=12345&productname=milk1&productprice=1499 HTTP/1.0\r\n\r\n");
		}
	    //debugging for now
	    //check whether you got success
		if(checkResponse(SUCCESS,sizeof(SUCCESS),15000)){
		   Serial.println("Data sent to server"); //log the info
		}else{
		   Serial.println("Data not sent to server");  //log the info
		}
		//get rid on any more Rx data and clear the buffer
		clearESPBuffer();

	//	delay(15000);
		//reducing the delay now to 1 sec.
		delay(1000);
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
	Serial1.print("AT+CIPSTART=\"TCP\",\"10.77.133.44\",8080\r\n");   //set TCP connection to the server.
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
    Serial1.print(payloadSize);  //for debuggin
    //int debugSize = 159;
    //Serial1.print(debugSize);
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

