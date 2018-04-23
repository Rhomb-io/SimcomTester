/**
 * Rhomb.io Simcom Tester
 *
 * Sketch básico para testear módulos esclavos SIM868.
 * El sketch únicamente comprobará que se puede extraer el IMEI del Simcom
 * y el ICCID de la tarjeta SIM. No se harán pruebas GSM o GPS.
 *
 * Procedimiento:
 * - Utiliza una placa Deimos o cualquier otra con Socket2. Inyecta el módulo
 *   SIMCOM en el Socket 2.
 * - Duino Mega en el master (o cualquier otro duino con más de un UART)
 * - Upload de código desde IDE Arduino
 * - Abre terminal, el output irá indicando el estado del test
 *
 * Al final del test el LED quedará encendido, fijo, si el simcom es OK.
 * Parpadeará en intervalos de 100milis si Hay FALLO.
 * Esto permite validar modulos sin necesidad de consola serial.
 *
 * Version 0.0.1
 * Author Jordi Enguídanos
 * @repository https://github.com/Rhomb-io/SimcomTester
 */

int const GSM_PWREN = 8;
int const GSM_PWRKEY = 9;
int const GPS_EN = 10;
int const GSM_STATUS = 11;
int const LED = 13;

char buffer[150];

int testsOk = 0;

void debug (String message) {
  Serial.println(message);
}

void debug (char * message) {
  Serial.println(message);
}

void simcomPowerKey (void) {
  debug(F(""));
  debug(F("--------------------------------"));
  debug(F("Iniciando TEST 1/3 (MODEM POWERON)"));

  for (char i=0;i<5;i++) {
    Serial.print("...");
    if (digitalRead(GSM_STATUS) == HIGH) {
      debug(F("HIGH"));
      break;
    } else {
      debug(F("LOW"));
      digitalWrite(GSM_PWRKEY, HIGH);
      delay(1500);                       // pulse for Power_ON
      digitalWrite(GSM_PWRKEY, LOW);
    }
    delay (1500);
  }

  if (digitalRead(GSM_STATUS) == HIGH) {
    debug(F("TEST 1/3 OK"));
    testsOk++;
  } else {
    debug(F("TEST 1/3 FAIL"));
  }
}

/**
 * Envía un comando al Serial1 y espera el tiempo indicado en `time`.
 * Almacena el resultado en la variable global `buffer`
 * @param  [String] command
 * @param  [int]    time
 * @return void
 */
void sendAt (String command, int time) {
  int counter = 0;
  strcpy(buffer, "");

  Serial1.println(command); // AT+CGSN
  delay(time);

  while (Serial1.available()) {
    buffer[counter] = Serial1.read();
    counter++;
  }
}

/**
 * Levanta los pines del simcom
 * Levanta comunicación serial
 * Enciende el simcom
 * Ejecuta test 2 y 3: getIMEI() y getICCID()
 */
void setup()
{
  // Inicializar los pins del SIMCOM
  pinMode(GPS_EN, OUTPUT);
  pinMode(GSM_PWREN, OUTPUT);
  pinMode(GSM_PWRKEY, OUTPUT);
  pinMode(GSM_STATUS, INPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(GSM_PWREN, HIGH);
  digitalWrite(GSM_PWRKEY, LOW);
  digitalWrite(GPS_EN, LOW);

  // i(nit serial. en "utils.c" se define "debug"F como alias de Serial.print)nl
  Serial.begin(115200);
  Serial1.begin(115200);
  delay(1000);

  // TEST 1 - habiltiar simcom
  simcomPowerKey();

  // TEST 2 - get imei
  getIMEI();

  // TEST 3 - get iccid
  getICCID();

  if (testsOk == 3) {
    digitalWrite(LED, true);
    debug(F(""));
    debug(F("-------------------------------------------------"));
    debug(F("         ÉXITO - TODOS LOS TEST PASADOS"));
    debug(F("-------------------------------------------------"));
  } else {
    debug(F(""));
    debug(F("-------------------------------------------------"));
    debug(F("           ERROR - ALGO HA IDO MAL..."));
    debug(F("-------------------------------------------------"));
  }
}

void loop (void)
{
  if (testsOk != 3) {
    digitalWrite(LED, !digitalRead(LED));
    delay(200);
  }
}

void getIMEI (void)
{
  debug(F(""));
  debug(F("--------------------------------"));
  debug(F("Iniciando TEST 2/3 (CHECK IMEI)"));

  sendAt("AT+CGSN", 1500);

  // extraer imei
  char * imei;
  imei = strtok(buffer, "\n");
  imei = strtok(NULL, "\n");
  strncpy(imei, imei, 15);

  Serial.print("IMEI:");
  debug(imei);

  if (strlen(imei) == 16) {
    debug(F("TEST 2/3 OK"));
    testsOk++;
  } else {
    debug(F("TEST 2/3 FAIL"));
  }
}


void getICCID (void)
{
  debug(F(""));
  debug(F("--------------------------------"));
  debug(F("Iniciando TEST 3/3 (CHECK ICCID)"));

  sendAt("AT+ICCID", 1500);

  // extraer iccid
  char * iccid;
  iccid = strtok(buffer, "\n");
  iccid = strtok(NULL, " ");
  iccid = strtok(NULL, "\n");

  Serial.print("ICCID:");
  Serial.println(iccid);
  Serial.println(strlen(iccid));

  if (strlen(iccid) == 21) {
    debug(F("TEST 3/3 OK"));
    testsOk++;
  } else {
    debug(F("TEST 3/3 FAIL"));
  }
}
