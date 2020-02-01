

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
#include <metal/stdlib.h>
#include <metal/string.h>
#else
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#endif

#include "zowetypes.h"
#include "alloc.h"
#include "cellpool.h"
#include "zos.h"

unsigned int cellpoolGetDWordAlignedSize(unsigned int size) {

  unsigned rem = size % 8;

  if (rem == 0) {
    return size;
  }

  return size + 8 - rem;

}

CPID cellpoolBuild(unsigned int pCellCount,
                   unsigned int sCellCount,
                   unsigned int cellSize,
                   int subpool, int key,
                   const CPHeader *header) {

  CPID cpid = CPID_NULL;

  ALLOC_STRUCT31(
    STRUCT31_NAME(below2G),
    STRUCT31_FIELDS(
      char parmList[64];
      CPHeader header;
    )
  );

  if (below2G == NULL) { /* This can only fail in LE 64-bit */
    return cpid;
  }

  below2G->header = *header;

  if (isCallerCrossMemory() || isCallerSRB()) {

    __asm(

        ASM_PREFIX
        "         SYSSTATE PUSH                                                  \n"
        "         SYSSTATE OSREL=ZOSV1R6                                         \n"
  #ifdef _LP64
        "         SAM31                                                          \n"
        "         SYSSTATE AMODE64=NO                                            \n"
  #endif

        "         CPOOL BUILD"
        ",PCELLCT=(%[pcell])"
        ",SCELLCT=(%[scell])"
        ",CSIZE=(%[csize])"
        ",SP=(%[sp])"
        ",KEY=(%[key])"
        ",TCB=0"
        ",LOC=(31,64)"
        ",CPID=(%[cpid])"
        ",HDR=%[header]"
        ",MF=(E,%[parmList])"
        "                                                                        \n"

  #ifdef _LP64
        "         SAM64                                                          \n"
  #endif
        "         SYSSTATE POP                                                   \n"

        : [cpid]"=NR:r0"(cpid)

        : [pcell]"r"(pCellCount),
          [scell]"r"(sCellCount),
          [csize]"r"(cellSize),
          [sp]"r"(subpool),
          [key]"r"(key),
          [header]"m"(below2G->header),
          [parmList]"m"(below2G->parmList)

        : "r0", "r1", "r14", "r15"

    );

  } else {

    __asm(

        ASM_PREFIX
        "         SYSSTATE PUSH                                                  \n"
        "         SYSSTATE OSREL=ZOSV1R6                                         \n"
  #ifdef _LP64
        "         SAM31                                                          \n"
        "         SYSSTATE AMODE64=NO                                            \n"
  #endif

        "         CPOOL BUILD"
        ",PCELLCT=(%[pcell])"
        ",SCELLCT=(%[scell])"
        ",CSIZE=(%[csize])"
        ",SP=(%[sp])"
        ",KEY=(%[key])"
        ",LOC=(31,64)"
        ",CPID=(%[cpid])"
        ",HDR=%[header]"
        ",MF=(E,%[parmList])"
        "                                                                        \n"

  #ifdef _LP64
        "         SAM64                                                          \n"
  #endif
        "         SYSSTATE POP                                                   \n"

        : [cpid]"=NR:r0"(cpid)

        : [pcell]"r"(pCellCount),
          [scell]"r"(sCellCount),
          [csize]"r"(cellSize),
          [sp]"r"(subpool),
          [key]"r"(key),
          [header]"m"(below2G->header),
          [parmList]"m"(below2G->parmList)

        : "r0", "r1", "r14", "r15"

    );

  }

  FREE_STRUCT31(
    STRUCT31_NAME(below2G)
  );

  return cpid;
}

void cellpoolDelete(CPID cellpoolID) {

  __asm(

      ASM_PREFIX
      "         SYSSTATE PUSH                                                  \n"
      "         SYSSTATE OSREL=ZOSV1R6                                         \n"
#ifdef _LP64
      "         SAM31                                                          \n"
      "         SYSSTATE AMODE64=NO                                            \n"
#endif

      "         CPOOL DELETE,CPID=(%[cpid])                                    \n"

#ifdef _LP64
      "         SAM64                                                          \n"
#endif
      "         SYSSTATE POP                                                   \n"

      :
      : [cpid]"r"(cellpoolID)
      : "r0", "r1", "r14", "r15"
  );

}

void *cellpoolGet(CPID cellpoolID, bool conditional) {

  uint64_t callerGPRs[12] = {0};

  void * __ptr32 cell = NULL;

  /*
   * Notes about the use of callerGPRs:
   *
   * - The registers must be saved before switching to AMODE 31 and restored
   *   after switching back to AMODE 64, because the stack storage containing
   *   the callerGPRs may be above 2G.
   *
   * - Register 13 is being saved in callerGPRs, changed to point to callerGPRs,
   *   and then restored back to its original value when the registers are
   *   restored. All parameters must be passed in registers on the CPOOL request
   *   because of R13 being changed.
   *
   */

  if (conditional) {
    __asm(

        ASM_PREFIX
        "         STMG  2,13,%[gprs]                                             \n"
        "         LA    13,%[gprs]                                               \n"
  #ifdef _LP64
        "         SAM31                                                          \n"
  #endif

        "         CPOOL GET,C,CPID=(%[cpid]),REGS=USE                            \n"

  #ifdef _LP64
        "         SAM64                                                          \n"
  #endif
        "         LMG   2,13,0(13)                                               \n"

        : [cell]"=NR:r1"(cell)
        : [gprs]"m"(callerGPRs), [cpid]"NR:r1"(cellpoolID)
        : "r0", "r1", "r14", "r15"
    );
  } else {
    __asm(

        ASM_PREFIX
        "         STMG  2,13,%[gprs]                                             \n"
        "         LA    13,%[gprs]                                               \n"
  #ifdef _LP64
        "         SAM31                                                          \n"
  #endif

        "         CPOOL GET,U,CPID=(%[cpid]),REGS=USE                            \n"

  #ifdef _LP64
        "         SAM64                                                          \n"
  #endif
        "         LMG   2,13,0(13)                                               \n"

        : [cell]"=NR:r1"(cell)
        : [gprs]"m"(callerGPRs), [cpid]"NR:r1"(cellpoolID)
        : "r0", "r1", "r14", "r15"
    );
  }

  return cell;
}

void cellpoolFree(CPID cellpoolID, void *cell) {

  uint64_t callerGPRs[12] = {0};

  /*
   * Notes about the use of callerGPRs:
   *
   * - The registers must be saved before switching to AMODE 31 and restored
   *   after switching back to AMODE 64, because the stack storage containing
   *   the callerGPRs may be above 2G.
   *
   * - Register 13 is being saved in callerGPRs, changed to point to callerGPRs,
   *   and then restored back to its original value when the registers are
   *   restored. All parameters must be passed in registers on the CPOOL request
   *   because of R13 being changed.
   *
   */

  __asm(

      ASM_PREFIX
      "         STMG  2,13,%[gprs]                                             \n"
      "         LA    13,%[gprs]                                               \n"
#ifdef _LP64
      "         SAM31                                                          \n"
#endif

      "         CPOOL FREE,CPID=(%[cpid]),CELL=(%[cell]),REGS=USE              \n"

#ifdef _LP64
      "         SAM64                                                          \n"
#endif
      "         LMG   2,13,0(13)                                               \n"

      :
      : [gprs]"m"(callerGPRs), [cpid]"NR:r1"(cellpoolID), [cell]"NR:r0"(cell)
      : "r0", "r1", "r14", "r15"
  );

}

/* Tests (TODO move to a designated place)

LE:

xlc "-Wa,goff" \
"-Wc,LANGLVL(EXTC99),FLOAT(HEX),agg,exp,list(),so(),goff,xref,gonum,roconst,gonum,ASM,ASMLIB('SYS1.MACLIB'),LP64,XPLINK" \
-DCELLPOOL_TEST -I ../h -o cellpool \
alloc.c \
cellpool.c \
timeutls.c \
utils.c \
zos.c \

Metal:

CFLAGS=(-S -M -qmetal -q64 -DSUBPOOL=132 -DMETTLE=1 -DMSGPREFIX='"IDX"'
-qreserved_reg=r12
-Wc,"arch(8),agg,exp,list(),so(),off,xref,roconst,longname,lp64"
-I ../h )

ASFLAGS=(-mgoff -mobject -mflag=nocont --TERM --RENT)

LDFLAGS=(-V -b ac=1 -b rent -b case=mixed -b map -b xref -b reus)

xlc "${CFLAGS[@]}" -DCELLPOOL_TEST \
alloc.c \
cellpool.c \
metalio.c \
qsam.c \
timeutls.c \
utils.c \
zos.c \

as "${ASFLAGS[@]}" -aegimrsx=alloc.asm alloc.s
as "${ASFLAGS[@]}" -aegimrsx=cellpool.asm cellpool.s
as "${ASFLAGS[@]}" -aegimrsx=metalio.asm metalio.s
as "${ASFLAGS[@]}" -aegimrsx=qsam.asm qsam.s
as "${ASFLAGS[@]}" -aegimrsx=timeutls.asm timeutls.s
as "${ASFLAGS[@]}" -aegimrsx=utils.asm utils.s
as "${ASFLAGS[@]}" -aegimrsx=zos.asm zos.s

ld "${LDFLAGS[@]}" -e main \
-o "//'$USER.DEV.LOADLIB(CELLPOOL)'" \
alloc.o \
cellpool.o \
metalio.o \
qsam.o \
timeutls.o \
utils.o \
zos.o \
> CELLPOOL.link

*/

#define CELLPOOL_TEST_STATUS_OK        0
#define CELLPOOL_TEST_STATUS_FAILURE   8

static int testUnconditionalCellPoolGet(void) {

  unsigned psize = 10;
  unsigned ssize = 2;
  unsigned cellSize = 512;
  int sp = 132, key = 8;
  CPHeader header = {"TEST-CP-HEADER"};
  bool isConditional = false;

  CPID id = cellpoolBuild(psize, ssize, cellSize, sp, key, &header);
  if (id == CPID_NULL) {
    printf("error: cellpoolBuild failed\n");
    return CELLPOOL_TEST_STATUS_FAILURE;
  }

  int status = CELLPOOL_TEST_STATUS_OK;

  for (int i = 0; i < 100; i++) {
    void *cell = cellpoolGet(id, isConditional);
    if (cell == NULL) {
      printf("error: cellpoolGet(unconditional) test failed, cell #%d\n", i);
      status = CELLPOOL_TEST_STATUS_FAILURE;
      break;
    }
  }

  cellpoolDelete(id);

  return status;
}

static int testConditionalCellPoolGet(void) {

  unsigned psize = 10;
  unsigned ssize = 2;
  unsigned cellSize = 512;
  int sp = 132, key = 8;
  CPHeader header = {"TEST-CP-HEADER"};
  bool isConditional = true;

  CPID id = cellpoolBuild(psize, ssize, cellSize, sp, key, &header);
  if (id == CPID_NULL) {
    printf("error: cellpoolBuild failed\n");
    return CELLPOOL_TEST_STATUS_FAILURE;
  }

  int status = CELLPOOL_TEST_STATUS_FAILURE;

  for (int i = 0; i < psize + 1; i++) {
    void *cell = cellpoolGet(id, isConditional);
    if (cell == NULL && i == psize) {
        status = CELLPOOL_TEST_STATUS_OK;
        break;
    }
  }

  if (status != CELLPOOL_TEST_STATUS_OK) {
    printf("error: cellpoolGet(conditional) test failed\n");
  }

  cellpoolDelete(id);

  return status;
}

static int testCellPoolFree(void) {

  unsigned psize = 10;
  unsigned ssize = 2;
  unsigned cellSize = 512;
  int sp = 132, key = 8;
  CPHeader header = {"TEST-CP-HEADER"};
  bool isConditional = true;

  void *cells[10] = {0};

  CPID id = cellpoolBuild(psize, ssize, cellSize, sp, key, &header);
  if (id == CPID_NULL) {
    printf("error: cellpoolBuild failed\n");
    return CELLPOOL_TEST_STATUS_FAILURE;
  }

  int status = CELLPOOL_TEST_STATUS_OK;

  for (int i = 0; i < sizeof(cells) / sizeof(cells[0]); i++) {
    cells[i] = cellpoolGet(id, isConditional);
    if (cells[i] == NULL) {
      printf("error: cellpoolFree test failed (alloc 1), cell #%d\n", i);
      status = CELLPOOL_TEST_STATUS_FAILURE;
      break;
    }
  }

  if (status == CELLPOOL_TEST_STATUS_OK) {

    for (int i = 0; i < sizeof(cells) / sizeof(cells[0]); i++) {
      cellpoolFree(id, cells[i]);
      cells[i] = NULL;
    }

    for (int i = 0; i < sizeof(cells) / sizeof(cells[0]); i++) {
      cells[i] = cellpoolGet(id, isConditional);
      if (cells[i] == NULL) {
        printf("error: cellpoolFree test failed (alloc 2), cell #%d\n", i);
        status = CELLPOOL_TEST_STATUS_FAILURE;
        break;
      }
    }

  }

  cellpoolDelete(id);

  return status;
}


static int testCellPool(void) {

  int status = CELLPOOL_TEST_STATUS_OK;

  if (status == CELLPOOL_TEST_STATUS_OK) {
    status = testUnconditionalCellPoolGet();
  }

  if (status == CELLPOOL_TEST_STATUS_OK) {
    status = testConditionalCellPoolGet();
  }

  if (status == CELLPOOL_TEST_STATUS_OK) {
    status = testCellPoolFree();
  }

  return status;
}

#ifdef _LP64

ZOWE_PRAGMA_PACK

typedef struct IARCP64ParmList_tag {
  unsigned char version;
  unsigned char flags1;
#define IARCP64_FLAGS1_COMMON_YES       0x80
#define IARCP64_FLAGS1_FPROT_YES        0x40
#define IARCP64_FLAGS1_TYPE_DREF        0x20
#define IARCP64_FLAGS1_TYPE_FIXED       0x10
#define IARCP64_FLAGS1_PRESERVE_YES     0x08
#define IARCP64_FLAGS1_FAILMODE_RC      0x04
#define IARCP64_FLAGS1_MEMLIMIT_NO      0x02
#define IARCP64_FLAGS1_KEYUSED_MOTKN    0x01
  unsigned char flags2;
#define IARCP64_FLAGS2_OWNER_PRIMARY    0x20
#define IARCP64_FLAGS2_OWNER_SECONDARY  0x10
#define IARCP64_FLAGS2_OWNER_HOME       0x08
#define IARCP64_FLAGS2_OWNER_BYASID     0x02
#define IARCP64_FLAGS2_CALLERKEY_NO     0x01
  unsigned char flags3;
#define IARCP64_FLAGS3_TRAILER_YES      0x80
#define IARCP64_FLAGS3_TRAILER_NO       0x40
#define IARCP64_FLAGS3_LOCALSYSAREA_YES 0x20
#define IARCP64_FLAGS3_EXTENTSIZE_2G    0x10
  unsigned char dumpprio;
  unsigned char dump;
#define IARCP64_DUMP_LIKERGN            0x80
#define IARCP64_DUMP_LIKELSQA           0x40
#define IARCP64_DUMP_LIKECSA            0x20
#define IARCP64_DUMP_LIKESQA            0x10
  unsigned char owningTask;
#define IARCP64_OWNINGTASK_MOTHER       0x80
#define IARCP64_OWNINGTASK_IPT          0x40
#define IARCP64_OWNINGTASK_JOBSTEP      0x20
#define IARCP64_OWNINGTASK_CMRO         0x10
#define IARCP64_OWNINGTASK_RCT          0x08
#define IARCP64_OWNINGTASK_CURRENT      0x00
  char reserved0[2];
  unsigned char key00ToF0;
  unsigned short owningASID;
  unsigned int cellSize;
  union {
    struct {
      CPID64 outputCPID;
      CPHeader header;
      uint64_t motkn;
      char reserved1[16];
    };
    struct {
      CPID64 inputCPID;
    };
  };
} IARCP64ParmList;

ZOWE_PRAGMA_PACK_RESET

ZOWE_PRAGMA_PACK

typedef struct BuildFlags_tag {
  char flags1;
  char flags2;
  char flags3;
} BuildFlags;

typedef unsigned char OwningTask;

ZOWE_PRAGMA_PACK_RESET

static int iarcp64Build(const CPHeader *header,
                        unsigned int cellSize,
                        BuildFlags flags,
                        OwningTask task,
                        CPID64 *id,
                        int *reasonCode) {

  IARCP64ParmList cpParmList;
  memset(&cpParmList, 0, sizeof(IARCP64ParmList));
  cpParmList.flags1 |= IARCP64_FLAGS1_FPROT_YES;
  cpParmList.flags1 |= IARCP64_FLAGS1_FAILMODE_RC;
  cpParmList.flags3 |= IARCP64_FLAGS3_TRAILER_NO;
  cpParmList.flags1 |= flags.flags1;
  cpParmList.flags2 |= flags.flags2;
  cpParmList.flags3 |= flags.flags3;
  cpParmList.dumpprio = 50;
  cpParmList.dump |= IARCP64_DUMP_LIKERGN;
  cpParmList.owningTask = task;
  cpParmList.cellSize = cellSize;
  memcpy(&cpParmList.header, header, sizeof(CPHeader));

  int rc = 0, rsn = 0;
  uint64_t parmListAddress = (uint64)&cpParmList;

  __asm(
      ASM_PREFIX
      /* load parm list to R1 */
      "         LG    1,0(,%2)                                                 \n"
#ifndef _LP64
      "         SAM64                                                          \n"
#endif
      /* call IARCP64 */
      "         LG    14,72(0,0)                                               \n"
      "         L     14,772(14,0)                                             \n"
      "         L     14,208(14,0)                                             \n"
      "         OILL  14,16                                                    \n"
      "         PC    0(14)                                                    \n"
#ifndef _LP64
      "         SAM31                                                          \n"
#endif
      /* save RC and RSN */
      "         ST    15,%0                                                    \n"
      "         ST    0,%1                                                     \n"
      : "=m"(rc), "=m"(rsn)
      : "r"(&parmListAddress)
      : "r0", "r1", "r14", "r15"
  );

  if (rc == 0) {
    *id = cpParmList.inputCPID;
  } else {
    *reasonCode = rsn;
  }
  return rc;
}

#define IARCP64_GET_FLAGS_FAILMODE_RC   0x0001
#define IARCP64_GET_FLAGS_EXPAND_NO     0x0002
#define IARCP64_GET_FLAGS_TRACE_NO      0x0004

static int iarcp64Get(CPID64 id,
                      unsigned short flags,
                      void **cell,
                      int *reasonCode) {

  struct {
    char eyecatcher[8];
    uint64_t prev;
    uint64_t next;
    uint64_t gprs[15];
  } iarcp64SaveArea;

  int rc = 0, rsn = 0;
  void *localCell;

  __asm(
      ASM_PREFIX
      /* load input parms (flags, pool ID) */
      "         LLGH  0,0(,%2)                                                 \n"
      "         LG    1,0(,%3)                                                 \n"
      /* save GPRs */
      "         STG   13,8(,%4)                                                \n"
      "         LA    13,0(,%4)                                                \n"
      "         STMG  2,12,40(13)                                              \n"
#ifndef _LP64
      "         SAM64                                                          \n"
#endif
      /* call IARCP64 */
      "         LG    14,X'7A8'(0,0)                                           \n"
      "         LG    15,X'008'(14,0)                                          \n"
      "         BASR  14,15                                                    \n"
#ifndef _LP64
      "         SAM31                                                          \n"
#endif
      /* restore GPRs */
      "         LMG   2,12,40(13)                                              \n"
      "         LG    13,8(,13)                                                \n"
      /* save cell, RC and RSN */
      "         STG   1,0(,%5)                                                 \n"
      "         ST    15,%0                                                    \n"
      "         ST    0,%1                                                     \n"
      : "=m"(rc), "=m"(rsn)
      : "r"(&flags), "r"(&id), "r"(&iarcp64SaveArea), "r"(&localCell)
      : "r0", "r1", "r14", "r15"
  );


  if (rc == 0) {
    *cell = localCell;
  } else {
    *reasonCode = rsn;
  }
  return rc;
}

static void iarcp64Free(void *cell) {

  struct {
    char eyecatcher[8];
    uint64_t prev;
    uint64_t next;
    uint64_t gprs[15];
  } iarcp64SaveArea;

  __asm(
      ASM_PREFIX
      /* load cell to R1 */
      "         LG    1,0(,%0)                                                 \n"
      /* save GPRs */
      "         STG   13,8(,%1)                                                \n"
      "         LA    13,0(,%1)                                                \n"
      "         STMG  2,12,40(13)                                              \n"
#ifndef _LP64
      "         SAM64                                                          \n"
#endif
      /* call IARCP64 */
      "         LG    14,X'7A8'(0,0)                                           \n"
      "         LG    15,X'010'(14,0)                                          \n"
      "         BASR  14,15                                                    \n"
#ifndef _LP64
      "         SAM31                                                          \n"
#endif
      /* restore GPRs */
      "         LMG   2,12,40(13)                                              \n"
      "         LG    13,8(,13)                                                \n"
      :
      : "r"(&cell), "r"(&iarcp64SaveArea)
      : "r0", "r1", "r14", "r15"
  );

}

static void iarcp64Delete(CPID64 id) {

  __asm(
      ASM_PREFIX
      /* load pool ID to R1 */
      "         LG    1,0(,%0)                                                 \n"
#ifndef _LP64
      "         SAM64                                                          \n"
#endif
      /* call IARCP64 */
      "         LG    14,72(0,0)                                               \n"
      "         L     14,772(14,0)                                             \n"
      "         L     14,208(14,0)                                             \n"
      "         OILL  14,21                                                    \n"
      "         PC    0(14)                                                    \n"
#ifndef _LP64
      "         SAM31                                                          \n"
#endif
      :
      : "r"(&id)
      : "r0", "r1", "r14", "r15"
  );

}

#define CELL_POOL_FLAGS_NONE                0x00000000
#define CELL_POOL_FLAGS_CROSS_MEMORY_OWNER  0x00000001

CPID64 cellpoolBuild64(unsigned int cellSize,
                       const CPHeader *header,
                       int *serviceRC) {

  CPID64 id;

  BuildFlags iarcp64Flags = {0, 0, 0};
  OwningTask owningTask = IARCP64_OWNINGTASK_CURRENT;

  int buildRC = 0, buildRSN = 0;
  buildRC = iarcp64Build(header, cellSize, iarcp64Flags, owningTask, &id,
                         &buildRSN);
  if (buildRC != 0) {
    if (serviceRC != NULL) {
      *serviceRC = (buildRC << 16);
      *serviceRC |= ((buildRSN >> 8) & 0x0000FFFF);
    }
  }

  return id;
}

CPID64 cellpoolBuild64Xmem(unsigned int cellSize,
                           const CPHeader *header,
                           int *serviceRC) {

  CPID64 id;

  BuildFlags iarcp64Flags = {0, 0, 0};
  OwningTask owningTask = IARCP64_OWNINGTASK_CMRO;

  int buildRC = 0, buildRSN = 0;
  buildRC = iarcp64Build(header, cellSize, iarcp64Flags, owningTask, &id,
                         &buildRSN);
  if (buildRC != 0) {
    if (serviceRC != NULL) {
      *serviceRC = (buildRC << 16);
      *serviceRC |= ((buildRSN >> 8) & 0x0000FFFF);
    }
  }

  return id;
}

void cellpoolDelete64(CPID64 id) {

  iarcp64Delete(id);

}

void *cellpoolGet64(CPID64 id,
                    bool noExpansion,
                    int *serviceRC) {

  unsigned short flags = 0;
  flags |= IARCP64_GET_FLAGS_TRACE_NO;
  flags |= IARCP64_GET_FLAGS_FAILMODE_RC;
  if (noExpansion) {
    flags |= IARCP64_GET_FLAGS_EXPAND_NO;
  }

  void *cell = NULL;

  int getRC = 0, getRSN = 0;
  getRC = iarcp64Get(id, flags, &cell, &getRSN);
  if (getRC != 0) {
    if (serviceRC != NULL) {
      *serviceRC = (getRC << 16);
      *serviceRC |= ((getRSN >> 8) & 0x0000FFFF);
    }
  }

  return cell;
}

void cellpoolFree64(CPID64 id, void *cell) {
  iarcp64Free(cell);
}

#endif /* _LP64 */

#ifdef CELLPOOL_TEST
int main() {
#else
static int notMain() {
#endif

  printf("info: starting cellpool test\n");

  int status = CELLPOOL_TEST_STATUS_OK;

  status = testCellPool();

  if (status == CELLPOOL_TEST_STATUS_OK) {
    printf("info: SUCCESS, tests have passed\n");
  } else {
    printf("error: FAILURE, some tests have failed\n");
  }

  return status;
}

/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/

