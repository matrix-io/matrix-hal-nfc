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

#include "NdefMessage.h"

NdefMessage::NdefMessage(void) { _recordCount = 0; }

NdefMessage::NdefMessage(const uint8_t* data, const int numBytes) {
    // DumpHex(data, numBytes, 16);

    _recordCount = 0;

    int index = 0;

    while (index <= numBytes) {
        // decode tnf - first uint8_t is tnf with bit flags
        // see the NFDEF spec for more info
        uint8_t tnf_uint8_t = data[index];
        // bool mb = tnf_uint8_t & 0x80;
        bool me = tnf_uint8_t & 0x40;
        // bool cf = tnf_uint8_t & 0x20;
        bool sr = tnf_uint8_t & 0x10;
        bool il = tnf_uint8_t & 0x8;
        uint8_t tnf = (tnf_uint8_t & 0x7);

        NdefRecord record = NdefRecord();
        record.setTnf(tnf);

        index++;
        int typeLength = data[index];

        uint32_t payloadLength = 0;
        if (sr) {
            index++;
            payloadLength = data[index];
        } else {
            payloadLength = (static_cast<uint32_t>(data[index]) << 24) |
                            (static_cast<uint32_t>(data[index + 1]) << 16) |
                            (static_cast<uint32_t>(data[index + 2]) << 8) |
                            static_cast<uint32_t>(data[index + 3]);
            index += 4;
        }

        int idLength = 0;
        if (il) {
            index++;
            idLength = data[index];
        }

        index++;
        record.setType(&data[index], typeLength);
        index += typeLength;

        if (il) {
            record.setId(&data[index], idLength);
            index += idLength;
        }

        record.setPayload(&data[index], payloadLength);
        index += payloadLength;

        addRecord(record);

        if (me) break;  // last message
    }
}

NdefMessage::NdefMessage(const NdefMessage& rhs) {
    _recordCount = rhs._recordCount;
    for (unsigned int i = 0; i < _recordCount; i++) {
        _records[i] = rhs._records[i];
    }
}

NdefMessage::~NdefMessage() {}

NdefMessage& NdefMessage::operator=(const NdefMessage& rhs) {
    if (this != &rhs) {
        // delete existing records
        for (unsigned int i = 0; i < _recordCount; i++) {
            // TODO Dave: is this the right way to delete existing records?
            _records[i] = NdefRecord();
        }

        _recordCount = rhs._recordCount;
        for (unsigned int i = 0; i < _recordCount; i++) {
            _records[i] = rhs._records[i];
        }
    }
    return *this;
}

unsigned int NdefMessage::getRecordCount() { return _recordCount; }

int NdefMessage::getEncodedSize() {
    int size = 0;
    for (unsigned int i = 0; i < _recordCount; i++) {
        size += _records[i].getEncodedSize();
    }
    return size;
}

// TODO change this to return uint8_t*
void NdefMessage::encode(uint8_t* data) {
    // assert sizeof(data) >= getEncodedSize()
    uint8_t* data_ptr = &data[0];

    for (unsigned int i = 0; i < _recordCount; i++) {
        _records[i].encode(data_ptr, i == 0, (i + 1) == _recordCount);
        // TODO can NdefRecord.encode return the record size?
        data_ptr += _records[i].getEncodedSize();
    }
}

bool NdefMessage::addRecord(NdefRecord& record) {
    if (_recordCount < MAX_NDEF_RECORDS) {
        _records[_recordCount] = record;
        _recordCount++;
        return true;
    } else {
        return false;
    }
}

void NdefMessage::addMimeMediaRecord(std::string mimeType,
                                     std::string payload) {
    uint8_t payloadBytes[payload.length() + 1];
    // payload.getBytes(payloadBytes, sizeof(payloadBytes));
    std::copy(payload.begin(), payload.end(), payloadBytes);

    addMimeMediaRecord(mimeType, payloadBytes, payload.length());
}

void NdefMessage::addMimeMediaRecord(std::string mimeType, uint8_t* payload,
                                     int payloadLength) {
    NdefRecord r = NdefRecord();
    r.setTnf(TNF_MIME_MEDIA);

    uint8_t type[mimeType.length() + 1];
    // mimeType.getBytes(type, sizeof(type));
    std::copy(mimeType.begin(), mimeType.end(), type);
    r.setType(type, mimeType.length());

    r.setPayload(payload, payloadLength);

    addRecord(r);
}

void NdefMessage::addTextRecord(std::string text) { addTextRecord(text, "en"); }

void NdefMessage::addTextRecord(std::string text, std::string encoding) {
    NdefRecord r = NdefRecord();
    r.setTnf(TNF_WELL_KNOWN);

    uint8_t RTD_TEXT[1] = {
        0x54};  // TODO this should be a constant or preprocessor
    r.setType(RTD_TEXT, sizeof(RTD_TEXT));

    // X is a placeholder for encoding length
    // TODO is it more efficient to build w/o string concatenation?
    std::string payloadString = "X" + encoding + text;

    uint8_t payload[payloadString.length() + 1];
    // payloadString.getBytes(payload, sizeof(payload));
    std::copy(payloadString.begin(), payloadString.end(), payload);

    // replace X with the real encoding length
    payload[0] = encoding.length();

    r.setPayload(payload, payloadString.length());

    addRecord(r);
}

void NdefMessage::addUriRecord(std::string uri) {
    NdefRecord* r = new NdefRecord();
    r->setTnf(TNF_WELL_KNOWN);

    uint8_t RTD_URI[1] = {
        0x55};  // TODO this should be a constant or preprocessor
    r->setType(RTD_URI, sizeof(RTD_URI));

    // X is a placeholder for identifier code
    std::string payloadString = "X" + uri;

    uint8_t payload[payloadString.length() + 1];
    // payloadString.getBytes(payload, sizeof(payload));
    std::copy(payloadString.begin(), payloadString.end(), payload);

    // add identifier code 0x0, meaning no prefix substitution
    payload[0] = 0x0;

    r->setPayload(payload, payloadString.length());

    addRecord(*r);
    delete (r);
}

void NdefMessage::addEmptyRecord() {
    NdefRecord* r = new NdefRecord();
    r->setTnf(TNF_EMPTY);
    addRecord(*r);
    delete (r);
}

NdefRecord NdefMessage::getRecord(int index) {
    if (index > -1 && index < static_cast<int>(_recordCount)) {
        return _records[index];
    } else {
        return NdefRecord();  // would rather return NULL
    }
}

NdefRecord NdefMessage::operator[](int index) { return getRecord(index); }

#ifdef NDEF_USE_SERIAL
void NdefMessage::print() {}
#endif
