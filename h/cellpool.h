

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

#define cellpoolBuild CPBUILD
#define cellpoolDelete CPDELETE
#define cellpoolGet CPGET
#define cellpoolFree CPFREE

#endif

ZOWE_PRAGMA_PACK

typedef int32_t CPID;

typedef struct CPHeader_tag {
  char text[24];
} CPHeader;

ZOWE_PRAGMA_PACK_RESET

CPID cellpoolBuild(unsigned int pCellCount,
                   unsigned int sCellCount,
                   unsigned int cellSize,
                   int subpool, int key,
                   const CPHeader *header);

void cellpoolDelete(CPID cellpoolID);

void *cellpoolGet(CPID cellpoolID, bool conditional);

void cellpoolFree(CPID cellpoolID, void *cell);

#endif /* H_CELLPOOL_H_ */


/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/

