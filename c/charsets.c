

/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html
  
  SPDX-License-Identifier: EPL-2.0
  
  Copyright Contributors to the Zowe Project.
*/

#ifdef METTLE 
#include <metal/metal.h>
#include <metal/stddef.h>
#include <metal/stdio.h>
#include <metal/stdlib.h>
#include <metal/string.h>
#include <metal/stdarg.h>  
#include "metalio.h"

#else
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>

#endif /* METTLE */

#include "zowetypes.h"
#include "alloc.h"
#include "utils.h"
#include "charsets.h"

#ifdef __ZOWE_OS_WINDOWS

/*
   Windows doc 

   int MultiByteToWideChar(
   _In_      UINT   CodePage,          // expected
   _In_      DWORD  dwFlags,           // composite|precomposed|failOnInputError
   _In_      LPCSTR lpMultiByteStr,    // inputString
   _In_      int    cbMultiByte,       // length of inputString or -1 if nullTerminated and feeling adventurous
   _Out_opt_ LPWSTR lpWideCharStr,     // receivingBuffer (or NULL for brave souls who want Windows to allocate)
   _In_      int    cchWideChar        // length in WideChars of output
  );

  IBM resources
    http://www-01.ibm.com/software/globalization/ccsid/ccsid_registered.html

  HERE
  1) what does 1200 really mean
  2) wcTest -> convert to wideNative
  3) 
*/

int charsetToWideNative(char  *input, 
                        int    inputLength, 
                        int    inputCCSID,
                        int    outputMode,
                        char **outputArg,
                        int    outputLength,
                        ShortLivedHeap *slh,
                        int   *conversionLength,
                        int   *reasonCode){
  LPWSTR outputBuffer;
  if (outputMode == CHARSET_OUTPUT_USE_BUFFER){
    if (outputLength < 2*inputLength){
      return CHARSET_SHORT_BUFFER;
    }
    outputBuffer = *((LPWSTR*)outputArg);
  } else if (outputMode == CHARSET_OUTPUT_SAFE_MALLOC){
    outputLength = 3*inputLength;
    outputBuffer = (LPWSTR)safeMalloc(outputLength,"Conversion Temp Buffer");
  } else {
    outputLength = 3*inputLength;
    outputBuffer = (LPWSTR)SLHAlloc(slh,outputLength);
  }
  int status = MultiByteToWideChar(inputCCSID,0,input,inputLength,outputBuffer,outputLength);
  if (status == 0){
    *reasonCode = GetLastError();
    if (outputMode == CHARSET_OUTPUT_SAFE_MALLOC){
      safeFree((char*)outputBuffer,outputLength);
    }
    return CHARSET_CONVERSION_ROUTINE_FAILURE;
  } else {
    *conversionLength = status*2;                      /* 2 because windows returns size in "wide chars" */
    if (outputMode == CHARSET_OUTPUT_SAFE_MALLOC){
      char *finalOutput = safeMalloc(*conversionLength,"Conversion Final Buffer");
      memcpy(finalOutput,(char*)outputBuffer,*conversionLength);
      safeFree((char*)outputBuffer,outputLength);
      *outputArg = finalOutput;
      return CHARSET_CONVERSION_SUCCESS;
    } else {
      *outputArg = (char*)outputBuffer;
      return CHARSET_CONVERSION_SUCCESS;
    }
  }
}

int convertCharset(const char *input,
                   int inputLength, 
                   int inputCCSID,
                   int outputMode,
                   char **output, 
                   int outputLength, 
                   int outputCCSID,
                   ShortLivedHeap *slh, // optional
                   int *conversionOutputLength,
                   int *reasonCode){
  
  return 0;
}

#elif defined(__ZOWE_OS_ZOS)

static
const unsigned char EBCDIC_TO_ASCII[256] = {
/*         0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F */
/* 0 */ 0x00, 0x01, 0x02, 0x03, 0x9C, 0x09, 0x86, 0x7F, 0x97, 0x8D, 0x8E, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
/* 1 */ 0x10, 0x11, 0x12, 0x13, 0x9D, 0x85, 0x08, 0x87, 0x18, 0x19, 0x92, 0x8F, 0x1C, 0x1D, 0x1E, 0x1F,
/* 2 */ 0x80, 0x81, 0x82, 0x83, 0x84, 0x0A, 0x17, 0x1B, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x05, 0x06, 0x07,
/* 3 */ 0x90, 0x91, 0x16, 0x93, 0x94, 0x95, 0x96, 0x04, 0x98, 0x99, 0x9A, 0x9B, 0x14, 0x15, 0x9E, 0x1A,
/* 4 */ 0x20, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x2E, 0x3C, 0x28, 0x2B, 0x7C,
/* 5 */ 0x26, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x21, 0x24, 0x2A, 0x29, 0x3B, 0x5E,
/* 6 */ 0x2D, 0x2F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x2C, 0x25, 0x5F, 0x3E, 0x3F,
/* 7 */ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x60, 0x3A, 0x23, 0x40, 0x27, 0x3D, 0x22,
/* 8 */ 0xFF, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
/* 9 */ 0xFF, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
/* A */ 0xFF, 0x7E, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0xFF, 0xFF, 0xFF, 0x5B, 0xFF, 0xFF,
/* B */ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x5D, 0xFF, 0xFF,
/* C */ 0x7B, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
/* D */ 0x7D, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
/* E */ 0x5C, 0xFF, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
/* F */ 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

#define EBCDIC_TERM_CHAR 0xFF

inline static size_t troo(void *dest, const void *src, size_t len,
                          const void *table, char term) {

  size_t newLen = len;

  __asm(
      ASM_PREFIX

      "&LX      SETA  &LX+1                                                    \n"
      "&LXTR    SETC  'LXTR&LX'                                                \n"

      "&LXTR    DS    0H                                                       \n"
      "         TROO  %[dest],%[src]                                           \n"
      "         JO    &LXTR                                                    \n"

      : "+NR:r9"(newLen)
      : "NR:r1"(table), [dest]"NR:r8"(dest), [src]"r"(src), "NR:r0"(term)
      :
  );

  return len - newLen;
}

#include "//'SYS1.SCUNHF(CUNHC)'"

static inline int callCUNLCNV(const char *input, int inputLength, int inputCCSID,
                              char *output, int outputLength, int outputCCSID,
                              int *conversionOutputLength) {

  unsigned char dda [CUNBCPRM_DDA_REQ];
  unsigned char workBuffer [4096];
#ifdef _LP64
  CUN4BCPR parms = { CUNBCPRM_DEFAULT };
#else
  CUNBCPRM parms ={ CUNBCPRM_DEFAULT};
#endif

  parms.Src_Buf_Ptr = (void *)input;
  parms.Targ_Buf_Ptr = output;
  parms.Targ_Buf_Len = outputLength;
  parms.Src_Buf_Len = inputLength;
  parms.Src_CCSID = inputCCSID;
  parms.Targ_CCSID = outputCCSID;
  memcpy(parms.Technique, "LMER", 4);
  parms.Wrk_Buf_Ptr = workBuffer;
  parms.Wrk_Buf_Len = sizeof(workBuffer);
  parms.DDA_Buf_Ptr = dda;
  parms.DDA_Buf_Len = CUNBCPRM_DDA_REQ;

  if (TRACE_CHARSET_CONVERSION) {
    printf("Before CUNLCNV parms\n");
#ifdef _LP64
    dumpbuffer((char *)&parms, sizeof(CUN4BCPR));
#else
    dumpbuffer((char *)&parms,sizeof(CUNBCPRM));
#endif
  }

#ifdef _LP64
  CUN4LCNV(&parms);
#else
  CUNLCNV(&parms);
#endif

  *conversionOutputLength = ((char *)parms.Targ_Buf_Ptr) - output;

  return parms.Return_Code;
}

int convertCharset(const char *input,
                   int inputLength, 
                   int inputCCSID,
                   int outputMode,
                   char **output, 
                   int outputLength, 
                   int outputCCSID,
                   ShortLivedHeap *slh, // optional
                   int *conversionOutputLength, 
                   int *reasonCode){

  int outputAllocLength = 3*inputLength; /* paranoia */
  char *outputBuffer;

  switch (outputMode){
  case CHARSET_OUTPUT_USE_BUFFER:
    if (outputLength < 2*inputLength){
      return CHARSET_SHORT_BUFFER;
    }
    outputBuffer = *output;
    break;
  case CHARSET_OUTPUT_SAFE_MALLOC:
    outputBuffer = safeMalloc(outputAllocLength,"Conversion Buffer");
    if (outputBuffer == NULL) {
      return CHARSET_ALLOC_FAILED;
    }
    outputLength = outputAllocLength;
    break;
  case CHARSET_OUTPUT_USE_SLH:
    outputBuffer = SLHAlloc(slh,outputAllocLength);
    if (outputBuffer == NULL) {
      return CHARSET_ALLOC_FAILED;
    }
    outputLength = outputAllocLength;
    break;
  default:
    return CHARSET_INTERNAL_ERROR;
  }

  int totalConvLen = 0;
  int fastPathConvLen = 0;
  int slowPathConvLen = 0;

  if (inputCCSID == CCSID_IBM1047 && outputCCSID == CCSID_UTF_8) {
    fastPathConvLen = troo(outputBuffer, input, inputLength,
                           EBCDIC_TO_ASCII, EBCDIC_TERM_CHAR);
  }

  int returnCode = CHARSET_CONVERSION_SUCCESS;

  if (fastPathConvLen != inputLength) {

    const char *slowPathIn = input + fastPathConvLen;
    int slowPathInLen = inputLength - fastPathConvLen;
    char *slowPathOut = outputBuffer + fastPathConvLen;
    int slowPathOutLen = outputLength - fastPathConvLen;

    int slowRC = callCUNLCNV(slowPathIn, slowPathInLen, inputCCSID,
                             slowPathOut, slowPathOutLen, outputCCSID,
                             &slowPathConvLen);
    if (slowRC != 0) {
      returnCode = CHARSET_CONVERSION_ROUTINE_FAILURE;
      *reasonCode = slowRC;
    }

  }

  totalConvLen = fastPathConvLen + slowPathConvLen;

  switch (outputMode) {
  case CHARSET_OUTPUT_USE_BUFFER:
    break;
  case CHARSET_OUTPUT_SAFE_MALLOC:
  {
    char *result = safeMalloc(totalConvLen, "Converted Buffer");
    if (result != NULL) {
      memcpy(result, outputBuffer, totalConvLen);
    } else {
      totalConvLen = 0;
      returnCode = CHARSET_ALLOC_FAILED;
    }
    safeFree(outputBuffer, outputAllocLength);
    outputBuffer = NULL;

    *output = result;
  }
  break;
  case CHARSET_OUTPUT_USE_SLH:
    break;
  }

  *conversionOutputLength = totalConvLen;

  return returnCode;
}

#define CHARSETNAME_SIZE 15
int getCharsetCode(const char *charsetName) {
  char localArray[CHARSETNAME_SIZE + 1] = {0};

  /* Check for null pointer */
  if (charsetName == NULL) {
    return -1;
  }

  /* Make sure last element is 0 */
  if (strlen(charsetName) > CHARSETNAME_SIZE) {
    return -1;
  }
# undef CHARSETNAME_SIZE
  strcpy( localArray, charsetName);
  strupcase (localArray);

  if ((!strcmp(localArray, "ISO-8859-1"))  ||
      (!strcmp(localArray, "ISO8859-1"))) {
    return CCSID_ISO_8859_1;
  }
  else if (!strcmp(localArray, "IBM-1047")) {
    return CCSID_IBM1047;
  }
  else if (!strcmp(localArray, "UTF-8")) {
    return CCSID_UTF_8;
  }
  else if (!strcmp(localArray, "UTF-16")) {
    return CCSID_UTF_16;
  }
  else if (!strcmp(localArray, "UTF-16BE")) {
    return CCSID_UTF_16_BE;
  }
  else if (!strcmp(localArray, "UTF-16LE")) {
    return CCSID_UTF_16_LE;
  }
  else {
    return -1;
  }
}

#elif defined(__ZOWE_OS_LINUX) || defined(__ZOWE_OS_AIX) /* end of ZOWE_OS_ZOS */

#include <iconv.h>
#include <errno.h>


/*
  ICONV names

  I have gone with the IBM ccsid numbers to name some iconv_charsets 

  what is "WCHAR_T" (is it UTF16 (BE/LE)?) 

  NOTE: iconv_open is EXPENSIVE. It would make sense to keep a dictionary of
        already-opened converters and reuse them.
 */

static const char *getCharsetName(int ibmCode){
  switch (ibmCode){
  case CCSID_ISO_8859_1:
    return "ISO-8859-1";
  case CCSID_IBM1047:
    return "IBM-1047";
  case CCSID_UTF_8:
    return "UTF-8";
  case CCSID_UTF_16:
    return "UTF-16";
  case CCSID_UTF_16_BE:
    return "UTF-16BE";
  case CCSID_UTF_16_LE:
    return "UTF-16LE";
  default:
    return NULL;
  }
}

int convertCharset(const char *input,
                   int inputLength, 
                   int inputCCSID,
                   int outputMode,
                   char **output, 
                   int outputLength, 
                   int outputCCSID,
                   ShortLivedHeap *slh, // optional
                   int *conversionOutputLength, 
                   int *reasonCode){
  *reasonCode  = 0;

  const char *inputCharset = getCharsetName(inputCCSID);
  const char *outputCharset = getCharsetName(outputCCSID);
  if ((inputCharset == NULL) || (outputCharset == NULL)){
    return CHARSET_UNKNOWN_CCSID;
  }

  iconv_t converter = iconv_open (outputCharset, inputCharset);
  if (converter == (iconv_t) -1){
    *reasonCode = errno;
    return CHARSET_CONVERSION_UNIMPLEMENTED;
  }

  const char* inputBuffer = input;
  size_t inputSize = (size_t)inputLength;

  char *outputBuffer = NULL;
  size_t outputSize = 0;

  size_t outputAllocLength = 3*inputSize; /* paranoia (but could ACTUALLY be 4*inputSize!) */

  switch (outputMode){
  case CHARSET_OUTPUT_USE_BUFFER:
    outputBuffer = *output;
    outputSize = (size_t)outputLength;
    outputAllocLength = 0;
    break;
  case CHARSET_OUTPUT_SAFE_MALLOC:
    outputBuffer = safeMalloc(outputAllocLength,"Conversion Buffer");
    outputSize = outputAllocLength;
    break;
  case CHARSET_OUTPUT_USE_SLH:
    outputBuffer = SLHAlloc(slh,outputAllocLength);
    outputSize = outputAllocLength;
    break;
  default:
    return CHARSET_INTERNAL_ERROR;
  }

  /* size_t iconv (iconv_t cd, char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft) */
  char* outputBufferStart = outputBuffer;
  int result = CHARSET_CONVERSION_SUCCESS;

  size_t iconv_status = iconv(converter, (char **)&inputBuffer, &inputSize,
                              &outputBuffer, &outputSize);
  iconv_close(converter);

  if (iconv_status == -1){
    switch(errno) {
    case E2BIG: 
      result = CHARSET_SHORT_BUFFER;
      break;

    case EILSEQ:
    case EINVAL:
      result = CHARSET_CONVERSION_ROUTINE_FAILURE;
      break;

    default:
      result = CHARSET_INTERNAL_ERROR;
      break;
    }
    switch(outputMode) {
    case CHARSET_OUTPUT_USE_BUFFER:
    case CHARSET_OUTPUT_USE_SLH:
      break;
    case CHARSET_OUTPUT_SAFE_MALLOC:
      safeFree(outputBufferStart,outputAllocLength);
      break;
    }
  } else {
    result = CHARSET_CONVERSION_SUCCESS;
    size_t actualOutputLength = outputBuffer-outputBufferStart;
    *conversionOutputLength = (int) actualOutputLength;
    switch(outputMode) {
    case CHARSET_OUTPUT_USE_BUFFER:
    case CHARSET_OUTPUT_USE_SLH:
      *output = outputBufferStart;
      break;
    case CHARSET_OUTPUT_SAFE_MALLOC:
      /* PITA - safeFree has to be called with the size of the buffer,
         so if we didn't use all of (which is unlikely...), we have
         to make yet another copy and return the pointer to that. */
      *output = safeMalloc(*conversionOutputLength,"Converted Buffer");
      memcpy(*output,outputBufferStart,actualOutputLength);
      safeFree(outputBufferStart,outputAllocLength);
      break;
    }
  }
  return result;
}
#else  
#error Unknown OS
#endif




/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html
  
  SPDX-License-Identifier: EPL-2.0
  
  Copyright Contributors to the Zowe Project.
*/

