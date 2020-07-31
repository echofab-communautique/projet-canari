
// Choose program options.
//#define PRINT_RAW_DATA
#define USE_AVG

// Arduino pin numbers.
const int sharpLEDPin = 7;   // Arduino digital pin 7 connect to sensor LED.
const int sharpVoPin = A3;   // Arduino analog pin 5 connect to sensor Vo.



#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


// For averaging last N raw voltage readings.
#ifdef USE_AVG
#define N 100
static unsigned long VoRawTotal = 0;
static int VoRawCount = 0;
#endif // USE_AVG

// Set the typical output voltage in Volts when there is zero dust.
static float Voc = 0.6;

// Use the typical sensitivity in units of V per 100ug/m3.
const float K = 0.5;

/////////////////////////////////////////////////////////////////////////////

// Helper functions to print a data value to the serial monitor.
void printValue(String text, unsigned int value, bool isLast = false) {
  Serial.print(text);
  Serial.print("=");
  Serial.print(value);
  if (!isLast) {
    Serial.print(", ");
  }
}
void printFValue(String text, float value, String units, bool isLast = false) {
  Serial.print(text);
  Serial.print("=");
  Serial.print(value);
  Serial.print(units);
  if (!isLast) {
    Serial.print(", ");
  }
}

/////////////////////////////////////////////////////////////////////////////

// Arduino setup function.
void setup() {

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
  }
  // Set LED pin for output.
  pinMode(sharpLEDPin, OUTPUT);

  // Start the hardware serial port for the serial monitor.
  Serial.begin(9600);

  // Wait two seconds for startup.
  delay(2000);
  Serial.println("");
  Serial.println("GP2Y1014AU0F Demo");
  Serial.println("=================");
}

// Arduino main loop.
void loop() {
  // Turn on the dust sensor LED by setting digital pin LOW.
  digitalWrite(sharpLEDPin, LOW);

  // Wait 0.28ms before taking a reading of the output voltage as per spec.
  delayMicroseconds(280);

  // Record the output voltage. This operation takes around 100 microseconds.
  int VoRaw = analogRead(sharpVoPin);

  // Turn the dust sensor LED off by setting digital pin HIGH.
  digitalWrite(sharpLEDPin, HIGH);

  // Wait for remainder of the 10ms cycle = 10000 - 280 - 100 microseconds.
  delayMicroseconds(9620);

  // Print raw voltage value (number from 0 to 1023).
#ifdef PRINT_RAW_DATA
  printValue("VoRaw", VoRaw, true);
  Serial.println("");
#endif // PRINT_RAW_DATA

  // Use averaging if needed.
  float Vo = VoRaw;
#ifdef USE_AVG
  VoRawTotal += VoRaw;
  VoRawCount++;
  if ( VoRawCount >= N ) {
    Vo = 1.0 * VoRawTotal / N;
    VoRawCount = 0;
    VoRawTotal = 0;
  } else {
    return;
  }
#endif // USE_AVG

  // Compute the output voltage in Volts.
  Vo = Vo / 1024.0 * 5.0;
  printFValue("Vo", Vo * 1000.0, "mV");

  // Convert to Dust Density in units of ug/m3.
  float dV = Vo - Voc;
  if ( dV < 0 ) {
    dV = 0;
    Voc = Vo;
  }
  float dustDensity = dV / K * 100.0;
  printFValue("DustDensity", dustDensity, "ug/m3", true);
  Serial.println("");

  if ((dustDensity >= 0) && (dustDensity <= 12)) {
    Serial.println("- Good air -");
    display.clearDisplay();                  // Efface tout ce qui ce trouve a l'écran.
    display.setTextSize(1);                  // Ajuste la dimension du texte qui sera affiché.
    display.setTextColor(WHITE);             // Ajuste la couleur du texte qui sera affiché (Dois rester blanc.
    display.setCursor(30, 10);                // Ajuste la position du texte sur l'écran.
    display.print(dustDensity);            // Le texte qui sera imprimé a l'écran
    display.print(" Good air");
    display.display();                       // Permet a l'écran de changer ses caracthères.
  } else if ((dustDensity >= 13) && (dustDensity <= 35)) {
    Serial.println("- Moderate air -");
    display.clearDisplay();                  // Efface tout ce qui ce trouve a l'écran.
    display.setTextSize(1);                  // Ajuste la dimension du texte qui sera affiché.
    display.setTextColor(WHITE);             // Ajuste la couleur du texte qui sera affiché (Dois rester blanc.
    display.setCursor(30, 10);                // Ajuste la position du texte sur l'écran.
    display.print(dustDensity);            // Le texte qui sera imprimé a l'écran
    display.print(" Moyenne");
    display.display();
  } else if ((dustDensity >= 36) && (dustDensity <= 55)) {
    Serial.println("- Unhealthy for sensitive groupe -");
    display.clearDisplay();                  // Efface tout ce qui ce trouve a l'écran.
    display.setTextSize(1);                  // Ajuste la dimension du texte qui sera affiché.
    display.setTextColor(WHITE);             // Ajuste la couleur du texte qui sera affiché (Dois rester blanc.
    display.setCursor(30, 10);                // Ajuste la position du texte sur l'écran.
    display.print(dustDensity);            // Le texte qui sera imprimé a l'écran
    display.print(" Unhealthy");
    display.display();
  } else if ((dustDensity >= 56) && (dustDensity <= 150)) {
    Serial.println("- bad -");
    display.clearDisplay();                  // Efface tout ce qui ce trouve a l'écran.
    display.setTextSize(1);                  // Ajuste la dimension du texte qui sera affiché.
    display.setTextColor(WHITE);             // Ajuste la couleur du texte qui sera affiché (Dois rester blanc.
    display.setCursor(30, 10);                // Ajuste la position du texte sur l'écran.
    display.print(dustDensity);            // Le texte qui sera imprimé a l'écran
    display.print(" Unhealthy");
    display.display();
  } else if ((dustDensity >= 151) && (dustDensity <= 250)) {
    Serial.println("- Very unhealty -");
    display.clearDisplay();                  // Efface tout ce qui ce trouve a l'écran.
    display.setTextSize(1);                  // Ajuste la dimension du texte qui sera affiché.
    display.setTextColor(WHITE);             // Ajuste la couleur du texte qui sera affiché (Dois rester blanc.
    display.setCursor(30, 10);                // Ajuste la position du texte sur l'écran.
    display.print(dustDensity);            // Le texte qui sera imprimé a l'écran
    display.print(" Dangerous");
    display.display();
  } else {
    Serial.println("- You will die soon-");
    display.clearDisplay();                  // Efface tout ce qui ce trouve a l'écran.
    display.setTextSize(1);                  // Ajuste la dimension du texte qui sera affiché.
    display.setTextColor(WHITE);             // Ajuste la couleur du texte qui sera affiché (Dois rester blanc.
    display.setCursor(30, 10);                // Ajuste la position du texte sur l'écran.
    display.print(dustDensity);            // Le texte qui sera imprimé a l'écran
    display.print(" RUN!");
    display.display();
  }



} // END PROGRAM
