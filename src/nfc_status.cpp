
#include "./nfc_status.h"

namespace matrix_hal {

/* If status is negative then it is standard NXP library error, not simplified
NXP library error. Errors are handled this way so that end user can do `cout <<
DescStatus(FunctionName())` and get the error logged. */
std::string NFCStatus(int64_t status_type) {
    // Handle wrapper errors
    switch (status_type) {
        case INCORRECT_CARD_FOR_FUNCTION:
            return "Incorrect Card Type For Function";
            break;
        default:
            break;
    }
    // Handle NXP errors
    if (status_type < 0)
        return NXPLibStatus(static_cast<uint32_t>(-status_type));
    else
        return NXPSimplifiedLibStatus(static_cast<uint32_t>(status_type));
}

std::string NXPSimplifiedLibStatus(uint32_t status_type) {
    switch (status_type) {
        case PH_NFCLIB_STATUS_SUCCESS:
            return "Success";
            break;
        case PH_NFCLIB_STATUS_INVALID_STATE:
            return "Invalid State";
            break;
        case PH_NFCLIB_STATUS_INVALID_PARAMETER:
            return "Invalid Parameter";
            break;
        case PH_NFCLIB_STATUS_BUFFER_OVERFLOW:
            return "Buffer Overflow";
            break;
        case PH_NFCLIB_STATUS_ABORTED:
            return "Aborted";
            break;
        case PH_NFCLIB_STATUS_INTERNAL_ERROR:
            return "Internal Error";
            break;
        case PH_NFCLIB_STATUS_PEER_ACTIVATION_DONE:
            return "Activation Done";
            break;
        case PH_NFCLIB_STATUS_MULTIPLE_PEERS_DETECTED:
            return "Multiple Peers Detected";
            break;
        case PH_NFCLIB_STATUS_PEER_ACTIVATION_FAILED:
            return "Peer Activation Failed";
            break;
        case PH_NFCLIB_STATUS_NO_PEER_DISCOVERED:
            return "No Peer Discovered";
            break;
        case PH_NFCLIB_STATUS_PEER_DEACTIVATION_FAILED:
            return "Peer Deactivation Failed";
            break;
        case PH_NFCLIB_STATUS_RF_TRANSMISSION_ERROR:
            return "RF Transmission Error";
            break;
        case PH_NFCLIB_STATUS_RF_TIMEOUT_ERROR:
            return "RF Timeout Error";
            break;
        case PH_NFCLIB_STATUS_RF_PROTOCOL_ERROR:
            return "RF Protocol Error";
            break;
        case PH_NFCLIB_STATUS_RF_PEER_LOST:
            return "RF Peer Lost";
            break;
        case INCORRECT_CARD_FOR_FUNCTION:
            return "Incorrect Card Type For Function";
            break;
        default:
            return "Unknown Error";
            break;
    }
}

std::string NXPLibStatus(uint32_t status_type) {
    switch (status_type & PH_ERR_MASK) {
        case PH_ERR_SUCCESS:
            return "Success";
            break;
        case PH_ERR_IO_TIMEOUT:
            return "IO Timeout, no reply received";
            break;
        case PH_ERR_INTEGRITY_ERROR:
            return "Integrity Error, wrong CRC or parity detected";
            break;
        case PH_ERR_COLLISION_ERROR:
            return "Collision Error";
            break;
        case PH_ERR_BUFFER_OVERFLOW:
            return "Buffer Overflow, attempted to write beyond buffer size";
            break;
        case PH_ERR_FRAMING_ERROR:
            return "Framing Error, invalid frame format";
            break;
        case PH_ERR_PROTOCOL_ERROR:
            return "Protocol Error, response violated protocol";
            break;
        case PH_ERR_AUTH_ERROR:
            return "Authentication Error";
            break;
        case PH_ERR_READ_WRITE_ERROR:
            return "Read/Write Error, occured in RAM/ROM or Flash";
            break;
        case PH_ERR_TEMPERATURE_ERROR:
            return "Temperature Error, RC sensors detected overheating";
            break;
        case PH_ERR_RF_ERROR:
            return "RF Error";
            break;
        case PH_ERR_INTERFACE_ERROR:
            return "Interface Error, occured in RC communication";
            break;
        case PH_ERR_LENGTH_ERROR:
            return "Length Error";
            break;
        case PH_ERR_RESOURCE_ERROR:
            return "Resource Error";
            break;
        case PH_ERR_TX_NAK_ERROR:
            return "NAK Error, TX rejected sanely by the counterpart";
            break;
        case PH_ERR_RX_NAK_ERROR:
            return "NAK Error, RX request rejected sanely by the counterpart";
            break;
        case PH_ERR_EXT_RF_ERROR:
            return "Error due to External RF";
            break;
        case PH_ERR_NOISE_ERROR:
            return "EMVCo EMD Noise Error";
            break;
        case PH_ERR_ABORTED:
            return "HAL shutdown was called";
            break;
        case PH_ERR_INTERNAL_ERROR:
            return "Internal Error";
            break;
        case PH_ERR_INVALID_DATA_PARAMS:
            return "Invalid Data Parameters Error, layer ID check failed";
            break;
        case PH_ERR_INVALID_PARAMETER:
            return "Parameter Error, invalid parameter supplied";
            break;
        case PH_ERR_PARAMETER_OVERFLOW:
            return "Parameter Overflow, reading/writing produced an overflow";
            break;
        case PH_ERR_UNSUPPORTED_PARAMETER:
            return "Parameter not supported";
            break;
        case PH_ERR_UNSUPPORTED_COMMAND:
            return "Command not supported";
            break;
        case PH_ERR_USE_CONDITION:
            return "Condition Error";
            break;
        case PH_ERR_KEY:
            return "Key Error";
            break;
        case PH_ERR_OSAL_ERROR:
            return "OSAL Error occurred during initialization";
            break;
        case PHAC_DISCLOOP_FAILURE:
            return "Failure due to error from lower layer";
            break;
        case PHAC_DISCLOOP_COLLISION_PENDING:
            return "Collision pending";
            break;
        case PHAC_DISCLOOP_EXTERNAL_RFON:
            return "External RF field on";
            break;
        case PHAC_DISCLOOP_EXTERNAL_RFOFF:
            return "External RF field off";
            break;
        case PHAC_DISCLOOP_NO_TECH_DETECTED:
            return "No card/device detected";
            break;
        case PHAC_DISCLOOP_NO_DEVICE_RESOLVED:
            return "No card/device resolved";
            break;
        case PHAC_DISCLOOP_LPCD_NO_TECH_DETECTED:
            return "LPCD succeeded but no card/device detected";
            break;
        case PHAC_DISCLOOP_MULTI_TECH_DETECTED:
            return "Multiple cards/devices detected";
            break;
        case PHAC_DISCLOOP_MULTI_DEVICES_RESOLVED:
            return "Multiple cards/devices resolved";
            break;
        default:
            return "Unknown Error";
            break;
    }
}

}  // namespace matrix_hal
