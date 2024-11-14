#include <MPU6050_tockn.h>

/* Pins */ 
#define RIGHT_PWM_PIN 5
#define RIGHT_DIR_PIN 7
#define LEFT_PWM_PIN 6
#define LEFT_DIR_PIN 8
#define STBY_PIN 3
#define BUTTON 2

/* Arbitrary values */

// Motor speeds
#define MIN_SPEED 35 // Min speed for driveStraight()
#define MAX_SPEED 45 // Max speed for driveStraight(); In reality, it's 255
#define LEFT_TURN_SPEED 50 // Speed for turnLeft()
#define RIGHT_TURN_SPEED 50 // Speed for turnRight()

// 90 degrees; Used in turnLeft() and turnRight()
#define GYRO_90 168.5

// driveStraight() times
#define ONE_SQUARE_TIME 4450 // 50 cm
#define FORWARD_START_TIME 3382 // 38 cm
#define FORWARD_END_TIME 3382 // 38 cm
#define TURN_ADJUST_TIME 392 // 4.4 cm
#define TURN_ADJUST_WAIT_TIME 100 // Delay time after the turn adjust and before the turn

// Cap on the duration of the turn + the wait afterwards
volatile int TURN_TIME = 4000; // 2500 <= TURN_TIME <= 4500

// What we hope the current angle to be
volatile float angle;

/* Wait handler thingy */
struct Waiter {
  long targetTime;
  int numOfOneSquare;
  int numOfTurn;

  long subTime;
  double waitTime;
  
  void calculate();
  void print();
  void wait();
  void specialWait();
};

/* Global variables */
MPU6050 mpu(Wire);
volatile bool isRunning = false;
Waiter w;

/* Function headers */
void turnLeft();
void turnRight();

void forwardStart(); 
void forwardEnd();
void forwardOneSquare();
void backwardOneSquare();
void driveStraight(int ms, bool isForward = true);

void forward();
void backward();
void left();
void right();
void brake();

void setSpeed(int speed);
void setSpeeds(int left, int right);
void changeSpeed(int &speed, int increment);

static void changeMode();
void my_init();