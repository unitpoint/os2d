/*
 * this is the internal transfer function.
 *
 * HISTORY
 * 14-Aug-06 File creation. Ported from Arm Thumb. Sylvain Baro
 *  3-Sep-06 Commented out saving of r1-r3 (r4 already commented out) as I
 *           read that these do not need to be saved.  Also added notes and
 *           errors related to the frame pointer. Richard Tew.
 *
 * NOTES
 *
 *   It is not possible to detect if fp is used or not, so the supplied
 *   switch function needs to support it, so that you can remove it if
 *   it does not apply to you.
 *
 * POSSIBLE ERRORS
 *
 *   "fp cannot be used in asm here"
 *
 *   - Try commenting out "fp" in REGS_TO_SAVE.
 *
 */

#define STACK_REFPLUS 1

#ifdef SLP_EVAL
#define STACK_MAGIC 0
#define REGS_TO_SAVE_GENERAL "r4", "r5", "r6", "r7", "r8", "r9", "lr"
#if defined(__SOFTFP__)
#define REGS_TO_SAVE REGS_TO_SAVE_GENERAL
#elif defined(__VFP_FP__)
#define REGS_TO_SAVE REGS_TO_SAVE_GENERAL, "d8", "d9", "d10", "d11", \
                                           "d12", "d13", "d14", "d15"
#elif defined(__MAVERICK__)
#define REGS_TO_SAVE REGS_TO_SAVE_GENERAL, "mvf4", "mvf5", "mvf6", "mvf7", \
                                           "mvf8", "mvf9", "mvf10", "mvf11", \
                                           "mvf12", "mvf13", "mvf14", "mvf15"
#else
#define REGS_TO_SAVE REGS_TO_SAVE_GENERAL, "f4", "f5", "f6", "f7"
#endif

static int
slp_switch(void)
{
        void *fp;
        register int *stackref, stsizediff;
        __asm__ volatile ("" : : : REGS_TO_SAVE);
        __asm__ volatile ("mov r0, fp\n\tstr r0, %0" : "=m" (fp) : : "r0");
        __asm__ ("mov %0,sp" : "=r" (stackref));
        {
                SLP_SAVE_STATE(stackref, stsizediff);
                __asm__ volatile (
                    "add sp,sp,%0\n"
                    "add fp,fp,%0\n"
                    :
                    : "r" (stsizediff)
                    );
                SLP_RESTORE_STATE();
        }
        __asm__ volatile ("ldr r0, %0\n\tmov fp, r0" : : "m" (fp) : "r0");
        __asm__ volatile ("" : : : REGS_TO_SAVE);
        return 0;
}

#endif