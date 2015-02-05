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

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "inc/FreeRTOS.h"
#include "inc/semphr.h"

typedef struct debug_alloc_block {
  void *next;
  uint8_t bytes[];
} debug_alloc_block;

#define DEBUG_ALLOC_BYTES_OFFSET (((debug_alloc_block *) NULL)->bytes)

static SemaphoreHandle_t debug_alloc_mutex;

static void *debug_alloc_first_block = NULL;

void debug_alloc_init(void) {
  debug_alloc_mutex = xSemaphoreCreateRecursiveMutex();
}

void *debug_alloc(size_t size) {
  xSemaphoreTakeRecursive(debug_alloc_mutex, portMAX_DELAY);
  debug_alloc_block *out = (debug_alloc_block *) malloc(size + sizeof(void*));
  if (!out) {
    printf("DEBUG ALLOC ERROR: Out of memory.\n");
    while (true) {}
  }
  out->next = debug_alloc_first_block;
  debug_alloc_first_block = out;
  xSemaphoreGiveRecursive(debug_alloc_mutex);
  return out->bytes;
}

void debug_free(void *ptr) {
  if (!ptr) return;
  xSemaphoreTakeRecursive(debug_alloc_mutex, portMAX_DELAY);
  debug_alloc_block *block = (debug_alloc_block *)
      ((uint8_t *) ptr - DEBUG_ALLOC_BYTES_OFFSET);
  bool found_block = false;
  debug_alloc_block **previous = &debug_alloc_first_block;
  debug_alloc_block *b = debug_alloc_first_block;
  while (!found_block && b != NULL) {
    if (b == block) {
      found_block = true;
    } else {
      previous = b->next;
    }
  }
  if (found_block) {
    *previous = block->next;
  } else {
    printf("DEBUG ALLOC ERROR: Attempt to free non-allocated block.\n");
    while (true) {}
  }
  free(block);
  xSemaphoreGiveRecursive(debug_alloc_mutex);
}
