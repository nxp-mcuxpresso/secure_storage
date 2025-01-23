#
# Copyright 2025 NXP
#
# SPDX-License-Identifier: Apache-2.0

if (CONFIG_MCUX_COMPONENT_middleware.SECURE_STORAGE_ITS_IMPLEMENTATION)
    mcux_add_source(
        SOURCES src/its/its.c
        SOURCES src/its/implementation.c
        BASE_PATH ${SdkRootDirPath}/middleware/secure_storage
    )
    mcux_add_source(
        SOURCES include/its.h
        SOURCES include/common.h
        SOURCES include/its/common.h
        SOURCES include/its/store.h
        SOURCES include/its/transform.h
        SOURCES include/its/transform/aead_get.h
        BASE_PATH ${SdkRootDirPath}/middleware/secure_storage
    )
    mcux_add_include(
        INCLUDES include
        BASE_PATH ${SdkRootDirPath}/middleware/seure_storage/include
    )
endif()

if (CONFIG_MCUX_COMPONENT_middleware.SECURE_STORAGE_ITS_TRANSFORM_IMPLEMENTATION_AEAD)
    mcux_add_source(
        SOURCES src/its/transform/aead.c
        SOURCES src/its/transform/aead_get.c
        BASE_PATH ${SdkRootDirPath}/middleware/secure_storage
    )
    mcux_add_source(
        SOURCES include/its.h
        SOURCES include/common.h
        SOURCES include/its/common.h
        SOURCES include/its/store.h
        SOURCES include/its/transform.h
        SOURCES include/its/transform/aead_get.h
        BASE_PATH ${SdkRootDirPath}/middleware/secure_storage
    )
    mcux_add_include(
        INCLUDES include
        BASE_PATH ${SdkRootDirPath}/middleware/seure_storage/include
    )
endif()
