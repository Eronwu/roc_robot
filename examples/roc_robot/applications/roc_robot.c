#include <roc_robot.h>

#define DBG_SECTION_NAME  "roc_robot"
#define DBG_LEVEL         DBG_LOG
#include <rtdbg.h>

ST_ROC_ROBOT roc_robot;
// CAR
chassis_t chas;

void roc_robot_init(void *parameter)
{
    wheel_t* c_wheels = (wheel_t*) rt_malloc(sizeof(wheel_t) * 4);
    if (c_wheels == RT_NULL)
    {
        LOG_E("Failed to malloc memory for wheels");
    }

    // 1.1 Create two motors
    roc_robot.left_forward_motor = dual_pwm_motor_create(LEFT_FORWARD_A_PWM, LEFT_FORWARD_A_PWM_CHANNEL, LEFT_FORWARD_B_PWM, LEFT_FORWARD_B_PWM_CHANNEL);
    roc_robot.left_backward_motor = dual_pwm_motor_create(LEFT_BACKWARD_A_PWM, LEFT_BACKWARD_A_PWM_CHANNEL, LEFT_BACKWARD_B_PWM, LEFT_BACKWARD_B_PWM_CHANNEL);
    roc_robot.right_forward_motor = dual_pwm_motor_create(RIGHT_FORWARD_A_PWM, RIGHT_FORWARD_A_PWM_CHANNEL, RIGHT_FORWARD_B_PWM, RIGHT_FORWARD_B_PWM_CHANNEL);
    roc_robot.right_backward_motor = dual_pwm_motor_create(RIGHT_BACKWARD_A_PWM, RIGHT_BACKWARD_A_PWM_CHANNEL, RIGHT_BACKWARD_B_PWM, RIGHT_BACKWARD_B_PWM_CHANNEL);

    // 1.2 Create two encoders
    roc_robot.left_forward_encoder  = ab_phase_encoder_create(LEFT_FORWARD_ENCODER_PIN_A, LEFT_FORWARD_ENCODER_PIN_B, PULSE_PER_REVOL);
    roc_robot.left_backward_encoder = ab_phase_encoder_create(LEFT_BACKWARD_ENCODER_PIN_A, LEFT_BACKWARD_ENCODER_PIN_B, PULSE_PER_REVOL);
    roc_robot.right_forward_encoder  = ab_phase_encoder_create(RIGHT_FORWARD_ENCODER_PIN_A, RIGHT_FORWARD_ENCODER_PIN_B, PULSE_PER_REVOL);
    roc_robot.right_backward_encoder = ab_phase_encoder_create(RIGHT_BACKWARD_ENCODER_PIN_A, RIGHT_BACKWARD_ENCODER_PIN_B, PULSE_PER_REVOL);

    // 1.3 Create two pid contollers
    roc_robot.left_forward_pid  = inc_pid_controller_create(PID_PARAM_KP, PID_PARAM_KI, PID_PARAM_KD);
    roc_robot.left_backward_pid = inc_pid_controller_create(PID_PARAM_KP, PID_PARAM_KI, PID_PARAM_KD);
    roc_robot.right_forward_pid  = inc_pid_controller_create(PID_PARAM_KP, PID_PARAM_KI, PID_PARAM_KD);
    roc_robot.right_backward_pid = inc_pid_controller_create(PID_PARAM_KP, PID_PARAM_KI, PID_PARAM_KD);

    // 1.4 Add two wheels
    c_wheels[0] = roc_robot.left_forward_wheel = wheel_create((motor_t)roc_robot.left_forward_motor, (encoder_t)roc_robot.left_forward_encoder, (controller_t)roc_robot.left_forward_pid, WHEEL_RADIUS, GEAR_RATIO);
    c_wheels[1] = roc_robot.left_backward_wheel= wheel_create((motor_t)roc_robot.left_backward_motor, (encoder_t)roc_robot.left_backward_encoder, (controller_t)roc_robot.left_backward_pid, WHEEL_RADIUS, GEAR_RATIO);
    c_wheels[2] = roc_robot.right_forward_wheel = wheel_create((motor_t)roc_robot.right_forward_motor, (encoder_t)roc_robot.right_forward_encoder, (controller_t)roc_robot.right_forward_pid, WHEEL_RADIUS, GEAR_RATIO);
    c_wheels[3] = roc_robot.right_backward_wheel = wheel_create((motor_t)roc_robot.right_backward_motor, (encoder_t)roc_robot.right_backward_encoder, (controller_t)roc_robot.right_backward_pid, WHEEL_RADIUS, GEAR_RATIO);

    // 2. Iinialize Kinematics - Two Wheel Differential Drive
    kinematics_t c_kinematics = kinematics_create(FOUR_WD, WHEEL_DIST_X, WHEEL_DIST_Y, WHEEL_RADIUS);

    // 3. Initialize Chassis
    chas = chassis_create(c_wheels, c_kinematics);

    // 4. Enable Chassis
    chassis_enable(chas);

   	// Set Sample time
    encoder_set_sample_time(chas->c_wheels[0]->w_encoder, SAMPLE_TIME);
    encoder_set_sample_time(chas->c_wheels[1]->w_encoder, SAMPLE_TIME);
    encoder_set_sample_time(chas->c_wheels[2]->w_encoder, SAMPLE_TIME);
    encoder_set_sample_time(chas->c_wheels[3]->w_encoder, SAMPLE_TIME);
    
    controller_set_sample_time(chas->c_wheels[0]->w_controller, PID_SAMPLE_TIME);
    controller_set_sample_time(chas->c_wheels[1]->w_controller, PID_SAMPLE_TIME);
    controller_set_sample_time(chas->c_wheels[2]->w_controller, PID_SAMPLE_TIME);
    controller_set_sample_time(chas->c_wheels[3]->w_controller, PID_SAMPLE_TIME);
    
}

void roc_robot_go_forward(void)
{
    LOG_D("roc_robot_go_forward");
    dual_pwm_motor_set_speed(roc_robot.left_forward_motor, roc_robot.speed *10);
    dual_pwm_motor_set_speed(roc_robot.left_backward_motor, roc_robot.speed *10);
    dual_pwm_motor_set_speed(roc_robot.right_forward_motor, roc_robot.speed *10);
    dual_pwm_motor_set_speed(roc_robot.right_backward_motor, roc_robot.speed *10);
    roc_robot.status = E_RIGHT;
}

void roc_robot_go_backward(void)
{
    LOG_D("roc_robot_go_backward");
    dual_pwm_motor_set_speed(roc_robot.left_forward_motor, -roc_robot.speed *10);
    dual_pwm_motor_set_speed(roc_robot.left_backward_motor, -roc_robot.speed *10);
    dual_pwm_motor_set_speed(roc_robot.right_forward_motor, -roc_robot.speed *10);
    dual_pwm_motor_set_speed(roc_robot.right_backward_motor, -roc_robot.speed *10);
    roc_robot.status = E_BACK;
}

void roc_robot_turn_left(void)
{
    LOG_D("roc_robot_turn_left");
    dual_pwm_motor_set_speed(roc_robot.left_forward_motor, roc_robot.speed *10);
    dual_pwm_motor_set_speed(roc_robot.left_backward_motor, roc_robot.speed *10);
    dual_pwm_motor_set_speed(roc_robot.right_forward_motor, roc_robot.speed *10);
    dual_pwm_motor_set_speed(roc_robot.right_backward_motor, roc_robot.speed *10);
    roc_robot.status = E_LEFT;
}

void roc_robot_turn_right(void)
{
    LOG_D("roc_robot_turn_right");
    dual_pwm_motor_set_speed(roc_robot.left_forward_motor, roc_robot.speed *10);
    dual_pwm_motor_set_speed(roc_robot.left_backward_motor, roc_robot.speed *10);
    dual_pwm_motor_set_speed(roc_robot.right_forward_motor, roc_robot.speed *10);
    dual_pwm_motor_set_speed(roc_robot.right_backward_motor, roc_robot.speed *10);
    roc_robot.status = E_RIGHT;
}

void roc_robot_stop(void)
{
    LOG_D("roc_robot_stop");
    dual_pwm_motor_set_speed(roc_robot.left_forward_motor, 0);
    dual_pwm_motor_set_speed(roc_robot.left_backward_motor, 0);
    dual_pwm_motor_set_speed(roc_robot.right_forward_motor, 0);
    dual_pwm_motor_set_speed(roc_robot.right_backward_motor, 0);
    roc_robot.status = E_STOP;
}

void roc_robot_drive(int degree)
{
    LOG_D("roc_robot_drive %d", degree);
}

void roc_robot_set_speed(rt_int16_t s)
{
    LOG_D("roc_robot_set_speed %d", s);
    roc_robot.speed = s;
}

int  roc_robot_get_speed(void)
{
    return roc_robot.speed;
}

void roc_robot_set_status(E_ROC_ROBOT_STATUS s)
{
    roc_robot.status = s;
}

E_ROC_ROBOT_STATUS roc_robot_get_status(void)
{
    return roc_robot.status;
}

