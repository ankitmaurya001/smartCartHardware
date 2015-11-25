/**
 * This is used for reading the rfid values with the reader attached to hardwareSerial Rx2 of the mega.
 * The ESP8266 is connected to the hardware serial Rx1 and Tx1 of the mega.
 * As soon as we get the value into serial Rx2 i.e value from the rfid reader,we will send
 * it to data.sparkfun.com(local version).We will maintain a local array of structures(has to be on a server in the long run),
 * which we will look into and send the corresponding values to the data.sparkfun.com(local version).
 **/

//SSID + KEY
const char SSID_ESP[] = "PowerRangers";
const char SSID_KEY[] = "dhawanisgreat";


  //DEFINE ALL GLOBAL VAARIABLES HERE
  //The no of products in the database.
  const int noOfProducts = 2;

  int rfidID[30];

  //This data structure is used to store information about the product.
  struct {
    int tagID[30];
    int productID;
    String productName;
    float productPrice;
  } products[2];



  //DEFINE KEYWORDS HERE
  const char keywordOk[] = "OK";
  const char keywordReady[] = "ready";
  const char keyword_no_change[] = "no change";
  const char keyword_blank[] = "#&";
  const char keyword_ip[] = "192.";
  const char keyword_rn[] = "\r\n";
  const char keyword_quote[] = "\"";
  const char keyword_carrot[] = ">";
  const char keyword_sendok[] = "SEND OK";
  const char keyword_linkdisc[] = "Unlink";

  //DEFINE FUNCTIONS HERE
  int  readDataFromRFIDReader();
  int validateTheTag(String tagID);
  boolean sendDataToServer(int productID);
  void setupESP();
  boolean read_until_ESP(const char keyword1[], int key_size,
      int timeout_val, byte mode);
  void serial_dump_ESP();

  void setup() {
    // initialize both serial ports:
    Serial.begin(115200); //used for logging the prints
    Serial1.begin(115200); //connected to Serial1
    Serial2.begin(115200); //connected to the RFID reader
     //maintain the table
  products[0].tagID[30] = {0};
  products[0].productID = 1;
  products[0].productName = "Wrogn Shirt";
  products[0].productPrice = 1999.0;

  products[1].tagID[30] = {0};
  products[1].productID = 2;
  products[1].productName = "Levi's Jeans";
  products[1].productPrice = 2999.0;
    setupESP(); //go setup the ESP
  }

  void loop() {
    readDataFromRFIDReader();
    delay(10000);
    /* tagID     = readDataFromRFIDReader();
     //see id there is some valid data to send
     if(tagID != NULL){
     productID   = validateTheTag(tagID);
     }
     if(productID !=0){
     sendStatus  = sendDataToServer(productID);
     }
     */
    //  sendDataToServer(1);
    // setupESP();            //go setup the ESP

  }

  /*FUNCTION NAME:
   * readDataFromRFIDReader
   *
   * DESCRIPTION:
   * This is used to read the value from RFID reader , if the tag is scanned.
   *
   * PARAMETERS:
   * None.
   *
   * RETURN VALUES:
   * The no of bytes read.
   */

  int readDataFromRFIDReader() {
    int count = 0;
    // read data from RFID reader is available
    while (Serial2.available()) {
      rfidID[count] = Serial2.read();
      count++;
    }
    Serial.println("Reading RFID count \n");
    Serial.write(count);
    return count;
  }

  /*FUNCTION NAME:
   * validateTheTag
   *
   * DESCRIPTION:
   * This is used to validate the tag value read from the RFID reader with the stored values
   * in the structure(have to be the store database in the long run).
   *
   * PARAMETERS:
   * String tagID
   *
   * RETURN VALUES:
   * The productID.
   */
  /*
   int validateTheTag(String tagID){
   int i;
   for(i=0;i<noOfProducts;i++){
   if(products[i].tagID == tagID){
   return (products[i].productID);
   }
   }

   //return 0 is product not found.
   return 0;
   }
   */

  void setupESP() {
    //send AT to see if module is alive
    Serial1.print("AT\r\n");
    //this read_until_... function is used to find a keyword in the ESP response
    if (read_until_ESP(keywordOk, sizeof(keywordOk), 5000, 0))//go look for keyword "OK" with a 5sec timeout
      Serial.println("ESP CHECK OK");
    else
      Serial.println("ESP CHECK FAILED");
    serial_dump_ESP();//this just reads everything in the buffer and what's still coming from the ESP

    //reset the module
    Serial1.print("AT+RST\r\n");
    //this read_until_... function is used to find a keyword in the ESP response
    if (read_until_ESP(keywordReady, sizeof(keywordReady), 5000, 0))//go look for keyword "OK" with a 5sec timeout
      Serial.println("ESP RESET OK");
    else
      Serial.println("ESP RESET FAILED");
    serial_dump_ESP();//this just reads everything in the buffer and what's still coming from the ESP


    //set CWMODE = 3 both
    Serial1.print("AT+CWMODE=3\r\n");// set the CWMODE
    if (read_until_ESP(keywordOk, sizeof(keywordOk), 1000, 0))//go look for keyword "OK"
      Serial.println("ESP CWMODE SET");
    else
      Serial.println("ESP CWMODE SET FAILED"); //probably going to fail, since a 'no change' is returned if already set - would be nice to check for two words
    serial_dump_ESP();

    //Here's where the SSID and PW are set
    Serial1.print("AT+CWJAP=\"");// set the SSID AT+CWJAP="SSID","PW"
    Serial1.print(SSID_ESP);//from constant
    Serial1.print("\",\"");
    Serial1.print(SSID_KEY);//form constant
    Serial1.print("\"\r\n");
    if (read_until_ESP(keywordOk, sizeof(keywordOk), 10000, 0))//go look for keyword "OK"
      Serial.println("ESP SSID SET OK");
    else
      Serial.println("ESP SSID SET FAILED");
    serial_dump_ESP();

    //set single connection
    Serial1.print("AT+CIPMUX=0\r\n");// set the CIPMUX
    if (read_until_ESP(keywordOk, sizeof(keywordOk), 5000, 0))//go look for keyword "OK" or "no change
      Serial.println("ESP CIPMUX SET");
    else
      Serial.println("ESP CIPMUX SET FAILED");
    serial_dump_ESP();

  }



//This function goes and finds a keyword received from the ESP
// const char keyword1[] = the keyword that this function will look for and return '1' when found
// int key_size = size of that keyword - sizeof()
// int timeout_val - timeout if the keword is not found (in milliseconds 5000=5sec)
// byte mode - if '1', this function will save every byte received to 'scratch_data_from_ESP[]' with length of 'scratch_length' - useful for parsing between keywords
boolean read_until_ESP(const char keyword1[], int key_size, int timeout_val,
    byte mode) {
  unsigned long timeout_start_val;
  timeout_start_val = millis();//for the timeout
  char data_in[20];//this is the buffer - if keyword is longer than 20, then increase this
  key_size--;//since we're going to get an extra charachter from the sizeof()

  //FILL UP THE BUFFER
  for (byte i = 0; i < key_size; i++) {//we only need a buffer as long as the keyword

    //timing control
    while (!Serial1.available()) {//wait until a new byte is sent down from the ESP - good way to keep in lock-step with the serial port
      if ((millis() - timeout_start_val) > timeout_val) {//if nothing happens within the timeout period, get out of here
        Serial.println("timeout");
        return 0;//this will end the function
      }//timeout
    }// while !avail

    data_in[i] = Serial1.read();// save the byte to the buffer 'data_in[]

  }//for i

  //THE BUFFER IS FULL, SO START ROLLING NEW DATA IN AND OLD DATA OUT
  while (1) {//stay in here until the keyword found or a timeout occurs

    //run through the entire buffer and look for the keyword
    //this check is here, just in case the first thing out of the ESP was the keyword, meaning the buffer was actually filled with the keyword
    for (byte i = 0; i < key_size; i++) {
      if (keyword1[i] != data_in[i])//if it doesn't match, break out of the search now
        break;//get outta here
      if (i == (key_size - 1)) {//we got all the way through the keyword without breaking, must be a match!
        return 1; //return a 1 and get outta here!
      }//if
    }//for byte i


    //start rolling the buffer
    for (byte i = 0; i < (key_size - 1); i++) {// keysize-1 because everthing is shifted over - see next line
      data_in[i] = data_in[i + 1];// so the data at 0 becomes the data at 1, and so on.... the last value is where we'll put the new data
    }//for


    //timing control
    while (!Serial1.available()) {// same thing as done in the buffer
      if ((millis() - timeout_start_val) > timeout_val) {
        Serial.println("timeout");
        return 0;
      }//timeout
    }// while !avail


    data_in[key_size - 1] = Serial1.read();//save the new data in the last position in the buffer


    /*  JUST FOR DEBUGGING
     if(ESP8266.overflow())
     Serial.println("*OVER");
     */

  }//while 1


}//read until ESP


//pretty simple function - read everything out of the serial buffer and whats coming and get rid of it
void serial_dump_ESP() {
  char temp;
  while (Serial1.available()) {
    temp = Serial1.read();
    delay(1);//could play around with this value if buffer overflows are occuring
  }//while
}



