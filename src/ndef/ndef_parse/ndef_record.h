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

#ifndef NdefRecord_h
#define NdefRecord_h

#include <memory.h>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#define TNF_EMPTY 0x0
#define TNF_WELL_KNOWN 0x01
#define TNF_MIME_MEDIA 0x02
#define TNF_ABSOLUTE_URI 0x03
#define TNF_EXTERNAL_TYPE 0x04
#define TNF_UNKNOWN 0x05
#define TNF_UNCHANGED 0x06
#define TNF_RESERVED 0x07

class NDEFRecord {
   public:
    NDEFRecord();
    NDEFRecord(const NDEFRecord &rhs);
    ~NDEFRecord();
    NDEFRecord &operator=(const NDEFRecord &rhs);

    int GetEncodedSize();
    void Encode(uint8_t *data, bool firstRecord, bool lastRecord);

    unsigned int GetTypeLength();
    int GetPayloadLength();
    unsigned int GetIdLength();

    uint8_t GetTnf();
    void GetType(uint8_t *type);
    void getPayload(uint8_t *payload);
    void GetId(uint8_t *id);

    // convenience methods
    std::string GetType();
    std::string GetId();

    void SetTnf(uint8_t tnf);
    void SetType(const uint8_t *type, const unsigned int numBytes);
    void SetPayload(const uint8_t *payload, const int numBytes);
    void SetId(const uint8_t *id, const unsigned int numBytes);

    std::string ToString();

   private:
    uint8_t GetTnfByte(bool firstRecord, bool lastRecord);
    uint8_t _tnf;  // 3 bit
    unsigned int _typeLength;
    int _payloadLength;
    unsigned int _idLength;
    uint8_t *_type;
    uint8_t *_payload;
    uint8_t *_id;
};

#endif
