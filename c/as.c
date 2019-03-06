

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
#include <metal/stdio.h>
#include <metal/string.h>
#else
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#endif

#include "zowetypes.h"
#include "alloc.h"
#include "as.h"

#include "utils.h"

#ifndef __ZOWE_OS_ZOS
#error z/OS targets are supported only
#endif

/*

xlc "-Wa,goff" \
"-Wc,LANGLVL(EXTC99),FLOAT(HEX),agg,exp,list(),so(),goff,xref,gonum,roconst,gonum,ASM,ASMLIB('SYS1.MACLIB'),LP64,XPLINK" \
-I ../h -o as as.c alloc.c utils.c timeutls.c

*/

int main() {

  int rc = 0, rsn = 0;


  return 0;
}


/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/

