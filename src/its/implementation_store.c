/* Copyright 2025 NXP
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 *  This file is custom implemetation of PSA ITS APIs using
 *  the headers provided in Zephyr secure storage implementation.
 *  The implementtaion directly calls into store layer without
 *  transforming the data.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <zephyr/secure_storage/its.h>
#include <zephyr/secure_storage/its/store.h>

#include <secure_storage_log.h>

#if !defined(MIN)
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#endif

#define PSA_ITS_MAGIC_STRING "PSA\0ITS\0"
#define PSA_ITS_MAGIC_LENGTH 8

typedef struct {
    uint8_t magic[PSA_ITS_MAGIC_LENGTH];
    uint32_t size;
    psa_storage_create_flags_t flags;
} psa_its_file_header_t;

static void log_failed_operation(const char *operation, const char *preposition, psa_status_t ret)
{
	LOG_ERR("Failed to %s data %s storage. (%d)", operation, preposition, ret);
}

static psa_status_t get_stored_data(
		secure_storage_its_uid_t uid,                
		uint8_t *stored_data,
                size_t stored_data_size,
		size_t *stored_data_len)
{
	psa_status_t ret;

	ret = secure_storage_its_store_get(uid, stored_data_size,
					   stored_data, stored_data_len);
	if (ret != PSA_SUCCESS) {
		if (ret != PSA_ERROR_DOES_NOT_EXIST) {
			log_failed_operation("retrieve", "from", ret);
		}
		return ret;
	}
	return PSA_SUCCESS;
}

static psa_status_t get_entry_info(secure_storage_its_uid_t uid,
                                   size_t *data_len, psa_storage_create_flags_t *create_flags)
{
	psa_status_t ret;
        psa_its_file_header_t header;
	size_t header_len;

	ret = get_stored_data(uid, (uint8_t *)&header, sizeof(header), &header_len);
	if (ret == PSA_SUCCESS) {
               if (memcmp(header.magic, PSA_ITS_MAGIC_STRING,PSA_ITS_MAGIC_LENGTH) != 0) {
                    return PSA_ERROR_DATA_CORRUPT;
                }
		*create_flags = header.flags;
                *data_len = header.size;
	}
	return ret;
}

static bool check_stored_entry(secure_storage_its_uid_t uid, size_t data_length, const void *p_data,
			      psa_storage_create_flags_t create_flags, psa_status_t *ret)
{
	psa_storage_create_flags_t existing_create_flags;
	size_t existing_entry_len;

	*ret = get_entry_info(uid, &existing_entry_len, &existing_create_flags);
	if (*ret != PSA_SUCCESS) {
		/* The entry either doesn't exist or is corrupted. */
		/* Allow overwriting corrupted entries to not be stuck with them forever. */
		return false;
	}
	if (existing_create_flags & PSA_STORAGE_FLAG_WRITE_ONCE) {
		*ret = PSA_ERROR_NOT_PERMITTED;
		return true;
	}
       
	return false;
}


static psa_status_t store_entry(secure_storage_its_uid_t uid, size_t data_length,
				const void *p_data, psa_storage_create_flags_t create_flags)
{
	psa_status_t ret;
	uint8_t stored_data[CONFIG_SECURE_STORAGE_ITS_MAX_DATA_SIZE];
	size_t stored_data_len;
        psa_its_file_header_t header;
        
        memcpy(header.magic, PSA_ITS_MAGIC_STRING, PSA_ITS_MAGIC_LENGTH);
        header.size = data_length;
        header.flags = create_flags;
        
        memcpy(stored_data, (uint8_t *)&header, sizeof(header));        
        stored_data_len = sizeof(header);
        
        memcpy(stored_data +  sizeof(header), p_data, data_length);        
        stored_data_len += data_length;

	ret = secure_storage_its_store_set(uid, stored_data_len, stored_data);
	if (ret != PSA_SUCCESS) {
		log_failed_operation("write", "to", ret);
	}
	return ret;
}


psa_status_t secure_storage_its_get_info(secure_storage_its_uid_t uid,
					 struct psa_storage_info_t *p_info)
{
	psa_status_t ret;
        psa_its_file_header_t header;
        size_t data_len;
        
	if (uid.uid == 0 || p_info == NULL) {
		return PSA_ERROR_INVALID_ARGUMENT;
	}

	ret = get_stored_data(uid, (uint8_t *)&header, sizeof(header), &data_len);
	if (ret == PSA_SUCCESS) {
               if (memcmp(header.magic, PSA_ITS_MAGIC_STRING,PSA_ITS_MAGIC_LENGTH) != 0) {
                    return PSA_ERROR_DATA_CORRUPT;
                }
		p_info->size = header.size;
                p_info->flags = header.flags;
	}
	return ret;
}

psa_status_t secure_storage_its_set(secure_storage_its_uid_t uid, size_t data_length,
				    const void *p_data, psa_storage_create_flags_t create_flags)
{
	psa_status_t ret;

	if (uid.uid == 0 || (p_data == NULL && data_length != 0)) {
		return PSA_ERROR_INVALID_ARGUMENT;
	}
	if (create_flags & ~SECURE_STORAGE_ALL_CREATE_FLAGS) {
		return PSA_ERROR_NOT_SUPPORTED;
	}
	if (data_length + sizeof(psa_its_file_header_t) > CONFIG_SECURE_STORAGE_ITS_MAX_DATA_SIZE) {
		LOG_DBG("Passed data length (%zu) exceeds maximum allowed (%u).",
			data_length, CONFIG_SECURE_STORAGE_ITS_MAX_DATA_SIZE);
		return PSA_ERROR_INSUFFICIENT_STORAGE;
	}
       
        /* Check for flags if entry already exist. Is overwrite permitted */
	if (check_stored_entry(uid, data_length, p_data, create_flags, &ret)) {
		return ret;
	}
        
   	ret = store_entry(uid, data_length, p_data, create_flags);
	
        return ret;
}

psa_status_t secure_storage_its_get(secure_storage_its_uid_t uid, size_t data_offset,
				    size_t data_size, void *p_data, size_t *p_data_length)
{
	if (uid.uid == 0 || (p_data == NULL && data_size != 0) || p_data_length == NULL) {
		return PSA_ERROR_INVALID_ARGUMENT;
	}
	if (data_size == 0) {
		*p_data_length = 0;
		return PSA_SUCCESS;
	}
        if (data_offset + data_size < data_offset) {
                return PSA_ERROR_INVALID_ARGUMENT;
        }
	psa_status_t ret;
        psa_storage_create_flags_t create_flags;
	size_t entry_len;
        uint8_t stored_data[CONFIG_SECURE_STORAGE_ITS_MAX_DATA_SIZE];
	size_t stored_data_len;

        ret = get_entry_info(uid, &entry_len, &create_flags);
        if (ret != PSA_SUCCESS)
            return ret;
        
        /* Check if offset is beyond the data in ITS entry */
        if (data_offset > entry_len) {
                return PSA_ERROR_INVALID_ARGUMENT;
        }
        
        /* Check  if requetsed data is more than data in ITS entry */
        if (data_offset + data_size > entry_len) {
                return PSA_ERROR_INVALID_ARGUMENT;
        }

        /* Get all the data of  ITS entry in buffer on stack */
        ret = get_stored_data(uid, stored_data, CONFIG_SECURE_STORAGE_ITS_MAX_DATA_SIZE, &stored_data_len);
	if (ret != PSA_SUCCESS) {
		return ret;
	}
        
        *p_data_length = MIN(data_size, stored_data_len - sizeof(psa_its_file_header_t) - data_offset);
        
        /* The returned data has PSA ITS header in beginning. Copy from after the header and offset needed */
        memcpy(p_data, stored_data + sizeof(psa_its_file_header_t) + data_offset, *p_data_length);
        	
	return ret;
}


psa_status_t secure_storage_its_remove(secure_storage_its_uid_t uid)
{
	psa_status_t ret;
	psa_storage_create_flags_t create_flags;
	size_t data_len;

	if (uid.uid == 0) {
		return PSA_ERROR_INVALID_ARGUMENT;
	}

	ret = get_entry_info(uid, &data_len, &create_flags);
	if (ret == PSA_SUCCESS && (create_flags & PSA_STORAGE_FLAG_WRITE_ONCE)) {
		return PSA_ERROR_NOT_PERMITTED;
	}
	/* Allow overwriting corrupted entries as well to not be stuck with them forever. */
	if (ret == PSA_SUCCESS || ret == PSA_ERROR_STORAGE_FAILURE) {
		ret = secure_storage_its_store_remove(uid);
		if (ret != PSA_SUCCESS) {
			log_failed_operation("remove", "from", ret);
			return PSA_ERROR_STORAGE_FAILURE;
		}
	}
	return ret;
}
