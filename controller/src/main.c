// Licensed to Pioneers in Engineering under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  Pioneers in Engineering licenses
// this file to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
//  with the License.  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License

#include <stdio.h>
#include <string.h>

// USB
#include "usbd_core.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if_template.h"
#include "usbd_desc.h"

// Interpreter
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <ngl_vm.h>
#include <ngl_buffer.h>
#include <ngl_package.h>

#include "inc/radio.h"
#include "inc/runtime.h"
#include "inc/runtime_interface.h"

#include "inc/FreeRTOS.h"
#include "inc/button_driver.h"
#include "inc/driver_glue.h"
#include "inc/i2c_master.h"
#include "inc/led_driver.h"
#include "inc/malloc_lock.h"
#include "inc/pindef.h"
#include "inc/stm32f4xx.h"
#include "inc/core_cm4.h"
#include "inc/core_cmInstr.h"
#include "inc/task.h"
#include "inc/xbee_framing.h"
#include "inc/smartsensor/smartsensor.h"

#include "legacy_piemos_framing.h"   // NOLINT(build/include)
#include "ngl_types.h"   // NOLINT(build/include)


USBD_HandleTypeDef USBD_Device;


// TODO(rqou): This really doesn't go here.
int8_t PiEMOSAnalogVals[7];
uint8_t PiEMOSDigitalVals[8];

static void SystemClock_Config(void)
{
  /*RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  __PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK |
                                 RCC_CLOCKTYPE_HCLK |
                                 RCC_CLOCKTYPE_PCLK1 |
                                 RCC_CLOCKTYPE_PCLK2);

  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);*/
}


int main(int argc, char **argv) {
  // Not useful
  (void) argc;
  (void) argv;

  init_malloc_lock();
  debug_alloc_init();

  // USB Init
  //HAL_Init();
  SystemClock_Config();
  //USBD_Init(&USBD_Device, &VCP_Desc, 0);
  //
  //USBD_RegisterClass(&USBD_Device, &USBD_CDC);
  //USBD_CDC_RegisterInterface(&USBD_Device,
  //                           &USBD_CDC_Template_fops);
  //USBD_Start(&USBD_Device);

  // Setup I2C
  // i2c1_init();

  radioInit();

  led_driver_init();
  button_driver_init();

  // Setup SmartSensors
  smartsensor_init();

  runtimeInit();

  vTaskStartScheduler();
}


caddr_t _sbrk(int increment) {
  extern char end asm("end");
  register char *pStack asm("sp");

  static char *s_pHeapEnd;

  if (!s_pHeapEnd)
    s_pHeapEnd = &end;

  if (s_pHeapEnd + increment > pStack)
    return (caddr_t)-1;

  char *pOldHeapEnd = s_pHeapEnd;
  s_pHeapEnd += increment;
  return (caddr_t)pOldHeapEnd;
}

//xtern PCD_HandleTypeDef hpcd;

void OTG_FS_IRQHandler(void) {
  //HAL_PCD_IRQHandler(&hpcd);
}
