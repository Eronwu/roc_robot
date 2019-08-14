#ifndef   _PROTOCOLPARSER_H_
#define   _PROTOCOLPARSER_H_
#include "Protocol.h"

#define BUFFER_SIZE 128

typedef struct {
    rt_uint8_t buffer[BUFFER_SIZE];
    ST_PROTOCOL *recv;
    uint8_t protocol_data_len;
    rt_bool_t m_recv_flag, m_send_success;   // recevive flag
    rt_uint8_t *m_header;                    // protocol header
    uint8_t m_package_length;                // recevie package length
    uint16_t m_check_sum;
    // uint8_t m_recv_data_index;            // get recevie data index
} ST_PROTOCOL_PARSER;

extern ST_PROTOCOL_PARSER pareser_package;
void protocol_parser_reinit(void);
rt_bool_t protocol_recev_data(void);
//rt_bool_t protocol_parser(rt_uint8_t *data, rt_uint8_t len);
rt_bool_t protocol_parser(void);
E_ROBOT_TYPE protocol_get_robot_type(void);
rt_uint8_t protocol_get_robot_addr(void);
E_CONTOROL_FUNC protocol_get_function(void);
rt_int8_t protocol_get_speed(void);
rt_int16_t protocol_get_degree(void);
rt_uint16_t protocol_get_button_value(void);
rt_bool_t protocol_get_button(rt_uint16_t button);
rt_uint8_t* protocol_get_cmd_line(void);
rt_bool_t protocol_send_package(ST_PROTOCOL *send_dat,int len);
rt_uint8_t protocol_get_data_length(void);
rt_uint8_t protocol_get_package_length(void);
rt_uint16_t protocol_check_sum(void);             // get package check sum

#endif // _PROTOCOLPARSER_H_
