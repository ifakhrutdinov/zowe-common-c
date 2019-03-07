#include <metal/metal.h>
#include <metal/stddef.h>
#include <metal/stdio.h>
#include <metal/string.h>

#include "as.h"
#include "metalio.h"

/*

COMMON=../../zowe-common-c

CFLAGS=(-S -M -qmetal -q64 -DSUBPOOL=132 -DMETTLE=1 -DMSGPREFIX='"IDX"'
-qreserved_reg=r12
-Wc,"arch(8),agg,exp,list(),so(),off,xref,roconst,longname,lp64"
-I $COMMON/h )

ASFLAGS=(-mgoff -mobject -mflag=nocont --TERM --RENT)

LDFLAGS=(-V -b ac=1 -b rent -b case=mixed -b map -b xref -b reus)

xlc "${CFLAGS[@]}" -DCMS_CLIENT \
$COMMON/c/alloc.c \
$COMMON/c/as.c \
$COMMON/c/metalio.c \
$COMMON/c/qsam.c \
$COMMON/c/timeutls.c \
$COMMON/c/utils.c \
$COMMON/c/zos.c \
$COMMON/c/astest-prog.c \

as "${ASFLAGS[@]}" -aegimrsx=alloc.asm alloc.s
as "${ASFLAGS[@]}" -aegimrsx=as.asm as.s
as "${ASFLAGS[@]}" -aegimrsx=metalio.asm metalio.s
as "${ASFLAGS[@]}" -aegimrsx=qsam.asm qsam.s
as "${ASFLAGS[@]}" -aegimrsx=timeutls.asm timeutls.s
as "${ASFLAGS[@]}" -aegimrsx=utils.asm utils.s
as "${ASFLAGS[@]}" -aegimrsx=zos.asm zos.s
as "${ASFLAGS[@]}" -aegimrsx=astest-prog.asm astest-prog.s

export _LD_SYSLIB="//'SYS1.CSSLIB'://'CEE.SCEELKEX'://'CEE.SCEELKED'://'CEE.SCEERUN'://'CEE.SCEERUN2'://'CSF.SCSFMOD0'"

ld "${LDFLAGS[@]}" -e main \
-o "//'$USER.DEV.LOADLIB(ASTPROG)'" \
alloc.o \
as.o \
metalio.o \
qsam.o \
timeutls.o \
utils.o \
zos.o \
astest-prog.o \
> ASTPROG.link

*/

static void sleep(int seconds){
  int waitValue = seconds * 100;
  __asm(" STIMER WAIT,BINTVL=%0\n" : : "m"(waitValue));
}

int main() {

  supervisorMode(TRUE);

  int extractRC = 0, extractRSN = 0;
  ASParm *parm = NULL;

  extractRC = addressSpaceExtractParm(&parm, &extractRSN);
  printf("address space extract RC = %d, RSN = %d, parm = 0x%p\n",
         extractRC, extractRSN, parm);
  if (parm) {
    dumpbuffer(parm->data, parm->length);
  }

  for (int i = 0; i < 100; i++) {
    printf("Hello from custom a address space\n");
    sleep(2);
  }

  return 0;
}
