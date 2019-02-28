

/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/

#include <metal/metal.h>
#include <metal/stddef.h>
#include <metal/stdio.h>

#include "zowetypes.h"
#include "alloc.h"
#include "logstream.h"

#ifndef __ZOWE_OS_ZOS
#error z/OS targets are supported only
#endif

/*

xlc "-Wa,goff" \
"-Wc,LANGLVL(EXTC99),FLOAT(HEX),agg,exp,list(),so(),goff,xref,gonum,roconst,gonum,ASM,ASMLIB('SYS1.MACLIB'),LP64,XPLINK" \
-DCMS_CLIENT -D_OPEN_THREADS=1  -I $COMMON/h -I ../h -o logstream logstream.c \
$COMMON/c/alloc.c \

*/

int logstreamDefineStruct(const LogstreamStructName *name,
                          unsigned int maxLogNumber,
                          unsigned short maxBlockSize,
                          unsigned short averageBlockSize,
                          int *rsn) {


  ALLOC_STRUCT31(
    STRUCT31_NAME(below2G),
    STRUCT31_FIELDS(
      LogstreamStructName name;
      unsigned int maxLogNumber;
      unsigned int maxBlockSize;
      unsigned int averageBlockSize;
      char answerArea[40]; /* IXGANSAA  */
      unsigned int answerAreaLength;
      char plist[512];
      int rc;
      int rsn;
    )
  );


  below2G->name = *name;
  below2G->maxLogNumber = maxLogNumber;
  below2G->maxBlockSize = maxBlockSize;
  below2G->averageBlockSize = averageBlockSize;
  below2G->answerAreaLength = sizeof(below2G->answerArea);

  __asm(

      ASM_PREFIX
      "         IXGINVNT REQUEST=DEFINE"
      ",TYPE=STRUCTURE"
      ",STRUCTNAME=%[structName]"
      ",LOGSNUM=%[logNum]"
      ",MAXBUFSIZE=%[maxBlksize]"
      ",AVGBUFSIZE=%[avgBlksize]"
      ",ANSAREA=%[answerArea]"
      ",ANSLEN=%[answerAreaLen]"
      ",RETCODE=%[rc]"
      ",RSNCODE=%[rsn]"
      ",MF=(E,%[plist],COMPLETE)"
      ""

      : [rc]"=m"(below2G->rc), [rsn]"=m"(below2G->rc)

      : [structName]"m"(below2G->name),
        [logNum]"m"(below2G->maxLogNumber),
        [maxBlksize]"m"(below2G->maxBlockSize),
        [avgBlksize]"m"(below2G->averageBlockSize),
        [answerArea]"m"(below2G->answerArea),
        [answerAreaLen]"m"(below2G->answerAreaLength),
        [plist]"m"(below2G->plist)

      : "r0", "r1", "r14", "r15"

  );

  int rc = below2G->rc;
  *rsn = below2G->rsn;

  FREE_STRUCT31(
    STRUCT31_NAME(below2G)
  );

  return rc;
}
int main() {

  LogstreamStructName structName = {"IREK            "};
  int rc = 0, rsn = 0;
  rc = logstreamDefineStruct(&structName, 2, 4096, 1024, &rsn);

  printf("rc = %d, rsn = %d\n", rc, rsn);

  return 0;
}


/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/

