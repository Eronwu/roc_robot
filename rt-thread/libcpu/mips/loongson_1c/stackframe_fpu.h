/*
 * ls1c FPU's stackframe
 * �ʼ���룬��������뵽stackframe.h�е�SAVE_ALL, RESTORE_ALL��RESTORE_ALL_AND_RET�У�
 * �����ǵ�Դ�ļ�"stackframe.h"λ��Ŀ¼"libcpu\mips\common"�ڣ���Ӱ�쵽����mips cpu
 * ���ԣ������½���Դ�ļ�
 */
#ifndef __OPENLOONGSON_STACKFRAME_FPU_H
#define __OPENLOONGSON_STACKFRAME_FPU_H


#include "../common/asm.h"
#include "../common/mipsregs.h"
#include "../common/stackframe.h"



#define PT_FPU_R0               (0)
#define PT_FPU_R2               ((PT_FPU_R0) + 2*LONGSIZE)
#define PT_FPU_R4               ((PT_FPU_R2) + 2*LONGSIZE)
#define PT_FPU_R6               ((PT_FPU_R4) + 2*LONGSIZE)
#define PT_FPU_R8               ((PT_FPU_R6) + 2*LONGSIZE)
#define PT_FPU_R10              ((PT_FPU_R8) + 2*LONGSIZE)
#define PT_FPU_R12              ((PT_FPU_R10) + 2*LONGSIZE)
#define PT_FPU_R14              ((PT_FPU_R12) + 2*LONGSIZE)
#define PT_FPU_R16              ((PT_FPU_R14) + 2*LONGSIZE)
#define PT_FPU_R18              ((PT_FPU_R16) + 2*LONGSIZE)
#define PT_FPU_R20              ((PT_FPU_R18) + 2*LONGSIZE)
#define PT_FPU_R22              ((PT_FPU_R20) + 2*LONGSIZE)
#define PT_FPU_R24              ((PT_FPU_R22) + 2*LONGSIZE)
#define PT_FPU_R26              ((PT_FPU_R24) + 2*LONGSIZE)
#define PT_FPU_R28              ((PT_FPU_R26) + 2*LONGSIZE)
#define PT_FPU_R30              ((PT_FPU_R28) + 2*LONGSIZE)

#define PT_FPU_SIZE             ((((PT_FPU_R30) + 2*LONGSIZE) + (2*PTRSIZE-1)) & ~(2*PTRSIZE-1))


    .macro SAVE_FPU
    .set push
    .set noreorder
#ifdef RT_USING_FPU
    move k1, sp                     /* �����ֳ� */
    and k0, k1, 0xFFFFFFF8          /* 8�ֽڶ��� */    
    PTR_SUBU sp, k0, PT_FPU_SIZE    /* ����ջ�� */
    s.d $f0, PT_FPU_R0(sp)
    s.d $f2, PT_FPU_R2(sp)
    s.d $f4, PT_FPU_R4(sp)
    s.d $f6, PT_FPU_R6(sp)
    s.d $f8, PT_FPU_R8(sp)
    s.d $f10, PT_FPU_R10(sp)
    s.d $f12, PT_FPU_R12(sp)
    s.d $f14, PT_FPU_R14(sp)
    s.d $f16, PT_FPU_R16(sp)
    s.d $f18, PT_FPU_R18(sp)
    s.d $f20, PT_FPU_R20(sp)
    s.d $f22, PT_FPU_R22(sp)
    s.d $f24, PT_FPU_R24(sp)
    s.d $f26, PT_FPU_R26(sp)
    s.d $f28, PT_FPU_R28(sp)
    s.d $f30, PT_FPU_R30(sp)
    move sp, k1                     /* �ָ��ֳ� */
#endif
    .set reorder
    .set pop
    .endm


    .macro RESTORE_FPU
    .set push
    .set noreorder
#ifdef RT_USING_FPU
    move k1, sp                     /* �����ֳ� */
    and k0, k1, 0xFFFFFFF8          /* 8�ֽڶ��� */
    PTR_SUBU sp, k0, PT_FPU_SIZE    /* ����ջ��*/
    l.d $f0, PT_FPU_R0(sp)
    l.d $f2, PT_FPU_R2(sp)
    l.d $f4, PT_FPU_R4(sp)
    l.d $f6, PT_FPU_R6(sp)
    l.d $f8, PT_FPU_R8(sp)
    l.d $f10, PT_FPU_R10(sp)
    l.d $f12, PT_FPU_R12(sp)
    l.d $f14, PT_FPU_R14(sp)
    l.d $f16, PT_FPU_R16(sp)
    l.d $f18, PT_FPU_R18(sp)
    l.d $f20, PT_FPU_R20(sp)
    l.d $f22, PT_FPU_R22(sp)
    l.d $f24, PT_FPU_R24(sp)
    l.d $f26, PT_FPU_R26(sp)
    l.d $f28, PT_FPU_R28(sp)
    l.d $f30, PT_FPU_R30(sp)
    move sp, k1                     /* �ָ��ֳ� */
#endif
    .set reorder
    .set pop
    .endm


#endif

