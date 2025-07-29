/* 
 * Copyright 2025 NXP
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <stdio.h>

#include <zephyr/secure_storage/its/store.h>

#include <secure_storage_log.h>

#include "lfs.h"

extern int lfs_get_default_config(struct lfs_config *lfsc);
extern int lfs_storage_init(const struct lfs_config *lfsc);

lfs_t lfs;
struct lfs_config cfg;
int lfs_mounted;

#define PSA_ITS_STORAGE_FILENAME_PATTERN "%08x%08x%08x"
enum { NAME_BUF_SIZE = sizeof(CONFIG_SECURE_STORAGE_ITS_STORE_LITTLEFS_PREFIX)
		       + 3 * (sizeof(secure_storage_its_uid_t) + 1) };

static void make_name(secure_storage_its_uid_t uid, char name[static NAME_BUF_SIZE])
{
	snprintf(name, NAME_BUF_SIZE, "%s/" PSA_ITS_STORAGE_FILENAME_PATTERN,
		 CONFIG_SECURE_STORAGE_ITS_STORE_LITTLEFS_PREFIX,
                 uid.caller_id,
                 (unsigned)(uid.uid >> 32),
                 (unsigned)(uid.uid & 0xffffffff));
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
 
        res = lfs_file_open(&lfs, &file, name, LFS_O_WRONLY | LFS_O_CREAT);
        if (res)
        {
            LOG_DBG("\rError opening file: %i\r\n", res);
            return PSA_ERROR_INSUFFICIENT_STORAGE;
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
        
        return status;
}

psa_status_t secure_storage_its_store_get(secure_storage_its_uid_t uid, size_t data_size,
					  void *data, size_t *data_length)
{
        int res;
        lfs_file_t file;
	char name[NAME_BUF_SIZE];

        if (!lfs_mounted)
        {
            LOG_ERR("LFS not mounted\r\n");
            return PSA_ERROR_STORAGE_FAILURE;
        }

  	make_name(uid, name);
        res = lfs_file_open(&lfs, &file, name, LFS_O_RDONLY);
        if (res)
        {
            LOG_DBG("\rError opening file: %i\r\n", res);
            return PSA_ERROR_DOES_NOT_EXIST;
        }

        res = lfs_file_read(&lfs, &file, data, data_size);
        if (res < 0)
        {
            LOG_ERR("\rError reading file: %i\r\n", res);
            return PSA_ERROR_STORAGE_FAILURE;
        }

        *data_length = res;
        
        res = lfs_file_close(&lfs, &file);
        if (res)
        {
            LOG_ERR("\rError closing file: %i\r\n", res);
            return PSA_ERROR_STORAGE_FAILURE;
        }        
        
        return PSA_SUCCESS;
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
 
        res = lfs_file_open(&lfs, &file, name, LFS_O_WRONLY | LFS_O_CREAT);
        if (res)
        {
            LOG_DBG("\rError opening file: %i\r\n", res);
            return PSA_ERROR_INSUFFICIENT_STORAGE;
        }
        
        if (data_offset != 0)
        {
              res = lfs_file_seek(&lfs, &file, data_offset, LFS_SEEK_CUR);
              if (res < 0)
              {
                  return PSA_ERROR_STORAGE_FAILURE;
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
        
        return status;
}

psa_status_t secure_storage_its_store_get_from_offset(secure_storage_its_uid_t uid, size_t data_size,
                                                       void *data, size_t *data_length, uint32_t data_offset)
{
        int res;
        lfs_file_t file;
	char name[NAME_BUF_SIZE];

        if (!lfs_mounted)
        {
            LOG_ERR("LFS not mounted\r\n");
            return PSA_ERROR_STORAGE_FAILURE;
        }

  	make_name(uid, name);
        res = lfs_file_open(&lfs, &file, name, LFS_O_RDONLY);
        if (res)
        {
            LOG_DBG("\rError opening file: %i\r\n", res);
            return PSA_ERROR_DOES_NOT_EXIST;
        }
        
        if (data_offset != 0)
        {
              res = lfs_file_seek(&lfs, &file, data_offset, LFS_SEEK_CUR);
              if (res < 0)
              {
                  return PSA_ERROR_STORAGE_FAILURE;
              }
        }

        res = lfs_file_read(&lfs, &file, data, data_size);
        if (res < 0)
        {
            LOG_ERR("\rError reading file: %i\r\n", res);
            return PSA_ERROR_STORAGE_FAILURE;
        }

        *data_length = res;
        
        res = lfs_file_close(&lfs, &file);
        if (res)
        {
            LOG_ERR("\rError closing file: %i\r\n", res);
            return PSA_ERROR_STORAGE_FAILURE;
        }        
        
        return PSA_SUCCESS;
}

psa_status_t secure_storage_its_store_remove(secure_storage_its_uid_t uid)
{
	int res;
	char name[NAME_BUF_SIZE];
	make_name(uid, name);      
        
        if (!lfs_mounted)
        {
            LOG_ERR("LFS not mounted\r\n");
            return PSA_ERROR_STORAGE_FAILURE;
        }

        res = lfs_remove(&lfs, name);
        if (res)
        {
            LOG_ERR("\rError while removing: %i\r\n", res);
            return PSA_ERROR_STORAGE_FAILURE;
        }
        
        return PSA_SUCCESS;
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
