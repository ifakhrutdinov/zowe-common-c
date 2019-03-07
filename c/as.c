

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
#include <metal/stdint.h>
#include <metal/stdio.h>
#include <metal/string.h>
#else
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#endif

#include "zowetypes.h"
#include "alloc.h"
#include "as.h"
#include "utils.h"

#ifndef __ZOWE_OS_ZOS
#error z/OS targets are supported only
#endif

/*

xlc "-Wa,goff" -DAS_TEST \
"-Wc,LANGLVL(EXTC99),FLOAT(HEX),agg,exp,list(),so(),goff,xref,gonum,roconst,gonum,ASM,ASMLIB('SYS1.MACLIB')" \
"-Wl,ac=1" -I ../h -o as as.c alloc.c utils.c timeutls.c zos.c ; extattr +a as

*/

static void sleep(int seconds){
  int waitValue = seconds * 100;
  __asm(" STIMER WAIT,BINTVL=%0\n" : : "m"(waitValue));
}

ZOWE_PRAGMA_PACK

typedef struct ASCrossMemoryInfo_tag {
  void * __ptr32 axlist;
  void * __ptr32 tklist;
  void * __ptr32 elxlist;
} ASCrossMemoryParms;

ZOWE_PRAGMA_PACK_RESET

#define AS_ATTR_XMPT      0x00800000
#define AS_ATTR_PXMT      0x00400000
#define AS_ATTR_NOMT      0x00200000
#define AS_ATTR_NOMD      0x00100000
#define AS_ATTR_1LPU      0x00080000
#define AS_ATTR_2LPU      0x00040000
#define AS_ATTR_N2LP      0x00020000
#define AS_ATTR_PRIV      0x00010000
#define AS_ATTR_NOSWAP    0x00008000
#define AS_ATTR_PERM      0x00004000
#define AS_ATTR_CANCEL    0x00002000
#define AS_ATTR_RESERVED  0x00001000
#define AS_ATTR_HIPRI     0x00000800
#define AS_ATTR_NONURG    0x00000400
#define AS_ATTR_KEEPRGN   0x00000200
#define AS_ATTR_REUSASID  0x00000100
#define AS_ATTR_JOBSPACE  0x00000080
#define AS_ATTR_ASCBV31   0x00000040
#define AS_ATTR_MAXRGN    0x00000020

typedef struct ASMacroParmList_tag {

  uint8_t version;
#define ASCRE_VERSION 1
  uint32_t attributes : 24;

  EightCharString * __ptr32 initRoutineName;
  uint32_t initRoutineNameALET;

  ASParmString * __ptr32 startParmString;
  uint32_t startParmStringALET;

  ASUserToken * __ptr32 utoken;
  uint32_t utokenALET;

  ASParm * __ptr32 parm;
  uint32_t parmALET;

  void * __ptr32 termRoutine;

  void * __ptr32 axlist;
  uint32_t axlistALET;
  void * __ptr32 tklist;
  uint32_t tklistALET;
  void * __ptr32 elxlist;
  uint32_t elxlistALET;

  ASOutputData * __ptr32 oda;
  uint32_t odaALET;

  char reserved[8];

} ASMacroParmList;

static int addressSpaceCreateInternal(const ASParmString *startParmString,
                                      const ASParm *parm,
                                      const EightCharString *initProgramName,
                                      void * __ptr32 termRoutine,
                                      ASUserToken *termRoutineUserToken,
                                      uint16_t attributes,
                                      ASCrossMemoryParms *xmemParms,
                                      ASOutputData *result,
                                      int *reasonCode) {

  ALLOC_STRUCT31(
    STRUCT31_NAME(below2G),
    STRUCT31_FIELDS(
      ASParmString startParmString;
      ASParm parm;
      EightCharString initProgramName;
      ASUserToken utoken;
      ASOutputData oda;
      ASMacroParmList parmList;
    )
  );

  /* Init 31-bit data */
  if (startParmString) {
    below2G->startParmString = *startParmString;
  }

  if (parm) {
    below2G->parm = *parm;
  }

  if (initProgramName) {
    below2G->initProgramName = *initProgramName;
  } else {
    EightCharString dummyInit = {"IEFBR14 "};
    below2G->initProgramName = dummyInit;
  }

  if (termRoutine && termRoutineUserToken) {
    below2G->utoken = *termRoutineUserToken;
  }

  /* Init ASCRE parameter list */
  ASMacroParmList * __ptr32 parmList = &below2G->parmList;
  parmList->version = ASCRE_VERSION;
  parmList->attributes = attributes;
  parmList->initRoutineName = &below2G->initProgramName;
  parmList->startParmString = &below2G->startParmString;
  parmList->utoken = termRoutine ? &below2G->utoken : NULL;
  parmList->parm = &below2G->parm;
  parmList->termRoutine = termRoutine;
  if (xmemParms) {
    parmList->axlist = xmemParms->axlist;
    parmList->tklist = xmemParms->tklist;
    parmList->elxlist = xmemParms->elxlist;
  }
  parmList->oda = &below2G->oda;

  int rc = 0, rsn = 0;
  __asm(
      ASM_PREFIX
      "L@ASCRE  DS    0H                                                       \n"
      "         PUSH  USING                                                    \n"
      "         DROP                                                           \n"
      "         LARL  10,L@ASCRE                                               \n"
      "         USING L@ASCRE,10                                               \n"

      "         ASCRE MF=(E,%[parmList])                                       \n"
      "         ST    15,%[rc]                                                 \n"
      "         ST    0,%[rsn]                                                 \n"

      "         POP   USING                                                    \n"
      : [rc]"=m"(rc), [rsn]"=m"(rsn)
      : [parmList]"m"(below2G->parmList)
      : "r0", "r1", "r10", "r14", "r15"
  );

  if (result) {
    *result = below2G->oda;
  }

  FREE_STRUCT31(
    STRUCT31_NAME(below2G)
  );
  below2G = NULL;

  if (reasonCode) {
    *reasonCode = rsn;
  }
  return rc;
}

int addressSpaceCreate(const ASParmString *startParmString,
                       const ASParm *parm,
                       uint32_t attributes,
                       ASOutputData *result,
                       int *reasonCode) {

  return addressSpaceCreateInternal(startParmString, parm, NULL, NULL, NULL,
                                    attributes, NULL, result, reasonCode);

}

int addressSpaceTerminate(uint64_t stoken, int *reasonCode) {

  ALLOC_STRUCT31(
    STRUCT31_NAME(below2G),
    STRUCT31_FIELDS(
      uint64_t stoken;
    )
  );

  below2G->stoken = stoken;

  int rc = 0, rsn = 0;
  __asm(
      ASM_PREFIX
      "L@ASDES  DS    0H                                                       \n"
      "         PUSH  USING                                                    \n"
      "         DROP                                                           \n"
      "         LARL  10,L@ASDES                                               \n"
      "         USING L@ASDES,10                                               \n"

      "         ASDES STOKEN=(%[stoken])                                       \n"
      "         ST    15,%[rc]                                                 \n"
      "         ST    0,%[rsn]                                                 \n"

      "         POP   USING                                                    \n"
      : [rc]"=m"(rc), [rsn]"=m"(rsn)
      : [stoken]"r"(&below2G->stoken)
      : "r0", "r1", "r10", "r14", "r15"
  );

  FREE_STRUCT31(
    STRUCT31_NAME(below2G)
  );
  below2G = NULL;

  if (reasonCode) {
    *reasonCode = rsn;
  }
  return rc;
}

int addressSpaceExtractParm(ASParm **parm, int *reasonCode) {

  ALLOC_STRUCT31(
    STRUCT31_NAME(below2G),
    STRUCT31_FIELDS(
      uint64_t stoken;
    )
  );

  ASParm *__ptr32 result = NULL;
  int rc = 0, rsn = 0;
  __asm(
      ASM_PREFIX
      "L@ASEXT  DS    0H                                                       \n"
      "         PUSH  USING                                                    \n"
      "         DROP                                                           \n"
      "         LARL  10,L@ASEXT                                               \n"
      "         USING L@ASEXT,10                                               \n"

      "         ASEXT ASPARM                                                   \n"
      "         ST    1,%[parm]                                                \n"
      "         ST    15,%[rc]                                                 \n"
      "         ST    0,%[rsn]                                                 \n"

      "         POP   USING                                                    \n"
      : [parm]"=m"(result), [rc]"=m"(rc), [rsn]"=m"(rsn)
      :
      : "r0", "r1", "r10", "r14", "r15"
  );

  FREE_STRUCT31(
    STRUCT31_NAME(below2G)
  );
  below2G = NULL;

  if (parm) {
    *parm = result;
  }

  if (reasonCode) {
    *reasonCode = rsn;
  }
  return rc;
}

#ifdef AS_TEST
int main() {
#else
static int not_main() {
#endif

  supervisorMode(TRUE);

  int rc = 0, rsn = 0;
  ASParmString startParmString = {.length = 7, .text = "ASTPROG"};
  ASParm parm = {.length = 6, .data = "hello!"};
  ASOutputData result = {0};

  rc = addressSpaceCreate(&startParmString, &parm, AS_ATTR_NOSWAP, &result, &rsn);
  printf("ASCRE RC = %d, RSN = %d\n", rc, rsn);
  printf("Output data:\n");
  dumpbuffer((char *)&result, sizeof(ASOutputData));

  sleep(5);

  rc = addressSpaceTerminate(result.stoken, &rsn);
  printf("ASDES RC = %d, RSN = %d\n", rc, rsn);

  sleep(10);

  return 0;
}


/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/

