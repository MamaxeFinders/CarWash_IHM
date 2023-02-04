#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define NUMSYSTEM 1
#define PRINTSCANRESULTS 0
int SystemCnt; //Count the number of SYSTEM found (should be 1 only)
int CHANNEL;
uint8_t peer_addr[6];
float Pulse_to_Credit = 1.00; //Factor for each pulse
int Initial_Credit = 0; //Initial credit entered before START
float Remaining_Credit = 0.00; //Credit left after START
long startMillis = 0;
bool Session_Started = false;
bool Button_pressed = false;
long counter = 0;
float Program_Factor[] = {0.00, 1.00, 1.00,1.00,1.00}; //Factor for each progam (Program_Factor[0] not used)

#define PULSE_INPUT 32
#define SHOCK_INPUT 13

#define PROG_1_INPUT 33
#define PROG_2_INPUT 25
#define PROG_3_INPUT 26
#define PROG_4_INPUT 27
#define STOP_INPUT 14
//#define EXTRA_INPUT 12

#define CARDADDRESS_1_IN 35
#define CARDADDRESS_2_IN 34
#define CARDADDRESS_3_IN 39
int Card_Address;

#define LED_OUT 15

#define RELAY_1_OUT 19
#define RELAY_2_OUT 18
#define RELAY_3_OUT 5
#define RELAY_4_OUT 17
#define RELAY_5_OUT 16
#define RELAY_6_OUT 4
#define RELAY_7_OUT 0
#define RELAY_8_OUT 2
int RELAYS[] = {RELAY_1_OUT,RELAY_2_OUT,RELAY_3_OUT,RELAY_4_OUT,RELAY_5_OUT,RELAY_6_OUT,RELAY_7_OUT,RELAY_8_OUT};

// ---- COOIN PULSE ---- //
unsigned long Pulse_time = 0; //variables to keep track of the timing of recent interrupts
unsigned long last_Pulse_time = 0;
volatile int Coin_Pulses = 0;
void IRAM_ATTR PULSE_PROG() {
  Pulse_time = millis();
  if (Pulse_time - last_Pulse_time > 1000) {
    last_Pulse_time = Pulse_time;
    Coin_Pulses++;
  }
}
// ---- EXTRA PULSE ---- //
unsigned long Extra_time = 0; //variables to keep track of the timing of recent interrupts
unsigned long Last_Extra_time = 0;
void IRAM_ATTR EXTRA_PROG() {
  Extra_time = millis();
  if (Extra_time - Last_Extra_time > 1000) {
    Last_Extra_time = Extra_time;
    Coin_Pulses = Coin_Pulses + 2;
  }
}

int Number_of_Shocks = 0;
bool Shock_Triggered = false;
unsigned long Shock_time = 0; //variables to keep track of the timing of recent interrupts
unsigned long last_Shock_time = 0;
volatile int Shock_count = 0;
void IRAM_ATTR SHOCK_TRIGGER(){
  if (millis() - last_Shock_time > 1000) { //Check how many pulse within 1000 ms
    last_Shock_time = Shock_time;
    if(Shock_count > 80){Shock_Triggered = true;}
  }  Shock_count++;
}

// ---- BUTTONS WITH INTERRUPT ---- //
volatile int BUTTON_PROG = 0;
void IRAM_ATTR BUTTON_PROG_1() {BUTTON_PROG = 1;}
void IRAM_ATTR BUTTON_PROG_2() {BUTTON_PROG = 2;}
void IRAM_ATTR BUTTON_PROG_3() {BUTTON_PROG = 3;}
void IRAM_ATTR BUTTON_PROG_4() {BUTTON_PROG = 4;}
void IRAM_ATTR BUTTON_STOP() {BUTTON_PROG = 10;}

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

esp_now_peer_info_t System[NUMSYSTEM] = {};
typedef struct struct_System_Info  {/*KEEP*/
  int System_Number;
  String System_SSID;
  String System_MAC;
  int32_t System_Signal;
  int8_t System_isFauly;
} struct_System_Info; struct_System_Info System_Info; /*KEEP*/

typedef struct struct_message_to_System { /*KEEP*/
  int CardCAISSE;
  int Request = 0;
  float Credit = 00.00;
  int RequestCode = 0;
  int Shocks = 0;
} struct_message_to_System; struct_message_to_System message_to_System; /*KEEP*/

typedef struct struct_message_From_System { /*KEEP*/
  int CardCAISSE;
  int Request;
  float Credit;
  int Message;
} struct_message_From_System; struct_message_From_System message_From_System; /*KEEP*/

// ---- SEND DATA ---- //
esp_now_peer_info_t peerInfo; /*KEEP*/
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) { /*KEEP*/
  char macStr[18];
  int MACtest = snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Sent to: "); Serial.println(MACtest); //Serial.println(macStr);
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
void SendMessage(int a, int b, float c, int d, int e) { // send data
  if(Card_Address > 0){
  message_to_System.CardCAISSE = a;
  message_to_System.Request = b;
  message_to_System.Credit = c;
  message_to_System.RequestCode = d;
  message_to_System.Shocks = e;

  char macStr[18];snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",peer_addr[0], peer_addr[1], peer_addr[2], peer_addr[3], peer_addr[4], peer_addr[5]);
  memcpy(&peerInfo.peer_addr, peer_addr, 6);
  Serial.print("Peer MAC2: "); Serial.println(macStr);
  if (!esp_now_is_peer_exist(peer_addr)) {esp_now_add_peer(&peerInfo);}
  esp_err_t result = esp_now_send(peer_addr, (uint8_t *) &message_to_System, sizeof(message_to_System));
  if (result == ESP_OK)  {Serial.println("Message sent");}
  else if (result == ESP_ERR_ESPNOW_NOT_INIT)  {Serial.println("ESPNOW not Init.");}
  else if (result == ESP_ERR_ESPNOW_ARG)       {Serial.println("Invalid Argument");}
  else if (result == ESP_ERR_ESPNOW_INTERNAL)  {Serial.println("Internal Error");}
  else if (result == ESP_ERR_ESPNOW_NO_MEM)    {Serial.println("ESP_ERR_ESPNOW_NO_MEM");}
  else if (result == ESP_ERR_ESPNOW_NOT_FOUND) {Serial.println("Peer not found.");}
  else  {Serial.println("Unknown error");}
  }}

// ---- callback when data is recv from System ---- //
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int data_len) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  memcpy(&message_From_System, incomingData, sizeof(message_From_System));
  Serial.print("Massage received by: "); Serial.println(macStr);
  Serial.print("Data size: "); Serial.println(sizeof(message_From_System));
    if(message_From_System.Request == 2){ // Request 2 for Credit
      Serial.print("New Credit of: "); Serial.println(message_From_System.Credit);
      Coin_Pulses = Coin_Pulses + message_From_System.Credit;
    }
}
// ======================================== SETUP ======================================== //
void setup() {
  Serial.begin(115200);
  // ---- PINS SETUP ---- //
  pinMode(LED_OUT, OUTPUT);
  pinMode(RELAY_1_OUT, OUTPUT); pinMode(RELAY_2_OUT, OUTPUT); pinMode(RELAY_3_OUT, OUTPUT); pinMode(RELAY_4_OUT, OUTPUT);
  pinMode(RELAY_6_OUT, OUTPUT); pinMode(RELAY_7_OUT, OUTPUT); pinMode(RELAY_4_OUT, OUTPUT); pinMode(RELAY_8_OUT, OUTPUT);
  pinMode(SHOCK_INPUT, INPUT_PULLUP);attachInterrupt(digitalPinToInterrupt(SHOCK_INPUT), SHOCK_TRIGGER, RISING);
  pinMode(PULSE_INPUT, INPUT_PULLUP);attachInterrupt(digitalPinToInterrupt(PULSE_INPUT), PULSE_PROG, FALLING);
  pinMode(PROG_1_INPUT, INPUT_PULLUP);attachInterrupt(digitalPinToInterrupt(PROG_1_INPUT), BUTTON_PROG_1, FALLING);
  pinMode(PROG_2_INPUT, INPUT_PULLUP);attachInterrupt(digitalPinToInterrupt(PROG_2_INPUT), BUTTON_PROG_2, FALLING);
  pinMode(PROG_3_INPUT, INPUT_PULLUP);attachInterrupt(digitalPinToInterrupt(PROG_3_INPUT), BUTTON_PROG_3, FALLING);
  pinMode(PROG_4_INPUT, INPUT_PULLUP);attachInterrupt(digitalPinToInterrupt(PROG_4_INPUT), BUTTON_PROG_4, FALLING);
  pinMode(STOP_INPUT, INPUT_PULLUP);attachInterrupt(digitalPinToInterrupt(STOP_INPUT), BUTTON_STOP, FALLING);

  // -- CARD ADDRESS -- //
  pinMode(CARDADDRESS_1_IN, INPUT); /*KEEP*/
  pinMode(CARDADDRESS_2_IN, INPUT); /*KEEP*/
  pinMode(CARDADDRESS_3_IN, INPUT); /*KEEP*/
  delay(2000);
  Card_Address = digitalRead(CARDADDRESS_1_IN) * 1 + digitalRead(CARDADDRESS_2_IN) * 2 + digitalRead(CARDADDRESS_3_IN) * 4;
  System_Info.System_SSID = "CAISSE" + String(Card_Address);
  Serial.print("CardCAISSE_ID : ");Serial.println(System_Info.System_SSID);

  // ---- LCD SETUP ----//
  lcd.init();  // initialize the lcd
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print(" INITIALISATION ");
  lcd.setCursor(0, 1); lcd.print(String(System_Info.System_SSID));

  // ---- WIFI SETUP ----//
  if(Card_Address == 0){
    Serial.println("MODE SOLO");
    Coin_Pulses = 2.00;
    for(int i=0;i<2;i++){
      digitalWrite(LED_OUT, HIGH);delay(1000);
      digitalWrite(LED_OUT, LOW);delay(1000);
    }
  }else{
    ScanForSystem();
    configDeviceAP(System_Info.System_SSID);
    InitESPNow();    
    Serial.print("CAISSE OK : ");Serial.print("AP MAC: "); Serial.println(WiFi.softAPmacAddress());
  }
 
  Set_Relays(7); //Acctivate coin acceptor
  digitalWrite(LED_OUT, HIGH);
  lcd.clear();    
  lcd.setCursor(0, 0); lcd.print("BONJOUR         ");
  lcd.setCursor(0, 1); lcd.print("INSEREZ PIECE   ");
}
// ======================================== LOOP ======================================== //
void loop() {
// ---- IF SHOCK ---- //
  if(Shock_Triggered){
    Shock_Triggered = false;
    Number_of_Shocks++;
    Serial.print("NUMBER OF SHOCKS : ");Serial.println(Number_of_Shocks);   
    for(int s = 0; s < Number_of_Shocks; s++){
      lcd.clear();lcd.setCursor(0, 1); lcd.print("! ALERTE CHOC !");
      Set_Relays(8); // Alarm
      delay(1000);
      if(Number_of_Shocks > 2){ //Stop the program at third shock
        delay(5000);
        BUTTON_PROG = 10;
        Number_of_Shocks = 0;
      }
      Set_Relays(0);
      delay(1000);
    }
    if(Coin_Pulses == 0 && BUTTON_PROG < 1){
      lcd.clear();    
      lcd.setCursor(0, 0); lcd.print("BONJOUR         ");
      lcd.setCursor(0, 1); lcd.print("INSEREZ PIECE   ");
    }
    SendMessage(Card_Address, 9, Remaining_Credit,BUTTON_PROG,Number_of_Shocks); //Send CardCAISSE,Request,Credit,RequestCode
  } else if (BUTTON_PROG == 10 || Remaining_Credit < 0) {
// PROGRAM STOP //
    Serial.println("COUNT STOP");
    lcd.clear();lcd.setCursor(6, 1);lcd.print("STOP");
    Initial_Credit = 0; //Initial credit entered before START
    Remaining_Credit = 0; //Credit left after START
    startMillis = 0;
    Session_Started = false;
    Button_pressed = false;
    counter = 0;
    Coin_Pulses = 0;
    BUTTON_PROG = 0;
    Number_of_Shocks = 0;
    Set_Relays(0);
    delay(5000);
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print("BONJOUR         ");
    lcd.setCursor(0, 1); lcd.print("INSEREZ PIECE   ");
    Set_Relays(7); //Activate coin acceptor
  }else if (Coin_Pulses > 0 && BUTTON_PROG < 1) {
// ---- IF FIRST COIN INSERTED ---- //    
    Number_of_Shocks = 0;
    Initial_Credit = Pulse_to_Credit * Coin_Pulses;
    Remaining_Credit = Initial_Credit;
    Serial.print("INITIAL CREDIT : ");
    Serial.println(Initial_Credit);
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("CREDIT : " + String(float(Initial_Credit)) + " E");  
  } else if (BUTTON_PROG > 0 && Initial_Credit > 0) {
// ---- PROGRAM START ---- //      
    if (!Session_Started) {
      startMillis = millis();
      Coin_Pulses = 0;
      Session_Started = true;
      Remaining_Credit = Initial_Credit;
      SendMessage(Card_Address, 1, Remaining_Credit, BUTTON_PROG,0); //Send CardCAISSE,Request,Credit,RequestCode
    }
    Set_Relays(BUTTON_PROG);
    counter = (millis() - startMillis) / 1000;
    startMillis = millis();
    Remaining_Credit = Remaining_Credit - ((float)counter * Program_Factor[BUTTON_PROG] / 20.0 / 3.0); // equivalent to 5 sents every 1 sec
    Serial.print("PROGRAM : "); Serial.print(String(BUTTON_PROG));Serial.print(" REM CREDIT : "); Serial.println(String(Remaining_Credit));
    lcd.setCursor(0, 0);lcd.print("PROGRAM  " + String(BUTTON_PROG) + "      ");
    lcd.setCursor(0, 1);lcd.print("CREDIT : " + String(float(Remaining_Credit)) + " E");
  }
  delay(1000);
}
// ======================================== FUNCTIONS ======================================== //
// ---- SET RELAYS ---- //
void Set_Relays(int v){
  for(int p = 1; p <= (sizeof(RELAYS)/sizeof(RELAYS[0])) ; p++){
    if(v == p){
      digitalWrite(RELAYS[p-1], HIGH);
    }else{
      digitalWrite(RELAYS[p-1], LOW);
    }
  }
}

// ---- Init ESP Now with fallback ---- //
void InitESPNow() {
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);
  }  else {
    Serial.println("ESPNow Init Failed");
    ESP.restart();
  }
}

// ---- config AP SSID ---- //
void configDeviceAP(String SSID) {
  String Password = "123456789";
  bool result = WiFi.softAP(SSID.c_str(), Password.c_str(), CHANNEL, 0);
  if (!result) {
    Serial.println("AP Config failed.");
  } else {
    Serial.print("AP Config Success for : ");
    Serial.println(String(SSID));
  }
}

// ---- SCAN FOR SYSTEM ----//
void ScanForSystem() {
  WiFi.mode(WIFI_AP_STA); //Optional
  int8_t scanResults = WiFi.scanNetworks();
  memset(System, 0, sizeof(System));
  SystemCnt = 0;
  if (scanResults == 0) {
    Serial.println("No WiFi devices in AP Mode found");
  } else {
    Serial.print("Found "); Serial.print(scanResults); Serial.println(" devices ");
    for (int i = 0; i < scanResults; ++i) {
      // Print SSID and RSSI for each device found
      String SSID = WiFi.SSID(i); //Provide the name of the network found
      int32_t RSSI = WiFi.RSSI(i); //Check signal performances from -100 to 0
      String BSSIDstr = WiFi.BSSIDstr(i); //Basic Service Set Identification) that is another name of MAC address of a network discovered during the scan
      if (PRINTSCANRESULTS) {
        Serial.print(i + 1); Serial.print(": "); Serial.print(SSID); Serial.print(" ["); Serial.print(BSSIDstr); Serial.print("]"); Serial.print(" ("); Serial.print(RSSI); Serial.print(") ch: "); Serial.println(CHANNEL);
      }
      delay(10);
      // Check if the current device starts with `CAISSE`
      if (SSID.indexOf("SYSTEM_CAISSE") == 0 && SystemCnt < NUMSYSTEM) {
        CHANNEL = WiFi.channel(i);
        Serial.print(i + 1); Serial.print(": "); Serial.print(SSID); Serial.print(" ["); Serial.print(BSSIDstr); Serial.print("]"); Serial.print(" ("); Serial.print(RSSI); Serial.print(") ch: "); Serial.println(CHANNEL);
        // Get BSSID => Mac Address of the System
        int mac[6];
        if ( 6 == sscanf(BSSIDstr.c_str(), "%x:%x:%x:%x:%x:%x",  &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]) ) {
          for (int ii = 0; ii < 6; ++ii ) {
            System[SystemCnt].peer_addr[ii] = (uint8_t) mac[ii];
            peer_addr[ii] = (uint8_t) mac[ii]; // For unique SYSTEM
          }
        }
        System[SystemCnt].channel = CHANNEL; // pick a channel
        System[SystemCnt].encrypt = 0; // no encryption
        esp_err_t addStatus = esp_now_add_peer(&System[SystemCnt]);
        if (addStatus == ESP_OK) {// Pair success
          Serial.println("Pair success");
          System_Info.System_Number = SystemCnt + 1;
          System_Info.System_SSID = SSID;
          System_Info.System_MAC = BSSIDstr;
          System_Info.System_Signal = RSSI;
          System_Info.System_isFauly = 0;
        } else if (addStatus == ESP_ERR_ESPNOW_NOT_INIT) {Serial.println("ESPNOW Not Init");
        } else if (addStatus == ESP_ERR_ESPNOW_ARG) {Serial.println("Add Peer - Invalid Argument");
        } else if (addStatus == ESP_ERR_ESPNOW_FULL) {Serial.println("Peer list full");
        } else if (addStatus == ESP_ERR_ESPNOW_NO_MEM) {Serial.println("Out of memory");
        } else if (addStatus == ESP_ERR_ESPNOW_EXIST) {Serial.println("Peer Exists");
        } else {Serial.println("Not sure what happened");}
        SystemCnt++;
      }
    }
  }
  if (SystemCnt > 0) {Serial.print(SystemCnt); Serial.println(" SYSTEM found,");
  } else {Serial.print("No SYSTEM Found => Restarting");
    for(int i = 0; i < 3; i++){
      Serial.print(".");
      digitalWrite(LED_OUT, HIGH);delay(200);
      digitalWrite(LED_OUT, LOW);delay(200);
    }Serial.println(".");
    lcd.clear();
    delay(2000);
    ESP.restart();
  }
  WiFi.scanDelete();// clean up ram
}
