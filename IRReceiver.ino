/*
  DigitalReadSerial
 Reads a digital input on pin 2, prints the result to the serial monitor 
 
 This example code is in the public domain.
 */

// digital pin 2 has a pushbutton attached to it. Give it a name:
int irr1 = 3;
int LED = 13;
int LED2 = 12;

unsigned long time, dt, timeout;
unsigned int signal;
// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  // make the pushbutton's pin an input:
  pinMode(irr1, INPUT);
  digitalWrite(LED, LOW);
  digitalWrite(LED2, LOW);
  time = micros();
  dt = 0;
  timeout = 0;
  signal = 0;
}

// the loop routine runs over and over again forever:
void loop() {
  if(detectLeader(irr1)) {
    signal = 0;
    Serial.print(analyzeSignal(irr1), BIN);
    digitalWrite(LED, HIGH);
  }else {
    digitalWrite(LED, LOW);
  }
  delay(500);
  // print out the state of the button:
  
  //delayMicroseconds(10);        // delay in between reads for stability
}

boolean detectLeader(const int irr) {
  time = micros();
  while(!digitalRead(irr));
  dt = micros() - time;
  timeout += dt;

  if (dt > 8000 && dt < 9000) {
    time = micros();
    while(digitalRead(irr));
    dt = micros() - time;
    timeout += dt;
    
    if (dt > 4000 && dt < 5000) {
      timeout = 0;
      return true;
    }
  }
  
  if(timeout < 100000) {
    detectLeader(irr);
  }else {
    timeout = 0;
    return false;
  }
}

unsigned int analyzeSignal(const int irr) {
  while(!digitalRead(irr));
  time = micros();
  while(digitalRead(irr));
  dt = micros() - time;
  
  if(dt > 500 && dt < 700) {
    return 0x0000 | analyzeSignal(irr) << 1;
  }else if(dt > 1600 && dt < 1800) {
    return 0x0001 | analyzeSignal(irr) << 1;
  }else if(dt > 10000) {
    return signal;
  }
}
