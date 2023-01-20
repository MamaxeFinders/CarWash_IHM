#include <esp_now.h>
#include <WiFi.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager //https://microdigisoft.com/esp32-with-wifimanager-to-manage-ssid-and-password-no-hard-coding/
#include <HTTPClient.h>

// -- TEMP SENSOR -- //
#include "max6675.h"
#define RELAY_OUT 26
int thermoCLK = 12;
int thermoCS = 14;
int thermoDO = 27;
float Temp_Value;
float Min_Temperature = 2.00;
bool Bot_Temperature = false;
MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

// -- TELEGRAM -- //
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

#define LED_CAISSE1 17
#define LED_CAISSE2 5
#define LED_CAISSE3 18
#define LED_CAISSE4 19

// VARIABLES FOR PAIRING
#define NUMSLAVES 20
#define PRINTSCANRESULTS 0
// Google script ID and required credentials
String GOOGLE_SCRIPT_ID = "AKfycbziCv5KrLd04WmRT_k1B10jjVmOscfB7w6_DkPbByOzOITigoVcK06lYO42ExSkvbdJ";    // change Gscript ID
unsigned long Check_time = 0; //variables to keep track of the timing of recent interrupts
unsigned long last_Check_time = 0;

esp_now_peer_info_t slaves[NUMSLAVES] = {};
int CHANNEL = 1;
int SlaveCnt;
String Faulty_MAC_address;

// Initialize Telegram BOT
String BOTtoken = "XXXXXXXXXXXXXXXXXXXXX";  // your Bot Token (Get from Botfather)
String CHAT_ID; // "XXXXXXXXXXXXX"
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

bool CAISSE_response_OK = false;
String messages = "";
String messageSHOCK = "";
String Bot_Message = "";
typedef struct struct_Slaves_Info  {/*KEEP*/
int Slave_Number[NUMSLAVES];
String Slave_SSID[NUMSLAVES]; 
uint8_t Slave_MAC[NUMSLAVES][6]; 
int32_t Slave_Signal[NUMSLAVES];
unsigned int Slave_isFauly[NUMSLAVES];
}struct_Slaves_Info; struct_Slaves_Info Slaves_Info; /*KEEP*/

typedef struct struct_message_to_Caisse { /*KEEP*/
int CardCAISSE; 
int Request = 0;
float Credit = 00.00; 
int Message = 0;} struct_message_to_Caisse;
struct_message_to_Caisse message_to_Caisse; /*KEEP*/

typedef struct struct_message_From_Caisse { /*KEEP*/
int CardCAISSE; 
int Request;
float Credit; 
int Message;
int Shocks;} struct_message_From_Caisse;
struct_message_From_Caisse message_From_Caisse; /*KEEP*/

// send data and confirm the message was sent
void SendMessage(String a, int b, float c, int d, uint8_t peer_addr[6]) {
  message_to_Caisse.Request = b;
  message_to_Caisse.Credit = c;
  message_to_Caisse.Message = d;
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",peer_addr[0], peer_addr[1], peer_addr[2], peer_addr[3], peer_addr[4], peer_addr[5]);
    esp_err_t result = esp_now_send(peer_addr, (uint8_t *) &message_to_Caisse, sizeof(message_to_Caisse));
    Serial.print("Send Status for: ");Serial.print(a); Serial.print(" : ");Serial.print(macStr);Serial.print(" : ");
    if (result == ESP_OK) { Serial.println("Success");
    } else if (result == ESP_ERR_ESPNOW_NOT_INIT) {Serial.println("ESPNOW not Init.");
    } else if (result == ESP_ERR_ESPNOW_ARG) {Serial.println("Invalid Argument");
    } else if (result == ESP_ERR_ESPNOW_INTERNAL) {Serial.println("Internal Error");
    } else if (result == ESP_ERR_ESPNOW_NO_MEM) {Serial.println("ESP_ERR_ESPNOW_NO_MEM");
    } else if (result == ESP_ERR_ESPNOW_NOT_FOUND) {Serial.println("Peer not found.");
    } else {Serial.println("Not sure what happened");}
  delay(500); //Wait for callback feedback before sending another message
}
// callback when data is sent from SYSTEM to CAISSE and check receiving status
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Sent to: "); Serial.println(macStr); //Serial.println(macStr);
  if(status == ESP_NOW_SEND_SUCCESS){Serial.println("MAC acknoledge : SUCCESS");
    CAISSE_response_OK = true;
  }else{Serial.println("MAC acknoledge : FAILED");
    CAISSE_response_OK = false; 
  }   
}

// callback when data is recv from Master
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int data_len) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  memcpy(&message_From_Caisse, incomingData, sizeof(message_From_Caisse));
  Serial.print("Last Packet Recv from: "); Serial.print(macStr);Serial.print(" : Size: "); Serial.println(sizeof(message_From_Caisse));
  if(message_From_Caisse.Request == 1){ //PROGEM received, send data to Google
    messages = "message=PROGRAM&status=ok&caisse=" + String(message_From_Caisse.CardCAISSE) + "&credit=" + String(message_From_Caisse.Credit); 
  }else if(message_From_Caisse.Request == 9){ //Send Alert to Telegram Bot
    messages = "message=SHOCK&status=Shock&caisse=" + String(message_From_Caisse.CardCAISSE) + "&credit=" + String(message_From_Caisse.Credit);
    messageSHOCK = message_From_Caisse.CardCAISSE;
  }else{
    Serial.println("MESSAGE RECEIVED UNKNOWN !");
  }
}
// ======================================== SETUP ======================================== //
void setup() {
  Serial.begin(115200);
// ---- SET PINS LED ---- //
  pinMode(LED_CAISSE1,OUTPUT);
  pinMode(LED_CAISSE2,OUTPUT);
  pinMode(LED_CAISSE3,OUTPUT);
  pinMode(LED_CAISSE4,OUTPUT);
  ConfigDeviceAP();// Init WifiManager
  Serial.print("SYSTEM LAVAGE START : ");Serial.print("STA MAC: "); Serial.println(WiFi.macAddress());// This is the mac address of the Master in Station Mode
  InitESPNow();delay(1000);// Init ESPNow with a fallback logic
  ScanForSlave();delay(1000); // Scan all CAISSEs
    WiFi.mode(WIFI_AP_STA); //Optional
    WiFi.softAP("SYSTEM_CAISSE", "123456789", CHANNEL, false, NUMSLAVES); //ssid, password, channel, hide_SSID, max_connection
  if (WiFi.status() == WL_CONNECTED) {
   Serial.print("SYSTEM_CAISSE available on CHANNEL : ");Serial.println(CHANNEL);
   String messageBOTtoken = "message=GET&DATA=BOTtoken"; 
   String Response_BOTtoken = Get_Data_from_Google(String(messageBOTtoken));
   Serial.print("DATA RECEIVED : ");Serial.println(Response_BOTtoken);
   delay(1000);
   String messageCHAT_ID = "message=GET&DATA=CHAT_ID"; 
   String Response_CHAT_ID = Get_Data_from_Google(String(messageCHAT_ID));
   Serial.print("DATA RECEIVED : ");Serial.println(Response_CHAT_ID);
     if(Response_BOTtoken == "Error on HTTP request" || Response_CHAT_ID == "Error on HTTP request"){
      digitalWrite(LED_CAISSE1,LOW);digitalWrite(LED_CAISSE2,LOW);digitalWrite(LED_CAISSE3,LOW);digitalWrite(LED_CAISSE4,LOW);
      ESP.restart();
     }else{
      BOTtoken = Response_BOTtoken;
      CHAT_ID = Response_CHAT_ID;
     }
  }else{
    Serial.println(WiFi.status());
    digitalWrite(LED_CAISSE1,LOW);digitalWrite(LED_CAISSE2,LOW);digitalWrite(LED_CAISSE3,LOW);digitalWrite(LED_CAISSE4,LOW);
    ESP.restart();
  }
  // ---- SET TELEGRAM BOT ---- //
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  bot.sendMessage(CHAT_ID, "lavage_bot actif", "");
  const String commands = ("["
                              "{\"command\":\"status\",\"description\":\"status system\"},"
                              "{\"command\":\"caisse1\",\"description\":\"2 euros caisse 1\"},"
                              "{\"command\":\"caisse2\",\"description\":\"2 euros caisse 2\"},"
                              "{\"command\":\"caisse3\",\"description\":\"2 euros caisse 3\"},"
                              "{\"command\":\"caisse4\",\"description\":\"2 euros caisse 4\"},"
                              "{\"command\":\"temp\",\"description\":\"temperature\"}"// no comma on last command
                              "]");
   bot.setMyCommands(commands);
   pinMode(RELAY_OUT, OUTPUT); // Relay if low temperature
}

// ======================================== LOOP ======================================== //
unsigned long previousMillis = 0;
unsigned long interval = 3600000; //1000*60*60 for each hour
void loop() {
// -- CHECK TEMP -- //
Temp_Value = thermocouple.readCelsius();
if(Temp_Value < Min_Temperature){
  digitalWrite(RELAY_OUT, HIGH);
  if(!Bot_Temperature){
    bot.sendMessage(CHAT_ID,"ALERT Temp : " + String(Temp_Value) + " *C");
    Bot_Temperature = true;
  }
}else if(Temp_Value > Min_Temperature + 3){
  digitalWrite(RELAY_OUT, LOW);
    if(Bot_Temperature){
    bot.sendMessage(CHAT_ID,"Temp OK " + String(Temp_Value)+ " *C");
    Bot_Temperature = false;
  }
}
// ---- CHECK IF CAISSE ARE OK ---- //
unsigned long currentMillis = millis();
if (currentMillis - previousMillis >= interval) { //Check every X hours
  String messageSEND = "message=PING&temp=";
    for (int ii = 0; ii < SlaveCnt; ii++){
        SendMessage(Slaves_Info.Slave_SSID[ii],1,00.00,0,Slaves_Info.Slave_MAC[ii]); //int CardCAISSE,int Request,float Credit, int Message,const uint8_t peer_addr        
        Serial.print("TEST : ");Serial.print(Slaves_Info.Slave_SSID[ii]);Serial.print(" : ");
        if(CAISSE_response_OK){
          Serial.println("OK");
          Slaves_Info.Slave_isFauly[ii] = 0;
        }else{
          for(int jj = 0; jj<5 ; jj++){ // Chack 5 times to confirm faulty CAISSE
            delay(5000);
            if(!CAISSE_response_OK){
              Slaves_Info.Slave_isFauly[ii]++;
              Serial.print("FAILED ");Serial.println(Slaves_Info.Slave_isFauly[ii]);
              SendMessage(Slaves_Info.Slave_SSID[ii],1,00.00,0,Slaves_Info.Slave_MAC[ii]);
              delay(1000);
              if(Slaves_Info.Slave_isFauly[ii] > 4){Serial.println(" !!!! ALERT CAISSE FAULTY !!!");}
            }
          }
        delay(1000);
        }
        messageSEND = messageSEND + "&" + String(Slaves_Info.Slave_SSID[ii]) + "=" + String(Slaves_Info.Slave_isFauly[ii]); 
      delay(1000);
    }
   Send_Data_to_Google(messageSEND);
   delay(500);
   Send_Data_to_Google("message=SENSOR&temp="+String(Temp_Value));
   delay(500);
   previousMillis = millis();
  }else if(messages != ""){
    Send_Data_to_Google(messages);
    if(messageSHOCK != ""){
      bot.sendMessage(CHAT_ID,"Shock sur " + messageSHOCK);
    }
    messages = "";
    messageSHOCK = "";
    delay(500);
  }else if(Bot_Message != ""){
    if(Bot_Message == "TEMP_REQUEST"){
        bot.sendMessage(CHAT_ID,"Temperature : "+ String(Temp_Value) + "*C");
      }
    for (int ii = 0; ii < SlaveCnt; ii++){
      if(Slaves_Info.Slave_SSID[ii] == Bot_Message){
        Serial.print("Credit sent to");Serial.println(String(Slaves_Info.Slave_SSID[ii]));
        SendMessage(Slaves_Info.Slave_SSID[ii],2,02.00,0,Slaves_Info.Slave_MAC[ii]); //int CardCAISSE,int Request,float Credit, int Message,const uint8_t peer_addr
      }else if(Bot_Message == "status"){
        bot.sendMessage(CHAT_ID, String(Slaves_Info.Slave_SSID[ii]) + " a " + String(Slaves_Info.Slave_isFauly[ii]) + " faute(s)", "Markdown");
      }
    }
    Bot_Message = "";
  }else{
      int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
          while (numNewMessages){
            Serial.println("got response");
            handleNewMessages(numNewMessages);
            numNewMessages = bot.getUpdates(bot.last_message_received + 1);
          }
  }
delay(1000);
}
// ======================================== FUNCTIONS ======================================== //
// ---- HANDLE TELEGRAM MESSAGES ---- //
void handleNewMessages(int numNewMessages){
  Serial.print("handleNewMessages : ");
  Serial.println(numNewMessages);
    for (int i = 0; i < numNewMessages; i++){
    telegramMessage &msg = bot.messages[i];
    Serial.println("Received " + msg.text);
         if (msg.text == "/status"){Bot_Message = "status";}
    else if (msg.text == "/caisse1"){Bot_Message = "CAISSE1";}
    else if (msg.text == "/caisse2"){Bot_Message = "CAISSE2";}
    else if (msg.text == "/caisse3"){Bot_Message = "CAISSE3";}
    else if (msg.text == "/caisse4"){Bot_Message = "CAISSE4";}
    else if (msg.text == "/temp"){Bot_Message = "TEMP_REQUEST";}
    CHAT_ID = String(msg.chat_id);
    bot.sendMessage(CHAT_ID, "Message recu", "");
  }
}

// ---- SEND DATA TO GOOGLE ---- //
String Send_Data_to_Google(String Message_to_http){
    String urlFinal = "https://script.google.com/macros/s/"+GOOGLE_SCRIPT_ID+"/exec?"+Message_to_http;
    Serial.print("POST data to spreadsheet:");
    Serial.println(urlFinal);
    HTTPClient http;
    http.begin(urlFinal.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET(); 
    Serial.print("HTTP Status Code: ");
    Serial.println(httpCode);
    //---------------------------------------------------------------------
    //getting response from google sheet
    String payload;
    if (httpCode > 0) {
        payload = http.getString();
        //Serial.println("Payload: "+payload);    
    }
    //---------------------------------------------------------------------
    http.end();
  return payload;
}
// ---- GET DATA FROM GOOGLE ---- //
String Get_Data_from_Google(String Message_to_http){
    HTTPClient http;
    String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?"+Message_to_http;
    Serial.print("Making a request from URL :");
    Serial.println(url);
    http.begin(url.c_str()); //Specify the URL and certificate
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET();
    String payload;
    if (httpCode > 0) { //Check for the returning code
      payload = http.getString();
      return payload;
    }
    else {
      Serial.println("Error on HTTP request");
      return "Error on HTTP request";
    }
    http.end();
}


// ---- config AP SSID ---- //
void ConfigDeviceAP() {
  WiFiManager wm;
    bool res;
    res = wm.autoConnect("SYSTEM_LAVAGE","123456789"); // password protected ap
    wm.setEnableConfigPortal(true);
    if(!res) {Serial.println("Failed to connect");
        ESP.restart();
    } else {
      CHANNEL = WiFi.channel();
      Serial.print("connected...yeey : Channel ");Serial.println(CHANNEL);}
}

//// __________________ ESP NOW PAIRING __________________ ////
// ---- INIT ESP ---- //
void InitESPNow() {
  //WiFi.disconnect();
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);
  }  else {Serial.println("ESPNow Init Failed"); ESP.restart();}
}

// ---- SCAN FOR SLAVE ----//
void ScanForSlave() {
  int8_t scanResults = WiFi.scanNetworks();
  //reset slaves
  memset(slaves, 0, sizeof(slaves));
  SlaveCnt = 0;
  Serial.println("");
  if (scanResults == 0) {
    Serial.println("No WiFi devices in AP Mode found");
  } else {
    Serial.print("Found "); Serial.print(scanResults); Serial.println(" devices ");
    for (int i = 0; i < scanResults; ++i) {
      // Print SSID and RSSI for each device found
      String SSID = WiFi.SSID(i); //Provide the name of the network found
      int32_t RSSI = WiFi.RSSI(i); //Check signal performances from -100 to 0
      String BSSIDstr = WiFi.BSSIDstr(i); //Basic Service Set Identification) that is another name of MAC address of a network discovered during the scan
      //uint8_t bssid[6];WiFi.BSSID(bssid);
      if (PRINTSCANRESULTS) {
        Serial.print(i + 1); Serial.print(": "); Serial.print(SSID); Serial.print(" ["); Serial.print(BSSIDstr); Serial.print("]"); Serial.print(" ("); Serial.print(RSSI); Serial.print(")"); Serial.println("");
      }
      delay(10);
      // Check if the current device starts with `CAISSE`
      if (SSID.indexOf("CAISSE") == 0 && SlaveCnt < NUMSLAVES) {
        // SSID of interest
        Serial.print(SSID); Serial.print(" ["); Serial.print(BSSIDstr); Serial.print("]"); Serial.print(" ("); Serial.print(RSSI); Serial.print(")"); Serial.println("");
        // Get BSSID => Mac Address of the Slave
        int mac[6];
        if ( 6 == sscanf(BSSIDstr.c_str(), "%x:%x:%x:%x:%x:%x",  &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]) ) {
          for (int ii = 0; ii < 6; ++ii ) {
            slaves[SlaveCnt].peer_addr[ii] = (uint8_t) mac[ii];
            Slaves_Info.Slave_MAC[SlaveCnt][ii] = (uint8_t) mac[ii];
          }
        }
        if(SSID == "CAISSE1"){digitalWrite(LED_CAISSE1, HIGH);};
        if(SSID == "CAISSE2"){digitalWrite(LED_CAISSE2, HIGH);};
        if(SSID == "CAISSE3"){digitalWrite(LED_CAISSE3, HIGH);};
        if(SSID == "CAISSE4"){digitalWrite(LED_CAISSE4, HIGH);};
        
        slaves[SlaveCnt].channel = CHANNEL; // pick a channel
        slaves[SlaveCnt].encrypt = 0; // no encryption
        esp_err_t addStatus = esp_now_add_peer(&slaves[SlaveCnt]);
        if (addStatus == ESP_OK) {// Pair success
          Serial.println("Pair success");
        Slaves_Info.Slave_Number[SlaveCnt] = SlaveCnt+1;
        Slaves_Info.Slave_SSID[SlaveCnt] = SSID;
        Slaves_Info.Slave_Signal[SlaveCnt] = RSSI;   
        Slaves_Info.Slave_isFauly[SlaveCnt] = 0;     
        } else if (addStatus == ESP_ERR_ESPNOW_NOT_INIT) {Serial.println("ESPNOW Not Init");
        } else if (addStatus == ESP_ERR_ESPNOW_ARG) {Serial.println("Add Peer - Invalid Argument");
        } else if (addStatus == ESP_ERR_ESPNOW_FULL) {Serial.println("Peer list full");
        } else if (addStatus == ESP_ERR_ESPNOW_NO_MEM) {Serial.println("Out of memory");
        } else if (addStatus == ESP_ERR_ESPNOW_EXIST) {Serial.println("Peer Exists");
        } else {Serial.println("Not sure what happened");
        }
        SlaveCnt++;
      }
    }
  }
  if (SlaveCnt > 0) {
    Serial.print(SlaveCnt); Serial.println(" Slave(s) found, processing..");
  } else {Serial.println("No CAISSE Found, trying again."); }
  WiFi.scanDelete();// clean up ram
}
