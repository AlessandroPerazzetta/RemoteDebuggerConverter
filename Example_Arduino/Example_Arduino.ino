////////
// Libraries Arduino
//
// Library: Remote debug - debug over telnet - for Esp8266 (NodeMCU) or ESP32
// Author: Joao Lopes
//
// Attention: This library is only for help development. Please not use this in production
//
// Sample to show how to use it - advanced one
//
// Example of use:
//
//        if (Debug.isActive(Debug.<level>)) { // <--- This is very important to reduce overheads and work of debug levels
//            Debug.printf("bla bla bla: %d %s\n", number, str);
//            Debug.println("bla bla bla");
//        }
//
// Or short way (prefered if only one debug at time)
//
//		debugAln("This is a any (always showed) - var %d", var);
//
//		debugV("This is a verbose - var %d", var);
//		debugD("This is a debug - var %d", var);
//		debugI("This is a information - var %d", var);
//		debugW("This is a warning - var %d", var);
//		debugE("This is a error - var %d", var);
//
//		debugV("This is println");
//
//
///////

// Libraries

#if defined (ESP8266)

#define USE_MDNS true // Use the MDNS ?

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

#ifdef USE_MDNS
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#endif

//#include <ESP8266WebServer.h> // Discomment if you need web server`

#define USE_ARDUINO_OTA true

#elif defined(ESP32)

#define USE_MDNS true // Use the MDNS ?

// Includes do ESP32

#include <WiFi.h>

#ifdef USE_MDNS
#include <DNSServer.h>
#include "ESPmDNS.h"
#endif

#define USE_ARDUINO_OTA  true

#else

#error "The board must be ESP8266 or ESP32"

#endif // ESP

// ArduinoOTA

#ifdef USE_ARDUINO_OTA
#include <ArduinoOTA.h>
#endif

// Production

//#define PRODUCTION true

// HTTP Web server - discomment if you need this
//#if defined ESP8266
//ESP8266WebServer HTTPServer(80);
//#elif defined ESP32
//WebServer HTTPServer(80);
//#endif

// Remote debug over telnet - not recommended for production, only for development
// I put it to show how to do code clean to development and production

#ifndef PRODUCTION // Not in PRODUCTION

#include "RemoteDebug.h"        //https://github.com/JoaoLopesF/RemoteDebug

// Instance of RemoteDebug

RemoteDebug Debug;

#endif

// Host name

#define HOST_NAME "rem-debug" // PLEASE CHANGE IT

// Time

uint32_t mLastTime = 0;
uint32_t mTimeSeconds = 0;

// Buildin Led ON ?

boolean mLedON = false;

// Time

uint8_t mRunSeconds = 0;
uint8_t mRunMinutes = 0;
uint8_t mRunHours = 0;

// Globals for this example

boolean mBoolean = false;
char mChar = 'X';
byte mByte = 'Y';
int mInt = 1;
unsigned int mUInt = 2;
long mLong = 3;
unsigned long mULong = 4;
float mFloat = 5.0f;
double mDouble = 6.0;

String mString = "This is a string";
String mStringLarge = "This is a large stringggggggggggggggggggggggggggggggggggggggggggggg";

char mCharArray[] = "This is a char array";
char mCharArrayLarge[] = "This is a large char arrayyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy";

int mIntArray[5] = {1 ,2 ,3, 4, 5};

//const char mCharArrayConst[] = "This is const";

////// Setup

void setup() {

	// Initialize the Serial (educattional use only, not need in production)

	Serial.begin(115200);

	// Buildin led of ESP

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

	// Connect WiFi

	connectWiFi();

	// Host name of WiFi

#ifdef ESP8266
	WiFi.hostname(HOST_NAME);
#endif

#ifdef USE_ARDUINO_OTA
	// Update over air (OTA)

	initializeOTA();
#endif

	// Register host name in mDNS

#if defined (USE_MDNS) && defined(HOSTNAME)
	if (MDNS.begin(HOST_NAME)) {
		Serial.print("* MDNS responder started. Hostname -> ");
		Serial.println(HOST_NAME);
	}
	// Register the services

	// MDNS.addService("http", "tcp", 80);   // Web server - discomment if you need this

	MDNS.addService("telnet", "tcp", 23);// Telnet server RemoteDebug
#endif

	// HTTP web server
	// Discomment if you need this
	//
	// HTTPServer.on("/", handleRoot);
	//
	// HTTPServer.onNotFound(handleNotFound);
	//
	// HTTPServer.begin();
//
// #ifndef PRODUCTION // Not in PRODUCTION
//     Serial.println("* HTTP server started");
// #endif

	// Initialize the telnet server of RemoteDebug

#ifndef PRODUCTION // Not in PRODUCTION

	Debug.begin(HOST_NAME); // Initiaze the telnet server

	//Debug.setPassword("r3m0t0."); // Password on telnet connection ?

	Debug.setResetCmdEnabled(true); // Enable the reset command

	//Debug.showDebugLevel(false); // To not show debug levels
	//Debug.showTime(true); // To show time
	//Debug.showProfiler(true); // To show profiler - time between messages of Debug
	// Good to "begin ...." and "end ...." messages

	Debug.showProfiler(true); // Profiler
	Debug.showColors(true); // Colors

	// Debug.setSerialEnabled(true); // if you wants serial echo - only recommended if ESP is plugged in USB

	String helpCmd = "bench1 - Benchmark 1\n";
	helpCmd.concat("bench2 - Benchmark 2");

	Debug.setHelpProjectsCmds(helpCmd);
	Debug.setCallBackProjectCmds(&processCmdRemoteDebug);

	// This sample

	Serial.println("* Arduino RemoteDebug Library");
	Serial.println("*");
	Serial.print("* WiFI connected. IP address: ");
	Serial.println(WiFi.localIP());
	Serial.println("*");
	Serial.println(
			"* Please use the telnet client (telnet for Mac/Unix or putty and others for Windows)");
	Serial.println("*");
	Serial.println("* This sample will send messages of debug in all levels.");
	Serial.println("*");
	Serial.println(
			"* Please try change debug level in telnet, to see how it works");
	Serial.println("*");

#endif

}

void loop() {

#ifndef PRODUCTION // Not in PRODUCTION
	// Time of begin of this loop
	uint32_t timeBeginLoop = millis();
#endif

	// Each second

	if ((millis() - mLastTime) >= 1000) {

		// Time

		mLastTime = millis();

		mTimeSeconds++;

		// Blink the led

		mLedON = !mLedON;
		digitalWrite(LED_BUILTIN, (mLedON) ? LOW : HIGH);

#ifndef PRODUCTION // Not in PRODUCTION

		// Debug the time (verbose level) (without shortcut)

		debugV("* Time: %u seconds (VERBOSE)", mTimeSeconds);

		if (mTimeSeconds % 5 == 0) { // Each 5 seconds

			// Debug levels

			debugV("* This is a message of debug level VERBOSE");
			debugD("* This is a message of debug level DEBUG");
			debugI("* This is a message of debug level INFO");
			debugW("* This is a message of debug level WARNING");
			debugE("* This is a message of debug level ERROR");

			// Call a function

			foo();

		}
#endif
	}

	////// Services on Wifi

#ifdef USE_ARDUINO_OTA
	// Update over air (OTA)

	ArduinoOTA.handle();
#endif

	//// Web server
	// Discomment if you need this
	//
	// HTTPServer.handleClient();

#ifndef PRODUCTION // Not in PRODUCTION

	// Remote debug over telnet

	Debug.handle();

#endif

	// Give a time for ESP

	yield();

	// Show a debug - warning if time of these loop is over 50 (info) or 100 ms (warning)

#ifndef PRODUCTION // Not in PRODUCTION

	uint32_t time = (millis() - timeBeginLoop);

	if (time > 100) {
		debugI("* Time elapsed for the loop: %u ms.", time);
	} else if (time > 200) {
		debugW("* Time elapsed for the loop: %u ms.", time);
	}
#endif

}


// Function example to show a new auto function name of debug* macros

void foo() {

  uint8_t var = 1;

  debugV("this is a debug - var %u", var);
  debugV("This is a println");
}

#ifndef PRODUCTION // Not in PRODUCTION

// Process commands from RemoteDebug

void processCmdRemoteDebug() {

	String lastCmd = Debug.getLastCommand();

	if (lastCmd == "bench1") {

		// Benchmark 1 - Printf

		debugA("* Benchmark 1 - one Printf");


		uint32_t timeBegin = millis();
		uint8_t times = 50;

		for (uint8_t i = 1; i <= times; i++) {
			debugA("%u - 1234567890 - AAAA", i);

		}

		debugA("* Time elapsed for %u printf: %ld ms.\n", times,
					(millis() - timeBegin));

	} else if (lastCmd == "bench2") {

		// Benchmark 2 - Print/println

		debugA("* Benchmark 2 - Print/Println");

		uint32_t timeBegin = millis();
		uint8_t times = 50;

		for (uint8_t i = 1; i <= times; i++) {
			if (Debug.isActive(Debug.ANY)) {
				Debug.print(i);
				Debug.print(" - 1234567890");
				Debug.println(" - AAAA");
			}
		}

		debugA("* Time elapsed for %u printf: %ld ms.\n", times,
					(millis() - timeBegin));
	}
}
#endif

// Simple benckmark of integers

void benchInt() {

	int test = 0;

	for (int i = 0; i < BENCHMARK_EXECS; i++) {

		// Some integer operations

		test++;
		test += 2;
		test -= 2;
		test *= 2;
		test /= 2;
	}

	// Note: Debug always is used here

	debugA("*** Benchmark of integers. %u exec.", BENCHMARK_EXECS);

}

// Simple benckmark of floats

void benchFloat() {

	float test = 0;

	for (int i = 0; i < BENCHMARK_EXECS; i++) {

		// Some float operations

		test++;
		test += 2;
		test -= 2;
		test *= 2;
		test /= 2;
	}

	// Note: Debug always is used here

	debugA("*** Benchmark of floats, %u exec.", BENCHMARK_EXECS);

}

// Simple benckmark of GPIO

void benchGpio() {

//	const int execs = (BENCHMARK_EXECS / 10); // Reduce it
	const int execs = BENCHMARK_EXECS;

	for (int i = 0; i < execs; i++) {

		// Some GPIO operations

		digitalWrite(LED_BUILTIN, HIGH);
		digitalRead(LED_BUILTIN);
		digitalWrite(LED_BUILTIN, LOW);

		analogRead(A0);
		analogRead(A0);
		analogRead(A0);

	}

	// Note: Debug always is used here

	debugA("*** Benchmark of GPIO. %u exec.", execs);

}

// Run all benchmarks

void benchAll() {

	benchInt();
	benchFloat();
	benchGpio();

	// Note: Debug always is used here

	debugA("*** All Benchmark done.");

}

// Example functions with argument (only 1) to call from serial monitor
// Note others types is not yet available in this version of SerialDebug

void funcArgStr (String str) {

	debugA("*** called with arg.: %s", str.c_str());
}
void funcArgChar (char character) {

	debugA("*** called with arg.: %c", character);
}
void funcArgInt (int number) {

	debugA("*** called with arg.: %d", number);
}

void connectWiFi() {

	////// Connect WiFi

#ifdef EM_DEPURACAO
	Serial.println("*** connectWiFi: begin conection ...");
#endif

#ifdef ESP32
	// ESP32 // TODO: is really necessary ?
	WiFi.enableSTA(true);
	delay(100);
#endif

	// Connect with SSID and password stored

	WiFi.begin();

	// Wait connection

	uint32_t timeout = millis() + 20000; // Time out

	while (WiFi.status() != WL_CONNECTED && millis() < timeout) {
		delay(250);
		Serial.print(".");
	}

	// Not connected yet?

	if (WiFi.status() != WL_CONNECTED) {

		// SmartConfig

		WiFi.beginSmartConfig();

		// Wait for SmartConfig packet from mobile

		Serial.println("connectWiFi: Waiting for SmartConfig.");

		while (!WiFi.smartConfigDone()) {
			delay(500);
			Serial.print(".");
		}

		Serial.println("");
		Serial.println("connectWiFi: SmartConfig received.");

		// Wait for WiFi to connect to AP

		Serial.println("connectWiFi: Waiting for WiFi");

		while (WiFi.status() != WL_CONNECTED) {
			delay(500);
			Serial.print(".");
		}
	}

	// End

	Serial.println("");
	Serial.print("connectWiFi: connect a ");
	Serial.println(WiFi.SSID());
	Serial.print("IP: ");
	Serial.println(WiFi.localIP().toString());

}

#ifdef USE_ARDUINO_OTA

// Initialize o Arduino OTA

void initializeOTA() {

	// TODO: option to authentication (password)

#if defined ESP8266

	ArduinoOTA.onStart([]() {
		Serial.println("* OTA: Start");
	});
	ArduinoOTA.onEnd([]() {
		Serial.println("\n*OTA: End");
	});
	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		Serial.printf("*OTA: Progress: %u%%\r", (progress / (total / 100)));
	});
	ArduinoOTA.onError([](ota_error_t error) {
		Serial.printf("*OTA: Error[%u]: ", error);
		if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
		else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
		else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
		else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
		else if (error == OTA_END_ERROR) Serial.println("End Failed");
	});

#elif defined ESP32

	// ArduinoOTA

	ArduinoOTA.onStart([]() {
		String type;
		if (ArduinoOTA.getCommand() == U_FLASH)
			type = "sketch";
		else // U_SPIFFS
			type = "filesystem";
			Serial.println("Start updating " + type);
		}).onEnd([]() {
		Serial.println("\nEnd");
	}).onProgress([](unsigned int progress, unsigned int total) {
		Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
	}).onError([](ota_error_t error) {
		Serial.printf("Error[%u]: ", error);
		if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
		else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
		else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
		else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
		else if (error == OTA_END_ERROR) Serial.println("End Failed");
	});

#endif

	// Begin

	ArduinoOTA.begin();

}

#endif

/////////// Handles
// Discomment if you need this
//
// void handleRoot() {
//
//     // Root web page
//
//     HTTPServer.send(200, "text/plain", "hello from esp8266 - RemoteDebug Sample!");
// }
//
// void handleNotFound(){
//
//     // Page not Found
//
//     String message = "File Not Found\n\n";
//     message.concat("URI: ");
//     message.concat(HTTPServer.uri());
//     message.concat("\nMethod: ");
//     message.concat((HTTPServer.method() == HTTP_GET)?"GET":"POST");
//     message.concat("\nArguments: ");
//     message.concat(HTTPServer.args());
//     message.concat("\n");
//     for (uint8_t i=0; i<HTTPServer.args(); i++){
//         message.concat(" " + HTTPServer.argName(i) + ": " + HTTPServer.arg(i) + "\n");
//     }
//     HTTPServer.send(404, "text/plain", message);
// }

/////////// End
