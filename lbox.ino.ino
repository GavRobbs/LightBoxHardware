#include <SPI.h>
#include <WiFi101.h>
#include "secrets.h"

//Remember to create your own secrets.h file containing the strings as char arrays for ssid, pass and server

#define PIN_CS 10
#define PIN_IRQ 9
#define PIN_RST 8

#define STATIC_MODE 0
#define BLINK_MODE 1
#define PATTERN_MODE 2

#define WHITE_LED 7
#define RED_LED 6
#define GREEN_LED 5
#define ORANGE_LED 4
#define BLUE_LED 3
#define YELLOW_LED 2



String data_buff;

int status = WL_IDLE_STATUS;
WiFiClient client;

int colors[6] = {0, 0 ,0 ,0 ,0 ,0};
int blink_on = 0;
int blink_delay = 0;
int psdelay = 0;
int mode;
int count = 0;
boolean newdata = true;
char pattern[32];
bool newpattern = true;

void RequestData(){
  newdata = true;
  if(client.connect(server,80)) {
    // Make a HTTP request:
    client.println("GET /todo HTTP/1.1");
    client.println("Host: fathomless-sands-81815.herokuapp.com");
    client.println("Connection: close");
    client.println();
  }
};

void setup() {
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  WiFi.setPins(PIN_CS, PIN_IRQ, PIN_RST, -1);
  Serial.begin(9600);
  
  while(status != WL_CONNECTED){
    status = WiFi.begin(ssid, pass);
    delay(5000);
  }

  memset(pattern, "\0", sizeof(char)*32);

  Serial.println("Setup complete");

   RequestData();
}

void AnalyzeData(){
  newdata = false;
  String temp;
  if(data_buff.indexOf("context:todo") != -1){
    temp = data_buff.substring(data_buff.indexOf("context:todo"));    
    int endtype = temp.indexOf("type:0;");
    if(endtype != -1){
      temp = temp.substring(endtype+7);
      sscanf(temp.c_str(),"colors:%i,%i,%i,%i,%i,%i;blink:%i;blink_delay:%i;", &colors[0], &colors[1], &colors[2], &colors[3], &colors[4], &colors[5], &blink_on, &blink_delay);
      if(blink_on == 1){
        mode = BLINK_MODE;
      } else{
        mode = STATIC_MODE;
      }
    } else{
      //This has to be a specified pattern then
      memset(pattern, "\0", sizeof(char)*32);
      temp = temp.substring(temp.indexOf("type:1;")+7);
      sscanf(temp.c_str(),"pcode:%[^;];delay:%i;", &pattern, &psdelay);
      mode = PATTERN_MODE; 
      newpattern = true;
    }
  } else if(data_buff.indexOf("context:change") != -1){
    temp = data_buff.substring(data_buff.indexOf("context:change"));
    Serial.println(temp);   
  } else{
    //I dunno.
  }
}

void TurnOffEverything(){
  digitalWrite(WHITE_LED, LOW);
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(ORANGE_LED, LOW);
  digitalWrite(BLUE_LED,LOW);
  digitalWrite(YELLOW_LED, LOW);  
}

void CharToLed(char c){
  switch(c){
    case 'w':
    digitalWrite(WHITE_LED, HIGH);
    break;
    case 'r':
    digitalWrite(RED_LED, HIGH);
    break;
    case 'b':
    digitalWrite(BLUE_LED, HIGH);
    break;
    case 'g':
    digitalWrite(GREEN_LED, HIGH);
    break;
    case 'o':
    digitalWrite(ORANGE_LED, HIGH);
    break;
    case 'y':
    digitalWrite(YELLOW_LED, HIGH);
    break;
    default:
    break;
  }
}

void DisplayPattern(){
  if(newpattern == true){
    count = 0;  
    newpattern = false;
  } else{
    count++;
    if(count == strlen(pattern)){
      count = 0;
    }
  }
  TurnOffEverything();
  CharToLed(pattern[count]);
}

void loop() {
  while(client.available()){
    char c = client.read();
    data_buff.concat(c);
    newdata = true;
  }
  if(newdata){
    AnalyzeData();
  };
  if(mode == STATIC_MODE){
    digitalWrite(WHITE_LED, colors[0]);
    digitalWrite(RED_LED, colors[1]);
    digitalWrite(GREEN_LED, colors[2]);
    digitalWrite(ORANGE_LED, colors[3]);
    digitalWrite(BLUE_LED, colors[4]);
    digitalWrite(YELLOW_LED, colors[5]);            
  } else if(mode == BLINK_MODE){
    digitalWrite(WHITE_LED, colors[0]);
    digitalWrite(RED_LED, colors[1]);
    digitalWrite(GREEN_LED, colors[2]);
    digitalWrite(ORANGE_LED, colors[3]);
    digitalWrite(BLUE_LED, colors[4]);
    digitalWrite(YELLOW_LED, colors[5]);
    delay(blink_delay);
    TurnOffEverything();
    delay(blink_delay);
  } else if(mode == PATTERN_MODE){
    DisplayPattern();
    delay(psdelay);
    TurnOffEverything();
    delay(psdelay);
  }
}
