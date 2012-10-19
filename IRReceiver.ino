int on[] = {//mColor:0
  255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255
};

int off[] = {//mColor:0
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,
};


int irr1 = 3;
int irr2 = 4;
byte tID = 0x01;

int R = 9;//15
int G = 11;//17
int B = 10;//16

int state = 0;
int phase = 0;

byte data[5];
byte mColor = 0x00;
byte signal = 0x00;
byte pirr[2];

unsigned long time = 0;
unsigned long dt = 0;
unsigned long timeout = 0;
unsigned long t1 = 0;
unsigned long dt1 = 0;

void updateLED(unsigned long t) {
  phase += t/50;
  if (phase > 127) phase -= 128;
  
  switch (mColor) {
    case 0:
      analogWrite(R, on[phase]);
      analogWrite(G, on[phase]);
      analogWrite(B, on[phase]);
      break;

    case 1:
      analogWrite(R, on[phase]);
      analogWrite(G, off[phase]);
      analogWrite(B, off[phase]);
      break;
      
    case 2:
      analogWrite(R, off[phase]);
      analogWrite(G, on[phase]);
      analogWrite(B, off[phase]);
      break;
      
    case 3:
      analogWrite(R, off[phase]);
      analogWrite(G, off[phase]);
      analogWrite(B, on[phase]);
      break;
      
    case 4:
      analogWrite(R, on[phase]);
      analogWrite(G, on[phase]);
      analogWrite(B, off[phase]);
      break;

    case 5:
      analogWrite(R, off[phase]);
      analogWrite(G, on[phase]);
      analogWrite(B, on[phase]);
      break;
      
    case 6:
      analogWrite(R, on[phase]);
      analogWrite(G, off[phase]);
      analogWrite(B, on[phase]);
      break;
      
    default:
      analogWrite(R, off[phase]);
      analogWrite(G, off[phase]);
      analogWrite(B, off[phase]);
      break;
  }
}

void setup() {
  pinMode(irr1, INPUT);
  pinMode(irr2, INPUT);

  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);

  analogWrite(R, 0);
  analogWrite(G, 0);
  analogWrite(B, 0);

  pirr[0] = 0x00;
  pirr[1] = 0x00;
  tID = 0x01;
  time = micros();
  
  Serial.begin(9600);
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
      updateLED(dt1);
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
