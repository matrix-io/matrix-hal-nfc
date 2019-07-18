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

#ifndef NdefMessage_h
#define NdefMessage_h

#include <memory.h>
#include <cstdint>
#include <string>

#include "NdefRecord.h"

#define MAX_NDEF_RECORDS 4

class NdefMessage {
   public:
    NdefMessage(void);
    NdefMessage(const uint8_t* data, const int numBytes);
    NdefMessage(const NdefMessage& rhs);
    ~NdefMessage();
    NdefMessage& operator=(const NdefMessage& rhs);

    int getEncodedSize();  // need so we can pass array to encode
    void encode(uint8_t* data);

    bool addRecord(NdefRecord& record);
    void addMimeMediaRecord(std::string mimeType, std::string payload);
    void addMimeMediaRecord(std::string mimeType, uint8_t* payload,
                            int payloadLength);
    void addTextRecord(std::string text);
    void addTextRecord(std::string text, std::string encoding);
    void addUriRecord(std::string uri);
    void addEmptyRecord();

    unsigned int getRecordCount();
    NdefRecord getRecord(int index);
    NdefRecord operator[](int index);

#ifdef NDEF_USE_SERIAL
    void print();
#endif
   private:
    NdefRecord _records[MAX_NDEF_RECORDS];
    unsigned int _recordCount;
};

#endif
