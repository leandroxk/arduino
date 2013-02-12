#include "gps_logger.h"
#include <HardwareSerial.h>
#include <SD.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>

extern HardwareSerial Serial;

const char* GPS_DIR = "/GPS";

TinyGPS gps;

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

File get_log_file() {
	char path[strlen(GPS_DIR) + 14];
	strcpy(path, GPS_DIR);
	strcat(path, "/");
	strcat(path, "02121118.NMA");

	return SD.open(path, FILE_WRITE);
}

void loop() {
	File gps_log = get_log_file();

	const char* mnea = "$GPRMC,092750.000,A,5321.6802,N,00630.3372,W,0.02,31.66,280511,,,A*43";
	gps_log.println(mnea);
	gps_log.close();


	delay(1000);
}
