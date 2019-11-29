#include <metal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "metalio.h"

#include "zowetypes.h"
#include "alloc.h"
#include "crossmemory.h"
#include "recovery.h"
#include "zos.h"

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
      "         SAM31                                                          \n"
      "         SYSSTATE AMODE64=NO                                            \n"
      "         WTO     LINKAGE=BRANCH,MF=(E,%[parm])                          \n"
      "         SAM64                                                          \n"
      "         SYSSTATE AMODE64=YES                                           \n"
      :
      : [parm]"m"(wto_parm)
      : "r0", "r1", "r14", "r15"
  );

}

int rcvrscxt(RecoveryContext *context) {

  RecoveryContext **recoveryContextHandle = NULL;
  __asm("  " : "=NR:r12"(recoveryContextHandle));

  *recoveryContextHandle = context;

  return RC_RCV_OK;
}

RecoveryContext *rcvrgcxt() {

  RecoveryContext **recoveryContextHandle = NULL;
  __asm("  " : "=NR:r12"(recoveryContextHandle));

  return *recoveryContextHandle;

}

static int writeSMF(const void * __ptr32 smfRecord) {

#pragma insert_asm(" IEESMCA , ")

  ALLOC_STRUCT31(
    STRUCT31_NAME(below2G),
    STRUCT31_FIELDS(
      char saveArea[72];
      char workArea[20];
    )
  );

  int rc = 0;

  int wasProblem = supervisorMode(TRUE);
  int oldKey = setKey(0);

  char subSystem[4] = "SYS ";

  __asm(
      ASM_PREFIX
#ifdef _LP64
      "         SAM31                                                          \n"
      "         SYSSTATE AMODE64=NO                                            \n"
#endif
      "         LR    2,13                                                     \n"
      "         LR    13,%[saveArea]                                           \n"
      "         SMFEWTM (%[record]),BRANCH=YES,SUBSYS=(%[ss]),WRKAREA=(%[wrka])\n"
      "         LR    13,2                                                     \n"
#ifdef _LP64
      "         SAM64                                                          \n"
      "         SYSSTATE AMODE64=YES                                           \n"
#endif

      : "=NR:r15"(rc)

      : [record]"r"(smfRecord),
        [saveArea]"r"(&below2G->saveArea),
        [ss]"r"(&subSystem),
        [wrka]"r"(&below2G->workArea)

      : "r0", "r1", "r2", "r14", "r15"
  );

  setKey(oldKey);
  if (wasProblem) {
    supervisorMode(FALSE);
  }

  FREE_STRUCT31(
    STRUCT31_NAME(below2G)
  );

  return rc;
}

static void srb_routine(void) {

  wto("IREK_TEST: hi from an SRB");

  RecoveryContext *recoveryContext = NULL;
  RecoveryContext **recoveryContextHandle = &recoveryContext;
  __asm("  " : : "NR:r12"(recoveryContextHandle));

  recoveryEstablishRouter(0);

  int pushRC = recoveryPush("srb_routine()",
                            RCVR_FLAG_RETRY | RCVR_FLAG_DELETE_ON_RETRY,
                            "Recovery dump",
                            NULL, NULL,
                            NULL, NULL);

  if (pushRC == RC_RCV_OK) {
    int i = 9 / 0;
  } else {
    wto("srb_routine recovered from an ABEND\n");
  }

  if (pushRC == RC_RCV_OK) {
    recoveryPop();
  }

  recoveryRemoveRouter();

  int cms_rc = cmsPrintf(&(CrossMemoryServerName){"ZWESIS_IRF      "},
                         "You've got a message from an SRB\n");

  char record[1024];
  memset(record, 'A', sizeof(record));

  __packed struct {
    short length;
    short segmentDescriptor;
    int flags : 8;
#define SMF_FLAG_SUBTYPE_VALID 0x40
    int type : 8;
    int time;
    int date;
    char sid[4];
    char ssi[4];
    uint16_t subtype;
  } *smfHeader = (void *)record;

  smfHeader->length = 888;
  smfHeader->flags |= SMF_FLAG_SUBTYPE_VALID;
  smfHeader->type = 30;
  smfHeader->time = 33333;
  smfHeader->date = 11111;
  memcpy(smfHeader->sid, "SS01", sizeof(smfHeader->sid));
  memcpy(smfHeader->ssi, "SYS ", sizeof(smfHeader->ssi));
  smfHeader->subtype = 112;

  int wasProblem = supervisorMode(TRUE);
  int oldKey = setKey(0);

  int rc;
  for (int i = 0 ; i < 10; i++) {
    rc = writeSMF(smfHeader);
    if (rc != 20) {
      break;
    }
  }

  wto("IREK_TEST: SMF rc=%d", rc);

  setKey(oldKey);
  if (wasProblem) {
    supervisorMode(FALSE);
  }

  wto("IREK_TEST: done with SRB, cms_rc=%d", cms_rc);

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

      "         SAM31                                                          \n"
      "         SYSSTATE AMODE64=NO                                            \n"

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

      "         SAM64                                                          \n"
      "         SYSSTATE AMODE64=YES                                           \n"

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

int main(int argc, char **argv) {

  printf("hello from SRB test\n");

  supervisorMode(TRUE);

  stoken_t my_stoken = get_my_stoken();

  int32_t sched_rc = 0;
  int32_t cc = 0;
  int32_t srb_rc = 0;
  int32_t srb_rsn = 0;

  sched_rc = sched_sync_srb((void *)srb_routine, NULL, my_stoken, NULL,
                            &cc, &srb_rc, &srb_rsn);

  printf("sched_rc=%d, cc=%d, srb_rc=0x%08x, srb_rsn=0x%08x\n",
         sched_rc, cc, srb_rc, srb_rsn);

  supervisorMode(FALSE);

  return 0;
}

char *getCAA(void) { return NULL; } // hack to avoid linking unnecessary code

#pragma map(srb_test_dsect, "SRBDSCT")
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

      "         CSECT ,                                                        \n"
      "         RMODE ANY                                                      \n"

  );

}
