#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "Keypad.h"
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

//this device can connect 2.4 GHz only
const char* ssid     = "USS";           //Enter your SSID
const char* password = "maybesleep";    //Enter your password

const String host = "61.90.233.80";     //Enter your host name
const int httpPort = 2240;              //Enter your port number
String action = "maybesleep/Auth.php?data={var}";

const byte Rows= 4;                     //number of rows on the keypad i.e. 4
const byte Cols= 3;                     //number of columns on the keypad i,e, 3
byte rPins[Rows]= {10,16,0,2};          //pin of Rows 0 to 3
byte cPins[Cols]= {14,12,13};           //pin of Columns 0 to 2

char keymap[Rows][Cols]=
{
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

boolean check = false;
String var = "";
static String input = "";
static String res = "";

Keypad kpd = Keypad(makeKeymap(keymap), rPins, cPins, Rows, Cols);
LiquidCrystal_I2C lcd(0x27, 16, 4);

String encoder(String input){
  int code = input.toInt();
  int encode = ((code+25000)*8)+30000;
  Serial.println(code);
  Serial.println(encode);
  return String(encode);
}

void setResponse(String hostD,int res){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Connect to ");
  lcd.print(ssid);
  lcd.setCursor(0,1);
  lcd.print("Connected.");
  lcd.setCursor(0,2);
  lcd.print("To ");
  lcd.print(hostD);
  lcd.setCursor(0,3);
  if(res == 1){
    lcd.print("Auth Success.");
    Serial.print("Authorizetion success.");
  }else if(res == 0){
    lcd.print("Auth Failed!!");
    Serial.print("Authorizetion fail!!");
  }
}

void setConnect(String hostD){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Connect to ");
  lcd.print(ssid);
  lcd.setCursor(0,1);
  lcd.print("Connected.");
  lcd.setCursor(0,2);
  lcd.print("To ");
  lcd.print(hostD);
  lcd.setCursor(0,3);
  lcd.print("Connection failed.");
}

void setSend(String hostD){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Connect to ");
  lcd.print(ssid);
  lcd.setCursor(0,1);
  lcd.print("Connected.");
  lcd.setCursor(0,2);
  lcd.print("To ");
  lcd.print(hostD);
  lcd.setCursor(0,3);
  lcd.print("Sending....");
}

void setDisplay(String inputD){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Connect to ");
  lcd.print(ssid);
  lcd.setCursor(0,1);
  lcd.print("Connected.");
  lcd.setCursor(0,2);
  lcd.print("Enter your ID : ");
  lcd.setCursor(0,3);
  lcd.print(inputD);
}

void setup() {
  Serial.begin(115200);
  lcd.begin();
  delay(10);

  // Connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
   delay(500);
   Serial.print(".");
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("Connect to ");
   lcd.print(ssid);
   lcd.setCursor(0,1);
   lcd.print("Disconnect....");
  }

   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("Connect to ");
   lcd.print(ssid);
   lcd.setCursor(0,1);
   lcd.print("Connected.");

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

int value = 0;

void loop() {
  
         char keypressed = kpd.getKey();
         if((keypressed == '#') && (input.length() == 18))
         {
              Serial.println("Input is : "+input);
              var = input;
              input = "";
              check = true;
         }
         else if (keypressed == '*'){
              input = "";
              Serial.println("Clear.");
              setDisplay(input);
         }
         else if (keypressed != NO_KEY)
         { 
              if(input.length() < 18 && keypressed != '#')
              {
                input += keypressed;
                Serial.println(input);
              }
              setDisplay(input);
         }
 
 if(check){
     ++value;

     Serial.print("connecting to ");
     Serial.println(host);
  
     //URI for the request
     action.replace("{var}",var);
     //action.replace("{var}",encoder(var));
     String urlRe = "http://" + host + "/" + action;
    
     Serial.print("Requesting URL: ");
     Serial.println(urlRe);
    
     //send the request to the server
     HTTPClient clientRe;                                 //Declare an object of class HTTPClient
     clientRe.begin(urlRe);                               //Specify request destination
     int httpCode = clientRe.GET();                       //Send the request

     String response = "";
     if (httpCode > 0) { //Check the returning code
       response = clientRe.getString();                   //Get the request response payload
       Serial.println(response);                          //Print the response payload
     }
     clientRe.end();                                      //Close connection     
     setResponse(host,response.toInt());

     Serial.println("closing connection"); 
     check = false;
  }
}
