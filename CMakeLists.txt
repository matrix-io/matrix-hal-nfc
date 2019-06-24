# This build file is for matrix-hal-nfc, it integrates with the NXP NFC Reader Library
cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_STANDARD 11)

project(matrix_hal_nfc C CXX)

# NXP Lib requires this
option(PLATFORM_RaspberryPi  "Platform is RaspberryPi"                                ON)

add_definitions(-Wall)

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

# Compile NXP lib
add_subdirectory(/usr/local/include/matrix_nfc/nxp_nfc/NxpNfcRdLib nxp_lib)

# Include header files needed for compiling matrix-hal-nfc 
include_directories(
    ./src/
    # NXP Library Init
    /usr/local/include/matrix_nfc/nxp_nfc/Nfcrdlib_SimplifiedAPI_ISO/intfs
    /usr/local/include/matrix_nfc/nxp_nfc/NfcrdlibEx3_NFCForum/intfs/
)

set(nxp_init "/usr/local/include/matrix_nfc/nxp_nfc/NfcrdlibEx3_NFCForum/src/phApp_Init.c")
set(nxp_cards "/usr/local/include/matrix_nfc/nxp_nfc/NfcrdlibEx3_NFCForum/src/cards.c")

set(matrix_hal_nfc_headers
./src/nfc_info.h
./src/nfc_sensor.h
)

set (matrix_hal_nfc_src
./src/nfc_sensor.cpp
)

find_package(Threads)
find_library (FFTW_LIBRARIES NAMES fftw3f )
find_library(WIRINGPI_LIB NAMES wiringPi)
find_library(WIRINGPI_DEV_LIB NAMES wiringPiDev)
find_library(CRYPT_LIB NAMES crypt)
find_library(GFLAGS_LIB NAMES gflags)
message(STATUS "gflags found =>" "${GFLAGS_LIB}")

add_library(matrix_hal_nfc SHARED ${nxp_init} ${nxp_cards} ${matrix_hal_nfc_src})
set_property(TARGET matrix_hal_nfc PROPERTY CXX_STANDARD 11)

target_link_libraries(
    matrix_hal_nfc
    NxpRdLib_Osal
    NxpRdLib_Platform
    NxpRdLib_acDiscLoop
    NxpRdLib_halHw
    NxpRdLib_balReg
    NxpRdLib_KeyStore
    NxpRdLib_phTools
    NxpRdLib_palFelica
    NxpRdLib_palI18092mPI
    NxpRdLib_palI14443p4a
    NxpRdLib_palI14443p4
    NxpRdLib_palI14443p4
    NxpRdLib_palMifare
    NxpRdLib_alFelica
    NxpRdLib_alMful
    NxpRdLib_alMfdf
    NxpRdLib_alT1T
    NxpRdLib_alTop
    NxpRdLib_alMfc
    NxpRdLib_palSli15693
    NxpRdLib_palI18000p3m3
    NxpRdLib_alI15693
    NxpRdLib_alI18000p3m3
    NxpRdLib_alSli
    NxpRdLib_palEpcUid
    NxpRdLib_lnLlcp
    NxpRdLib_palI14443p4mC
    NxpRdLib_palI18092mT
    NxpRdLib_ceT4T
    NxpRdLib_palEpcUid
    NxpRdLib_phTools
    NxpRdLib_palI18092mT
    NxpRdLib_npSnep
    NxpRdLib_phNfcLib
    ${CMAKE_THREAD_LIBS_INIT}
)

install(TARGETS matrix_hal_nfc DESTINATION lib)

install(FILES ${matrix_hal_nfc_headers} DESTINATION include/matrix_nfc)