#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

Adafruit_SSD1306 oledDisplay(SCREEN_WIDTH, SCREEN_HEIGHT);

const int pinPhotoR = A0;
const int pinThermR = A1;
const int pinButton = 12;
const int r1 = 10000;
const int darkValue = 1024;

bool showLUX = false;
float lux;
float temp;

void setup() {
	Serial.begin(9600);
	oledDisplay.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
	
	// Show initial display buffer contents on the screen --
  	// the library initializes this with an Adafruit splash screen.
	  oledDisplay.display();
 	delay(2000); // Pause for 2 seconds
	
	 // Clear the buffer
  	oledDisplay.clearDisplay();

	pinMode(pinButton, INPUT);
}

void loop() {
	float raw = analogRead(pinPhotoR);
	lux = calcLux(raw);
	temp = readTemp(pinThermR, 10000);
	
	int button = digitalRead(pinButton);
	if (button == LOW) {
		showLUX = !showLUX;
		Serial.println(String("TEMP: ") + temp);
		Serial.println(String("LUX: ") + lux);
	}
	
	displayInfo();
	delay(250);
}

void displayInfo() {
	oledDisplay.clearDisplay();
	
	oledDisplay.setTextSize(1);
	oledDisplay.setTextColor(SSD1306_WHITE);
	oledDisplay.setCursor(2,2);
	if (showLUX) {
		oledDisplay.println(String("LUX: ") + lux);
	} else {
		oledDisplay.println(String("TEMP: ") + temp + String(" °C"));
	}
	oledDisplay.display();
}

float calcLux(float raw) {
	raw *= 0.001;
	float rLDR = r1 / ((5.0 / raw) - 1);
	return pow(darkValue / rLDR, 1.0/0.77);
}

//função que faz leitura da temperatura e retorna o valor em graus celcius
float readTemp(int pin, float resistencia){
	//valores constante para calculo da temperatura
	float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07; 

	// Lê a entrada
  float tempOut = (float)analogRead(pin);
//calculo R2, demonstração no arquivo pdf da aula
  tempOut = resistencia * (1023.0 / tempOut - 1.0);
  tempOut = log(tempOut);
// Equação de Steinhart–Hart 
  tempOut = (1.0 / (c1 + c2*tempOut + c3*tempOut*tempOut*tempOut));// Equação de Steinhart–Hart 
  tempOut = tempOut - 273.15; //temperatura em Graus celcius
  //Tf = (Tc * 9.0)/ 5.0 + 32.0; // temp em fahrenheit
  return tempOut;
}