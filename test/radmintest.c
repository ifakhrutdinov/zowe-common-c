#ifdef METTLE
#include <metal/metal.h>
#include <metal/stddef.h>
#include <metal/stdint.h>
#include <metal/stdio.h>
#include <metal/stdlib.h>
#include <metal/string.h>
#include "metalio.h"
#else
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

#include "zowetypes.h"
#include "alloc.h"
#include "zos.h"
#include "qsam.h"
#include "radmin.h"
#include "utils.h"

/*

LE 31-bit:

xlc "-Wa,goff" "-Wc,LANGLVL(EXTC99),FLOAT(HEX),agg,exp,list(),so(),goff,xref,\
gonum,roconst,gonum,ASM,ASMLIB('SYS1.MACLIB')" \
"-Wa,goff" '-Wl,ac=1' -I ../h  -o radmintest radmintest.c ../c/alloc.c \
../c/zos.c ../c/utils.c ../c/radmin.c ; extattr +a radmintest

Metal C 31-bit:

xlc -S -M -qmetal -DSUBPOOL=132 -DMETTLE=1 -DMSGPREFIX='"IDX"' \
-qreserved_reg=r12 -Wc,"arch(8),agg,exp,list(),so(),off,xref,roconst" \
-I ../h -o alloc.s ../c/alloc.c
as -mgoff -mobject -mflag=nocont --TERM --RENT -aegimrsx=alloc.asm \
-o alloc.o alloc.s

xlc -S -M -qmetal -DSUBPOOL=132 -DMETTLE=1 -DMSGPREFIX='"IDX"' \
-qreserved_reg=r12 -Wc,"arch(8),agg,exp,list(),so(),off,xref,roconst" \
-I ../h -o metalio.s ../c/metalio.c
as -mgoff -mobject -mflag=nocont --TERM --RENT -aegimrsx=metalio.asm \
-o metalio.o metalio.s

xlc -S -M -qmetal -DSUBPOOL=132 -DMETTLE=1 -DMSGPREFIX='"IDX"' \
-qreserved_reg=r12 -Wc,"arch(8),agg,exp,list(),so(),off,xref,roconst" \
-I ../h -o zos.s ../c/zos.c
as -mgoff -mobject -mflag=nocont --TERM --RENT -aegimrsx=zos.asm \
-o zos.o zos.s

xlc -S -M -qmetal -DSUBPOOL=132 -DMETTLE=1 -DMSGPREFIX='"IDX"' \
-qreserved_reg=r12 -Wc,"arch(8),agg,exp,list(),so(),off,xref,roconst" \
-I ../h -o qsam.s ../c/qsam.c
as -mgoff -mobject -mflag=nocont --TERM --RENT -aegimrsx=qsam.asm \
-o qsam.o qsam.s

xlc -S -M -qmetal -DSUBPOOL=132 -DMETTLE=1 -DMSGPREFIX='"IDX"' \
-qreserved_reg=r12 -Wc,"arch(8),agg,exp,list(),so(),off,xref,roconst" \
-I ../h -o radmin.s ../c/radmin.c
as -mgoff -mobject -mflag=nocont --TERM --RENT -aegimrsx=radmin.asm \
-o radmin.o radmin.s

xlc -S -M -qmetal -DSUBPOOL=132 -DMETTLE=1 -DMSGPREFIX='"IDX"' \
-qreserved_reg=r12 -Wc,"arch(8),agg,exp,list(),so(),off,xref,roconst" \
-I ../h -o utils.s ../c/utils.c
as -mgoff -mobject -mflag=nocont --TERM --RENT -aegimrsx=utils.asm \
-o utils.o utils.s

xlc -S -M -qmetal -DSUBPOOL=132 -DMETTLE=1 -DMSGPREFIX='"IDX"' \
-qreserved_reg=r12 -Wc,"arch(8),agg,exp,list(),so(),off,xref,roconst" \
-I ../h -o radmintest.s radmintest.c
as -mgoff -mobject -mflag=nocont --TERM --RENT -aegimrsx=radmintest.asm \
-o radmintest.o radmintest.s

export _LD_SYSLIB="//'SYS1.CSSLIB'"
ld -V -b ac=1 -b rent -b map -b xref -b reus -e main -o \
"//'DEV.LOADLIB(RDMNTEST)'" radmintest.o alloc.o metalio.o zos.o qsam.o \
utils.o radmin.o > radmintest.link

Metal C 64-bit:

xlc -S -M -qmetal -DSUBPOOL=132 -DMETTLE=1 -DMSGPREFIX='"IDX"' -q64 \
-qreserved_reg=r12 -Wc,"arch(8),agg,exp,list(),so(),off,xref,roconst,lp64" \
-I ../h -o alloc.s ../c/alloc.c
as -mgoff -mobject -mflag=nocont --TERM --RENT -aegimrsx=alloc.asm \
-o alloc.o alloc.s

xlc -S -M -qmetal -DSUBPOOL=132 -DMETTLE=1 -DMSGPREFIX='"IDX"' -q64 \
-qreserved_reg=r12 -Wc,"arch(8),agg,exp,list(),so(),off,xref,roconst,lp64" \
-I ../h -o metalio.s ../c/metalio.c
as -mgoff -mobject -mflag=nocont --TERM --RENT -aegimrsx=metalio.asm \
-o metalio.o metalio.s

xlc -S -M -qmetal -DSUBPOOL=132 -DMETTLE=1 -DMSGPREFIX='"IDX"' -q64 \
-qreserved_reg=r12 -Wc,"arch(8),agg,exp,list(),so(),off,xref,roconst,lp64" \
-I ../h -o zos.s ../c/zos.c
as -mgoff -mobject -mflag=nocont --TERM --RENT -aegimrsx=zos.asm \
-o zos.o zos.s

xlc -S -M -qmetal -DSUBPOOL=132 -DMETTLE=1 -DMSGPREFIX='"IDX"' -q64 \
-qreserved_reg=r12 -Wc,"arch(8),agg,exp,list(),so(),off,xref,roconst,lp64" \
-I ../h -o qsam.s ../c/qsam.c
as -mgoff -mobject -mflag=nocont --TERM --RENT -aegimrsx=qsam.asm \
-o qsam.o qsam.s

xlc -S -M -qmetal -DSUBPOOL=132 -DMETTLE=1 -DMSGPREFIX='"IDX"' -q64 \
-qreserved_reg=r12 -Wc,"arch(8),agg,exp,list(),so(),off,xref,roconst,lp64" \
-I ../h -o radmin.s ../c/radmin.c
as -mgoff -mobject -mflag=nocont --TERM --RENT -aegimrsx=radmin.asm \
-o radmin.o radmin.s

xlc -S -M -qmetal -DSUBPOOL=132 -DMETTLE=1 -DMSGPREFIX='"IDX"' -q64 \
-qreserved_reg=r12 -Wc,"arch(8),agg,exp,list(),so(),off,xref,roconst,lp64" \
-I ../h -o utils.s ../c/utils.c
as -mgoff -mobject -mflag=nocont --TERM --RENT -aegimrsx=utils.asm \
-o utils.o utils.s

xlc -S -M -qmetal -DSUBPOOL=132 -DMETTLE=1 -DMSGPREFIX='"IDX"' -q64 \
-qreserved_reg=r12 -Wc,"arch(8),agg,exp,list(),so(),off,xref,roconst,lp64" \
-I ../h -o radmintest.s radmintest.c
as -mgoff -mobject -mflag=nocont --TERM --RENT -aegimrsx=radmintest.asm \
-o radmintest.o radmintest.s

export _LD_SYSLIB="//'SYS1.CSSLIB'"
ld -V -b ac=1 -b rent -b map -b xref -b reus -e main -o \
"//'DEV.LOADLIB(RDMNTEST)'" radmintest.o alloc.o metalio.o zos.o qsam.o \
utils.o radmin.o > radmintest.link

*/

#ifndef METTLE
  #ifdef _LP64
  #error Metal C 31/64-bit and LE 31-bit are supported only
  #endif
#endif

#define RC_TEST_OK      0
#define RC_TEST_FAILED  8

static const char *getTarget() {
#ifdef METTLE
  #ifdef _LP64
  return "Metal C 64-bit";
  #else
  return "Metal C 31-bit";
  #endif
#else
  #ifdef _LP64
  return "LE 64-bit";
  #else
  return "LE 31-bit";
  #endif
#endif
}

static int testExtractProfileFunction() {

  printf("info: about to test R_admin extract profile function\n");

  RadminCallerAuthInfo authInfo = {0};
  RadminBasicUserPofileInfo result;
  size_t profilesExtracted = 0;
  RadminStatus status;

  const char *userID = "ZWESTC";
  const char *expectedUserID = "ZWESTC  ";
  const char *expectedGroup = "ZWEUSER ";
  const char *expectedName = "ZWE STARTED TASK    ";

  int extractRC = radminExtractBasicUserProfileInfo(
      authInfo,
      userID,
      1,
      &result,
      &profilesExtracted,
      &status
  );

  if (extractRC != RC_RADMIN_OK) {
    printf("error: non zero RC %d, status:\n", extractRC);
    dumpbuffer((char *)&status, sizeof(RadminStatus));
    return RC_TEST_FAILED;
  }

  if (memcmp(result.userID, expectedUserID, sizeof(result.userID)) ||
      memcmp(result.defaultGroup, expectedGroup, sizeof(result.defaultGroup)) ||
      memcmp(result.name, expectedName, sizeof(result.name))) {
    printf("info: unexpected results returned, count = %zu, dump:\n",
           profilesExtracted);
    dumpbuffer((char *)&result, sizeof(RadminBasicUserPofileInfo));
    return RC_TEST_FAILED;
  }

  printf("info: user profile extraction test completed successfully\n");

  return RC_TEST_OK;
}

#define RADMIN_COMMAND_BUFFER_SIZE  512

static int getOperatorCommand(RadminAPIStatus status,
                              const RadminCommandOutput *out,
                              void *userData) {

  char *radminCommand = userData;

  const RadminCommandOutput *nextOutput = out;
  while (nextOutput != NULL) {
    printf("info: R_admin output @ 0x%p\n", nextOutput);
    dumpbuffer((char *)nextOutput, nextOutput->totalLength);

    if (memcmp(nextOutput->eyecatcher, RADMIN_COMMAND_OUTPUT_EYECATCHER_CMD,
               sizeof(nextOutput->eyecatcher)) == 0) {
      memcpy(
          radminCommand,
          nextOutput->firstMessageEntry.text,
          min(nextOutput->firstMessageEntry.length,
              RADMIN_COMMAND_BUFFER_SIZE)
      );

    }

    nextOutput = nextOutput->next;
  }

  return 0;
}

static int testResourceFunction() {

  printf("info: about to test R_admin resource function\n");

  RadminCallerAuthInfo authInfo = {0};
  size_t profilesExtracted = 0;
  RadminStatus status = {0};

  char parmListBuffer[4096] = {0};

  RadminResParmList *parmList = (RadminResParmList *)&parmListBuffer;
  {
    parmList->classNameLength = 8;
    memcpy(parmList->className, "FACILITY", sizeof(parmList->className));
    parmList->flags |= RADMIN_RES_FLAG_NORUN;
    parmList->flags |= RADMIN_RES_FLAG_RETCMD;
    parmList->segmentCount = 1;
    RadminSegment *segment = parmList->segmentData;
    memcpy(segment->name, "BASE    ", sizeof(segment->name));
    segment->flag |= RADMIN_SEGMENT_FLAG_Y;
    segment->fieldCount = 2;
    RadminField *field1 = segment->firstField;
    memcpy(field1->name, "PROFILE ", sizeof(field1->name));
    field1->flag |= RADMIN_FIELD_FLAG_Y;
    field1->dataLength = 16;
    memcpy(field1->data, "ZOWE.E3270.RES01", field1->dataLength);
    RadminField *field2 =
        (RadminField *)((char *)field1 + sizeof(RadminField) +
            field1->dataLength);
    memcpy(field2->name, "UACC    ", sizeof(field2->name));
    field2->flag |= RADMIN_FIELD_FLAG_Y;
    field2->dataLength = 4;
    memcpy(field2->data, "NONE", field2->dataLength);
  }

  char operatorCommand[RADMIN_COMMAND_BUFFER_SIZE] = {0};

  int rc = radminPerformResAction(
      authInfo,
      RADMIN_RES_ACTION_ADD_GENRES,
      parmList,
      getOperatorCommand,
      operatorCommand,
      &status
  );

  if (rc != RC_RADMIN_OK) {
    printf("error: non zero RC %d, status:\n", rc);
    dumpbuffer((char *)&status, sizeof(RadminStatus));
    return RC_TEST_FAILED;
  }

  const char *expectedCommand = "RDEFINE FACILITY             "
                                "(ZOWE.E3270.RES01)  UACC        (NONE)";
  if (memcmp(operatorCommand, expectedCommand, strlen(expectedCommand)) != 0) {
    printf("error: unexpected R_admin output:\n");
    dumpbuffer(operatorCommand, sizeof(operatorCommand));
    return RC_TEST_FAILED;
  }

  printf("info: resource action test completed successfully\n");

  return RC_TEST_OK;
}

static int testGroupActionFunction() {

  printf("info: about to test R_admin group action function\n");

  RadminCallerAuthInfo authInfo = {0};
  size_t profilesExtracted = 0;
  RadminStatus status = {0};

  char parmListBuffer[4096] = {0};

  RadminGroupParmList *parmList = (RadminGroupParmList *)&parmListBuffer;
  {
    parmList->groupNameLength = 8;
    memcpy(parmList->groupName, "ZOWEUSER", sizeof(parmList->groupName));
    parmList->flags |= RADMIN_GROUP_FLAG_NORUN;
    parmList->flags |= RADMIN_GROUP_FLAG_RETCMD;
    parmList->segmentCount = 1;
    RadminSegment *segment = parmList->segmentData;
    memcpy(segment->name, "BASE    ", sizeof(segment->name));
    segment->flag |= RADMIN_SEGMENT_FLAG_Y;
    segment->fieldCount = 0;
  }

  char operatorCommand[RADMIN_COMMAND_BUFFER_SIZE] = {0};

  int rc = radminPerformGroupAction(
      authInfo,
      RADMIN_GROUP_ACTION_ADD_GROUP,
      parmList,
      getOperatorCommand,
      operatorCommand,
      &status
  );

  if (rc != RC_RADMIN_OK) {
    printf("error: non zero RC %d, status:\n", rc);
    dumpbuffer((char *)&status, sizeof(RadminStatus));
    return RC_TEST_FAILED;
  }

  const char *expectedCommand = "ADDGROUP ZOWEUSER ";
  if (memcmp(operatorCommand, expectedCommand, strlen(expectedCommand)) != 0) {
    printf("error: unexpected R_admin output:\n");
    dumpbuffer(operatorCommand, sizeof(operatorCommand));
    return RC_TEST_FAILED;
  }

  printf("info: group action test completed successfully\n");

  return RC_TEST_OK;
}

static int testConnectionActionFunction() {

  printf("info: about to test R_admin connection action function\n");

  RadminCallerAuthInfo authInfo = {0};
  size_t profilesExtracted = 0;
  RadminStatus status = {0};

  char parmListBuffer[4096] = {0};

  RadminConnectionParmList *parmList =
      (RadminConnectionParmList *)&parmListBuffer;
  {
    parmList->userIDLength = 8;
    memcpy(parmList->userID, "ZWEUSER ", sizeof(parmList->userID));
    parmList->flags |= RADMIN_CONNECTION_FLAG_NORUN;
    parmList->flags |= RADMIN_CONNECTION_FLAG_RETCMD;
    parmList->segmentCount = 1;
    RadminSegment *segment = parmList->segmentData;
    memcpy(segment->name, "BASE    ", sizeof(segment->name));
    segment->flag |= RADMIN_SEGMENT_FLAG_Y;
    segment->fieldCount = 2;
    RadminField *field1 = segment->firstField;
    memcpy(field1->name, "GROUP   ", sizeof(field1->name));
    field1->flag |= RADMIN_FIELD_FLAG_Y;
    field1->dataLength = 8;
    memcpy(field1->data, "ZOWETEST", field1->dataLength);
    RadminField *field2 =
        (RadminField *)((char *)field1 + sizeof(RadminField) +
            field1->dataLength);
    memcpy(field2->name, "AUTH    ", sizeof(field2->name));
    field2->flag |= RADMIN_FIELD_FLAG_Y;
    field2->dataLength = 3;
    memcpy(field2->data, "USE", field2->dataLength);

  }

  char operatorCommand[RADMIN_COMMAND_BUFFER_SIZE] = {0};

  int rc = radminPerformConnectionAction(
      authInfo,
      RADMIN_CONNECTION_ACTION_ADD_CONNECTION,
      parmList,
      getOperatorCommand,
      operatorCommand,
      &status
  );

  if (rc != RC_RADMIN_OK) {
    printf("error: non zero RC %d, status:\n", rc);
    dumpbuffer((char *)&status, sizeof(RadminStatus));
    return RC_TEST_FAILED;
  }

  const char *expectedCommand =
      "CONNECT ZWEUSER   GROUP       (ZOWETEST) AUTHORITY   (USE)";
  if (memcmp(operatorCommand, expectedCommand, strlen(expectedCommand)) != 0) {
    printf("error: unexpected R_admin output:\n");
    dumpbuffer(operatorCommand, sizeof(operatorCommand));
    return RC_TEST_FAILED;
  }

  printf("info: connection action test completed successfully\n");

  return RC_TEST_OK;
}

static int testGenresAccessListExtraction() {

  printf("info: about to test R_admin extract genres access list function\n");

  RadminCallerAuthInfo authInfo = {0};
  size_t profilesExtracted = 0;
  RadminStatus status;

  const char *profile = "ZWES.IS";
  const char *class = "FACILITY";
  RadminAccessListEntry result[512] = {0};
  size_t entriesExtracted = 0;

  int extractRC = radminExtractGenresAccessList(
      authInfo,
      class,
      profile,
      result,
      sizeof(result) / sizeof(result[0]),
      &entriesExtracted,
      &status
  );

  if (extractRC != RC_RADMIN_OK) {
    printf("error: non zero RC %d, status:\n", extractRC);
    dumpbuffer((char *)&status, sizeof(RadminStatus));
    return RC_TEST_FAILED;
  }

  printf("Profile \'%s\':\n", profile);
  for (int i = 0; i < entriesExtracted; i++) {
    printf("Entry  #%04d: %8.8s - %8.8s\n",
           i, result[i].id, result[i].accessType);
  }

  printf("info: genres access list extraction test completed successfully\n");

  return RC_TEST_OK;
}

static int testGenresExtraction() {

  printf("info: about to test R_admin extract genres profile function\n");

  RadminCallerAuthInfo authInfo = {0};
  size_t profilesExtracted = 0;
  RadminStatus status;

  const char *profile = NULL;
  const char *class = "FACILITY";
  RadminBasicGenresPofileInfo result[4096] = {0};
  size_t entriesExtracted = 0;

  int extractRC = radminExtractBasicGenresProfileInfo(
      authInfo,
      class,
      profile,
      sizeof(result) / sizeof(result[0]),
      result,
      &entriesExtracted,
      &status
  );

  if (extractRC != RC_RADMIN_OK) {
    printf("error: non zero RC %d, status:\n", extractRC);
    dumpbuffer((char *)&status, sizeof(RadminStatus));
    return RC_TEST_FAILED;
  }

  for (int i = 0; i < entriesExtracted; i++) {
    printf("Entry  #%04d: %.48s - %.8s\n",
           i, result[i].profile, result[i].owner);
  }

  printf("info: genres profile extraction test completed successfully\n");

  return RC_TEST_OK;
}

static int testGroupAccessListExtraction() {

  printf("info: about to test R_admin extract group access list function\n");

  RadminCallerAuthInfo authInfo = {0};
  size_t profilesExtracted = 0;
  RadminStatus status;

  const char *group = "ZWEUSER";
  RadminAccessListEntry result[512] = {0};
  size_t entriesExtracted = 0;

  int extractRC = radminExtractGroupAccessList(
      authInfo,
      group,
      result,
      sizeof(result) / sizeof(result[0]),
      &entriesExtracted,
      &status
  );

  if (extractRC != RC_RADMIN_OK) {
    printf("error: non zero RC %d, status:\n", extractRC);
    dumpbuffer((char *)&status, sizeof(RadminStatus));
    return RC_TEST_FAILED;
  }

  printf("Group \'%s\':\n", group);
  for (int i = 0; i < entriesExtracted; i++) {
    printf("Entry  #%04d: %8.8s - %8.8s\n",
           i, result[i].id, result[i].accessType);
  }

  printf("info: genres access list extraction test completed successfully\n");

  return RC_TEST_OK;
}

static int testGroupExtraction() {

  printf("info: about to test R_admin extract group profile function\n");

  RadminCallerAuthInfo authInfo = {0};
  size_t profilesExtracted = 0;
  RadminStatus status;

  const char *profile = NULL;
  RadminBasicGroupPofileInfo result[4096] = {0};
  size_t entriesExtracted = 0;

  int extractRC = radminExtractBasicGroupProfileInfo(
      authInfo,
      profile,
      sizeof(result) / sizeof(result[0]),
      result,
      &entriesExtracted,
      &status
  );

  if (extractRC != RC_RADMIN_OK) {
    printf("error: non zero RC %d, status:\n", extractRC);
    dumpbuffer((char *)&status, sizeof(RadminStatus));
    return RC_TEST_FAILED;
  }

  for (int i = 0; i < entriesExtracted; i++) {
    printf("Entry  #%04d: %.8s - %.8s - %.8s\n",
           i, result[i].group, result[i].owner, result[i].superiorGroup);
  }

  printf("info: gorup profile extraction test completed successfully\n");

  return RC_TEST_OK;
}

static int dumpCommand(RadminAPIStatus status,
                       const RadminCommandOutput *result,
                       void *userData) {

  dumpbuffer((char *)result, result->totalLength);

  return 0;
}

static int testRACFCommand() {

  printf("info: about to test R_admin RACF command function\n");

  RadminCallerAuthInfo authInfo = {0};
  RadminStatus status;

  int radminRC = radminRunRACFCommand(
      authInfo,
      "SETROPTS REFRESH RACLIST(XFACILIT)",
      dumpCommand,
      NULL,
      &status
  );

  if (radminRC != RC_RADMIN_OK) {
    printf("error: non zero RC %d, status:\n", radminRC);
    dumpbuffer((char *)&status, sizeof(RadminStatus));
    return RC_TEST_FAILED;
  }

  printf("info: R_admin RACF command test completed successfully\n");

  return RC_TEST_OK;
}


int main() {

  printf("info: hello from R_admin test (%s)\n", getTarget());

  supervisorMode(TRUE);

  int status = RC_TEST_OK;

  do {

    status = testExtractProfileFunction();
    if (status != RC_TEST_OK) {
      break;
    }

    status = testResourceFunction();
    if (status != RC_TEST_OK) {
      break;
    }

    status = testGroupActionFunction();
    if (status != RC_TEST_OK) {
      break;
    }

    status = testConnectionActionFunction();
    if (status != RC_TEST_OK) {
      break;
    }

    status = testGenresAccessListExtraction();
    if (status != RC_TEST_OK) {
      break;
    }

    status = testGenresExtraction();
    if (status != RC_TEST_OK) {
      break;
    }

    status = testGroupAccessListExtraction();
    if (status != RC_TEST_OK) {
      break;
    }

    status = testGroupExtraction();
    if (status != RC_TEST_OK) {
      break;
    }

    status = testRACFCommand();
    if (status != RC_TEST_OK) {
      break;
    }

  } while (0);

  supervisorMode(FALSE);

  if (status != RC_TEST_OK) {
    printf("error: R_admin tests status - FAILURE (%d)\n", status);
  } else {
    printf("info: R_admin tests status - SUCCESS (%d)\n", status);
  }

  return status;
}
