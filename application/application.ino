//Aplicatia preia informatii din aer(temperatura, umiditatea, calitatea aerului) si le trimite prin bluetooth pe smarphone(aplicatia bluetooth terminal) 

//biblioteci
#include "BluetoothSerial.h"
#include "DHT.h"
#include "WiFi.h"




//definire detalii senzor
String locatieDevice="Room1";

//definire sensor calitate aer + variabile
int pinSenzorAer = 35;
float senzorAer;
float umiditate;
float temperatura;
String temperatureString = "";
String umiditateString="";
String calitateAerString="";

//definire sensor DHT22(temperatura+umiditate) +  variabile
#define DHTPIN 4 
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);



//definire obiecte Bluetooth
BluetoothSerial SerialBT;
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// TREBUIE INLOCUIT CU CREDENTIALELE WIFI PERSONALE
String ssid;
String password;
int reteaAleasa;


void setup() {
//pornire senzori umiditate+aer:
  dht.begin();
//pornire terminal pc pentru debug:
  Serial.begin(115200);
//setare nume aparat bluetooth:
  SerialBT.begin("ESP32");
//afisare in terminalul PC caa setarile initiale au fost efectuate:
  //Serial.println("Aparatul este gata de imperecheat cu smarphone-ul");

delay(5000);

ssid=(String)alegereRetea();
password=(String)cerereParola();
SerialBT.println(ssid);
SerialBT.println(password);
delay(1000);
WIFIConnect(ssid, password);

}

void loop() {
//delay de 10 secunde la fiecare iteratie:
  delay(1000);
//apelare functie de citire a informatiilor receptate de senzorului de calitate al aerului:
 // CitireSenzorAer();
//apelare functie de citire a informatiilor receptate de senzori de umiditate si temperatura:
 // CitireDHTSensors();
//apelarea functiei prin care se trimit informatiile catre telefon:
  //trimitereParametriiBluetooth();

}


String alegereRetea(){
    SerialBT.println("Incepe Scanarea");
    // Aceasta functie intoare numarul de retele gasite:
    int n = WiFi.scanNetworks();
    delay(2500);
    SerialBT.println("S-a terminat scanarea");
    if (n == 0) {
        SerialBT.println("Retele gasite: ");
    } else {
        SerialBT.print("Retele gasite:  ");
        SerialBT.print(n);
        SerialBT.println("");
        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            SerialBT.print(i + 1);
            SerialBT.print(": ");
            SerialBT.print(WiFi.SSID(i));
            SerialBT.print(" (");
            SerialBT.print(WiFi.RSSI(i));
            SerialBT.print(")");
            SerialBT.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
            delay(10);
        }
    }
    SerialBT.println("");
    SerialBT.println("Alegeti o retea din lista: ");
    
//cele doua device-uri sunt in asteptare pentru a citi streamul venit de la telefon:
while (true) {
  if (SerialBT.available()) {//daca vin biti de la telefon
    reteaAleasa = SerialBT.read()-48; //numerele pornesc de la 48 cu aplicatia "serial bluetooth terminal"
    SerialBT.flush();
    break;
  }
  
}   

//iterarea listei cu retelele disponibile
    for (int i = 0; i < n; ++i) {
      if(reteaAleasa==i+1){//daca numarul retelei introduse este egal cu i+1 pentru ca array-ul incepe de la 0, dar in terminal prima retea are pozitia 1
        ssid=(WiFi.SSID(i));//atunci reteaua aleasa este reteaua de pe pozitia i
        
      }
    }
    
//este chemata functia de mai jos care conecteaza device-ul la wifi:
return ssid;

}

String cerereParola(){
 //alegerea parolei:
 SerialBT.println("Introduceti parola:"); 
  char c;
  String pass; 
  char test='\0';
  String passTemp;
//daca vin biti de la telefon:
while (true) {//bucla infinita care asteapta alegerea unei parole      
  if (SerialBT.available()) {
         //passTemp=SerialBT.readStringUntil('\n');
         //SerialBT.println(passTemp);
      c = (char)SerialBT.read ();
      pass += c; 
      }  
      //SerialBT.flush();
  if (c == '\n'){
    SerialBT.flush();
    break;
     }
     
}
String finalPass;
const char* testPass = pass.c_str();
for(int i=0;i<pass.length()-2;i++){
  finalPass=finalPass+testPass[i];
}
return finalPass;
}


void WIFIConnect(String ssid, String password){

  SerialBT.print("Conectare....");
  WiFi.begin(ssid.c_str(), password.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    SerialBT.print(".");
    int count=1;
    delay(200);
   
  }

  SerialBT.println();
  SerialBT.println("WiFi conectat.");
  SerialBT.println();
}




void trimitereParametriiBluetooth(){
  umiditateString= "umiditate: "+ String(umiditate)+" %";
  temperatureString = "temperatura: "+String(temperatura)+" *C";
  calitateAerString = "calitate aer: "+String(senzorAer)+ "/900";
  SerialBT.println(temperatureString);
  SerialBT.println(umiditateString);
  SerialBT.println(calitateAerString);
  SerialBT.println();
  SerialBT.println();
}

void CitireSenzorAer(){
  senzorAer=analogRead(pinSenzorAer);
}

void CitireDHTSensors(){
umiditate= dht.readHumidity();
temperatura = dht.readTemperature();

}
