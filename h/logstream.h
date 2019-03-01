

/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/

#ifndef H_LOGSTREAM_H_
#define H_LOGSTREAM_H_

typedef struct LogstreamStructName_tag {
  char text[16];
} LogstreamStructName;

typedef struct LogstreamHLQ_tag {
  char name[8];
} LogstreamHLQ;

typedef struct LogstreamName_tag {
  char text[26];
} LogstreamName;

typedef struct LogstreamDescription_tag {
  char text[16];
} LogstreamDescription;

typedef struct LogstreamToken_tag {
  char value[16];
} LogstreamToken;

typedef struct LogstreamBrowseToken_tag {
  char value[4];
} LogstreamBrowseToken;

typedef int LogstreamECB;

#ifndef __LONGNAME__

#define logstreamDefineStruct LOGSDSTU
#define logstreamDeleteStruct LOGSDEL
#define logstreamDefineCFLogstream LOGSCFLD
#define logstreamDefineDASDLogstream LOGSDASD
#define logstreamDeleteLogstream LOGSDELL
#define logstreamConnect LOGSCONN
#define logstreamDisconntect LOGSDISC
#define logstreamBrowseStart LOGSBRWS
#define logstreamBrowseReset LOGSBRWR
#define logstreamReadCursor LOGSRCUR
#define logstreamWrite LOGSWRT

#endif

int logstreamDefineStruct(const LogstreamStructName *name,
                          unsigned int maxLogNumber,
                          unsigned short maxBlockSize,
                          unsigned short averageBlockSize,
                          int *rsn);

int logstreamDeleteStruct(const LogstreamStructName *name, int *rsn);

int logstreamDefineCFLogstream(const LogstreamName *streamName,
                               const LogstreamStructName *structName,
                               const LogstreamDescription *description,
                               int *rsn);

int logstreamDefineDASDLogstream(const LogstreamName *streamName,
                                 const LogstreamHLQ *hlq,
                                 unsigned int datasetSize,
                                 const LogstreamDescription *description,
                                 int *rsn);

int logstreamDeleteLogstream(const LogstreamName *name, int *rsn);

int logstreamConnect(const LogstreamName *streamName,
                     bool readOnly,
                     LogstreamToken *token,
                     int *rsn);

int logstreamDisconntect(const LogstreamToken *token, int *rsn);

int logstreamBrowseStart(const LogstreamToken *streamToken,
                         bool fromYoungest,
                         LogstreamECB * __ptr32 completionECB,
                         LogstreamBrowseToken *browseToken,
                         int *rsn);

int logstreamBrowseReset(const LogstreamToken *streamToken,
                         const LogstreamBrowseToken *browseToken,
                         bool toYoungest,
                         LogstreamECB * __ptr32 completionECB,
                         int *rsn);

int logstreamReadCursor(const LogstreamToken *streamToken,
                        const LogstreamBrowseToken *browseToken,
                        bool youngToOld,
                        void *resultBuffer,
                        unsigned int resultBufferSize,
                        unsigned int *spaceRequired,
                        LogstreamECB * __ptr32 completionECB,
                        int *rsn);

int logstreamWrite(const LogstreamToken *streamToken,
                   const void *data,
                   unsigned int dataSize,
                   LogstreamECB * __ptr32 completionECB,
                   int *rsn);


#endif /* H_LOGSTREAM_H_ */


/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/

