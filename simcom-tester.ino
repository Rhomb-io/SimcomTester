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
 * Version 0.0.1
 * Author Jordi Enguídanos
 * @repository https://github.com/M2MSystemSource/SimcomTester
 */

int GSM_PWREN = 8;
int GSM_PWRKEY = 9;
int GPS_EN = 10;
int GSM_STATUS = 11;

char buffer[150];

int testsOk = 0;

void debug (String message) {
  Serial.println(message);
}

void debug (char * message) {
  Serial.println(message);
}

void simcomPowerKey (void) {
  Serial.println("");
  Serial.println("--------------------------------");
  Serial.println("Iniciando TEST 1/3 (MODEM POWERON)");

  for (char i=0;i<5;i++) {
    Serial.print("...");
    if (digitalRead(GSM_STATUS) == HIGH) {
      Serial.println("HIGH");
      break;
    } else {
      Serial.println("LOW");
      digitalWrite(GSM_PWRKEY, HIGH);
      delay(1500);                       // pulse for Power_ON
      digitalWrite(GSM_PWRKEY, LOW);
    }
    delay (1500);
  }

  if (digitalRead(GSM_STATUS) == HIGH) {
    debug("TEST 1/3 OK");
    testsOk++;
  } else {
    debug("TEST 1/3 FAIL");
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
  digitalWrite(GSM_PWREN, HIGH);
  digitalWrite(GSM_PWRKEY, LOW);
  digitalWrite(GPS_EN, LOW);

  // init serial. en "utils.c" se define "debug" como alias de Serial.printnl
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
    debug("");
    debug("-------------------------------------------------");
    debug("         ÉXITO - TODOS LOS TEST PASADOS");
    debug("-------------------------------------------------");
  } else {
    debug("");
    debug("-------------------------------------------------");
    debug("           ERROR - ALGO HA IDO MAL...");
    debug("-------------------------------------------------");
  }
}

void loop (void)
{
  // nada por aquí...
}

void getIMEI (void)
{
  debug("");
  debug("--------------------------------");
  debug("Iniciando TEST 2/3 (CHECK IMEI)");

  sendAt("AT+CGSN", 1500);

  // extraer imei
  char * imei;
  imei = strtok(buffer, "\n");
  imei = strtok(NULL, "\n");
  strncpy(imei, imei, 15);

  Serial.print("IMEI:");
  Serial.println(imei);

  if (strlen(imei) == 16) {
    debug("TEST 2/3 OK");
    testsOk++;
  } else {
    debug("TEST 2/3 FAIL");
  }
}


void getICCID (void)
{
  debug("");
  debug("--------------------------------");
  debug("Iniciando TEST 3/3 (CHECK ICCID)");

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
    debug("TEST 3/3 OK");
    testsOk++;
  } else {
    debug("TEST 3/3 FAIL");
  }
}
