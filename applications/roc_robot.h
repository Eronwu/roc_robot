#ifndef  __ROC_ROBOT_H__
#define  __ROC_ROBOT_H__


#include <rtthread.h>
#include <chassis.h>

// MOTOR
#define LEFT_FORWARD_A_PWM            "pwm4"
#define LEFT_FORWARD_A_PWM_CHANNEL    1         // GPIO PD12
#define LEFT_FORWARD_B_PWM            "pwm4"
#define LEFT_FORWARD_B_PWM_CHANNEL    2         // GPIO PD13

#define LEFT_BACKWARD_A_PWM           "pwm4"   
#define LEFT_BACKWARD_A_PWM_CHANNEL   3         // GPIO PB8
#define LEFT_BACKWARD_B_PWM           "pwm4"
#define LEFT_BACKWARD_B_PWM_CHANNEL   4         // GPIO PB9

#define RIGHT_FORWARD_A_PWM           "pwm2"
#define RIGHT_FORWARD_A_PWM_CHANNEL   1          // GPIO PA0
#define RIGHT_FORWARD_B_PWM           "pwm2"
#define RIGHT_FORWARD_B_PWM_CHANNEL   2          // GPIO PA1

#define RIGHT_BACKWARD_A_PWM          "pwm2"
#define RIGHT_BACKWARD_A_PWM_CHANNEL  3          // GPIO PB10
#define RIGHT_BACKWARD_B_PWM          "pwm2"
#define RIGHT_BACKWARD_B_PWM_CHANNEL  4          // GPIO PB11

// ENCODER
#define LEFT_FORWARD_ENCODER_PIN     34     // GET_PIN(C, 2)
#define LEFT_BACKWARD_ENCODER_PIN    36     // GET_PIN(C, 4)
#define RIGHT_FORWARD_ENCODER_PIN    38     // GET_PIN(C, 6)
#define RIGHT_BACKWARD_ENCODER_PIN   39     // GET_PIN(C, 7)

#define PULSE_PER_REVOL             20     // Real value 20
#define SAMPLE_TIME                1000

// PID
#define PID_SAMPLE_TIME             50

// WHEEL
#define WHEEL_RADIUS             0.066
#define GEAR_RATIO                   1

#define WHEEL_DIST_X                 0
#define WHEEL_DIST_Y              0.13

// Car Thread
#define THREAD_PRIORITY             10
#define THREAD_STACK_SIZE          512
#define THREAD_TIMESLICE             5

typedef enum
{
    E_BLUETOOTH_CONTROL = 0,
    E_INFRARED_REMOTE_CONTROL,
    E_INFRARED_TRACKING_MODE,
    E_INFRARED_AVOIDANCE_MODE,
    E_ULTRASONIC_AVOIDANCE,
    E_PS2_REMOTE_CONTROL,
    E_NRF24L01_CONTROL,
    E_ULTRASONIC_INFRARED_AVOIDANCE,
    E_PIANO_MODE,
    E_RGB_MODE,
    E_LED_MAXTRIX_MODE,
    E_CMD_LINE_MODE,
    E_LIGHT_SEEKING_MODE,
    E_ULTRASONIC_FOLLOW_MODE,
    E_SMARTCAR_CONTROL_MAX,
} E_ROC_ROBOT_CONTROL_MODE;

typedef enum
{
    E_FORWARD = 0,
    E_BACK,
    E_LEFT,
    E_RIGHT,
    E_RIGHT_ROTATE,
    E_LEFT_ROTATE,
    E_STOP,
    E_RUNNING,
    E_SPEED_UP,
    E_SPEED_DOWN,
    E_LOW_POWER,
} E_ROC_ROBOT_STATUS;


typedef struct {

    motor_t left_forward_motor, left_backward_motor, right_forward_motor, right_backward_motor;
    encoder_t left_forward_encoder, left_backward_encoder, right_forward_encoder, right_backward_encoder;
    pid_control_t left_forward_pid, left_backward_pid, right_forward_pid, right_backward_pid;
    wheel_t left_forward_wheel, left_backward_wheel, right_forward_wheel, right_backward_wheel;

    motor_t x_servo, y_servo;
    kinematics_t c_kinematics;
    E_ROC_ROBOT_STATUS status;
    rt_int8_t speed ;
    rt_int16_t degree;
}ST_ROC_ROBOT;

//extern ST_ROC_ROBOT roc_robot;

void roc_robot_init(void *parameter);
/*
void roc_robot_go_forward(void);
void roc_robot_go_backward(void);
void roc_robot_turn_left(void);
void roc_robot_turn_right(void);
void roc_robot_stop(void);
void roc_robot_drive(int degree);
void roc_robot_set_speed(rt_int16_t s);
int  roc_robot_get_speed(void);
void roc_robot_set_status(E_ROC_ROBOT_STATUS status);
*/

#endif  /* _ROC_ROBOT_H_ */

