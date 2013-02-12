#include "gps_logger.h"
#include <HardwareSerial.h>
#include <SD.h>

extern HardwareSerial Serial;

const char* GPS_DIR = "GPS";

void setup() {
	 Serial.begin(9600);
	 pinMode(10, OUTPUT);

	 if (!SD.begin()) {
		 Serial.println("SD Error!");
		 return;
	 }

	 File gps_dir = SD.open(GPS_DIR);
	 if (!gps_dir.available()) {
		 SD.mkdir(const_cast<char*>(GPS_DIR));
	 }
	 gps_dir.close();
}

File open_gps_file() {
	char path[strlen(GPS_DIR) + 14];
	strcat(path, GPS_DIR);
	strcat(path, "/");
	strcat(path, "02112218.NMA");

	return SD.open(path, FILE_WRITE);
}

void loop() {
	File gps_log = open_gps_file();

	String text;
	text.concat("$GPRMC");
	text.concat(",");
	text.concat("092750.000");
	text.concat(",");
	text.concat("A");
	text.concat("5321.6802");
	text.concat(",");
	text.concat("N");
	text.concat(",");
	text.concat("00630.3372");
	text.concat(",");
	text.concat("W");
	text.concat(",");
	text.concat("0.02");
	text.concat(",");
	text.concat("31.66");
	text.concat(",");
	text.concat("280511");
	text.concat(",,,");
	text.concat("A*43");

	gps_log.println(text);
	gps_log.close();

	delay(1000);
}
