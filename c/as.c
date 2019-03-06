

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
#include "zos.h"

#ifndef __ZOWE_OS_ZOS
#error z/OS targets are supported only
#endif

/*

xlc "-Wa,goff" \
"-Wc,LANGLVL(EXTC99),FLOAT(HEX),agg,exp,list(),so(),goff,xref,gonum,roconst,gonum,ASM,ASMLIB('SYS1.MACLIB')" \
"-Wl,ac=1" -I ../h -o as as.c alloc.c utils.c timeutls.c zos.c ; extattr +a as

*/

static void sleep(int seconds){
  int waitValue = seconds * 100;
  __asm(" STIMER WAIT,BINTVL=%0\n" : : "m"(waitValue));
}

struct ODA {
  char stoken[8];
  ASCB * __ptr32 ascb;
  void * __ptr32 commECBs;
  char reserved[8];
};

struct UserToken {
  char data[64];
};

struct STPARM {
  uint16_t length;
  char text[124];
};

int main() {

  int rc = 0, rsn = 0;

  supervisorMode(TRUE);

  ALLOC_STRUCT31(
    STRUCT31_NAME(below2G),
    STRUCT31_FIELDS(
      struct STPARM parm;
      struct ODA outputData;
      char parmList[512];
    )
  );

#define PARM_STRING "IEESYSAS.IREK,PROG=ZWESIS01"

  below2G->parm.length = strlen(PARM_STRING);
  memcpy(below2G->parm.text, PARM_STRING, strlen(PARM_STRING));

  __asm(
      ASM_PREFIX

      "L@ASCRE  DS    0H                                                       \n"
      "         PUSH  USING                                                    \n"
      "         DROP                                                           \n"
      "         LARL  10,L@ASCRE                                               \n"
      "         USING L@ASCRE,10                                               \n"

      "         ASCRE STPARM=%[parm]"
      ",INIT='IEFBR14'"
      ",ODA=%[oda]"
//      ",TRMEXIT=(%[termExit])"
//      ",UTOKEN=%[userToken]"
      ",ATTR=NOSWAP"
      ",MF=(E,%[parmList])                                                     \n"
      "         ST    15,%[rc]                                                 \n"
      "         ST    0,%[rsn]                                                 \n"

      "         POP   USING                                                    \n"

      : [rc]"=m"(rc), [rsn]"=m"(rsn)

      : [parm]"m"(below2G->parm),
        [oda]"m"(below2G->outputData),
        [parmList]"m"(below2G->parmList)

      : "r0", "r1", "r14", "r15"

  );

  printf("ASCRE RC = %d, RSN = %d\n", rc, rsn);
  printf("Output data:\n");
  dumpbuffer((char *)&below2G->outputData, sizeof(struct ODA));

  sleep(10);

  FREE_STRUCT31(
    STRUCT31_NAME(below2G)
  );
  below2G = NULL;

  return 0;
}


/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/

