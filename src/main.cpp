#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

int confirmacion;
int contador=0;

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
const int pir = 14;

//Tipo de dato estructura
typedef struct struct_message {
    int a;
} struct_message;

// Crear dos datos con el formato estructura 
struct_message myDataRec;
struct_message myDataSen;

esp_now_peer_info_t peerInfo;

// Funcion para  enviar datos
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  if (status == ESP_NOW_SEND_SUCCESS){
    confirmacion=1;
   }else{
   confirmacion=2;
   }
}

// Esta funcion se ejecuta cuando se recibe el dato
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myDataRec, incomingData, sizeof(myDataRec));
}
 
void setup() {
  //Datos para la comunicacion Monitor Serie
  Serial.begin(9600);
  pinMode(pir, INPUT_PULLDOWN);
  // Configuracion del modo Wifi
  
  WiFi.mode(WIFI_STA);

  // Inicializa el protocolo esp-now
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  // Funcion para recibir el paquete 
  esp_now_register_recv_cb(OnDataRecv);
  // Funcion para el envio de datos
  esp_now_register_send_cb(OnDataSent);
  //Este apartado del peer es utilizado para el envio del paquete
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  // Add peer 
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}
 
void loop() {
  if (digitalRead(pir) == HIGH){
    // Set values to send
    myDataSen.a = 2;
    // Enviar el mensaje ESP-NOW
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myDataSen, sizeof(myDataSen));
    while(contador<300){
      Serial.print("esto vale contador: ");
      Serial.println(contador);
      if(digitalRead(pir)==HIGH){contador=0;}
      delay(2000);
      contador++;
    }
  } else {
    myDataSen.a=0; 
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myDataSen, sizeof(myDataSen));
  }
  delay(2000);
}