#include "protocol.h"
#include "protocol_parser.h"
#include <rtthread.h>
#include <ringbuffer.h>

#define DBG_SECTION_NAME  "protocol_parser"
#define DBG_LEVEL         DBG_LOG
#include <rtdbg.h>

ST_PROTOCOL_PARSER pareser_package;
ST_PROTOCOL package;
extern struct rt_ringbuffer *tcp_dat;

rt_uint16_t protocol_check_sum(void) {
    rt_uint16_t sum = 0;
    if (pareser_package.recv != NULL) {
        sum += pareser_package.recv->len;
        sum += pareser_package.recv->type;
        sum += pareser_package.recv->addr;
        sum += pareser_package.recv->function;
        for (rt_uint8_t i = 0; i < pareser_package.protocol_data_len; i++) {
            sum += *(pareser_package.recv->data + i);
        }
    }
    return sum;
}

rt_bool_t protocol_recev_data(void)
{
    rt_bool_t avilable = RT_FALSE;
    rt_uint8_t preRecvLen = 0, m_RecvDataIndex = 0;
    rt_uint8_t dat, *p;
    while (rt_ringbuffer_data_len(tcp_dat) != 0) {
        rt_ringbuffer_getchar(tcp_dat, &dat);
       // LOG_D("dat = %x", dat);
        if (avilable == RT_FALSE && dat == PROTOCOL_START_CODE) {
            avilable = RT_TRUE;
            //rt_memset(pareser_package.buffer, 0, BUFFER_SIZE);
            //LOG_D("start recv");
            p = pareser_package.buffer;
            preRecvLen = 0;
            *p++ = dat;
            m_RecvDataIndex = 0;
            continue;
        }
        if (avilable) {
            if (dat == PROTOCOL_END_CODE && preRecvLen == m_RecvDataIndex) {
              //  LOG_D("Recev end");
                *p = dat;
                // m_RecvDataIndex++;
                pareser_package.m_package_length = preRecvLen + 2;
                pareser_package.m_recv_flag = RT_TRUE;
                return RT_TRUE;
           } else {
                *p++ = dat;
                m_RecvDataIndex++;
                if (m_RecvDataIndex == 1) {
                   preRecvLen = dat;
                   if (preRecvLen > BUFFER_SIZE-2) return RT_FALSE;
                }
                if ((m_RecvDataIndex > preRecvLen) && (preRecvLen != 0)) {
                    for (int i = m_RecvDataIndex; i > 0; i--) {
                       LOG_E("error dat = %x", pareser_package.buffer[i]);
                        if (pareser_package.buffer[i] == PROTOCOL_START_CODE) {
                            p = &pareser_package.buffer[i];
                            preRecvLen = pareser_package.buffer[i+1];
                            m_RecvDataIndex -= i;
                            continue;
                        }
                    }
                    avilable = RT_FALSE;
                }
                if (m_RecvDataIndex >= BUFFER_SIZE - 1) {
                    for (int i = 0; i < BUFFER_SIZE; i++) {
                        LOG_E("%x ", pareser_package.buffer[i]);
                    }
                    LOG_E("buffer is over");
                    return RT_FALSE;
                 }
            }
        }
        //rt_thread_delay(5);
    }
    return avilable;
}

//rt_bool_t protocol_parser(rt_uint8_t *data, rt_uint8_t len) {
rt_bool_t protocol_parser(void) {
    rt_uint8_t *p = pareser_package.buffer;
    if (!pareser_package.m_recv_flag)
        return RT_FALSE;

    pareser_package.m_header = p;
    pareser_package.recv->start_code = *p;
    pareser_package.recv->len = *(++p);
    pareser_package.protocol_data_len = pareser_package.recv->len - 6;
    pareser_package.recv->type = (E_ROBOT_TYPE)(*(++p));
    pareser_package.recv->addr = *(++p);
    pareser_package.recv->function = (E_CONTOROL_FUNC)(*(++p));
    pareser_package.recv->data = ++p;
    pareser_package.recv->sum = (*(p + pareser_package.protocol_data_len) << 8) | (*(p + pareser_package.protocol_data_len + 1));
    // p = p + pareser_package.protocol_data_len + 2;  // calulate date point

    if (protocol_check_sum() == pareser_package.recv->sum) {
        return RT_TRUE;
    } else {
        LOG_E("check_sun = %d, sum = %d", protocol_check_sum(), pareser_package.recv->sum);
    }
    return RT_FALSE;
}

void protocol_parser_reinit(void) {
    rt_memset(pareser_package.buffer, 0, BUFFER_SIZE);
    //if (pareser_package.recv != NULL) rt_free(pareser_package.recv);
    pareser_package.recv = &package;
    pareser_package.protocol_data_len = 0;
    pareser_package.m_recv_flag = RT_FALSE;
    pareser_package.m_send_success = RT_FALSE;   // recevive flag
    pareser_package.m_header = NULL;             // protocol header
    pareser_package.m_package_length = 0;        // recevie package length
    pareser_package.m_check_sum = 0;
}

E_ROBOT_TYPE protocol_get_robot_type(void)
{
    return pareser_package.recv->type;
}

rt_uint8_t protocol_get_robot_addr(void)
{
    return pareser_package.recv->addr;
}

E_CONTOROL_FUNC protocol_get_function(void)
{
    return pareser_package.recv->function;
}

rt_int8_t protocol_get_speed(void)
{
    if (pareser_package.recv->function == E_ROBOT_CONTROL_SPEED) {
        return (rt_int8_t)(*(pareser_package.recv->data));
    } else {
        return 0;
    }
}
rt_int16_t protocol_get_degree(void)
{
    if (pareser_package.recv->function == E_ROBOT_CONTROL_DIRECTION) {
        return (uint16_t)(*(pareser_package.recv->data)<< 8) | (uint16_t)(*(pareser_package.recv->data + 1));
    } else {
        return 0;
    }
}

rt_uint16_t protocol_get_button_value(void)
{
    if (pareser_package.recv->function == E_BUTTON) {
        return (uint16_t)(*(pareser_package.recv->data)<< 8) | (uint16_t)(*(pareser_package.recv->data + 1));
    }
    return 0xFFFF;
}

rt_bool_t protocol_get_button(rt_uint16_t button)
{
    return (protocol_get_button_value() == button);
}

rt_uint8_t* protocol_get_cmd_line(void) {
    return NULL;
}

rt_bool_t protocol_send_package(ST_PROTOCOL *send_dat,int len)
{
    return RT_TRUE;
}

rt_uint8_t protocol_get_data_length(void)
{
    return pareser_package.protocol_data_len;
}

rt_uint8_t protocol_get_package_length(void)
{
    return pareser_package.recv->len;
}

