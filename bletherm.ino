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

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)

BLEPeripheral blePeripheral = BLEPeripheral(BLE_REQ, BLE_RDY, BLE_RST);

BLEService tempService = BLEService("CCC0");
BLEFloatCharacteristic tempCharacteristic = BLEFloatCharacteristic("CCC1", BLERead | BLENotify);
BLEDescriptor tempDescriptor = BLEDescriptor("2901", "Temp in Celsius");

BLEService testService = BLEService("CC25");
BLEFloatCharacteristic testCharacteristic = BLEFloatCharacteristic("CC32", BLERead | BLENotify);
BLEDescriptor testDescriptor = BLEDescriptor("2765", "Test");

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //Declaring the display name (display)
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

float lastTempReading;

void blePeripheralConnectHandler(BLECentral& central) {
  Serial.print(F("Connected event, central: "));
  Serial.println(central.address());
}

void blePeripheralDisconnectHandler(BLECentral& central) {
  Serial.print(F("Disconnected event, central: "))
  Serial.println(central.address());
}

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

void displayTestText() {
    display.setTextSize(1);
  display.setCursor(0, 50);
  display.println("test text123456");
}

void setTempCharacteristicValue() {
  float reading = (mlx.readObjectTempC(), 1);
  //  float reading = random(100);
  tempCharacteristic.setValue(reading);
  Serial.print(F("Temperature: ")); Serial.print(reading); Serial.println(F("C"));
  lastTempReading = reading;
}

void setTestCharacteristic() {
  testCharacteristic.setValue(11);
}

void setup() {
  Serial.begin(115200);
#if defined (__AVR_ATmega32U4__)
  delay(5000);  //5 seconds delay for enabling to see the start up comments on the serial board
#endif

  blePeripheral.setLocalName("Temperature");

  blePeripheral.setAdvertisedServiceUuid(tempService.uuid());
  blePeripheral.addAttribute(tempService);
  blePeripheral.addAttribute(tempCharacteristic);
  blePeripheral.addAttribute(tempDescriptor);

  blePeripheral.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  blePeripheral.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);

  blePeripheral.begin();

  mlx.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //Start the OLED display
  display.clearDisplay();
  display.display();


  Serial.println(F("BLE Temperature Sensor Peripheral"));
}

void loop() {
  blePeripheral.poll();
  setTempCharacteristicValue();
  setTestCharacteristic();
  displayTemp();
  displayTestText();
  delay(500);
}
