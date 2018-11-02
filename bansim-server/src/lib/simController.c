#include <stdio.h>
#include <math.h>
#include <string.h>
#include "../headers/simController.h"
#include "../headers/logger.h"
#include <sys/time.h>


#define LED_RPM_NUMLEDS                 12
#define LED_RPM_START_RATIO             0.75
#define LED_RPM_MAX_RATIO               0.95
#define LED_BLINK_DELAY_MILLIS          250L
#define LED_NEUTRAL_DELAY_MILLIS        3000L
#define TACHOMETER_DELAY_MILLIS         100L

#define BINARY_CMD_CHANGE_MODE          0x00
#define BINARY_CMD_LED1                 0x01
#define BINARY_CMD_BLINK                0x02
#define BINARY_CMD_NEUTRAL              0x03
#define BINARY_CMD_KITT                 0x04
#define BINARY_CMD_TC                   0x05
#define BINARY_CMD_FLAGS                0x06
#define BINARY_CMD_PITLIMIT             0x07
#define BINARY_CMD_BOOST                0x08
#define BINARY_CMD_DRS                  0x09
#define BINARY_CMD_ABS                  0x0a
#define BINARY_CMD_TRACTION             0x0b
#define BINARY_CMD_RESET                0x0c


static int lastGameState = 0;
static int lastLedOn = 0;
static int lastSpeed = 0;
static int lastTCRpms = 0;
static int lastGear = 0;
static int lastEngineOn = 0;
static int lastFlag = AC_NO_FLAG;
static int lastPitLimiter = 0;
static int lastBoost = 0;
static int lastDrs = 0;
static char lastABS = 0;
static char lastTraction = 0;


static int blinkOn = 0; // 3 estados. 0=OFF, 1=MARCADO, 2=ON
static long startBlinkTime;

static int neutralOn = 0; // 3 estados. 0=OFF, 1=MARCADO, 2=ON
static long startNeutralTime;

static int kittOn = 0;

static long lastTachometer = 0;

void reset();
int sendSimBoardCmdByte(serialContext* ctx, char cmd, char value);
int sendSimBoardCmdInt(serialContext* ctx, char cmd, uint16_t value);

void loadDefaultSimCtrlContext(simCtrlContext* ctx) {
    memset(ctx, 0, sizeof (ctx));
}

void setSimCtrlCOMPort(simCtrlContext* ctx, int comPort) {
    ctx->comPort = comPort;
}

void setSimCtrlSimSource(simCtrlContext* ctx, simSourceContext * simSrcCtx) {
    ctx->simSrcCtx = simSrcCtx;
}

int initializetSimCtrlContext(simCtrlContext* ctx) {
    loadDefaultSerialContext(&ctx->serialCtx);
    setSerialPort(&ctx->serialCtx, ctx->comPort);

    if (initializeSerialContext(&ctx->serialCtx) != 0) {
        blog(LOG_ERROR, "Error inicializando contexto serie. Abortando servidor ...");
        return -1;
    }
    blog(LOG_INFO, "Conexion con puerto COM%d establecida", ctx->serialCtx.comPortNumber);

    return 0;
}

void freeSimCtrlContext(simCtrlContext* ctx) {
    //    if(ctx->serialCtx != NULL){
    sendSimBoardCmdByte(&ctx->serialCtx, BINARY_CMD_LED1, 0);
    freeSerialContext(&ctx->serialCtx);
    //    }
}

char* itoa(int value, char * buffer, int radix) {
    if (sprintf(buffer, "%d", value)) return buffer;
    else return NULL;
}

long current_timestamp() {
    struct timeval te;
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec * 1000LL + te.tv_usec / 1000; // caculate milliseconds
    return milliseconds;
}

void reset() {
    lastGameState = 0;
    lastLedOn = 0;
    lastSpeed = 0;
    lastTCRpms = 0;
    lastGear = 0;
    lastEngineOn = 0;
    lastFlag = AC_NO_FLAG;
    lastPitLimiter = 0;
    lastBoost = 0;
    lastDrs = 0;
    lastABS = 0;
    lastTraction = 0;
    blinkOn = 0;
    neutralOn = 0;
    kittOn = 0;
    lastTachometer = 0;
    
    sendSimBoardCmdByte(&ctx->serialCtx, BINARY_CMD_RESET, 0);
}

int refreshLEDBar(simCtrlContext* ctx) {
    long currentTime = current_timestamp();
    char buff[5];

    int gameState = getGameState(ctx->simSrcCtx);

    // Kitt Mode
    if (kittOn == 1 && (gameState != AC_PAUSE)) {
        sendSimBoardCmdByte(&ctx->serialCtx, BINARY_CMD_KITT, 0);
        kittOn = 0;
    } else if (kittOn == 0 && (gameState == AC_PAUSE)) {
        sendSimBoardCmdByte(&ctx->serialCtx, BINARY_CMD_KITT, 1);
        kittOn = 1;
    }


    if (gameState == GAME_INGAME_PLAYING) {
        float throttle = getThrottle(ctx->simSrcCtx);
        int nGear = getGear(ctx->simSrcCtx);
        int rpms = getRpm(ctx->simSrcCtx);
        int maxRpms = getMaxRpms(ctx->simSrcCtx) * LED_RPM_MAX_RATIO;
        double ledsThres = maxRpms*LED_RPM_START_RATIO;
        double ledLen = (maxRpms - ledsThres) / LED_RPM_NUMLEDS;
        int engineOn = (rpms > 10); // Increiblemente con el motor calado da 0.98 (en PCars)...

        int numLeds = 0;


        if (rpms > ledsThres) {
            numLeds = round((rpms - ledsThres) / ledLen); //FIXME

            if (numLeds > LED_RPM_NUMLEDS) {
                numLeds = LED_RPM_NUMLEDS;
            }
        }

        // Cambio en numero de leds
        if (lastLedOn != numLeds) {
            itoa(numLeds, buff, 10);
            sendSimBoardCmdByte(&ctx->serialCtx, BINARY_CMD_LED1, (uint16_t) numLeds); // Disable other leds mode if active (BLINK, NEUTRAL, ... ))
            blinkOn = neutralOn = 0;
        }


        // Blink start condition
        if (blinkOn == 0 && numLeds == LED_RPM_NUMLEDS) {
            startBlinkTime = currentTime;
            blinkOn = 1;
        }


        // Todos los leds ON mas de LED_BLINK_DELAY_MILLIS
        if (blinkOn == 1 && (current_timestamp() - startBlinkTime) > LED_BLINK_DELAY_MILLIS) {
            sendSimBoardCmdByte(&ctx->serialCtx, BINARY_CMD_BLINK, 1);
            blinkOn = 2;
        }


        // Start Engine
        if (lastEngineOn != engineOn) {
            if (engineOn == 0) {
                sendSimBoardCmdByte(&ctx->serialCtx, BINARY_CMD_NEUTRAL, 1);
                neutralOn = 2;
            } else {
                sendSimBoardCmdByte(&ctx->serialCtx, BINARY_CMD_NEUTRAL, 0);
                neutralOn = 0;
            }
        } else {
            // Neutral start condition
            if (engineOn && neutralOn == 0 && nGear == 1 && numLeds == 0 && throttle == 0) {
                startNeutralTime = currentTime;
                neutralOn = 1;
            }

            // Neutral start/stop
            if (neutralOn > 0) {
                if (neutralOn == 2 && (nGear != 1 || numLeds != 0 || throttle != 0 || !engineOn)) {
                    sendSimBoardCmdByte(&ctx->serialCtx, BINARY_CMD_NEUTRAL, 0);
                    neutralOn = 0;
                } else if (neutralOn == 1 && (currentTime - startNeutralTime) > LED_NEUTRAL_DELAY_MILLIS) {
                    sendSimBoardCmdByte(&ctx->serialCtx, BINARY_CMD_NEUTRAL, 1);
                    neutralOn = 2;
                }
            }
        }

        // Direct Cars magnitudes
        lastLedOn = numLeds;
        lastGear = nGear;
        lastEngineOn = engineOn;
    }

    lastGameState = gameState;

    return 1;
}

void refresh8Segments(simCtrlContext* ctx) {
    char buff [8];

    // Acceso sucio directo por estructuras. TODO: acceso getters
    int speed = getSpeed(ctx->simSrcCtx);

    if (speed != lastSpeed) {
        lastSpeed = speed;

        itoa(speed, buff, 10);
        //sendSimBoardCmd(&ctx->serialCtx, "SEG1", buff);
    }
}

void refreshTachometer(simCtrlContext* ctx) {
    long currentTime = current_timestamp();

    if (currentTime - lastTachometer > TACHOMETER_DELAY_MILLIS) {
        lastTachometer = currentTime;

        int rpms = getRpm(ctx->simSrcCtx);
        if (lastTCRpms != rpms) {
            sendSimBoardCmdInt(&ctx->serialCtx, BINARY_CMD_TC, (uint16_t) rpms);
        }

        lastTCRpms = rpms;
    }
}

int refreshLED2Bar(simCtrlContext* ctx) {
    int gameState = getGameState(ctx->simSrcCtx);

    if (gameState == GAME_INGAME_PLAYING) {
        int pitLimit = getPitLimiter(ctx->simSrcCtx);
        int flag = getFlagStatus(ctx->simSrcCtx);
        int boost = (getTurboBoost(ctx->simSrcCtx) == 1) ? 1 : 0;
        int drs = getDRS(ctx->simSrcCtx);
        char abs = getABS(ctx->simSrcCtx);
        char traction = getTraction(ctx->simSrcCtx);

        if (lastFlag != flag) {
            sendSimBoardCmdByte(&ctx->serialCtx, BINARY_CMD_FLAGS, (uint16_t) flag);
            lastFlag = flag;
        }

        if (lastPitLimiter != pitLimit) {
            sendSimBoardCmdByte(&ctx->serialCtx, BINARY_CMD_PITLIMIT, (uint16_t) pitLimit);
            lastPitLimiter = pitLimit;
        }

        if (lastBoost != boost) {
            sendSimBoardCmdByte(&ctx->serialCtx, BINARY_CMD_BOOST, (uint16_t) boost);
            lastBoost = boost;
        }

        if (lastDrs != drs) {
            sendSimBoardCmdByte(&ctx->serialCtx, BINARY_CMD_DRS, (uint16_t) drs);
            lastDrs = drs;
        }

        if (lastABS != abs) {
            sendSimBoardCmdByte(&ctx->serialCtx, BINARY_CMD_ABS, (uint16_t) abs);
            lastABS = abs;
        }
        
        if (lastTraction != traction) {
            sendSimBoardCmdByte(&ctx->serialCtx, BINARY_CMD_TRACTION, (uint16_t) traction);
            lastTraction = traction;
        }
    }
}

int refreshMainPanel(simCtrlContext* ctx) {
    refreshLEDBar(ctx);
    refreshLED2Bar(ctx);
    //    refresh8Segments(ctx);
    refreshTachometer(ctx);
}

int sendSimBoardCmdByte(serialContext* ctx, char cmd, char value) {
    char buff[3];
    int readed;

    buff[0] = cmd;
    buff[1] = value;
    buff[2] = '\n';

    writeSerialData(ctx, buff, 3);
    readed = readSerialData(ctx, buff, 1);
    if (readed < 1) {
        printf("No serial Data readed!! TC %d", value);
    }
    return 0;
}

int sendSimBoardCmdInt(serialContext* ctx, char cmd, uint16_t value) {
    byte buff[4];
    int readed;

    buff[0] = cmd;
    buff[1] = (value >> 8);
    buff[2] = value & 0xff;
    buff[3] = '\n';

    writeSerialData(ctx, buff, 4);
    readed = readSerialData(ctx, buff, 1);
    if (readed < 1) {
        printf("No serial Data readed!! TC %d", value);
    }
    return 0;
}