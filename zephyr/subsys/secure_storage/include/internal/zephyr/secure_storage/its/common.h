/* 
 * Copyright (c) 2024 Nordic Semiconductor
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * \file
 *
 * This source code is imported from Zephyr project.
 * Note a few include modifications were required to make it compile in a non
 * Zephyr environment.
 *
 */

#ifndef SECURE_STORAGE_ITS_COMMON_H
#define SECURE_STORAGE_ITS_COMMON_H

/** @file zephyr/secure_storage/its/common.h
 * @brief Common definitions of the secure storage subsystem's ITS APIs.
 */
#include "../common.h"
#if defined(ZEPHYR_SDK)
#include <zephyr/toolchain.h>
#endif

#include <psa/storage_common.h>

/** @brief The ID of the caller from which the ITS API call originates.
 * This is used to prevent ID collisions between different callers that are not aware
 * of each other and so might use the same numerical IDs, e.g. PSA Crypto and PSA ITS.
 */
typedef enum {
	SECURE_STORAGE_ITS_CALLER_PSA_ITS,
	SECURE_STORAGE_ITS_CALLER_PSA_PS,
	SECURE_STORAGE_ITS_CALLER_MBEDTLS,
	SECURE_STORAGE_ITS_CALLER_COUNT
} secure_storage_its_caller_id_t;

/** The UID (caller + entry IDs) of an ITS entry. */
typedef struct {
	psa_storage_uid_t uid;
	secure_storage_its_caller_id_t caller_id;
} __attribute__((packed)) secure_storage_its_uid_t;     //NXP
#endif
