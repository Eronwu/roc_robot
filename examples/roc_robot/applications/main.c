#include <rtthread.h>
#include <board.h>
#include <rtdevice.h>
#include <roc_robot.h>
#include <ringbuffer.h>
#include <protocol_parser.h>

#define DBG_SECTION_NAME  "main"
#define DBG_LEVEL         DBG_LOG
#include <rtdbg.h>


#define APP_CONNECT_LED_PIN           PIN_LED_B

struct rt_ringbuffer *tcp_dat;
extern int wifi_auto_connect(void);
extern void tcprecvserv(void *parameter);
extern int connected;

void led_flash(void *parameter)
{

    while(1) {
        if (connected < 0) {
            LOG_D("led_flash ");
            rt_pin_write(APP_CONNECT_LED_PIN, PIN_LOW);
            rt_thread_mdelay(500);
            rt_pin_write(APP_CONNECT_LED_PIN, PIN_HIGH);
            rt_thread_mdelay(500);
        } else {
            //LOG_D("led_on ");
            rt_pin_write(APP_CONNECT_LED_PIN, PIN_LOW);
            rt_thread_mdelay(1000);
        }
    }
}

void wifi_control(void *parameter)
{
    LOG_D("wifi_control start ......");
    while (1) {
        protocol_recev_data();
        if (protocol_parser()) {  
            switch(protocol_get_function()) {
                case E_ROBOT_CONTROL_DIRECTION:
                    LOG_D("E_ROBOT_CONTROL_DIRECTION");
                   // roc_robot_drive(roc_robot_drive());
                    break;
                case E_ROBOT_CONTROL_SPEED:
                    LOG_D("E_ROBOT_CONTROL_SPEED");
                    roc_robot_set_speed(protocol_get_speed());
                    break;
                case E_BUTTON:
                    LOG_D("E_BUTTON");
                    switch(protocol_get_button_value()) {
                        case BT_PAD_UP:
                            roc_robot_go_forward();
                            break;
                        case BT_PAD_DOWN:
                            roc_robot_go_backward();
                            break;
                        case BT_PAD_LEFT:
                            roc_robot_turn_left();
                            break;
                        case BT_PAD_RIGHT:
                            roc_robot_turn_right();
                            break;
                    }
                    break;
            }

        }
      // if (roc_robot_get_status() != E_STOP)
      // roc_robot_stop();
       protocol_parser_reinit();
       rt_thread_mdelay(5);
    }

}

int main(void)
{
    rt_thread_t ret = RT_NULL;
    tcp_dat = rt_ringbuffer_create(2*BUFFER_SIZE);
    wifi_auto_connect();
    roc_robot_init(0);
    rt_pin_mode(APP_CONNECT_LED_PIN, PIN_MODE_OUTPUT);
    ret = rt_thread_create("led_flash",
                        led_flash, RT_NULL,
                        THREAD_STACK_SIZE,
                        THREAD_PRIORITY, THREAD_TIMESLICE);
     
    if (ret != RT_NULL)
    {
        rt_thread_startup(ret);
    }
    /* ret = rt_thread_create("tcprecvserv",
                        tcprecvserv, (void *)pareser_package.buffer,
                        THREAD_STACK_SIZE,
                        THREAD_PRIORITY, THREAD_TIMESLICE);
     
    if (ret != RT_NULL)
    {
        rt_thread_startup(ret);
    } */
	ret = rt_thread_create("wifi_control",
                        wifi_control, RT_NULL,
                        THREAD_STACK_SIZE,
                        THREAD_PRIORITY, THREAD_TIMESLICE);
    if (ret != RT_NULL)
    {
        rt_thread_startup(ret);
    }

    tcprecvserv((void *)pareser_package.buffer);
}


