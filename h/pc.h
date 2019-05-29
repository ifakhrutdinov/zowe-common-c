

/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/

#ifndef H_PC_H_
#define H_PC_H_

#ifdef METTLE
#include <metal/metal.h>
#include <metal/stdint.h>
#else
#include "stdint.h"
#endif

#include "zowetypes.h"

ZOWE_PRAGMA_PACK

typedef struct PCLinkageIndex {
  int sequenceNumber;
  int pcNumber;
} PCLinkageIndex;

ZOWE_PRAGMA_PACK_RESET

#ifndef __LONGNAME__
#define pcSetAllAddressSpaceAuthority PCAXSET
#define pcReserveLinkageIndex PCLXRES
#define pcFreeLinkageIndex PCLXFRE
#define pcMakeEntryTableDescriptor PCETCRED
#define pcAddToEntryTableDescriptor PCETADD
#define pcRemoveEntryTableDescriptor PCETREMD
#define pcCreateEntryTable PCETCRE
#define pcDestroyEntryTable PCETDES
#define pcConnectEntryTable PCETCON
#define pcDisconnectEntryTable PCETDIS
#endif

typedef enum PCLinkageIndexSize_tag {
  LX_SIZE_12,
  LX_SIZE_16,
  LX_SIZE_23,
  LX_SIZE_24,
} PCLinkageIndexSize;

typedef uint32_t PCEntryTableToken;

typedef struct ETD_tag EntryTableDescriptor;

int pcSetAllAddressSpaceAuthority(void);

int pcReserveLinkageIndex(bool isSystem, bool isReusable,
                          PCLinkageIndexSize indexSize,
                          PCLinkageIndex *result);
int pcFreeLinkageIndex(PCLinkageIndex index, bool forced);

EntryTableDescriptor *pcMakeEntryTableDescriptor(void);
void pcAddToEntryTableDescriptor(EntryTableDescriptor *descriptor,
                                 void * __ptr32 routine,
                                 uint32_t routineParameter1,
                                 uint32_t routineParameter2,
                                 bool isSASNOld,
                                 bool isAMODE64,
                                 bool isSUP,
                                 bool isSpaceSwitch,
                                 int key);
void pcRemoveEntryTableDescriptor(EntryTableDescriptor *descriptor);

int pcCreateEntryTable(const EntryTableDescriptor *descriptor,
                       PCEntryTableToken *resultToken);
int pcDestroyEntryTable(PCEntryTableToken token, bool purge);

int pcConnectEntryTable(PCEntryTableToken token, PCLinkageIndex index);
int pcDisconnectEntryTable(PCEntryTableToken token);

#endif /* H_PC_H_ */


/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/
