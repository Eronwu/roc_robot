#include <roc_robot.h>

#define DBG_SECTION_NAME  "roc_robot"
#define DBG_LEVEL         DBG_LOG
#include <rtdbg.h>

ST_ROC_ROBOT roc_robot;
// CAR
chassis_t chas;

void roc_robot_init(void *parameter)
{
     st_dc_motor left_forward_dc_motor = {LEFT_FORWARD_A_PWM, LEFT_FORWARD_B_PWM, LEFT_FORWARD_A_PWM_CHANNEL, LEFT_FORWARD_B_PWM_CHANNEL, RT_NULL, RT_NULL};
     st_dc_motor left_backward_dc_motor = {LEFT_BACKWARD_A_PWM, LEFT_BACKWARD_B_PWM, LEFT_BACKWARD_A_PWM_CHANNEL, LEFT_BACKWARD_B_PWM_CHANNEL, RT_NULL, RT_NULL};
     st_dc_motor right_forward_dc_motor = {RIGHT_FORWARD_A_PWM, RIGHT_FORWARD_B_PWM, RIGHT_FORWARD_A_PWM_CHANNEL, RIGHT_FORWARD_B_PWM_CHANNEL, RT_NULL, RT_NULL};
     st_dc_motor right_backward_dc_motor = {RIGHT_BACKWARD_A_PWM, RIGHT_BACKWARD_B_PWM, RIGHT_BACKWARD_A_PWM_CHANNEL, RIGHT_BACKWARD_B_PWM_CHANNEL, RT_NULL, RT_NULL};

    wheel_t* c_wheels = (wheel_t*) rt_malloc(sizeof(wheel_t) * 4);
    if (c_wheels == RT_NULL)
    {
        LOG_D("Failed to malloc memory for wheels");
    }

    // 1.1 Create two motors
    roc_robot.left_forward_motor = motor_create(DC_MOTOR, (void *)&left_forward_dc_motor);
    roc_robot.left_backward_motor = motor_create(DC_MOTOR, (void *)&left_backward_dc_motor);
    roc_robot.right_forward_motor = motor_create(DC_MOTOR, (void *)&right_forward_dc_motor);
    roc_robot.right_backward_motor = motor_create(DC_MOTOR, (void *)&right_backward_dc_motor);

    // 1.2 Create two encoders
    roc_robot.left_forward_encoder  = encoder_create(LEFT_FORWARD_ENCODER_PIN, PULSE_PER_REVOL);
    roc_robot.left_backward_encoder = encoder_create(LEFT_BACKWARD_ENCODER_PIN, PULSE_PER_REVOL);
    roc_robot.right_forward_encoder  = encoder_create(RIGHT_FORWARD_ENCODER_PIN, PULSE_PER_REVOL);
    roc_robot.right_backward_encoder = encoder_create(RIGHT_BACKWARD_ENCODER_PIN, PULSE_PER_REVOL);

    // 1.3 Create two pid contollers
    roc_robot.left_forward_pid  = pid_create();
    roc_robot.left_backward_pid = pid_create();
    roc_robot.right_forward_pid  = pid_create();
    roc_robot.right_backward_pid = pid_create();

    // 1.4 Add two wheels
    c_wheels[0] = roc_robot.left_forward_wheel = wheel_create(roc_robot.left_forward_motor, roc_robot.left_forward_encoder, roc_robot.left_forward_pid, WHEEL_RADIUS, GEAR_RATIO);
    c_wheels[1] = roc_robot.left_backward_wheel= wheel_create(roc_robot.left_backward_motor, roc_robot.left_backward_encoder, roc_robot.left_backward_pid, WHEEL_RADIUS, GEAR_RATIO);
    c_wheels[2] = roc_robot.right_forward_wheel = wheel_create(roc_robot.right_forward_motor, roc_robot.right_forward_encoder, roc_robot.right_forward_pid, WHEEL_RADIUS, GEAR_RATIO);
    c_wheels[3] = roc_robot.right_backward_wheel = wheel_create(roc_robot.right_backward_motor, roc_robot.right_backward_encoder, roc_robot.right_backward_pid, WHEEL_RADIUS, GEAR_RATIO);

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
    
    pid_set_sample_time(chas->c_wheels[0]->w_pid, PID_SAMPLE_TIME);
    pid_set_sample_time(chas->c_wheels[1]->w_pid, PID_SAMPLE_TIME);
    pid_set_sample_time(chas->c_wheels[2]->w_pid, PID_SAMPLE_TIME);
    pid_set_sample_time(chas->c_wheels[3]->w_pid, PID_SAMPLE_TIME);
}
