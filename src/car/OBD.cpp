/*************************************************************************
* Arduino Library for OBD-II UART/I2C Adapter
* Distributed under BSD License
* Visit http://freematics.com for more information
* (C)2012-2016 Stanley Huang <stanleyhuangyc@gmail.com>
*************************************************************************/

#include "OBD.h"
#include "System.h"
#include "Log.h"
#include <cstdio>
#include <cstring>
#include <ctype.h>
#include "LEDs.h"
#include "Color.h"

// ESP32 UART implementation
ESP32UART OBDUART;

void ESP32UART::begin(unsigned long baud_rate, uint32_t config, int8_t rxPin, int8_t txPin) {
  // Clean up any existing driver before reinstalling
//    if (initialized) {
//        end();
//    }

  uart_config_t uart_config = {
    .baud_rate = (int) baud_rate,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .rx_flow_ctrl_thresh = 122,
    .source_clk = UART_SCLK_DEFAULT,
    .flags = {}
  };

  // Configure UART parameters
  uart_param_config(uart_num, &uart_config);
  uart_set_pin(uart_num, txPin, rxPin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

  // Install UART driver (with ring buffer)
  uart_driver_install(uart_num, 1024, 1024, 0, NULL, 0);

  debugLed[0] = rgb::Color::YELLOW();

  initialized = true;
  INFO("UART Initialized");
}

void ESP32UART::end() {
    if (initialized) {
        uart_driver_delete(uart_num);
        initialized = false;
    }
}

size_t ESP32UART::write(const char* str) {
    if (!initialized) return 0;
    auto len = strlen(str);
    return uart_write_bytes(uart_num, str, len);
}

size_t ESP32UART::write(char c) {
    if (!initialized) return 0;
    return uart_write_bytes(uart_num, &c, 1);
}

size_t ESP32UART::print(const char* str) {
    return write(str);
}

size_t ESP32UART::print(unsigned long num) {
    char buffer[32];
    sprintf(buffer, "%lu", num);
    return write(buffer);
}

size_t ESP32UART::print(char c) {
    return write(c);
}

int ESP32UART::available() {
    if (!initialized) return 0;
    size_t available_bytes = 0;
    uart_get_buffered_data_len(uart_num, &available_bytes);
    return (int)available_bytes;
}

int ESP32UART::read() {
    if (!initialized) return -1;
    uint8_t data;
    int len = uart_read_bytes(uart_num, &data, 1, pdMS_TO_TICKS(0));
    return (len > 0) ? data : -1;
}

//#define DEBUG Serial

/*************************************************************************
* OBD-II UART Adapter
*************************************************************************/

byte COBD::sendCommand(const char* cmd, char* buf, byte bufsize, int timeout)
{
	write(cmd);
  dataIdleLoop();
	return receive(buf, bufsize, timeout);
}

void COBD::sendQuery(byte pid)
{
	char cmd[8];
	sprintf(cmd, "%02X%02X\r", dataMode, pid);
#ifdef DEBUG
	debugOutput(cmd);
#endif
	write(cmd);
}

bool COBD::readPID(byte pid, int& result, int timeout)
{
	// send a query command
  sendQuery(pid);
	// receive and parse the response
	return getResult(pid, result, timeout);
}

byte COBD::readPID(const byte pid[], byte count, int result[], int timeout)
{
	byte results = 0;
	for (byte n = 0; n < count; n++) {
		if (readPID(pid[n], result[n], timeout)) {
			results++;
		}
	}
	return results;
}

byte COBD::readDTC(uint16_t codes[], byte maxCodes)
{
	/*
	Response example:
	0: 43 04 01 08 01 09
	1: 01 11 01 15 00 00 00
	*/
	byte codesRead = 0;
 	for (byte n = 0; n < 6; n++) {
		char buffer[128];
		sprintf(buffer, n == 0 ? "03\r" : "03%02X\r", n);
		write(buffer);
		if (receive(buffer, sizeof(buffer)) > 0) {
			if (!strstr(buffer, "NO DATA")) {
				char *p = strstr(buffer, "43");
				if (p) {
					while (codesRead < maxCodes && *p) {
						p += 6;
						if (*p == '\r') {
							p = strchr(p, ':');
							if (!p) break;
							p += 2;
						}
						uint16_t code = hex2uint16(p);
						if (code == 0) break;
						codes[codesRead++] = code;
					}
				}
				break;
			}
		}
	}
	return codesRead;
}

void COBD::clearDTC()
{
	char buffer[32];
	write("04\r");
	receive(buffer, sizeof(buffer));
}

void COBD::write(const char* s)
{
#ifdef DEBUG
	DEBUG.print("<<<");
	DEBUG.println(s);
#endif
	OBDUART.write(s);
}

int COBD::normalizeData(byte pid, char* data)
{
	int result;
	switch (pid) {
	case PID_RPM:
	case PID_EVAP_SYS_VAPOR_PRESSURE: // kPa
		result = getLargeValue(data) >> 2;
		break;
	case PID_FUEL_PRESSURE: // kPa
		result = getSmallValue(data) * 3;
		break;
	case PID_COOLANT_TEMP:
	case PID_INTAKE_TEMP:
	case PID_AMBIENT_TEMP:
	case PID_ENGINE_OIL_TEMP:
		result = getTemperatureValue(data);
		break;
	case PID_THROTTLE:
	case PID_COMMANDED_EGR:
	case PID_COMMANDED_EVAPORATIVE_PURGE:
	case PID_FUEL_LEVEL:
	case PID_RELATIVE_THROTTLE_POS:
	case PID_ABSOLUTE_THROTTLE_POS_B:
	case PID_ABSOLUTE_THROTTLE_POS_C:
	case PID_ACC_PEDAL_POS_D:
	case PID_ACC_PEDAL_POS_E:
	case PID_ACC_PEDAL_POS_F:
	case PID_COMMANDED_THROTTLE_ACTUATOR:
	case PID_ENGINE_LOAD:
	case PID_ABSOLUTE_ENGINE_LOAD:
	case PID_ETHANOL_FUEL:
	case PID_HYBRID_BATTERY_PERCENTAGE:
		result = getPercentageValue(data);
		break;
	case PID_MAF_FLOW: // grams/sec
		result = getLargeValue(data) / 100;
		break;
	case PID_TIMING_ADVANCE:
		result = (int)(getSmallValue(data) / 2) - 64;
		break;
	case PID_DISTANCE: // km
	case PID_DISTANCE_WITH_MIL: // km
	case PID_TIME_WITH_MIL: // minute
	case PID_TIME_SINCE_CODES_CLEARED: // minute
	case PID_RUNTIME: // second
	case PID_FUEL_RAIL_PRESSURE: // kPa
	case PID_ENGINE_REF_TORQUE: // Nm
		result = getLargeValue(data);
		break;
	case PID_CONTROL_MODULE_VOLTAGE: // V
		result = getLargeValue(data) / 1000;
		break;
	case PID_ENGINE_FUEL_RATE: // L/h
		result = getLargeValue(data) / 20;
		break;
	case PID_ENGINE_TORQUE_DEMANDED: // %
	case PID_ENGINE_TORQUE_PERCENTAGE: // %
		result = (int)getSmallValue(data) - 125;
		break;
	case PID_SHORT_TERM_FUEL_TRIM_1:
	case PID_LONG_TERM_FUEL_TRIM_1:
	case PID_SHORT_TERM_FUEL_TRIM_2:
	case PID_LONG_TERM_FUEL_TRIM_2:
	case PID_EGR_ERROR:
		result = ((int)getSmallValue(data) - 128) * 100 / 128;
		break;
	case PID_FUEL_INJECTION_TIMING:
		result = ((int32_t)getLargeValue(data) - 26880) / 128;
		break;
	case PID_CATALYST_TEMP_B1S1:
	case PID_CATALYST_TEMP_B2S1:
	case PID_CATALYST_TEMP_B1S2:
	case PID_CATALYST_TEMP_B2S2:
		result = getLargeValue(data) / 10 - 40;
		break;
	case PID_AIR_FUEL_EQUIV_RATIO: // 0~200
		result = (long)getLargeValue(data) * 200 / 65536;
		break;
	default:
		result = getSmallValue(data);
	}
	return result;
}

char* COBD::getResponse(byte& pid, char* buffer, byte bufsize, int timeoutMs)
{
	while (receive(buffer, bufsize, timeoutMs) > 0) {
    char *p = buffer;
		while ((p = strstr(p, "41 "))) {
		    p += 3;
		    byte curpid = hex2uint8(p);
		    if (pid == 0) pid = curpid;
		    if (curpid == pid) {
		        errors = 0;
		        p += 2;
		        if (*p == ' ')
		            return p + 1;
		    }
		}
	}
	return 0;
}

bool COBD::getResult(byte& pid, int& result, int timeoutMs)
{
	char buffer[64];
  char* data = getResponse(pid, buffer, sizeof(buffer), timeoutMs);
	if (!data) {
		recover();
		errors++;
		return false;
	}
	result = normalizeData(pid, data);
	return true;
}

void COBD::enterLowPowerMode()
{
  	char buf[32];
	sendCommand("ATLP\r", buf, sizeof(buf));
}

void COBD::leaveLowPowerMode()
{
	// simply send any command to wake the device up
	char buf[32];
	sendCommand("ATI\r", buf, sizeof(buf), 1000);
}

char* COBD::getResultValue(char* buf)
{
	char* p = buf;
	for (;;) {
		if (isdigit(*p) || *p == '-') {
			return p;
		}
		p = strchr(p, '\r');
		if (!p) break;
		if (*(++p) == '\n') p++;
	}
	return 0;
}

float COBD::getVoltage()
{
    char buf[32];
	if (sendCommand("ATRV\r", buf, sizeof(buf)) > 0) {
		char* p = getResultValue(buf);
		if (p) return (float)atof(p);
    }
    return 0;
}

bool COBD::getVIN(char* buffer, byte bufsize)
{
	for (byte n = 0; n < 5; n++) {
		if (sendCommand("0902\r", buffer, bufsize)) {
			int len = hex2uint16(buffer);
			char *p = strstr(buffer + 4, "0: 49 02 01");
			if (p) {
				char *q = buffer;
				p += 11; // skip the header
				do {
					while (*(++p) == ' ');
					for (;;) {
						*(q++) = hex2uint8(p);
						while (*p && *p != ' ') p++;
						while (*p == ' ') p++;
						if (!*p || *p == '\r') break;
					}
					p = strchr(p, ':');
				} while(p);
				*q = 0;
				if (q - buffer == len - 3) {
					return true;
				}
			}
		}
		rgb::System::MilliSleep(100);
	}
    return false;
}

bool COBD::isValidPID(byte pid)
{
	if (pid >= 0x7f)
		return true;
	pid--;
	byte i = pid >> 3;
	byte b = 0x80 >> (pid & 0x7);
	return (pidmap[i] & b) != 0;
}

byte COBD::begin(int8_t rxPin, int8_t txPin)
{
	long baudrates[] = {115200, 38400};
	byte version = 0;
	for (byte n = 0; n < sizeof(baudrates) / sizeof(baudrates[0]) && version == 0; n++) {
    INFO("Beginning UART");
    OBDUART.begin(baudrates[n], UART_PARITY_DISABLE, rxPin, txPin);
    version = getVersion();
    INFO("UART Version %i", version);
    if (version != 0) {
      if (n == 0) {
        debugLed[0] = rgb::Color::GREEN();
      }
      else {
        debugLed[0] = rgb::Color::BLUE();
      }
    }
	}
	return version;
}

byte COBD::getVersion()
{
	byte version = 0;
	for (byte n = 0; n < 3; n++) {
		char buffer[32];
		if (sendCommand("ATI\r", buffer, sizeof(buffer), 20)) {
			char *p = strchr(buffer, ' ');
			if (p) {
				p += 2;
				version = (*p - '0') * 10 + (*(p + 2) - '0');
				break;
			}
		}
	}
	return version;
}

byte COBD::receive(char* buffer, byte bufsize, int timeout)
{
	unsigned char n = 0;
	unsigned long startTime = rgb::System::MilliTime();
	char c = 0;
	for (;;) {
    if (OBDUART.available()) {
      c = OBDUART.read();
			if (!buffer) {
			       n++;
			} else if (n < bufsize - 1) {
				if (c == '.' && n > 2 && buffer[n - 1] == '.' && buffer[n - 2] == '.') {
					// waiting siginal
					n = 0;
					timeout = OBD_TIMEOUT_LONG;
				} else {
					if (c == '\r' || c == '\n' || c == ' ') {
						if (n == 0 || buffer[n - 1] == '\r' || buffer[n - 1] == '\n') continue;
					}
					buffer[n++] = c;
				}
			}
		} else {
      if (c == '>') {
				// prompt char received
				break;
			}
			if ((int)(rgb::System::MilliTime() - startTime) > timeout) {
        // timeout
			    break;
			}
      dataIdleLoop();
		}
	}
	if (buffer) {
		buffer[n] = 0;
	}
#ifdef DEBUG
	DEBUG.print(">>>");
	DEBUG.println(buffer);
#endif
	return n;
}

void COBD::recover()
{
	sendCommand("\r", 0, 0, 20);
}

bool COBD::init(OBD_PROTOCOLS protocol)
{
	const char *initcmd[] = {"ATZ\r", "ATE0\r", "ATH0\r"};
	char buffer[64];

	m_state = OBD_DISCONNECTED;
	for (unsigned char i = 0; i < sizeof(initcmd) / sizeof(initcmd[0]); i++) {
		write(initcmd[i]);
		if (receive(buffer, sizeof(buffer), OBD_TIMEOUT_LONG) == 0) {
			return false;
		}
	}
	if (protocol != PROTO_AUTO) {
		sprintf(buffer, "ATSP %u\r", protocol);
		write(buffer);
		if (receive(buffer, sizeof(buffer), OBD_TIMEOUT_LONG) == 0 && !strstr(buffer, "OK")) {
			return false;
		}
	}

	// load pid map
	memset(pidmap, 0, sizeof(pidmap));
	bool success = false;
	for (byte i = 0; i < 4; i++) {
		byte pid = i * 0x20;
		sendQuery(pid);
		if (receive(buffer, sizeof(buffer), OBD_TIMEOUT_LONG) > 0) {
			char *p = buffer;
			while ((p = strstr(p, "41 "))) {
				p += 3;
				if (hex2uint8(p) == pid) {
					p += 2;
					for (byte n = 0; n < 4 && *(p + n * 3) == ' '; n++) {
						pidmap[i * 4 + n] = hex2uint8(p + n * 3 + 1);
					}
					success = true;
				}
			}
		}
	}

	if (success) {
		m_state = OBD_CONNECTED;
		errors = 0;
	}
	return success;
}

void COBD::end()
{
	m_state = OBD_DISCONNECTED;
	OBDUART.end();
}

bool COBD::setBaudRate(unsigned long baudrate)
{
    OBDUART.print("ATBR1 ");
    OBDUART.print(baudrate);
    OBDUART.print('\r');
    rgb::System::MilliSleep(50);
    OBDUART.end();
    OBDUART.begin(baudrate, UART_PARITY_DISABLE, -1, -1);
    recover();
    return true;
}

bool COBD::memsInit()
{
	char buf[16];
	return sendCommand("ATTEMP\r", buf, sizeof(buf)) > 0 && !strchr(buf, '?');
}

bool COBD::memsRead(int16_t* acc, int16_t* gyr, int16_t* mag, int16_t* temp)
{
	char buf[64];
	bool success;
	if (acc) {
		success = false;
		if (sendCommand("ATACL\r", buf, sizeof(buf)) > 0) do {
			char* p = getResultValue(buf);
			if (!p) break;
			acc[0] = atoi(p++);
			if (!(p = strchr(p, ','))) break;
			acc[1] = atoi(++p);
			if (!(p = strchr(p, ','))) break;
			acc[2] = atoi(++p);
			success = true;
		} while (0);
		if (!success) return false;
	}
	if (gyr) {
		success = false;
		if (sendCommand("ATGYRO\r", buf, sizeof(buf)) > 0) do {
			char* p = getResultValue(buf);
			if (!p) break;
			gyr[0] = atoi(p++);
			if (!(p = strchr(p, ','))) break;
			gyr[1] = atoi(++p);
			if (!(p = strchr(p, ','))) break;
			gyr[2] = atoi(++p);
			success = true;
		} while (0);
		if (!success) return false;
	}
	if (temp) {
		success = false;
		if (sendCommand("ATTEMP\r", buf, sizeof(buf)) > 0) {
			char* p = getResultValue(buf);
			if (p) {
				*temp = (atoi(p) + 12412) / 34;
				success = true;
			}
		}
		if (!success) return false;
	}
	return true;
}

#ifdef DEBUG
void COBD::debugOutput(const char *s)
{
	DEBUG.print('[');
	DEBUG.print(millis());
	DEBUG.print(']');
	DEBUG.print(s);
}
#endif