#include "EverythingElse.h"

// #define NO_WAIT

void Waiter::calculate() {
  subTime = FORWARD_START_TIME + (numOfOneSquare * ONE_SQUARE_TIME) + (numOfTurn * (TURN_TIME + TURN_ADJUST_TIME + TURN_ADJUST_WAIT_TIME)) + FORWARD_END_TIME;
  waitTime = (double) (targetTime - subTime) / numOfOneSquare;
}
void Waiter::print() {
  Serial.print("\n\nTarget time:\t");
  Serial.println(targetTime);

  Serial.print("Sub time:\t");
  Serial.println(subTime);

  Serial.print("Wait time:\t");
  Serial.println(waitTime);

  Serial.print("New time:\t");
  Serial.println(subTime + waitTime * numOfOneSquare);

  if (subTime > targetTime) {
    Serial.println("\nsubTime > targetTime.\nDecrease TURN_TIME or shorten sequence()!");
  } else if (waitTime >= 2500) {
    Serial.println("\nwaitTime >= 2500.\nIncrease TURN_TIME or prolong sequence()! Just for sure make waitTime < 3000!");
  }
}
void Waiter::wait() {
  delay((int) waitTime);
}
void Waiter::specialWait() {
  delay((int) ceil(waitTime));
}

void turnLeft() {
  #ifndef NO_WAIT
  long ms = millis();
  #endif

  driveStraight(TURN_ADJUST_TIME);
  delay(TURN_ADJUST_WAIT_TIME);

  mpu.update();
  int startAngle = angle;

  left();
  setSpeed(LEFT_TURN_SPEED);

  while (round(mpu.getAngleZ()) < startAngle + GYRO_90) {
    mpu.update();
  }

  brake();

  angle += GYRO_90;

  #ifndef NO_WAIT
  ms = TURN_TIME - (millis() - ms);
  delay(ms);
  #else
  delay(300);
  #endif
}
void turnRight() {
  #ifndef NO_WAIT
  long ms = millis();
  #endif

  driveStraight(TURN_ADJUST_TIME);
  delay(TURN_ADJUST_WAIT_TIME);

  mpu.update();
  int startAngle = angle;

  right();
  setSpeed(RIGHT_TURN_SPEED);

  while (round(mpu.getAngleZ()) > startAngle - GYRO_90) {
    mpu.update();
  }

  brake();

  angle -= GYRO_90;

  #ifndef NO_WAIT
  ms = TURN_TIME - (millis() - ms);
  delay(ms);
  #else
  delay(300);
  #endif
}

void driveStraight(int ms, bool isForward = true) {
  long startMs = millis();

  int targetAngle = angle;
  int currentAngle = targetAngle;
  int deltaAngle = 0; // currentAngle - targetAngle

  int direction = 1;
  if (isForward) {
    forward();
  } else {
    backward();
    direction = -1;
  }

  int leftSpeed = MIN_SPEED + 3; // plus 3 for wiggle room
  int rightSpeed = leftSpeed;
  setSpeeds(leftSpeed, rightSpeed);

  while (millis() - startMs < ms) {
    mpu.update();
    currentAngle = round(mpu.getAngleZ());
    deltaAngle = currentAngle - targetAngle;

    changeSpeed(leftSpeed, +deltaAngle * direction);
    changeSpeed(rightSpeed, -deltaAngle * direction);
    setSpeeds(leftSpeed, rightSpeed);
  }
  brake();
}
void forwardOneSquare() {
  driveStraight(ONE_SQUARE_TIME, true);
  #ifndef NO_WAIT
  w.wait();
  #else
  delay(300);
  #endif
}
void backwardOneSquare() {
  driveStraight(ONE_SQUARE_TIME, false);
  #ifndef NO_WAIT
  w.wait();
  #else
  delay(300);
  #endif
}
void forwardStart() {
  driveStraight(FORWARD_START_TIME, true);
  #ifndef NO_WAIT
  w.specialWait();
  #else
  delay(300);
  #endif
}
void forwardEnd() {
  driveStraight(FORWARD_END_TIME, true);
}

void forward() {
  digitalWrite(RIGHT_DIR_PIN, HIGH);
  digitalWrite(LEFT_DIR_PIN, HIGH);
  digitalWrite(STBY_PIN, HIGH);
}
void backward() {
  digitalWrite(RIGHT_DIR_PIN, LOW);
  digitalWrite(LEFT_DIR_PIN, LOW);
  digitalWrite(STBY_PIN, HIGH);
}
void left() {
  digitalWrite(RIGHT_DIR_PIN, HIGH);
  digitalWrite(LEFT_DIR_PIN, LOW);
  digitalWrite(STBY_PIN, HIGH);
}
void right() {
  digitalWrite(RIGHT_DIR_PIN, LOW);
  digitalWrite(LEFT_DIR_PIN, HIGH);
  digitalWrite(STBY_PIN, HIGH);
}
void brake() {
  setSpeed(0);
  digitalWrite(STBY_PIN, LOW);
  digitalWrite(RIGHT_DIR_PIN, LOW);
  digitalWrite(LEFT_DIR_PIN, LOW);
}
void setSpeed(int speed) {
  analogWrite(LEFT_PWM_PIN, speed);
  analogWrite(RIGHT_PWM_PIN, speed);
}
void setSpeeds(int left, int right) {
  analogWrite(LEFT_PWM_PIN, left);
  analogWrite(RIGHT_PWM_PIN, right);
}
void changeSpeed(int &speed, int increment) {
  int newSpeed = speed + increment;
  if (newSpeed < MIN_SPEED) speed = MIN_SPEED;
  else if (newSpeed > MAX_SPEED) speed = MAX_SPEED; // may change max_speed
  else speed = newSpeed;
}

static void changeMode() {
  isRunning = !isRunning;
}
void my_init() {
  pinMode(RIGHT_DIR_PIN, OUTPUT);
  pinMode(RIGHT_PWM_PIN, OUTPUT);
  pinMode(LEFT_DIR_PIN, OUTPUT);
  pinMode(LEFT_PWM_PIN, OUTPUT);
  pinMode(STBY_PIN, OUTPUT);

  pinMode(BUTTON, INPUT_PULLUP);
  attachInterrupt(0, changeMode, FALLING);

  brake();
  Wire.begin();
  mpu.begin();
}