#
# Copyright 2025 NXP
#
# SPDX-License-Identifier: Apache-2.0

if (CONFIG_MCUX_COMPONENT_middleware.SECURE_STORAGE)
    mcux_add_macro(
        "-DSECURE_STORAGE"
    )
endif()

if (CONFIG_MCUX_COMPONENT_middleware.SECURE_STORAGE_ITS_IMPLEMENTATION)
    mcux_add_source(
        SOURCES src/its/implementation.c
        BASE_PATH ${SdkRootDirPath}/middleware/secure_storage/zephyr/subsys/secure_storage
    )
    mcux_add_source(
        SOURCES include/secure_storage_log.h
        BASE_PATH ${SdkRootDirPath}/middleware/secure_storage
    )
    mcux_add_source(
        SOURCES include/internal/zephyr/secure_storage/its.h
        SOURCES include/internal/zephyr/secure_storage/common.h
        SOURCES include/internal/zephyr/secure_storage/its/common.h
        SOURCES include/internal/zephyr/secure_storage/its/store.h
        SOURCES include/internal/zephyr/secure_storage/its/transform.h
        SOURCES include/internal/zephyr/secure_storage/its/transform/aead_get.h
        SOURCES include/psa/error.h
        SOURCES include/psa/storage_common.h
        SOURCES include/psa/internal_trusted_storage.h
        BASE_PATH ${SdkRootDirPath}/middleware/secure_storage/zephyr/subsys/secure_storage
    )
    mcux_add_include(
        INCLUDES include
        INCLUDES include/internal
        BASE_PATH ${SdkRootDirPath}/middleware/secure_storage/zephyr/subsys/secure_storage
    )
    mcux_add_include(
        INCLUDES include
        BASE_PATH ${SdkRootDirPath}/middleware/secure_storage/
    )
    mcux_add_macro(
        "-DSECURE_STORAGE_ITS_IMPLEMENTATION"
    )
endif()

if (CONFIG_MCUX_COMPONENT_middleware.SECURE_STORAGE_ITS_IMPLEMENTATION_WITH_ONLY_STORE)
    mcux_add_source(
        SOURCES src/its/implementation_store.c
        BASE_PATH ${SdkRootDirPath}/middleware/secure_storage
    )
    mcux_add_source(
        SOURCES include/secure_storage_log.h
        BASE_PATH ${SdkRootDirPath}/middleware/secure_storage
    )
    mcux_add_source(
        SOURCES include/internal/zephyr/secure_storage/its.h
        SOURCES include/internal/zephyr/secure_storage/common.h
        SOURCES include/internal/zephyr/secure_storage/its/common.h
        SOURCES include/internal/zephyr/secure_storage/its/store.h
        SOURCES include/psa/error.h
        SOURCES include/psa/storage_common.h
        SOURCES include/psa/internal_trusted_storage.h
        BASE_PATH ${SdkRootDirPath}/middleware/secure_storage/zephyr/subsys/secure_storage
    )
    mcux_add_include(
        INCLUDES include
        INCLUDES include/internal
        BASE_PATH ${SdkRootDirPath}/middleware/secure_storage/zephyr/subsys/secure_storage
    )
    mcux_add_include(
        INCLUDES include
        BASE_PATH ${SdkRootDirPath}/middleware/secure_storage/
    )
    mcux_add_macro(
        "-DSECURE_STORAGE_ITS_IMPLEMENTATION_WITH_ONLY_STORE"
    )
endif()

if (CONFIG_MCUX_COMPONENT_middleware.SECURE_STORAGE_ITS_TRANSFORM_IMPLEMENTATION_AEAD)
    mcux_add_source(
        SOURCES src/its/transform/aead.c
        SOURCES src/its/transform/aead_get.c
        BASE_PATH ${SdkRootDirPath}/middleware/secure_storage/zephyr/subsys/secure_storage
    )
    mcux_add_source(
        SOURCES include/secure_storage_log.h
        BASE_PATH ${SdkRootDirPath}/middleware/secure_storage
    )
    mcux_add_source(
        SOURCES include/internal/zephyr/secure_storage/its.h
        SOURCES include/internal/zephyr/secure_storage/common.h
        SOURCES include/internal/zephyr/secure_storage/its/common.h
        SOURCES include/internal/zephyr/secure_storage/its/store.h
        SOURCES include/internal/zephyr/secure_storage/its/transform.h
        SOURCES include/internal/zephyr/secure_storage/its/transform/aead_get.h
        SOURCES include/psa/error.h
        SOURCES include/psa/storage_common.h
        SOURCES include/psa/internal_trusted_storage.h
        BASE_PATH ${SdkRootDirPath}/middleware/secure_storage/zephyr/subsys/secure_storage
    )
    mcux_add_include(
        INCLUDES include
        INCLUDES include/internal
        BASE_PATH ${SdkRootDirPath}/middleware/secure_storage/zephyr/subsys/secure_storage
    )
    mcux_add_include(
        INCLUDES include
        BASE_PATH ${SdkRootDirPath}/middleware/secure_storage/
    )
    mcux_add_macro(
        "-DSECURE_STORAGE_ITS_TRANSFORM_IMPLEMENTATION_AEAD"
    )
endif()

if (CONFIG_MCUX_COMPONENT_middleware.SECURE_STORAGE_ITS_STORE_IMPLEMENTATION_FATFS)
    mcux_add_source(
        SOURCES src/its/store/fatfs_its.c
        BASE_PATH ${SdkRootDirPath}/middleware/secure_storage
    )
    mcux_add_source(
        SOURCES include/secure_storage_log.h
        SOURCES include/store_extension.h
        SOURCES include/secure_storage.h
        BASE_PATH ${SdkRootDirPath}/middleware/secure_storage
    )
    mcux_add_source(
        SOURCES include/internal/zephyr/secure_storage/its.h
        SOURCES include/internal/zephyr/secure_storage/common.h
        SOURCES include/internal/zephyr/secure_storage/its/common.h
        SOURCES include/internal/zephyr/secure_storage/its/store.h
        SOURCES include/psa/error.h
        SOURCES include/psa/storage_common.h
        SOURCES include/psa/internal_trusted_storage.h
        BASE_PATH ${SdkRootDirPath}/middleware/secure_storage/zephyr/subsys/secure_storage
    )
    mcux_add_include(
        INCLUDES include
        INCLUDES include/internal
        BASE_PATH ${SdkRootDirPath}/middleware/secure_storage/zephyr/subsys/secure_storage
    )
    mcux_add_include(
        INCLUDES include
        BASE_PATH ${SdkRootDirPath}/middleware/secure_storage/
    )
    mcux_add_macro(
        "-DSECURE_STORAGE_ITS_STORE_IMPLEMENTATION_FATFS"
    )
endif()

if (CONFIG_MCUX_COMPONENT_middleware.SECURE_STORAGE_ITS_STORE_IMPLEMENTATION_LITTLEFS)
    mcux_add_source(
        SOURCES src/its/store/littlefs_its.c
        BASE_PATH ${SdkRootDirPath}/middleware/secure_storage
    )
    mcux_add_source(
        SOURCES include/secure_storage_log.h
        SOURCES include/secure_storage.h
        SOURCES include/store_extension.h
        BASE_PATH ${SdkRootDirPath}/middleware/secure_storage
    )
    mcux_add_source(
        SOURCES include/internal/zephyr/secure_storage/its.h
        SOURCES include/internal/zephyr/secure_storage/common.h
        SOURCES include/internal/zephyr/secure_storage/its/common.h
        SOURCES include/internal/zephyr/secure_storage/its/store.h
        SOURCES include/psa/error.h
        SOURCES include/psa/storage_common.h
        SOURCES include/psa/internal_trusted_storage.h
        BASE_PATH ${SdkRootDirPath}/middleware/secure_storage/zephyr/subsys/secure_storage
    )
    mcux_add_include(
        INCLUDES include
        INCLUDES include/internal
        BASE_PATH ${SdkRootDirPath}/middleware/secure_storage/zephyr/subsys/secure_storage
    )
    mcux_add_include(
        INCLUDES include
        BASE_PATH ${SdkRootDirPath}/middleware/secure_storage/
    )
    mcux_add_macro(
        "-DSECURE_STORAGE_ITS_STORE_IMPLEMENTATION_LITTLEFS"
    )
endif()

if (CONFIG_MCUX_COMPONENT_middleware.SECURE_STORAGE_ITS_STORE_IMPLEMENTATION_SETTINGS)
    mcux_add_source(
        SOURCES src/its/store/nvs_its.c
        BASE_PATH ${SdkRootDirPath}/middleware/secure_storage
    )
    mcux_add_source(
        SOURCES include/secure_storage_log.h
        SOURCES include/secure_storage.h
        SOURCES include/store_extension.h
        BASE_PATH ${SdkRootDirPath}/middleware/secure_storage
    )
    mcux_add_source(
        SOURCES include/internal/zephyr/secure_storage/its.h
        SOURCES include/internal/zephyr/secure_storage/common.h
        SOURCES include/internal/zephyr/secure_storage/its/common.h
        SOURCES include/internal/zephyr/secure_storage/its/store.h
        SOURCES include/psa/error.h
        SOURCES include/psa/storage_common.h
        SOURCES include/psa/internal_trusted_storage.h
        BASE_PATH ${SdkRootDirPath}/middleware/secure_storage/zephyr/subsys/secure_storage
    )
    mcux_add_include(
        INCLUDES include
        INCLUDES include/internal
        BASE_PATH ${SdkRootDirPath}/middleware/secure_storage/zephyr/subsys/secure_storage
    )
    mcux_add_include(
        INCLUDES include
        BASE_PATH ${SdkRootDirPath}/middleware/secure_storage/
    )
    mcux_add_macro(
        "-DSECURE_STORAGE_ITS_STORE_IMPLEMENTATION_NVS"
    )
endif()
