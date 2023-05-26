 /*
  * Copyright (C) 2015 Southern Storm Software, Pty Ltd.
  *
  * Permission is hereby granted, free of charge, to any person obtaining a
  * copy of this software and associated documentation files (the "Software"),
  * to deal in the Software without restriction, including without limitation
  * the rights to use, copy, modify, merge, publish, distribute, sublicense,
  * and/or sell copies of the Software, and to permit persons to whom the
  * Software is furnished to do so, subject to the following conditions:
  *
  * The above copyright notice and this permission notice shall be included
  * in all copies or substantial portions of the Software.
  *
  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
  * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  * DEALINGS IN THE SOFTWARE.
  */
  
 #include "EAX.h"
 #include "Crypto.h"
 #include <string.h>
  
 EAXCommon::EAXCommon()
 {
     state.encPosn = 0;
     state.authMode = 0;
 }
  
 EAXCommon::~EAXCommon()
 {
     clean(state);
 }
  
 size_t EAXCommon::keySize() const
 {
     return omac.blockCipher()->keySize();
 }
  
 size_t EAXCommon::ivSize() const
 {
     // Can use any size but 16 is recommended.
     return 16;
 }
  
 size_t EAXCommon::tagSize() const
 {
     // Tags can be up to 16 bytes in length.
     return 16;
 }
  
 bool EAXCommon::setKey(const uint8_t *key, size_t len)
 {
     return omac.blockCipher()->setKey(key, len);
 }
  
 bool EAXCommon::setIV(const uint8_t *iv, size_t len)
 {
     // Must have at least 1 byte for the IV.
     if (!len)
         return false;
  
     // Hash the IV to create the initial nonce for CTR mode.  Also creates B.
     omac.initFirst(state.counter);
     omac.update(state.counter, iv, len);
     omac.finalize(state.counter);
  
     // The tag is initially the nonce value.  Will be XOR'ed with
     // the hash of the authenticated and encrypted data later.
     memcpy(state.tag, state.counter, 16);
  
     // Start the hashing context for the authenticated data.
     omac.initNext(state.hash, 1);
     state.encPosn = 16;
     state.authMode = 1;
  
     // The EAX context is ready to go.
     return true;
 }
  
 void EAXCommon::encrypt(uint8_t *output, const uint8_t *input, size_t len)
 {
     if (state.authMode)
         closeAuthData();
     encryptCTR(output, input, len);
     omac.update(state.hash, output, len);
 }
  
 void EAXCommon::decrypt(uint8_t *output, const uint8_t *input, size_t len)
 {
     if (state.authMode)
         closeAuthData();
     omac.update(state.hash, input, len);
     encryptCTR(output, input, len);
 }
  
 void EAXCommon::addAuthData(const void *data, size_t len)
 {
     if (state.authMode)
         omac.update(state.hash, (const uint8_t *)data, len);
 }
  
 void EAXCommon::computeTag(void *tag, size_t len)
 {
     closeTag();
     if (len > 16)
         len = 16;
     memcpy(tag, state.tag, len);
 }
  
 bool EAXCommon::checkTag(const void *tag, size_t len)
 {
     // Can never match if the expected tag length is too long.
     if (len > 16)
         return false;
  
     // Compute the final tag and check it.
     closeTag();
     return secure_compare(state.tag, tag, len);
 }
  
 void EAXCommon::clear()
 {
     clean(state);
 }
  
 void EAXCommon::closeAuthData()
 {
     // Finalise the OMAC hash and XOR it with the final tag.
     omac.finalize(state.hash);
     for (uint8_t index = 0; index < 16; ++index)
         state.tag[index] ^= state.hash[index];
     state.authMode = 0;
  
     // Initialise the hashing context for the ciphertext data.
     omac.initNext(state.hash, 2);
 }
  
 void EAXCommon::encryptCTR(uint8_t *output, const uint8_t *input, size_t len)
 {
     while (len > 0) {
         // Do we need to start a new block?
         if (state.encPosn == 16) {
             // Encrypt the counter to create the next keystream block.
             omac.blockCipher()->encryptBlock(state.stream, state.counter);
             state.encPosn = 0;
  
             // Increment the counter, taking care not to reveal
             // any timing information about the starting value.
             // We iterate through the entire counter region even
             // if we could stop earlier because a byte is non-zero.
             uint16_t temp = 1;
             uint8_t index = 16;
             while (index > 0) {
                 --index;
                 temp += state.counter[index];
                 state.counter[index] = (uint8_t)temp;
                 temp >>= 8;
             }
         }
  
         // Encrypt/decrypt the current input block.
         uint8_t size = 16 - state.encPosn;
         if (size > len)
             size = (uint8_t)len;
         for (uint8_t index = 0; index < size; ++index)
             output[index] = input[index] ^ state.stream[(state.encPosn)++];
  
         // Move onto the next block.
         len -= size;
         input += size;
         output += size;
     }
 }
  
 void EAXCommon::closeTag()
 {
     // If we were only authenticating, then close off auth mode.
     if (state.authMode)
         closeAuthData();
  
     // Finalise the hash over the ciphertext and XOR with the final tag.
     omac.finalize(state.hash);
     for (uint8_t index = 0; index < 16; ++index)
         state.tag[index] ^= state.hash[index];
 }
