

/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/

#ifdef METTLE
#include <metal/metal.h>
#include <metal/stdio.h>
#else
#include <stdio.h>
#endif

#include "zowetypes.h"
#include "alloc.h"
#include "utils.h"
#include "zos.h"

#ifndef __ZOWE_OS_ZOS
#error z/OS targets are supported only
#endif

/*

xlc "-Wa,goff" \
"-Wc,LANGLVL(EXTC99),FLOAT(HEX),agg,exp,list(),so(),goff,xref,gonum,roconst,gonum,ASM,ASMLIB('SYS1.MACLIB'),LP64,XPLINK" \
"-Wl,ac=1" -I ../h -o ucbtest \
alloc.c \
utils.c \
timeutls.c \
zos.c \
ucbtest.c \

*/

#define RC_OK       0
#define RC_FAILURE  8

/* See z/OS MVS Data Areas Volume 4 (SCF - XTL) for details */
typedef struct UCBCommon_tag {
  uint8_t flags1;
  uint8_t flags2;
  uint8_t id;
  uint8_t status;
  /* UCBSTAT values */
#define UCB_COMMON_STATUS_ONLI        0x80 /* Volume is online */
#define UCB_COMMON_STATUS_CHGS        0x40 /* Online -> offline is in progress*/
#define UCB_COMMON_STATUS_RESV        0x20
#define UCB_COMMON_STATUS_UNLD        0x10
#define UCB_COMMON_STATUS_ALOC        0x08
#define UCB_COMMON_STATUS_PRES        0x04
#define UCB_COMMON_STATUS_SYSR        0x02
#define UCB_COMMON_STATUS_DADI        0x01
  int16_t deviceNumber;
  uint8_t ioFlagA;
  uint8_t ioFlagB;
  void * __ptr32 nextUCB;
  uint8_t flags3;
  char deviceNumberString[3];
  uint8_t modelBits;
  uint8_t optionFlags;
  uint8_t classBits;
  char deviceCode;
  uint8_t ioFlagC;
  uint32_t extensionAddress : 24;
} UCBCommon;

static int getUCBCommonCopy(const char *volser, UCBCommon *result, int *reasonCode) {

#define UCBSCAN_WORKAREA_SIZE     100 /* from the doc */
#define UCBSCAN_PARM_LIST_SIZE    116 /* from UCBSCAN */
#define UCBSCAN_DEV_SEGMENT_SIZE  24  /* from the doc */

  typedef struct UCBArea_tag {
    UCBCommon ucbCommon;
    char ucbDeviceArea[UCBSCAN_DEV_SEGMENT_SIZE];
  } UCBArea;

  ALLOC_STRUCT31(
    STRUCT31_NAME(below2G),
    STRUCT31_FIELDS(
      unsigned char workArea[UCBSCAN_WORKAREA_SIZE];
      UCBArea ucb;
      char volser[6];
      unsigned char parmList[UCBSCAN_PARM_LIST_SIZE];
      int32_t rc;
      int32_t rsn;
    )
  );

#undef UCBSCAN_WORKAREA_SIZE
#undef UCBSCAN_PARM_LIST_SIZE
#undef UCBSCAN_DEV_SEGMENT_SIZE

  memset(below2G->volser, ' ', sizeof(below2G->volser));
  memcpy(below2G->volser, volser, min(sizeof(below2G->volser), strlen(volser)));

  __asm(
      ASM_PREFIX
#ifdef _LP64
      "         SAM31                                                          \n"
      "         SYSSTATE AMODE64=NO                                            \n"
#endif
      "         UCBSCAN COPY,WORKAREA=%[wka],UCBAREA=%[ucb]"
      ",CMXTAREA=NONE"
      ",UCBPAREA=NONE"
      ",DCEAREA=NONE"
      ",DCELEN=0"
      ",VOLSER=%[volser]"
      ",DYNAMIC=YES"
      ",RANGE=ALL"
      ",DEVCLASS=DASD"
      ",PLISTVER=1"
      ",MF=(E,%[plist],COMPLETE)"
      ",RETCODE=%[rc]"
      ",RSNCODE=%[rsn]                                                         \n"
#ifdef _LP64
      "         SAM64                                                          \n"
      "         SYSSTATE AMODE64=YES                                           \n"
#endif
      : [ucb]"=m"(below2G->ucb), [rc]"=m"(below2G->rc), [rsn]"=m"(below2G->rsn)
      : [wka]"m"(below2G->workArea), [volser]"m"(below2G->volser),
        [plist]"m"(below2G->parmList)
      : "r0", "r1", "r14", "r15"
  );

  *result = below2G->ucb.ucbCommon;
  *reasonCode = below2G->rsn;
  int rc = below2G->rc;

  FREE_STRUCT31(
    STRUCT31_NAME(below2G)
  );

  return rc;
}

int main(int argc, char **argv) {

  if (argc < 2) {
    printf("error: volser not specified\n");
    return RC_FAILURE;
  }

  const char *volser = argv[1];

  printf("info: starting UCBSCAN test for volser %s\n");

  UCBCommon ucb = {0};
  int ucbRC = 0, ucbRSN = 0;
  ucbRC = getUCBCommonCopy(volser, &ucb, &ucbRSN);

  printf("info: UCB status = %02X\n", ucb.status);

  if (ucbRC == 0) {
    printf("info: UCB dump for volser %s\n", volser);
    dumpbuffer((char *)&ucb, sizeof(ucb));
  } else {
    printf("error: UCBSCAN failed, RC = %d, RSN = 0x%08X\n", ucbRC, ucbRSN);
    return RC_FAILURE;
  }

  return RC_OK;
}


/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/

