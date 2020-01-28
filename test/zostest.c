#ifdef METTLE
#include <metal/metal.h>
#include <metal/stdarg.h>
#include <metal/stddef.h>
#include <metal/stdio.h>
#include <metal/stdlib.h>
#include <metal/string.h>
#include "metalio.h"
#else
#include "stdarg.h"
#include "stddef.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#endif

/*

Metal C 31-bit:

xlc -S -M -qmetal -DSUBPOOL=132 -DMETTLE=1 -DMSGPREFIX='"IDX"' -qreserved_reg=r12 -Wc,"arch(8),agg,exp,list(),so(),off,xref,roconst,longname" -I ../h -o alloc.s ../c/alloc.c
as -mgoff -mobject -mflag=nocont --TERM --RENT -aegimrsx=alloc.asm -o alloc.o alloc.s
xlc -S -M -qmetal -DSUBPOOL=132 -DMETTLE=1 -DMSGPREFIX='"IDX"' -qreserved_reg=r12 -Wc,"arch(8),agg,exp,list(),so(),off,xref,roconst,longname" -I ../h -o metalio.s ../c/metalio.c
as -mgoff -mobject -mflag=nocont --TERM --RENT -aegimrsx=metalio.asm -o metalio.o metalio.s
xlc -S -M -qmetal -DSUBPOOL=132 -DMETTLE=1 -DMSGPREFIX='"IDX"' -qreserved_reg=r12 -Wc,"arch(8),agg,exp,list(),so(),off,xref,roconst,longname" -I ../h -o qsam.s ../c/qsam.c
as -mgoff -mobject -mflag=nocont --TERM --RENT -aegimrsx=qsam.asm -o qsam.o qsam.s
xlc -S -M -qmetal -DSUBPOOL=132 -DMETTLE=1 -DMSGPREFIX='"IDX"' -qreserved_reg=r12 -Wc,"arch(8),agg,exp,list(),so(),off,xref,roconst,longname" -I ../h -o timeutls.s ../c/timeutls.c
as -mgoff -mobject -mflag=nocont --TERM --RENT -aegimrsx=timeutls.asm -o timeutls.o timeutls.s
xlc -S -M -qmetal -DSUBPOOL=132 -DMETTLE=1 -DMSGPREFIX='"IDX"' -qreserved_reg=r12 -Wc,"arch(8),agg,exp,list(),so(),off,xref,roconst,longname" -I ../h -o utils.s ../c/utils.c
as -mgoff -mobject -mflag=nocont --TERM --RENT -aegimrsx=utils.asm -o utils.o utils.s
xlc -S -M -qmetal -DSUBPOOL=132 -DMETTLE=1 -DMSGPREFIX='"IDX"' -qreserved_reg=r12 -Wc,"arch(8),agg,exp,list(),so(),off,xref,roconst,longname" -I ../h -o zos.s ../c/zos.c
as -mgoff -mobject -mflag=nocont --TERM --RENT -aegimrsx=zos.asm -o zos.o zos.s

cp zostest.c zostest.c
xlc -S -M -qmetal -DSUBPOOL=132 -DMETTLE=1 -DMSGPREFIX='"IDX"' -qreserved_reg=r12 -Wc,"arch(8),agg,exp,list(),so(),off,xref,roconst,longname" -I h -I ../h -o zostest.s zostest.c
as -mgoff -mobject -mflag=nocont --TERM --RENT -aegimrsx=zostest.asm -o zostest.o zostest.s

export _LD_SYSLIB="//'SYS1.CSSLIB'://'CEE.SCEELKEX'://'CEE.SCEELKED'://'CEE.SCEERUN'://'CEE.SCEERUN2'://'CSF.SCSFMOD0'"
ld -V -b ac=1 -b rent -b case=mixed -b map -b xref -b reus -e main -o "//'$USER.DEV.LOADLIB(ZOSTEST)'" \
zostest.o alloc.o metalio.o qsam.o timeutls.o utils.o zos.o > zostest.link

Metal C 64-bit:

xlc -S -M -qmetal -q64 -DSUBPOOL=132 -DMETTLE=1 -DMSGPREFIX='"IDX"' -qreserved_reg=r12 -Wc,"arch(8),agg,exp,list(),so(),off,xref,roconst,longname,lp64" -I ../h -o alloc.s ../c/alloc.c
as -mgoff -mobject -mflag=nocont --TERM --RENT -aegimrsx=alloc.asm -o alloc.o alloc.s
xlc -S -M -qmetal -q64 -DSUBPOOL=132 -DMETTLE=1 -DMSGPREFIX='"IDX"' -qreserved_reg=r12 -Wc,"arch(8),agg,exp,list(),so(),off,xref,roconst,longname,lp64" -I ../h -o metalio.s ../c/metalio.c
as -mgoff -mobject -mflag=nocont --TERM --RENT -aegimrsx=metalio.asm -o metalio.o metalio.s
xlc -S -M -qmetal -q64 -DSUBPOOL=132 -DMETTLE=1 -DMSGPREFIX='"IDX"' -qreserved_reg=r12 -Wc,"arch(8),agg,exp,list(),so(),off,xref,roconst,longname,lp64" -I ../h -o qsam.s ../c/qsam.c
as -mgoff -mobject -mflag=nocont --TERM --RENT -aegimrsx=qsam.asm -o qsam.o qsam.s
xlc -S -M -qmetal -q64 -DSUBPOOL=132 -DMETTLE=1 -DMSGPREFIX='"IDX"' -qreserved_reg=r12 -Wc,"arch(8),agg,exp,list(),so(),off,xref,roconst,longname,lp64" -I ../h -o timeutls.s ../c/timeutls.c
as -mgoff -mobject -mflag=nocont --TERM --RENT -aegimrsx=timeutls.asm -o timeutls.o timeutls.s
xlc -S -M -qmetal -q64 -DSUBPOOL=132 -DMETTLE=1 -DMSGPREFIX='"IDX"' -qreserved_reg=r12 -Wc,"arch(8),agg,exp,list(),so(),off,xref,roconst,longname,lp64" -I ../h -o utils.s ../c/utils.c
as -mgoff -mobject -mflag=nocont --TERM --RENT -aegimrsx=utils.asm -o utils.o utils.s
xlc -S -M -qmetal -q64 -DSUBPOOL=132 -DMETTLE=1 -DMSGPREFIX='"IDX"' -qreserved_reg=r12 -Wc,"arch(8),agg,exp,list(),so(),off,xref,roconst,longname,lp64" -I ../h -o zos.s ../c/zos.c
as -mgoff -mobject -mflag=nocont --TERM --RENT -aegimrsx=zos.asm -o zos.o zos.s

xlc -S -M -qmetal -q64 -DSUBPOOL=132 -DMETTLE=1 -DMSGPREFIX='"IDX"' -qreserved_reg=r12 -Wc,"arch(8),agg,exp,list(),so(),off,xref,roconst,longname,lp64" -I h -I ../h -o zostest.s zostest.c
as -mgoff -mobject -mflag=nocont --TERM --RENT -aegimrsx=zostest.asm -o zostest.o zostest.s

export _LD_SYSLIB="//'SYS1.CSSLIB'://'CEE.SCEELKEX'://'CEE.SCEELKED'://'CEE.SCEERUN'://'CEE.SCEERUN2'://'CSF.SCSFMOD0'"
ld -V -b ac=1 -b rent -b case=mixed -b map -b xref -b reus -e main -o "//'$USER.DEV.LOADLIB(ZOSTEST)'" \
zostest.o alloc.o metalio.o qsam.o timeutls.o utils.o zos.o > zostest.link

LE 31-bit:

xlc -D_OPEN_THREADS=1 "-Wa,goff" "-Wc,LANGLVL(EXTC99),FLOAT(HEX),agg,exp,list(),so(),goff,xref,gonum,roconst,gonum,ASM,ASMLIB('SYS1.MACLIB'),ASMLIB('CEE.SCEEMAC')" '-Wl,ac=1' \
-I ../h -o zostest zostest.c ../c/alloc.c ../c/timeutls.c ../c/utils.c ../c/zos.c

LE 64-bit:

xlc -D_OPEN_THREADS=1 "-Wa,goff" "-Wc,LP64,XPLINK,LANGLVL(EXTC99),FLOAT(HEX),agg,exp,list(),so(),goff,xref,gonum,roconst,gonum,ASM,ASMLIB('SYS1.MACLIB'),ASMLIB('CEE.SCEEMAC')" '-Wl,ac=1' \
-I ../h -o zostest zostest.c ../c/alloc.c ../c/timeutls.c ../c/utils.c ../c/zos.c

*/

#include "zowetypes.h"
#include "alloc.h"
#include "utils.h"
#include "zos.h"

#ifdef METTLE

#ifdef _LP64

#pragma prolog(srb_routine,\
"        PUSH  USING                              \n\
SRBPRLG  DS    0H                                 \n\
         BAKR  14,0                               \n\
         SAM64                                    \n\
         LARL  10,SRBPRLG                         \n\
         USING SRBPRLG,10                         \n\
         LA    2,64                               \n\
         SPKA  0(2)                               \n\
         STORAGE OBTAIN,COND=YES,LENGTH=8192,KEY=4,SP=230,LOC=31,BNDRY=PAGE \n\
         LTR   15,15                              \n\
         JNZ   SRBRET                             \n\
SRBINIT  DS    0H                                 \n\
         LLGFR 13,1                               \n\
         USING SRBSTACK,13                        \n\
         EREGG 0,1                                \n\
         LA    13,SRBSA                           \n\
\
         J     SRBHNDLR                           \n\
\
         LTORG                                    \n\
         DROP  10                                 \n\
SRBHNDLR DS    0H                                 \n\
         DROP                                     \n\
         POP   USING                               ")

#else

#pragma prolog(srb_routine,\
"        PUSH  USING                              \n\
SRBPRLG  DS    0H                                 \n\
         BAKR  14,0                               \n\
         SAM31                                    \n\
         LARL  10,SRBPRLG                         \n\
         USING SRBPRLG,10                         \n\
         LA    2,64                               \n\
         SPKA  0(2)                               \n\
         STORAGE OBTAIN,COND=YES,LENGTH=8192,KEY=4,SP=230,LOC=31,BNDRY=PAGE \n\
         LTR   15,15                              \n\
         JNZ   SRBRET                             \n\
SRBINIT  DS    0H                                 \n\
         LLGFR 13,1                               \n\
         USING SRBSTACK,13                        \n\
         EREGG 0,1                                \n\
         LA    13,SRBSA                           \n\
\
         J     SRBHNDLR                           \n\
\
         LTORG                                    \n\
         DROP  10                                 \n\
SRBHNDLR DS    0H                                 \n\
         DROP                                     \n\
         POP   USING                               ")

#endif

#pragma epilog(srb_routine,\
"        PUSH  USING                              \n\
SRBEPLG  DS    0H                                 \n\
         LARL  10,SRBEPLG                         \n\
         USING SRBEPLG,10                         \n\
         SLFI  13,SRBPARML                        \n\
         LR    1,13                               \n\
         STORAGE RELEASE,COND=NO,LENGTH=8192,KEY=4,SP=230,ADDR=(1) \n\
SRBRET   DS    0H                                 \n\
         DROP  10                                 \n\
         LR    15,0                               \n\
         EREGG 0,1                                \n\
         SPKA  0                                  \n\
         PR                                       \n\
         DROP                                     \n\
         POP   USING                               ")

static void wto(const char *format, ...) {

  __packed struct {
    __packed struct {
      uint8_t reply_buff_len;
      uint8_t length;
      uint8_t flags1;
      uint8_t flags2;
    } header;
    char msg_buff[126];
  } wto_parm = {0};

  va_list args;
  va_start(args, format);
  int print_rc = vsnprintf(wto_parm.msg_buff, sizeof(wto_parm.msg_buff),
                           format, args);
  va_end(args);

  if (print_rc >= 0) {
    wto_parm.header.length =
        sizeof(wto_parm.header) + strlen(wto_parm.msg_buff);
  } else {
    return;
  }

  __asm(
#ifdef _LP64
      "         SAM31                                                          \n"
      "         SYSSTATE AMODE64=NO                                            \n"
#endif
      "         WTO     LINKAGE=BRANCH,MF=(E,%[parm])                          \n"
#ifdef _LP64
      "         SAM64                                                          \n"
      "         SYSSTATE AMODE64=YES                                           \n"
#endif
      :
      : [parm]"m"(wto_parm)
      : "r0", "r1", "r14", "r15"
  );

}

static void srb_routine(void) {

#ifdef _LP64
  wto("IREK_TEST: hi from an SRB 64");
#else
  wto("IREK_TEST: hi from an SRB 31");
#endif

  bool isSRB = isCallerSRB();

  wto("IREK_TEST: done with SRB, isSRB=%d", isSRB);

}

 typedef struct stoken_t {
   char value[8];
 } stoken_t;

static int sched_sync_srb(void * __ptr32 ep,
                          void * __ptr32 parm,
                          stoken_t token,
                          void * __ptr32 frr_addr,
                          int * __ptr32 comp_code,
                          int * __ptr32 srb_rc,
                          int * __ptr32 srb_rsn) {

  char macro_parm_list[512];

  int rc = 0;

  unsigned int local_ep = ((int)ep) | 0x80000000;
  unsigned int local_parm = (int)parm;
  unsigned int local_frr_addr =
      (frr_addr != NULL) ? (((int)frr_addr) | 0x80000000) : 0;
  stoken_t local_stoken = token;

  __asm(
      ASM_PREFIX

#ifdef _LP64
      "         SAM31                                                          \n"
      "         SYSSTATE AMODE64=NO                                            \n"
#endif

      "         IEAMSCHD EPADDR=(%[ep])"
      ",ENV=STOKEN"
      ",PRIORITY=LOCAL"
      ",SYNCH=YES"
      ",PARM=(%[parm])"
      ",TARGETSTOKEN=(%[stoken])"
      ",FRRADDR=(%[frr])"
      ",SYNCHCOMPADDR=%[cc]"
      ",SYNCHCODEADDR=%[srb_rc]"
      ",SYNCHRSNADDR=%[srb_rsn]"
      ",RETCODE=%[rc]"
      ",MF=(E,(%[macro_parm_list]),COMPLETE)                                   \n"

#ifdef _LP64
      "         SAM64                                                          \n"
      "         SYSSTATE AMODE64=YES                                           \n"
#endif

      : [rc]"=m"(rc),
        [cc]"=m"(comp_code),
        [srb_rc]"=m"(srb_rc),
        [srb_rsn]"=m"(srb_rsn)

      : [ep]"r"(&local_ep),
        [parm]"r"(&local_parm),
        [stoken]"r"(&local_stoken),
        [frr]"r"(&local_frr_addr),
        [macro_parm_list]"r"(&macro_parm_list)

      : "r0", "r1", "r14", "r15"

  );

  return rc;
}

static stoken_t get_my_stoken(void) {

  ASSB *assb = getASSB(getASCB());

  stoken_t token = *(stoken_t *)&assb->assbstkn;

  return token;
}

static int testSRB(void) {

  supervisorMode(TRUE);

  stoken_t my_stoken = get_my_stoken();

  int32_t sched_rc = 0;
  int32_t cc = 0;
  int32_t srb_rc = 0;
  int32_t srb_rsn = 0;

  sched_rc = sched_sync_srb((void *)srb_routine, NULL, my_stoken, NULL,
                            &cc, &srb_rc, &srb_rsn);

  printf("testSRB: sched_rc=%d, cc=%d, srb_rc=0x%08x, srb_rsn=0x%08x\n",
         sched_rc, cc, srb_rc, srb_rsn);

  supervisorMode(FALSE);

  return cc ? -1 : 0;
}

#endif /* METTLE */

static int ssar(int newValue) {

  int oldValue = 0;
  __asm(
      ASM_PREFIX
      "         ESAR  2                                                        \n"
      "         SSAR  1                                                        \n"
      : "=NR:r2"(oldValue)
      : "NR:r1"(newValue)
      :
  );

  return oldValue;
}

static int axset(int val) {

  int axsetRC = 0;
  __asm(
      ASM_PREFIX
      "         SYSSTATE PUSH                                                  \n"
      "         SYSSTATE OSREL=ZOSV1R6                                         \n"
#ifdef _LP64
      "         SAM31                                                          \n"
      "         SYSSTATE AMODE64=NO                                            \n"
#endif
      "         AXSET AX=(1)                                                   \n"
#ifdef _LP64
      "         SAM64                                                          \n"
      "         SYSSTATE AMODE64=YES                                           \n"
#endif
      "         ST    15,%0                                                    \n"
      "         SYSSTATE POP                                                   \n"
      : "=m"(axsetRC)
      : "NR:r1"(val)
      : "r0", "r1", "r14", "r15"
  );

  return axsetRC;
}

static int testCrossMemory(void) {

  int wasProblem = supervisorMode(TRUE);
  axset(1);

  int oldSASN = ssar(1);

  bool isXmem = isCallerCrossMemory();

  ssar(oldSASN);

  axset(0);
  if (wasProblem) {
    supervisorMode(FALSE);
  }

  printf("testCrossMemory: isCallerCrossMemory() = %d\n", isXmem);

  return isXmem ? 0 : -1;
}

static int testLocked(void) {

  int wasProblem = supervisorMode(TRUE);
  int oldKey = setKey(0);

  __asm(
      ASM_PREFIX
#ifdef _LP64
      "         SAM31                                                          \n"
      "         SYSSTATE AMODE64=NO                                            \n"
#endif
      "         SETLOCK OBTAIN,TYPE=LOCAL,MODE=COND,REGS=USE                   \n"
#ifdef _LP64
      "         SAM64                                                          \n"
      "         SYSSTATE AMODE64=YES                                           \n"
#endif
      "         NOPR  0                                                        \n"
      :
      :
      : "r0", "r1", "r14", "r15"
  );

  bool isLocked = isCallerLocked();

  __asm(
      ASM_PREFIX
#ifdef _LP64
      "         SAM31                                                          \n"
      "         SYSSTATE AMODE64=NO                                            \n"
#endif
      "         SETLOCK RELEASE,TYPE=LOCAL,REGS=USE                            \n"
#ifdef _LP64
      "         SAM64                                                          \n"
      "         SYSSTATE AMODE64=YES                                           \n"
#endif
      "         NOPR  0                                                        \n"
      :
      :
      : "r0", "r1", "r14", "r15"
  );

  setKey(oldKey);
  if (wasProblem) {
    supervisorMode(FALSE);
  }

  printf("testLocked: isCallerLocked() = %d\n", isLocked);

  return isLocked ? 0 : -1;
}

static int testNormalMode(void) {

  bool isSRB = isCallerSRB();
  bool isXmem = isCallerCrossMemory();
  bool isLocked = isCallerLocked();


 printf("testNormalMode: isCallerSRB() = %d\n", isSRB);
 printf("testNormalMode: isCallerCrossMemory() = %d\n", isXmem);
 printf("testNormalMode: isCallerLocked() = %d\n", isLocked);

  return (isSRB || isXmem || isLocked) ? -1 : 0;
}

int main() {

#ifdef _LP64
  printf("info: hello from zos 64\n");
#else
  printf("info: hello from zos 31\n");
#endif

  testNormalMode();
  testCrossMemory();
  testLocked();

#ifdef METTLE
  testSRB();
#endif

  printf("info: test is successful\n");

  return 0;
}

#ifdef METTLE
#pragma insert_asm(" IHAPSA   ")
#pragma insert_asm(" IHAASCB  ")
#else
void dsects_only_os_c(void) {
  __asm(" IHAPSA   \n"
        " IHAASCB  \n"
        );
}
#endif

void srb_test_dsect() {

  __asm(
      "         DS    0F                                                       \n"
      "SRBSTACK DSECT ,                                                        \n"
      /* parameters on stack */
      "SRBPARM  DS    0F                   SRB exit parm list                  \n"
      "SRBTPRM1 DS    A                    parm #1                             \n"
      "SRBTPRM  DS    A                    parm #1                             \n"
      "SRBPLPAD DS    0F                   padding                             \n"
      "SRBPARML EQU   *-SRBPARM                                                \n"
      /* 31-bit save area */
      "SRBSA    DS    0F                                                       \n"
      "SRBSARSV DS    CL4                                                      \n"
      "SRBSAPRV DS    A                    previous save area                  \n"
      "SRBSANXT DS    A                    next save area                      \n"
      "SRBSAGPR DS    15F                  GPRs                                \n"
      "SRBSTCKL EQU   *-SRBSTACK                                               \n"
      "         EJECT ,                                                        \n"
#ifdef METTLE
      "         CSECT ,                                                        \n"
      "         RMODE ANY                                                      \n"
#endif

  );

}
