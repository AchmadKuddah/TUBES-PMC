// blowfish.h interface file for blowfish.cpp
// _THE BLOWFISH ENCRYPTION ALGORITHM_
// by Bruce Schneier
// Revised code--3/20/94
// Converted to C++ class 5/96, Jim Conger

#define MAXKEYBYTES 56 // 448 bits max
#define NPASS 16 // SBox passes

#define DWORD unsigned long
#define WORD unsigned short
#define BYTE unsigned char

class CBlowFish {
  private:
    DWORD *PArray;
    DWORD(*SBoxes)[256];
    void Blowfish_encipher(DWORD *xl, DWORD *xr);
    void Blowfish_decipher(DWORD *xl, DWORD *xr);

  public:
    CBlowFish();
    ~CBlowFish();
    void Initialize(BYTE key[], int keybytes);
    DWORD GetOutputLength(DWORD lInputLong);
    DWORD Encode(BYTE *pInput, BYTE *pOutput, DWORD lSize);
    void Decode(BYTE *pInput, BYTE *pOutput, DWORD lSize);
};

// choose a byte order for your hardware
#define ORDER_DCBA // chosing Intel in this case

#ifdef ORDER_DCBA // DCBA - little endian - intel
union aword {
  DWORD dword;
  BYTE byte[4];
  struct {
    unsigned int byte3: 8;
    unsigned int byte2: 8;
    unsigned int byte1: 8;
    unsigned int byte0: 8;
  } w;
};
#endif

#ifdef ORDER_ABCD // ABCD - big endian - motorola
union aword {
  DWORD dword;
  BYTE byte[4];
  struct {
    unsigned int byte0: 8;
    unsigned int byte1: 8;
    unsigned int byte2: 8;
    unsigned int byte3: 8;
  } w;
};
#endif

#ifdef ORDER_BADC // BADC - vax
union aword {
  DWORD dword;
  BYTE byte[4];
  struct {
    unsigned int byte1: 8;
    unsigned int byte0: 8;
    unsigned int byte3: 8;
    unsigned int byte2: 8;
  } w;
};
#endif
// blowfish.h2  header file containing random number tables

static DWORD bf_P[NPASS + 2]  = {
  0x243f6a88, 0x85a308d3, 0x13198a2e, 0x03707344,
  0xa4093822, 0x299f31d0, 0x082efa98, 0xec4e6c89,
  0x452821e6, 0x38d01377, 0xbe5466cf, 0x34e90c6c,
  0xc0ac29b7, 0xc97c50dd, 0x3f84d5b5, 0xb5470917,
  0x9216d5d9, 0x8979fb1b,
};
static DWORD bf_S[4][32] = {
 0x1b0a7441, 0x4ba3348c, 0xc5be7120, 0xc37632d8,
  0xdf359f8d, 0x9b992f2e, 0xe60b6f47, 0x0fe3f11d,
  0xe54cda54, 0x1edad891, 0xce6279cf, 0xcd3e7e6f,
  0x1618b166, 0xfd2c1d05, 0x848fd2c5, 0xf6fb2299,
  0xf523f357, 0xa6327623, 0x93a83531, 0x56cccd02,
  0xacf08162, 0x5a75ebb5, 0x6e163697, 0x88d273cc,
  0xde966292, 0x81b949d0, 0x4c50901b, 0x71c65614,
  0xe6c6c7bd, 0x327a140a, 0x45e1d006, 0xc3f27b9a,
  0xc9aa53fd, 0x62a80f00, 0xbb25bfe2, 0x35bdd2f6,
  0x71126905, 0xb2040222, 0xb6cbcf7c, 0xcd769c2b,
  0x53113ec0, 0x1640e3d3, 0x38abbd60, 0x2547adf0,
  0xba38209c, 0xf746ce76, 0x77afa1c5, 0x20756060,
  0x85cbfe4e, 0x8ae88dd8, 0x7aaaf9b0, 0x4cf9aa7e,
  0x1948c25c, 0x02fb8a8c, 0x01c36ae4, 0xd6ebe1f9,
  0x90d4f869, 0xa65cdea0, 0x3f09252d, 0xc208e69f,
  0xb74e6132, 0xce77e25b, 0x578fdfe3, 0x3ac372e6,
};

// blowfish.cpp C++ class implementation of the BLOWFISH encryption algorithm
// _THE BLOWFISH ENCRYPTION ALGORITHM_
// by Bruce Schneier
// Revised code--3/20/94
// Converted to C++ class 5/96, Jim Conger

#define S(x,i)(SBoxes[i][x.w.byte##i])
#define bf_F(x)(((S(x,0) + S(x,1)) ^ S(x,2)) + S(x,3))
#define ROUND(a,b,n)(a.dword ^= bf_F(b) ^ PArray[n])

CBlowFish::CBlowFish() {
  PArray = new DWORD[18];
  SBoxes = new DWORD[4][256];
}

CBlowFish::~CBlowFish() {
  delete PArray;
  delete[] SBoxes;
}

// the low-level (private) encryption function
void CBlowFish::Blowfish_encipher(DWORD *xl, DWORD *xr) {
  union aword Xl, Xr;
  Xl.dword = *xl;
  Xr.dword = *xr;
  Xl.dword ^= PArray[0];
  ROUND(Xr, Xl, 1); ROUND(Xl, Xr, 2);
  ROUND(Xr, Xl, 3); ROUND(Xl, Xr, 4);
  ROUND(Xr, Xl, 5); ROUND(Xl, Xr, 6);
  ROUND(Xr, Xl, 7); ROUND(Xl, Xr, 8);
  ROUND(Xr, Xl, 9); ROUND(Xl, Xr, 10);
  ROUND(Xr, Xl, 11); ROUND(Xl, Xr, 12);
  ROUND(Xr, Xl, 13); ROUND(Xl, Xr, 14);
  ROUND(Xr, Xl, 15); ROUND(Xl, Xr, 16);
  Xr.dword ^= PArray[17];
  *xr = Xl.dword;
  *xl = Xr.dword;
}

// the low-level (private) decryption function
void CBlowFish::Blowfish_decipher(DWORD *xl, DWORD *xr) {
  union aword Xl;
  union aword Xr;
  Xl.dword = *xl;
  Xr.dword = *xr;
  Xl.dword ^= PArray[17];
  ROUND(Xr, Xl, 16); ROUND(Xl, Xr, 15);
  ROUND(Xr, Xl, 14); ROUND(Xl, Xr, 13);
  ROUND(Xr, Xl, 12); ROUND(Xl, Xr, 11);
  ROUND(Xr, Xl, 10); ROUND(Xl, Xr, 9);
  ROUND(Xr, Xl, 8); ROUND(Xl, Xr, 7);
  ROUND(Xr, Xl, 6); ROUND(Xl, Xr, 5);
  ROUND(Xr, Xl, 4); ROUND(Xl, Xr, 3);
  ROUND(Xr, Xl, 2); ROUND(Xl, Xr, 1);
  Xr.dword ^= PArray[0];
  *xl = Xr.dword;
  *xr = Xl.dword;
}

// constructs the enctryption sieve
void CBlowFish::Initialize(BYTE key[], int keybytes) {
  int i, j;
  DWORD data, datal, datar;
  union aword temp;
  // first fill arrays from data tables
  for (i = 0; i < 18; i++)
    PArray[i] = bf_P[i];
  for (i = 0; i < 4; i++) {
    for (j = 0; j < 256; j++)
      SBoxes[i][j] = bf_S[i][j];
  }

  j = 0;
  for (i = 0; i < NPASS + 2; ++i) {
    temp.dword = 0;
    temp.w.byte0 = key[j];
    temp.w.byte1 = key[(j + 1) % keybytes];
    temp.w.byte2 = key[(j + 2) % keybytes];
    temp.w.byte3 = key[(j + 3) % keybytes];
    data = temp.dword;
    PArray[i] ^= data;
    j = (j + 4) % keybytes;
  }
  datal = 0;
  datar = 0;
  for (i = 0; i < NPASS + 2; i += 2) {
    Blowfish_encipher(&datal, &datar);
    PArray[i] = datal;
    PArray[i + 1] = datar;
  }
  for (i = 0; i < 4; ++i) {
    for (j = 0; j < 256; j += 2) {
      Blowfish_encipher(&datal, &datar);
      SBoxes[i][j] = datal;
      SBoxes[i][j + 1] = datar;
    }
  }
}

// get output length, which must be even MOD 8
DWORD CBlowFish::GetOutputLength(DWORD lInputLong) {
  DWORD lVal;
  lVal = lInputLong % 8 ; // find out if there's an uneven number of bytes at the end
  if (lVal != 0) return lInputLong + 8 - lVal;
  else return lInputLong;
}

// Encode pIntput into pOutput. Input length in lSize. Returned value
// is length of output which will be even MOD 8 bytes. Input buffer and
// output buffer can be the same, but be sure buffer length is even MOD 8.
DWORD CBlowFish::Encode(BYTE * pInput, BYTE * pOutput, DWORD lSize) {
  DWORD lCount, lOutSize, lGoodBytes;
  BYTE *pi, *po;
  int i, j;
  int SameDest = (pInput == pOutput ? 1 : 0);
  lOutSize = GetOutputLength(lSize);
  for (lCount = 0; lCount < lOutSize; lCount += 8) {
    if (SameDest) {
      // if encoded data is being written into input buffer
      if (lCount < lSize - 7) {
        // if not dealing with uneven bytes at end
        Blowfish_encipher((DWORD *) pInput, (DWORD *)(pInput + 4));
      } else {
        // pad end of data with null bytes to complete encryption
        po = pInput + lSize ; // point at byte past the end of actual data
        j = (int)(lOutSize - lSize) ; // number of bytes to set to null
        for (i = 0; i < j; i++)
          *po++ = 0;
        Blowfish_encipher((DWORD *) pInput, (DWORD *)(pInput + 4));
      }
      pInput += 8;
    } else {
      // output buffer not equal to input buffer, so must copy
      // input to output buffer prior to encrypting
      if (lCount < lSize - 7) {
        // if not dealing with uneven bytes at end
        pi = pInput;
        po = pOutput;
        for (i = 0; i < 8; i++)
          // copy bytes to output
          *po++ = *pi++;
        Blowfish_encipher((DWORD *) pOutput, (DWORD *)(pOutput + 4));
        // now encrypt them
      } else {
        // pad end of data with null bytes to complete encryption
        lGoodBytes = lSize - lCount ; // number of remaining data bytes
        po = pOutput;
        for (i = 0; i < (int) lGoodBytes; i++)
          *po++ = *pInput++;
        for (j = i; j < 8; j++)
          *po++ = 0;
        Blowfish_encipher((DWORD *) pOutput, (DWORD *)(pOutput + 4));
      }
      pInput += 8;
      pOutput += 8;
    }
  }
  return lOutSize;
}

// Decode pIntput into pOutput. Input length in lSize. Input buffer and
// output buffer can be the same, but be sure buffer length is even MOD 8.
void CBlowFish::Decode(BYTE * pInput, BYTE * pOutput, DWORD lSize) {
  DWORD lCount;
  BYTE *pi, *po;
  int i;
  int SameDest = (pInput == pOutput ? 1 : 0);
  for (lCount = 0; lCount < lSize; lCount += 8) {
    if (SameDest) {
      // if encoded data is being written into input buffer
      Blowfish_decipher((DWORD *) pInput, (DWORD *)(pInput + 4));
      pInput += 8;
    } else {
      // output buffer not equal to input buffer
      // so copy input to output before decoding
      pi = pInput;
      po = pOutput;
      for (i = 0; i < 8; i++) *po++ = *pi++;
      Blowfish_decipher((DWORD *) pOutput, (DWORD *)(pOutput + 4));
      pInput += 8;
      pOutput += 8;
    }
  }
}
