int Pins[] = {2,3,4,5,6,7};
byte counter = 0; // Binärzahl des 6-Bit-Zählers
int throttle = 0; // Gashebel in Prozent
int AnalogPin = 0;
char up_down;

void setup()
{
  for (int i =0;i<6;i++)
  {
    pinMode(Pins[i], OUTPUT);
  }
Serial.begin(9600);
}

void loop() 
{
  if (Serial.available() > 0) {
                // read the incoming byte:
                up_down = Serial.read();
               // throttle = Serial.parseInt();
                
                if (up_down == 'w'){throttle = throttle+2;}
                if (up_down == 's'){throttle = throttle-2;}
                
                if (throttle > 100){throttle = 100;}
                if (throttle < 0) {throttle = 0;}
                
                
                counter = throttle*0.41;
                
                
                Serial.print(up_down);
                Serial.print("\t Throttle: ");
                Serial.print(throttle);
                Serial.print("\t Binary: ");
                Serial.print(counter);
                
                           
                displayBinary(counter);
                
                float analog = analogRead(AnalogPin);
                Serial.print("\t Voltage: ");
                Serial.println(analog/204.6);
                }

// delay(10);
  
}

void displayBinary(byte numToShow)
{
  for (int i =0;i<6;i++)
  {
    if (bitRead(numToShow, i)==1)
    {
      digitalWrite(Pins[i], HIGH); 
    }
    else
    {
      digitalWrite(Pins[i], LOW); 
    }
  }

}
