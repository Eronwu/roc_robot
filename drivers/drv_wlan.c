/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-08-30     ZeroFree     the first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <wlan_mgnt.h>
#include <wlan_prot.h>
#include <wlan_cfg.h>
#include <fal.h>
#include <rt_ota.h>

#include "drv_wlan.h"
#include "drv_gpio.h"

#define DBG_ENABLE
#define DBG_SECTION_NAME    "WLAN"
#define DBG_COLOR
#define DBG_LEVEL           DBG_LOG
#include <rtdbg.h>

#ifdef BSP_USING_WIFI

#define WIFI_IMAGE_PARTITION_NAME       "wifi_image"
#define WIFI_INIT_THREAD_STACK_SIZE      (1024 * 4)
#define WIFI_INIT_THREAD_PRIORITY        (RT_THREAD_PRIORITY_MAX/2)
#define WIFI_INIT_WAIT_TIME              (rt_tick_from_millisecond(100))

extern int wifi_hw_init(void);
extern void wwd_thread_notify_irq(void);

static const struct fal_partition *partition = RT_NULL;
static rt_uint32_t init_flag = 0;

struct rt_wlan_device *bcm_hw_wlan_dev_alloc(void)
{
    struct rt_wlan_device *wlan;

    wlan = rt_malloc(sizeof(struct rt_wlan_device));

    return wlan;
}

int wiced_platform_resource_size(int resource)
{
    int size = 0;

    /* Download firmware */
    if (resource == 0)
    {
        /* initialize fal */
        fal_init();
        partition = fal_partition_find(WIFI_IMAGE_PARTITION_NAME);
        if (partition == RT_NULL)
        {
            LOG_E("%s partition is not exist, please check your configuration!");
            return size;
        }

        if ((rt_ota_init() >= 0) && (rt_ota_part_fw_verify(partition) >= 0))
        {
            size = rt_ota_get_raw_fw_size(partition);
        }
    }

    return size;
}

int wiced_platform_resource_read(int resource, uint32_t offset, void *buffer, uint32_t buffer_size)
{
    int transfer_size = 0;

    if (partition == RT_NULL)
    {
        return 0;
    }

    /* read RF firmware from partition */
    transfer_size = fal_partition_read(partition, offset, buffer, buffer_size);

    return transfer_size;
}

#ifdef RT_USING_PM
void wiced_platform_keep_awake(void)
{
    rt_pm_request(PM_RUN_MODE_NORMAL);
}

void wiced_platform_let_sleep(void)
{
    rt_pm_release(PM_RUN_MODE_NORMAL);
}
#endif

/**
 * return:1 initialize done
 *        0 not initialize
 */
int rt_hw_wlan_get_initialize_status(void)
{
    return init_flag;
}

/**
 * wait milliseconds for wifi low level initialize complete
 * 
 * time_ms: timeout in milliseconds
 */ 
int rt_hw_wlan_wait_init_done(rt_uint32_t time_ms)
{
    rt_uint32_t time_cnt = 0;

    /* wait wifi low level initialize complete */
    while (time_cnt <= (time_ms / 100))
    {
        time_cnt++;
        rt_thread_mdelay(100);
        if (rt_hw_wlan_get_initialize_status() == 1)
        {
            break;
        }
    }

    if (time_cnt > (time_ms / 100))
    {
        return -RT_ETIMEOUT;
    }

    return RT_EOK;
}

static void _wiced_irq_handler(void *param)
{
    wwd_thread_notify_irq();
}

static void wifi_init_thread_entry(void *parameter)
{
    /* set wifi irq handle, must be initialized first */
    rt_pin_mode(PIN_WIFI_IRQ, PIN_MODE_INPUT_PULLUP);
    rt_pin_attach_irq(PIN_WIFI_IRQ, PIN_IRQ_MODE_RISING_FALLING, _wiced_irq_handler, RT_NULL);
    rt_pin_irq_enable(PIN_WIFI_IRQ, PIN_IRQ_ENABLE);

    /* initialize low level wifi(ap6181) library */
    wifi_hw_init();

    /* waiting for sdio bus stability */
    rt_thread_delay(WIFI_INIT_WAIT_TIME);

    /* set wifi work mode */
    rt_wlan_set_mode(RT_WLAN_DEVICE_STA_NAME, RT_WLAN_STATION);

    /* set wifi use lwip protocol */
    rt_wlan_prot_attach(RT_WLAN_DEVICE_STA_NAME, RT_WLAN_PROT_LWIP);
    init_flag = 1;
}

int rt_hw_wlan_init(void)
{
    if (init_flag == 1)
    {
        return RT_EOK;
    }

#ifdef BSP_USING_WIFI_THREAD_INIT
    rt_thread_t tid = RT_NULL;

    tid = rt_thread_create("wifi_init", wifi_init_thread_entry, RT_NULL, WIFI_INIT_THREAD_STACK_SIZE, WIFI_INIT_THREAD_PRIORITY, 20);
    if (tid)
    {
        rt_thread_startup(tid);
    }
    else
    {
        LOG_E("Create wifi initialization thread fail!");
        return -RT_ERROR;
    }
#else
    wifi_init_thread_entry(RT_NULL);
    init_flag = 1;
#endif

    return RT_EOK;
}

#ifdef BSP_USING_WIFI_AUTO_INIT
INIT_APP_EXPORT(rt_hw_wlan_init);
#endif

#endif
