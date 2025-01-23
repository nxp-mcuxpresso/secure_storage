/* Copyright 2025 NXP
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SECURE_STORAGE_ITS_STORE_EXTENSION_H
#define SECURE_STORAGE_ITS_STORE_EXTENSION_H

/** @file store_extension.h The secure storage ITS store extension module.
 *
 * The functions declared in this header implement the ITS store extension module.
 * They are meant to be called only by the ITS implementation.
 * These are to be used for the storage subsystems which allow to read/write at an offset.
 */
#include <zephyr/secure_storage/its/common.h>

/** @brief Writes the data of an ITS entry to the storage medium at required offset
 *
 * @param uid         The entry's UID.
 * @param data_length The number of bytes in `data`.
 * @param data        The data to store.
 * @param data_offset The offset at which the data needs to be stored
 *
 * @return One of the return values of `psa_its_set()`.
 */
psa_status_t secure_storage_its_store_set_at_offset(secure_storage_its_uid_t uid,
                                                    size_t data_length, const void *data,
                                                    uint32_t offset);

/** @brief Retrieves the data of an ITS entry from the storage medium at required offset
 *
 * @param[in]  uid         The entry's UID.
 * @param[in]  data_size   The size of `data` in bytes.
 * @param[in]  data_offset The offset from  which the data needs to be retreived
 * @param[out] data        The buffer to which the entry's stored data is written.
 * @param[out] data_length On success, the number of bytes written to `data`.
 *                         May be less than `data_size`.
 *
 * @return One of the return values of `psa_its_get()`.
 */
psa_status_t secure_storage_its_store_get_at_offset(secure_storage_its_uid_t uid, size_t data_size,
                                                    void *data, size_t *data_length,
                                                    uint32_t data_offset);


#endif
