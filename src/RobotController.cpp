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

const int (&calculateAngles(const double& x, const double& y, const double& z))[3] {
    static int angles[3];

    double D = sqrt(pow(x,2) + pow(y,2));
    double d = D - L3;
    double z_offset = z - shoulder_offset;
    double R = sqrt(pow(d,2) + pow(z_offset,2));
    double alpha_1 = (acos(d/R))*(180.00/PI);
    double alpha_2 = (acos((pow(L2,2) + pow(R,2) - pow(L2,2))/(2*L1*R)))*(180.00/PI);
    double theta_1;
    double theta_2 = ((acos((pow(L1,2) + pow(L2,2) - pow(R,2))/(2*L1*L2)))*(180.00/PI));
    double theta_3;

    if (z >= shoulder_offset) {
        theta_1 = (alpha_1 + alpha_2); //theta 1
        theta_3 = 180.00 - ((180.00 - (alpha_2 + theta_2)) - alpha_1); //theta 3
    } else {
        theta_1 = (alpha_2 - alpha_1); //theta 1
        theta_3 = 180.00 - ((180.00 - (alpha_2 + theta_2)) + alpha_1); //theta 3
    }

    angles[0] = static_cast<int>(theta_1);
    angles[1] = static_cast<int>(theta_2);
    angles[2] = static_cast<int>(theta_3);

    return angles;
}

void assignCoordinate(const double& x, const double& y, const double& z) {
  int thetas[3];
  
  moveBaseToCoordinate(x, y, z);
  const int (&result)[3] = calculateAngles(x, y, z);
  for (int i = 0; i < 3; ++i) {
    thetas[i] = result[i];
    Serial.print("Theta ");
    Serial.println(i + 1);
    Serial.println(thetas[i]);
  }
  assignAngles(current_angles[0], thetas[0], thetas[1], thetas[2], current_angles[4], current_angles[5]);

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