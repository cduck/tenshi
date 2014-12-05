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


// TODO(rqou): This really doesn't go here.
int8_t PiEMOSAnalogVals[7];
uint8_t PiEMOSDigitalVals[8];

int main(int argc, char **argv) {
  // Not useful
  (void) argc;
  (void) argv;

  init_malloc_lock();
  debug_alloc_init();

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
