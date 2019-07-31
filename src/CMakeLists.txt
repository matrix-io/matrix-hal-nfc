# This build file is for matrix-hal-nfc, it integrates with the NXP NFC Reader Library
project(matrix_hal_nfc C CXX)
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

# Compile NXP lib
add_subdirectory(/usr/local/include/matrix_nfc/nxp_nfc/NxpNfcRdLib nxp_lib)

# Include header files needed for compiling matrix-hal-nfc 
include_directories(
    ./
)

set(matrix_hal_nfc_headers
nfc_data.h
data/info_content.h
data/ndef_content.h
data/pages_content.h
nfc.h
nfc_init.h
nfc_utility.h
nfc_status.h
cards/mful.h
cards/ntag.h
ndef/ndef.h
ndef/ndef_parse/ndef_message.h
ndef/ndef_parse/ndef_record.h
)

set (matrix_hal_nfc_src
nfc.cpp
nfc_init.cpp
nfc_utility.cpp
nfc_status.cpp
cards/mful.cpp
cards/ntag.cpp
ndef/ndef.cpp
ndef/ndef_parse/ndef_message.cpp
ndef/ndef_parse/ndef_record.cpp
)

add_library(matrix_hal_nfc SHARED ${matrix_hal_nfc_src})
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

# install(FILES ${matrix_hal_nfc_headers} DESTINATION include/matrix_nfc)

foreach ( file ${matrix_hal_nfc_headers} )
    get_filename_component( dir ${file} DIRECTORY )
    install( FILES ${file} DESTINATION include/matrix_nfc/${dir} )
endforeach()