#include <SoftwareSerial.h>
// define pin values
const int left_wheel = 2;
const int pwm_l = 3;
const int left_back = 4;
const int right_back = 5;
const int pwm_r = 6;
const int right_wheel = 7;
const int trig = A0;
const int echo = A1;
const int trig2 = A2;
const int echo2 = A3;

SoftwareSerial EEBlue(10, 11); // RX | TX

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
int robot_mode = 3;
int quads = 4;
int l_power = 75;
int r_power = 105;  

void setup() {
  // set up serial monitor
  Serial.begin(115200);
  EEBlue.begin(115200); //Default Baud for comm, it may be different for your Module. Serial.println("The bluetooth gates are open.\n Connect to HC-05 from any other bluetooth device with 1234 as pairing key!.");

  // set up motors
  pinMode(left_wheel, OUTPUT);
  pinMode(right_wheel, OUTPUT);
  pinMode(left_back, OUTPUT);
  pinMode(right_back, OUTPUT);
  pinMode(pwm_l, OUTPUT);
  pinMode(pwm_r, OUTPUT);

  // set up ultrasound sensors
  // echo/trig correspond to the front ultrasound
  // echo2/trig2 correspond to the rear ultrasound
  pinMode(echo, INPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo2, INPUT);
  pinMode(trig2, OUTPUT);
  analogWrite(pwm_l, l_power);
  analogWrite(pwm_r, r_power);
}

void loop() {
  switch(robot_mode) {
    case 0:
      make_space();
      break;
    case 1:
      gantry();
      break;
    case 2:
      maintain_equidistance();
      break;
    case 3:
      check_bt();
      break;
  }
}

void check_bt() {
  if (EEBlue.available()) {
    char bt_val = EEBlue.read();
    switch(bt_val) {
      case 'w':
        forward(1);
        break;
      case 'a':
        rotate_l(quads);
        break;
      case 's':
        backward(1);
        break;
      case 'd':
        rotate_r(quads);
        break;
      case '5':
        quads++;
        break;
      case '6':
        quads--;
        break;
      case '2':
        l_power += 25;
        analogWrite(pwm_l, l_power);
        break;
      case '1':
        l_power -= 5;
        analogWrite(pwm_l, l_power);
        break;
      case '9':
        r_power += 25;
        analogWrite(pwm_r, r_power);
        break;
      case '8':
        r_power -= 5;
        analogWrite(pwm_r, r_power);
        break;
    }
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
  int sum = 0;
  int n_trials = 15;
  // fire the ultrasound multiple times and take the minimum reading
  for (int a = 0; a < n_trials; ++a) {
    int distance = fire_ultrasound(cur_trig, cur_echo);
    sum += distance;
  }
  return distance / n_trials;
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
  digitalWrite(left_wheel, LOW);
  digitalWrite(right_wheel, LOW);
  digitalWrite(left_back, LOW);
  digitalWrite(right_back, LOW);
}

void rotate(int quadrants) {
  halt();
  digitalWrite(left_back, HIGH);
  digitalWrite(right_wheel, HIGH);
  delay(quadrants*100);
  halt();
}

void rotate_l(int quadrants) {
  halt();
  digitalWrite(left_back, HIGH);
  digitalWrite(right_wheel, HIGH);
  delay(quadrants*100);
  halt();
}

void rotate_r(int quadrants) {
  halt();
  digitalWrite(left_wheel, HIGH);
  digitalWrite(right_back, HIGH);
  delay(quadrants*100);
  halt();
}

void forward(int distance) {
  halt();
  digitalWrite(left_wheel, HIGH);
  digitalWrite(right_wheel, HIGH);
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
  analogWrite(left_wheel, 255);
  analogWrite(right_wheel, 255);
}

void backward_continuous() {
  halt();
  analogWrite(left_back, 255);
  analogWrite(right_back, 255);
}