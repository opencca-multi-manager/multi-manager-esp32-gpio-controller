#include <Arduino.h>

#define INPUT_BUF_SIZE 64

static char inputBuf[INPUT_BUF_SIZE];
static int inputPos = 0;
static bool inputOverflow = false;

#define MAX_PIN 39

static bool pinConfigured[MAX_PIN + 1];
static const int validPins[] = {2, 4, 5, 12, 13, 14, 15, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33, 34, 35, 36, 39};
static const int numValid = (int)(sizeof(validPins) / sizeof(validPins[0]));

bool validatePin(int pin) {
  for (int i = 0; i < numValid; i++) {
    if (validPins[i] == pin) return true;
  }
  Serial.printf("ERR: pin %d is not available\n", pin);
  return false;
}

void handleSet(int pin, bool high) {
  if (!validatePin(pin)) return;
  if (pin >= 34) {
    Serial.printf("ERR: pin %d is input-only\n", pin);
    return;
  }
  pinMode(pin, OUTPUT);
  digitalWrite(pin, high ? HIGH : LOW);
  pinConfigured[pin] = true;
  Serial.println("OK");
}

void handleGet(int pin) {
  if (!validatePin(pin)) return;
  if (!pinConfigured[pin]) {
    Serial.printf("WARN: pin %d was not previously set, you are reading from input\n", pin);
    pinMode(pin, INPUT);
    pinConfigured[pin] = true;
  }
  int val = digitalRead(pin);
  Serial.printf("PIN %d: %s\n", pin, val == HIGH ? "HIGH" : "LOW");
}

char *trim(char *str) {
  while (*str == ' ' || *str == '\t' || *str == '\r') str++;
  size_t len = strlen(str);
  if (len == 0) return str;
  char *end = str + len - 1;
  while (end > str && (*end == ' ' || *end == '\t' || *end == '\r'))
    *end-- = '\0';
  return str;
}

int parsePin(char *str, char **endp) {
  long pin = strtol(str, endp, 10);
  if (*endp == str) {
    Serial.println("ERR: missing pin number");
    return -1;
  }
  return (int)pin;
}

void processCommand(char *raw) {
  char *line = trim(raw);
  for (char *p = line; *p; p++) *p = toupper(*p);

  if (*line == '\0') return;

  if (strncmp(line, "SET ", 4) == 0) {
    char *endp;
    int pin = parsePin(line + 4, &endp);
    if (pin < 0) return;
    char *val = trim(endp);
    if (strcmp(val, "HIGH") == 0) {
      handleSet(pin, true);
    } else if (strcmp(val, "LOW") == 0) {
      handleSet(pin, false);
    } else {
      Serial.println("ERR: expected HIGH or LOW");
    }
  } else if (strncmp(line, "GET ", 4) == 0) {
    char *endp;
    int pin = parsePin(line + 4, &endp);
    if (pin < 0) return;
    handleGet(pin);
  } else {
    Serial.println("ERR: unknown command");
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("ESP32 GPIO Controller ready");
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      if (inputOverflow) {
        Serial.println("ERR: input too long");
        inputOverflow = false;
      } else {
        inputBuf[inputPos] = '\0';
        processCommand(inputBuf);
      }
      inputPos = 0;
    } else if (inputPos < INPUT_BUF_SIZE - 1) {
      inputBuf[inputPos++] = c;
    } else {
      inputOverflow = true;
    }
  }
}
