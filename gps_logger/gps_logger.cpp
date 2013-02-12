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

int nmea_checksum(char *s) {
	int c = 0;
	while (*s) {
		c ^= *s++;
	}

	return c;
}

String build_nmea_string() {
	char text[90];
	strcpy(text, "$GPRMC");
	strcat(text, ",");
	strcat(text, "092750.000");
	strcat(text, ",");
	strcat(text, "A");
	strcat(text, "5321.6801");
	strcat(text, ",");
	strcat(text, "N");
	strcat(text, ",");
	strcat(text, "00630.3372");
	strcat(text, ",");
	strcat(text, "W");
	strcat(text, ",");
	strcat(text, "0.02");
	strcat(text, ",");
	strcat(text, "31.66");
	strcat(text, ",");
	strcat(text, "280511");
	strcat(text, ",,,");
	strcat(text, "A*");

	String mnea = String(text);
	mnea.concat(String(nmea_checksum(text)));

	return mnea;
}


void loop() {
	File gps_log = SD.open("teste", FILE_WRITE);

	gps_log.println(build_nmea_string());
	gps_log.close();

	delay(1000);
}
