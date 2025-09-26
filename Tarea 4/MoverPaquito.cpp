#include "WiFiEsp.h"
#include "WiFiEspUdp.h"

// --------------------- CLASE RUEDA ---------------------
class Rueda {
public:
    int pin1;
    int pin2; 
    int speedPin;

    Rueda(int pin1, int pin2, int speedPin) {
        this->pin1 = pin1;
        this->pin2 = pin2;
        this->speedPin = speedPin;
        pinMode(pin1, OUTPUT); pinMode(pin2, OUTPUT);  pinMode(speedPin, OUTPUT); 
    }

    void fwd(int speed) {
        digitalWrite(pin1, LOW); digitalWrite(pin2, HIGH); analogWrite(speedPin, speed);
    }

    void bck(int speed) {
        digitalWrite(pin1, HIGH); digitalWrite(pin2, LOW); analogWrite(speedPin, speed);
    }

    void stop() {
        analogWrite(speedPin, 0);
    }
};


// --------------------- CLASE PAQUITO ---------------------
class Paquito {
public:
    Rueda frontR;
    Rueda backR;
    Rueda frontL;
    Rueda backL;

    Paquito() 
      : frontR(22, 24, 9),
        backR(5, 6, 11),
        frontL(26, 28, 10),
        backL(7, 8, 12) 
    {}

    /*
     * Avanza
     */
    void go_advance(int speed){
       backL.fwd(speed);
       backR.fwd(speed);
       frontR.fwd(speed);
       frontL.fwd(speed); 
    }

    /*
     * Retrocede
     */
    void go_back(int speed){
       backL.bck(speed);
       backR.bck(speed);
       frontR.bck(speed);
       frontL.bck(speed); 
    }

    /*
     * Gira a la izquierda
     */
    void left_turn(int speed) { 
       backL.fwd(0);
       backR.fwd(speed);
       frontR.fwd(speed);
       frontL.fwd(0); 
    }
    
    /*
     * Gira a la derecha
     */
    void right_turn(int speed) {
       backL.fwd(speed);
       backR.bck(0);
       frontR.bck(0);
       frontL.fwd(speed); 
    }


    /*
     * Retrocede girando a la izquierda
     */
    void left_back(int speed) {
       backL.fwd(0);
       backR.bck(speed);
       frontR.bck(speed);
       frontL.fwd(0); 
    }

    /*
     * Retrocede girando a la derecha
     */
    void right_back(int speed) {
       RL_bck(speed);
       RR_fwd(0);
       FR_fwd(0);
       FL_bck(speed); 
    }

    /*
     * Gira sobre su eje en la dirección de las manecillas del reloj
     */
    void clockwise(int speed) {
       RL_fwd(speed);
       RR_bck(speed);
       FR_bck(speed);
       FL_fwd(speed); 
    }

    /*
     * Gira sobre su eje en dirección contraria a las manecillas del reloj
     */
    void countclockwise(int speed) {
       RL_bck(speed);
       RR_fwd(speed);
       FR_fwd(speed);
       FL_bck(speed); 
    }

    /*
     * Desplazamiento horizontal a la derecha
     */
    void right_shift(int speed) {
      frontL.fwd(speed); 
      backL.bck(speed); 
      backR.fwd(speed);
      frontR.bck(speed);
    }

     /*
     * Desplazamiento horizontal a la izquierda
     */
    void left_shift(int speed){
      frontL.bck(speed);
      backL.fwd(speed);
      backR.bck(speed);
      frontR.fwd(speed);
    }


    /*
     * Diagonal hacia adelante-derecha
     */
    void diagonal_fwd_right(int speed) {
        frontL.fwd(speed);
        backR.fwd(speed);
        frontR.fwd(0);
        backL.fwd(0);
    }

    /*
     * Diagonal hacia adelante-izquierda
     */
    void diagonal_fwd_left(int speed) {
        frontR.fwd(speed);
        backL.fwd(speed);
        frontL.fwd(0);
        backR.fwd(0);
    }

    /*
     * Diagonal hacia atrás-derecha
     */
    void diagonal_back_right(int speed) {
        frontR.bck(speed);
        backL.bck(speed);
        frontL.bck(0);
        backR.bck(0);
    }

    /*
     * Diagonal hacia atrás-izquierda
     */
    void diagonal_back_left(int speed) {
        frontL.bck(speed);
        backR.bck(speed);
        frontR.bck(0);
        backL.bck(0);
    }


    /*
    * Detener todas las ruedas
    */
    void stopAll(){ frontR.stop(); backR.stop(); frontL.stop(); backL.stop();}

    void FL_fwd(int speed){frontL.fwd(speed);}
    void FR_fwd(int speed){frontR.fwd(speed);}
    void RL_fwd(int speed){ backL.fwd(speed);}
    void RR_fwd(int speed){backR.fwd(speed);}

    void frena_frente_izq(){analogWrite(speedPinL,0);}
    void frena_frente_der(){analogWrite(speedPinR,0);}
    void frena_tras_izq() {analogWrite(speedPinLB,0);}
    void frena_tras_der() {analogWrite(speedPinRB,0);}

};


// --------------------- CLASE WiFiPaquito ---------------------
class WiFiPaquito {
public:
    #ifndef HOTSPOT
    char ssid[20] = "********";
    char pass[20] = "********";
    #else
    char ssid[10] = "paq";
    #endif

    int status = WL_IDLE_STATUS;
    char packetBuffer[5];
    WiFiEspUDP Udp;
    unsigned int localPort = 8888;
    Paquito paquito;
    static const int SPEED = 150;

    void init_WiFi() {
      Serial1.begin(115200);
      Serial1.write("AT+UART_DEF=9600,8,1,0,0\r\n");
      delay(200);
      Serial1.write("AT+RST\r\n");
      delay(200);
      Serial1.begin(9600);
      WiFi.init(&Serial1);

      if (WiFi.status() == WL_NO_SHIELD) {
        Serial.println("WiFi shield not present");
        while (true);
      }
      while (status != WL_CONNECTED) {
        #ifndef HOTSPOT
        status = WiFi.begin(ssid, pass);
        #else
        status = WiFi.beginAP(ssid, 10, "", 0);
        #endif
      }
      
      Serial.println("You're connected to the network");
      printWifiStatus();
      Udp.begin(localPort);
      
      Serial.print("Listening on port ");
      Serial.println(localPort);
    }

    void mover() {
       int packetSize = Udp.parsePacket();
      if (packetSize) {                               
        Serial.print("Received packet of size ");
        Serial.print(packetSize);
        int len = Udp.read(packetBuffer, 255);
        if (len > 0) {
          packetBuffer[len] = 0;
        }
        char c = packetBuffer[0];
        Serial.print(" char: ");
        Serial.println(c);
        switch (c)    //serial control instructions
        {  
          // Avanza llanta por llanta
          case '1':paquito.FL_fwd(SPEED);break;
          case '2':paquito.FR_fwd(SPEED);break;
          case '3':paquito.RL_fwd(SPEED);break;
          case '4':paquito.RR_fwd(SPEED);break;
          // Frena llanta por llanta
          case '5':paquito.frena_frente_izq();break;
          case '6':paquito.frena_frente_der();break;
          case '7':paquito.frena_tras_izq();break;
          case '8':paquito.frena_tras_der();break;

          // Mueve el carrito
          case 'a':paquito.go_advance(SPEED);break;         //Avanzar
          case 'b':paquito.go_back(SPEED);break;            //Retroceder
          case 'e':paquito.stopAll(); break;                //Detener
          case 'l':paquito.left_turn();break;               //Girar izquierda
          case 'r':paquito.right_turn();break;              //Girar derecha  

          case 'w':paquito.diagonal_fwd_right(SPEED); break; //Diagonal delante derecha
          case 'x':paquito.diagonal_fwd_left(SPEED); break; //Diagonal delante izquierda
          case 'y':paquito.diagonal_back_right(SPEED); break; //Diagonal atras derecha
          case 'z':paquito.diagonal_back_left(SPEED); break; //Diagonal atras izquierda

          case 'c':paquito.right_shift(SPEED);break; //Desplazamiento horizontal derecha
          case 'd':paquito.left_shift(SPEED);break; //Desplazamiento horizontal izquierda

          default:break;
        }
      }
     
    }
};

// --------------------- OBJETO GLOBAL ---------------------
WiFiPaquito robot;

// --------------------- SETUP Y LOOP ---------------------
void setup() {
  Serial.begin(9600);
  robot.init_WiFi();
}

void loop() {
  robot.mover();
}

