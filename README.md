# MATRIX HAL NFC (WIP)

matrix-hal-nfc is a simple, easy to use wrapper for using NXP's NFC Reader Library with the PN512 chip on the [MATRIX Creator](https://matrix-io.github.io/matrix-documentation/matrix-creator/overview/).

Currently the wrapper only supports detecting and reading NFC tags.

## License

All files in this repository (not including the proprietary NXP NFC Reader Library) are distributed under the GPL3 license.

## External components

NXP releases the source code to their library upon registering an account with their website.

Start by downloading the library files from here:

https://www.nxp.com/products/identification-security/secure-car-access/nfc-reader-library-software-support-for-nfc-frontend-solutions:NFC-READER-LIBRARY?tab=In-Depth_Tab#nogo

After you click download, click `4.04.05 NFC Reader Library for PN512`.

Then click `NFC Reader Library v4.040.05 R2 for PNEV512B including all software examples`.

A file called SW297940.zip should begin downloading.

Then clone this repo with

```
git clone https://github.com/matrix-io/matrix-hal-nfc.git
```

Move the SW297940.zip into the cloned matrix-hal-nfc folder.

Once complete, you can apply the MATRIX Creator specific config patch and install the library into /usr/include/matrix_nfc/nxp_nfc with the following commands:

```
unzip SW297940.zip -d nxp_nfc && patch < creator_nfc_pins.patch ./nxp_nfc/NxpNfcRdLib/intfs/phPlatform_Port_Pi_RC523.h && sudo mkdir -p /usr/local/include/matrix_nfc/nxp_nfc/ && sudo cp -r nxp_nfc/ /usr/local/include/matrix_nfc/ && sudo chmod 755 -R /usr/local/include/matrix_nfc/ && sudo rm -r nxp_nfc
```

## Installation

Please ensure that you have first followed the above steps and that the NXP Library is extracted to /usr/include/matrix_nfc/nxp_nfc.

In the root directory of this repository there is a rebuild.sh file. This will run cmake and make, and also install the needed headers in `/usr/local/include/matrix_nfc/` and the libmatrix_hal_nfc.so library file in `/usr/local/lib/`.

After building is complete, the compiled nfc_read example will be in the build folder.

## Examples

Currently the examples reside in ./examples/. This shows the intended wrapper usage.
