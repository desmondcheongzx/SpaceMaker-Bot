const int left_wheel = 9;
const int right_wheel = 10;
const int right_back = 6;
const int left_back = 5;

const int trig = A0;
const int echo = A1;
const int trig2 = 11;
const int echo2 = 12;

int robot_mode = 1;

void setup() {
  Serial.begin(9600);
  Serial.println("hi");
  pinMode(left_wheel, OUTPUT);
  pinMode(right_wheel, OUTPUT);
  pinMode(left_back, OUTPUT);
  pinMode(right_back, OUTPUT);

  pinMode(echo, INPUT);    // Set Ultrasonic echo port as input
  pinMode(trig, OUTPUT);
  pinMode(echo2, INPUT);    // Set Ultrasonic echo port as input
  pinMode(trig2, OUTPUT);   // Set Ultrasonic trig port as input
}
/*
void loop() {
  if (robot_mode == 0) {
    int move_dist = wait_for_person() / 7;
    forward(move_dist);
    Serial.println("rotating 180 degrees");
    rotate(2);
    while (true) {
      Serial.println("scanning");
      delay(2000);
      int distance = get_distance();
      if (distance < 60) {
        Serial.println("not good enough");
        move_dist = distance / 8;
        forward(move_dist);
        delay(1000);
        rotate(2);
        continue;
      }
      else {
        break;
      }
    }
  }
  else {
    gantry();
  }
}*/

void loop() {
  maintain_equidistance();
}

void gantry() {  
  int move_dist = fast_wait() / 7;
  forward(move_dist);
  delay(3000);
  backward(move_dist);
}


int fast_wait() {
  int distance = 0;
  while (true) {
    distance = get_distance();
    if (distance > 50) {
      continue;
    }
    break;
  }
  Serial.print("moving them apart now...");
  return distance;
}

int wait_for_person() {
  int distance = 0;
  int distance2 = 0;
  while (true) {
    rotate(1);
    distance = get_distance();
    if (distance > 50) {
      continue;
    }
    Serial.println("spotted person");
    rotate(2);
    delay(1000);
    distance2 = get_distance();
    if (distance + distance2 > 120) {
      continue;
    }
    break;
  }
  Serial.print("moving them apart now...");
  return distance2;
}

void maintain_equidistance() {
  while (true) {
    for (int a = 0; a < 5; ++a){
      int direction = is_equidistant();
      Serial.println(direction);
      if (direction == 0) {
        continue;
      }
      if (direction > 0) {
        forward(1);
      } else {
        backward(1);
      }
    }
    halt();
    rotate(1);
  }
}

int is_equidistant() {
  int lowest = 1000;
  int lowest2 = 1000;
  for (int a = 0; a < 5; ++a) {
    int distance = 0;
    int distance2 = 0;
    digitalWrite(trig, LOW);    // set trig port low level for 2μs
    delayMicroseconds(2);
    digitalWrite(trig, HIGH);  // set trig port high level for 10μs(at least 10μs)
    delayMicroseconds(10);
    digitalWrite(trig, LOW);    // set trig port low level
    float Fdistance = pulseIn(echo, HIGH);  // Read echo port high level time(unit:μs)
    
    digitalWrite(trig2, LOW);
    delayMicroseconds(2);
    digitalWrite(trig2, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig2, LOW);
    
    float Fdistance2 = pulseIn(echo2, HIGH);  // Read echo port high level time(unit:μs)
    Fdistance /= 58;       // Distance(m) =(time(s) * 344(m/s)) / 2     /****** The speed of sound is 344m/s.*******/
                                   //  ==> 2*Distance(cm) = time(μs) * 0.0344(cm/μs)
                                   // ==> Distance(cm) = time(μs) * 0.0172 = time(μs) / 58
    Fdistance2 /= 58;
    distance = Fdistance;
    distance2 = Fdistance2;
    if (distance < lowest) {
      lowest = distance;
    }
    if (distance2 < lowest2) {
      lowest2 = distance2;
    }
    
    Serial.print("Distance1:");      //Output Distance(cm)
    Serial.println(Fdistance);         //display distance
    Serial.print("Distance2:");      //Output Distance(cm)
    Serial.println(Fdistance2);         //display distance
  }
  int diff = lowest2 - lowest;
  if (diff > 2) {
    return -1; // back distance greater, move backwards
  }
  if (diff < -2) {
    return 1; // front distance greater, move forward
  }
  return 0;
}

int get_distance() {
  int lowest = 1000;
  for (int a = 0; a < 15; ++a) {
    int distance = 0;
    digitalWrite(trig, LOW);    // set trig port low level for 2μs
    delayMicroseconds(2);
    digitalWrite(trig, HIGH);  // set trig port high level for 10μs(at least 10μs)
    delayMicroseconds(10);
    digitalWrite(trig, LOW);    // set trig port low level
    float Fdistance = pulseIn(echo, HIGH);  // Read echo port high level time(unit:μs)
    Fdistance= Fdistance/58;       // Distance(m) =(time(s) * 344(m/s)) / 2     /****** The speed of sound is 344m/s.*******/
                                   //  ==> 2*Distance(cm) = time(μs) * 0.0344(cm/μs)
                                   // ==> Distance(cm) = time(μs) * 0.0172 = time(μs) / 58
    Serial.print("Distance:");      //Output Distance(cm)
    Serial.println(Fdistance);         //display distance
    distance = Fdistance;
    if (distance < lowest) {
      lowest = distance;
    }
  }
  return lowest;
}

void halt() {
  analogWrite(left_wheel, 0);
  analogWrite(right_wheel, 0);
  analogWrite(left_back, 0);
  analogWrite(right_back, 0);
}

void rotate(int quadrants) {
  analogWrite(left_back, 255);
  analogWrite(right_wheel, 235);
  delay(quadrants*1000);
  halt();
}

void forward(int distance) {
  analogWrite(left_back, 0);
  analogWrite(right_back, 0);
  analogWrite(left_wheel, 255);
  analogWrite(right_wheel, 255);
  //delay(1000 * distance);
  //halt();
}

void backward(int distance) {
  analogWrite(left_wheel, 0);
  analogWrite(right_wheel, 0);
  analogWrite(left_back, 255);
  analogWrite(right_back, 255);
  //delay(1000 * distance);
  //halt();
}
