# ~~~
# Copyright (c) 2025-2026 The Khronos Group Inc.
# Copyright (c) 2025-2026 RasterGrid Kft.
#
# SPDX-License-Identifier: Apache-2.0
# ~~~
file(GLOB PC_JSON_PATHS "${ARG_PATH}/*.json")

foreach(PC_JSON_PATH IN LISTS PC_JSON_PATHS)
    cmake_path(ABSOLUTE_PATH PC_JSON_PATH BASE_DIRECTORY "${ARG_PATH}" NORMALIZE)
    list(APPEND JSON_SOURCES "${PC_JSON_PATH}")
    file(READ "${PC_JSON_PATH}" PC_JSON)
    string(JSON ROOT_LENGTH LENGTH "${PC_JSON}")
    math(EXPR ROOT_LENGTH_STOP "${ROOT_LENGTH} - 1")
    foreach(ROOT_INDEX RANGE ${ROOT_LENGTH_STOP})
        string(JSON MEMBER_NAME MEMBER "${PC_JSON}" ${ROOT_INDEX})
        if(MEMBER_NAME MATCHES "(Compute|Graphics)PipelineState")
            string(JSON SHADER_FILENAMES GET "${PC_JSON}" ${MEMBER_NAME} "ShaderFileNames")
            string(JSON SHADER_FILENAMES_LENGTH LENGTH "${SHADER_FILENAMES}")
            math(EXPR SHADER_FILENAMES_LENGTH_STOP "${SHADER_FILENAMES_LENGTH} - 1")
            foreach(SHADER_FILENAME_INDEX RANGE ${SHADER_FILENAMES_LENGTH_STOP})
                string(JSON SHADER_FILENAME GET "${PC_JSON}" ${MEMBER_NAME} "ShaderFileNames" ${SHADER_FILENAME_INDEX} "filename")
                cmake_path(ABSOLUTE_PATH SHADER_FILENAME BASE_DIRECTORY "${ARG_PATH}" NORMALIZE)
                list(APPEND SHADER_SOURCES "${SHADER_FILENAME}")
            endforeach()
        endif()
    endforeach()
endforeach()

list(JOIN JSON_SOURCES "\\\n" JSON_DEPS)
list(JOIN SHADER_SOURCES "\\\n" SHADER_DEPS)
set(DEPFILE_CONTENTS "${ARG_OUTPUT}: ${JSON_DEPS} ${SHADER_DEPS}")

file(WRITE "${ARG_DEPFILE}" "${DEPFILE_CONTENTS}")
