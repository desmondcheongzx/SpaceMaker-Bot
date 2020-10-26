// define pin values
const int left_forward = 9;
const int right_wheel = 10;
const int right_back = 6;
const int left_back = 5;
const int trig = A0;
const int echo = A1;
const int trig2 = A2;
const int echo2 = A3;

/* set robot mode
 * TODO: use IR receiver to set robot mode
 * 
 * Mode 0: Tacit Space Maker, finds people standing within 6
 *         feet of each other and encourages them to move apart
 *
 * Mode 1: Gantry, blocks a path after someone crosses for a few
 *         seconds before moving back to discourage tailgating
 *
 * Mode 2: Equidistance mode, maintains equidistance in all directions
 *         so that a population of SpaceMaker bots can create social
 *         distance
 */
int robot_mode = 2;

void setup() {
  // set up serial monitor
  Serial.begin(9600);

  // set up motors
  pinMode(left_forward, OUTPUT);
  pinMode(right_wheel, OUTPUT);
  pinMode(left_back, OUTPUT);
  pinMode(right_back, OUTPUT);

  // set up ultrasound sensors
  // echo/trig correspond to the front ultrasound
  // echo2/trig2 correspond to the rear ultrasound
  pinMode(echo, INPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo2, INPUT);
  pinMode(trig2, OUTPUT);
}

void loop() {
  if (robot_mode == 0) {
    make_space();
  }
  else if (robot_mode == 1) {
    gantry();
  }
  else {
    maintain_equidistance();
  }
}

void make_space() {
  // Repeatedly moves back and forth between people
  // standing too close to encourage them to move apart
  int move_dist = wait_for_person() / 7;
  forward(move_dist);
  rotate(2);
  while (true) {
    delay(2000);
    int distance = get_distance(trig, echo);
    if (distance < 60) {
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

void gantry() {
  // Wait for someone to pass by before blocking the path
  // behind them for 3 seconds
  int move_dist = fast_wait() / 7;
  forward(move_dist);
  delay(3000);
  backward(move_dist);
}

int fast_wait() {
  // wait for any object to pass within 50 cm of the bot
  int distance = 0;
  while (true) {
    distance = get_distance(trig, echo);
    if (distance > 50) {
      continue;
    }
    break;
  }
  return distance;
}

int wait_for_person() {
  int distance = 0;
  // Wait for two people to be less than 6 feet apart
  while (true) {
    rotate(1);
    int distance = get_distance(trig, echo);
    if (distance > 50) {
      continue;
    }
    int rear_distance = get_distance(trig2, echo2);
    if (distance + rear_distance > 120) {
      continue;
    }
    break;
  }
  return distance;
}

void maintain_equidistance() {
  while (true) {
    for (int a = 0; a < 5; ++a){
      int direction = is_equidistant();
      Serial.println("Direction to move (+ve = forwards, -ve = backwards):");
      Serial.println(direction);
      if (direction == 0) {
        continue;
      }
      if (direction > 0) {
        forward_continuous();
      } else {
        backward_continuous();
      }
    }
    rotate(1);
  }
}

int is_equidistant() {
  int lowest = 1000;
  int rear_lowest = 1000;
  // fire both front and rear ultrasounds multiple times and
  // get the difference between their readings
  for (int a = 0; a < 5; ++a) {
    int distance = fire_ultrasound(trig, echo);
    int rear_distance = fire_ultrasound(trig2, echo2);
    if (distance < lowest) {
      lowest = distance;
    }
    if (rear_distance < rear_lowest) {
      rear_lowest = rear_distance;
    }
  }
  int diff = rear_lowest - lowest;
  if (diff > 2) {
    return -1; // rear distance greater, move backwards
  }
  if (diff < -2) {
    return 1; // front distance greater, move forward
  }
  return 0;
}

int get_distance(int cur_trig, int cur_echo) {
  int lowest = 1000;
  // fire the ultrasound multiple times and take the minimum reading
  for (int a = 0; a < 15; ++a) {
    int distance = fire_ultrasound(cur_trig, cur_echo);
    if (distance < lowest) {
      lowest = distance;
    }
  }
  return lowest;
}

int fire_ultrasound(int cur_trig, int cur_echo) {
  int distance = 0;
  digitalWrite(cur_trig, LOW);
  delayMicroseconds(2);
  digitalWrite(cur_trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(cur_trig, LOW);
  float Fdistance = pulseIn(echo, HIGH);
  // compute distance in cm
  Fdistance /= Fdistance / 58;

  // convert distance to an integer
  distance = Fdistance;
  
  return distance;  
}

void halt() {
  analogWrite(left_forward, 0);
  analogWrite(right_wheel, 0);
  analogWrite(left_back, 0);
  analogWrite(right_back, 0);
}

void rotate(int quadrants) {
  halt();
  analogWrite(left_back, 255);
  analogWrite(right_wheel, 235);
  delay(quadrants*1000);
  halt();
}

void forward(int distance) {
  halt();
  analogWrite(left_forward, 255);
  analogWrite(right_wheel, 255);
  delay(1000 * distance);
  halt();
}

void backward(int distance) {
  halt();
  analogWrite(left_back, 255);
  analogWrite(right_back, 255);
  delay(1000 * distance);
  halt();
}

void forward_continuous() {
  halt();
  analogWrite(left_forward, 255);
  analogWrite(right_wheel, 255);
}

void backward_continuous() {
  halt();
  analogWrite(left_back, 255);
  analogWrite(right_back, 255);
}