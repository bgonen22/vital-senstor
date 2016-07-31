#include <Adafruit_NeoPixel.h>
#include <NewPing.h>
#define trigPin 3
#define echoPin 2
#define led 8

// SPI pins for sd reader
//#define cs 10
//#define mosi 11
//#define miso 12
//#define sck 13
//

//#define timeLimit 10000//milliseconds
#define speakerPin 9
//#define delayTime 100//milliseconds
#define maxBeeps 300

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN            6

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      5

int badLoops;
int goodLoops;
int beepStart;
int beepCount;
int threshold = 10;//(timeLimit / delayTime);

int iterations_for_avg = 5;
NewPing sonar(trigPin, echoPin ,300 ) ;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// -----------------------------------------------
//turn off alarm
// -----------------------------------------------
void unAlert(){
  digitalWrite(speakerPin, LOW);
  badLoops = 0;
  goodLoops = 0;
  beepCount = 0;
  light(pixels.Color(0,0,0));
  Serial.println("SILENT!");
}

// -----------------------------------------------
//turn on alarm
// -----------------------------------------------
void startAlert(){
  unsigned long dist;
  digitalWrite(speakerPin, LOW);  
  int far;
  while (beepCount <= maxBeeps) {
    far = 0;
  //  analogWrite(speakerPin,190);
  //  light(pixels.Color(0,0,150)); //blue
    delay(1000);
    light(pixels.Color(150,0,0)); //red
    delay(1000);
    dist = get_dist();    
    while (dist > 250 || dist < 5) {
      far++;
      delay(100);
      dist = get_dist();    
      if (far == 3) {
        break;
      }
    }
    if (far == 3) {
      break;
    }
   }
   Serial.println("OUT OF ALARM");  
   beepCount += 1;
}
// -----------------------------------------------
// get_dist
// -----------------------------------------------
unsigned long get_dist() {
    unsigned long duration,distance;
      //10 microsec trigger pulse
 /*   digitalWrite(trigPin, LOW); 
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    duration = pulseIn(echoPin, HIGH);
    //convert measurement to distance in cm
    distance = (duration/2) / 29.1;
    Serial.print(duration);
    Serial.print(" dist ");
    Serial.println(distance);
    return distance;
*/
    delay(50);    
    duration = sonar.ping_median(iterations_for_avg);
    distance = sonar.convert_cm(duration);
    Serial.print(duration);
    Serial.print(" dist ");
    Serial.println(distance);
    return distance;

}

// -----------------------------------------------
// Aziz light!
// -----------------------------------------------
void light (uint32_t color) {
  for (int i = 0 ; i < NUMPIXELS; ++i) {
    pixels.setPixelColor(i, color); 
  }
  pixels.show();
}

// -----------------------------------------------
// setup
// -----------------------------------------------
void setup() {
  badLoops = 0;
  goodLoops = 0;
  beepCount = 0;
  Serial.begin (9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(led, OUTPUT);
  pinMode(speakerPin, OUTPUT);
  pixels.begin(); // This initializes the NeoPixel library.
}

// -----------------------------------------------
// loop
// -----------------------------------------------
void loop() {
  // put your main code here, to run repeatedly:
  unsigned long distance;
  distance = get_dist();
  
  if (distance < 250 && distance > 5) {  // Turn on LED if something is between 5 and 250 cm away from sensor.
    digitalWrite(led,HIGH);
    badLoops += 1;
    if(badLoops >= threshold) //if badLoops reaches threshold, turn on alarm
    startAlert();
    goodLoops = 0;
  }

//if goodLoops reaches certain value, turn off alarm and reset badLoops
  else {
    goodLoops += 1;
    light(pixels.Color(0,0,0));

    if(goodLoops >= 5); //set goodloops value (depending on amount of noise)
      unAlert();
    digitalWrite(led,LOW);
  }
   Serial.print("loops: ");
  Serial.print(goodLoops);
  Serial.print(" ");
  Serial.print(badLoops);
  Serial.println();
     
  delay(1000);
 
}
