/* 
 * Copyright 2025 NXP
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/secure_storage/its/store.h>
#include <errno.h>
#include <stdio.h>

#include "ff.h"

#include <secure_storage_log.h>


#define PSA_ITS_STORAGE_FILENAME_PATTERN "%08x%08x%08x"
enum { NAME_BUF_SIZE = sizeof(CONFIG_SECURE_STORAGE_ITS_STORE_FATFS_PREFIX) - 1
		       + 3 * (sizeof(secure_storage_its_uid_t) + 1) };

static void make_name(secure_storage_its_uid_t uid, char name[static NAME_BUF_SIZE])
{
	snprintf(name, NAME_BUF_SIZE, "%s" PSA_ITS_STORAGE_FILENAME_PATTERN,
		 CONFIG_SECURE_STORAGE_ITS_STORE_FATFS_PREFIX,
                 uid.caller_id,
                 (unsigned)(uid.uid >> 32),
                 (unsigned)(uid.uid & 0xffffffff));
}

psa_status_t secure_storage_its_store_set(secure_storage_its_uid_t uid,
					  size_t data_length, const void *data)
{
	int ret;
	char name[NAME_BUF_SIZE];
        FIL stream;
        FRESULT error;
        psa_status_t status = PSA_ERROR_STORAGE_FAILURE;
        size_t n;
      
	make_name(uid, name);
        error = f_open(&stream, name, (FA_WRITE | FA_READ | FA_CREATE_ALWAYS));
        if (error) {
            if (error != FR_EXIST) {
                return PSA_ERROR_DOES_NOT_EXIST;
            }
        }
        if (data_length != 0) {
            error = f_write(&stream, data, data_length, &n);
            if (error || n != data_length) {
                goto exit;
            }
        }
        status = PSA_SUCCESS;

exit:

        error = f_close(&stream);
        if (status == PSA_SUCCESS && error != 0) {
            status = PSA_ERROR_INSUFFICIENT_STORAGE;
        }

        return status;
}

psa_status_t secure_storage_its_store_get(secure_storage_its_uid_t uid, size_t data_size,
					  void *data, size_t *data_length)
{
	psa_status_t ret;
	char name[NAME_BUF_SIZE];
        FRESULT error;
        size_t n;
        FIL stream;
        
	make_name(uid, name);
        error = f_open(&stream, name, (FA_READ));
        if (error) {
            if (error != FR_EXIST) {
                return PSA_ERROR_DOES_NOT_EXIST;
            }
        }
        
        error = f_read(&stream, data, data_size, &n);
        if ((error)) {
            ret = PSA_ERROR_STORAGE_FAILURE;
        } else {
            *data_length = n;
            ret = PSA_SUCCESS;
        }     
        
	return ret;
}

psa_status_t secure_storage_its_store_remove(secure_storage_its_uid_t uid)
{
	int ret;
	char name[NAME_BUF_SIZE];
        FIL stream;
        FRESULT error;

	make_name(uid, name);      
        
        error = f_open(&stream, name, (FA_WRITE | FA_READ | FA_CREATE_ALWAYS));
        if (error) {
            if (error != FR_EXIST) {
                return PSA_ERROR_DOES_NOT_EXIST;
            }
        }

        f_close(&stream);

        if (f_unlink(name) != FR_OK) {
            return PSA_ERROR_STORAGE_FAILURE;
        }

        return PSA_SUCCESS;
}

/* Create the top level directory for NVM storage */
int sd_its_fs_initialize(void)
{
    FRESULT error;

    error = f_mkdir(_T(CONFIG_SECURE_STORAGE_ITS_STORE_FATFS_PREFIX));
    if (error) {
        if (error != FR_EXIST) {
            return -1;
        }
    }

    return 0;
}
