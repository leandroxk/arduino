#include "gps_logger.h"
#include <HardwareSerial.h>
#include <SD.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>

extern HardwareSerial Serial;

const char* GPS_DIR = "/GPS";
const unsigned int MAX_HDOP = 200;
const byte GREEN_LED = 6;
const byte RED_LED = 7;
const byte RX = 4;
const byte TX = 3;

long line_number;
File gps_filename;
TinyGPS gps;
SoftwareSerial nss(TX, RX);

void setup() {
	Serial.begin(115200);
	nss.begin(9600);
	pinMode(10, OUTPUT);
	pinMode(GREEN_LED, OUTPUT);
	pinMode(RED_LED, OUTPUT);

	Serial.println("Iniciando...");
	if (!SD.begin())
		Serial.println("SD Erro!");
}


File create_file(const char* path) {
	if (char* file = strrchr(path, '/')) {
		int dir_size = strlen(path) - strlen(file);
		char dir_path[dir_size + 1];
		strncpy(dir_path, path, dir_size);
		dir_path[dir_size] = '\0';

		File dir = SD.open(dir_path);
		if (!dir.available())
			SD.mkdir(const_cast<char*>(dir_path));
	}

	return SD.open(path, FILE_WRITE);
}

File create_gps_file() {
	int year;
	unsigned long age;
	byte month, day, hour, minute, second, hundredth;
	gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredth, &age);

	String filename;
	filename.concat(GPS_DIR);
	filename.concat('/');
	filename.concat(year);
	filename.concat('/');
	filename.concat(month);
	filename.concat('/');
	filename.concat(day);
	filename.concat('/');
	filename.concat(hour);
	filename.concat(minute);
	filename.concat(second);
	filename.concat(".GPS");

	char buf[filename.length() + 1];
	filename.toCharArray(buf, sizeof(buf));

	Serial.print("Criando arquivo... ");
	Serial.println(buf);
	return create_file(buf);
}

String create_gps_line() {
	float lat, lon;
	unsigned long age;
	gps.f_get_position(&lat, &lon, &age);

	short satellites = gps.satellites();

	String line;
	line.concat(line_number++);
	line.concat(',');

	char buf[12];
	line.concat(dtostrf(lat, 4, 6, buf));
	line.concat(',');

	line.concat(dtostrf(lon, 4, 6, buf));
	line.concat(',');

	int year;
	byte month, day, hour, minute, second, hundredth;
	gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredth, &age);

	line.concat(day);
	line.concat('/');
	line.concat(month);
	line.concat('/');
	line.concat(year);
	line.concat(" ");
	line.concat(hour);
	line.concat(":");
	line.concat(minute);
	line.concat(":");
	line.concat(second);
	line.concat(',');
	line.concat(satellites);
	line.concat(',');
	line.concat(dtostrf(gps.f_altitude(), 4, 2, buf));
	line.concat(',');
	line.concat(gps.hdop());
	line.concat(',');
	line.concat(age);

	return line;
}

void signal_fixed_status() {
	pinMode(RED_LED, LOW);
	pinMode(GREEN_LED, HIGH);
}

void signal_unavailable_status() {
	pinMode(GREEN_LED, LOW);
	pinMode(RED_LED, HIGH);
}

void signal_instable_status() {
	pinMode(RED_LED, LOW);
	pinMode(GREEN_LED, HIGH);
	delay(100);
	pinMode(GREEN_LED, LOW);
}

void loop() {
	bool newData = false;
	unsigned long start = millis();
	while (millis() - start < 1000) {
		while (nss.available()) {
			if (gps.encode(nss.read()))
				newData = true;
		}
	}

	if (newData) {
		if (gps.hdop() < MAX_HDOP)
			signal_fixed_status();
		else
			signal_instable_status();

		if (!gps_filename) {
			gps_filename = create_gps_file();
		}

		String line = create_gps_line();
		gps_filename.println(line);
		gps_filename.flush();

		Serial.println(line);
	} else {
		signal_unavailable_status();
	}
}

