#include <ThingSpeak.h>
#include <ESP8266WiFi.h>

// Connect to WiFi
char ssid[] = "Investigadores";           // Network SSID (name).
char pass[] = "12345678";                 // Network password.
long channelID = 722844;                  // Channel ID.
char writeAPIKey[] = "F03UTXYS8L2FUDHV";  // Channel Write API Key.

const unsigned long postingInterval = 10L * 1000L; // Post data every 10 seconds.

WiFiClient client;        // Initialize the Wifi client library.

int connectWifi();
unsigned long lastConnectionTime = 0;

// Parameters
const int drive_distance = 60;            // cm
const int motor_power = 300;              // 0-1024
const int motor_offset = 4;               // Diff. when driving straight
const int wheel_d = 63;                   // Wheel diameter (mm)
const float wheel_c = PI * wheel_d;       // Wheel circumference (mm)
const int counts_per_rev = 40;            // (4 pairs N-S) * (48:1 gearbox) * (2 falling/rising edges) = 384

// Pins
//Motor A (RIGHT)
const int enc_r_pin = D4;  //2;          // D4 EncoderA
const int pwma_pin = D2;  //4;           // D2 PWM A
const int ain1_pin = D0;  //16;          // D0 Motor A
const int ain2_pin = D1;  //5;           // D1 Motor A

//const int stby_pin = 11;

//Motor B (LEFT)
const int enc_l_pin = D5; //14;         // D5 EncoderB
const int pwmb_pin = D6; //12;          // D6 PWM B
const int bin1_pin = D7; //13;          // D7 Motor B
const int bin2_pin = D8; //15;          // D8 Motor B        

// Globals
volatile unsigned long enc_l = 0;
volatile unsigned long enc_r = 0;
unsigned long millis_anterior = 0;
int i;

// Arrays

float distance[8];
float angles[8];
float path[8];

void setup() {
  // Serial initialize and connect
  Serial.begin(115200);
  delay(10);
  Serial.println("Start");

  connectWifi();

  Serial.println("Connected to wifi");
  ThingSpeak.begin(client);
  
  // Pin 
  pinMode(enc_l_pin, INPUT_PULLUP);
  pinMode(enc_r_pin, INPUT_PULLUP);
  pinMode(pwma_pin, OUTPUT);
  pinMode(ain1_pin, OUTPUT);
  pinMode(ain2_pin, OUTPUT);
  pinMode(pwmb_pin, OUTPUT);
  pinMode(bin1_pin, OUTPUT);
  pinMode(bin2_pin, OUTPUT);
//  pinMode(stby_pin, OUTPUT);

  // Set up interrupts
  attachInterrupt(digitalPinToInterrupt(enc_l_pin), countLeft, CHANGE);
  attachInterrupt(digitalPinToInterrupt(enc_r_pin), countRight, CHANGE);

  // Drive
  delay(3000);
//  enableMotors(true);

  // Ruta a seguir:
  
// Forward 60cm
//  driveStraight(drive_distance, motor_power);
//  unsigned long millis_actual = millis();

//  Turn Left
// Primera vuelta
  turnLeft();
  delay(1000); //60 Grados
//  turnLeft_Angle(3.1416/3);
  brake();
  delay(20);
  Serial.println(" ");
  Serial.println("Gira pi/3");
  Serial.print("Pulsos encoder A ");
  Serial.print(enc_r);
  Serial.print(" Angulo calc.");
  Serial.println(enc_r*9);
  angles[0] = enc_r*9;
  Serial.print("Pulsos encoder B ");
  Serial.print(enc_l);
  Serial.print(" Angulo calc.");
  Serial.println(enc_l*9);
  angles[1] = enc_l*9;
  Serial.println(" ");
  minimum(enc_l*9 , enc_r*9 , 60);
  //
  driveStraight(94, motor_power);
  delay(1000);
  //
  Serial.println("Avanza 93.91cm ");
  Serial.print("Pulsos encoder A ");
  Serial.print(enc_r);
  Serial.print(" Distancia calc.");
  Serial.println(enc_r*6.3*3.1416/40);
  distance[0] = enc_r*6.3*3.1416/40;
  Serial.print("Pulsos encoder B ");
  Serial.print(enc_l);
  Serial.print(" Distancia calc.");
  Serial.println(enc_l*6.3*3.1416/40);
  distance[1] = enc_l*6.3*3.1416/40;
  Serial.println(" ");
  minimum(enc_l*6.3*3.1416/40 , enc_r*6.3*3.1416/40 , 93.91);
// Segunda vuelta
  turnLeft_Angle(3*3.1416/18);
  delay(1000);
  brake();
  delay(20);
  Serial.println("Gira pi/6 ");
  Serial.print("Pulsos encoder A ");
  Serial.print(enc_r);
  Serial.print(" Angulo calc.");
  Serial.println(enc_r*9);
  angles[2] = enc_r*9;
  Serial.print("Pulsos encoder B ");
  Serial.print(enc_l);
  Serial.print(" Angulo calc.");
  Serial.println(enc_l*9);
  angles[3] = enc_l*9;
  Serial.println(" ");
  minimum(enc_l*9 , enc_r*9 , 30);
  //
  driveStraight(84,motor_power);
  delay(1000);
  Serial.println("Avanza 84cm");
  Serial.print("Pulsos encoder A ");
  Serial.print(enc_r);
  Serial.print(" Distancia calc.");
  Serial.println(enc_r*6.3*3.1416/40);
  distance[2] = enc_r*6.3*3.1416/40;
  Serial.print("Pulsos encoder B ");
  Serial.print(enc_l);
  Serial.print(" Distancia calc.");
  Serial.println(enc_l*6.3*3.1416/40);
  distance[3] = enc_l*6.3*3.1416/40;
  Serial.println(" ");
  minimum(enc_l*6.3*3.1416/40 , enc_r*6.3*3.1416/40 , 84);
// Tercera vuelta
  turnRight_Angle(55*3.1416/180);
  delay(1000);
  brake();
  delay(20);
  Serial.println("Gira pi/4 ");
  Serial.print("Pulsos encoder A ");
  Serial.print(enc_r);
  Serial.print(" Angulo calc.");
  Serial.println(enc_r*9);
  angles[4] = enc_r*9;
  Serial.print("Pulsos encoder B ");
  Serial.print(enc_l);
  Serial.print(" Angulo calc.");
  Serial.println(enc_l*9);
  angles[5] = enc_l*9;
  Serial.println(" ");
  minimum(enc_l*9 , enc_r*9 , 45);
  //
  driveStraight(119,motor_power);
  delay(1000);
  Serial.println("Avanza 119cm");
  Serial.print("Pulsos encoder A ");
  Serial.print(enc_r);
  Serial.print(" Distancia calc.");
  Serial.println(enc_r*6.3*3.1416/40);
  distance[4] = enc_r*6.3*3.1416/40;
  Serial.print("Pulsos encoder B ");
  Serial.print(enc_l);
  Serial.print(" Distancia calc.");
  Serial.println(enc_l*6.3*3.1416/40);
  distance[5] = enc_l*6.3*3.1416/40;
  Serial.println(" ");
  minimum(enc_l*6.3*3.1416/40 , enc_r*6.3*3.1416/40 , 119);
// Cuarta vuelta
//
//  turnRight();
//  delay(20);
  turnRight_Angle(10*3.1416/180);
  delay(1000);
  brake();
  delay(20);
  Serial.println("Gira pi/12 ");
  Serial.print("Pulsos encoder A ");
  Serial.print(enc_r);
  Serial.print(" Angulo calc.");
  Serial.println(enc_r*9);
  angles[6] = enc_r*9;
  Serial.print("Pulsos encoder B ");
  Serial.print(enc_l);
  Serial.print(" Angulo calc.");
  Serial.println(enc_l*9);
  angles[7] = enc_l*9;
  Serial.println(" ");
  minimum(enc_l*9 , enc_r*9 , 15);
  //
  driveStraight(94, motor_power);
  delay(500);
  Serial.println("Avanza 93.915cm");
  Serial.print("Pulsos encoder A ");
  Serial.print(enc_r);
  Serial.print(" Distancia calc.");
  Serial.println(enc_r*6.3*3.1416/40);
  distance[6] = enc_r*6.3*3.1416/40;
  Serial.print("Pulsos encoder B ");
  Serial.print(enc_l);
  Serial.print(" Distancia calc.");
  Serial.println(enc_l*6.3*3.1416/40);
  distance[7] = enc_l*6.3*3.1416/40;
  minimum(enc_l*6.3*3.1416/40 , enc_r*6.3*3.1416/40 , 93.91);
  Serial.println(" ");
//  driveStraight(85, motor_power);
//  turnRight();
  //delay(100);
  //driveStraight(75, motor_power);
  //delay(120); 45 grados
  //delay(215); 90 Grados
//  // Forward for 1m
//  driveStraight(100, motor_power);
//  // Turn Right
//  turnRight();
//  delay(300);
//===========================================

//turnLeft_Angle(0.7854);
  //turnLeft_Angle(103*3.1416/180); // EN RADIANES
//  driveStraight(119, motor_power); //EN CENTIMETROS
//  turnRight_Angle(19.82*3.1416/180);
//  driveStraight(73.43, motor_power);
//  turnLeft_Angle(49.228*3.1416/180);
//  driveStraight(220, motor_power);
  brake();

   // If interval time has passed since the last connection, write data to ThingSpeak.
  
}

void loop() 
{
//  for (i = 0; i<8; i++)
//  {
    if (millis() - lastConnectionTime > postingInterval)
    {
      ThingSpeak.setField(1, path[0]);
      Serial.println(path[0]);
      ThingSpeak.setField(2, path[1]);
      Serial.println(path[1]);
      ThingSpeak.setField(3, path[2]);
      Serial.println(path[2]);
      ThingSpeak.setField(4, path[3]);
      Serial.println(path[3]);
      ThingSpeak.setField(5, path[4]);
      Serial.println(path[4]);
      ThingSpeak.setField(6, path[5]);
      Serial.println(path[5]);
      ThingSpeak.setField(7, path[6]);
      Serial.println(path[6]);
      ThingSpeak.setField(8, path[7]);
      Serial.println(path[7]);
      ThingSpeak.writeFields(channelID, writeAPIKey);

      lastConnectionTime = millis();
    }
//  delay(15000);
//  } 
  // Send a zero to mark the end of path
//  if (millis() - lastConnectionTime > postingInterval)
//    {
//      ThingSpeak.setField(1, 0);
//      Serial.println(path[i]);
//      ThingSpeak.writeFields(channelID, writeAPIKey);
//
//      lastConnectionTime = millis();
//    }
  // Do nothing
}

void driveStraight(float dist, int power) 
{

  unsigned long num_ticks_l;
  unsigned long num_ticks_r;

  // Set initial motor power
  int power_l = motor_power;
  int power_r = motor_power;

  // Used to determine which way to turn to adjust
  unsigned long diff_l;
  unsigned long diff_r;

  // Reset encoder counts
  enc_l = 0;
  enc_r = 0;

  // Remember previous encoder counts
  unsigned long enc_l_prev = enc_l;
  unsigned long enc_r_prev = enc_r;

  // Calculate target number of ticks
  float num_rev = (dist * 10) / wheel_c;  // Convert to mm
  unsigned long target_count = num_rev * counts_per_rev;
  
  
  // Drive until one of the encoders reaches desired count
  while ( (enc_l < target_count) && (enc_r < target_count) ) {

    // Sample number of encoder ticks
    num_ticks_l = enc_l;
    num_ticks_r = enc_r;

  
    // Drive
    drive(power_l, power_r);

    // Number of ticks counted since last time
    diff_l = num_ticks_l - enc_l_prev;
    diff_r = num_ticks_r - enc_r_prev;

    // Store current tick counter for next time
    enc_l_prev = num_ticks_l;
    enc_r_prev = num_ticks_r;

    // If left is faster, slow it down and speed up right
    if ( diff_l > diff_r ) {
      power_l -= motor_offset;
      power_r += motor_offset;
    }

    // If right is faster, slow it down and speed up left
    if ( diff_l < diff_r ) {
      power_l += motor_offset;
      power_r -= motor_offset;
    }

    // Brief pause to let motors respond
    delay(20);
  }

  // Brake
//  brake();
}

void driveBack(float dist, int power) 
{

  unsigned long num_ticks_l;
  unsigned long num_ticks_r;

  // Set initial motor power
  int power_l = motor_power;
  int power_r = motor_power;

  // Used to determine which way to turn to adjust
  unsigned long diff_l;
  unsigned long diff_r;

  // Reset encoder counts
  enc_l = 0;
  enc_r = 0;

  // Remember previous encoder counts
  unsigned long enc_l_prev = enc_l;
  unsigned long enc_r_prev = enc_r;

  // Calculate target number of ticks
  float num_rev = (dist * 10) / wheel_c;  // Convert to mm
  unsigned long target_count = num_rev * counts_per_rev;
  
  
  // Drive until one of the encoders reaches desired count
  while ( (enc_l < target_count) && (enc_r < target_count) ) {

    // Sample number of encoder ticks
    num_ticks_l = enc_l;
    num_ticks_r = enc_r;

  
    // Drive
    driveBack(power_l, power_r);

    // Number of ticks counted since last time
    diff_l = num_ticks_l - enc_l_prev;
    diff_r = num_ticks_r - enc_r_prev;

    // Store current tick counter for next time
    enc_l_prev = num_ticks_l;
    enc_r_prev = num_ticks_r;

    // If left is faster, slow it down and speed up right
    if ( diff_l > diff_r ) {
      power_l -= motor_offset;
      power_r += motor_offset;
    }

    // If right is faster, slow it down and speed up left
    if ( diff_l < diff_r ) {
      power_l += motor_offset;
      power_r -= motor_offset;
    }

    // Brief pause to let motors respond
    delay(20);
  }

  // Brake
 // brake();
}

//void enableMotors(boolean en) {
//  if ( en ) {
//    digitalWrite(stby_pin, HIGH);
//  } else {
//    digitalWrite(stby_pin, LOW);
//  }
//}

// Attempts a connection to WiFi network and repeats until successful.
int connectWifi()
{
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Connecting to WiFi");
    delay(2500);
  }
  Serial.println("Connected");
}

void drive(int power_a, int power_b) {


    digitalWrite(ain1_pin, LOW);
    digitalWrite(ain2_pin, HIGH);
 
    digitalWrite(bin1_pin, LOW);
    digitalWrite(bin2_pin, HIGH);
 
  analogWrite(pwma_pin, abs(power_a));
  analogWrite(pwmb_pin, abs(power_b));
}

void driveBack(int power_a, int power_b) {


    digitalWrite(ain1_pin, HIGH);
    digitalWrite(ain2_pin, LOW);
 
    digitalWrite(bin1_pin, HIGH);
    digitalWrite(bin2_pin, LOW);
 
  analogWrite(pwma_pin, abs(power_a));
  analogWrite(pwmb_pin, abs(power_b));
}

void turnLeft() {
  // Reset encoder counts
  enc_l = 0;
  enc_r = 0;
  
    digitalWrite(ain1_pin, LOW);
    digitalWrite(ain2_pin, HIGH);
 
    digitalWrite(bin1_pin, HIGH);
    digitalWrite(bin2_pin, LOW);
 
  analogWrite(pwma_pin, 300);
  analogWrite(pwmb_pin, 300);
}

void turnRight() {
  // Reset encoder counts
  enc_l = 0;
  enc_r = 0;

    digitalWrite(ain1_pin, HIGH);
    digitalWrite(ain2_pin, LOW);
 
    digitalWrite(bin1_pin, LOW);
    digitalWrite(bin2_pin, HIGH);
 
  analogWrite(pwma_pin, 300);
  analogWrite(pwmb_pin, 300);
}

void turnLeft_Angle(int theta) {
  unsigned long num_ticks_l;
  unsigned long num_ticks_r;
  // Reset encoder counts
  enc_l = 0;
  enc_r = 0;

   // Calculate target number of ticks
   int arc=0;
  arc = 6.2*theta; // en centimetros
  float num_rev = (arc * 10) / wheel_c;  // Convert to mm
  unsigned long target_count = num_rev * counts_per_rev;

  while ( (enc_l < target_count) || (enc_r < target_count) ) {
 // while ( (enc_l < target_count) && (enc_r < target_count)) {
 
    num_ticks_l = enc_l;
    num_ticks_r = enc_r;
    
    digitalWrite(ain1_pin, LOW);
    digitalWrite(ain2_pin, HIGH);
 
    digitalWrite(bin1_pin, HIGH);
    digitalWrite(bin2_pin, LOW);
 
  analogWrite(pwma_pin, 350);
  analogWrite(pwmb_pin, 350);
  }
}

void turnRight_Angle(int theta) {
  unsigned long num_ticks_l;
  unsigned long num_ticks_r;
  // Reset encoder counts
  enc_l = 0;
  enc_r = 0;
   // Calculate target number of ticks
   int arc=0;
  arc = 7.4*theta; // en centimetros
  float num_rev = (arc * 10) / wheel_c;  // Convert to mm
  unsigned long target_count = num_rev * counts_per_rev;

  while ( (enc_l < target_count) || (enc_r < target_count) ) {

    num_ticks_l = enc_l;
    num_ticks_r = enc_r;

    digitalWrite(ain1_pin, HIGH);
    digitalWrite(ain2_pin, LOW);
 
    digitalWrite(bin1_pin, LOW);
    digitalWrite(bin2_pin, HIGH);
 
  analogWrite(pwma_pin, 350);
  analogWrite(pwmb_pin, 350);
  }
}

void brake() {
  digitalWrite(ain1_pin, LOW);
  digitalWrite(ain2_pin, LOW);
  digitalWrite(bin1_pin, LOW);
  digitalWrite(bin2_pin, LOW);
  analogWrite(pwma_pin, 0);
  analogWrite(pwmb_pin, 0);
}

void countLeft() {
  enc_l++;
}

void countRight() {
  enc_r++;
}

void minimum(float e_l, float e_r, float x) {
  float l = abs(x - e_l);
  float r = abs(x - e_r);

  if (l >= r){ path[i] = e_r; }
  else{ path[i] = e_l; }

  i++;
  }
