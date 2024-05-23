# ~~~
# Copyright (c) 2024 The Khronos Group Inc.
# Copyright (c) 2024 RasterGrid Kft.
#
# SPDX-License-Identifier: Apache-2.0
# ~~~

# Test install
set(test_install_dir "${CMAKE_CURRENT_BINARY_DIR}/install")
add_test(NAME integration.install
    COMMAND ${CMAKE_COMMAND} --install ${PROJECT_BINARY_DIR} --prefix ${test_install_dir} --config $<CONFIG>
)

# Test add_subdirectory support
add_test(NAME integration.add_subdirectory
    COMMAND ${CMAKE_CTEST_COMMAND}
        --build-and-test ${CMAKE_CURRENT_LIST_DIR}/integration ${CMAKE_CURRENT_BINARY_DIR}/add_subdirectory
        --build-generator ${CMAKE_GENERATOR}
        --build-options -DFIND_PACKAGE_TESTING=OFF -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
)

# Test find_package support
add_test(NAME integration.find_package
    COMMAND ${CMAKE_CTEST_COMMAND}
        --build-and-test ${CMAKE_CURRENT_LIST_DIR}/integration ${CMAKE_CURRENT_BINARY_DIR}/find_package
        --build-generator ${CMAKE_GENERATOR}
        --build-options -DFIND_PACKAGE_TESTING=ON "-DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH};${test_install_dir}"
)

set_tests_properties(integration.find_package PROPERTIES DEPENDS integration.install)
