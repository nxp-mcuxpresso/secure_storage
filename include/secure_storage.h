/* Copyright 2025 NXP
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SECURE_STORAGE_H
#define SECURE_STORAGE_H

/** @file secure_storage.h The secure storage ITS initialization.
 *
 * The funcion declared in this header implements the initialization of underlying
 * store. 
 */
#include <zephyr/secure_storage/its/common.h>

/** @brief Initalize the secure storage.
 *
 * @return PSA_SUCCESS on success and PSA_ERROR_STORAGE_FAILURE on failure.
 */
psa_status_t secure_storage_its_initialize(void);

#endif
