#include <rtthread.h>
#include <sys/socket.h>
#include "netdb.h"
#include <ringbuffer.h>

#define BUFFER_SIZE   128
extern struct rt_ringbuffer *tcp_dat;
int connected = -1;
void tcprecvserv(void *parameter)
{
    unsigned char *recv_data; 
    socklen_t sin_size;
    int sock, bytes_received;
    struct sockaddr_in server_addr, client_addr;
    rt_bool_t stop = RT_FALSE;
    int ret;
    int nNetTimeout = 20;
    recv_data = (unsigned char *)rt_malloc(BUFFER_SIZE);
    rt_kprintf("tcpserv start ......\n");
    if (recv_data == RT_NULL)
    {
        rt_kprintf("No memory\n");
        return;
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        rt_kprintf("Socket error\n");

        rt_free(recv_data);
        return;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5000);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    rt_memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout, sizeof(int)); 
    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        rt_kprintf("Unable to bind\n");
        rt_free(recv_data);
        return;
    }

    if (listen(sock, 5) == -1)
    {
        rt_kprintf("Listen error\n");

        /* release recv buffer */
        rt_free(recv_data);
        return;
    }

    rt_kprintf("\nTCPServer Waiting for client on port 5000...\n");
    while (stop != RT_TRUE)
    {
        sin_size = sizeof(struct sockaddr_in);

        rt_kprintf("Listen start = %d\n", connected);
        connected = accept(sock, (struct sockaddr *)&client_addr, &sin_size);
        rt_kprintf("Listen end = %d\n", connected);
        if (connected < 0)
        {
            rt_kprintf("accept connection failed! errno = %d\n", errno);
            continue;
        }

        rt_kprintf("I got a connection from (%s , %d)\n",
        inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        while (1)
        {
           /* ret = send(connected, send_data, strlen(send_data), 0);
            if (ret < 0)
            {
                closesocket(connected);
                rt_kprintf("\nsend error,close the socket.\r\n");
                break;
            } else if (ret == 0) {
                rt_kprintf("\n Send warning,send function return 0.\r\n");
            }
            */
            bytes_received = recv(connected, recv_data, BUFFER_SIZE, MSG_WAITALL);
            if (bytes_received < 0)
            {
                closesocket(connected);
                break;
            } else if (bytes_received == 0) {
                rt_kprintf("\nReceived warning,recv function return 0.\r\n");
                closesocket(connected);
                connected = -1;
                break;
            }
           /*
            recv_data[bytes_received] = '\0';
            if (strcmp(recv_data, "q") == 0 || strcmp(recv_data, "Q") == 0)
            {
                closesocket(connected);
                break;
            } else if (strcmp(recv_data, "exit") == 0) {

                closesocket(connected);
                stop = RT_TRUE;
                break;
            } else {
                rt_kprintf("RECEIVED DATA = %s \n", recv_data);
            } */
          /* rt_kprintf("len=%d\n", bytes_received);
           for(int i = 0; i < bytes_received; i++) {
                rt_kprintf("%x ", *(recv_data+i));
           }*/
           rt_kprintf("RECEIVED DATA = %s \n", recv_data);
           rt_ringbuffer_put_force(tcp_dat, (const rt_uint8_t *)recv_data, bytes_received);
        }
    }

    closesocket(sock);
    rt_ringbuffer_destroy(tcp_dat);
    rt_free(recv_data);
    return ;
}
//MSH_CMD_EXPORT(tcpserv, a tcp server sample);
