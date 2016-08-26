#include <Adafruit_NeoPixel.h>
#include <NewPing.h>
#define trigPin 3
#define echoPin 2
#define greenLed 8
#define redLed 4

// SPI pins for sd reader
//#define cs 10
//#define mosi 11
//#define miso 12
//#define sck 13
//


#define speakerPin 9
#define maxBeeps 3000
#define DIST 100
// Which pin on the Arduino is connected to the NeoPixels?
#define STRIP_PIN            6

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      5

int badLoops;
int goodLoops;
int beepStart;
int threshold = 10;// number of badLoops to start the alarm
int goodThreshold = 3; // number of samples to reset the badLoops

int iterations_for_avg = 5;
NewPing sonar(trigPin, echoPin ,300 ) ;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, STRIP_PIN, NEO_GRB + NEO_KHZ800);
// -----------------------------------------------
//turn off alarm
// -----------------------------------------------
void unAlert(){
  digitalWrite(speakerPin, LOW);
  badLoops = 0;
  goodLoops = 0;
  light(pixels.Color(0,0,0));
  Serial.println("SILENT!");
}

// -----------------------------------------------
//turn on alarm
// -----------------------------------------------
void startAlert(){
  unsigned long dist;
 // digitalWrite(speakerPin, LOW);  
  int far;
  int beepCount;
  far = 0;  
  beepCount = 0;
  Serial.println("start aler");
  digitalWrite(redLed,HIGH);
  while (beepCount <= maxBeeps && far < 3) {
    Serial.print("beepCount ");
    Serial.print(beepCount);
    Serial.print(" far: ");
    Serial.println(far);
    analogWrite(speakerPin,190);
    light(pixels.Color(0,0,150)); //blue
    delay(1000);
    light(pixels.Color(150,0,0)); //red
    delay(1000); 
    dist = get_dist();   
    if (dist > DIST || dist < 5) {
      far++;
      delay(100);
      dist = get_dist();    
      if (far == 3) {
        break;
      }
    } else {
      far = 0;    
    }
    beepCount++ ;
   }
   Serial.println("OUT OF ALARM");  
   unAlert();
}
// -----------------------------------------------
// get_dist
// -----------------------------------------------
unsigned long get_dist() {
    unsigned long duration,distance;
    delay(50);         
    duration = sonar.ping_median(iterations_for_avg);
    distance = sonar.convert_cm(duration);
    Serial.print(duration);
    Serial.print(" dist: ");
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
  Serial.begin (9600);
  Serial.println("setup - begin");
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(speakerPin, OUTPUT);
  pixels.begin(); // This initializes the NeoPixel library.
  unAlert();

}

// -----------------------------------------------
// loop
// -----------------------------------------------
void loop() {
    
  // put your main code here, to run repeatedly:
  unsigned long distance;
  distance = get_dist();

  if (distance < DIST && distance > 5) {  // Turn on LED if something is between 5 and 250 cm away from sensor.
    digitalWrite(greenLed,HIGH);
    badLoops += 1;
    goodLoops = 0;
    if(badLoops >= threshold) { //if badLoops reaches threshold, turn on alarm    
      startAlert();     
    }    
  } else {
    goodLoops++;
    if (goodLoops >= goodThreshold) {
      badLoops = 0;
      digitalWrite(greenLed,LOW);
    }
  }

  Serial.print("loops: ");
  Serial.println(badLoops);
       
  delay(1000);
 
}
