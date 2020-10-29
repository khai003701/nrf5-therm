#include <SPI.h>
#include <BLEPeripheral.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>

// define pins
#define BLE_REQ   10
#define BLE_RDY   2
#define BLE_RST   9

#define SCREEN_WIDTH 128 // display width
#define SCREEN_HEIGHT 64 // display height
#define OLED_RESET    -1 // reset pin 

// setup bluetooth and peripherals
BLEPeripheral blePeripheral = BLEPeripheral(BLE_REQ, BLE_RDY, BLE_RST);

BLEService tempService = BLEService("CCC0");
BLEFloatCharacteristic tempCharacteristic = BLEFloatCharacteristic("CCC1", BLERead | BLENotify);
BLEDescriptor tempDescriptor = BLEDescriptor("2901", "Temp in Celsius");

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); 
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

float lastTempReading;

// event handlers for connect / disconnect
void blePeripheralConnectHandler(BLECentral& central) {
  Serial.print(F("Connected event, central: "));
  Serial.println(central.address());
}

void blePeripheralDisconnectHandler(BLECentral& central) {
  Serial.print(F("Disconnected event, central: "))
  Serial.println(central.address());
}

// display code
void displayTemp() {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 20);
  display.println("Target");

  display.setTextSize(1);
  display.setCursor(50, 20);
  display.println(mlx.readObjectTempC(), 1);

  display.setCursor(110, 20);
  display.println("C");

  display.display();
}

// void displayTestText() {
 // display.setTextSize(1);
 // display.setCursor(0, 50);
 // display.println("test text123456");
// }

void setTempCharacteristicValue() {
  float reading = (mlx.readObjectTempC(), 1);
  //  float reading = random(10);
  tempCharacteristic.setValue(reading);
  Serial.print(reading);
  Serial.println(F("C"));
  lastTempReading = reading;
}

void setup() {
  Serial.begin(115200);
  
  // init temperature service
  blePeripheral.setLocalName("Temperature");

  blePeripheral.setAdvertisedServiceUuid(tempService.uuid());
  blePeripheral.addAttribute(tempService);
  blePeripheral.addAttribute(tempCharacteristic);
  blePeripheral.addAttribute(tempDescriptor);

  blePeripheral.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  blePeripheral.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);

  blePeripheral.begin();

  mlx.begin(); // init sensor
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // start the display
  display.clearDisplay(); // clear display buffer
  display.display();


  Serial.println(F("started"));
}

void loop() {
  blePeripheral.poll();
  setTempCharacteristicValue();
  displayTemp();
  // displayTestText();
  delay(500);
}
