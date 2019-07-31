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

add_executable(nfc_rgb nfc_rgb.cpp)
set_property(TARGET nfc_rgb PROPERTY CXX_STANDARD 11)
target_link_libraries(nfc_rgb matrix_creator_hal)
target_link_libraries(nfc_rgb matrix_hal_nfc)
target_link_libraries(nfc_rgb ${CMAKE_THREAD_LIBS_INIT})

add_executable(nfc_rainbow_switch nfc_rainbow_switch.cpp)
set_property(TARGET nfc_rainbow_switch PROPERTY CXX_STANDARD 11)
target_link_libraries(nfc_rainbow_switch matrix_creator_hal)
target_link_libraries(nfc_rainbow_switch matrix_hal_nfc)
target_link_libraries(nfc_rainbow_switch ${CMAKE_THREAD_LIBS_INIT})

add_executable(nfc_read_mful_tag nfc_read_mful_tag.cpp)
set_property(TARGET nfc_read_mful_tag PROPERTY CXX_STANDARD 11)
target_link_libraries(nfc_read_mful_tag matrix_creator_hal)
target_link_libraries(nfc_read_mful_tag matrix_hal_nfc)
target_link_libraries(nfc_read_mful_tag ${CMAKE_THREAD_LIBS_INIT})

add_executable(nfc_write_mful_tag nfc_write_mful_tag.cpp)
set_property(TARGET nfc_write_mful_tag PROPERTY CXX_STANDARD 11)
target_link_libraries(nfc_write_mful_tag matrix_creator_hal)
target_link_libraries(nfc_write_mful_tag matrix_hal_nfc)
target_link_libraries(nfc_write_mful_tag ${CMAKE_THREAD_LIBS_INIT})

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