/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-07-31     tanek        first version
 */

#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>

#include <drv_pmtimer.h>
#include <drv_clock.h>
#include <stm32l4xx_hal.h>

/**
 * This function will put STM32L4xx into run/sleep mode.
 *
 * @param pm pointer to power manage structure
 */
static void _drv_pm_enter(struct rt_pm *pm)
{
    rt_uint32_t mode;

    mode = pm->current_mode;

    switch (mode)
    {
    case PM_RUN_MODE_NORMAL:
        break;

    case PM_SLEEP_MODE_SLEEP:
        HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
        break;

    case PM_SLEEP_MODE_TIMER:
        HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
        rt_update_system_clock();
        break;

    case PM_SLEEP_MODE_SHUTDOWN:
        HAL_PWREx_EnterSHUTDOWNMode();
        break;

    default:
        RT_ASSERT(0);
        break;
    }
}

/**
 * This function will cause STM32L4xx to exit run/sleep mode
 *
 * @param pm pointer to power manage structure
 */
static void _drv_pm_exit(struct rt_pm *pm)
{
    rt_uint32_t mode;

    RT_ASSERT(pm != RT_NULL);

    mode = pm->current_mode;

    switch (mode)
    {
    case PM_RUN_MODE_NORMAL:
        break;

    case PM_SLEEP_MODE_SLEEP:
        break;

    case PM_SLEEP_MODE_TIMER:
        break;

    case PM_SLEEP_MODE_SHUTDOWN:
        break;

    default:
        RT_ASSERT(0);
        break;
    }
}

#if PM_RUN_MODE_COUNT > 1
/**
 * This function will cause STM32L4xx to change the system clock.
 *
 * @param pm pointer to power manage structure
 * @param frequency new frequency to run mode
 */
static void _drv_pm_frequency_change(struct rt_pm *pm, rt_uint32_t frequency)
{
    return;
}
#endif

/**
 * This function caculate the PM tick from OS tick
 *
 * @param tick OS tick
 *
 * @return the PM tick
 */
static rt_tick_t stm32l4_pm_tick_from_os_tick(rt_tick_t tick)
{
    rt_uint32_t freq = stm32l4_lptim_get_countfreq();

    return (freq * tick / RT_TICK_PER_SECOND);
}

/**
 * This function caculate the OS tick from PM tick
 *
 * @param tick PM tick
 *
 * @return the OS tick
 */
static rt_tick_t stm32l4_os_tick_from_pm_tick(rt_uint32_t tick)
{
    static rt_uint32_t os_tick_remain = 0;
    rt_uint32_t ret, freq;

    freq = stm32l4_lptim_get_countfreq();
    ret = (tick * RT_TICK_PER_SECOND + os_tick_remain) / freq;

    os_tick_remain += (tick * RT_TICK_PER_SECOND);
    os_tick_remain %= freq;

    return ret;
}

/**
 * This function start the timer of pm
 *
 * @param pm Pointer to power manage structure
 * @param timeout How many OS Ticks that MCU can sleep
 */
static void _drv_pm_timer_start(struct rt_pm *pm, rt_uint32_t timeout)
{
    RT_ASSERT(pm != RT_NULL);
    RT_ASSERT(timeout > 0);

    if (timeout != RT_TICK_MAX)
    {
        /* Convert OS Tick to pmtimer timeout value */
        timeout = stm32l4_pm_tick_from_os_tick(timeout);
        if (timeout > stm32l4_lptim_get_tick_max())
        {
            timeout = stm32l4_lptim_get_tick_max();
        }

        /* Enter PM_TIMER_MODE */
        stm32l4_lptim_start(timeout);
    }
}

/**
 * This function stop the timer of pm
 *
 * @param pm Pointer to power manage structure
 */
static void _drv_pm_timer_stop(struct rt_pm *pm)
{
    RT_ASSERT(pm != RT_NULL);

    /* Reset pmtimer status */
    stm32l4_lptim_stop();
}

/**
 * This function calculate how many OS Ticks that MCU have suspended
 *
 * @param pm Pointer to power manage structure
 *
 * @return OS Ticks
 */
static rt_tick_t _drv_pm_timer_get_tick(struct rt_pm *pm)
{
    rt_uint32_t timer_tick;

    RT_ASSERT(pm != RT_NULL);

    timer_tick = stm32l4_lptim_get_current_tick();

    return stm32l4_os_tick_from_pm_tick(timer_tick);
}

/**
 * This function initialize the power manager
 */
static int drv_pm_hw_init(void)
{
    static const struct rt_pm_ops _ops =
    {
        _drv_pm_enter,
        _drv_pm_exit,
#if PM_RUN_MODE_COUNT > 1
        _drv_pm_frequency_change,
#endif
        _drv_pm_timer_start,
        _drv_pm_timer_stop,
        _drv_pm_timer_get_tick
    };

    rt_uint8_t timer_mask;

    /* initialize timer mask */
    timer_mask = 1UL << PM_SLEEP_MODE_TIMER;

    /* initialize system pm module */
    rt_system_pm_init(&_ops, timer_mask, RT_NULL);

    return 0;
}
INIT_BOARD_EXPORT(drv_pm_hw_init);
