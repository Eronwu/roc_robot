#ifndef  _PROTOCOL_H_
#define  _PROTOCOL_H_
#include <rtdef.h>
#define PROTOCOL_START_CODE 0xAA
#define PROTOCOL_END_CODE   0x55

//These are our button constants
#define BT_SELECT      0x0001
#define BT_START       0x0002
#define BT_PAD_RIGHT   0x0004
#define BT_PAD_UP      0x0008
#define BT_PAD_LEFT    0x0010
#define BT_PAD_DOWN    0x0020
#define BT_L3          0x0040 
#define BT_R3          0x0080 
#define BT_L2          0x0100
#define BT_R2          0x0200
#define BT_L1          0x0400
#define BT_R1          0x0800
#define BT_GREEN       0x1000
#define BT_RED         0x2000
#define BT_BLUE        0x4000
#define BT_PINK        0x8000
#define BT_TRIANGLE    0x1000
#define BT_CIRCLE      0x2000
#define BT_CROSS       0x4000
#define BT_SQUARE      0x8000


typedef enum
{
    E_BATTERY = 1,
    E_LED,
    E_BUZZER,
    E_INFO,
    E_ROBOT_CONTROL_DIRECTION,
    E_ROBOT_CONTROL_SPEED,
    E_TEMPERATURE,
    E_INFRARED_TRACKING,
    E_ULTRASONIC,
    E_INFRARED_REMOTE,
    E_INFRARED_AVOIDANCE,
    E_CONTROL_MODE,  //12
    E_BUTTON,
    E_LED_MAXTRIX,
    E_CMD_LINE,
    E_VERSION,
    E_UPGRADE,
    E_PHOTORESISTOR,
    E_SERVER_DEGREE,
    E_CONTOROL_CODE_MAX,
} E_CONTOROL_FUNC;

typedef enum
{
    E_SMARTCAR = 0,
    E_HUMMER_BOT,
    E_AURORA_RACING,
    E_PANTHER_TANK,
    E_BEETLE_BOT,
    E_BALANCE_CAR,
    E_OTTO,
    E_QUADCOPTER,
    E_HELLO_BOT,
    E_MIRAGE_TANK,
    E_TYPE_MAX,
} E_ROBOT_TYPE;

typedef struct
{
    rt_uint8_t start_code ;   // 8bit 0xAA
    rt_uint8_t len;           // protocol package data length
    E_ROBOT_TYPE type;
    rt_uint8_t addr;
    E_CONTOROL_FUNC function;      // 8 bit
    rt_uint8_t *data;         // n bit
    rt_uint16_t sum;       // check sum 16bit
    rt_uint8_t end_code;      // 8bit 0x55
} ST_PROTOCOL;

#endif // _PROTOCOL_H_
