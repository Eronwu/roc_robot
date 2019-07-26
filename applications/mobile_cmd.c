#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <chassis.h>
#include <stdio.h>
#include <stdlib.h>

float stof(const char *s)
{
    float rez = 0, fact = 1;
    if (*s == '-')
    {
        s++;
        fact = -1;
    };
    for (int point_seen = 0; *s; s++)
    {
        if (*s == '.')
        {
            point_seen = 1;
            continue;
        };
        int d = *s - '0';
        if (d >= 0 && d <= 9)
        {
            if (point_seen)
                fact /= 10.0f;
            rez = rez * 10.0f + (float)d;
        };
    };
    return rez * fact;
};

extern chassis_t chas;

static void print_help()
{
    rt_kprintf("Usage: mobile_robot [x] [y] [w] [duration]\n");
}

static void mobile_robot(int argc, char *argv[])
{
    if (argc < 5)
    {
        print_help();
        return;
    }
    float linear_x = stof(argv[1]);
    float linear_y = stof(argv[2]);
    float angular_z = stof(argv[3]);
    int duration = atoi(argv[4]);

    struct velocity target_vel;

    target_vel.linear_x = linear_x;   // m/s
    target_vel.linear_y = linear_y;   // m/s
    target_vel.angular_z = angular_z; // rad/s

    chassis_set_velocity(chas, target_vel);
    if (duration > 0)
    {
        rt_thread_mdelay(duration);

        target_vel.linear_x = 0;  // m/s
        target_vel.linear_y = 0;  // m/s
        target_vel.angular_z = 0; // rad/s

        chassis_set_velocity(chas, target_vel);
    }
}
MSH_CMD_EXPORT(mobile_robot, move mobile robot);
