/*
  DigitalReadSerial
 Reads a digital input on pin 2, prints the result to the serial monitor 
 
 This example code is in the public domain.
 */

// digital pin 2 has a pushbutton attached to it. Give it a name:
int irr1 = 3;
int irr2 = 4;
byte tID = 0x01;

int LED = 13;
int LED2 = 12;

int state;
byte data[5], mColor, signal, pirr[2];
unsigned long time, dt, timeout;
//unsigned int signal;
// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  // make the pushbutton's pin an input:
  pinMode(irr1, INPUT);
  pinMode(irr2, INPUT);

  digitalWrite(LED, LOW);
  digitalWrite(LED2, LOW);
  time = micros();
  dt = 0;
  timeout = 0;
  signal = 0x00;
  state = 0x00;
  mColor = 0x00;
  pirr[0] = 0x00;
  pirr[1] = 0x01;
}

// the loop routine runs over and over again forever:
void loop() {
  switch (state) {
    case 0:
      data[0] = 0x02;
      data[1] = 0x00;
      data[2] = tID;
      data[3] = 0x00;
      data[4] = mColor;
    
      Serial.write(data, 5);
      delay(100);
      break;
      
    case 1:
      if(detectLeader(irr1)) {
        data[0] = 0x00;//mode:connect
        data[1] = analyzeSignal(irr1);
        data[2] = tID;
        data[3] = 0x00;//type:Data
        data[4] = mColor;
        Serial.write(data, 5);
        digitalWrite(LED, HIGH);
        pirr[0] = data[1];
      }else {
        data[0] = 0x01;//mode:disconnect
        data[1] = pirr[0];
        data[2] = tID;
        data[3] = 0x00;//type:Data
        data[4] = mColor;
        Serial.write(data, 5);
        digitalWrite(LED, LOW);
      }
      delay(100);
  
      if(detectLeader(irr2)) {
        data[0] = 0x00;//mode:connect
        data[1] = analyzeSignal(irr2);
        data[2] = tID;
        data[3] = 0x01;//type:Stream
        data[4] = mColor;
        Serial.write(data, 5);
        digitalWrite(LED2, HIGH);
        pirr[1] = data[1];
      }else {
        data[0] = 0x01;//mode:disconnect
        data[1] = pirr[0];
        data[2] = tID;
        data[3] = 0x01;//type:Stream
        data[4] = mColor;
        Serial.write(data, 5);
        digitalWrite(LED2, LOW);
      }
      delay(100);
  }
  
  if(Serial.available() > 0) {
    mColor = Serial.read();
    state = 1;
  }
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

byte analyzeSignal(const int irr) {
  while(!digitalRead(irr));
  time = micros();
  while(digitalRead(irr));
  dt = micros() - time;
  
  if(dt > 500 && dt < 700) {
    return 0x00 | analyzeSignal(irr) << 1;
  }else if(dt > 1600 && dt < 1800) {
    return 0x01 | analyzeSignal(irr) << 1;
  }else if(dt > 10000) {
    return signal;
  }
}
