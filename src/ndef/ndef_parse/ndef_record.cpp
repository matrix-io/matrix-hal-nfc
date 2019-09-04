/* Software License Agreement (BSD License)

Copyright (c) 2013-2014, Don Coleman
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holders nor the
names of its contributors may be used to endorse or promote products
derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

#include "ndef_record.h"

namespace matrix_hal {

NDEFRecord::NDEFRecord() {
    // Serial.println("NDEFRecord Constructor 1");
    _tnf = 0;
    _typeLength = 0;
    _payloadLength = 0;
    _idLength = 0;
    _type = (uint8_t *)NULL;
    _payload = (uint8_t *)NULL;
    _id = (uint8_t *)NULL;
}

NDEFRecord::NDEFRecord(const NDEFRecord &rhs) {
    // Serial.println("NDEFRecord Constructor 2 (copy)");

    _tnf = rhs._tnf;
    _typeLength = rhs._typeLength;
    _payloadLength = rhs._payloadLength;
    _idLength = rhs._idLength;
    _type = (uint8_t *)NULL;
    _payload = (uint8_t *)NULL;
    _id = (uint8_t *)NULL;

    if (_typeLength) {
        _type = (uint8_t *)malloc(_typeLength);
        memcpy(_type, rhs._type, _typeLength);
    }

    if (_payloadLength) {
        _payload = (uint8_t *)malloc(_payloadLength);
        memcpy(_payload, rhs._payload, _payloadLength);
    }

    if (_idLength) {
        _id = (uint8_t *)malloc(_idLength);
        memcpy(_id, rhs._id, _idLength);
    }
}

// TODO NDEFRecord::NDEFRecord(tnf, type, payload, id)

NDEFRecord::~NDEFRecord() {
    // Serial.println("NDEFRecord Destructor");
    if (_typeLength) {
        free(_type);
    }

    if (_payloadLength) {
        free(_payload);
    }

    if (_idLength) {
        free(_id);
    }
}

NDEFRecord &NDEFRecord::operator=(const NDEFRecord &rhs) {
    // Serial.println("NDEFRecord ASSIGN");

    if (this != &rhs) {
        // free existing
        if (_typeLength) {
            free(_type);
        }

        if (_payloadLength) {
            free(_payload);
        }

        if (_idLength) {
            free(_id);
        }

        _tnf = rhs._tnf;
        _typeLength = rhs._typeLength;
        _payloadLength = rhs._payloadLength;
        _idLength = rhs._idLength;

        if (_typeLength) {
            _type = (uint8_t *)malloc(_typeLength);
            memcpy(_type, rhs._type, _typeLength);
        }

        if (_payloadLength) {
            _payload = (uint8_t *)malloc(_payloadLength);
            memcpy(_payload, rhs._payload, _payloadLength);
        }

        if (_idLength) {
            _id = (uint8_t *)malloc(_idLength);
            memcpy(_id, rhs._id, _idLength);
        }
    }
    return *this;
}

// size of records in uint8_ts
int NDEFRecord::GetEncodedSize() {
    int size = 2;  // tnf + typeLength
    if (_payloadLength > 0xFF) {
        size += 4;
    } else {
        size += 1;
    }

    if (_idLength) {
        size += 1;
    }

    size += (_typeLength + _payloadLength + _idLength);

    return size;
}

void NDEFRecord::Encode(uint8_t *data, bool firstRecord, bool lastRecord) {
    // assert data > GetEncodedSize()

    uint8_t *data_ptr = &data[0];

    *data_ptr = GetTnfByte(firstRecord, lastRecord);
    data_ptr += 1;

    *data_ptr = _typeLength;
    data_ptr += 1;

    if (_payloadLength <= 0xFF) {  // short record
        *data_ptr = _payloadLength;
        data_ptr += 1;
    } else {  // long format
        // 4 uint8_ts but we store length as an int
        data_ptr[0] = 0x0;  // (_payloadLength >> 24) & 0xFF;
        data_ptr[1] = 0x0;  // (_payloadLength >> 16) & 0xFF;
        data_ptr[2] = (_payloadLength >> 8) & 0xFF;
        data_ptr[3] = _payloadLength & 0xFF;
        data_ptr += 4;
    }

    if (_idLength) {
        *data_ptr = _idLength;
        data_ptr += 1;
    }

    // Serial.println(2);
    memcpy(data_ptr, _type, _typeLength);
    data_ptr += _typeLength;

    if (_idLength) {
        memcpy(data_ptr, _id, _idLength);
        data_ptr += _idLength;
    }

    memcpy(data_ptr, _payload, _payloadLength);
    data_ptr += _payloadLength;
}

uint8_t NDEFRecord::GetTnfByte(bool firstRecord, bool lastRecord) {
    int value = _tnf;

    if (firstRecord) {  // mb
        value = value | 0x80;
    }

    if (lastRecord) {  //
        value = value | 0x40;
    }

    // chunked flag is always false for now
    // if (cf) {
    //     value = value | 0x20;
    // }

    if (_payloadLength <= 0xFF) {
        value = value | 0x10;
    }

    if (_idLength) {
        value = value | 0x8;
    }

    return value;
}

uint8_t NDEFRecord::GetTnf() { return _tnf; }

void NDEFRecord::SetTnf(uint8_t tnf) { _tnf = tnf; }

unsigned int NDEFRecord::GetTypeLength() { return _typeLength; }

int NDEFRecord::GetPayloadLength() { return _payloadLength; }

unsigned int NDEFRecord::GetIdLength() { return _idLength; }

std::string NDEFRecord::GetType() {
    char type[_typeLength + 1];
    memcpy(type, _type, _typeLength);
    type[_typeLength] = '\0';  // null terminate
    return std::string(type);
}

// Get payload string
std::string NDEFRecord::GetPayload() {
    char payload[_payloadLength + 1];
    memcpy(payload, _payload, _payloadLength);
    payload[_payloadLength] = '\0';  // null terminate
    return std::string(payload);
}

// this assumes the caller created type correctly
void NDEFRecord::GetType(uint8_t *type) { memcpy(type, _type, _typeLength); }

void NDEFRecord::SetType(const uint8_t *type, const unsigned int numBytes) {
    if (_typeLength) {
        free(_type);
    }

    _type = (uint8_t *)malloc(numBytes);
    memcpy(_type, type, numBytes);
    _typeLength = numBytes;
}

// assumes the caller sized payload properly
void NDEFRecord::GetPayload(uint8_t *payload) {
    memcpy(payload, _payload, _payloadLength);
}

void NDEFRecord::SetPayload(const uint8_t *payload, const int numBytes) {
    if (_payloadLength) {
        free(_payload);
    }

    _payload = (uint8_t *)malloc(numBytes);
    memcpy(_payload, payload, numBytes);
    _payloadLength = numBytes;
}

std::string NDEFRecord::GetId() {
    char id[_idLength + 1];
    memcpy(id, _id, _idLength);
    id[_idLength] = '\0';  // null terminate
    return std::string(id);
}

void NDEFRecord::GetId(uint8_t *id) { memcpy(id, _id, _idLength); }

void NDEFRecord::SetId(const uint8_t *id, const unsigned int numBytes) {
    if (_idLength) {
        free(_id);
    }

    _id = (uint8_t *)malloc(numBytes);
    memcpy(_id, id, numBytes);
    _idLength = numBytes;
}

std::string BytesToString(const uint8_t *vec, const uint8_t size) {
    std::stringstream result;
    for (int i = 0; i < size; i++) {
        if ((vec[i] < 0x20) || (vec[i] > 0x7e))
            result << '.';
        else
            result << vec[i];
    }
    result << std::flush;
    return result.str();
}

std::string NDEFRecord::ToString() {
    std::stringstream result;
    result << "NDEF Record" << std::flush;
    result << " TNF 0x" << std::setfill('0') << std::hex << std::uppercase
           << +_tnf << std::endl;
    switch (_tnf) {
        case TNF_EMPTY:
            result << "Empty" << std::endl;
            break;
        case TNF_WELL_KNOWN:
            result << "Well Known" << std::endl;
            break;
        case TNF_MIME_MEDIA:
            result << "Mime Media" << std::endl;
            break;
        case TNF_ABSOLUTE_URI:
            result << "Absolute URI" << std::endl;
            break;
        case TNF_EXTERNAL_TYPE:
            result << "External" << std::endl;
            break;
        case TNF_UNKNOWN:
            result << "Unknown" << std::endl;
            break;
        case TNF_UNCHANGED:
            result << "Unchanged" << std::endl;
            break;
        case TNF_RESERVED:
            result << "Reserved" << std::endl;
            break;
        default:
            result << std::endl;
    }
    result << "Type Length 0x" << std::setfill('0') << std::hex
           << std::uppercase << +_typeLength << std::endl;
    result << "Payload Length 0x" << std::setfill('0') << std::hex
           << std::uppercase << +_payloadLength << std::endl;
    if (_idLength) {
        result << "Id Length 0x" << std::setfill('0') << std::hex
               << std::uppercase << +_idLength << std::endl;
    }
    result << "Type" << std::endl;
    result << BytesToString(_type, _typeLength) << std::endl;
    result << "Payload" << std::endl;
    result << BytesToString(_payload, _payloadLength) << std::endl;
    if (_idLength) {
        result << "Id" << std::endl;
        result << BytesToString(_id, _idLength) << std::endl;
    }
    result << "Record is " << GetEncodedSize() << " bytes" << std::flush;
    return result.str();
}

}  // namespace matrix_hal