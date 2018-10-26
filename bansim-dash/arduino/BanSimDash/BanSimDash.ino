/*
 * BanSimBoard - Banshee 2016
 * 
 * Controlador sobre Arduino Pro Micro (Leonardo)
 * para control de BanSim
 * 
 * Componentes
 * 
 *  - Array Leds principal para rpms
 *  - Doble array de leds para flags
 *  - Tacometro
 *  - Botonera
 *  - Freno de mano
 */

#include <Adafruit_NeoPixel.h>
#include <TimerThree.h>
#include <TimerOne.h>

#define BANSIMBOARD_VERSION           0.7

// Input Modes
#define INPUT_MODE_ASCII              0
#define INPUT_MODE_BINARY             1

// Binary Commands
#define BINARY_CMD_CHANGE_MODE        0x00
#define BINARY_CMD_LED1               0x01
#define BINARY_CMD_BLINK              0x02
#define BINARY_CMD_NEUTRAL            0x03
#define BINARY_CMD_KITT               0x04
#define BINARY_CMD_TC                 0x05
#define BINARY_CMD_FLAG               0x06
#define BINARY_CMD_PITLIM             0x07
#define BINARY_CMD_BOOST              0x08
#define BINARY_CMD_DRS                0x09

// Led1 Modes
#define LED1_INTERRUPT_MODE_NONE      0
#define LED1_INTERRUPT_MODE_BLINK     1
#define LED1_INTERRUPT_MODE_NEUTRAL   2
#define LED1_INTERRUPT_MODE_KITT      3
#define LED1_DEFAULT_BRIGHTNESS       25

// Led2 Modes
#define LED2_DEFAULT_BRIGHTNESS       25
#define LED2_INTERRUPT_FLAG_NONE      0
#define LED2_INTERRUPT_FLAG_BLUE      1
#define LED2_INTERRUPT_FLAG_YELLOW    2
#define LED2_INTERRUPT_FLAG_BLACK     3
#define LED2_INTERRUPT_FLAG_WHITE     4
#define LED2_INTERRUPT_FLAG_CHECK     5
#define LED2_INTERRUPT_FLAG_PENALTY   6


// TODO: Permitir establecer mediante comandos serie
// Leds Array Config
const uint8_t DEFAULT_LED_ARRAY_SIZE     = 12;
const uint8_t DEFAULT_LED_ARRAY_PIN      = 6;
const uint8_t DEFAULT_LED_BLINK_MILLIS   = 100;
const uint8_t DEFAULT_LED_NEUTRAL_MILLIS = 250;
const uint8_t DEFAULT_LED_KITT_MILLIS    = 100;
const uint8_t DEFAULT_LED_KITT_LEN       = 3;

const uint8_t DEFAULT_LED2_ARRAY_SIZE    = 8;
const uint8_t DEFAULT_LED2_ARRAY_PIN     = 14;
const uint8_t DEFAULT_LED2_BLINK_MILLIS  = 200;


// Serial Config
#define SERIAL_SPEED                  9600

// Timers Default periods
#define REFRESH_TIMER_MICROSECS       1500L
#define TACHOMETER_TIMER_MICROSECS    3000L

// Led1 Config
#define DEFAULT_LED_ARRAY_PIN         6
#define DEFAULT_LED_ARRAY_SIZE        12
#define DEFAULT_LED_BLINK_MILLIS      100L
#define DEFAULT_LED_NEUTRAL_MILLIS    250L

// Tachometer Config
#define DEFAULT_TACHOMETER_ARRAY_PIN  5
#define TACHOMETER_MAX_RPMS           11000
#define TACHOMETER_PWM_DUTY           512



//////////////
// Constants 
//////////////

// Leds Array Config
const uint8_t LED_ARRAY_SIZE        = DEFAULT_LED_ARRAY_SIZE;
const uint8_t LED_ARRAY_PIN         = DEFAULT_LED_ARRAY_PIN;
const uint8_t LED2_ARRAY_SIZE       = DEFAULT_LED2_ARRAY_SIZE;
const uint8_t LED2_ARRAY_PIN        = DEFAULT_LED2_ARRAY_PIN;



// Tachometer config
const uint8_t TACHOMETER_ARRAY_PIN  = DEFAULT_TACHOMETER_ARRAY_PIN;

///////////////////
// Volatile Vars
///////////////////

// Leds1 General Vars
volatile uint8_t leds1Mode;
volatile uint8_t nLeds1Active;
volatile uint8_t lastNLeds1Active;

// Leds Blink Control
volatile unsigned long lastBlinkTime;
volatile uint8_t ledsBlinkState;

// Leds2 Blink Control
volatile unsigned long lastBlinkTime2;
volatile uint8_t ledsBlinkState2;

// Leds Neutral Control
volatile unsigned long lastNeutralTime;
volatile uint8_t ledsNeutralState;

// Leds Kitt Control
volatile unsigned long lastKittTime;
volatile int8_t ledsKittState;
volatile int8_t ledsKittDirection;


//volatile unsigned long avgTimeClbk = -1;
volatile uint8_t leds2FlagMode;
volatile uint8_t lastLeds2FlagMode;

volatile uint8_t leds2PitLimMode;
volatile uint8_t lastLeds2PitLimMode;

volatile uint8_t leds2BoostMode;
volatile uint8_t lastLeds2BoostMode;

volatile uint8_t leds2DRSMode;
volatile uint8_t lastLeds2DRSMode;

////////////////
// Global Vars
////////////////

// Adafruit leds controller - Necesario instanciar la variable? 
Adafruit_NeoPixel leds(DEFAULT_LED_ARRAY_SIZE, DEFAULT_LED_ARRAY_PIN);
Adafruit_NeoPixel leds2(DEFAULT_LED2_ARRAY_SIZE, DEFAULT_LED2_ARRAY_PIN);

uint8_t inputMode = INPUT_MODE_ASCII;
//uint8_t inputMode = INPUT_MODE_BINARY;

// Array para la lectura de comandos ASCII
char    cmd[16];
uint8_t cmdlen = 0;
uint8_t echo   = 1;

// Array bytes para la lectura de comandos binarios
// TODO: ...


/* 
 * Main Setup
 */
void setup() {
  int i;
  
  // Serial Initialization
  Serial.begin(SERIAL_SPEED);
  //Serial1.begin(SERIAL_SPEED);
  
  // Generic set pin mode. Pins 0 y 1 reserved to USB-Serial port.
  for(i= 2; i<=16; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, HIGH); // Set pull up resistor
  }

  // To Debug
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  
  ////////////////////////////
  // Inicializacion de Timers
  ////////////////////////////
  
  // Timer 1. Rutina de refresco
  Timer1.initialize(REFRESH_TIMER_MICROSECS);
  Timer1.attachInterrupt(refreshCallback);

  // Timer 3. Tacometro
  Timer3.initialize(calculateTachPWMPeriod((uint8_t) TACHOMETER_MAX_RPMS));
  Timer3.pwm(DEFAULT_TACHOMETER_ARRAY_PIN, TACHOMETER_PWM_DUTY);
  delay(2000L);
  Timer3.initialize(calculateTachPWMPeriod((uint8_t) 0));
  Timer3.pwm(DEFAULT_TACHOMETER_ARRAY_PIN, TACHOMETER_PWM_DUTY);
    
  
  // Initializacion de array de Leds 1
  leds.setPin(DEFAULT_LED_ARRAY_PIN);
  leds.setNumPixels(DEFAULT_LED_ARRAY_SIZE);
  leds.setBrightness(LED1_DEFAULT_BRIGHTNESS);
  clearLedArray();
  leds.show();
  
  // Leds Mode
  leds1Mode = LED1_INTERRUPT_MODE_NONE;
  
  // Leds1 General Vars
  nLeds1Active = lastNLeds1Active = ledsBlinkState = 0;
  
    // Initializacion de array de Leds 2  
  leds2.setPin(DEFAULT_LED2_ARRAY_PIN);
  leds2.setNumPixels(DEFAULT_LED2_ARRAY_SIZE);
  leds2.setBrightness(LED2_DEFAULT_BRIGHTNESS);
  clearLed2Array();
  
  // Leds2 General Vars
  leds2FlagMode = lastLeds2FlagMode = (uint8_t) LED2_INTERRUPT_FLAG_NONE;
}


/*
 * Main Loop
 */
void loop() {
  // Read Command. No Block
  if(inputMode == INPUT_MODE_ASCII) {
    if(readASCIICommand()){
      executeASCIICommand();
    }   
  } else {
    executeBinaryCommand(); // Efficient way. Read and execute.
  }
}

/*
 * Reads serial port ASCII command
 */
boolean readASCIICommand() {
  char c;
  while(Serial.available() > 0) {
    c = Serial.read();
    if(echo) {
      Serial.write(c);
      if(c == '\r') Serial.write('\n');
    }
    if(c == '\r') return true;
    cmd[cmdlen] = c;
    cmdlen++;
  }
  
  return false;
}

/*
 * Execute ASCII command
 */
void executeASCIICommand() {
  uint8_t error;
  cmd[cmdlen] = '\0';
  
  if(echo){
    Serial.write("Execute command : ");
    Serial.write(cmd);
    Serial.write('\n');
  }
  
  if(strcmp(cmd,"VERSION") == 0) {
    error = cmdVersion();
  } else if(strncmp(cmd,"SET ", 4) == 0) {
    error = cmdSet(cmd+4);
  } else if(strcmp(cmd,"HELP") == 0) {
    error = cmdHelp();
  } else {
    error = 1;
  }
  
  //if(echo) {
    if(error == 0) {
      Serial.println("OK");
    } else {
      Serial.println("ERROR");
    //}  
  }
  cmdlen = 0;
  
  Serial.flush();
}

/*
 *  Lee y ehecuta un comando binario.
 *  
 *  Un comando binario consta de 2 bytes. <OP><VALUE>
 */
boolean executeBinaryCommand(){
  // Read Operand
  boolean cmdRes = false;
  byte op;
  
  byte value[2];

  if(Serial.available() < 2){
    return false;
  }

  op = Serial.read();

  if(echo) {
    Serial.print("Binary Op : "); Serial.println(op, DEC);
  }

  switch(op) {
    case byte( BINARY_CMD_CHANGE_MODE):
      //binarySetLeds1();
      break;
    case byte(BINARY_CMD_LED1):
      binarySetLeds1();
      break;
    case byte(BINARY_CMD_BLINK):
      binarySetLeds1Blink();
      break;
    case byte(BINARY_CMD_NEUTRAL):
      binarySetNeutral();
      break;
    case byte(BINARY_CMD_KITT):
      binarySetKitt();
      break;
    case byte(BINARY_CMD_TC):
      binarySetTachometer();
      break;
    case byte(BINARY_CMD_FLAG):
      binarySetFlags();
      break;
    case byte(BINARY_CMD_PITLIM):
      binarySetTachometer();
      break;
    case byte(BINARY_CMD_BOOST):
      binarySetBoost();
      break;
   case byte(BINARY_CMD_DRS):
      binarySetDRS();
      break;
    default: 
      if(echo) Serial.println("Comando no reconocido.");
      cmdRes = true;
  }

  while(Serial.available() > 0){
    if(Serial.read() == '\r') break;
  }

  Serial.write('\n');
  Serial.flush();
  return cmdRes;
}


/*
 * Parsea y ejecuta un comando ASCII
 */
uint8_t cmdSet(char* v) {
 
  if(strncmp(v,"ECHO=",5) == 0) {
    setEcho(atoi(v+5));

  // Leds Number MODE
  } else if(strncmp(v,"L1N=",4) == 0) {
    setLeds1(atoi(v+4));
    
// 7 Segments     
//  } else if(strncmp(v,"SEG1=",5) == 0) {
//    parseSegValue(seg1, atoi(v+5));

  // Blink Mode
  } else if(strncmp(v,"L1BLINK=",8) == 0){
    setLeds1Blink(atoi(v+8));

  // Neutral Mode
  } else if(strncmp(v,"L1NEUTRAL=",10) == 0){
    setNeutral(atoi(v+10));
    
  // Kitt Mode
  } else if(strncmp(v,"L1KITT=",7) == 0){
    setKitt(atoi(v+7));
     
  // Tachometer
  } else if(strncmp(v,"TC=",3) == 0){
    setTachometer(atoi(v+3));
  
  // Flag Mode  
  } else if(strncmp(v,"FLAGS=",6) == 0){
    setFlags(atoi(v+6));
 
  // Pit Limiter
  } else if(strncmp(v,"PITLIM=",7) == 0){
    setFlags(atoi(v+7));
    
  // Boost
  } else if(strncmp(v,"BOOST=",6) == 0){
    setBoost(atoi(v+6));

  // DRS
  } else if(strncmp(v,"DRS=",4) == 0){
    setDRS(atoi(v+4));
  
  // Default error
  } else {
    return 1;
  }
  return 0;
}


////////////////////////////
// Manejadores de comandos
////////////////////////////


/*
 * Echo Handler
 */
void setEcho(uint8_t echoMode) {
  echo = echoMode;
}


void binarySetLeds1(){
  if(Serial.available() < 1)
    return;

  unsigned int value = Serial.read();
  if(echo) { Serial.print("Value : "); Serial.println(value, DEC); }

  setLeds1(value);
}

/* 
 * Establece la tira de leds1 a 'numLeds'
 */
void setLeds1(uint8_t numLeds) {
   leds1Mode    = LED1_INTERRUPT_MODE_NONE;
   nLeds1Active = numLeds;   
}


void binarySetLeds1Blink(){
  if(Serial.available() < 1)
    return;

  unsigned int value = Serial.read();
  if(echo) { Serial.print("Value : "); Serial.println(value, DEC); }

  setLeds1Blink(value);
}

/*
 * Activa/Desactiva el parpadeo de leds1 
 * 
 * Cualquier otro comando sobre leds1 desactivara el parpadeo
 */
void setLeds1Blink(uint8_t blink) {
  if(blink == 1){
    leds1Mode     = LED1_INTERRUPT_MODE_BLINK;
    lastBlinkTime = millis();
  }else{
    leds1Mode = LED1_INTERRUPT_MODE_NONE;      
  }
}

void binarySetNeutral(){
  if(Serial.available() < 1)
    return;

  unsigned int value = Serial.read();
  if(echo) { Serial.print("Value : "); Serial.println(value, DEC); }

  setNeutral(value);
}

/*
 * Activa/Desactica el modo neutral de leds1
 * 
 * Cualquier otro comando sobre leds1 desactivara el modo neutral
 * Establece el numero de leds activos a 0. 
 */
void setNeutral(uint8_t neutral) {
  if(neutral == 1){
    leds1Mode         = LED1_INTERRUPT_MODE_NEUTRAL;
    lastNeutralTime   = millis();
  }else{
    leds1Mode         = LED1_INTERRUPT_MODE_NONE;      
    nLeds1Active      = 0;
    lastNLeds1Active  = 1; // Diferente del ultimo para que lo trate la interrupcion. Mejorar.
  }
}

void binarySetKitt(){
  if(Serial.available() < 1)
    return;

  unsigned int value = Serial.read();
  if(echo) { Serial.print("Value : "); Serial.println(value, DEC); }

  setKitt(value);
}

/*
 * Activa/Desactica el modo neutral de leds1
 * 
 * Cualquier otro comando sobre leds1 desactivara el modo neutral
 * Establece el numero de leds activos a 0. 
 */
void setKitt(uint8_t kitt) {
 if(kitt == 1){
    leds1Mode          = LED1_INTERRUPT_MODE_KITT;
    lastKittTime       = millis();
    ledsKittState      = -3;
    ledsKittDirection  = 1;
  }else{
    leds1Mode         = LED1_INTERRUPT_MODE_NONE;      
    nLeds1Active      = 0;
    lastNLeds1Active  = 1; // Diferente del ultimo para que lo trate la interrupcion. Mejorar.
  }
}

void binarySetTachometer(){
  unsigned long currentTime;
  
  if(Serial.available() < 2)
    return;

  uint16_t value = Serial.read();
  value =  Serial.read() | value << 8;
  if(echo) { Serial.print("Value : "); Serial.println(value, HEX); }

  currentTime = millis();

  cli();
  digitalWrite(2, HIGH);
  setTachometer(value);
  digitalWrite(2, LOW);
  sei();
}

/*
 * Establece la señal de tacometro a las rpms indicadas.
 * 
 * Usado Timer3
 */
void setTachometer(uint16_t rpms) {
  if(echo) { Serial.print("Set TC rpms:"); Serial.print(rpms); }
  
  if(rpms >= 0/* && rpms <= TACHOMETER_MAX_RPMS*/) {
    long period = calculateTachPWMPeriod(rpms);
    
    if(echo){
      Serial.print("Set Period : ");
      Serial.println((long) period);
    }
    
    /*    
     * BUG: La libreria no establece correctamente un nuevo periodo para determinados valores 
            en función del periodo con el que fuera inicializada.

            Ej: Inicializada a 11000 rpms no funciona para valores entre 2000 y 7000.

    Timer3.setPeriod(period);
    Timer3.restart();
    */
    
    // Debido al bug anterior se realiza siempre una inicializacion del timer
    Timer3.initialize(period);
    Timer3.pwm(DEFAULT_TACHOMETER_ARRAY_PIN, 512);    
  }
}

void binarySetFlags(){
  if(Serial.available() < 1)
    return;

  unsigned int value = Serial.read();
  if(echo) { Serial.print("Value : "); Serial.println(value, DEC); }

  setFlags(value);
}


void setPitLimiter(uint8_t pitLimState) {
   leds2PitLimMode = pitLimState;
}

void binaryPitLimiter(){
  if(Serial.available() < 1)
    return;

  unsigned int value = Serial.read();
  if(echo) { Serial.print("Value : "); Serial.println(value, DEC); }

  setPitLimiter(value);
}

void setFlags(uint8_t flag) {
   leds2FlagMode = flag;
}

void binarySetBoost(){
  if(Serial.available() < 1)
    return;

  unsigned int value = Serial.read();
  if(echo) { Serial.print("Value : "); Serial.println(value, DEC); }

  setBoost(value);
}

void setBoost(uint8_t boost) {
   leds2BoostMode = boost;
}

void binarySetDRS(){
  if(Serial.available() < 1)
    return;

  unsigned int value = Serial.read();
  if(echo) { Serial.print("Value : "); Serial.println(value, DEC); }

  setDRS(value);
}

void setDRS(uint8_t drs) {
   leds2DRSMode = drs;
}


/*
 * Help Handler
 */
uint8_t cmdHelp() {
  Serial.print("\n\nBanSimBoard - Help  Version : ");
  Serial.println(BANSIMBOARD_VERSION);
  Serial.println(" Commands :\n");
  Serial.println("  HELP\\r             : Show this help");
  Serial.println("  ECHO=<0|1>\\r       : Turn on/off echo mode");
  Serial.println("  VERSION\\r          : Show Version");
  Serial.println('\n');
  Serial.println(" ASCII MODE :");
  Serial.println("    SET <OPERATION>=<VALUE>");
  Serial.println('\n');
  Serial.println("      L1N=<N>         : Turn on Led1 N Leds");
  Serial.println("      L1BLINK=<0|1>   : Turn on/off Led1 Blink");
  Serial.println("      L1NEUTRAL=<0|1> : Turn on/off Led1 Neutral");
  Serial.println("      KITT=<0|1>      : Turn on/off Kitt Mode");
  Serial.println("      TC=<RPM>        : Set tachometer at RPM");
  Serial.println("      FLAG=<FLAG>     : Set Flag Mode");
  Serial.println("      PITLIM=<0|1>    : Set Pit Limiter");
  Serial.println("      BOOST=<0|1>     : Boost");
  Serial.println("      DRS=<0|1|2>     : DRS off/zone/on");
  return 0;
}


/*
 * Version Handler
 */
uint8_t cmdVersion() {
  Serial.print("\n\nBanSimBoard - Version : ");
  Serial.println(BANSIMBOARD_VERSION);
  return 0;
}

//-------------------
// Led Array Funcs
//-------------------

/*
 * Enciende 'numLeds' de leds segun el esquema de color predefinido
 */

void loadLedArray(uint8_t numLeds){
  uint8_t i;
  
  clearLedArray();
  for(i = 0; i < numLeds && i < DEFAULT_LED_ARRAY_SIZE; i++){
    if(i < 4){
      leds.setPixelColor(i, '\x00', '\xff', '\x00');
    }else if(i < 8){
      leds.setPixelColor(i, '\xff', '\x00', '\x00');
    }else{
      leds.setPixelColor(i, '\x00', '\x00', '\xff');
    }
  }  
}

/*
 * Establece la tira de leds en modo neutral en funcion de 'phase' (leds pares o impares)
 */
void loadLedNeutralArray(uint8_t numLeds, uint8_t phase){
  uint8_t i;
  
  clearLedArray();
  for(i = 0; i < numLeds && i < DEFAULT_LED_ARRAY_SIZE; i++){
    if(i % 2 == phase){
      if(i < 4){
        leds.setPixelColor(i, '\x00', '\xff', '\x00');
      }else if(i < 8){
        leds.setPixelColor(i, '\xff', '\x00', '\x00');
      }else{
        leds.setPixelColor(i, '\x00', '\x00', '\xff');
      }
    }
  }  
}


void loadLedKittArray(uint8_t numLeds, int8_t phase, int8_t dir){
  uint8_t i;
  uint8_t nLed;

  clearLedArray();

  for (i = 0; i < DEFAULT_LED_KITT_LEN; i++){
    if(dir == 1)
      nLed = phase + i;
    else 
      nLed = phase - i;
      
    if(nLed >= 0 && nLed < DEFAULT_LED_ARRAY_SIZE){
      leds.setPixelColor(nLed, '\xff', '\x00', '\x00');        
    }
  }  
}

/*
 * Apaga la tira de leds
 */
void clearLedArray(){
  int i;  
  for(i = 0; i < DEFAULT_LED_ARRAY_SIZE; i++)
     leds.setPixelColor(i, '\x00', '\x00', '\x00'); // setPixelColor(uint16_t n, uint32_t c), 
}


void loadLedFlagMode(uint8_t flagMode, uint8_t blink){
  uint8_t i;
  clearLed2Array();
  
  if(blink) {
    for(i = 0; i < DEFAULT_LED2_ARRAY_SIZE; i++){  
      if(flagMode == LED2_INTERRUPT_FLAG_BLUE) {
        leds2.setPixelColor(i, '\x00', '\x00', '\xff');
      } else if (flagMode == LED2_INTERRUPT_FLAG_YELLOW) {
        leds2.setPixelColor(i, '\xff', '\x00', '\xff');    
      } else if (flagMode == LED2_INTERRUPT_FLAG_BLACK) {
        leds2.setPixelColor(i, '\x00', '\xff', '\x00');
      } else if (flagMode == LED2_INTERRUPT_FLAG_WHITE) {
        leds2.setPixelColor(i, '\xff', '\xff', '\xff');
      } else if (flagMode == LED2_INTERRUPT_FLAG_CHECK) {
        leds2.setPixelColor(i, '\xff', '\xff', '\x22');
      } else if (flagMode == LED2_INTERRUPT_FLAG_PENALTY) {
        leds2.setPixelColor(i, '\xff', '\x00', '\x00');
      }
    }  
  }
}

void loadLedPitLimiterMode(uint8_t pitLimitMode, uint8_t blink){
  uint8_t i;
  clearLed2Array();
  
  if(blink) {
    for(i = 0; i < DEFAULT_LED2_ARRAY_SIZE; i++){   
      leds2.setPixelColor(i, '\xff', '\xff', '\xff');
    } 
  }
}

void loadBoostMode(uint8_t boostMode){
  uint8_t i;
  clearLed2Array();
  
  if(boostMode == 1) {
    for(i = 0; i < DEFAULT_LED2_ARRAY_SIZE; i++){   
      leds2.setPixelColor(i, '\x00', '\x00', '\xff');
    } 
  }
}

void loadDRSMode(uint8_t drsMode){
  uint8_t i;
  clearLed2Array();

  for(i = 0; i < DEFAULT_LED2_ARRAY_SIZE; i++){   
    if(drsMode == 2 || (drsMode == 1 && (i % (DEFAULT_LED2_ARRAY_SIZE / 2)) == 0)) {
      leds2.setPixelColor(i, '\x00', '\xff', '\x00');
    }
  } 
}

void clearLed2Array(){
  int i;  
  for(i = 0; i < DEFAULT_LED2_ARRAY_SIZE; i++)
     leds2.setPixelColor(i, '\x00', '\x00', '\x00'); // setPixelColor(uint16_t n, uint32_t c), 
}

/*
 * Calcula el periodo de la señal PWM equivalente para las revoluciones dadas.
 *
 * Modo del tacometro: 2 cilindros. Una revolucion del cigueñal corresponde a un ciclo completo del motor.
 */
unsigned long calculateTachPWMPeriod(uint16_t rpm) {
  // 1 / (x / 60) * 10^6
  // 6.10^7  / x
  return 60000000L / rpm;
}


/*
 * Refresh Callback
 */

void refreshCallback(void){
  digitalWrite(3, HIGH);

  unsigned long currentTime = millis();
  uint8_t processedLeds1 = 0, processedLeds2 = 0;
  
  ////////////
  // Leds 1
  ////////////
  
  // Leds Array Refresh
  if(leds1Mode == LED1_INTERRUPT_MODE_NONE && nLeds1Active != lastNLeds1Active){
    loadLedArray(nLeds1Active);
    lastNLeds1Active = nLeds1Active;
    processedLeds1 = 1;
  } 
  

  // Leds Blink
  if(leds1Mode == LED1_INTERRUPT_MODE_BLINK && abs(currentTime-lastBlinkTime) > DEFAULT_LED_BLINK_MILLIS){
    lastBlinkTime = currentTime;
    
    if(ledsBlinkState == 0){
       loadLedArray(nLeds1Active);  
       ledsBlinkState = 1;
    }else{
       loadLedArray(0);      
       ledsBlinkState = 0;
    }

    processedLeds1 = 1;
  }
  
  // Leds Neutral
  if(leds1Mode == LED1_INTERRUPT_MODE_NEUTRAL && abs(currentTime-lastNeutralTime) > DEFAULT_LED_NEUTRAL_MILLIS){
    lastNeutralTime = currentTime;

    loadLedNeutralArray(DEFAULT_LED_ARRAY_SIZE, ledsNeutralState);  
    
    if(ledsNeutralState == 0){
       ledsNeutralState = 1;
    }else{
       ledsNeutralState = 0;
    }
    processedLeds1 = 1;
  }
  
   // Leds Kitt
  if(leds1Mode == LED1_INTERRUPT_MODE_KITT && abs(currentTime-lastKittTime) > DEFAULT_LED_KITT_MILLIS && DEFAULT_LED_KITT_LEN < DEFAULT_LED_ARRAY_SIZE){
    lastKittTime = currentTime;
    
    if(ledsKittState <= -3 && ledsKittDirection == -1){
      ledsKittDirection = 1;
    } else if(ledsKittState >= (DEFAULT_LED_ARRAY_SIZE + DEFAULT_LED_KITT_LEN) && ledsKittDirection == 1) {
      ledsKittDirection = -1;      
    }
    
    loadLedKittArray(DEFAULT_LED_ARRAY_SIZE, ledsKittState, ledsKittDirection);
    ledsKittState = ledsKittState + ledsKittDirection;
    
    processedLeds1 = 1;
  }
  
  
  
  ////////////
  // Leds 2
  ////////////
 
  // Flags 
  if(leds2FlagMode != lastLeds2FlagMode || (leds2FlagMode != LED2_INTERRUPT_FLAG_NONE && abs(currentTime-lastBlinkTime2) > DEFAULT_LED2_BLINK_MILLIS)) {
    
    loadLedFlagMode(leds2FlagMode, ledsBlinkState2);
    
    if(ledsBlinkState2 == 0) {
      ledsBlinkState2 = 1;
    } else {
       ledsBlinkState2 = 0;    
    }
    lastBlinkTime2 = currentTime;
    processedLeds2 = 1;      
  }
  
  // Pit Limit
  if(processedLeds2 == 0) {    
    if((leds2PitLimMode == 0 && leds2PitLimMode != lastLeds2PitLimMode) || (leds2PitLimMode == 1 &&  abs(currentTime-lastBlinkTime2) > DEFAULT_LED2_BLINK_MILLIS)) {
      
      loadLedPitLimiterMode(leds2PitLimMode, ledsBlinkState2);
      if(ledsBlinkState2 == 0) {
        ledsBlinkState2 = 1;
      } else {
         ledsBlinkState2 = 0;    
      }
      lastBlinkTime2 = currentTime;
      processedLeds2 = 1;      
    }
  }
  
  // Boost
  if(processedLeds2 == 0 && leds2BoostMode != lastLeds2BoostMode) {
    loadBoostMode(leds2BoostMode);
    lastLeds2BoostMode = leds2BoostMode;
    processedLeds2 = 1;
  }
  
  // DRS
  if(processedLeds2 == 0 && leds2DRSMode != lastLeds2DRSMode) {
    loadDRSMode(leds2DRSMode);
    lastLeds2BoostMode = leds2BoostMode;
    processedLeds2 = 1;
  }
  
  
  if(processedLeds1 == 1) {
    //cli();
    leds.show();
    //sei();
  }
  
  if(processedLeds2 == 1) {
    //cli();
    leds2.show();
    //sei();
  }
  
  digitalWrite(3, LOW);
}


