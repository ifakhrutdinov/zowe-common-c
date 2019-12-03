#ifdef METTLE
#include <metal/metal.h>
#include <metal/stddef.h>
#include <metal/stdio.h>
#include <metal/stdlib.h>
#include <metal/string.h>
#include "metalio.h"
#else
#include "stddef.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#endif

/*
LE 31-bit:

xlc "-Wa,goff" "-Wc,LANGLVL(EXTC99),FLOAT(HEX),agg,exp,list(),so(),goff,xref,gonum,roconst,gonum,ASM,ASMLIB('SYS1.MACLIB'),ASMLIB('CEE.SCEEMAC')" '-Wl,ac=1' \
-I ../h -o isgenqtest isgenqtest.c ../c/isgenq.c

*/

#include "zowetypes.h"
#include "isgenq.h"

static void sleep(int seconds){
  int waitValue = seconds * 100;
  __asm(" STIMER WAIT,BINTVL=%0\n" : : "m"(waitValue));
}


static QName QNAME = {"ZWES    "};
static RName RNAME = {.length = 4, .value = "IREK"};

int main(int argc, char **argv) {

  if (argc < 2) {
    printf("error: bad parms, use {x, s}\n");
    return 8;
  }

  char mode = argv[1][0];
  if (mode != 'x' && mode != 's') {
    printf("error: bad parms, use {x, s}\n");
    return 8;
  }

  ENQToken token;

  int rc = 0, rsn = 0;

  if (mode == 'x') {

    rc = isgenqTestExclusiveLock(&QNAME, &RNAME, ISGENQ_SCOPE_SYSTEM, &rsn);
    printf("exclusive test rc = %d, rsn = %08X\n", rc, rsn);

    rc = isgenqTryExclusiveLock(&QNAME, &RNAME, ISGENQ_SCOPE_SYSTEM, &token, &rsn);
    printf("exclusive lock or fail rc = %d, rsn = %08X\n", rc, rsn);

    rc = isgenqGetExclusiveLock(&QNAME, &RNAME, ISGENQ_SCOPE_SYSTEM, &token, &rsn);
    printf("exclusive lock rc = %d, rsn = %08X\n", rc, rsn);

    sleep(10);

  } else {

    rc = isgenqTestSharedLock(&QNAME, &RNAME, ISGENQ_SCOPE_SYSTEM, &rsn);
    printf("shared test rc = %d, rsn = %08X\n", rc, rsn);

    rc = isgenqTrySharedLock(&QNAME, &RNAME, ISGENQ_SCOPE_SYSTEM, &token, &rsn);
    printf("shared lock or fail rc = %d, rsn = %08X\n", rc, rsn);

    rc = isgenqGetSharedLock(&QNAME, &RNAME, ISGENQ_SCOPE_SYSTEM, &token, &rsn);
    printf("shared lock rc = %d, rsn = %08X\n", rc, rsn);

    sleep(10);

  }

  rc = isgenqReleaseLock(&token, &rsn);
  printf("release rc = %d, rsn = %08X\n", rc, rsn);

  return 0;
}
