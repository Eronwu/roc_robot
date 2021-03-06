#include <roc_robot.h>
#include <math.h>

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
    roc_robot.left_forward_motor = single_pwm_motor_create(LEFT_FORWARD_PWM, LEFT_FORWARD_PWM_CHANNEL, LEFT_FORWARD_GPIO_PIN_IN1, LEFT_FORWARD_GPIO_PIN_IN2);
    roc_robot.left_backward_motor = single_pwm_motor_create(LEFT_BACKWARD_PWM, LEFT_BACKWARD_PWM_CHANNEL, LEFT_BACKWARD_GPIO_PIN_IN1, LEFT_BACKWARD_GPIO_PIN_IN2);
    roc_robot.right_forward_motor = single_pwm_motor_create(RIGHT_FORWARD_PWM, RIGHT_FORWARD_PWM_CHANNEL, RIGHT_FORWARD_GPIO_PIN_IN1, RIGHT_FORWARD_GPIO_PIN_IN2);
    roc_robot.right_backward_motor = single_pwm_motor_create(RIGHT_BACKWARD_PWM, RIGHT_BACKWARD_PWM_CHANNEL, RIGHT_BACKWARD_GPIO_PIN_IN1, RIGHT_BACKWARD_GPIO_PIN_IN2);


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
    kinematics_t c_kinematics = kinematics_create(MECANUM, WHEEL_DIST_X, WHEEL_DIST_Y, WHEEL_RADIUS);

    // 3. Initialize Chassis
    chas = chassis_create(c_wheels, c_kinematics);

    roc_robot_stop();

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
    single_pwm_motor_set_speed(roc_robot.left_forward_motor, roc_robot.speed *10);
    single_pwm_motor_set_speed(roc_robot.left_backward_motor, roc_robot.speed *10);
    single_pwm_motor_set_speed(roc_robot.right_forward_motor, roc_robot.speed *10);
    single_pwm_motor_set_speed(roc_robot.right_backward_motor, roc_robot.speed *10);
    roc_robot.status = E_FORWARD;
}

void roc_robot_go_backward(void)
{
    LOG_D("roc_robot_go_backward");
    single_pwm_motor_set_speed(roc_robot.left_forward_motor, -roc_robot.speed *10);
    single_pwm_motor_set_speed(roc_robot.left_backward_motor, -roc_robot.speed *10);
    single_pwm_motor_set_speed(roc_robot.right_forward_motor, -roc_robot.speed *10);
    single_pwm_motor_set_speed(roc_robot.right_backward_motor, -roc_robot.speed *10);
    roc_robot.status = E_BACK;
}

void roc_robot_turn_right(void)
{
    LOG_D("roc_robot_turn_right");
    single_pwm_motor_set_speed(roc_robot.left_forward_motor, roc_robot.speed *10);
    single_pwm_motor_set_speed(roc_robot.left_backward_motor, -roc_robot.speed *10);
    single_pwm_motor_set_speed(roc_robot.right_forward_motor, -roc_robot.speed *10);
    single_pwm_motor_set_speed(roc_robot.right_backward_motor, roc_robot.speed *10);
    roc_robot.status = E_RIGHT;
}

void roc_robot_turn_right_rotate(void)
{
    LOG_D("roc_robot_turn_right_rotate");
    single_pwm_motor_set_speed(roc_robot.left_forward_motor, roc_robot.speed *10);
    single_pwm_motor_set_speed(roc_robot.left_backward_motor, roc_robot.speed *10);
    single_pwm_motor_set_speed(roc_robot.right_forward_motor, -roc_robot.speed *10);
    single_pwm_motor_set_speed(roc_robot.right_backward_motor, -roc_robot.speed *10);
    roc_robot.status = E_RIGHT_ROTATE;
}

void roc_robot_turn_left(void)
{
    LOG_D("roc_robot_turn_left");
    single_pwm_motor_set_speed(roc_robot.left_forward_motor, -roc_robot.speed *10);
    single_pwm_motor_set_speed(roc_robot.left_backward_motor, roc_robot.speed *10);
    single_pwm_motor_set_speed(roc_robot.right_forward_motor, roc_robot.speed *10);
    single_pwm_motor_set_speed(roc_robot.right_backward_motor, -roc_robot.speed *10);
    roc_robot.status = E_LEFT;
}

void roc_robot_turn_left_rotate(void)
{
    LOG_D("roc_robot_turn_left_rotate");
    single_pwm_motor_set_speed(roc_robot.left_forward_motor, -roc_robot.speed *10);
    single_pwm_motor_set_speed(roc_robot.left_backward_motor, -roc_robot.speed *10);
    single_pwm_motor_set_speed(roc_robot.right_forward_motor, roc_robot.speed *10);
    single_pwm_motor_set_speed(roc_robot.right_backward_motor, roc_robot.speed *10);
    roc_robot.status = E_LEFT_ROTATE;
}


void roc_robot_stop(void)
{
    LOG_D("roc_robot_stop");
    single_pwm_motor_set_speed(roc_robot.left_forward_motor, 0);
    single_pwm_motor_set_speed(roc_robot.left_backward_motor, 0);
    single_pwm_motor_set_speed(roc_robot.right_forward_motor, 0);
    single_pwm_motor_set_speed(roc_robot.right_backward_motor, 0);
    roc_robot.status = E_STOP;
}

void roc_robot_run(rt_int16_t x, rt_int16_t y, rt_int16_t rotate)
{
    rt_int16_t lf_speed = x + y + rotate; 
    rt_int16_t lb_speed = -x + y + rotate;
    rt_int16_t rf_speed = -x + y -rotate;
    rt_int16_t rb_speed = x + y - rotate;
    single_pwm_motor_set_speed(roc_robot.left_forward_motor, lf_speed *10);
    single_pwm_motor_set_speed(roc_robot.left_backward_motor, lb_speed *10);
    single_pwm_motor_set_speed(roc_robot.right_forward_motor, rf_speed *10);
    single_pwm_motor_set_speed(roc_robot.right_backward_motor, rb_speed *10);
}

void roc_robot_drive(rt_uint16_t degree)
{
    LOG_D("roc_robot_drive %d", degree);
    rt_int16_t x, y;


    if (degree == 0XFFFF) {
        roc_robot_stop();
    } else {
        x = cos(degree)*roc_robot.speed;
        y = sin(degree)*roc_robot.speed;
        roc_robot_run(x, y, 0);
    }
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

