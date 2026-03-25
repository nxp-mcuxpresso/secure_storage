/* 
 * Copyright 2025 NXP
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <stdio.h>

#include <zephyr/secure_storage/its/store.h>

#include <secure_storage_log.h>
#include <secure_storage_lfs.h>

#include "lfs.h"

extern int lfs_get_default_config(struct lfs_config *lfsc);
extern int lfs_storage_init(const struct lfs_config *lfsc);

lfs_t lfs;
struct lfs_config cfg;
int lfs_mounted;

#if defined(SDK_OS_FREE_RTOS)
static osa_mutex_handle_t lfs_mutex;
static bool lfs_mutex_present;
#endif /* SDK_OS_FREE_RTOS */

#ifdef CONFIG_SECURE_STORAGE_64_BIT_UID
#define PSA_ITS_STORAGE_FILENAME_PATTERN "%08x%08x%08x"
#define PSA_ITS_UID_HEX_LENGTH (8 + 8)  // Two 8-digit hex fields for 64-bit UID
#else
#define PSA_ITS_STORAGE_FILENAME_PATTERN "%08x%08x"
#define PSA_ITS_UID_HEX_LENGTH 8  // One 8-digit hex field for 32-bit UID
#endif
 
// For 64-bit: 3 + 1 + 8 + 16 + 1 = 29 bytes
// For 32-bit: 3 + 1 + 8 + 8 + 1 = 21 bytes
enum {
    NAME_BUF_SIZE = sizeof(CONFIG_SECURE_STORAGE_ITS_STORE_LITTLEFS_PREFIX) - 1  // "its" = 3
                  + 1                                                             // "/"
                  + 8                                                             // caller_id (8 hex digits)
                  + PSA_ITS_UID_HEX_LENGTH                                       // uid (16 or 8 hex digits)
                  + 1                                                             // '\0'
};
 
static void make_name(secure_storage_its_uid_t uid, char name[static NAME_BUF_SIZE])
{
#ifdef CONFIG_SECURE_STORAGE_64_BIT_UID
    snprintf(name, NAME_BUF_SIZE, "%s/" PSA_ITS_STORAGE_FILENAME_PATTERN,
             CONFIG_SECURE_STORAGE_ITS_STORE_LITTLEFS_PREFIX,
             uid.caller_id,
             (unsigned)(uid.uid >> 32),
             (unsigned)(uid.uid & 0xffffffff));
#else
    snprintf(name, NAME_BUF_SIZE, "%s/" PSA_ITS_STORAGE_FILENAME_PATTERN,
             CONFIG_SECURE_STORAGE_ITS_STORE_LITTLEFS_PREFIX,
             uid.caller_id,
             (unsigned)uid.uid);
#endif
}

psa_status_t secure_storage_its_store_set(secure_storage_its_uid_t uid,
					  size_t data_length, const void *data)
{
	int res;
        lfs_file_t file;
	char name[NAME_BUF_SIZE];
        psa_status_t status = PSA_ERROR_STORAGE_FAILURE;

        if (!lfs_mounted)
        {
            LOG_ERR("LFS not mounted\r\n");
            return PSA_ERROR_STORAGE_FAILURE;
        }

	make_name(uid, name);
 
#if defined(SDK_OS_FREE_RTOS)
        if (lfs_mutex_present)
        {
                (void)OSA_MutexLock((osa_mutex_handle_t)lfs_mutex, osaWaitForever_c);
        }
#endif /* SDK_OS_FREE_RTOS */
        res = lfs_file_open(&lfs, &file, name, LFS_O_WRONLY | LFS_O_CREAT);

        if (res)
        {
            LOG_DBG("\rError opening file: %i\r\n", res);
            status = PSA_ERROR_INSUFFICIENT_STORAGE;
            goto exit;
        }

        res = lfs_file_write(&lfs, &file, data, data_length);
        if (res < 0)
        {
            LOG_ERR("\rError writing file: %i\r\n", res);
            status = PSA_ERROR_STORAGE_FAILURE;
            goto exit;
        }

        status = PSA_SUCCESS;
exit:        
        res = lfs_file_close(&lfs, &file);
        if (res)
        {
            LOG_ERR("\rError closing file: %i\r\n", res);
            status = PSA_ERROR_STORAGE_FAILURE;
        }
#if defined(SDK_OS_FREE_RTOS)
        if (lfs_mutex_present)
        {
            OSA_MutexUnlock((osa_mutex_handle_t)lfs_mutex);
        }
#endif /* SDK_OS_FREE_RTOS */
        
        return status;
}

psa_status_t secure_storage_its_store_get(secure_storage_its_uid_t uid, size_t data_size,
					  void *data, size_t *data_length)
{
        int res;
        lfs_file_t file;
	char name[NAME_BUF_SIZE];
        psa_status_t status = PSA_SUCCESS;

        if (!lfs_mounted)
        {
            LOG_ERR("LFS not mounted\r\n");
            return PSA_ERROR_STORAGE_FAILURE;
        }

  	make_name(uid, name);
#if defined(SDK_OS_FREE_RTOS)
        if (lfs_mutex_present)
        {
                (void)OSA_MutexLock((osa_mutex_handle_t)lfs_mutex, osaWaitForever_c);
        }
#endif /* SDK_OS_FREE_RTOS */
        res = lfs_file_open(&lfs, &file, name, LFS_O_RDONLY);
        if (res)
        {
            LOG_DBG("\rError opening file: %i\r\n", res);
            status = PSA_ERROR_DOES_NOT_EXIST;
            goto exit;
        }

        res = lfs_file_read(&lfs, &file, data, data_size);
        if (res < 0)
        {
            LOG_ERR("\rError reading file: %i\r\n", res);
            status =  PSA_ERROR_STORAGE_FAILURE;
            goto exit;
        }

        *data_length = res;
        
        res = lfs_file_close(&lfs, &file);
        if (res)
        {
            LOG_ERR("\rError closing file: %i\r\n", res);
            status = PSA_ERROR_STORAGE_FAILURE;
	    goto exit;
        }      
exit:
#if defined(SDK_OS_FREE_RTOS)
        if (lfs_mutex_present)
        {
            OSA_MutexUnlock((osa_mutex_handle_t)lfs_mutex);
        }
#endif /* SDK_OS_FREE_RTOS */  
        
        return status;
}

psa_status_t secure_storage_its_store_set_at_offset(secure_storage_its_uid_t uid,
                                                    size_t data_length, const void *data, uint32_t data_offset)
{
	int res;
        lfs_file_t file;
	char name[NAME_BUF_SIZE];
        psa_status_t status = PSA_ERROR_STORAGE_FAILURE;

        if (!lfs_mounted)
        {
            LOG_ERR("LFS not mounted\r\n");
            return PSA_ERROR_STORAGE_FAILURE;
        }

	make_name(uid, name);
#if defined(SDK_OS_FREE_RTOS)
        if (lfs_mutex_present)
        {
                (void)OSA_MutexLock((osa_mutex_handle_t)lfs_mutex, osaWaitForever_c);
        }
#endif /* SDK_OS_FREE_RTOS */
        res = lfs_file_open(&lfs, &file, name, LFS_O_WRONLY | LFS_O_CREAT);
        if (res)
        {
            LOG_DBG("\rError opening file: %i\r\n", res);
            status = PSA_ERROR_INSUFFICIENT_STORAGE;
            goto exit;
        }
        
        if (data_offset != 0)
        {
              res = lfs_file_seek(&lfs, &file, data_offset, LFS_SEEK_CUR);
              if (res < 0)
              {
                  status = PSA_ERROR_STORAGE_FAILURE;
                  goto exit;
              }
        }

        res = lfs_file_write(&lfs, &file, data, data_length);
        if (res < 0)
        {
            LOG_ERR("\rError writing file: %i\r\n", res);
            status = PSA_ERROR_STORAGE_FAILURE;
            goto exit;
        }

        status = PSA_SUCCESS;
exit:        
        res = lfs_file_close(&lfs, &file);
        if (res)
        {
            LOG_ERR("\rError closing file: %i\r\n", res);
            status = PSA_ERROR_STORAGE_FAILURE;
        }
#if defined(SDK_OS_FREE_RTOS)
        if (lfs_mutex_present)
        {
            OSA_MutexUnlock((osa_mutex_handle_t)lfs_mutex);
        }
#endif /* SDK_OS_FREE_RTOS */
        
        return status;
}

psa_status_t secure_storage_its_store_get_from_offset(secure_storage_its_uid_t uid, size_t data_size,
                                                       void *data, size_t *data_length, uint32_t data_offset)
{
        int res;
        lfs_file_t file;
	char name[NAME_BUF_SIZE];
        psa_status_t status = PSA_SUCCESS;

        if (!lfs_mounted)
        {
            LOG_ERR("LFS not mounted\r\n");
            return PSA_ERROR_STORAGE_FAILURE;
        }

  	make_name(uid, name);
#if defined(SDK_OS_FREE_RTOS)
        if (lfs_mutex_present)
        {
            (void)OSA_MutexLock((osa_mutex_handle_t)lfs_mutex, osaWaitForever_c);
        }
#endif /* SDK_OS_FREE_RTOS */
        res = lfs_file_open(&lfs, &file, name, LFS_O_RDONLY);
        if (res)
        {
            LOG_DBG("\rError opening file: %i\r\n", res);
            status = PSA_ERROR_DOES_NOT_EXIST;
            goto exit;
        }
        
        if (data_offset != 0)
        {
              res = lfs_file_seek(&lfs, &file, data_offset, LFS_SEEK_CUR);
              if (res < 0)
              {
                  status = PSA_ERROR_STORAGE_FAILURE;
                  goto exit;
              }
        }

        res = lfs_file_read(&lfs, &file, data, data_size);
        if (res < 0)
        {
            LOG_ERR("\rError reading file: %i\r\n", res);
            status = PSA_ERROR_STORAGE_FAILURE;
            goto exit;
        }

        *data_length = res;
        
        res = lfs_file_close(&lfs, &file);
        if (res)
        {
            LOG_ERR("\rError closing file: %i\r\n", res);
            status = PSA_ERROR_STORAGE_FAILURE;
	    goto exit;
        }        
exit:
#if defined(SDK_OS_FREE_RTOS)
        if (lfs_mutex_present)
        {
            OSA_MutexUnlock((osa_mutex_handle_t)lfs_mutex);
        }
#endif /* SDK_OS_FREE_RTOS */
        
        return status;
}

psa_status_t secure_storage_its_store_remove(secure_storage_its_uid_t uid)
{
	int res;
	char name[NAME_BUF_SIZE];
	make_name(uid, name);
        psa_status_t status = PSA_ERROR_STORAGE_FAILURE;
        
        if (!lfs_mounted)
        {
            LOG_ERR("LFS not mounted\r\n");
            return PSA_ERROR_STORAGE_FAILURE;
        }
#if defined(SDK_OS_FREE_RTOS)
        if (lfs_mutex_present)
        {
                (void)OSA_MutexLock((osa_mutex_handle_t)lfs_mutex, osaWaitForever_c);
        }
#endif /* SDK_OS_FREE_RTOS */
        res = lfs_remove(&lfs, name);
        if (res)
        {
            LOG_ERR("\rError while removing: %i\r\n", res);
            status = PSA_ERROR_STORAGE_FAILURE;
            goto exit;
        }

        status = PSA_SUCCESS;
exit:
#if defined(SDK_OS_FREE_RTOS)
        if (lfs_mutex_present)
        {
            OSA_MutexUnlock((osa_mutex_handle_t)lfs_mutex);
        }
#endif /* SDK_OS_FREE_RTOS */
        return status;
}

/* Create the top level directory for NVM storage */
psa_status_t secure_storage_its_initialize(void)
{
        int status;
        int res;
        lfs_get_default_config(&cfg);

        status = lfs_storage_init(&cfg);
        if (status != 0)
        {
            LOG_ERR("LFS storage init failed: %i\r\n", status);
            return PSA_ERROR_STORAGE_FAILURE;
        }
        
        res = lfs_mount(&lfs, &cfg);
        if (res)
        {
            LOG_DBG("\rError mounting LFS\r\n");
            LOG_DBG("Formatting filesystem. You will loose any data\r\n");
            res = lfs_format(&lfs, &cfg);
            if (res)
            {
                LOG_ERR("\rError formatting LFS: %d\r\n", res);
                return PSA_ERROR_STORAGE_FAILURE;
             }
             res = lfs_mount(&lfs, &cfg);
             if (res)
             {
                 LOG_ERR("Mounting failed even after formatting\r\n");
                 return PSA_ERROR_STORAGE_FAILURE;
              }
        }
        
        lfs_mounted = 1;
     
        LOG_DBG("\r\nCreate directory......\r\n");
        res = lfs_mkdir(&lfs, CONFIG_SECURE_STORAGE_ITS_STORE_LITTLEFS_PREFIX);
        if (res && res != LFS_ERR_EXIST)
        {
            LOG_ERR("Make directory failed.\r\n");
            return PSA_ERROR_STORAGE_FAILURE;
        }

        return PSA_SUCCESS;
}

psa_status_t secure_storage_notify_lfs_handler(void *lfs_handler)
{
        int res;
        lfs = *(lfs_t*)lfs_handler;
        psa_status_t status = PSA_ERROR_STORAGE_FAILURE;

#if defined(SDK_OS_FREE_RTOS)
        if (lfs_mutex_present)
        {
                (void)OSA_MutexLock((osa_mutex_handle_t)lfs_mutex, osaWaitForever_c);
        }
#endif /* SDK_OS_FREE_RTOS */
        res = lfs_mkdir(&lfs, CONFIG_SECURE_STORAGE_ITS_STORE_LITTLEFS_PREFIX);
        if (res && res != LFS_ERR_EXIST)
        {
            LOG_ERR("Make directory failed.\r\n");
            status =  PSA_ERROR_STORAGE_FAILURE;
            goto exit;
        }

        lfs_mounted = 1;
        status = PSA_SUCCESS;
exit:
#if defined(SDK_OS_FREE_RTOS)
        if (lfs_mutex_present)
        {
            OSA_MutexUnlock((osa_mutex_handle_t)lfs_mutex);
        }
#endif /* SDK_OS_FREE_RTOS */
        return status;
}

#if defined(SDK_OS_FREE_RTOS)
void secure_storage_notify_lfs_mutex(osa_mutex_handle_t lfs_app_mutex)
{
        assert(NULL != lfs_app_mutex);
        lfs_mutex = lfs_app_mutex;
        lfs_mutex_present = true;
}
#endif /* SDK_OS_FREE_RTOS */
