project(matrix_hal_nfc_examples C CXX)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_STANDARD 11)
add_definitions(-Wall)

# NXP Lib requires this
option(PLATFORM_RaspberryPi  "Platform is RaspberryPi"                                ON)

# NXP Lib requires this
if(${PLATFORM_RaspberryPi})
    add_definitions(-pthread)
    add_definitions(-DDEBUG)
    add_definitions(-DNXPBUILD__PH_RASPBERRY_PI)
endif(${PLATFORM_RaspberryPi})

# Include NXP Lib headers
include_directories(
    /usr/local/include/matrix_nfc/nxp_nfc/NxpNfcRdLib/intfs
    /usr/local/include/matrix_nfc/nxp_nfc/NxpNfcRdLib/types
)

include_directories(
    ../src/
)

add_executable(nfc_detect nfc_detect.cpp)
set_property(TARGET nfc_detect PROPERTY CXX_STANDARD 11)
target_link_libraries(nfc_detect matrix_creator_hal)
target_link_libraries(nfc_detect matrix_hal_nfc)
target_link_libraries(nfc_detect ${CMAKE_THREAD_LIBS_INIT})

add_executable(nfc_unique_color_toggle nfc_unique_color_toggle.cpp)
set_property(TARGET nfc_unique_color_toggle PROPERTY CXX_STANDARD 11)
target_link_libraries(nfc_unique_color_toggle matrix_creator_hal)
target_link_libraries(nfc_unique_color_toggle matrix_hal_nfc)
target_link_libraries(nfc_unique_color_toggle ${CMAKE_THREAD_LIBS_INIT})

add_executable(nfc_rainbow_toggle nfc_rainbow_toggle.cpp)
set_property(TARGET nfc_rainbow_toggle PROPERTY CXX_STANDARD 11)
target_link_libraries(nfc_rainbow_toggle matrix_creator_hal)
target_link_libraries(nfc_rainbow_toggle matrix_hal_nfc)
target_link_libraries(nfc_rainbow_toggle ${CMAKE_THREAD_LIBS_INIT})

add_executable(benchmark_nfc_readinfo benchmark_nfc_readinfo.cpp)
set_property(TARGET benchmark_nfc_readinfo PROPERTY CXX_STANDARD 11)
target_link_libraries(benchmark_nfc_readinfo matrix_creator_hal)
target_link_libraries(benchmark_nfc_readinfo matrix_hal_nfc)
target_link_libraries(benchmark_nfc_readinfo ${CMAKE_THREAD_LIBS_INIT})

add_executable(nfc_read_pages nfc_read_pages.cpp)
set_property(TARGET nfc_read_pages PROPERTY CXX_STANDARD 11)
target_link_libraries(nfc_read_pages matrix_creator_hal)
target_link_libraries(nfc_read_pages matrix_hal_nfc)
target_link_libraries(nfc_read_pages ${CMAKE_THREAD_LIBS_INIT})

add_executable(nfc_write_page nfc_write_page.cpp)
set_property(TARGET nfc_write_page PROPERTY CXX_STANDARD 11)
target_link_libraries(nfc_write_page matrix_creator_hal)
target_link_libraries(nfc_write_page matrix_hal_nfc)
target_link_libraries(nfc_write_page ${CMAKE_THREAD_LIBS_INIT})

add_executable(nfc_read_ndef nfc_read_ndef.cpp)
set_property(TARGET nfc_read_ndef PROPERTY CXX_STANDARD 11)
target_link_libraries(nfc_read_ndef matrix_creator_hal)
target_link_libraries(nfc_read_ndef matrix_hal_nfc)
target_link_libraries(nfc_read_ndef ${CMAKE_THREAD_LIBS_INIT})

add_executable(nfc_write_ndef nfc_write_ndef.cpp)
set_property(TARGET nfc_write_ndef PROPERTY CXX_STANDARD 11)
target_link_libraries(nfc_write_ndef matrix_creator_hal)
target_link_libraries(nfc_write_ndef matrix_hal_nfc)
target_link_libraries(nfc_write_ndef ${CMAKE_THREAD_LIBS_INIT})