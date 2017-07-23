#include <PID_v1.h>
#include <SPI.h>


const int ChipSelect      = 8;         //Chip-Select am Pin 8 des Arduino
const int AnalogPin       = A0;        //Liest den aktuellen Volt-Wert

const int wiper0writeAddr = B00000000; //Schreibadresse für Poti 0 des Digi-Pot
const int wiper1writeAddr = B00010000; //Schreibadresse für Poti 1 des Digi-Pot

const int led             = 9;    //Pin 9: LED

int x_wert                = 320;  //Wert der X-Koordinate 
int y_wert                = 240;  //Wert der Y-Koordinate
int digipot_wert          = 127;  //Wert zwischen 0 und 255 für das Digital-Potentiometer

String inString           = "";   //String für die Serial-Kommunikation

const int printing        = 1;    //Aktiviert oder deaktviert die Serial-Ausgabe


/* ############## PID #################*/

//Definiert die Variablen für den PID-Regler
double SetpointX, InputX, OutputX;
double SetpointY, InputY, OutputY;

//Die Einstellung der Werte des P-, I- und D-Anteils
double Kp=0.65,
       Ki=0.02,
       Kd=0.18;

//Initialisiert die Regler für die X- und Y-Koordinate
PID PIDX(&InputX, &OutputX, &SetpointX, Kp, Ki, Kd, DIRECT);
PID PIDY(&InputY, &OutputY, &SetpointY, Kp, Ki, Kd, DIRECT);

void setup() {
  
  // Setzt Chip-Select als AUSGANG und deaktviert ihn:
  pinMode(ChipSelect, OUTPUT);
  pinMode(ChipSelect, HIGH);

  // Initialisiert SPI und Serial:
  SPI.begin();
  Serial.begin(38400);
  Serial.println("Starte Programm\n");

  //LED als AUSGANG
  pinMode(led, OUTPUT);

  //Initialisiert beide Kanäle des Digital-Poti auf den Wert 127
  PotiSteuerung(127, 0);
  PotiSteuerung(127, 1);

  //Ausgangswerte für den Regler
  InputX = 320;
  InputY = 240;
  
  SetpointX = 320;
  SetpointY = 240;

  OutputX = 127;
  OutputY = 127;
    
  PIDX.SetMode(AUTOMATIC);
  PIDY.SetMode(AUTOMATIC);
  
}

void loop() {
  
  while (Serial.available() > 0) {
  SerialVerbindung();
  }

  InputX = x_wert;
  InputY = y_wert;
  
  PIDX.SetTunings(Kp, Ki, Kd);
  PIDY.SetTunings(Kp, Ki, Kd);
  
  PIDX.Compute();
  PIDY.Compute();

  PotiSteuerung(OutputX, 0);
  PotiSteuerung(OutputY, 1);

  Print();

  LedSteuerung();
}


void PotiSteuerung(int digipot_wert, int poti01){

  //Sendet den Einstellwert an den jeweiligen Kanal des Digital-Poti
  
  int adresse;
  if(poti01 == 0){adresse = wiper0writeAddr;}
  if(poti01 == 1){adresse = wiper1writeAddr;}

  digitalWrite(ChipSelect,LOW);
  SPI.transfer(adresse);
  SPI.transfer(digipot_wert);
  digitalWrite(ChipSelect,HIGH);
}

void SerialVerbindung(){

    //Empfängt die X- und Y-Werte des getrackten Objektes im Kamerabild
    
    int inChar = Serial.read();
    if (isDigit(inChar)) {
      inString += (char)inChar;
    }

    if(inChar == 42) {

      x_wert = inString.toInt();

      Print();
      inString = "";
    }
    
    if (inChar == 43) {

      y_wert = inString.toInt();
    
      Print();
      inString = "";
    }


}

void LedSteuerung(){

  //LED wird aktiviert, wenn die Abstände beider Achsen nahe am Sollwert liegen
  
  double abstandX = abs(SetpointX-InputX);
  double abstandY = abs(SetpointY-InputY);

  if(abstandX < 30 && abstandY < 30){digitalWrite(led, HIGH);}
  else{digitalWrite(led, LOW);}
  
}

void Print(){
  if(printing == 1){

  // Gibt die Werte auf der Konsole aus

  Serial.print("X-Wert: ");
  Serial.print(x_wert);

  Serial.print("\t OutputX: ");
  Serial.print(OutputX);

  Serial.print("\t Y-Wert: ");
  Serial.print(y_wert);

  Serial.print("\t OutputY: ");
  Serial.print(OutputY);
             
  float analog = analogRead(AnalogPin);
  Serial.print("\t Voltage: ");
  Serial.println(analog/204.6);
  }
}

