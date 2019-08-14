/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author            Notes
 * 2017-10-30     ZYH            the first version
 */
#include "drv_usbd.h"
#include <rtthread.h>
#include <rtdevice.h>
#include "board.h"


//#define DBG_ENABLE
#define DBG_SECTION_NAME "USBD"
#define DBG_LEVEL DBG_LOG
#define DBG_COLOR
#include <rtdbg.h>

static PCD_HandleTypeDef _stm_pcd;
static struct udcd _stm_udc;
static struct ep_id _ep_pool[] =
{
    {0x0,  USB_EP_ATTR_CONTROL,     USB_DIR_INOUT,  64, ID_ASSIGNED  },
    {0x1,  USB_EP_ATTR_BULK,        USB_DIR_IN,     64, ID_UNASSIGNED},
    {0x1,  USB_EP_ATTR_BULK,        USB_DIR_OUT,    64, ID_UNASSIGNED},
    {0x2,  USB_EP_ATTR_INT,         USB_DIR_IN,     64, ID_UNASSIGNED},
    {0x2,  USB_EP_ATTR_INT,         USB_DIR_OUT,    64, ID_UNASSIGNED},
    {0x3,  USB_EP_ATTR_BULK,        USB_DIR_IN,     64, ID_UNASSIGNED},
    {0x3,  USB_EP_ATTR_BULK,        USB_DIR_OUT,    64, ID_UNASSIGNED},
    {0xFF, USB_EP_ATTR_TYPE_MASK,   USB_DIR_MASK,   0,  ID_ASSIGNED  },
};

void OTG_FS_IRQHandler(void)
{
    rt_interrupt_enter();
    HAL_PCD_IRQHandler(&_stm_pcd);
    /* leave interrupt */
    rt_interrupt_leave();

}

void HAL_PCD_ResetCallback(PCD_HandleTypeDef *pcd)
{
    /* open ep0 OUT and IN */
    HAL_PCD_EP_Open(pcd, 0x00, 0x40, EP_TYPE_CTRL);
    HAL_PCD_EP_Open(pcd, 0x80, 0x40, EP_TYPE_CTRL);
    rt_usbd_reset_handler(&_stm_udc);
}

void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd)
{
    rt_usbd_ep0_setup_handler(&_stm_udc, (struct urequest *)hpcd->Setup);
}

void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
    if (epnum == 0)
    {
        rt_usbd_ep0_in_handler(&_stm_udc);
    }
    else
    {
        rt_usbd_ep_in_handler(&_stm_udc, 0x80 | epnum, hpcd->IN_ep[epnum].xfer_count);
    }
}

void HAL_PCD_ConnectCallback(PCD_HandleTypeDef *hpcd)
{
    rt_usbd_connect_handler(&_stm_udc);
}

void HAL_PCD_SOFCallback(PCD_HandleTypeDef *hpcd)
{
    rt_usbd_sof_handler(&_stm_udc);
}

void HAL_PCD_DisconnectCallback(PCD_HandleTypeDef *hpcd)
{
    rt_usbd_disconnect_handler(&_stm_udc);
}

void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
    if (epnum != 0)
    {
        rt_usbd_ep_out_handler(&_stm_udc, epnum, hpcd->OUT_ep[epnum].xfer_count);
    }
    else
    {
        rt_usbd_ep0_out_handler(&_stm_udc, hpcd->OUT_ep[0].xfer_count);
    }
}

void HAL_PCDEx_SetConnectionState(PCD_HandleTypeDef *hpcd, uint8_t state)
{
    if (state == 1)
    {
    }
    else
    {
    }
}

void HAL_PCD_MspInit(PCD_HandleTypeDef *pcdHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    if (pcdHandle->Instance == USB_OTG_FS)
    {
        /* USER CODE BEGIN USB_MspInit 0 */
        __HAL_RCC_GPIOA_CLK_ENABLE();
        GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        /* Peripheral clock enable */
        __HAL_RCC_USB_OTG_FS_CLK_ENABLE();

        /* Enable VDDUSB */
        if (__HAL_RCC_PWR_IS_CLK_DISABLED())
        {
            __HAL_RCC_PWR_CLK_ENABLE();
            HAL_PWREx_EnableVddUSB();
            __HAL_RCC_PWR_CLK_DISABLE();
        }
        else
        {
            HAL_PWREx_EnableVddUSB();
        }

        /* Peripheral interrupt init */
        HAL_NVIC_SetPriority(OTG_FS_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(OTG_FS_IRQn);
    }
}

void HAL_PCD_MspDeInit(PCD_HandleTypeDef *pcdHandle)
{
    if (pcdHandle->Instance == USB_OTG_FS)
    {
        /* Peripheral clock disable */
        __HAL_RCC_USB_OTG_FS_CLK_DISABLE();
        /**USB_OTG_FS GPIO Configuration
        PA11     ------> USB_OTG_FS_DM
        PA12     ------> USB_OTG_FS_DP
        */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);

        /* Disable VDDUSB */
        if(__HAL_RCC_PWR_IS_CLK_DISABLED())
        {
        __HAL_RCC_PWR_CLK_ENABLE();
        HAL_PWREx_DisableVddUSB();
        __HAL_RCC_PWR_CLK_DISABLE();
        }
        else
        {
        HAL_PWREx_DisableVddUSB();
        }

        /* Peripheral interrupt Deinit*/
        HAL_NVIC_DisableIRQ(OTG_FS_IRQn);
    }
}

static rt_err_t _ep_set_stall(rt_uint8_t address)
{
    //EP Set Stall
    if(address == 0)
    {
        address = 0x80;
    }
    HAL_PCD_EP_SetStall(&_stm_pcd, address);
    return RT_EOK;
}

static rt_err_t _ep_clear_stall(rt_uint8_t address)
{
    if(address == 0)
    {
        address = 0x80;
    }
    HAL_PCD_EP_ClrStall(&_stm_pcd, address);
    return RT_EOK;
}

static rt_err_t _set_address(rt_uint8_t address)
{
    HAL_PCD_SetAddress(&_stm_pcd, address);
    return RT_EOK;
}

static rt_err_t _set_config(rt_uint8_t address)
{
    return RT_EOK;
}

static rt_err_t _ep_enable(uep_t ep)
{
    RT_ASSERT(ep != RT_NULL);
    RT_ASSERT(ep->ep_desc != RT_NULL);
    HAL_PCD_EP_Open(&_stm_pcd, ep->ep_desc->bEndpointAddress,
                    ep->ep_desc->wMaxPacketSize, ep->ep_desc->bmAttributes);
    return RT_EOK;
}

static rt_err_t _ep_disable(uep_t ep)
{
    RT_ASSERT(ep != RT_NULL);
    RT_ASSERT(ep->ep_desc != RT_NULL);
    HAL_PCD_EP_Close(&_stm_pcd, ep->ep_desc->bEndpointAddress);
    return RT_EOK;
}

static rt_size_t _ep_read(rt_uint8_t address, void *buffer)
{
    rt_size_t size = 0;
    RT_ASSERT(buffer != RT_NULL);
    return size;
}

static rt_size_t _ep_read_prepare(rt_uint8_t address, void *buffer, rt_size_t size)
{
    HAL_PCD_EP_Receive(&_stm_pcd, address, buffer, size);
    return size;
}

static rt_size_t _ep_write(rt_uint8_t address, void *buffer, rt_size_t size)
{
    HAL_PCD_EP_Transmit(&_stm_pcd, address, buffer, size);
    return size;
}

static rt_err_t _ep0_send_status(void)
{
    HAL_PCD_EP_Transmit(&_stm_pcd, 0x00, NULL, 0);
    return RT_EOK;
}

static rt_err_t _suspend(void)
{
    return RT_EOK;
}

static rt_err_t _wakeup(void)
{
    return RT_EOK;
}

static rt_err_t _init(rt_device_t device)
{
    PCD_HandleTypeDef *pcd;
    /* Set LL Driver parameters */
    pcd = (PCD_HandleTypeDef *)device->user_data;

    pcd->Instance = USB_OTG_FS;
    pcd->Init.dev_endpoints = 4; 
    pcd->Init.speed = PCD_SPEED_FULL;
    pcd->Init.dma_enable = ENABLE;
    pcd->Init.ep0_mps = DEP0CTL_MPS_64;
    pcd->Init.phy_itface = PCD_PHY_EMBEDDED;
    pcd->Init.Sof_enable = DISABLE;
    pcd->Init.low_power_enable = DISABLE;
    pcd->Init.lpm_enable = DISABLE;
    pcd->Init.battery_charging_enable = DISABLE;
    pcd->Init.use_dedicated_ep1 = DISABLE;
    pcd->Init.vbus_sensing_enable = DISABLE;
      
    /* Initialize LL Driver */
    HAL_PCD_Init(pcd);
    HAL_PCDEx_SetRxFiFo(pcd, 0x80);
    HAL_PCDEx_SetTxFiFo(pcd, 0, 0x40);
    HAL_PCDEx_SetTxFiFo(pcd, 1, 0x40);
    HAL_PCDEx_SetTxFiFo(pcd, 2, 0x40);
    HAL_PCDEx_SetTxFiFo(pcd, 3, 0x40);
    HAL_PCD_Start(pcd);
    return RT_EOK;
}

const static struct udcd_ops _udc_ops =
{
    _set_address,
    _set_config,
    _ep_set_stall,
    _ep_clear_stall,
    _ep_enable,
    _ep_disable,
    _ep_read_prepare,
    _ep_read,
    _ep_write,
    _ep0_send_status,
    _suspend,
    _wakeup,
};

/**
  * Enable DMA controller clock
  * Configure DMA for memory to memory transfers
  *   hdma_memtomem_dma1_channel1
  */
static rt_err_t stm32l4_DMA_Init(void)
{
    DMA_HandleTypeDef hdma_memtomem_dma1_channel1;

    /* DMA controller clock enable */
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* Configure DMA request hdma_memtomem_dma1_channel1 on DMA1_Channel1 */
    hdma_memtomem_dma1_channel1.Instance = DMA1_Channel1;
    hdma_memtomem_dma1_channel1.Init.Request = DMA_REQUEST_0;
    hdma_memtomem_dma1_channel1.Init.Direction = DMA_MEMORY_TO_MEMORY;
    hdma_memtomem_dma1_channel1.Init.PeriphInc = DMA_PINC_ENABLE;
    hdma_memtomem_dma1_channel1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_memtomem_dma1_channel1.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_memtomem_dma1_channel1.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_memtomem_dma1_channel1.Init.Mode = DMA_NORMAL;
    hdma_memtomem_dma1_channel1.Init.Priority = DMA_PRIORITY_LOW;

    if (HAL_DMA_Init(&hdma_memtomem_dma1_channel1) != HAL_OK)
    {
        return RT_ERROR;
    }
    return RT_EOK;
}

#ifdef RT_USING_DEVICE_OPS
const static struct rt_device_ops _stm_udc_ops =
{
    _init,
    RT_NULL,
    RT_NULL,
    RT_NULL,
    RT_NULL,
    RT_NULL
};
#endif

int stm32l4_usbd_register(void)
{
    rt_memset((void *)&_stm_udc, 0, sizeof(struct udcd));
    _stm_udc.parent.type = RT_Device_Class_USBDevice;
#ifdef RT_USING_DEVICE_OPS
    _stm_udc.parent.ops  = &_stm_udc_ops;
#else
    _stm_udc.parent.init = _init;
#endif
    _stm_udc.parent.user_data = &_stm_pcd;
    _stm_udc.ops = &_udc_ops;
    /* Register endpoint infomation */
    _stm_udc.ep_pool = _ep_pool;
    _stm_udc.ep0.id = &_ep_pool[0];
    rt_device_register((rt_device_t)&_stm_udc, "usbd", 0);

    stm32l4_DMA_Init();

    rt_usb_device_init();

    return RT_EOK;
}
INIT_DEVICE_EXPORT(stm32l4_usbd_register);
