#include <SPI.h>


const int ChipSelect      = 8;         //Chip-Select am Pin 8 des Arduino
const int AnalogPin       = A0;        //Liest den aktuellen Volt-Wert

const int wiper0writeAddr = B00000000; //Schreibadresse für Poti 0 des Digi-Pot
const int wiper1writeAddr = B00010000; //Schreibadresse für Poti 1 des Digi-Pot

const int button1         = 2;     //Pin 2: Taste 1
const int button2         = 3;     //Pin 3: Taste 2
const int led             = 9;     //Pin 9: LED

int y_wert                = 0;
int digipot_wert          = 127; // Wert zwischen 0 und 255 für das Digital-Potentiometer
int setpoint              = 240;
int gain                  = 1;
int abstand               = 0;

long time                 = 0;     //Zeitmessung startet bei 0s.
long countdown            = 50;    //25ms Verzögerung

String inString = "";    // string to hold input

const int printing        = 0;
  


void setup() {
  
  // Setzt Chip-Select als AUSGANG und deaktviert ihn:
  pinMode(ChipSelect, OUTPUT);
  pinMode(ChipSelect, HIGH);

  // Initialisiert SPI und Serial:
  SPI.begin();
  Serial.begin(38400);
  Serial.println("Starte Programm\n");

  //Aktiviert den internen Pull-Up-Widerstand für die Tasten
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);

  //LED als AUSGANG
  pinMode(led, OUTPUT);

  //Initialisiert das Digital-Poti auf den Wert 0
  PotiSteuerung(127);

  //Interrupt für die Tasten
  attachInterrupt(1, PlusTaste,   LOW); //Taste 1
  attachInterrupt(0, MinusTaste,   LOW); //Taste 2

}

void loop() {
  
  while (Serial.available() > 0) {
  SerialVerbindung();
  }
  
  if(millis() - time > countdown){
    if(y_wert > setpoint){digipot_wert += gain;}
    if(y_wert < setpoint){digipot_wert -= gain;}
  }
  time = millis();

  AbstandBerechnen();
  if(abstand > 60){gain = 3;}
  else{gain = 1;}
  
  digipot_wert = constrain(digipot_wert, 0, 255);
  
  if(setpoint == 240){digitalWrite(led, HIGH);}
  else{digitalWrite(led, LOW);}
  
  PotiSteuerung(digipot_wert);

  Print();
}


void PotiSteuerung(int digipot_wert){

  digitalWrite(ChipSelect,LOW);
  SPI.transfer(wiper0writeAddr);
  SPI.transfer(digipot_wert);
  digitalWrite(ChipSelect,HIGH);
  
}

void SerialVerbindung(){
    int inChar = Serial.read();
    if (isDigit(inChar)) {
      inString += (char)inChar;
    }
    
    if (inChar == 10) {

      y_wert = inString.toInt();
    
      Print();
      inString = "";
    }
}

void PlusTaste() { 
  // Taste wird entprellt und zählt den aktuellen Menüschritt hoch
    setpoint += 10;
    Print();
}

void MinusTaste() { 
  // Taste wird entprellt und zählt den aktuellen Menüschritt hoch
    setpoint -= 10;
    Print();
}

void Print(){
  if(printing == 1){

  Serial.print("Y-Wert: ");
  Serial.print(y_wert);

  Serial.print("\t Setpoint: ");
  Serial.print(setpoint);

  Serial.print("\t Digipot:");
  Serial.print(digipot_wert);
             
  float analog = analogRead(AnalogPin);
  Serial.print("\t Voltage: ");
  Serial.println(analog/204.6);
  }
}

void AbstandBerechnen(){
  int abstand_roh = setpoint - y_wert;
  abstand = abs(abstand_roh);
}

