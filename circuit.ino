///////display
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
// Inicia um novo objeto do display oled
Adafruit_SSD1306 oledDisplay(SCREEN_WIDTH, SCREEN_HEIGHT);

//////Json
#include <ArduinoJson.h>
const int TAMANHO = 200;

// Pino do fotoressistor
const int pinPhotoR = A0;
// Pino do termistor
const int pinThermR = A1;
// Pino do botão
const int pinButton = 12;
// Resistência do fotoressistor
const int luxR1 = 10000;
// Resistência do termistor
const int thermR1 = 10000;
// Valor quando o fotoressistor detectar nenhuma luz
const float darkValue = 127410;
// Gamma do fotoressistor
const float gamma = 0.8582;

// Alterna entre mostrar a temperatura ou a luminosidade no display
bool showLUX = false;
// Luz UV atual
float lux;
// Temperatura atual
float temp;

void setup() {
	//inicializa comunicação serial
	Serial.begin(115200);
	// Inicia o display
	oledDisplay.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
	
	// Show initial display buffer contents on the screen --
  	// the library initializes this with an Adafruit splash screen.
	  oledDisplay.display();
 	delay(2000); // Pause for 2 seconds
	
	 // Clear the buffer
  	oledDisplay.clearDisplay();

	// Inicia o pino do botão como input
	pinMode(pinButton, INPUT);
}

void loop() {
	// Valor bruto do fotoressistor
	float rawLux = analogRead(pinPhotoR);
	// Valor bruto do termistor
	float rawTemp = analogRead(pinThermR);
	
	StaticJsonDocument<TAMANHO> json; //cria o objeto Json dos valores dos sensores
	json["topic"] = "sensores";
	json["rawTemp"] = rawTemp;
	json["rawLux"] = rawLux;
	json["tempR1"] = thermR1;
	json["luxR1"] = luxR1;
	json["luxGamma"] = gamma;
	json["luxDarkValue"] = darkValue;

	serializeJson(json, Serial);
  	Serial.println();
	
	// Calcula a luminosidade atual
	lux = calcLux(rawLux);
	// Lê a temperatura atual
	temp = calcTemp(rawTemp);
	
	// Estado do botão
	int button = digitalRead(pinButton);
	if (button == LOW) {
		// Alterna entre mostrar a temperatura ou a luminosidade no display
		showLUX = !showLUX;
		// [DEBUG] Mostra temperatura e luminosidade
		Serial.println(String("TEMP: ") + temp);
		Serial.println(String("LUX: ") + lux);
	}
	
	// Mostra o display
	displayInfo();
	// Espera 0.25 segundos
	delay(250);
}

void displayInfo() {
	// Limpa o display
	oledDisplay.clearDisplay();
	
	// Muda o tamanho do texto
	oledDisplay.setTextSize(1);
	// Muda a cor do texto
	oledDisplay.setTextColor(SSD1306_WHITE);
	// Posição inicial do texto
	oledDisplay.setCursor(2,2);
	if (showLUX) {
		// Mostra a luminosidade
		oledDisplay.print("LUX: ");
		oledDisplay.print(lux);
	} else {
		// Mostra a temperatura
		oledDisplay.print("TEMP: ");
		oledDisplay.print(temp);
		oledDisplay.print(" C");
	}
	oledDisplay.println();
	
	// Aplica os comandos do display
	oledDisplay.display();
}

// Função para calcular a luminosidade a partir de um valor bruto
float calcLux(float raw) {
	// Converte de 0-1023 para 0v-5v
	float voltage = raw * (5.0 / 1023.0);
	float rLDR = luxR1 / ((5.0 / voltage) - 1);
	return pow(abs(darkValue / rLDR), 1.0/gamma);
}

//função que faz leitura da temperatura e retorna o valor em graus celcius
float calcTemp(float raw){
	//valores constante para calculo da temperatura
	float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07; 

	// Lê a entrada
  float tempOut = raw;
//calculo R2, demonstração no arquivo pdf da aula
  tempOut = thermR1 * (1023.0 / tempOut - 1.0);
  tempOut = log(tempOut);
// Equação de Steinhart–Hart 
  tempOut = (1.0 / (c1 + c2*tempOut + c3*tempOut*tempOut*tempOut));// Equação de Steinhart–Hart 
  tempOut = tempOut - 273.15; //temperatura em Graus celcius
  //Tf = (Tc * 9.0)/ 5.0 + 32.0; // temp em fahrenheit
  return tempOut;
}