

/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "zowetypes.h"
#include "alloc.h"
#include "logstream.h"

#include "utils.h"

#ifndef __ZOWE_OS_ZOS
#error z/OS targets are supported only
#endif

/*

xlc "-Wa,goff" \
"-Wc,LANGLVL(EXTC99),FLOAT(HEX),agg,exp,list(),so(),goff,xref,gonum,roconst,gonum,ASM,ASMLIB('SYS1.MACLIB'),LP64,XPLINK" \
-I ../h -o logstream logstream.c alloc.c utils.c timeutls.c

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
      "                                                                        \n"

      : [rc]"=m"(below2G->rc), [rsn]"=m"(below2G->rsn)

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

int logstreamDefineCFLogstream(const LogstreamName *streamName,
                               const LogstreamStructName *structName,
                               const LogstreamDescription *description,
                               int *rsn) {

  ALLOC_STRUCT31(
    STRUCT31_NAME(below2G),
    STRUCT31_FIELDS(
      LogstreamName streamName;
      LogstreamStructName structName;
      LogstreamDescription description;
      char answerArea[40]; /* IXGANSAA  */
      unsigned int answerAreaLength;
      char plist[512];
      int rc;
      int rsn;
    )
  );


  below2G->streamName = *streamName;
  below2G->structName = *structName;
  below2G->description = *description;
  below2G->answerAreaLength = sizeof(below2G->answerArea);

  __asm(

      ASM_PREFIX
      "         IXGINVNT REQUEST=DEFINE"
      ",TYPE=LOGSTREAM"
      ",STREAMNAME=%[streamName]"
      ",STRUCTNAME=%[structName]"
      ",DASDONLY=NO"
      ",DESCRIPTION=%[description]"
      ",ANSAREA=%[answerArea]"
      ",ANSLEN=%[answerAreaLen]"
      ",RETCODE=%[rc]"
      ",RSNCODE=%[rsn]"
      ",MF=(E,%[plist],COMPLETE)"
      "                                                                        \n"

      : [rc]"=m"(below2G->rc), [rsn]"=m"(below2G->rsn)

      : [streamName]"m"(below2G->streamName),
        [structName]"m"(below2G->structName),
        [description]"m"(below2G->description),
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


int logstreamConntect(const LogstreamName *streamName,
                      bool readOnly,
                      LogstreamToken *token,
                      int *rsn) {

  ALLOC_STRUCT31(
    STRUCT31_NAME(below2G),
    STRUCT31_FIELDS(
      LogstreamName streamName;
      LogstreamToken token;
      char answerArea[40]; /* IXGANSAA  */
      unsigned int answerAreaLength;
      char plist[512];
      int rc;
      int rsn;
    )
  );


  below2G->streamName = *streamName;
  below2G->answerAreaLength = sizeof(below2G->answerArea);

  /* Init required parameters. */
  __asm(

      ASM_PREFIX
      "         IXGCONN REQUEST=CONNECT"
      ",STREAMNAME=%[streamName]"
      ",ANSAREA=%[answerArea]"
      ",ANSLEN=%[answerAreaLen]"
      ",MF=(M,%[plist],COMPLETE)"
      "                                                                        \n"

      :

      : [streamName]"m"(below2G->streamName),
        [answerArea]"m"(below2G->answerArea),
        [answerAreaLen]"m"(below2G->answerAreaLength),
        [plist]"m"(below2G->plist)

      : "r0", "r1", "r14", "r15"

  );

  /* Modify parameters. */
  if (readOnly) {
    __asm(
        ASM_PREFIX
        "         IXGCONN REQUEST=CONNECT,AUTH=READ,MF=(M,%[plist],NOCHECK)"
        : : [plist]"m"(below2G->plist) : "r0", "r1", "r14", "r15"
    );
  } else {
    __asm(
        ASM_PREFIX
        "         IXGCONN REQUEST=CONNECT,AUTH=WRITE,MF=(M,%[plist],NOCHECK)"
        : : [plist]"m"(below2G->plist) : "r0", "r1", "r14", "r15"
    );
  }

  /* Execute request. */
  __asm(

      ASM_PREFIX
      "         IXGCONN REQUEST=CONNECT"
      ",STREAMTOKEN=%[token]"
      ",RETCODE=%[rc]"
      ",RSNCODE=%[rsn]"
      ",MF=(E,%[plist],NOCHECK)"
      "                                                                        \n"

      : [token]"=m"(below2G->token),
        [rc]"=m"(below2G->rc), [rsn]"=m"(below2G->rsn)

      : [plist]"m"(below2G->plist)

      : "r0", "r1", "r14", "r15"

  );

  /* Sad... that's what the doc says:
   * If you use IXGCONN REQUEST=CONNECT,...,MF=(E,parmlist,NOCHECK) with either
   *  the STREAMTOKEN=xxxx or the USERDATA=yyyy keyword, the following
   *  procedure must be followed. When the processing is complete, move the
   *  STREAMTOKEN or USERDATA values from the parameter list specified on
   *  MF= to your own storage.  */
  memcpy(token->value, &below2G->plist[170], 16);

  int rc = below2G->rc;
  *rsn = below2G->rsn;

  FREE_STRUCT31(
    STRUCT31_NAME(below2G)
  );

  return rc;
}


int logstreamDisconntect(const LogstreamToken *token, int *rsn) {

  ALLOC_STRUCT31(
    STRUCT31_NAME(below2G),
    STRUCT31_FIELDS(
      LogstreamToken token;
      char answerArea[40]; /* IXGANSAA  */
      unsigned int answerAreaLength;
      char plist[512];
      int rc;
      int rsn;
    )
  );


  below2G->token = *token;
  below2G->answerAreaLength = sizeof(below2G->answerArea);

  __asm(

      ASM_PREFIX
      "         IXGCONN REQUEST=DISCONNECT"
      ",STREAMTOKEN=%[token]"
      ",ANSAREA=%[answerArea]"
      ",ANSLEN=%[answerAreaLen]"
      ",RETCODE=%[rc]"
      ",RSNCODE=%[rsn]"
      ",MF=(E,%[plist],COMPLETE)"
      "                                                                        \n"

      : [rc]"=m"(below2G->rc), [rsn]"=m"(below2G->rsn)

      : [token]"m"(below2G->token),
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

//  LogstreamStructName structName = {"IREK            "};
  int rc = 0, rsn = 0;
//  rc = logstreamDefineStruct(&structName, 2, 4096, 1024, &rsn);
//
//  printf("define struct rc = %d, rsn = 0x%08X\n", rc, rsn);
//
//  LogstreamName streamName = {"TEST.STREAM               "};
//  LogstreamDescription description = {"THIS_IS_A_TEST  "};
//  rc = logstreamDefineCFLogstream(&streamName, &structName, &description, &rsn);
//
//  printf("define stream rc = %d, rsn = 0x%08X\n", rc, rsn);

  LogstreamName streamName = {"SYSPLEX.OPERLOG           "};
  LogstreamToken token = {0};
  rc = logstreamConntect(&streamName, false, &token, &rsn);

  printf("token:\n");
  dumpbuffer((char *)&token, sizeof(token));
  printf("connect rc = %d, rsn = 0x%08X\n", rc, rsn);

  rc = logstreamDisconntect(&token, &rsn);
  printf("disconnect rc = %d, rsn = 0x%08X\n", rc, rsn);

  return 0;
}


/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/
