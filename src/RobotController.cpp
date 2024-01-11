Servo M1;
Servo M2;
Servo M3;
Servo M4;
Servo M5;
Servo M6;

int power_pin = 12;

void RobotController::initializeServos() {
  pinMode(power_pin, OUTPUT);
  digitalWrite(power_pin, LOW);
  // Writing Servo variables to their respective pins
  M1.attach(11);
  M2.attach(10);
  M3.attach(9);
  M4.attach(6);
  M5.attach(5);
  M6.attach(3);

  // Initial positions for each servo
  M1.write(0);
  M2.write(90);
  M3.write(90);
  M4.write(90);
  M5.write(90);
  M6.write(90);

  delay(1000);  // Allow time for servos to reach initial positions

  digitalWrite(power_pin, HIGH);
}

const int angle_boundaries[6][2] = {{0, 180}, {20, 160}, {0, 180}, {0, 180}, {0, 180}, {10, 90}};

Servo servos[6] = {M1, M2, M3, M4, M5, M6};

int current_angles[6] = {0, 90, 90, 90, 90, 90};

void RobotController::change_by_one(int servo_index, char op) {
  if (op == '+') {
    current_angles[servo_index]++;
  } else if (op == '-') {
    current_angles[servo_index]--;
  }
  if (angleBoundariesCheck(current_angles)) {
    servos[servo_index].write(current_angles[servo_index]);
    delay(30);
  }
}

bool RobotController::move_to_angle(int servo_index, int angle) {
  char op;
  if(current_angles[servo_index] == angle) {
    return true;
  }
  
  if (current_angles[servo_index] < angle) {
    op = '+';
  } else {
    op = '-';
  }

  change_by_one(servo_index, op);

  return move_to_angle(servo_index, angle);
}

bool RobotController::angleBoundariesCheck(const int angles[6]) {
  for (int i = 0; i < 6; i++) {
    if ((angles[i] < angle_boundaries[i][0]) || (angles[i] > angle_boundaries[i][1])) {
      return false;
    }
  }

  return true;
}

void RobotController::assignAngles(const int& M1_angle, const int& M2_angle, const int& M3_angle, const int& M4_angle, const int& M5_angle, const int& M6_angle) {
  bool run = true;
  const int new_angles[6] = {M1_angle, M2_angle, M3_angle, M4_angle, M5_angle, M6_angle};
  if (angleBoundariesCheck(new_angles)) {
    for (int i = 5; i >= 0; i--) {
      if(!run) {
        break;
      }
      run = move_to_angle(i, new_angles[i]);
      delay(100);
    }
  }
}

void RobotController::closeGrip() {
  move_to_angle(5, 90);
}

void RobotController::openGrip() {
  move_to_angle(5, 20);
}