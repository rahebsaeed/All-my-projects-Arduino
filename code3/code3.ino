#include <Servo.h>
#include <math.h>

// Vars for Inversekinematics
float A; // Angle opposite side a (between band c)
float B; // Angle opposite side b
float C; // Angle opposite side c
float theta; // Angle formed between line from origin to (x, y) and the horizontal
float x;   // x position of foot in cm
float y;   // y position of foot in cm
float c;   // Hypotenuse length in cm
const float pi = M_PI; // Store pi in a less annoying format
const float correction_factor_r = 9.839228721; // Correction factor to shift the foot position to the
                                                  // correct position, caused by foot angle

// Define variables for FSRs
const int fsrPins[4] = {0, 1, 2, 3};
float py[4];               // for foot height calibration
float fsrReading[4];       // to store FSR readings
int fsrVoltage;            // the analog reading converted to voltage
unsigned long fsrResistance; // The voltage converted to resistance, can be very big so make "long"
unsigned long fsrConductance;

// Define servo objects, F=Front R=Rear L=Left R=Right
Servo servo_FR1, servo_FR2, servo_FL1, servo_FL2, servo_RL1, servo_RL2, servo_RR1, servo_RR2;

// Servo Angles
float vinkel[2];

// Define robot Constants
const float a = 6;         // upper leg length (cm)
const float b = 6;         // low leg length (cm)
const float ben_fram = 8.5; // Distance from body center to leg x axis (cm)
const float ben_sid = 9.85; // Distance from body center to leg z axis (cm) KOLLA GENOM VIKTIIIIIIIIIIIIIIIIIIIIGT
const float init_ben[4][2] = { // define initial leg positions RL,FL,RR,FR
    {-ben_fram, -ben_sid},  //
    {ben_fram, -ben_sid},   //
    {-ben_fram, ben_sid},   //
    {ben_fram, ben_sid},    //
};
// define array of leg components
Servo ben_lista[4][2] = {
    {servo_RL1, servo_RL2}, //
    {servo_FL1, servo_FL2}, //
    {servo_RR1, servo_RR2}, //
    {servo_FR1, servo_FR2}, //
};
// Gait pattern generation variables (res of movement/ no of points interpolated-1)
const float steg_langd = 0.1; // i.e. length of each timestep
const int delta_x = 4;       // footstep length
const int delay_steg = 2;    // length of initial delay of movement
const int steg = delta_x / steg_langd;       // no of points
const int steg_delay = delay_steg / steg_langd; // no of points
float parab[steg_delay + steg][2];
float cope1 = 0;
float cope2 = 0;

// Global variables to keep track of
float koords[4][2]; // location of feet
float com[2];      // center of mass location

// Define variables for Controller
const float KP = 1, KI = 0, KD = 0;
float err = 0;
float previousError = 0;
float integral = 0;
float derivative = 0;
float PID = 0;
float deltaT = 0;
float currentTime = 0;
float previousTime = 0;
float r = 0;

// constants to keep inside support polygon
const float marg = 1.5;
const float leg_marg = 0.4;

// ==============================================

void inverse_func() {
  if (sqrt(sq(x) + sq(y)) <= 12) { // keep within bounds

    c = sqrt(sq(x) + sq(y));         // pythagorean theorem
    B = (acos((sq(b) - sq(a) - sq(c)) / (-2 * a * c))) * (180 / pi); // Law of cosines: Angle opposite upper arm
                                                                       // section
    C = (acos((sq(c) - sq(b) - sq(a)) / (-2 * a * b))) * (180 / pi); // Law of cosines: Angle opposite
                                                                       // hypotenuse
    theta = (acos(x / c)) * (180 / pi);                                 // Solve for theta to
                                                                       // correct for lower joint’s impact on upper joint’s
                                                                       // angle
    vinkel[0] = (theta - B);                                             // necessary angle
    vinkel[1] = C;                                                       // neceesary angle
  }
  // else {
  // Serial.println("Out of Bounds");
  // }
}
void moveR_func(float dx, float dy, Servo obj1, Servo obj2) { // Inv kinematics for right leg
  x = -dx;
  y = sqrt(2) * 6 + dy;
  inverse_func();
  obj1.write(vinkel[0]);
  obj2.write(180 - vinkel[1] + correction_factor_r);
}

void moveL_func(float dx, float dy, Servo obj1, Servo obj2) { // Inv kinematics for left leg;
  x = -dx;
  y = sqrt(2) * 6 + dy;
  inverse_func();
  obj1.write(180 - vinkel[0]);
  obj2.write(vinkel[1] - correction_factor_r);
}
float** Parab_func(float delta_x) { // Generate parabola for leg movement

  // coefficients for parabola
  const float a = 4 / (delta_x);
  const float b = -a * delta_x;
  for (int i = steg_delay - 1; i < steg_delay + steg; i++) {
    // fix x
    float xled = (i - steg_delay) * (delta_x / (steg - 1));
    if (i != 0) {
      cope1 = (i - steg_delay) * (delta_x / (steg - 1));
    }
    parab[i][0] = (i - steg_delay + 1) * (delta_x / (steg - 1)) - cope1; // x vector
    if (i != 0) {
      cope2 = a * sq(xled) + b * xled;
    }
    parab[i][1] = (a * sq(parab[i][0] + cope1) + b * (parab[i][0] + cope1)) - cope2; // y vector
  }
}
float app_com() { // Approximate location of com

  com[0] = 0;
  com[1] = 0;

  float fsr_sum = 0;

  for (int i = 0; i < 4; i++) {
    fsr_sum += fsrReading[i];
  }

  for (int i = 0; i < 4; i++) {
    com[0] += (init_ben[i][0] + koords[i][0]) * fsrReading[i] / fsr_sum;
    com[1] += (init_ben[i][1]) * fsrReading[i] / fsr_sum;
  }
}
float support_func(int i) { // Generate parabola for leg movement//i==footofground
  float k = 0;
  float m = 0;
  app_com(); // approximatecomposition
  for (int j = 1; j < 5; j++) {
    if (i != j) {
      if (i % 2 == j % 2) {
        // coefficients for parabola
        k = (init_ben[4 - j][0] + koords[4 - j][0] - (init_ben[j - 1][0] + koords[j - 1][0])) /
            (init_ben[4 - j][1] - init_ben[j - 1][1]);
        m = init_ben[4 - j][0] + koords[4 - j][0] - k * init_ben[4 - j][1];
      }
    }
  }
  float x_pos = k * 0 + m;
  float marg2 = marg + leg_marg;
  if (i % 2 == 0) {
    marg2 = -marg - leg_marg;
  }
  r = x_pos + marg2;
}

float read_fsrs() { // read fsrs
  for (int i = 0; i < 4; i++) {
    fsrReading[i] = analogRead(fsrPins[i]);

    // analog voltage reading ranges from about 0 to 1023 which maps to 0V to 5V (=5000mV)
    fsrVoltage = map(fsrReading[i], 0, 1023, 0, 5000);

    if (fsrVoltage != 0) {
      // The voltage=Vcc * R/(R+FSR) where R=10K and Vcc=5V
      // so FSR=((Vcc-V) * R)/V yaymath!
      fsrResistance = 5000 - fsrVoltage; // fsrVoltage is in millivolts so 5V=5000mV
      fsrResistance *= 2660;              // 10K resistor
      fsrResistance /= fsrVoltage;

      fsrConductance = 1000000; // we measure in micromhos so
      fsrConductance /= fsrResistance;
      fsrReading[i] = fsrConductance;
    }
  }
}
// ==============================================

void setup(void) {

  Serial.begin(9600);

  // Attach servos
  servo_FR1.attach(2);
  servo_FR2.attach(3);
  servo_RR1.attach(4);
  servo_RR2.attach(5);
  servo_FL1.attach(6);
  servo_FL2.attach(7);
  servo_RL1.attach(8);
  servo_RL2.attach(9);

  // Move legs to start position
  moveR_func(0, 0, servo_FR1, servo_FR2);
  delay(500);
  moveL_func(0, 0, servo_FL1, servo_FL2);
  delay(500);
  moveR_func(0, 0, servo_RR1, servo_RR2);
  delay(500);
  moveL_func(0, 0, servo_RL1, servo_RL2);
  delay(500);

  // Foot height calibration
  for (int i = 0; i < 4; i++) {
    while (fsrReading[i] < 40) {
      fsrReading[i] = analogRead(fsrPins[i]);
      if (i < 2) {
        moveL_func(0, py[i], ben_lista[i][0], ben_lista[i][1]);
      } else {
        moveR_func(0, py[i], ben_lista[i][0], ben_lista[i][1]);
      }
      delay(25);
      py[i] += 0.025;
    }
    koords[i][1] = py[i]; // update y koordinate
    delay(500);
  }

  // init parabola function
  Parab_func(delta_x);
}

// ==============================================

void loop(void) {
  for (int i = 0; i < 4; i++) {

    read_fsrs();
    support_func(i + 1); // get reference point for COM
    err = (r - com[0]);    // compute error for PID controller, switch signs for feet

    for (int j = 0; j < steg + steg_delay; j++) {

      Serial.print("reference:");
      Serial.print(r);
      Serial.print(",");
      Serial.print("y:");
      Serial.println(com[0]);
      Serial.print(",");
      Serial.print("error:");
      Serial.println(err);

      // control loop
      PID = KP * err + KI * integral + KD * derivative;
      float u = constrain(PID, -0.2, 0.2); // constrain signal to avoid windup/saturation issues
      koords[0][0] -= u;
      koords[1][0] -= u;
      koords[2][0] -= u;
      koords[3][0] -= u;

      // Parabola gait
      koords[i][0] += parab[j][0];
      koords[i][1] += parab[j][1];

      // move legs
      moveL_func(koords[0][0], koords[0][1], servo_RL1, servo_RL2);
      moveL_func(koords[1][0], koords[1][1], servo_FL1, servo_FL2);
      moveR_func(koords[2][0], koords[2][1], servo_RR1, servo_RR2);
      moveR_func(koords[3][0], koords[3][1], servo_FR1, servo_FR2);

      // recompute error for control loop
      previousError = err;
      read_fsrs();
      app_com(); // approximate composition
      err = (r - com[0]);

      // compute stuff for integral and derivative part of PID
      previousTime = currentTime;          // Previous time is stored before the actual time read
      currentTime = millis();              // Current time actual time read
      deltaT = ((currentTime - previousTime) / 1000); // Divide by 1000 to get seconds
      integral += err * deltaT;
      derivative = (err - previousError) / deltaT;

      delay(500);
    }
    delay(500);
  }
}