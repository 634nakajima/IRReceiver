int irr1 = 3;
int irr2 = 4;
byte tID = 0x01;

int R = 9;//15
int G = 11;//17
int B = 10;//16

int state, phase;
byte data[5], mColor, signal, pirr[2];
unsigned long time, dt, timeout, t1, dt1;

void setup() {

  Serial.begin(9600);

  pinMode(irr1, INPUT_PULLUP);
  pinMode(irr2, INPUT_PULLUP);

  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);

  analogWrite(R, 0);
  analogWrite(G, 0);
  analogWrite(B, 0);

  dt = 0;
  timeout = 0;
  phase = 0;
  signal = 0x00;
  state = 0x00;
  mColor = 0x00;
  pirr[0] = 0x00;
  pirr[1] = 0x00;
  
  delay(100);
  time = micros();
}

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
      updateLED(100);
      break;
      
    case 1:
      t1 = millis();
      
      if(detectLeader(irr1)) {
        data[0] = 0x00;//mode:connect
        data[1] = analyzeSignal(irr1);
        data[2] = tID;
        data[3] = 0x00;//type:Data
        data[4] = mColor;
        Serial.write(data, 5);
        pirr[0] = data[1];
      }else {
        data[0] = 0x01;//mode:disconnect
        data[1] = pirr[0];
        data[2] = tID;
        data[3] = 0x00;//type:Data
        data[4] = mColor;
        Serial.write(data, 5);
      }
  
      if(detectLeader(irr2)) {
        data[0] = 0x00;//mode:connect
        data[1] = analyzeSignal(irr2);
        data[2] = tID;
        data[3] = 0x01;//type:Stream
        data[4] = mColor;
        Serial.write(data, 5);
        pirr[1] = data[1];
      }else {
        data[0] = 0x01;//mode:disconnect
        data[1] = pirr[1];
        data[2] = tID;
        data[3] = 0x01;//type:Stream
        data[4] = mColor;
        Serial.write(data, 5);
      }
      
      dt1 = millis() - t1;
      updateLED(dt);
      break;
      
    default:
      break;
  }
  
  if(Serial.available() > 0) {
      mColor = Serial.read();
      state = 1;
  }
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
  
  if (timeout > 60000) {
    timeout = 0;
    return false;
  }else {
    detectLeader(irr);
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
