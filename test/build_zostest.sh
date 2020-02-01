#!/bin/sh

set -eu

xlc -D_OPEN_THREADS=1 "-Wa,goff" "-Wc,LANGLVL(EXTC99),FLOAT(HEX),agg,exp,list(),so(),goff,xref,gonum,roconst,gonum,ASM,ASMLIB('SYS1.MACLIB'),ASMLIB('CEE.SCEEMAC')" '-Wl,ac=1' \
-I ../h -o zostest31 zostest.c ../c/alloc.c ../c/timeutls.c ../c/utils.c ../c/zos.c

extattr +a zostest31




xlc -D_OPEN_THREADS=1 "-Wa,goff" "-Wc,LP64,XPLINK,LANGLVL(EXTC99),FLOAT(HEX),agg,exp,list(),so(),goff,xref,gonum,roconst,gonum,ASM,ASMLIB('SYS1.MACLIB'),ASMLIB('CEE.SCEEMAC')" '-Wl,ac=1' \
-I ../h -o zostest64 zostest.c ../c/alloc.c ../c/timeutls.c ../c/utils.c ../c/zos.c

extattr +a zostest64



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

xlc -S -M -qmetal -DSUBPOOL=132 -DMETTLE=1 -DMSGPREFIX='"IDX"' -qreserved_reg=r12 -Wc,"arch(8),agg,exp,list(),so(),off,xref,roconst,longname" -I h -I ../h -o zostest.s zostest.c
as -mgoff -mobject -mflag=nocont --TERM --RENT -aegimrsx=zostest.asm -o zostest.o zostest.s

export _LD_SYSLIB="//'SYS1.CSSLIB'://'CEE.SCEELKEX'://'CEE.SCEELKED'://'CEE.SCEERUN'://'CEE.SCEERUN2'://'CSF.SCSFMOD0'"
ld -V -b ac=1 -b rent -b case=mixed -b map -b xref -b reus -e main -o "//'$USER.DEV.LOADLIB(ZOSTST31)'" \
zostest.o alloc.o metalio.o qsam.o timeutls.o utils.o zos.o > zostest.link




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
ld -V -b ac=1 -b rent -b case=mixed -b map -b xref -b reus -e main -o "//'$USER.DEV.LOADLIB(ZOSTST64)'" \
zostest.o alloc.o metalio.o qsam.o timeutls.o utils.o zos.o > zostest.link

