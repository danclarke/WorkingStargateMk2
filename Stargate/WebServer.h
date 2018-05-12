// WebServer.h

#ifndef _WEBSERVER_h
#define _WEBSERVER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <WiFi.h>
#include "picohttpparser.h"

class WebServerClass
{
public:
	WebServerClass();
	void init();
	void loop();

private:
	WiFiServer _server;
	bool _serverActive = false;
	char _httpBuffer[2048];

	void ensureConnected();
	void handleHttpRequest(WiFiClient client);
	void handleGetRequest(WiFiClient client, const char* path, size_t pathLen);
	void handlePostRequest(WiFiClient client, const struct phr_header *headers, size_t numHeaders);
	void returnFile(WiFiClient client, const unsigned char *file, long int len, const char* initialHeader);

	bool isStrMatch(const char *str, const char *buf, size_t bufLen);
};

extern WebServerClass WebServer;

#endif

