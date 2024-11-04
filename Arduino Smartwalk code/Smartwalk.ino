#include <SoftwareSerial.h>

// Define pin numbers for ultrasonic sensor, buzzer, and water sensor
const int trigPin = 7;           // Ultrasonic sensor trigger pin (changed from 9 to 7)
const int echoPin = 8;           // Ultrasonic sensor echo pin (changed from 10 to 8)
const int buzzerPin = 13;        // Buzzer pin
const int waterSensorPin = A5;   // Analog pin for the water level sensor

// Define pin numbers for GSM and buttons
SoftwareSerial mySerial(4, 5);   // RX (changed from 10 to 4), TX (changed from 11 to 5)
const int buttonRunPin = 2;      // Button for "run" message
const int buttonWalkPin = 3;     // Button for "walk" message

// Define thresholds
const int distanceThreshold = 40;    // Distance threshold for object detection (in cm)
const int waterLevelThreshold = 100; // Water level threshold for detection

// Variables to store sensor data
long duration;
int distance;
bool waterDetected = false;       // Water detection state
bool objectDetected = false;      // Object detection state

void setup() {
  // Set up serial communication
  Serial.begin(9600);
  mySerial.begin(9600);

  // Initialize pins for ultrasonic sensor, buzzer, and water sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(waterSensorPin, INPUT);   // Initialize water sensor pin

  // Initialize buttons for GSM
  pinMode(buttonRunPin, INPUT_PULLUP);
  pinMode(buttonWalkPin, INPUT_PULLUP);

  Serial.println("System Initializing...");
  delay(1000);

  mySerial.println("AT");
  Serial.println(readSerial());
}

void loop() {
  // Check if the "run" button is pressed (LOW state)
  if (digitalRead(buttonRunPin) == LOW) {
    SendMessage("EMERGENCY! I NEED HELP");
    delay(1000);  // Debounce delay
  }

  // Check if the "walk" button is pressed (LOW state)
  if (digitalRead(buttonWalkPin) == LOW) {
    SendMessage("I HAVE ARRIVED AT MY DESTINATION");
    delay(1000);  // Debounce delay
  }

  // Trigger the ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the echo pin
  duration = pulseIn(echoPin, HIGH);

  // Calculate distance
  distance = duration * 0.0344 / 2;

  // Print the distance to the serial monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm, Water Detected: ");

  // Read the water sensor state
  waterDetected = analogRead(waterSensorPin) > waterLevelThreshold;
  Serial.println(waterDetected ? "YES" : "NO");

  // Handle buzzer for water detection (buzz twice)
  if (waterDetected) {
    buzz(2);  // Buzz twice for water detection
  }

  // Handle continuous buzzer for object detection
  objectDetected = distance < distanceThreshold;
  if (objectDetected) {
    digitalWrite(buzzerPin, HIGH);  // Keep buzzer ON continuously for object detection
  } else {
    digitalWrite(buzzerPin, LOW);   // Turn OFF buzzer when no object detected
  }

  // Check for incoming messages on GSM
  if (mySerial.available()) {
    delay(1000);
    Serial.println(mySerial.readString());
  }

  // Small delay for stability
  delay(100);
}

// Function to send a custom message
void SendMessage(String message) {
  mySerial.println("AT+CMGF=1");  // Set GSM to text mode
  Serial.println(readSerial());

  mySerial.println("AT+CMGS=\"+639507898930\"");  // Recipient phone number
  Serial.println(readSerial());

  mySerial.println(message);  // Send the custom message
  mySerial.write(26);         // Send Ctrl+Z (ASCII 26) to send the message
  Serial.println("Message Sent!");
  Serial.println(readSerial());
}

// Function to read serial input from GSM module
String readSerial() {
  delay(100);
  String response = "";
  while (mySerial.available()) {
    response += mySerial.readString();
  }
  return response;
}

// Function to buzz the buzzer a specified number of times
void buzz(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(buzzerPin, HIGH);  // Activate the buzzer
    delay(300);                     // Buzz duration
    digitalWrite(buzzerPin, LOW);   // Deactivate the buzzer
    delay(250);                     // Pause between buzzes
  }
}
