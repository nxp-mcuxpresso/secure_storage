/* Copyright 2026 NXP
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SECURE_STORAGE_LFS_H
#define SECURE_STORAGE_LFS_H

/** @file secure_storage_lfs.h The secure storage Little fs ITS initialization.
 *
 * The functions declared in this header implement helper functions of underlying
 * store. 
 */
#include <zephyr/secure_storage/its/common.h>

#if defined(SDK_OS_FREE_RTOS)
#include "fsl_os_abstraction.h"
#endif /* SDK_OS_FREE_RTOS */

psa_status_t secure_storage_notify_lfs_handler(void *lfs_handler);

#if defined(SDK_OS_FREE_RTOS)
/** @brief Notifies secure storage module about a mutex to be used with little fs module.
 * It is important to call this function before @fn secure_storage_notify_lfs_handler.
 *
 * @param[in] lfs_app_mutex   Little fs mutex that has been created by an application.
 */
void secure_storage_notify_lfs_mutex(osa_mutex_handle_t lfs_app_mutex);
#endif /* SDK_OS_FREE_RTOS */
#endif
