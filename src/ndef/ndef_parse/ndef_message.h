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
#include <iostream>
#include <sstream>
#include <string>

#include "../../data/ndef_content.h"

#include "ndef_record.h"

namespace matrix_hal {

#define MAX_NDEF_RECORDS 4

class NDEFMessage {
   public:
    NDEFMessage(void);
    NDEFMessage(const matrix_hal::NDEFContent* content);
    NDEFMessage(const uint8_t* data, const int numBytes);
    NDEFMessage(const NDEFMessage& rhs);
    ~NDEFMessage();
    NDEFMessage& operator=(const NDEFMessage& rhs);

    int GetEncodedSize();  // need so we can pass array to Encode
    void Encode(uint8_t* data);

    bool AddRecord(NDEFRecord& record);
    void AddMimeMediaRecord(std::string mimeType, std::string payload);
    void AddMimeMediaRecord(std::string mimeType, uint8_t* payload,
                            int payloadLength);
    void AddTextRecord(std::string text);
    void AddTextRecord(std::string text, std::string encoding);
    void AddUriRecord(std::string uri);
    void AddEmptyRecord();

    unsigned int GetRecordCount();
    NDEFRecord GetRecord(int index);
    NDEFRecord operator[](int index);

    std::string ToString();

   private:
    NDEFRecord _records[MAX_NDEF_RECORDS];
    unsigned int _recordCount;
};

}  // namespace matrix_hal

#endif
