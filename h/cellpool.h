

/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/

#ifndef H_CELLPOOL_H_
#define H_CELLPOOL_H_

#ifdef METTLE
#include <metal/metal.h>
#include <metal/stddef.h>
#else
#include <stddef.h>
#endif

#include "zos.h"
#include "zowetypes.h"

#ifndef __LONGNAME__

#define cellpoolGetDWordAlignedSize CPASIZE
#define cellpoolBuild CPBUILD
#define cellpoolDelete CPDELETE
#define cellpoolGet CPGET
#define cellpoolFree CPFREE

#define cellpoolBuild64 CP4BUILD
#define cellpoolBuild64Xmem CP4XBLD
#define cellpoolDelete64 CP4DEL
#define cellpoolGet CP4GET
#define cellpoolFree64 CP4FREE

#endif

ZOWE_PRAGMA_PACK

typedef int32_t CPID;
typedef int64_t CPID64;

typedef struct CPHeader_tag {
  char text[24];
} CPHeader;

ZOWE_PRAGMA_PACK_RESET

unsigned int cellpoolGetDWordAlignedSize(unsigned int size);

CPID cellpoolBuild(unsigned int pCellCount,
                   unsigned int sCellCount,
                   unsigned int cellSize,
                   int subpool, int key,
                   const CPHeader *header);

void cellpoolDelete(CPID cellpoolID);

void *cellpoolGet(CPID cellpoolID, bool conditional);

void cellpoolFree(CPID cellpoolID, void *cell);

#ifdef _LP64

CPID64 cellpoolBuild64(unsigned int cellSize,
                       const CPHeader *header,
                       int *serviceRC);

CPID64 cellpoolBuild64Xmem(unsigned int cellSize,
                           const CPHeader *header,
                           int *serviceRC);

void cellpoolDelete64(CPID64 id);

void *cellpoolGet64(CPID64 id,
                    bool noExpansion,
                    int *serviceRC);

void cellpoolFree64(CPID64 id, void *cell);

#endif

#endif /* H_CELLPOOL_H_ */


/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/

