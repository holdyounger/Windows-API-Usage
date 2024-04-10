/* Sha256.h -- SHA-256 Hash
2021-01-01 : Igor Pavlov : Public domain */

#ifndef __7Z_SHA256_H
#define __7Z_SHA256_H

#include "7zTypes.h"

EXTERN_C_BEGIN

#define SHA256_NUM_BLOCK_WORDS  16
#define SHA256_NUM_DIGEST_WORDS  8

#define SHA256_BLOCK_SIZE   (SHA256_NUM_BLOCK_WORDS * 4)
#define SHA256_DIGEST_SIZE  (SHA256_NUM_DIGEST_WORDS * 4)

typedef void (MY_FAST_CALL *SHA256_FUNC_UPDATE_BLOCKS)(UInt32 state[8], const Byte *data, size_t numBlocks);

/*
  if (the system supports different SHA256 code implementations)
  {
    (CSha256::func_UpdateBlocks) will be used
    (CSha256::func_UpdateBlocks) can be set by
       Sha256_Init()        - to default (fastest)
       Sha256_SetFunction() - to any algo
  }
  else
  {
    (CSha256::func_UpdateBlocks) is ignored.
  }
*/

typedef struct
{
  SHA256_FUNC_UPDATE_BLOCKS func_UpdateBlocks;
  UInt64 count;
  UInt64 __pad_2[2];
  UInt32 state[SHA256_NUM_DIGEST_WORDS];

  Byte buffer[SHA256_BLOCK_SIZE];
} CSha256;


#define SHA256_ALGO_DEFAULT 0
#define SHA256_ALGO_SW      1
#define SHA256_ALGO_HW      2

/*
Sha256_SetFunction()
return:
  0 - (algo) value is not supported, and func_UpdateBlocks was not changed
  1 - func_UpdateBlocks was set according (algo) value.
*/

BoolInt Sha256_SetFunction2(CSha256 *p, unsigned algo);

void Sha256_InitState2(CSha256 *p);
void Sha256_Init2(CSha256 *p);
void Sha256_Update2(CSha256 *p, const Byte *data, size_t size);
void Sha256_Final2(CSha256 *p, Byte *digest);




// void MY_FAST_CALL Sha256_UpdateBlocks(UInt32 state[8], const Byte *data, size_t numBlocks);

/*
call Sha256Prepare() once at program start.
It prepares all supported implementations, and detects the fastest implementation.
*/

void Sha256Prepare(void);

EXTERN_C_END

#endif
