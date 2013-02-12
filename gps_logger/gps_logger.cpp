#include "gps_logger.h"
#include <HardwareSerial.h>
#include <SD.h>

extern HardwareSerial Serial;

const char* GPS_DIR = "/GPS";

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

void build_nmea_string(char* text) {
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

	int checksum = nmea_checksum(text);
	char buf[sizeof(checksum) + 1];
	itoa(checksum, buf, 10);
	strcat(text, buf);
}

File get_log_file() {
	char path[strlen(GPS_DIR) + 14];
	strcpy(path, GPS_DIR);
	strcat(path, "/");
	strcat(path, "02121118.NMA");

	return SD.open(path, FILE_WRITE);
}

void loop() {
	File gps_log = get_log_file();

	char mnea[90];
	build_nmea_string(mnea);

	gps_log.println(mnea);
	gps_log.close();
	Serial.println(mnea);

	delay(1000);
}
