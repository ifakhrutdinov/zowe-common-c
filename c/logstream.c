

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
#include <metal/string.h>
#else
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#endif

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

int logstreamDeleteStruct(const LogstreamStructName *name, int *rsn) {

  ALLOC_STRUCT31(
    STRUCT31_NAME(below2G),
    STRUCT31_FIELDS(
      LogstreamStructName name;
      char answerArea[40]; /* IXGANSAA  */
      unsigned int answerAreaLength;
      char plist[512];
      int rc;
      int rsn;
    )
  );


  below2G->name = *name;
  below2G->answerAreaLength = sizeof(below2G->answerArea);

  __asm(

      ASM_PREFIX
      "         IXGINVNT REQUEST=DELETE"
      ",TYPE=STRUCTURE"
      ",STRUCTNAME=%[structName]"
      ",ANSAREA=%[answerArea]"
      ",ANSLEN=%[answerAreaLen]"
      ",RETCODE=%[rc]"
      ",RSNCODE=%[rsn]"
      ",MF=(E,%[plist],COMPLETE)"
      "                                                                        \n"

      : [rc]"=m"(below2G->rc), [rsn]"=m"(below2G->rsn)

      : [structName]"m"(below2G->name),
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

int logstreamDefineDASDLogstream(const LogstreamName *streamName,
                                 const LogstreamHLQ *hlq,
                                 unsigned int datasetSize,
                                 const LogstreamDescription *description,
                                 int *rsn) {

  ALLOC_STRUCT31(
    STRUCT31_NAME(below2G),
    STRUCT31_FIELDS(
      LogstreamName streamName;
      LogstreamHLQ hlq;
      LogstreamDescription description;
      char answerArea[40]; /* IXGANSAA  */
      unsigned int answerAreaLength;
      unsigned int stgSize;
      char plist[512];
      int rc;
      int rsn;
    )
  );


  below2G->streamName = *streamName;
  below2G->hlq = *hlq;
  below2G->description = *description;
  below2G->answerAreaLength = sizeof(below2G->answerArea);
  below2G->stgSize = datasetSize;

  __asm(

      ASM_PREFIX
      "         IXGINVNT REQUEST=DEFINE"
      ",TYPE=LOGSTREAM"
      ",STREAMNAME=%[streamName]"
      ",HLQ=%[hlq]"
      ",STG_SIZE=%[stgSize]"
      ",DASDONLY=YES"
      ",DESCRIPTION=%[description]"
      ",ANSAREA=%[answerArea]"
      ",ANSLEN=%[answerAreaLen]"
      ",RETCODE=%[rc]"
      ",RSNCODE=%[rsn]"
      ",MF=(E,%[plist],COMPLETE)"
      "                                                                        \n"

      : [rc]"=m"(below2G->rc), [rsn]"=m"(below2G->rsn)

      : [streamName]"m"(below2G->streamName),
        [hlq]"m"(below2G->hlq),
        [stgSize]"m"(below2G->stgSize),
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

int logstreamDeleteLogstream(const LogstreamName *name, int *rsn) {

  ALLOC_STRUCT31(
    STRUCT31_NAME(below2G),
    STRUCT31_FIELDS(
      LogstreamName name;
      char answerArea[40]; /* IXGANSAA  */
      unsigned int answerAreaLength;
      char plist[512];
      int rc;
      int rsn;
    )
  );


  below2G->name = *name;
  below2G->answerAreaLength = sizeof(below2G->answerArea);

  __asm(

      ASM_PREFIX
      "         IXGINVNT REQUEST=DELETE"
      ",TYPE=LOGSTREAM"
      ",STREAMNAME=%[streamName]"
      ",ANSAREA=%[answerArea]"
      ",ANSLEN=%[answerAreaLen]"
      ",RETCODE=%[rc]"
      ",RSNCODE=%[rsn]"
      ",MF=(E,%[plist],COMPLETE)"
      "                                                                        \n"

      : [rc]"=m"(below2G->rc), [rsn]"=m"(below2G->rsn)

      : [streamName]"m"(below2G->name),
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

int logstreamConnect(const LogstreamName *streamName,
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
  if (!readOnly) {
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


int logstreamBrowseStart(const LogstreamToken *streamToken,
                         bool fromYoungest,
                         LogstreamECB * __ptr32 completionECB,
                         LogstreamBrowseToken *browseToken,
                         int *rsn) {

  ALLOC_STRUCT31(
    STRUCT31_NAME(below2G),
    STRUCT31_FIELDS(
      LogstreamToken streamToken;
      LogstreamBrowseToken browseToken;
      char answerArea[40]; /* IXGANSAA  */
      unsigned int answerAreaLength;
      char plist[512];
      int rc;
      int rsn;
    )
  );


  below2G->streamToken = *streamToken;
  below2G->answerAreaLength = sizeof(below2G->answerArea);

  /* Init required parameters. */
  __asm(

      ASM_PREFIX
      "         IXGBRWSE REQUEST=START"
      ",STREAMTOKEN=%[streamToken]"
      ",BROWSETOKEN=%[browseToken]"
      ",ANSAREA=%[answerArea]"
      ",ANSLEN=%[answerAreaLen]"
      ",MF=(M,%[plist],COMPLETE)"
      "                                                                        \n"

      :

      : [streamToken]"m"(below2G->streamToken),
        [browseToken]"m"(below2G->browseToken),
        [answerArea]"m"(below2G->answerArea),
        [answerAreaLen]"m"(below2G->answerAreaLength),
        [plist]"m"(below2G->plist)

      : "r0", "r1", "r14", "r15"

  );

  /* Modify parameters. */
  if (fromYoungest) {
    __asm(
        ASM_PREFIX
        "         IXGBRWSE REQUEST=START,YOUNGEST,MF=(M,%[plist],NOCHECK)"
        : : [plist]"m"(below2G->plist) : "r0", "r1", "r14", "r15"
    );
  }

  if (completionECB != NULL) {
    __asm(
        ASM_PREFIX
        "         IXGBRWSE REQUEST=START,MODE=SYNCECB,ECB=(%[ecb])"
        ",MF=(M,%[plist],NOCHECK)"
        :
        : [ecb]"r"(completionECB), [plist]"m"(below2G->plist)
        : "r0", "r1", "r14", "r15"
    );
  }

  /* Execute request. */
  __asm(

      ASM_PREFIX
      "         IXGBRWSE REQUEST=START"
      ",RETCODE=%[rc]"
      ",RSNCODE=%[rsn]"
      ",MF=(E,%[plist],NOCHECK)"
      "                                                                        \n"

      : [rc]"=m"(below2G->rc), [rsn]"=m"(below2G->rsn)

      : [plist]"m"(below2G->plist)

      : "r0", "r1", "r14", "r15"

  );

  *browseToken = below2G->browseToken;
  int rc = below2G->rc;
  *rsn = below2G->rsn;

  FREE_STRUCT31(
    STRUCT31_NAME(below2G)
  );

  return rc;
}

int logstreamBrowseReset(const LogstreamToken *streamToken,
                         const LogstreamBrowseToken *browseToken,
                         bool toYoungest,
                         LogstreamECB * __ptr32 completionECB,
                         int *rsn) {

  ALLOC_STRUCT31(
    STRUCT31_NAME(below2G),
    STRUCT31_FIELDS(
      LogstreamToken streamToken;
      LogstreamBrowseToken browseToken;
      char answerArea[40]; /* IXGANSAA  */
      unsigned int answerAreaLength;
      char plist[512];
      int rc;
      int rsn;
    )
  );


  below2G->streamToken = *streamToken;
  below2G->browseToken = *browseToken;
  below2G->answerAreaLength = sizeof(below2G->answerArea);

  /* Init required parameters. */
  __asm(

      ASM_PREFIX
      "         IXGBRWSE REQUEST=RESET"
      ",STREAMTOKEN=%[streamToken]"
      ",BROWSETOKEN=%[browseToken]"
      ",POSITION=OLDEST"
      ",ANSAREA=%[answerArea]"
      ",ANSLEN=%[answerAreaLen]"
      ",MF=(M,%[plist],COMPLETE)"
      "                                                                        \n"

      :

      : [streamToken]"m"(below2G->streamToken),
        [browseToken]"m"(below2G->browseToken),
        [answerArea]"m"(below2G->answerArea),
        [answerAreaLen]"m"(below2G->answerAreaLength),
        [plist]"m"(below2G->plist)

      : "r0", "r1", "r14", "r15"

  );

  /* Modify parameters. */
  if (toYoungest) {
    __asm(
        ASM_PREFIX
        "         IXGBRWSE REQUEST=RESET,POSITION=YOUNGEST"
        ",MF=(M,%[plist],NOCHECK)"
        : : [plist]"m"(below2G->plist) : "r0", "r1", "r14", "r15"
    );
  }

  if (completionECB != NULL) {
    __asm(
        ASM_PREFIX
        "         IXGBRWSE REQUEST=RESET,MODE=SYNCECB,ECB=(%[ecb])"
        ",MF=(M,%[plist],NOCHECK)"
        :
        : [ecb]"r"(completionECB), [plist]"m"(below2G->plist)
        : "r0", "r1", "r14", "r15"
    );
  }

  /* Execute request. */
  __asm(

      ASM_PREFIX
      "         IXGBRWSE REQUEST=RESET"
      ",RETCODE=%[rc]"
      ",RSNCODE=%[rsn]"
      ",MF=(E,%[plist],NOCHECK)"
      "                                                                        \n"

      : [rc]"=m"(below2G->rc), [rsn]"=m"(below2G->rsn)

      : [plist]"m"(below2G->plist)

      : "r0", "r1", "r14", "r15"

  );

  int rc = below2G->rc;
  *rsn = below2G->rsn;

  FREE_STRUCT31(
    STRUCT31_NAME(below2G)
  );

  return rc;
}

static bool isAddr64(const void *addr) {

  if (sizeof(addr) < 8) {
    return false;
  }

  return ((uint64)addr & 0xFFFFFFFF00000000) > 0;
}

int logstreamReadCursor(const LogstreamToken *streamToken,
                        const LogstreamBrowseToken *browseToken,
                        bool youngToOld,
                        void *resultBuffer,
                        unsigned int resultBufferSize,
                        unsigned int *spaceRequired,
                        LogstreamECB * __ptr32 completionECB,
                        int *rsn) {

  ALLOC_STRUCT31(
    STRUCT31_NAME(below2G),
    STRUCT31_FIELDS(
      LogstreamToken streamToken;
      LogstreamBrowseToken browseToken;
      unsigned int resultBufferSize;
      unsigned int spaceRequired;
      char answerArea[40]; /* IXGANSAA  */
      unsigned int answerAreaLength;
      char plist[512];
      int rc;
      int rsn;
    )
  );


  below2G->streamToken = *streamToken;
  below2G->browseToken = *browseToken;
  below2G->resultBufferSize = resultBufferSize;
  below2G->answerAreaLength = sizeof(below2G->answerArea);

  /* Init required parameters. */
  if (isAddr64(resultBuffer)) {
    __asm(

        ASM_PREFIX
        "         IXGBRWSE REQUEST=READCURSOR"
        ",STREAMTOKEN=%[streamToken]"
        ",BROWSETOKEN=%[browseToken]"
        ",BUFFER64=(%[buffer])"
        ",BUFFLEN=%[bufferLen]"
        ",DIRECTION=OLDTOYOUNG"
        ",ANSAREA=%[answerArea]"
        ",ANSLEN=%[answerAreaLen]"
        ",MF=(M,%[plist],COMPLETE)"
        "                                                                        \n"

        :

        : [streamToken]"m"(below2G->streamToken),
          [browseToken]"m"(below2G->browseToken),
          [buffer]"r"(resultBuffer),
          [bufferLen]"m"(below2G->resultBufferSize),
          [answerArea]"m"(below2G->answerArea),
          [answerAreaLen]"m"(below2G->answerAreaLength),
          [plist]"m"(below2G->plist)

        : "r0", "r1", "r14", "r15"

    );
  } else {
    __asm(

        ASM_PREFIX
        "         IXGBRWSE REQUEST=READCURSOR"
        ",STREAMTOKEN=%[streamToken]"
        ",BROWSETOKEN=%[browseToken]"
        ",BUFFER=(%[buffer])"
        ",BUFFLEN=%[bufferLen]"
        ",DIRECTION=OLDTOYOUNG"
        ",ANSAREA=%[answerArea]"
        ",ANSLEN=%[answerAreaLen]"
        ",MF=(M,%[plist],COMPLETE)"
        "                                                                        \n"

        :

        : [streamToken]"m"(below2G->streamToken),
          [browseToken]"m"(below2G->browseToken),
          [buffer]"r"(resultBuffer),
          [bufferLen]"m"(below2G->resultBufferSize),
          [answerArea]"m"(below2G->answerArea),
          [answerAreaLen]"m"(below2G->answerAreaLength),
          [plist]"m"(below2G->plist)

        : "r0", "r1", "r14", "r15"

    );
  }

  /* Modify parameters. */
  if (youngToOld) {
    __asm(
        ASM_PREFIX
        "         IXGBRWSE REQUEST=READCURSOR"
        ",DIRECTION=YOUNGTOOLD,MF=(M,%[plist],NOCHECK)"
        : : [plist]"m"(below2G->plist) : "r0", "r1", "r14", "r15"
    );
  }

  if (completionECB != NULL) {
    __asm(
        ASM_PREFIX
        "         IXGBRWSE REQUEST=READCURSOR,MODE=SYNCECB,ECB=(%[ecb])"
        ",MF=(M,%[plist],NOCHECK)"
        :
        : [ecb]"r"(completionECB), [plist]"m"(below2G->plist)
        : "r0", "r1", "r14", "r15"
    );
  }

  /* Execute request. */
  __asm(

      ASM_PREFIX
      "         IXGBRWSE REQUEST=READCURSOR"
      ",BLKSIZE=%[requiredSpace]"
      ",RETCODE=%[rc]"
      ",RSNCODE=%[rsn]"
      ",MF=(E,%[plist],NOCHECK)"
      "                                                                        \n"

      : [requiredSpace]"=m"(below2G->spaceRequired),
        [rc]"=m"(below2G->rc), [rsn]"=m"(below2G->rsn)

      : [plist]"m"(below2G->plist)

      : "r0", "r1", "r14", "r15"

  );

  *spaceRequired = below2G->spaceRequired;
  int rc = below2G->rc;
  *rsn = below2G->rsn;

  FREE_STRUCT31(
    STRUCT31_NAME(below2G)
  );

  return rc;
}

int logstreamWrite(const LogstreamToken *streamToken,
                   const void *data,
                   unsigned int dataSize,
                   LogstreamECB * __ptr32 completionECB,
                   int *rsn) {

  ALLOC_STRUCT31(
    STRUCT31_NAME(below2G),
    STRUCT31_FIELDS(
      LogstreamToken streamToken;
      unsigned int dataSize;
      char answerArea[40]; /* IXGANSAA  */
      unsigned int answerAreaLength;
      char plist[512];
      int rc;
      int rsn;
    )
  );


  below2G->streamToken = *streamToken;
  below2G->dataSize = dataSize;
  below2G->answerAreaLength = sizeof(below2G->answerArea);

  /* Init required parameters. */
  if (isAddr64(data)) {
    __asm(

        ASM_PREFIX
        "         IXGWRITE STREAMTOKEN=%[streamToken]"
        ",BUFFER64=(%[data])"
        ",BLOCKLEN=%[dataSize]"
        ",ANSAREA=%[answerArea]"
        ",ANSLEN=%[answerAreaLen]"
        ",MF=(M,%[plist],COMPLETE)"
        "                                                                        \n"

        :

        : [streamToken]"m"(below2G->streamToken),
          [data]"r"(data),
          [dataSize]"m"(below2G->dataSize),
          [answerArea]"m"(below2G->answerArea),
          [answerAreaLen]"m"(below2G->answerAreaLength),
          [plist]"m"(below2G->plist)

        : "r0", "r1", "r14", "r15"

    );
  } else {
    __asm(

        ASM_PREFIX
        "         IXGWRITE STREAMTOKEN=%[streamToken]"
        ",BUFFER=(%[data])"
        ",BLOCKLEN=%[dataSize]"
        ",ANSAREA=%[answerArea]"
        ",ANSLEN=%[answerAreaLen]"
        ",MF=(M,%[plist],COMPLETE)"
        "                                                                        \n"

        :

        : [streamToken]"m"(below2G->streamToken),
          [data]"r"(data),
          [dataSize]"m"(below2G->dataSize),
          [answerArea]"m"(below2G->answerArea),
          [answerAreaLen]"m"(below2G->answerAreaLength),
          [plist]"m"(below2G->plist)

        : "r0", "r1", "r14", "r15"

    );
  }

  /* Modify parameters. */
  if (completionECB != NULL) {
    __asm(
        ASM_PREFIX
        "         IXGWRITE MODE=SYNCECB,ECB=(%[ecb])"
        ",MF=(M,%[plist],NOCHECK)"
        :
        : [ecb]"r"(completionECB), [plist]"m"(below2G->plist)
        : "r0", "r1", "r14", "r15"
    );
  }

  /* Execute request. */
  __asm(

      ASM_PREFIX
      "         IXGWRITE RETCODE=%[rc],RSNCODE=%[rsn]"
      ",MF=(E,%[plist],NOCHECK)"
      "                                                                        \n"

      : [rc]"=m"(below2G->rc), [rsn]"=m"(below2G->rsn)

      : [plist]"m"(below2G->plist)

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

  int rc = 0, rsn = 0;

  LogstreamStructName structName = {"ZIS_TEST_STRUCT "};
  LogstreamName streamName = {"ZIS.TEST.STREAM           "};

  rc = logstreamDeleteLogstream(&streamName, &rsn);
  printf("delete stream rc = %d, rsn = 0x%08X\n", rc, rsn);

//  rc = logstreamDeleteStruct(&structName, &rsn);
//  printf("delete struct rc = %d, rsn = 0x%08X\n", rc, rsn);
//
//  return 0;

//  rc = logstreamDefineStruct(&structName, 2, 4096, 1024, &rsn);
//  printf("define struct rc = %d, rsn = 0x%08X\n", rc, rsn);

  LogstreamDescription description = {"THIS_IS_A_TEST  "};
  LogstreamHLQ hlq = {"SSUSER1 "};
  rc = logstreamDefineDASDLogstream(&streamName, &hlq, 50, &description, &rsn);
  printf("define stream rc = %d, rsn = 0x%08X\n", rc, rsn);

  LogstreamToken token = {0};
  rc = logstreamConnect(&streamName, false, &token, &rsn);
  printf("token:\n");
  dumpbuffer((char *)&token, sizeof(token));
  printf("connect rc = %d, rsn = 0x%08X\n", rc, rsn);

  const char *messages[] = {"hello", "this is a log stream test", "bye"};
  for (int i = 0; i < sizeof(messages) / sizeof(messages[0]); i++) {
    rc = logstreamWrite(&token, messages[i], strlen(messages[i]), NULL, &rsn);
    printf("write rc = %d, rsn = 0x%08X\n", rc, rsn);
  }

  LogstreamBrowseToken browseToken = {0};
  rc = logstreamBrowseStart(&token, false, NULL, &browseToken, &rsn);
  printf("browse token:\n");
  dumpbuffer((char *)&browseToken, sizeof(browseToken));
  printf("browse rc = %d, rsn = 0x%08X\n", rc, rsn);

  rc = logstreamBrowseReset(&token, &browseToken, true, NULL, &rsn);
  printf("browse token:\n");
  dumpbuffer((char *)&browseToken, sizeof(browseToken));
  printf("reset rc = %d, rsn = 0x%08X\n", rc, rsn);

  for (int i = 0; i < 5; i++) {

    char resultBuffer[512] = {0};
    unsigned int resultBufferSize = sizeof(resultBuffer);
    unsigned int requiredSize = 0;

    rc = logstreamReadCursor(&token, &browseToken, true,
                             resultBuffer, resultBufferSize, &requiredSize,
                             NULL, &rsn);

    printf("read rc = %d, rsn = 0x%08X, required size = %u\n", rc, rsn,
           requiredSize);
    dumpbuffer(resultBuffer, resultBufferSize);

  }

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

