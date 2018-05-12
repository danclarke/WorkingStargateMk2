#include <WiFi.h>
#include "StargateLogic.h"
#include "WebServer.h"
#include "config.h"
#include "webFiles.h"

// IP Configuration for network
IPAddress ip(192, 168, 0, 34);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(8, 8, 8, 8);

// Number of loops to wait for the WiFi connection prior to just restarting the connection request
#define MAX_WAIT_LOOPS	10

WebServerClass WebServer;

const char *VERB_POST = "POST";
const char *VERB_GET = "GET";
const char *VERB_OPTIONS = "OPTIONS";

const char *HEADER_CONTENTLENGTH = "Content-length";
const char *HTML_HEADER = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\nConnection: close\r\n";
const char *CSS_HEADER = "HTTP/1.1 200 OK\r\nContent-type: text/css\r\nConnection: close\r\n";
const char *SVG_HEADER = "HTTP/1.1 200 OK\r\nContent-type: image/svg+xml\r\nConnection: close\r\n";
const char *WOFF_HEADER = "HTTP/1.1 200 OK\r\nContent-type: font/woff\r\nConnection: close\r\n";
const char *NOTFOUND_RESPONSE = "HTTP/1.1 404 File Not Found\r\nContent-length: 0\r\nConnection: close\r\n\r\n";
const char *OPTIONS_RESPONSE = "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nAccess-Control-Allow-Methods: POST, GET, OPTIONS\r\nAccess-Control-Allow-Headers: Content-Type\r\nAccess-Control-Max-Age: 86400\r\nContent-length: 0\r\nConnection: close\r\nContent-Type: text/plain\r\n\r\n";
const char *OK_RESPONSE = "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nConnection: close\r\nContent-length: 2\r\n\r\nOK";

static void webServerTask(void *param)
{
	while (true)
	{
		WebServer.loop();
		delay(500); // Add a bit of a delay to free up CPU cycles
	}
}

WebServerClass::WebServerClass()
	: _server(80)
{
}

void WebServerClass::init()
{
	WiFi.begin(WIFI_SSID, WIFI_PASS);
	WiFi.config(ip, gateway, subnet, dns);

	// Create task that will handle the web server loop
	xTaskCreatePinnedToCore(webServerTask, "WebServer", 4096, NULL, 5, NULL, 0);
}

void WebServerClass::loop()
{
	ensureConnected();
	
	while (true)
	{
		WiFiClient client = _server.available();
		if (!client)
			return;

		handleHttpRequest(client);
	}
}

void WebServerClass::ensureConnected()
{
	if (WiFi.status() == WL_CONNECTED)
	{
		if (!_serverActive)
		{
			_server.begin();
			_serverActive = true;
		}

		return;
	}

	while (true)
	{
		uint8_t waitLoops = 0;
		while (WiFi.status() != WL_CONNECTED)
		{
			delay(1000);
			Serial.println("Waiting on WiFi connection");

			if (++waitLoops > MAX_WAIT_LOOPS)
				break;
		}

		if (WiFi.status() == WL_CONNECTED)
		{
			Serial.println("WiFi connected");
			_server.begin();
			_serverActive = true;
			return;
		}

		Serial.println("Attempting reconnection");
		_serverActive = false;
		_server.end();
		WiFi.begin(WIFI_SSID, WIFI_PASS);
		delay(1000);
	}
}

void WebServerClass::handleHttpRequest(WiFiClient client)
{
	size_t len = client.readBytes(_httpBuffer, 2048);
	if (len == 0)
	{
		client.stop();
		return;
	}

	// Read in request info
	size_t methodLen, pathLen;
	const char *method = NULL;
	const char *path = NULL;
	int minorVersion;
	struct phr_header headers[20];
	size_t numHeaders = sizeof(headers) / sizeof(headers[0]);
	int res = phr_parse_request(_httpBuffer, len, &method, &methodLen, &path, &pathLen, &minorVersion, headers, &numHeaders, 0);

	// Validate data sent
	if (res < 0 || pathLen <= 0 || methodLen <= 0)
	{
		client.stop();
		return;
	}

	// Return requested data
	if (isStrMatch(VERB_OPTIONS, method, methodLen))
		client.write(OPTIONS_RESPONSE);
	else if (isStrMatch(VERB_GET, method, methodLen))
		handleGetRequest(client, path, pathLen);
	else if (isStrMatch(VERB_POST, method, methodLen))
		handlePostRequest(client, headers, numHeaders);

	client.stop();
}

void WebServerClass::handleGetRequest(WiFiClient client, const char* path, size_t pathLen)
{
	// Very dirty matching here

	if (isStrMatch("/", path, pathLen) || isStrMatch("/index.htm", path, pathLen))
		returnFile(client, index_htm, index_htm_size, HTML_HEADER);
	if (isStrMatch("/main.css", path, pathLen))
		returnFile(client, main_css, main_css_size, CSS_HEADER);
	else if (isStrMatch("/fonts/chevrons.woff", path, pathLen))
		returnFile(client, fonts_chevrons_woff, fonts_chevrons_woff_size, WOFF_HEADER);

	// 404
	client.write(NOTFOUND_RESPONSE);
}

void WebServerClass::handlePostRequest(WiFiClient client, const struct phr_header *headers, size_t numHeaders)
{
	size_t contentLength = 0;

	// Find content length header
	for (uint8_t i = 0; i < numHeaders; ++i)
	{
		if (isStrMatch(HEADER_CONTENTLENGTH, headers[i].name, headers[i].name_len))
		{
			char lengthBuf[8];
			size_t len = headers[i].value_len < 8 ? headers[i].value_len : 7;
			memcpy(lengthBuf, headers[i].value, len);
			lengthBuf[len] = 0;
			contentLength = atoi(lengthBuf);
			break;
		}
	}

	if (contentLength == 0)
	{
		client.write(NOTFOUND_RESPONSE);
		return;
	}

	for (uint16_t i = 0; i < sizeof(_httpBuffer) - 4; ++i)
	{
		if (_httpBuffer[i] == '\r' && _httpBuffer[i+1] == '\n' && _httpBuffer[i+2] == '\r' && _httpBuffer[i+3] == '\n')
		{
			const char *jsonBuffer = &_httpBuffer[i+4];
			StargateLogic.processRequest(jsonBuffer, contentLength);
			client.write(OK_RESPONSE);
			return;
		}
	}

	// 404
	client.write(NOTFOUND_RESPONSE);
}

void WebServerClass::returnFile(WiFiClient client, const unsigned char *file, long int len, const char* initialHeader)
{
	char header[1024];
	size_t headerLen = strlen(initialHeader);
	if (headerLen > 1000)
		return;

	// Populate HTTP header
	memcpy(header, initialHeader, headerLen);
	headerLen += snprintf(&header[headerLen], sizeof(header) - headerLen, "Content-length: %i\r\n\r\n", len);
	header[headerLen] = 0;

	// Send header down the pipe
	client.write(header, headerLen);
	
	// Send file down the pipe, thanks to glorious memory mapping this is a single line
	client.write(file, len);
}

bool WebServerClass::isStrMatch(const char *str, const char *buf, size_t bufLen)
{
	size_t len = strlen(str);
	if (len != bufLen)
	{
		if (buf[bufLen - 1] != 0 || bufLen - 1 != len)
			return false;
	}

	for (size_t i = 0; i < bufLen; ++i)
	{
		if ((buf[i] | 32) != (str[i] | 32)) // Sorcery https://stackoverflow.com/a/35390639
			return false;
	}

	return true;
}
