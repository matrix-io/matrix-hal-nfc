# project(malos_service C CXX)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_STANDARD 11)

project(Example C CXX)

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

link_directories(${CMAKE_SOURCE_DIR}/../build/)

find_package(Threads)
find_library(FFTW_LIBRARIES NAMES fftw3f )
find_library(WIRINGPI_LIB NAMES wiringPi)
find_library(WIRINGPI_DEV_LIB NAMES wiringPiDev)
find_library(CRYPT_LIB NAMES crypt)
find_library(GFLAGS_LIB NAMES gflags)
message(STATUS "gflags found =>" "${GFLAGS_LIB}")

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

add_executable(nfc_read nfc_read.cpp)
set_property(TARGET nfc_read PROPERTY CXX_STANDARD 11)
target_link_libraries(nfc_read matrix_creator_hal)
target_link_libraries(nfc_read matrix_hal_nfc)
target_link_libraries(nfc_read ${CMAKE_THREAD_LIBS_INIT})