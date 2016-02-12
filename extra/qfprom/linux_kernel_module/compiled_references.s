;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                 T R U S T Z O N E      M O N I T O R
;
;
; GENERAL DESCRIPTION
;   This file contains the TZ monitor code to switch from
;   Scorpion's non-secure world to secure world and vice versa.
;   The TZ monitor executes in Scorpion's Monitor processing
;   mode and can be entered via a SMC exception or interrupts.
;
;   This monitor is based on ARM Sample Monitor. The following
;   copyright statement from ARM also applies:
;     (C) COPYRIGHT 2005 ARM Limited
;          ALL RIGHTS RESERVED
;
; INITIALIZATION AND SEQUENCING REQUIREMENTS
;   tzbsp_init must be called to initialize the TZ
;   monitor before it can perform any world switch.
;
; Copyright (c) 2009-2012 by QUALCOMM Technologies, Incorporated.All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                           EDIT HISTORY FOR FILE
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
;
; $Header: 
; when       who     what, where, why
; --------   ---     --------------------------------------------------------
; 06/25/09   sl      Initial revision.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;============================================================================
;
;                            MODULE INCLUDES
;
;============================================================================
; ---------------------
; MACRO: Mov32
; ---------------------
; Moves a 32 bit value into a register
  MACRO
  Mov32 $r, $val_l
    movw    $r, #($val_l & 0xFFFF)
    movt    $r, #(($val_l >> 16) & 0xFFFF)
  MEND
; ---------------------
; MACRO: Read32R
; ---------------------
; Reads a given word where the address is in a register
  MACRO
  Read32R $val_r, $addr_r, $work_r
    ldr     $val_r, [$addr_r, $work_r]
  MEND
; ---------------------
; MACRO: Read32L
; ---------------------
; Reads a given word where the address is a literal
  MACRO
  Read32L $val_r, $addr_l, $work_r
    Mov32   $val_r, $addr_l
    Read32R $val_r, $val_r, $work_r
  MEND
; ---------------------
; MACRO: Write32R
; ---------------------
; Writes a given word where the address is in a register
  MACRO
  Write32R $val_r, $addr_r, $work_r
    str     $val_r, [$addr_r, $work_r]
  MEND
; ---------------------
; MACRO: Write32L
; ---------------------
; Writes a given word where the address is a literal
  MACRO
  Write32L $val_r, $addr_l, $work_r, $addr_r
    Mov32    $addr_r, $addr_l
    Write32R $val_r, $addr_r, $work_r
  MEND
   GET tzbsp_libarm_asm.inc
   GET tzbsp_asm.inc
   GET qctps_common_macros.inc
;============================================================================
;
;                             MODULE DEFINES
;
;============================================================================
;
; CPSR Processing Modes M[4:0] (CPSR[M]) bits
CPSR_MODE_USR               EQU     0x10
CPSR_MODE_FIQ               EQU     0x11
CPSR_MODE_IRQ               EQU     0x12
CPSR_MODE_SVC               EQU     0x13
CPSR_MODE_MON               EQU     0x16
CPSR_MODE_ABT               EQU     0x17
CPSR_MODE_UND               EQU     0x1b
CPSR_MODE_SYS               EQU     0x1f
CPSR_MODE_MASK              EQU     0x1f
; CPSR Thumb Instruction Set State Bit (CPSR[T])
CPSR_T_BIT                  EQU     0x20
; CPSR Interrupt Mask (CPSR[I], CPSR[I]) bits
CPSR_I_BIT                  EQU     0x80
CPSR_F_BIT                  EQU     0x40
; SCR (Secure Configuration Register) bits
SCR_NS_BIT                  EQU     0x01    ; Non-Secure (NS) bit
SCR_IRQ_BIT                 EQU     0x02    ; IRQ bit
SCR_FIQ_BIT                 EQU     0x04    ; FIQ bit
SCR_EA_BIT                  EQU     0x08    ; EA bit
SCR_FW_BIT                  EQU     0x10    ; F Bit writable (FW) bit
SCR_AW_BIT                  EQU     0x20    ; A Bit writable (AW) bit
; Distance between S and N contexts in the workspace.
Base_Diff                   EQU     Workspace_S_Base - Workspace_N_Base
Workspace_Diff              EQU     Workspace_N_Base - Workspace_N_Begin
; ----------------
; MACRO: SaveState
; ----------------
; Saves registers to the workspace r13 points into.
; Registers r0-r3 are not corrupted.
    MACRO
    SaveState
        stmdb   sp, {r0-r14}^
        mov     r4, sp
        cps     #CPSR_MODE_IRQ
        mrs     r12, SPSR
        stmia   r4!, {r12-r14}
        cps     #CPSR_MODE_SVC
        mrs     r12, SPSR
        stmia   r4!, {r12-r14}
        cps     #CPSR_MODE_ABT
        mrs     r12, SPSR
        stmia   r4!, {r12-r14}
        cps     #CPSR_MODE_UND
        mrs     r12, SPSR
        stmia   r4!, {r12-r14}
        cps     #CPSR_MODE_FIQ
        mrs     r7, SPSR
        stmia   r4, {r7-r14}
        cps     #CPSR_MODE_MON
    MEND
; ---------------------
; MACRO: SaveStateNoGp
; ---------------------
; Saves registers to the workspace r13 points into.
; Registers r0-r3 are not corrupted. General purpose
; registers are not saved.
    MACRO
    SaveStateNoGp
        mov     r4, sp
        cps     #CPSR_MODE_IRQ
        mrs     r12, SPSR
        stmia   r4!, {r12-r14}
        cps     #CPSR_MODE_SVC
        mrs     r12, SPSR
        stmia   r4!, {r12-r14}
        cps     #CPSR_MODE_ABT
        mrs     r12, SPSR
        stmia   r4!, {r12-r14}
        cps     #CPSR_MODE_UND
        mrs     r12, SPSR
        stmia   r4!, {r12-r14}
        cps     #CPSR_MODE_FIQ
        mrs     r7, SPSR
        stmia   r4, {r7-r14}
        cps     #CPSR_MODE_MON
    MEND
; ----------------
; MACRO: LoadState
; ----------------
; Loads registers from the workspace r13 points into.
    MACRO
    LoadState
        mov     r1, sp
        cps     #CPSR_MODE_IRQ
        ldmia   r1!, {r12-r14}
        msr     SPSR_fsxc, r12
        cps     #CPSR_MODE_SVC
        ldmia   r1!, {r12-r14}
        msr     SPSR_fsxc, r12
        cps     #CPSR_MODE_ABT
        ldmia   r1!, {r12-r14}
        msr     SPSR_fsxc, r12
        cps     #CPSR_MODE_UND
        ldmia   r1!, {r12-r14}
        msr     SPSR_fsxc, r12
        cps     #CPSR_MODE_FIQ
        ldmia   r1,  {r7-r14}
        msr     SPSR_fsxc, r7
        cps     #CPSR_MODE_MON
        ldmdb   sp, {r0-r14}^
    MEND
; ----------------
; MACRO: Save_NS_PC_CPSR
; ----------------
; Save r0 and r1 into N_PC and N_CPSR, respectively.
; Will set the T_BIT, if necessary
    MACRO
    Save_NS_PC_CPSR $pc, $cpsr, $scratch
        bic     $cpsr, $cpsr, #CPSR_T_BIT ; Clear the T bit if currently set
        ands    $scratch, $pc, #1         ; Test for THUMB mode
        beq     %f1
        orr     $cpsr, $cpsr, #CPSR_T_BIT ; New PC is THUMB code
1       ldr     $scratch, =N_PC
        stmia   $scratch, {r0-r1}
    MEND
; ----------------
; MACRO: SaveSCR
; ----------------
    MACRO
    SaveSCR $rx, $ry, $rz
        RCP15_SCR $rx
        mrc       p15, 0, $ry, c0, c0, 5  ; Current MPIDR.
        and       $ry, $ry, #0xFF         ; AFFL0 is the CPU number.
        ldr       $rz, =tzbsp_scr         ; cached SCR base address.
        str       $rx, [$rz, $ry, LSL #2] ; Write SCR for this CPU.
    MEND
; -----------------
; MACRO: RestoreSCR
; -----------------
    MACRO
    RestoreSCR $rx, $ry
        mrc       p15, 0, $ry, c0, c0, 5  ; Current MPIDR.
        and       $ry, $ry, #0xFF         ; AFFL0 is the CPU number.
        ldr       $rx, =tzbsp_scr         ; cached SCR base address.
        ldr       $rx, [$rx, $ry, LSL #2] ; Read SCR for this CPU.
        WCP15_SCR $rx
    MEND
; -------------------
; MACRO: SetSecure
; -------------------
    MACRO
    SetSecure $rx
        ; Set up SCR for Secure world.
        ; All FIQ, IRQ and EA are taken by monitor and not maskable by NS.
        mov       $rx, #SCR_EA_BIT:OR:SCR_IRQ_BIT:OR:SCR_FIQ_BIT
        WCP15_SCR $rx
        misb
    MEND
; -------------------
; MACRO: SetNonSecure
; -------------------
    MACRO
    SetNonSecure $rx
        ; Set up SCR for Normal world.
        ; FIQ - monitor, not maskable by NS.
        ; IRQ - NS, maskable by NS.
        ; EA  - NS, maskable by NS.
        mov       $rx, #SCR_AW_BIT:OR:SCR_NS_BIT:OR:SCR_FIQ_BIT
        WCP15_SCR $rx
        misb
    MEND
; -------------------------------
; MACRO: MonCallReturnToNonSecure
; -------------------------------
; All monitor calls return to non-secure mode.
;
; Arguments:
; rx: Used as a work register, set to zero.
    MACRO
    MonCallReturnToNonSecure $rx
        SetNonSecure    $rx
        mov             $rx, #0     ; Don't leak anything in this reg.
        movs            pc, lr      ; Return to the normal world.
    MEND
;============================================================================
;
;                             MODULE IMPORTS
;
;============================================================================
    IMPORT  tzbsp_loop_here
    IMPORT  tzbsp_graphics_dcvs_reset
    IMPORT  tzbsp_graphics_dcvs_busystats_update
    IMPORT  tzbsp_dcache_flush_region
    IMPORT  tzbsp_dcache_clean_region
    IMPORT  tzbsp_exec_power_collapse_notifier
    IMPORT  tzbsp_cpus_in_lpm
    IMPORT  tzbsp_do_l2_init
    IMPORT  g_tzbsp_term_entry_counters
    IMPORT  g_tzbsp_term_exit_counters
    IMPORT  tzbsp_dcvs_system_event
    IMPORT  g_DcvsReturnVal0
    IMPORT  g_DcvsReturnVal1
; Not compiled in by default.
;    IMPORT  tzbsp_save_debug_regs
;    IMPORT  tzbsp_clear_pc_debug_locks
    IMPORT  tzbsp_ns_wdt_isr
    IMPORT  tzbsp_l1_dump_request_state
    IMPORT  tzbsp_l1_dump
    IMPORT  tzbsp_l2_dump
    IMPORT  tzbsp_cpu_pc_state
    IMPORT  tzbsp_reset_vector
    IMPORT  g_tzbsp_ns_debug_vector
    IMPORT  g_tzbsp_ns_debug_stack
    IMPORT  g_tzbsp_ns_debug_ctx
    IMPORT  g_tzbsp_ns_debug_thumb
    IMPORT  tzbsp_wdt_mask_bark
    IMPORT  tzbsp_mmu_cache_disable
    IMPORT  tzbsp_mmu_cache_enable_no_cfg
    IMPORT  mmu_remove_tz_mapping
    IMPORT  sysini_wrapper
    IMPORT  tzbsp_was_gdhs_pc
    IMPORT  |Image$$TZBSP_STACK$$Base|
    IMPORT  |Image$$TZBSP_STACK$$Length|
    IMPORT  |Image$$TZBSP_RAM_RW$$Base|
    IMPORT  |Image$$TZBSP_RAM_RW$$Length|
    IMPORT  |Image$$TZBSP_RAM_ZI$$ZI$$Base|
    IMPORT  |Image$$TZBSP_RAM_ZI$$ZI$$Length|
;============================================================================
;
;                             MODULE EXPORTS
;
;============================================================================
    EXPORT  tzbsp_init_monitor
    EXPORT  tzbsp_smc_lock
    EXPORT  tzbsp_boot_lock
    EXPORT  tzbsp_install_irq_function
    EXPORT  tzbsp_install_fiq_function
    EXPORT  tzbsp_save_spbl_context
    EXPORT  tzbsp_hand_control_to_ns
    EXPORT  tzbsp_hand_control_to_ns_nfdbg
    EXPORT  tzbsp_is_non_secure
    EXPORT  Workspace_N_Begin
    EXPORT  Workspace_S_Begin
    EXPORT  Workspace_Mon_Begin
    EXPORT  tzbsp_irq
    EXPORT  N_CPSR
    EXPORT  S_MON_IP
    EXPORT  tzbsp_set_ns_return_addr
    EXPORT  tzbsp_set_cnthctl
    EXPORT  tzbsp_set_cntfrq
    EXPORT  tzbsp_smc_handler ; Makes the symbol appear to debugger.
    EXPORT  tzbsp_smc_handler_nfdbg ; Makes the symbol appear to debugger.
    EXPORT  tzbsp_mon_fiq_handler ; Makes the symbol appear to debugger.
    EXPORT  tzbsp_mon_enter_pc ; Makes the symbol appear to debugger.
    EXPORT  tzbsp_scr
    EXPORT  tzbsp_mon_configure_smc_for_nfdbg
    EXPORT  tzbsp_mon_configure_smc_for_normal
    EXPORT  tzbsp_mon_save_banked_gp_regs
    EXPORT  tzbsp_mon_save_banked_sp_lr_regs
    EXPORT  g_tzbsp_clk_mutex
    EXPORT  tzbsp_mmu_disable_and_configure_ns
    EXPORT  tzbsp_disable_ns_mmu_after_sbl
    PRESERVE8
    CODE32
;============================================================================
;
;                             MODULE CODE
;
;============================================================================
;===================================================================
; Monitor vector table
;===================================================================
;
; MVBAR always handles SMC and FIQ interrupts. IRQ interrupts are
; handled by monitor only when in secure mode. When in non-secure
; mode, IRQs are handled by non-secure VBAR.
;
; Because the device starts in secure SVC mode, the TZBSP vector
; table is defined as the first entry in .scl file. Monitor vector
; table can reside in any secure memory location with 2^5 byte
; alignment.
;
    AREA    TZBSP_MON_VECTOR_TABLE, CODE, READONLY, ALIGN=5
; Monitor vector table for normal operation.
tzbsp_mon_vector_table
    B       tzbsp_err_fatal         ; Not used (reset done in SVC mode)
    B       tzbsp_err_fatal         ; Not used (ARM constraint)
    B       tzbsp_smc_handler       ; SMC
    B       tzbsp_err_fatal         ; Prefetch abt, not handled.
    B       tzbsp_err_fatal         ; Data abt, not handled.
    B       tzbsp_err_fatal         ; Not used / reserved
    B       tzbsp_mon_irq_handler   ; IRQ
    B       tzbsp_mon_fiq_handler   ; FIQ
; Monitor vector table for non-secure debug FIQ use case. Vector tables
; are 32 bytes in size, the align directive is just a safeguard.
    ALIGN 32
tzbsp_mon_vector_table_nfdbg
    B       tzbsp_err_fatal         ; Not used (reset done in SVC mode)
    B       tzbsp_err_fatal         ; Not used (ARM constraint)
    B       tzbsp_smc_handler_nfdbg ; SMC
    B       tzbsp_err_fatal         ; Prefetch abt, not handled.
    B       tzbsp_err_fatal         ; Data abt, not handled.
    B       tzbsp_err_fatal         ; Not used / reserved
    B       tzbsp_err_fatal         ; IRQ can never happen during NFDBG.
    B       tzbsp_err_fatal         ; FIQ can never happen during NFDBG.
;===================================================================
; Handle fatal error
;===================================================================
tzbsp_err_fatal
    b       tzbsp_err_fatal
;===================================================================
; Secure IRQ handler
;===================================================================
;
; IRQs are trapped to monitor only when in secure side, hence we
; can assume that the current context is secure context.
;   - IRQ nesting is not supported during monitor IRQ handling.
;   - The handler runs in monitor mode, hence sp and lr are banked.
;   - On entry CPSR.I=1 (Interrupt disable),
;     CPSR.A=1 (Async abort disable) and CPSR.F=1 (FIQ Disable)
;   - Because only one core can be in secure side at any given time
;     only one core can get an IRQ in secure side. In case one CPU
;     is waiting for secure side access in its monitor, it won't get
;     IRQs because its CPSR.I=1.
;
; IRQs are handled by the non-secure side. The interrupted secure
; state will be saved and returning to the non-secure side will go
; through the SMC handlers secure-to-non-secure exit routine. After
; that the flow is as follows:
;
; 1. Non-secure side IRQ handler will be invoked. It will save the
;    context (which is what we loaded in SMC exit routine).
;
; 2. Non-secure side will handle the IRQ.
;
; 3. At some point the originally interrupted HLOS thread is
;    continued.
;
; 4. The execution continues in non-secure Secure Channel Manager
;    (SCM) which will see that the call to secure side has returned.
;    In reality, this was a side effect of IRQ handling and the secure
;    side is in interupted state and should be called again to
;    complete the service request.
;
; 5. Secure Channel Manager calls the secure side again which will
;    restore the interrupted secure context and continue.
;
; Access to secure side is serialized by the non-secure SCM. Once
; secure side has been exited to handle non-secure IRQ, access to TZBSP
; is unlocked. If a SMC instruction is issued, bypassing the
; non-secure SCM and indicating a compromized non-secure side, it is
; possible to enter TZBSP and continue in a context that didn't make the
; original service call. If this happens, then TZBSP just completes
; the service and returns to non-secure side but in the wrong context.
; The original call context will most probably fail in an
; unpredictable way in the non-secure side.
;
tzbsp_mon_irq_handler
    clrex
    push    {r0-r3, r12, lr}        ; Make work registers
    ; Mark that we have exited secure side via an IRQ.
    ldr     r0, =tzbsp_irq          ; Location of IRQ state
    mov     r1, #1
    str     r1, [r0]                ; Set IRQ state high
    ; Modify saved secure state to continue from the interrupted
    ; instruction whenever TZBSP is rescheduled again by non-secure
    ; side.
    ldr     r0, =S_PC               ; Currently saved secure PC.
    sub     r1, lr, #4              ; The next secure instruction.
    mrs     r2, spsr                ; Current SPSR.
    stmia   r0, {r1, r2}            ; Overwrite saved secure state.
    ; Check if we need to run TZBSP IRQ intercept function.
    ldr     r0, =tzbsp_irq_function ; Location of function addr.
    ldr     r0, [r0]                ; Value of function addr.
    cmp     r0, #0                  ; Check if function is set
    blxne   r0                      ; Call the IRQ interceptor
    pop     {r0-r3, r12, lr}        ; Restore original context.
    b       schedule_irq_exit       ; Run the TZ exit routine.
    LTORG                           ; Ensure the literal pool is
                                    ; close enough for the tzbsp_irq
                                    ; and S_PC LDR instructions
;===================================================================
; Secure FIQ handler
;===================================================================
;
; All FIQs are trapped to monitor mode, regardles of the processor
; security state when FIQ is triggered.
;   - FIQ nesting is not supported during monitor FIQ handling.
;   - The handler runs in monitor mode, hence sp and lr are banked.
;   - On entry CPSR.I=1, CPSR.A=1 and CPSR.F=1.
;
; Because FIQ handler is executed in monitor mode, the normal set of
; banked FIQ registers are not available.
;
tzbsp_mon_fiq_handler
    clrex
    sub     lr, lr, #4              ; Construct return address.
    stmfd   sp!, {lr}               ; Save mon_lr.
    mrs     lr, SPSR                ; Grab mon_spsr.
    stmfd   sp!, {lr}               ; Save mon_spsr.
    stmfd   sp!, {r0-r12}           ; Save r0-r12.
    SaveSCR   r0, r1, r2            ; Save the current secure state
    SetSecure r0                    ; Make sure we are in secure state
    ldr     r0, =tzbsp_fiq_function ; Location of FIQ handler ptr.
    ldr     r0, [r0]                ; Value of FIQ handler ptr.
    cmp     r0, #0                  ; Check that FIQ handler is set.
    ldreq   r0, =tzbsp_loop_here    ; Handler NULL, load err loop.
    bxeq    r0                      ; Branch to err loop.
    blx     r0                      ; Handler found, call it.
    RestoreSCR r0, r1               ; Restore the secure state
    ldmfd   sp!, {r0-r12}           ; Restore r0-r12
    ldmfd   sp!, {lr}               ; Grab saved mon_spsr.
    msr     SPSR_cxsf, lr           ; Restore mon_spsr.
    ldmfd   sp!, {pc}^              ; Return from FIQ to mon_lr.
;===================================================================
; Secure Monitor Call handler
;===================================================================
;
; SCM handler handles the entry from non-secure side to secure side and
; the exit in opposite direction.
;   - The handler runs in monitor mode, hence sp and lr are banked.
;   - On entry CPSR.I=1, CPSR.A=1 and CPSR.F=1.
;
; Only a single core can execute TZ code at any given time. Because IRQ
; is disabled by default when in SMC handler, single core pre-emptive
; scheduling is not possible. Simultaneous multicore entry is prevented
; by a spinlock that must be acquired before entering TZ.
;
; Any modification of TZ shared state without acquiring the lock is not
; allowed.
;
; The spinlock is a last line of defense, having to wait on the spinlock
; indicates abnormal execution flow. In normal flow HLOS Secure Channel
; Manager (SCM) contains a semaphore for serializing access to secure side.
; In other words, in the normal execution flow the entering core is
; expected to claim the lock without waiting.
;
tzbsp_smc_handler
    push    {r0-r3}             ; Make some work registers.
    ; Detect which world executed the SMC
    mrc     p15, 0, r0, c1, c1, 0
    tst     r0, #SCR_NS_BIT
    beq     schedule_secure_exit
; ---------------------
; N to S context switch
; ---------------------
schedule_secure_entry
    ; Set up SCR for Secure world.
    SetSecure r0
    ; Catch monitor calls that should be handled in monitor only. For
    ; all commands the bottom 12 bits can be ignored and should be
    ; ignored because for monitor calls we don't care about argument count
    ; or whether or not the register call bit is set. All monitor calls are
    ; register based.
    pop     {r0}        ; Get r0 argument from NS call.
    mov     r2, r0, LSR #12 ; Grab the command ID
    ; Monitor call --- Power Collapse.
    mov     r1, #0x0402
    cmp     r1, r2
    popeq   {r1-r3}     ; Unwind stack, we know we have PC entry.
    beq     tzbsp_mon_enter_pc
    ; Monitor call --- L0/L1/L2 cache dump
    mov     r1, #0x0C05
    cmp     r1, r2
    popeq   {r1-r3}     ; Unwind stack
    beq     tzbsp_mon_cache_dump
    ; Monitor call --- Krait errata #21 workaround enable.
    mov     r1, #0x0404
    cmp     r1, r2
    popeq   {r1-r3}     ; Unwind stack, we know we have I/O read.
    beq     tzbsp_mon_krait_errata_21_workaround_enable
    ; Monitor call --- I/O read cmd.
    mov     r1, #0x1401
    cmp     r1, r2
    popeq   {r1-r3}     ; Unwind stack, we know we have I/O read.
    beq     tzbsp_mon_io_read
    ; Monitor call --- I/O write cmd.
    mov     r1, #0x1402
    cmp     r1, r2
    popeq   {r1-r3}     ; Unwind stack, we know we have I/O write
    beq     tzbsp_mon_io_write
    ; Monitor call --- DCVS fast system event
    mov     r1, #0x3404
    cmp     r1, r2
    popeq   {r1-r3}     ; Unwind stack, we know we have I/O write
    beq     tzbsp_mon_dcvs_fast_system_event
    ; Monitor call --- DCVS reset
    mov     r1, #0x1403
    cmp     r1, r2
    popeq   {r1-r3}     ; Unwind stack
    beq     tzbsp_mon_dcvs_reset
    ; Monitor call --- DCVS update
    mov     r1, #0x1404
    cmp     r1, r2
    popeq   {r1-r3}     ; Unwind stack
    beq     tzbsp_mon_dcvs_update
    


    push    {r0}        ; Restore context and continue normal SMC flow.
    ; Wait until TZ is not busy.
    WaitForAccess r0, r1, r2, tzbsp_smc_lock
    ; Got the lock, TZ is now exclusively ours.
    ; Store r14_mon and spsr_mon.
    ldr     r0, =N_PC
    mov     r1, lr
    mrs     r2, spsr
    stmia   r0, {r1, r2}
    ; Always clear the IRQ exit status when entering TZ.
    ldr     r1, =tzbsp_irq
    mov     r0, #0
    str     r0, [r1]            ; Clear IRQ state.
    pop     {r0-r3}             ; Restore non-secure r0-r3.
    ; Save the non-secure context.
    mov     lr, sp              ; Save the monitor stack address.
    ldr     sp, =Workspace_N_Base ; Start address for non-secure context save.
    SaveState                   ; Save NS, won't corrupt r0-r3
    ; Restore the secure side context.
    ldr     sp, =Workspace_S_Base ; Start address for secure context save.
    LoadState                   ; Context = S-Context
    mov     sp, lr              ; Restore the monitor stack address.
    ldr     lr, =S_PC           ; Start location for secure PC and SPSR.
    rfeia   lr                  ; Return to Secure World code.


; ---------------------
; S to N context switch
; --------------------
schedule_secure_exit
    ; Store the secure world return address and SCPR.
    ldr     r0, =S_PC           ; Currently saved secure PC.
    mov     r1, lr
    mrs     r2, spsr            ; Current secure SPSR.
    stmia   r0, {r1, r2}        ; Overwrite saved secure state.
    pop     {r0-r3}             ; Restore secure state
schedule_irq_exit
    ; Save the secure context and load non-secure context.
    mov     lr, sp              ; Save monitor stack.
    ldr     sp, =Workspace_S_Base   ; The start of secure context
    SaveState                   ; S-Context = Context
    sub     sp, sp, #Base_Diff  ; r13_mon = Ptr to N-Workspace
    LoadState                   ; Context = N-Context
    mov     sp, lr              ; Restore monitor stack.
    ; Secure side calls are supposed to made only by non-secure
    ; side Secure Channel Manager. The call to secure side returns
    ; r0-r1 from non-secure perspective. This means we can corrupt
    ; non-secure registers r2-r3.
    ; If we exited secure state via IRQ, then set r0 to a special return
    ; value one.
    ldr     r3, =tzbsp_irq
    ldr     r2, [r3]
    cmp     r2, #0              ; If non-zero, should do IRQ return.
    movne   r0, #1              ; Indicate IRQ exit to NS side.
    ldr     lr, =N_PC           ; Currently saved non-secure PC.
    ldr     lr, [lr]
    ldr     r3, =N_CPSR         ; Currently saved non-secure CPSR.
    ldr     r3, [r3]
    bic     r3, r3, #(1<<6)     ; Ensure the SPSR.F bit is zero
    msr     SPSR_fsxc, r3
    ; Clear the lock.
    ReleaseAccess r2, r3, tzbsp_smc_lock
    ; At this point any other core can enter TZBSP.
    mov     r2, #0
    mov     r3, #0
    MonCallReturnToNonSecure r3 ; Return to the normal world
    ; Assembling a literal pool close to SMC handler. Monitor is no large
    ; enough that without a literal pool SMC handler cannot generate all
    ; the variable addresses it needs.
    LTORG
;===================================================================
; Monitor Power Collapse Termination Handler
;===================================================================
; Handler for entering power collapse state. Monitor stack is unwound when this
; handler is invoked. Value zero is returned if entering power collapse failed.
;
; Arguments:
; r0: Command ID, can be corrupted.
; r1: Token, can be corrupted.
; r2: 0 - No L2 GDHS nor L2 power collapse.
;     1 - L2 is about to be power collapsed.
;     2 - Reserved (was used for CPU retention flag on a-family).
;     3 - L2 is about to be put to GDHS retention state.
tzbsp_mon_enter_pc
    ; Note: Should do any secure, per-cpu state saving here.
    push    {r2, lr}
    WaitForAccess r0, r1, r2, tzbsp_boot_lock
    ; Update CPU power collapse state
    mrc     p15, 0, r0, c0, c0, 5       ; Current MPIDR.
    and     r0, r0, #0xFF               ; AFFL0 is the CPU number.
    ldr     r1, =tzbsp_cpu_pc_state     ; Get current CPU state
    ldrb    r2, [r1, r0]                ;  for given CPU
    sub     r2, r2, #1                  ; Move state back to pwrdwn
    strb    r2, [r1, r0]                ; Save current CPU state
    ; Increment the number of CPUs in low power mode.
    ldr     r0, =tzbsp_cpus_in_lpm
    ldr     r1, [r0]
    add     r1, r1, #1
    str     r1, [r0]
    ; Disable the secure WDOG bark before sleeping to prevent
    ; unnecessary wake-ups. External C function, so must
    ; ensure 8-byte stack alignment (i.e. before popping r2)
    mov     r0, #1 ; mask the bark interrupt
    blx     tzbsp_wdt_mask_bark
    pop     {r2}                    ; Restore the L2 input arg
    ; Determine the PC type indicated by HLOS.
    cmp     r2, #1                  ; PC with L2 PC.
    beq     with_l2_pc
    cmp     r2, #3                  ; PC with L2 GDHS.
    beq     with_l2_gdhs
    b       pc_common               ; Default to #0 style handling.
with_l2_pc
    ldr     r0, =tzbsp_do_l2_init
    ldr     r1, [r0]
    orr     r2, r1, r2              ; don't erase other cpu l2 init vote
    str     r2, [r0]
    b       pc_common
with_l2_gdhs
    mov     r2, #1
    ldr     r0, =tzbsp_was_gdhs_pc
    ldr     r1, [r0]
    orr     r2, r1, r2              ; Don't erase other CPU GDHS vote.
    str     r2, [r0]
pc_common
    ldr     r0, =tzbsp_cpus_in_lpm
    ldr     r0, [r0]
    cmp     r0, #4    ; Are all CPUs down?
    bne     skip_cm                 ; Not all CPUs down, skip CM
    ldr     r0, =tzbsp_do_l2_init
    ldr     r0, [r0]
    cmp     r0, #1                  ; was the L2 flag set?
    bne     skip_cm                 ; L2 flag was not set, skip CM
do_cm
    mov     r1, #0                  ; Mark L2 invalid for SDI
    Mov32   r0, ((0xFE805000 + 0x734) + (0x2C))
    str     r1, [r0]
    push    {r12} ; For 8-byte stack alignment
    blx     tzbsp_exec_power_collapse_notifier ; Notify executive
    pop     {r12}                              ; client(s) of power
                                               ; collapse so they can
                                               ; do cache maintenance
    bl      tzbsp_flush_all_tz_regions ; Flush secure cache lines before
                                       ; updating lock.  Lock must be last.
    ; intentional fallthrough
skip_cm
    ReleaseAccess r0, r1, tzbsp_boot_lock
    ; intentional fallthrough
    ; Update the PC termination entry counter. Diagnostics area is non-cached.
    mrc     p15, 0, r0, c0, c0, 5            ; Current MPIDR.
    and     r0, r0, #0xFF                    ; AFFL0 is the CPU number.
    ldr     r1, =g_tzbsp_term_entry_counters ; Base of entry counter addresses.
    ldr     r1, [r1, r0, LSL #2]             ; Read the counter address.
    ldr     r2, [r1]                         ; Read the stored counter value.
    add     r2, r2, #1                       ; Increment the counter.
    str     r2, [r1]                         ; Write the new counter value.
    ; Save the debug registers for CPU before PC
    ; Note: Cannot debug with external debugger after this point
;    bl      tzbsp_save_debug_regs
    ; DSB is not needed before WFI on Krait.
    wfi
    ; Release debug locks that were supposed to be held across PC
    ; Note: This restores access to external debugger
;    bl      tzbsp_clear_pc_debug_locks
    ; Update the PC termination exit counter. Diagnostics area is non-cached.
    mrc     p15, 0, r0, c0, c0, 5           ; Current MPIDR.
    and     r0, r0, #0xFF                   ; AFFL0 is the CPU number.
    ldr     r1, =g_tzbsp_term_exit_counters ; Base of exit counter addresses.
    ldr     r1, [r1, r0, LSL #2]            ; Read the counter address.
    ldr     r2, [r1]                        ; Read the stored counter value.
    add     r2, r2, #1                      ; Increment the counter.
    str     r2, [r1]                        ; Write the new counter value.
    WaitForAccess r0, r1, r2, tzbsp_boot_lock
    ; Decrement the number of CPUs in low power mode.
    ldr     r0, =tzbsp_cpus_in_lpm
    ldr     r1, [r0]
    sub     r1, r1, #1
    str     r1, [r0]
    ; Clear the L2 counter in any case.
    ldr     r0, =tzbsp_do_l2_init
    mov     r1, #0
    str     r1, [r0]
    ; Clear the GDHS state as well.
    ldr     r0, =tzbsp_was_gdhs_pc
    mov     r1, #0
    str     r1, [r0]
    ; Variable tzbsp_was_l2_pc is only updated in warm boot -> no need to
    ; clear that here.
    ; Update CPU power collapse state
    mrc     p15, 0, r0, c0, c0, 5       ; Current MPIDR.
    and     r0, r0, #0xFF               ; AFFL0 is the CPU number.
    ldr     r1, =tzbsp_cpu_pc_state     ; Get current CPU state
    ldrb    r2, [r1, r0]                ;  for given CPU
    add     r2, r2, #1                  ; Move state back to pwrup
    strb    r2, [r1, r0]                ; Save current CPU state
    ; Power collapse didn't happen - L2 remains valid from SDI perspective.
    mov     r1, #1
    Mov32   r0, ((0xFE805000 + 0x734) + (0x2C))
    str     r1, [r0]
    ; Re-enable the secure WDOG bark; external C function, so must
    ; ensure 8-byte stack alignment
    push    {r0}
    mov     r0, #0 ; un-mask the bark interrupt
    blx     tzbsp_wdt_mask_bark
    pop     {r0}
    ReleaseAccess r0, r1, tzbsp_boot_lock
    pop     {lr}
    ; Entry to low power state failed, e.g. an interrupt was pending.
    ; Will have to return back to non-secure world.
    mov     r1, #0
    mov     r2, #0
    MonCallReturnToNonSecure r3 ; Return to the non-secure world.
;===================================================================
; Cache coherency handlers
;===================================================================
; Flush areas of memory that belong to TZ that are cacheable.  This
; must be called before exiting the secure side after any stores to
; cacheable memory.  Stack and monitor stack do not need to be flushed
; because stacks should be empty upon normal exit/entry from/to TZ.
; This function is not appropriate for IRQ exits.
;
tzbsp_flush_all_tz_regions
    push    {lr}
    ldr     r0, =|Image$$TZBSP_STACK$$Base|
    ldr     r1, =|Image$$TZBSP_STACK$$Length|
    bl      tzbsp_dcache_flush_region
    ldr     r0, =|Image$$TZBSP_RAM_RW$$Base|
    ldr     r1, =|Image$$TZBSP_RAM_RW$$Length|
    bl      tzbsp_dcache_flush_region
    ldr     r0, =|Image$$TZBSP_RAM_ZI$$ZI$$Base|
    ldr     r1, =|Image$$TZBSP_RAM_ZI$$ZI$$Length|
    bl      tzbsp_dcache_flush_region
    ldmfd   sp!, {pc}
;===================================================================
; Monitor I/O read handler
;===================================================================
; Handler for monitor mode I/O read. Monitor stack is unwound when this handler
; is invoked. Value zero is returned in r0 if the access is illegal.
;
; Arguments:
; r0: Command ID, can be corrupted.
; r1: Token, can be corrupted.
; r2: Address to be read.
; r3: Not used, can be corrupted.
tzbsp_mon_io_read
    mov     r0, #(-4)               ; -E_NOT_SUPPORTED
    mov     r1, #0
    mov     r2, #0
    MonCallReturnToNonSecure r3     ; Switch back to non-secure side.
;===================================================================
; Monitor I/O write handler
;===================================================================
; Handler for monitor mode I/O write. Monitor stack is unwound when this
; handler is invoked.
;
; Arguments:
; r0: Command ID, can be corrupted.
; r1: Token, can be corrupted.
; r2: Address to be written.
; r3: Value to be written.
tzbsp_mon_io_write
    mov     r0, #(-4)               ; -E_NOT_SUPPORTED
    mov     r1, #0
    mov     r2, #0
    MonCallReturnToNonSecure r3     ; Switch back to non-secure side.
;===================================================================
; Monitor DCVS reset handler
;===================================================================
; Arguments:
; r0: Command ID, can be corrupted.
; r1: Token, can be corrupted.
tzbsp_mon_dcvs_reset
    push    {r12, lr}
    blx     tzbsp_graphics_dcvs_reset
    mov     r0, #0
    mov     r1, #0
    mov     r2, #0
    pop     {r12, lr}
    MonCallReturnToNonSecure r3     ; Switch back to non-secure side.
;===================================================================
; Monitor DCVS update handler
;===================================================================
; Arguments:
; r0: Command ID, can be corrupted.
; r1: Token, can be corrupted.
; r2: Argument for update.
tzbsp_mon_dcvs_update
    push    {r12, lr}
    mov     r0, r2
    mov     r1, r3
    mov     r2, r4
    blx     tzbsp_graphics_dcvs_busystats_update
    ; R0 contains the update return value
    mov     r1, #0
    mov     r2, #0
    pop     {r12, lr}
    MonCallReturnToNonSecure r3     ; Switch back to non-secure side.
tzbsp_mon_dcvs_fast_system_event
    push    {r12, lr}
    ; Shuffle the incoming arguments to conform to the standard C call
    ; style.
    mov     r0, r2      ; core_id
    mov     r1, r3      ; event_id
    mov     r2, r4      ; param0
    mov     r3, r5      ; param1
    blx tzbsp_dcvs_system_event
    ; R0 contains the update return value
    ; R1 contains the first additional return value.
    ldr     r1, =g_DcvsReturnVal0
    ldr     r1, [r1]
    ; R2 contains the second additional return value.
    ldr     r2, =g_DcvsReturnVal1
    ldr     r2, [r2]
    pop     {r12, lr}
    MonCallReturnToNonSecure r3     ; Switch back to non-secure side.
;===================================================================
; Monitor cache dump handler
;===================================================================
; Handler for monitor mode cache dumping. Monitor stack is unwound when this
; handler is invoked.
;
; Arguments:
; r0: Command ID, can be corrupted.
; r1: Token, can be corrupted.
; r2: 0, handle L1 dump request similar to WDT bark, resulting in L0/L1 cache
;        dump, CPU context dumps and device reset.
;     1, only dump L1 cache and return to HLOS after the dump is done.
;     2, only dump L2 cache and return to HLOS after the dump is done.
;     Any other value for r2 will return -4 (-E_NOT_SUPPORTED).
tzbsp_mon_cache_dump
    cmp     r2, #0          ; If r2=0,
    beq     %f4             ;   then handle similar to WDT bark.
    cmp     r2, #1          ; If r2=1,
    beq     %f1             ;   then just do L0/L1 dump and return.
    cmp     r2, #2          ; If r2=2,
    beq     %f2             ;   then just do L2 dump and return.
    mov     r0, #(-4)       ; Else set -E_NOT_SUPPORTED...
    b       %f3             ;   ...and return.
    ; L1 cache dumping.
1   push    {r12, lr}
    blx     tzbsp_l1_dump   ; Jump to L1 dump handler.
    pop     {r12, lr}
    mov     r0, #0          ; Set E_SUCCESS return value.
    b       %f3             ; Return to non-secure side.
    ; L2 cache dumping.
2   push    {r12, lr}
    blx     tzbsp_l2_dump   ; Jump to L2 dump handler.
    pop     {r12, lr}
    mov     r0, #0          ; Set E_SUCCESS return value.
    ; Continue to local label 3; return to non-secure side.
    ; Exit sequence.
3   ; This local label is on a separate line due to the macro below.
    mov     r1, #0
    mov     r2, #0
    MonCallReturnToNonSecure r3     ; Switch back to non-secure side.
    ; L1 dumping similar to WDT bark never returns and causes device reset.
    ; Pushing of stack registers emulates FIQ handler. The stack layout must be
    ; identical to FIQ handler for the context dumping.
4   stmfd   sp!, {lr}                   ; Save mon_lr.
    mrs     lr, SPSR                    ; Grab mon_spsr.
    stmfd   sp!, {lr}                   ; Save mon_spsr.
    stmfd   sp!, {r0-r12}               ; Save r0-r12.
    ; Indicate that the L1 dump is caused by L1 request from kernel.
    mov     r0, #1
    ldr     r1, =tzbsp_l1_dump_request_state
    str     r0, [r1]
    ; Modify the saved SCR to indicate the "interrupted" context is non-secure.
    ; As we are entering WDT bark handling, FIQs are disabled and the call
    ; never returns, hence stomping the saved SCR is ok.
    mrc     p15, 0, r0, c0, c0, 5   ; Current MPIDR.
    and     r0, r0, #0xFF           ; AFFL0 is the CPU number.
    ldr     r1, =tzbsp_scr          ; Saved SCR base address.
    ldr     r2, [r1, r0, LSL #2]    ; Read the saved SCR.
    orr     r2, r2, #1              ; Ensure NS bit is set.
    str     r2, [r1, r0, LSL #2]    ; Write the saved SCR.
    ; Jump directly to WDT bark handler. Note that WDT bark related SGI is
    ; always registered by TZBSP. This means other CPUs will be able to receive
    ; and handle the WDT bark SGI even if HLOS has not registered WDT0 as FIQ.
    mov     r0, #0                  ; Pass NULL to WDT bark ISR.
    ldr     r1, =tzbsp_ns_wdt_isr   ; Location of WDT bark ISR.
    blx     r1                      ; Jump to WDT bark ISR.
    ; WDT bark ISR will never return.
;===================================================================
; Monitor Krait errata #21 workaround enable handler
;===================================================================
; Handler for applying the Krait errata #21 workaround.
;
; Arguments:
; r0: Command ID, can be corrupted.
; r1: Token, can be corrupted.
tzbsp_mon_krait_errata_21_workaround_enable
    ; This workaround command is no longer needed, but kept for backward
    ; compatibility.
    mov     r0, #0
    mov     r1, #0
    mov     r2, #0
    MonCallReturnToNonSecure r3     ; Switch back to non-secure side.
;===================================================================
; FUNCTION: void tzbsp_save_spbl_context( uin32 lr_nwd, uint32 sp_nwd )
;===================================================================
;
; Saves the SPBL state to monitor's NS context. SPBL is now known as
; SBL2.
;
; TZ initialization is called as a function from SPBL. TZ is entered
; from SPBL in secure mode and will return to SPBL in non-secure mode.
;
; This implies the following:
;   - Registers r0-r3 can be corrupted in TZ.
;   - SPBL context must be saved and NS banked registers must be
;     configured so that exiting from the secure side to SPBL emulates
;     a function return.
;
; We need to keep/configure the following SPBL state:
;   1. SPBL sp  : Written to NS saved context.
;   2. SPBL lr  : Written to NS saved context.
;
; Parameters
;   r0 - Entry ptr of the Normal World
;   r1 - Stack ptr of the Normal World
;
tzbsp_save_spbl_context FUNCTION
    msr     CPSR_c, #CPSR_MODE_MON:OR:CPSR_I_BIT:OR:CPSR_F_BIT
    ; ------------------------------------------------------------------
    ; Clear the whole workspace. Other values to be written on top.
    ; ------------------------------------------------------------------
    ldr     r2, =MonitorWorkspaceBase
1   mov     r3, #0x0
    str     r3, [r2],#4
    ldr     r3, =MonitorWorkspaceEnd
    cmp     r2, r3
    bne     %b1
    ; ------------------------------------------------------------------
    ; Build up the Non secure context by using the secure world context.
    ; ------------------------------------------------------------------
    ; Save the non-secure stack
    ldr     r2, =N_USR_R13
    str     r1, [r2]
    ldr     r2, =S_SVC_R13
    str     r1, [r2]
    ldr     r13, =Workspace_N_Base
    SaveState
    ; Store the Normal World "reset" PC and CPSR, checking if the
    ; PC is ARM or THUMB code.
    mov     r1, #CPSR_MODE_SVC:OR:CPSR_I_BIT
    Save_NS_PC_CPSR r0, r1, r2
    ; Pass through the SBL2 VBAR address.
    ldr     r1, =tzbsp_ns_vbar
    mrc     p15, 0, r0, c12, c0, 0  ; Current VBAR
    str     r0, [r1]
    msr     CPSR_c, #CPSR_MODE_SVC:OR:CPSR_I_BIT:OR:CPSR_F_BIT
    bx      lr
    ENDFUNC
;===================================================================
; FUNCTION: void tzbsp_hand_control_to_ns( uin32 lr_nwd )
;===================================================================
;
; Hands control the the non-secure side.
;
; Parameters
;   r0 - Entry ptr of the Normal World
;
tzbsp_hand_control_to_ns FUNCTION
    ; Change to monitor mode
    msr     CPSR_c, #CPSR_MODE_MON:OR:CPSR_I_BIT:OR:CPSR_F_BIT
    mov     r4, r0                  ; Save the normal world
                                    ; jump address in r4
    ; Unlock the boot flow for other CPUs.
    ReleaseAccess r1, r2, tzbsp_boot_lock
    ; After lock gets to coherency point any other core can boot
    mov     lr, r4                  ; Restore LR, normal world jump addr
    ; Clear the entire register file. For each mode SPSR is left as it is.
    mov     r1,  #0
    mov     r2,  #0
    mov     r3,  #0
    mov     r4,  #0
    mov     r5,  #0
    mov     r6,  #0
    mov     r7,  #0
    mov     r8,  #0
    mov     r9,  #0
    mov     r10, #0
    mov     r11, #0
    mov     r12, #0
    cps     #CPSR_MODE_SYS
    mov     r13, #0
    mov     r14, #0
    cps     #CPSR_MODE_IRQ
    mov     r13, #0
    mov     r14, #0
    cps     #CPSR_MODE_SVC
    mov     r13, #0
    mov     r14, #0
    cps     #CPSR_MODE_ABT
    mov     r13, #0
    mov     r14, #0
    cps     #CPSR_MODE_UND
    mov     r13, #0
    mov     r14, #0
    cps     #CPSR_MODE_FIQ
    mov     r8,  #0
    mov     r9,  #0
    mov     r10, #0
    mov     r11, #0
    mov     r12, #0
    mov     r13, #0
    mov     r14, #0
    ; Switch back to monitor mode
    cps     #CPSR_MODE_MON
    ; Store the Normal World "reset" PC and CPSR.
    msr     SPSR_cfsx, #CPSR_MODE_SVC:OR:CPSR_I_BIT
    ; Switch to non-secure state. Set up SCR for Normal world.
    SetNonSecure r0
    mov     r0, #0
    mov     r1, #0
    ; Jump to the non-secure side, will never return.
    movs    pc, lr
    ENDFUNC
;===================================================================
; FUNCTION: void tzbsp_mon_configure_smc_for_nfdbg(void)
;===================================================================
; Configures the monitor vector table for the non-secure debug FIQ
; use case. Must be called in monitor mode.
tzbsp_mon_configure_smc_for_nfdbg FUNCTION
    ldr     r0, =tzbsp_mon_vector_table_nfdbg
    mcr     p15, 0, r0, c12, c0, 1              ; MVBAR
    isb
    bx      lr
    ENDFUNC
;===================================================================
; FUNCTION: void tzbsp_mon_configure_smc_for_normal(void)
;===================================================================
; Configures the monitor vector table for normal TZ operation.
; Must be called in monitor mode.
tzbsp_mon_configure_smc_for_normal FUNCTION
    ldr     r0, =tzbsp_mon_vector_table
    mcr     p15, 0, r0, c12, c0, 1              ; MVBAR
    isb
    bx      lr
    ENDFUNC
;===================================================================
; FUNCTION: void tzbsp_mon_save_banked_gp_regs(uint32* buf)
;===================================================================
; Saves the banked general purpose registers to the provided buffer.
; Can only be called in monitor mode.
;
; Parameters
;   r0 - Start address of the save buffer.
tzbsp_mon_save_banked_gp_regs FUNCTION
    stmia   r0, {r13-r14}^
    add     r0, #8
    cps     #CPSR_MODE_IRQ
    mrs     r1, SPSR
    stmia   r0!, {r1, r13-r14}
    cps     #CPSR_MODE_SVC
    mrs     r1, SPSR
    stmia   r0!, {r1, r13-r14}
    cps     #CPSR_MODE_ABT
    mrs     r1, SPSR
    stmia   r0!, {r1, r13-r14}
    cps     #CPSR_MODE_UND
    mrs     r1, SPSR
    stmia   r0!, {r1, r13-r14}
    cps     #CPSR_MODE_FIQ
    mrs     r1, SPSR
    stmia   r0!, {r1, r8-r14}
    ; Switch back to monitor mode
    msr     CPSR_c, #CPSR_MODE_MON:OR:CPSR_I_BIT:OR:CPSR_F_BIT
    bx      lr
    ENDFUNC
;===================================================================
; FUNCTION: void tzbsp_mon_save_banked_sp_lr_regs(uint32* buf)
;===================================================================
; Saves the banked sp and lr registers of the interrupted process to
; the provided buffer.
; Can only be called in monitor mode.
;
; Parameters
;   r0 - Start address of the save buffer.
tzbsp_mon_save_banked_sp_lr_regs FUNCTION
    push    {r1-r2}
    mrs     r1, SPSR                    ; Interrupted process CPSR
    and     r1, r1, #CPSR_MODE_MASK
    cmp     r1, #CPSR_MODE_USR          ; Check for usr mode, if found
    moveq   r1, #CPSR_MODE_SYS          ; then use system mode
    mrs     r2, CPSR
    bic     r2, r2, #CPSR_MODE_MASK
    orr     r1, r2, r1
    msr     CPSR_c, r1                  ; Switch to the interruped context
    stmia   r0, {r13-r14}               ; Store sp and lr
    cps     #CPSR_MODE_MON              ; Switch back to monitor mode
    pop     {r1-r2}
    bx      lr
    ENDFUNC
;===================================================================
; Debug FIQ Secure Monitor Call handler
;===================================================================
; When debug FIQ exit from TZ happens, we only allow returning to
; TZ to continue FIQ handling. Non-secure context saving is not
; necessary when returning, as the non-secure context is created on
; the fly when switching from secure to non-secure debug FIQ handler.
tzbsp_smc_handler_nfdbg
    push    {r0-r3}             ; Make some work registers.
    ; Detect which world executed the SMC
    mrc     p15, 0, r0, c1, c1, 0
    tst     r0, #SCR_NS_BIT
    beq     nfdbg_secure_exit
    ; Return back from non-secure debug FIQ handler to TZ FIQ handler.
nfdbg_secure_entry
    ; Locks, monitor commands and non-secure context saving ignored. This
    ; SMC handler will only switch back to non-secure side, which is in
    ; interrupted FIQ handler state.
    pop     {r0-r3}                     ; Unwind stack (corrupting regs ok).
    SetSecure r0                        ; Set up SCR for Secure world.
    ; Restore the secure side context.
    mov     lr, sp              ; Save the monitor stack address.
    ldr     sp, =Workspace_S_NFDBG_Base ; Start address for secure context.
    LoadState                           ; Restore secure context.
    mov     sp, lr              ; Restore the monitor stack address.
    ldr     lr, =S_NFDBG_PC     ; Start location for secure PC and SPSR.
    rfeia   lr                  ; Return to Secure World code.
    ; Control is handed to non-secure side debug FIQ handler in MON mode.
    ; The control is never handed via SMC call, if exit from secure side
    ; is attempted via this SMC handler, then it is an error condition.
nfdbg_secure_exit
    b       nfdbg_secure_exit  ; Start waiting for wdog reset.
;===================================================================
; FUNCTION: void tzbsp_hand_control_to_ns_nfdbg(void)
;===================================================================
; Hand control to non-secure debug FIQ handler. Current context is saved and
; non-secure handler context is constructed from scratch. The return from
; the non-secure side will end up executing this handler from nfdbg_fiq_return.
; Because FIQ handling is done in MON mode it isn't possible to SMC from
; the debug FIQ ISR to non-secure side (monitor itself is the context
; switching entity).
tzbsp_hand_control_to_ns_nfdbg FUNCTION
    stmfd   sp!, {lr}
    ; Store the secure world return address and SCPR.
    ldr     r0, =S_NFDBG_PC      ; Address of saved PC
    adr     r1, nfdbg_fiq_return ; Address of the return label
    mrs     r2, cpsr             ; Current MON CPSR.
    stmia   r0, {r1, r2}         ; Overwrite saved secure state.
    ; Save the secure context and load non-secure context.
    mov     lr, sp                          ; Save monitor stack.
    ldr     sp, =Workspace_S_NFDBG_Base     ; The start of secure context.
    SaveState                               ; Save secure context.
    mov     sp, lr                          ; Restore monitor stack.
    ; Clear the entire register file. For each mode SPSR is left as it is.
    mov     r4,  #0
    mov     r5,  #0
    mov     r6,  #0
    mov     r7,  #0
    mov     r8,  #0
    mov     r9,  #0
    mov     r10, #0
    mov     r11, #0
    mov     r12, #0
    cps     #CPSR_MODE_SYS
    mov     r13, #0
    mov     r14, #0
    cps     #CPSR_MODE_IRQ
    mov     r13, #0
    mov     r14, #0
    cps     #CPSR_MODE_SVC               ; Set up stack for non-secure handler.
    ldr     r0, =g_tzbsp_ns_debug_stack  ; Location of non-secure stack addr.
    ldr     r0, [r0]                     ; Address of non-secure stack.
    mov     r13, r0
    mov     r14, #0
    cps     #CPSR_MODE_ABT
    mov     r13, #0
    mov     r14, #0
    cps     #CPSR_MODE_UND
    mov     r13, #0
    mov     r14, #0
    cps     #CPSR_MODE_FIQ
    mov     r8,  #0
    mov     r9,  #0
    mov     r10, #0
    mov     r11, #0
    mov     r12, #0
    mov     r13, #0
    mov     r14, #0
    cps     #CPSR_MODE_MON                  ; Switch back to monitor mode.
    ldr     lr, =g_tzbsp_ns_debug_vector    ; Location of non-secure jump addr.
    ldr     lr, [lr]                        ; Non-secure jump address.
    ; Construct non-secure CPSR. Non-secure debug FIQ handler will execute
    ; in SVC mode, IRQs and FIQs disabled.
    mov     r0, #CPSR_MODE_SVC:OR:CPSR_I_BIT:OR:CPSR_F_BIT
    ldr     r1, =g_tzbsp_ns_debug_thumb     ; Thumb mode status location.
    ldr     r1, [r1]                        ; Thumb mode status itself.
    cmp     r1, #1                          ; If 1, then non-secure...
    orreq   r0, #CPSR_T_BIT                 ; ...handler is Thumb code.
    msr     SPSR_cfsx, r0
    ldr     r0, =g_tzbsp_ns_debug_ctx   ; Location of debug context address.
    ldr     r0, [r0]    ; Address of debug context, passed to non-secure side.
    mov     r1, #0
    mov     r2, #0
    mov     r3, #0
    MonCallReturnToNonSecure r1 ; Jump to non-secure debug FIQ handler.
    ; When non-secure side returns control to TZ, it will continue from
    ; this label. Context is restored by tzbsp_smc_handler_nfdbg.
nfdbg_fiq_return
    ldmfd   sp!, {pc} ; Return to the debug FIQ handler.
    ENDFUNC
;===================================================================
; FUNCTION: void tzbsp_init_monitor( void )
;===================================================================
;
; Initializes the Secure Monitor
;
tzbsp_init_monitor FUNCTION
    ; ------------------------------------------------------------------
    ; Change to monitor mode and program the Monitor vector table.
    ; ------------------------------------------------------------------
    msr     CPSR_c, #CPSR_MODE_MON:OR:CPSR_I_BIT:OR:CPSR_F_BIT
    ldr     r0, =tzbsp_mon_vector_table
    mcr     p15, 0, r0, c12, c0, 1
    ; ------------------------------------------------------------------
    ; Initialize Non secure system control registers
    ; ------------------------------------------------------------------
    bl      tzbsp_non_secure_sysini
    ; Set up the CPU specific monitor stack.
    ldr     sp, =tzbsp_mon_stack_start ; load base address for stack
    mrc     p15, 0, r0, c0, c0, 5   ; Read MPIDR
    and     r0, r0, #0xFF           ; LSB is AFFL0 = CPU number.
    mov     r1, #(4 * 128)
    mul     r1, r0                  ; obtain stack offset for CPU
    sub     sp, r1                  ; shift stack to CPU specific stack
    ; Let's route FIQs to monitor to catch any spurious/unexpected FIQs during
    ; initialization.
    mov     r0, #SCR_FIQ_BIT
    mcr     p15, 0, r0, c1, c1, 0
    misb
    ; Change to Supervisor mode
    msr     CPSR_c, #CPSR_MODE_SVC:OR:CPSR_I_BIT:OR:CPSR_F_BIT
    ; Return to Secure World (SWD)
    bx      r14
    ENDFUNC
;===================================================================
; FUNCTION: void tzbsp_non_secure_sysini( void )
;===================================================================
;
; Initializes the non-secure banked registers. The full SYSINI is done in
; the secure world.
tzbsp_non_secure_sysini FUNCTION
    ; Copy the secure banked register for SCTLR. The banked copies for ACTLR
    ; and CCPR are handled by the sysini.
    mrc     p15, 0, r1, c1, c0, 0   ; Read secure SCTLR
    ; Switch to non-secure world.
    mov     r0, #SCR_FW_BIT:OR:SCR_AW_BIT:OR:SCR_NS_BIT
    mcr     p15, 0, r0, c1, c1, 0
    misb
    ; SCTLR is only partially banked between secure and
    ; non-secure worlds.
    mcr     p15, 0, r1, c1, c0, 0   ; Write SCTLR
    mov     r0, #0
    ; All fully banked CP15 registers have either zero or unpredictable
    ; HW reset value -> reset to zero.
    mcr     p15, 2, r0, c0, c0, 0   ; CSSELR
    mcr     p15, 0, r0, c2, c0, 0   ; TTBR0
    mcr     p15, 0, r0, c2, c0, 1   ; TTBR1
    mcr     p15, 0, r0, c2, c0, 2   ; TTBCR
    mcr     p15, 0, r0, c3, c0, 0   ; DACR
    mcr     p15, 0, r0, c5, c0, 0   ; DFSR
    mcr     p15, 0, r0, c5, c0, 1   ; IFSR
    mcr     p15, 0, r0, c5, c1, 0   ; ADFSR
    mcr     p15, 0, r0, c5, c1, 1   ; AIFSR
    mcr     p15, 0, r0, c6, c0, 0   ; DFAR
    mcr     p15, 0, r0, c6, c0, 2   ; IFAR
    mcr     p15, 0, r0, c7, c4, 0   ; PAR
    mcr     p15, 0, r0, c10, c2, 0  ; MAIR0 (was PRRR)
    mcr     p15, 0, r0, c10, c2, 1  ; MAIR1 (was NMRR)
    mcr     p15, 0, r0, c10, c4, 0  ; MMUDMTR
    mcr     p15, 0, r0, c10, c4, 1  ; MMUDCPR
    ldr     r1, =tzbsp_ns_vbar
    ldr     r0, [r1]
    mcr     p15, 0, r0, c12, c0, 0  ; VBAR
    ; Clear the VBAR to zero. The first value is passed through from
    ; SBL2.
    mov     r0, #0
    str     r0, [r1]
    mcr     p15, 0, r0, c13, c0, 0  ; FCSEIDR
    mcr     p15, 0, r0, c13, c0, 1  ; CONTEXTIDR
    mcr     p15, 0, r0, c13, c0, 2  ; TPIDRURW
    mcr     p15, 0, r0, c13, c0, 3  ; TPIDRURO
    mcr     p15, 0, r0, c13, c0, 4  ; TPIDRPRW
    ; Back to secure world.
    mov     r0, #SCR_FW_BIT:OR:SCR_AW_BIT
    mcr     p15, 0, r0, c1, c1, 0
    misb
    bx      lr
    ENDFUNC
;===================================================================
; FUNCTION - void tzbsp_install_irq_function( void (*) (void) )
;===================================================================
;
; Function to set the IRQ intercept function. Setting this function
; allows TZBSP to inspect raised IRQs. The function is not supposed
; to acknowledge/end the IRQ interrupts as IRQ handling is done in
; non-secure side. If the function is set to NULL (default), then
; IRQ is handed to non-secure side without TZBSP intervention.
;
; Parameters:
; r0 - Contains the function pointer of type void (*) (void). The
;      function will be executed in monitor mode when invoked.
;
tzbsp_install_irq_function FUNCTION
    ldr     r1, =tzbsp_irq_function
    str     r0, [r1]
    bx      lr
    ENDFUNC
;===================================================================
; FUNCTION - void tzbsp_set_ns_return_addr( uint32 addr )
;===================================================================
;
; Parameters:
; r0 - Contains the new NS return address
;
tzbsp_set_ns_return_addr FUNCTION
    ldr     r2, =N_CPSR
    ldr     r1, [r2]
    Save_NS_PC_CPSR r0, r1, r2
    bx      lr
    ENDFUNC
;===================================================================
; FUNCTION - void tzbsp_install_fiq_function( int (*) (void) )
;===================================================================
;
; Function to set the FIQ handler function. Setting this function
; allows TZBSP to handle raised FIQs. The function is supposed to
; acknowledge/end the FIQ interrupts. This function must always be
; set when receiving FIQs. That is, if FIQ is raised while the
; handler function is NULL, then the monitor will start waiting in
; an error loop.
;
; Parameters:
; r0 - Contains the function pointer of type void (*) (void). The
;      function will be executed in monitor mode when invoked.
;
tzbsp_install_fiq_function FUNCTION
    ldr     r1, =tzbsp_fiq_function
    str     r0, [r1]
    bx      lr
    ENDFUNC
;===================================================================
; FUNCTION - boolean tzbsp_is_non_secure(void)
;===================================================================
; Queries SCR[NS] state. Should only be used in monitor mode.
tzbsp_is_non_secure FUNCTION
    RCP15_SCR   r0
    and         r0, r0, #1              ; SCR[NS] is bit [0]
    bx          lr
    ENDFUNC
;======================================================================
; Allow PL1 access to the PL1 timer module
;======================================================================
tzbsp_set_cnthctl FUNCTION
    ; To modify the CNTHCTL register, we need to be in Monitor mode
    ; with the NS bit set
    msr   CPSR_c, #CPSR_MODE_MON:OR:CPSR_I_BIT:OR:CPSR_F_BIT
    mrc   p15, 0, r3, c1, c1, 0   ; Read the current SCR
    orr   r2, r3, #SCR_NS_BIT     ; Set SCR.NS=1
    mcr   p15, 0, r2, c1, c1, 0   ; write SCR
    ; Bits to allow PL1 and PL2 to set the access permission for PL1 timer
    mov   r0, #0x3                ; Set the PL1PCEN and PL1PCTEN bits
    mcr   p15, 4, r0, c14, c1, 0  ; Write to CNTHCTL register
    mcr   p15, 0, r3, c1, c1, 0   ; write SCR back to original
    msr   CPSR_c, #CPSR_MODE_SVC:OR:CPSR_I_BIT:OR:CPSR_F_BIT
    bx    lr
    ENDFUNC
;======================================================================
; Sets CNTFRQ, so as to be readable by PL0 and PL1
;
; r0  contains the frequency
;======================================================================
tzbsp_set_cntfrq FUNCTION
    mcr   p15, 0, r0, c14, c0, 0
    bx    lr
    ENDFUNC
;============================================================================
; TZBSP_MMU_DISABLE_AND_CONFIGURE_NS
;
; ARGS
;
;  DESCRIPTION -
;        Disables the secure MMU and, after copying the configuration to
;        non-secure world, enables the non-secure MMU.
;        Performs the following:
;                1- Disable I and D caches, and flush all caches
;                2- Disable secure MMU and invalidate TLB
;                3- Switch to Monitor mode, set NS bit, and copy TTBR0, TTBCR,
;                   and DACR to non-secure MMU
;                4- Enable I and D caches, enable MMU
;                5- Clear NS bit, switch to SVC, and remove TZ mappings from
;                   SBL pagetable (taken from TTBR0)
;============================================================================
tzbsp_mmu_disable_and_configure_ns FUNCTION
    stmfd   sp!, {r4, lr}
    bl      tzbsp_mmu_cache_disable
    ; Set the non-secure Translation Table registers to be the same
    ; as the current secure registers.  This requires being in Monitor
    ; mode and toggling the SCR.NS bit
    msr     CPSR_c, #CPSR_MODE_MON:OR:CPSR_I_BIT:OR:CPSR_F_BIT
    ; Read the TT registers
    mrc     p15, 0, r0, c2, c0, 0   ; TTBR0
    mrc     p15, 0, r1, c2, c0, 2   ; TTBCR
    mrc     p15, 0, r2, c3, c0, 0   ; DACR
    ; Switch to NS mode
    mrc     p15, 0, r4, c1, c1, 0   ; Read the current SCR
    orr     r3, r4, #SCR_NS_BIT     ; Set SCR.NS=1
    mcr     p15, 0, r3, c1, c1, 0   ; write SCR
    ; Write the NS TT registers, and enable the MMU and caches
    mcr     p15, 0, r0, c2, c0, 0   ; TTBR0
    mcr     p15, 0, r1, c2, c0, 2   ; TTBCR
    mcr     p15, 0, r2, c3, c0, 0   ; DACR
    mov     r3, r0                  ; Save TTBR0 for later use, the follow
                                    ; macros use r0
    bl      tzbsp_mmu_cache_enable_no_cfg
    mcr     p15, 0, r4, c1, c1, 0   ; write SCR back to original
    msr     CPSR_c, #CPSR_MODE_SVC:OR:CPSR_I_BIT:OR:CPSR_F_BIT
    ; Remove the TZ entries in the SBL pagetable
    mov     r0, r3
    blx     mmu_remove_tz_mapping   ; r0 holds Level-1 pagetable address
    ldmfd   sp!, {r4, pc}
    ENDFUNC
;============================================================================
; TZBSP_DISABLE_NS_MMU_AFTER_SBL
;
; ARGS
;
;  DESCRIPTION -
;       The following functions are performed:
;               1- Unlock the L2 cache lines
;               2- Clean and Invalidate L2
;               3- Invalidate TLB
;               4- Disable the non-secure MMU
;============================================================================
tzbsp_disable_ns_mmu_after_sbl FUNCTION
    stmfd   sp!, {r4-r11,lr}  ; Save the entire register map, since sysini doesn't
    ; Disable the non-secure MMU; this requires being in Monitor
    ; mode and toggling the SCR.NS bit
    msr     CPSR_c, #CPSR_MODE_MON:OR:CPSR_I_BIT:OR:CPSR_F_BIT
    mrc     p15, 0, r3, c1, c1, 0   ; Read the current SCR
    orr     r2, r3, #SCR_NS_BIT     ; Set SCR.NS=1
    mcr     p15, 0, r2, c1, c1, 0   ; write SCR
    bl      tzbsp_mmu_cache_disable
    ; Zero out the TTBR0
    mov     r0, #0
    mcr     p15, 0, r0, c2, c0, 0
    mcr     p15, 0, r3, c1, c1, 0   ; write SCR back to original
    msr     CPSR_c, #CPSR_MODE_SVC:OR:CPSR_I_BIT:OR:CPSR_F_BIT
    bl      tzbsp_mmu_cache_disable
    ; Do sysini with L2 invalidation.
    mov     r0, #1
    blx     sysini_wrapper
    bl      tzbsp_mmu_cache_enable_no_cfg
    ldmfd   sp!, {r4-r11,pc}
    ENDFUNC
;============================================================================
;
;                             MODULE DATA
;
;============================================================================
;===================================================================
; Monitor Workspace
;===================================================================
;
; Workspace for storing the non-secure and secure processor
; contexts. Only one call context is supported, hence only one CPU
; at any given time can execute secure side code. That is, the
; secure side is not re-entrant.
;
    AREA |MonitorWorkspaceDataArea|, DATA, READWRITE, ALIGN=2
; Using directive SPACE will allocate zeroed bytes.
MonitorWorkspaceBase
Workspace_N_Begin
N_PC           SPACE 0x4    ; PC to load to return to Normal World
N_CPSR         SPACE 0x4    ; CPSR to load when returning to Normal Worl
N_USR_R0       SPACE 0x4    ; Normal world registers
N_USR_R1       SPACE 0x4
N_USR_R2       SPACE 0x4
N_USR_R3       SPACE 0x4
N_USR_R4       SPACE 0x4
N_USR_R5       SPACE 0x4
N_USR_R6       SPACE 0x4
N_USR_R7       SPACE 0x4
N_USR_R8       SPACE 0x4
N_USR_R9       SPACE 0x4
N_USR_R10      SPACE 0x4
N_USR_R11      SPACE 0x4
N_USR_R12      SPACE 0x4
N_USR_R13      SPACE 0x4
N_USR_R14      SPACE 0x4
Workspace_N_Base
N_IRQ_SPSR     SPACE 0x4    ; Normal banked registers
N_IRQ_R13      SPACE 0x4
N_IRQ_R14      SPACE 0x4
N_SVC_SPSR     SPACE 0x4
N_SVC_R13      SPACE 0x4
N_SVC_R14      SPACE 0x4
N_ABT_SPSR     SPACE 0x4
N_ABT_R13      SPACE 0x4
N_ABT_R14      SPACE 0x4
N_UND_SPSR     SPACE 0x4
N_UND_R13      SPACE 0x4
N_UND_R14      SPACE 0x4
N_FIQ_SPSR     SPACE 0x4
N_FIQ_R8       SPACE 0x4
N_FIQ_R9       SPACE 0x4
N_FIQ_R10      SPACE 0x4
N_FIQ_R11      SPACE 0x4
N_FIQ_R12      SPACE 0x4
N_FIQ_R13      SPACE 0x4
N_FIQ_R14      SPACE 0x4
Workspace_N_End
Workspace_S_Begin
S_PC           SPACE 0x4    ; PC to load to return to Secure World
S_CPSR         SPACE 0x4    ; CPSR to load to return to Secure World
S_USR_R0       SPACE 0x4    ; Secure World registers
S_USR_R1       SPACE 0x4
S_USR_R2       SPACE 0x4
S_USR_R3       SPACE 0x4
S_USR_R4       SPACE 0x4
S_USR_R5       SPACE 0x4
S_USR_R6       SPACE 0x4
S_USR_R7       SPACE 0x4
S_USR_R8       SPACE 0x4
S_USR_R9       SPACE 0x4
S_USR_R10      SPACE 0x4
S_USR_R11      SPACE 0x4
S_USR_R12      SPACE 0x4
S_USR_R13      SPACE 0x4
S_USR_R14      SPACE 0x4
Workspace_S_Base
S_IRQ_SPSR     SPACE 0x4    ; Secure World banked registers
S_IRQ_R13      SPACE 0x4
S_IRQ_R14      SPACE 0x4
S_SVC_SPSR     SPACE 0x4
S_SVC_R13      SPACE 0x4
S_SVC_R14      SPACE 0x4
S_ABT_SPSR     SPACE 0x4
S_ABT_R13      SPACE 0x4
S_ABT_R14      SPACE 0x4
S_UND_SPSR     SPACE 0x4
S_UND_R13      SPACE 0x4
S_UND_R14      SPACE 0x4
S_FIQ_SPSR     SPACE 0x4
S_FIQ_R8       SPACE 0x4
S_FIQ_R9       SPACE 0x4
S_FIQ_R10      SPACE 0x4
S_FIQ_R11      SPACE 0x4
S_FIQ_R12      SPACE 0x4
S_FIQ_R13      SPACE 0x4
S_FIQ_R14      SPACE 0x4
Workspace_S_END
Workspace_S_NFDBG_Begin
S_NFDBG_PC           SPACE 0x4    ; PC to load to return to Secure World
S_NFDBG_CPSR         SPACE 0x4    ; CPSR to load to return to Secure World
S_NFDBG_USR_R0       SPACE 0x4    ; Secure World registers
S_NFDBG_USR_R1       SPACE 0x4
S_NFDBG_USR_R2       SPACE 0x4
S_NFDBG_USR_R3       SPACE 0x4
S_NFDBG_USR_R4       SPACE 0x4
S_NFDBG_USR_R5       SPACE 0x4
S_NFDBG_USR_R6       SPACE 0x4
S_NFDBG_USR_R7       SPACE 0x4
S_NFDBG_USR_R8       SPACE 0x4
S_NFDBG_USR_R9       SPACE 0x4
S_NFDBG_USR_R10      SPACE 0x4
S_NFDBG_USR_R11      SPACE 0x4
S_NFDBG_USR_R12      SPACE 0x4
S_NFDBG_USR_R13      SPACE 0x4
S_NFDBG_USR_R14      SPACE 0x4
Workspace_S_NFDBG_Base
S_NFDBG_IRQ_SPSR     SPACE 0x4    ; Secure World banked registers
S_NFDBG_IRQ_R13      SPACE 0x4
S_NFDBG_IRQ_R14      SPACE 0x4
S_NFDBG_SVC_SPSR     SPACE 0x4
S_NFDBG_SVC_R13      SPACE 0x4
S_NFDBG_SVC_R14      SPACE 0x4
S_NFDBG_ABT_SPSR     SPACE 0x4
S_NFDBG_ABT_R13      SPACE 0x4
S_NFDBG_ABT_R14      SPACE 0x4
S_NFDBG_UND_SPSR     SPACE 0x4
S_NFDBG_UND_R13      SPACE 0x4
S_NFDBG_UND_R14      SPACE 0x4
S_NFDBG_FIQ_SPSR     SPACE 0x4
S_NFDBG_FIQ_R8       SPACE 0x4
S_NFDBG_FIQ_R9       SPACE 0x4
S_NFDBG_FIQ_R10      SPACE 0x4
S_NFDBG_FIQ_R11      SPACE 0x4
S_NFDBG_FIQ_R12      SPACE 0x4
S_NFDBG_FIQ_R13      SPACE 0x4
S_NFDBG_FIQ_R14      SPACE 0x4
MonitorWorkspaceEnd
tzbsp_irq          SPACE 0x4 ; IRQ status, 0: no IRQ, !0: waiting IRQ return.
tzbsp_irq_function SPACE 0x4 ; IRQ intercept function address, can be NULL.
tzbsp_fiq_function SPACE 0x4 ; FIQ handler function address, cannot be NULL.
tzbsp_ns_vbar      SPACE 0x4 ; Address of the non-secure VBAR.
; Reserve space for SCR for each CPU for FIQ handling.
tzbsp_scr          SPACE 0x4 * 4
;===================================================================
; Monitor Stack
;===================================================================
;
; Monitor stack memory for each CPU. Stacks are full descending; the
; stack grows downwards and the current value of sp points to the
; last taken stack item. The stacks are 8 byte aligned.
;
; The same stack area is used by monitor for SMC, IRQ and FIQ
; handling.
;
; The stack area is aligned to Krait cache line size, which is 128 bytes (2^8).
    AREA |MonitorStack|, DATA, READWRITE, ALIGN=8
tzbsp_mon_stack_end     SPACE   (4 * 128) * 4
tzbsp_mon_stack_start   DCD     0xdeadbeef
;===================================================================
; Stores the context during an FIQ (taken in monitor mode)
;===================================================================
;
; Workspace for storing context of an interrupted process
;
    AREA |FIQWorkspaceDataArea|, DATA, READWRITE, ALIGN=2
Workspace_Mon_Begin
S_MON_IP       SPACE 0x4    ; Interrupted context IP
S_MON_SPSR     SPACE 0x4    ; CPSR of the interruped context
Workspace_Mon_Registers
S_MON_R0       SPACE 0x4    ; Interrupted context registers
S_MON_R1       SPACE 0x4
S_MON_R2       SPACE 0x4
S_MON_R3       SPACE 0x4
S_MON_R4       SPACE 0x4
S_MON_R5       SPACE 0x4
S_MON_R6       SPACE 0x4
S_MON_R7       SPACE 0x4
S_MON_R8       SPACE 0x4
S_MON_R9       SPACE 0x4
S_MON_R10      SPACE 0x4
S_MON_R11      SPACE 0x4
S_MON_R12      SPACE 0x4
S_MON_R13      SPACE 0x4
S_MON_R14      SPACE 0x4
;===================================================================
; TZ locks
;===================================================================
; Krait SWI, CTR[23:20], RGSIZE[3:0], Reservation Granule Size,
; Log2 of words in the reservation granule, reset value = 0x5 (32 words)
; Place the logs on separate L1 cache lines.  For Krait, line size is 64 bytes.
    AREA |MonitorLock|, DATA, READWRITE, ALIGN=7
tzbsp_smc_lock      SPACE 0x4 ; Spinlock mem, 0: not busy, 1: is busy.
filler0             SPACE 0x4 * 31
    AREA TZBSP_UNCACHED_ZI_LOCKS, DATA, READWRITE, ALIGN=7
tzbsp_boot_lock     SPACE 0x4 ; Spinlock mem, 0: not busy, 1: is busy.
filler1             SPACE 0x4 * 31
g_tzbsp_clk_mutex   SPACE 0x4 ; Spinlock mem, 0: not busy, 1: is busy.
filler3             SPACE 0x4 * 31
    END



;============================================================================
;
;             Scorpion architecture assembly language MMU support
;
; GENERAL DESCRIPTION
;   This file contains the Scorpion hierarchichal cache and mmu control functions.
;   File originaly taken from: 
;
; Copyright 2011 by QUALCOMM Technologies, Incorporated.  All Rights Reserved
;============================================================================
;============================================================================
;
;                       EDIT HISTORY FOR MODULE
;
; $Header: 
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
;
; when      who     what, where, why
; --------  ---     ---------------------------------------------------------
; 01/25/11  pre     Initial Version
;============================================================================
;============================================================================
;                             MODULE DEFINES
;============================================================================
;============================================================================
;                             MODULE INCLUDES
;============================================================================
        GBLS    current_node_name
        GBLS    current_node_type
        GBLA    current_node_aregcount
        GBLA    current_node_vregcount
        MACRO
        pusha   $stack, $reg
        str     $reg, [$stack, #-4]!
        MEND
        MACRO
        popa    $stack, $reg
        ldr     $reg, [$stack], #4
        MEND
        MACRO
        ENTRY_NODE $node_name
current_node_name       SETS    "$node_name"
        EXPORT  $node_name
        ALIGN
        ROUT
$node_name
        MEND
                                        ;  The end of an entry node.
        MACRO
        ENTRY_NODE_END
$current_node_name._exit
current_node_name       SETS    ""
        MEND
        MACRO
        LEAF_NODE $node_name
current_node_type       SETS    "Leaf"
        CODE16
        ENTRY_NODE $node_name
        bx      pc
        ALIGN
        CODE32
        EXPORT  $node_name._32
$node_name._32
        MEND
        MACRO
        LEAF_NODE_END $node_name
        ASSERT  "$current_node_type" = "Leaf"
        ENTRY_NODE_END $node_name
        bx     lr
current_node_type       SETS    ""
        MEND
        MACRO
        LEAF_NODE_16 $node_name
current_node_type       SETS    "Leaf16"
        CODE16
        ENTRY_NODE $node_name
        MEND
        MACRO
        LEAF_NODE_END_16 $node_name
        ASSERT  "$current_node_type" = "Leaf16"
        ENTRY_NODE_END $node_name
        bx     lr
current_node_type       SETS    ""
        MEND
        MACRO
        ALTERNATE_ENTRY_16 $node_name
        ASSERT  "$current_node_type" = "Leaf16"
        EXPORT $node_name
$node_name
        MEND
        MACRO
        blatox     $destreg
        ROUT
        tst     $destreg, #0x01          
        ldrne   lr, =%1
        ldreq   lr, =%2
        bx      $destreg
1
        CODE16
        bx      pc
        ALIGN
        CODE32
2
        MEND
    GET tzbsp_libarm_asm.inc
    GET tzbsp_asm.inc
    GET qctps_common_macros.inc
;============================================================================
;                             MODULE IMPORTS
;============================================================================
    IMPORT  tzbsp_init_tts
    IMPORT  tzbsp_cache_flush
    IMPORT  tzbsp_dcache_clean_all
    IMPORT  tzbsp_dcache_inval_all
    IMPORT  tzbsp_dcache_flush_all
    IMPORT  tzbsp_dcache_clean_region
    IMPORT  tzbsp_dcache_inval_region
    IMPORT  tzbsp_dcache_flush_region
    IMPORT  tzbsp_prrr
    IMPORT  tzbsp_nmrr
    IMPORT  tzbsp_dcache_inval_l1
;============================================================================
;                             MODULE EXPORTS
;============================================================================
    EXPORT  mmu_l1_tt               ; Memory for L1 TTs
    EXPORT  mmu_l2_tt               ; Memory for L2 TTs
    EXPORT  tzbsp_mmu_cache_init
    EXPORT  tzbsp_mmu_cache_disable
    EXPORT  tzbsp_mmu_cache_re_enable
    EXPORT  tzbsp_mmu_cache_enable_no_cfg
    EXPORT  tzbsp_mmu_disable       ; For L2 TT dump purposes only
    EXPORT  tzbsp_mmu_enable        ; For L2 TT dump purposes only
    EXPORT  tzbsp_tlb_inval_all
    EXPORT  tzbsp_itlb_inval_all
    EXPORT  tzbsp_tlb_inval_all_is
    EXPORT  tzbsp_tlb_inval_by_asid_is
    EXPORT  tzbsp_tlb_inval_by_mva_is
    EXPORT  tzbsp_dtlb_inval_all
    EXPORT  tzbsp_vtop
;============================================================================
;                             MODULE EQUATES
;============================================================================
C1_REG_DEFAULT        EQU    0x10C56870
; CPSR Processing Modes M[4:0] (CPSR[M]) bits
CPSR_MODE_USR               EQU     0x10
CPSR_MODE_FIQ               EQU     0x11
CPSR_MODE_IRQ               EQU     0x12
CPSR_MODE_SVC               EQU     0x13
CPSR_MODE_MON               EQU     0x16
CPSR_MODE_ABT               EQU     0x17
CPSR_MODE_UND               EQU     0x1b
CPSR_MODE_SYS               EQU     0x1f
; CPSR Interrupt Mask (CPSR[I], CPSR[I]) bits
CPSR_I_BIT                  EQU     0x80
CPSR_F_BIT                  EQU     0x40
; SCR (Secure Configuration Register) bits
SCR_NS_BIT                  EQU     0x01    ; Non-Secure (NS) bit
SCR_IRQ_BIT                 EQU     0x02    ; IRQ bit
SCR_FIQ_BIT                 EQU     0x04    ; FIQ bit
SCR_EA_BIT                  EQU     0x08    ; EA bit
SCR_FW_BIT                  EQU     0x10    ; F Bit writable (FW) bit
SCR_AW_BIT                  EQU     0x20    ; A Bit writable (AW) bit
;============================================================================
;                            MMU PAGETABLE
;
; Create a page table for the mmu. This pagetable is 16 kb and contains 4k
; word section descriptors that will be populated by create_pagetable.
;============================================================================
   AREA  L1_PAGE_TABLE_BASE, DATA, ALIGN = (((14))), NOINIT
mmu_l1_tt
   % (4 * 0x00001000)
   AREA  L2_PAGE_TABLE_BASE, DATA, ALIGN = 10, NOINIT
mmu_l2_tt
   % (4 * (0x00100000 / 0x00001000) * 17)
;============================================================================
;
;                        MACROS AND FUNCTIONS
;
;============================================================================
   AREA    ARM_MMU, CODE, READONLY
   PRESERVE8
   CODE32
;============================================================================
; MACRO mmu_set_default_cfg
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Sets a default value into the mmu control register
;   we do not really need to do this, but we do it to
;   know exactly the state of the processor.
;
; TBD - We should most likely only use this macro at initialization.
;
; TBD - handle default value as or of all the below from equates
;
;  Value  bit  Name    Function
; ---------------------------------------------------
;  0     0 - M bit - MMU disabled
;  0     1 - A bit - Alignment fault disabled
;  0     2 - C bit - Data Cache disabled
;  0     3 - W bit - Write Buffer enabled
;  ----------
;  111  4-6        - SHOULD BE ONES
;  0     7 - B bit - Little Endian
;  ----------
;  0     8 - S bit - MMU Protection disabled
;  0     9 - R bit - ROM Protection disabled
;  0    10 - F bit - SHOULD BE ZERO
;  0    11 - Z bit - Program Flow Protection disabled
;  ----------
;  0    12 - I bit - Level one Instruction Cache disabled
;  0    13 - V bit - Exception Vector Location is Normal
;  0    14 - RR bit- Replacement strategy is Normal/Random (not Round Robin)
;  0    15 - L4 bit- Loads to PC set the T bit
;  ----------
;  1    16 - DT bit- Global Data TCM enabled - SHOULD BE ONE for ARM11JF-S
;  0    17 -       - SHOULD BE ZERO
;  1    18 - IT bit- Global Instruction TCM enabled
;  0    19 -       - SHOULD BE ZERO
;  ----------
;  0    20 -       - SHOULD BE ZERO
;  0    21 - FI bit- Low Latency interrupt features disabled
;  1    22 - U bit - Unaligned data access enabled
;  1    23 - XP bit- HW page translation mechanism subpage AP bits disabled
;  ----------
;  0    24 - VE bit- Interrupt vectors are fixed (no VIC)
;  0    25 - EE bit- CPSR E bit set to 0 on taking an exception
;  0    26 -       - Reserved
;  0    27 - NMFI  - Non maskable fiq
;  ----------
;  0    28 - TRE   - TEX remap enable
;  0    29 - AFE   - Access flag enable
;  0    30 - TE    - Thumb Exception
;  0    31 -       - Reserved
;============================================================================
   MACRO
   mmu_set_default_cfg
   ldr    r0, =C1_REG_DEFAULT
   mcr    p15, 0, r0, c1, c0, 0
   MEND
;============================================================================
; MACRO mmu_invalidate_tlb
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Invalidates the entire Translation Look-aside Buffer (TLB) as a unified
;   operation (Data and Instruction). Invalidates all unlocked entries in the
;   TLB. Causes the prefetch buffer to be flushed. All following instructions
;   are fetched after the TLB invalidation.
;   We should do this before we enable to MMU.
;============================================================================
   MACRO
   mmu_invalidate_tlb
;   mcr     p15, 0, r0, c8, c7, 0
   TLBIALLIS
   MEND
;============================================================================
; MACRO mmu_enable_i_and_d_cache
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Enables the Cache
;   Does a read modify write, so no other bits in the control register are
;   affected
;============================================================================
   MACRO
   mmu_enable_i_and_d_cache
   mrc     p15, 0, r0, c1, c0, 0   ;  Read System Control register
   orr     r0, r0, # (1 << 2)      ;  C bit - Enable Data Caches
   orr     r0, r0, # (1 << 12)     ;  I bit - Enable Instruction Caches
   mcr     p15, 0, r0, c1, c0, 0   ;  Write System Control register
   MEND
;============================================================================
; MACRO mmu_disable_i_and_d_cache
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Disables the Data Cache
;   Does a read modify write, so no other bits in the control register are
;   affected
;============================================================================
   MACRO
   mmu_disable_i_and_d_cache
   mrc     p15, 0, r0, c1, c0, 0   ;  Read SCTLR, Control register
   bic     r0, r0, #0x4            ;  C bit - Disable Data Cache
   bic     r0, r0, #0x1000         ;  I bit - Disable Instruction Cache
   mcr     p15, 0, r0, c1, c0, 0   ;  Write SCTLR, Control register
   MEND
;============================================================================
; MACRO mmu_set_page_table_base
;
; ARGS
;   base is as follows:
;
;  bit  Name    Function
; ---------------------------------------------------
;  0 - C bit - Page table walk is Inner Cacheable?
;  1 - S bit - Page table walk is Shareable?
;  2 - IMP bit - Implementation defined (Parity enabled for tablewalk)
; 3-4 - RGN bits- Outer cachable attributes for page table walking:
;                 00=Normal memory, Outer Noncacheable
;                 01=Normal memory, Outer Write-Back, write allocate
;                 10=Normal memory, Outer Write-Through, no allocate on write
;                 11=Normal memory, Outer Write-Back, no allocate on write
;  5 - Not Outer Shareable bit. (ARMv7)
;
;  6 - Combines with bit 0 to be IRGN
;
; IRGN [6,0] when multiprocessing extensions are implemented
;      0b00 Normal memory, Inner Non-cacheable
;      0b01 Normal memory, Inner Write-Back Write-Allocate Cacheable
;      0b10 Normal memory, Inner Write-Through Cacheable
;      0b11 Normal memory, Inner Write-Back no Write-Allocate Cacheable
;
;14-31 - Pointer to level one translation table
;
;
; DESCRIPTION
;  Sets the base address of the mmu page table.
;============================================================================
   MACRO
   mmu_set_page_table_base  $base
   ldr     r0, =$base                ; location of our page table description
   mcr     p15, 0, r0, c2, c0, 0
   MEND
;============================================================================
; MACRO mmu_enable
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Enables the mmu
;   Does a read modify write, so no other bits in the control register are
;   affected
;============================================================================
   MACRO
   mmu_enable
   mrc     p15, 0, r0, c1, c0, 0      ; Read SCTLR, Control Register
   orr     r0, r0, # 0x1              ; Set MMU Enable bit
   mcr     p15, 0, r0, c1, c0, 0      ; Write SCTLR, Control Register
   MEND
;============================================================================
; MACRO mmu_disable
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Disables the mmu
;   Does a read modify write, so no other bits in the control register are
;   affected
;============================================================================
   MACRO
   mmu_disable
   mrc     p15, 0, r0, c1, c0, 0      ; Read SCTLR, Control Register
   bic     r0, r0, #0x1               ; Clear MMU bit
   mcr     p15, 0, r0, c1, c0, 0      ; Write SCTLR, Control Register
   MEND
;============================================================================
; MACRO mmu_align_fault_enable
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Enables address alignment fault checking
;   Does a read modify write, so no other bits in the control register are
;   affected
;============================================================================
   MACRO
   mmu_align_fault_enable
   mrc     p15, 0, r0, c1, c0, 0      ; Read SCTLR, Control Register
   orr     r0, r0, # 0x2              ; Set A bit - Enable alignment fault check
   mcr     p15, 0, r0, c1, c0, 0      ; Write SCTLR, Control Register
   MEND
;============================================================================
; MACRO mmu_set_dom_access
;
; ARGS
;   permissions
;     Contains 16 2 bit Domain fields
;
;      00 = No Access - Any access to this domain causes a domain fault
;      01 = Client - Accesses checked against access permissions in TLB entry
;      10 = Reserved - Any access to this domain causes a domain fault
;      11 = Master - No checking, no permission fault can be generated
;
;  +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
;  |D15|D14|D13|D12|D11|D10|D09|D08|D07|D06|D05|D04|D03|D02|D01|D00|
;  +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
;   struct
;   {
;      D15_Domain int:2
;      D14_Domain int:2
;      D13_Domain int:2
;      D12_Domain int:2
;      D11_Domain int:2
;      D10_Domain int:2
;      D9_Domain int:2
;      D8_Domain int:2
;      D7_Domain int:2
;      D6_Domain int:2
;      D5_Domain int:2
;      D4_Domain int:2
;      D3_Domain int:2
;      D2_Domain int:2
;      D1_Domain int:2
;      D0_Domain int:2
;   }
;
; DESCRIPTION
;   Sets the Access permissions for the 16 domains.
;
;============================================================================
   MACRO
   mmu_set_dom_access $hi_permissions, $lo_permissions
   mov     r0,   $lo_permissions
   movt    r0,   $hi_permissions
   mcr     p15, 0, r0, c3, c0, 0
   MEND
;============================================================================
; MACRO mmu_set_ttb_0
;
; ARGS
;   $boundary_size -
;   $base -
;
; DESCRIPTION
;   Sets a value that defines the Translation Table Base boundary that will
;   be used by TTB Registers, and loads Translation Table Base Register 0
;   with the base address of page table.
;   Default value of N after power up is 0 for backward compatible. Any
;   change to the Translation Table Base Register 0 needs to be paired with
;   write to Translation Table Base Control Register.
;============================================================================
   MACRO
   mmu_set_ttb_0  $boundary_size, $base
      ;TTBCR - TTB Control Register
      ; 2:0 - N - Size of TTBR0 translation table
      ;      0b000 - 16KB (default)
      ;      0b001 -  8KB
      ;      0b010 -  4KB
      ;      0b011 -  2KB
      ;      0b100 -  1KB
      ;      0b101 - 512B
      ;      0b110 - 256B
      ;      0b111 - 128B
      mcr   p15, 0, $boundary_size, c2, c0, 2
      ;TTBR0 - Translation Table Base Register 0
      ; 6 IRGN[0] Inner Region Cache Policy
      ;   (see IRGN[1] for definition of full field)
      ; 4:3 ORGN[1:0] Outer Region Cache
      ;   Policy
      ;   0b00 = non-cacheable
      ;   0b01 = copy-back, write-allocate
      ;   0b10 = write-through
      ;   0b11 = copy-back, write-no-allocate
      ; 1 SH Shareable
      ;   0 = Private memory region
      ;   1 = Shareable memory region
      ; 0 IRGN[1] Inner Region Cache Policy
      ;   0b00 = non-cacheable
      ;   0b01 = copy-back, write-allocate
      ;   0b10 = write-through
      ;   0b11 = copy-back, write-no-allocate
      orr   $base, $base, #0x0000007B
      mcr   p15, 0, $base, c2, c0, 0
      isb
   MEND
;============================================================================
; MACRO mmu_set_ttb_1
;
; ARGS
;   $base
;
; DESCRIPTION
;   Loads the Translation Table Base Register 1 with the base address of page
;   table.
;============================================================================
   MACRO
   mmu_set_ttb_1  $base
      ldr   r1, =0xFFFFC000
      and   $base, $base, r1     ; make sure [13:0] are 0.
      mcr   p15, 0, $base, c2, c0, 1
   MEND
;============================================================================
; Enable TeX Remap
;   tex remap scheme described in tzbsp_mmu_config.h
;============================================================================
   MACRO
   mmu_enable_tex_remap  $reg
      ldr     $reg, =tzbsp_prrr
      ldr     $reg, [$reg]
      mcr     p15, 0, $reg, c10, c2, 0 ; WCP15_PRRR $reg
      ldr     $reg, =tzbsp_nmrr
      ldr     $reg, [$reg]
      mcr     p15, 0, $reg, c10, c2, 1 ; WCP15_NMRR $reg
      ; Enable TEX remap
      mrc     p15, 0, $reg, c1, c0, 0    ; RCP15_SCTLR  $reg
      orr     $reg, $reg, #0x10000000
      mcr     p15, 0, $reg, c1, c0, 0    ; WCP15_SCTLR  $reg
   MEND
;============================================================================
; Enable AFE
;============================================================================
   MACRO
   mmu_enable_afe  $rx
      mrc     p15, 0, $rx, c1, c0, 0   ; RCP15_SCTLR  $reg
      orr     $rx, $rx, #0x20000000
      mcr     p15, 0, $rx, c1, c0, 0   ; WCP15_SCTLR  $reg
   MEND
;============================================================================
; mmu_enable_branch_prediction
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Enables branch prediction
;============================================================================
    MACRO
       mmu_enable_branch_prediction $r
       mrc     p15, 0, $r, c1, c0, 0   ; Read from CP15 Control Register
       orr     $r, $r, #(1 << 11)      ; Enable branch prediction
       mcr     p15, 0, $r, c1, c0, 0   ; Write back to CP15 Control Register
    MEND
;============================================================================
; mmu_disable_branch_prediction
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Disables branch prediction
;============================================================================
    MACRO
       mmu_disable_branch_prediction $r
       mrc     p15, 0, $r, c1, c0, 0   ; Read from CP15 Control Register
       bic     $r, $r, #(1 << 11)      ; Disable branch prediction
       mcr     p15, 0, $r, c1, c0, 0   ; Write back to CP15 Control Register
    MEND
;============================================================================
; tzbsp_tlb_inval_all
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Invalidate the entire unified TLB.
;============================================================================
tzbsp_tlb_inval_all FUNCTION
    mov     r0, #0
    mmu_invalidate_tlb
    dsb
    isb
    bx      lr
    ENDFUNC
;============================================================================
; tzbsp_itlb_inval_all
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Invalidate the entire intruction TLB.
;============================================================================
tzbsp_itlb_inval_all FUNCTION
    ldr     r0, =0x0
    mcr     p15, 0, r0, c8, c5, 0
    dsb
    isb
    bx      lr
    ENDFUNC
;============================================================================
; tzbsp_tlb_inval_all_is
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Invalidate the unified TLB inner shared
;============================================================================
tzbsp_tlb_inval_all_is FUNCTION
    ldr     r0, =0x0
    mcr     p15, 0, r0, c8, c3, 0
    dsb
    isb
    bx      lr
    ENDFUNC
;============================================================================
; tzbsp_tlb_inval_by_asid_is
;
; ARGS
;   $asid - passed in r0
;
; DESCRIPTION
;   Invalidate the unified TLB by ASID inner shared
;============================================================================
tzbsp_tlb_inval_by_asid_is FUNCTION
    mcr     p15, 0, r0, c8, c3, 2
    dsb
    isb
    bx      lr
    ENDFUNC
;============================================================================
; tzbsp_tlb_inval_by_mva_is
;
; ARGS
;   $mva - passed in r0
;
; DESCRIPTION
;   Invalidate the unified TLB by MVA inner shared
;============================================================================
tzbsp_tlb_inval_by_mva_is FUNCTION
    mcr     p15, 0, r0, c8, c3, 1
    dsb
    isb
    bx      lr
    ENDFUNC
;============================================================================
; tzbsp_dtlb_inval_all
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Invalidate the entire data TLB.
;============================================================================
tzbsp_dtlb_inval_all FUNCTION
    ldr     r0, =0x0
    mcr     p15, 0, r0, c8, c6, 0
    dsb
    isb
    bx      lr
    ENDFUNC
;============================================================================
; tzbsp_mmu_cache_disable
;
; ARGS
;
;
;  DESCRIPTION -
;        Disables the MMU and cache.
;           1- flushes out cache and write buffer
;           2- invaludates TLB
;           3- disable data and instruction cache
;           4- disables MMU
;        The page table is unaffected
;============================================================================
tzbsp_mmu_cache_disable FUNCTION
    stmfd   sp!, {r0-r12, lr}
    bl      tzbsp_cache_flush
    mov     r0, #0
    mmu_invalidate_tlb
    mmu_disable_i_and_d_cache
    mmu_disable
    ldmfd   sp!, {r0-r12, pc}
    ENDFUNC
;============================================================================
; tzbsp_mmu_cache_enable_no_cfg
;
;  Enables the MMU and caches, without applying any configurations
;
; ARGS
;
;
;  DESCRIPTION -
;        Enables the MMU and cache.
;           1- ICache and branch target invalidate
;           2- TLB invalidate
;           3- Enable D and I caches 
;           4- Enable MMU
;============================================================================
tzbsp_mmu_cache_enable_no_cfg FUNCTION
    push    {r0, lr}
    mov     r0, #0
    mcr     p15, 0, r0, c7, c5, 0   ; ICIALLU instruction & branch
                                    ; target invalidate
    mmu_invalidate_tlb
    ; Enable the caches and MMU
    mmu_enable_i_and_d_cache
    mmu_enable
    isb
    pop     {r0, pc}
    ENDFUNC
;============================================================================
; tzbsp_mmu_cache_re_enable
;
;  DESCRIPTION -
;        re-enables the MMU and cache.
;        The page table is unaffected
;============================================================================
tzbsp_mmu_cache_re_enable FUNCTION
    push    {r0, lr}
    mcr     p15, 0, r0, c7, c5, 0   ; ICIALLU instruction & branch
                                    ; target invalidate
    ldr     r0, =mmu_l1_tt
    bl      mmu_cache_init_p
    mmu_enable_i_and_d_cache
    pop     {r0, pc}
    ENDFUNC
;============================================================================
; mmu_cache_init_p
;  INPUT       - PAGE_TABLE_BASE ADDRESS
;  DESCRIPTION -
;        Initializes the ARM MMU.
;                1- sets a default value in the control register
;                2- sets the FSCE PID register to be flat mapping
;                3- sets the physical address of the page table
;                4- populates the page_table with section entries
;                5- sets the domain access permisions
;                6- flushes out cache and write buffer
;                7- enables the mmu
;============================================================================
mmu_cache_init_p
    push    {r0, lr}
    mov     r1, #0
    mcr     p15, 0, r1, c8, c7, 0       ; UTLBIALL: Invalidate instruction
                                        ; and data TLB
    mov     r1, #(((1 << 5)))
    mmu_set_ttb_0 r1, r0
    mov     r0, #0
    mcr     p15, 0, r0, c13, c0, 0      ; Set FCSEIDR Register to flat mapping
    mmu_set_dom_access #0x5555, #0x5555 ; set all domains for client access
    mmu_enable_tex_remap r0             ; Enable TEX Remapping
    isb
    mmu_enable                          ; Enable MMU
    pop     {r0, pc}
;============================================================================
; TZBSP_MMU_CACHE_INIT
;
;  DESCRIPTION -
;        Initializes the ARM MMU.
;                1- sets a default value in the control register
;                2- sets the physical address of the page table
;                3- populates the page_table with section entries
;                4- sets the domain access permissions
;                5- flushes out cache and write buffer
;                6- then enables the mmu
;                7- then enable data and instruction cache
;============================================================================
tzbsp_mmu_cache_init FUNCTION
    push    {r1, lr}
    bl      tzbsp_init_tts
    cmp     r0, #0
    bne     tzbsp_mmu_cache_init_err_return
    bl      tzbsp_cache_flush
    ldr     r0, =mmu_l1_tt
    bl      mmu_cache_init_p
    mmu_enable_i_and_d_cache
    mov     r0, #0
    pop     {r1, pc}
tzbsp_mmu_cache_init_err_return
    mov     r0, #-1
    pop     {r1, pc}
    ENDFUNC
;============================================================================
; TZBSP_MMU_DISABLE
;
;  DESCRIPTION -
;        Disables the ARM MMU.
;
;  NOTE -
;        This function does not perform any cache maintenance
;============================================================================
tzbsp_mmu_disable FUNCTION
    mmu_disable
    isb
    bx      lr
    ENDFUNC
;============================================================================
; TZBSP_MMU_ENABLE
;
;  DESCRIPTION -
;        Enables the ARM MMU.
;
;  NOTE -
;        This function does not perform any cache maintenance
;============================================================================
tzbsp_mmu_enable FUNCTION
    mmu_enable
    isb
    bx      lr
    ENDFUNC
;============================================================================
; tzbsp_vtop
;
;  ARGUMENTS -
;        uint32 virtual_addr
;        uint32 physical_addr
;
;  DESCRIPTION -
;        Converts input, virtual_addr, to physical address
;
;  NOTE -
;        This function is not interrupt safe
;============================================================================
tzbsp_vtop FUNCTION
    mcr     p15, 0, r0, c7, c8, 0  ; V2PCWPR (privledge current security read)
    isb
    mrc     p15, 0, r2, c7, c4, 0  ; Read PAR
    isb
    ; bit 0 indicates success with 0
    ands    r3, r2, #1
    bne     tzbsp_vtop_err_return
    mov     r3, #0xF000
    movt    r3, #0xFFFF
    and     r2, r3                 ; r2 = r2 & 0xFFFFF000
    mov     r3, #0xFFF
    and     r0, r3                 ; r0 = r0 & 0xFFF
    orr     r0, r2                 ; r0 = r2 | r0
    str     r0, [r1]
    mov     r0, #0                 ; Return E_SUCCESS
    bx      lr
tzbsp_vtop_err_return
    mov     r0, #-1                ; Return -E_FAILURE
    bx      lr
    ENDFUNC
;============================================================================
   END


;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;     T R U S T Z O N E   E N T R Y
;
; GENERAL DESCRIPTION
;   This file contains the TZBSP main entry
;
; EXTERNALIZED SYMBOLS
;   __main
;   _main
;
; INITIALIZATION AND SEQUENCING REQUIREMENTS
;
;
; Copyright (c) 2011-2012 by QUALCOMM Technologies, Incorporated.All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                           EDIT HISTORY FOR FILE
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
; $Header: 
;
; when       who     what, where, why
; --------   ---     --------------------------------------------------------
; 03/28/11   tk      Initial version
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;============================================================================
;
;                            MODULE INCLUDES
;
;============================================================================
; ---------------------
; MACRO: Mov32
; ---------------------
; Moves a 32 bit value into a register
  MACRO
  Mov32 $r, $val_l
    movw    $r, #($val_l & 0xFFFF)
    movt    $r, #(($val_l >> 16) & 0xFFFF)
  MEND
; ---------------------
; MACRO: Read32R
; ---------------------
; Reads a given word where the address is in a register
  MACRO
  Read32R $val_r, $addr_r, $work_r
    ldr     $val_r, [$addr_r, $work_r]
  MEND
; ---------------------
; MACRO: Read32L
; ---------------------
; Reads a given word where the address is a literal
  MACRO
  Read32L $val_r, $addr_l, $work_r
    Mov32   $val_r, $addr_l
    Read32R $val_r, $val_r, $work_r
  MEND
; ---------------------
; MACRO: Write32R
; ---------------------
; Writes a given word where the address is in a register
  MACRO
  Write32R $val_r, $addr_r, $work_r
    str     $val_r, [$addr_r, $work_r]
  MEND
; ---------------------
; MACRO: Write32L
; ---------------------
; Writes a given word where the address is a literal
  MACRO
  Write32L $val_r, $addr_l, $work_r, $addr_r
    Mov32    $addr_r, $addr_l
    Write32R $val_r, $addr_r, $work_r
  MEND
    GET tzbsp_libarm_asm.inc
    GET tzbsp_asm.inc
    GET qctps_common_macros.inc
;============================================================================
;
;                             MODULE DEFINES
;
;============================================================================
;============================================================================
;
;                             MODULE IMPORTS
;
;============================================================================
    ; Import the external symbols that are referenced in this module.
    IMPORT tzbsp_hand_control_to_ns
    IMPORT tzbsp_smc_lock
    IMPORT tzbsp_boot_lock
    IMPORT tzbsp_save_spbl_context
    IMPORT tzbsp_cold_init
    IMPORT tzbsp_cpu_init
    IMPORT tzbsp_loop_here
    IMPORT tzbsp_switch_execute_smc
    IMPORT Workspace_N_Begin
    IMPORT |Image$$TZBSP_STACK$$ZI$$Limit|
    IMPORT |Image$$TZBSP_STACK$$ZI$$Base|
    IMPORT |TZBSP_UNCACHED_ZI_BOOT_STACK$$Limit|
    IMPORT g_tzbsp_entry_counters
    IMPORT g_tzbsp_exit_counters
    IMPORT g_tzbsp_jump_addrs
    IMPORT g_tzbsp_warm_boot
; Not compiled in by default.
;    IMPORT tzbsp_restore_debug_regs
    IMPORT sysini_wrapper
    IMPORT sysini_wrapper_cold_init
    IMPORT tzbsp_secboot_set_shared_info
    IMPORT qsee_undef_handler
    IMPORT qsee_swi_handler
    IMPORT qsee_pabort_handler
    IMPORT qsee_dabort_handler
;============================================================================
;
;                             MODULE EXPORTS
;
;============================================================================
    ; Export the symbols __main and _main to prevent the linker from
    ; including the standard runtime library and startup routine.
    EXPORT   __main
    EXPORT   _main
    EXPORT   tzbsp_has_cpu_cold_booted
    EXPORT   RST_VEC
    EXPORT   tzbsp_cpus_in_lpm
    EXPORT   tzbsp_do_l2_init
    EXPORT   tzbsp_cpu_pc_state
    EXPORT   tzbsp_reset_vector
    EXPORT   tzbsp_shared_imem_slp_ticks_0
    EXPORT   tzbsp_was_l2_pc
    EXPORT   tzbsp_was_gdhs_pc
;============================================================================
;
;                             MODULE DATA AREA
;
;============================================================================
    ;========================================================================
    ; Section TZBSP_VECTOR_TABLE must be defined first in the scatter-load
    ;========================================================================
    PRESERVE8
    CODE32
    AREA    TZBSP_VECTOR_TABLE, CODE, READONLY, ALIGN=16
    ENTRY
__main
_main
; Cold boot entry into TZ
RST_VEC
tzbsp_reset_vector
    ldr     pc, =tzbsp_entry_chooser ; Reset
    ldr     pc, =qsee_undef_handler  ; Undefined instruction
    ldr     pc, =qsee_swi_handler    ; SVC
    ldr     pc, =qsee_pabort_handler ; Prefetch abort
    ldr     pc, =qsee_dabort_handler ; Data abort
    ldr     pc, =tzbsp_loop_here     ; Not used / reserved
    ldr     pc, =tzbsp_loop_here     ; IRQ
    ldr     pc, =tzbsp_loop_here     ; FIQ
tzbsp_entry_chooser
    ldr     r3, =tzbsp_has_cpu_cold_booted ; cpu_cold with no offset is cpu0
    ldr     r3, [r3]
    cmp     r3, #0
    beq     tzbsp_entry_handler  ; Entry from SPBL
    b       tzbsp_reset_handler  ; CPU1 cold boot, or any warm boot entry
;======================================================================
; When the reset handler is invoked we are either doing a secondary CPU cold
; boot or warm booting any of the CPUs. CPU0 cold boot is a special case
; handled in tzbsp_entry_handler. Because this is a real reset handler, we can
; use all available registers.
;======================================================================
tzbsp_reset_handler
    ; ------------------------------------------------------------------
    ; Verify our load address.
    ; ------------------------------------------------------------------
    adr     r0, tzbsp_reset_handler
    ldr     r1, =tzbsp_reset_handler
    cmp     r0, r1
0   bne     %0
    ; Set the warm boot status for each CPU. As this is a per-CPU
    ; variable, this can be done outside the boot lock.
    mrc     p15, 0, r0, c0, c0, 5
    and     r0, r0, #0xFF
    ldr     r1, =g_tzbsp_warm_boot
    mov     r2, #1
    str     r2, [r1, r0, LSL #2]
    ; ------------------------------------------------------------------
    ; Wait until we get exclusive access to boot path. This is a
    ; last line of defense preventing multiple CPUs doing the init
    ; simultaneously.
    ; ------------------------------------------------------------------
    WaitForAccess r0, r1, r2, tzbsp_boot_lock
    ; We got the lock, TZ boot path is now exclusively ours.
    ; Decrement the number of CPUs in low power mode.
    ldr     r0, =tzbsp_cpus_in_lpm
    ldr     r1, [r0]
    sub     r1, r1, #1
    str     r1, [r0]
    ; Update the entry counter.
    mrc     p15, 0, r0, c0, c0, 5       ; Current MPIDR.
    and     r0, r0, #0xFF               ; AFFL0 is the CPU number.
    ldr     r1, =g_tzbsp_entry_counters ; Base of entry counter addresses.
    ldr     r1, [r1, r0, LSL #2]        ; Read the counter address.
    ldr     r2, [r1]                    ; Read the stored counter value.
    add     r2, r2, #1                  ; Increment the counter.
    str     r2, [r1]                    ; Write the new counter value.
    ; Dump the current MPM_SLEEP_TIMETICK_COUNT_VAL to TZ area in Shared
    ; IMEM. This is for power collapse debugging. Note that r0 must contain
    ; the current CPU number.
    ;TODO: Remove once 8974v1 support is dropped (delete tzbsp_shared_imem_slp_ticks_0)
    ;Mov32   r1, ((0xFE805000 + 0x734) + (0x04))
    ldr     r1, =tzbsp_shared_imem_slp_ticks_0
    ldr     r1, [r1]
    add     r1, r0, LSL #2 ; Select this CPU's slot.
    Mov32   r2, 0xfc4a3000 ; MPM_SLEEP_TIMETICK_COUNT_VAL addr
    ldr     r2, [r2]       ; Read the counter value.
    str     r2, [r1]       ; Store it to CPU specific slot in TZ shared IMEM.
    ; -----------------------------------------------------------------------
    ; Set up a stack pointer in Supervisor,Undefined,Abort mode.
    ; -----------------------------------------------------------------------
    ; Set the Stack Pointer address for supervisor mode
    msr     CPSR_c,#TZBSP_Mode_SVC:OR:TZBSP_I_Bit:OR:TZBSP_F_Bit
    ldr     r0, =|TZBSP_UNCACHED_ZI_BOOT_STACK$$Limit|
    mov     r13, r0
    ; Set the Stack Pointer address for undefined mode
    ldr     r0, =tzbsp_abt_stack
    add     r0, #0x400              ; Size is defined below.
    sub     r0, r0, #0x8
    msr     CPSR_c, #TZBSP_Mode_UND:OR:TZBSP_I_Bit:OR:TZBSP_F_Bit
    mov     r13, r0                 ; set Undefined SP
    ; Set the Stack Pointer address for abort mode
    msr     CPSR_c, #TZBSP_Mode_ABT:OR:TZBSP_I_Bit:OR:TZBSP_F_Bit
    mov     r13, r0                 ; set Abort SP
    ; Switch back to Supervisor mode
    msr     CPSR_c, #TZBSP_Mode_SVC:OR:TZBSP_I_Bit:OR:TZBSP_F_Bit
    ; Restore debug registers as early as possible after stack is set
    ; up to help debugging PC entry/exit
;    bl      tzbsp_restore_debug_regs
    ; Figure out if the L2 cache needs to be invalidated and initialized.
    ; TZ will have to do L2 setup only when L2 cache has been power collapsed.
    ; Check if all cpus are going to lpm and the l2 init flag is set.
    mov     r0, #0
    ldr     r1, =tzbsp_cpus_in_lpm
    ldr     r1, [r1]
    cmp     r1, #(4 - 1)  ; Are other CPUs down?
    bne     %f2                         ; Not all CPUs were PC'd skip L2 init.
    ldr     r0, =tzbsp_do_l2_init
    ldr     r0, [r0]                    ; r0 contains L2 init flag for wrapper
    ; SYSINI must be done after the spinlock, because L2 cache common init
    ; is done conditionally. If all cores are released simultaneously from
    ; reset, only the core that first hits the SYSINI will apply the L2
    ; cache common init.
2   blx     sysini_wrapper
    ; At this point, L2 is valid; mark it as such, for SDI
    mov     r1, #0x1
    Mov32   r0, ((0xFE805000 + 0x734) + (0x2C))
    str     r1, [r0]
    ; Save the L2 sysini initialization state for QDSS tracing workaround.
    ldr     r0, =tzbsp_do_l2_init
    ldr     r0, [r0]
    ldr     r1, =tzbsp_was_l2_pc
    str     r0, [r1]
    ; Clear the L2 counter in any case.
    ldr     r0, =tzbsp_do_l2_init
    mov     r1, #0
    str     r1, [r0]
    ldr     r0, =tzbsp_cpu_init
    blx     r0
    ; Update the exit counter (don't corrupt r0).
    mrc     p15, 0, r1, c0, c0, 5       ; Current MPIDR.
    and     r1, r1, #0xFF               ; AFFL0 is the CPU number.
    ldr     r2, =g_tzbsp_exit_counters  ; Base of exit counter addresses.
    ldr     r2, [r2, r1, LSL #2]        ; Read the counter address.
    ldr     r3, [r2]                    ; Read the stored counter value.
    add     r3, r3, #1                  ; Increment the counter.
    str     r3, [r2]                    ; Write the new counter value.
    ; Clear the warm boot status for each CPU. Not that r1 from above needs
    ; to be preserved for this (contains the CPU id).
    ldr     r2, =g_tzbsp_warm_boot
    mov     r3, #0
    str     r3, [r2, r1, LSL #2]
    ; Return value from tzbsp_cpu_init, in r0, contains the HLOS jump address.
    ldr     r1, =tzbsp_hand_control_to_ns
    bx      r1                                  ; Never returns
;============================================================================
; Handles the init routine and main entry point for TZBSP
;
; Scorpion cold boot flow is slightly tainted as TZ is called from SPBL. The
; result is that we have to handle two different reset flows. In cold boot
; case TZ is called as function by SPBL and function calling conventions
; apply (cannot corrupt SPBL registers other than r0-r3). In warm boot the
; reset is coming straight from metal and we can corrupt any register.
;
;   Tasks include:
;   (1) Save necessary context information from caller
;   (2) Initialize the Trustzone monitor
;   (3) Jump to the Trustzone Init handler
;============================================================================
tzbsp_entry_handler
    ; ------------------------------------------------------------------
    ; Start by disabling IRQ and FIQ interrupts, when the boot flows
    ; from SPBL we cannot be sure of IRQ and FIQ masks.
    ; ------------------------------------------------------------------
    msr     CPSR_c,#TZBSP_Mode_SVC:OR:TZBSP_I_Bit:OR:TZBSP_F_Bit
    ; ------------------------------------------------------------------
    ; Save location of secboot shared info, passed by SBL as a pointer,
    ; currently in r0.  If not provided, spin.
    ; ------------------------------------------------------------------
    push    {r14}
    mov     r1, #0
    cmp     r0, r1
null_secboot_ftbl
    beq     null_secboot_ftbl
    ldr     r1, =tzbsp_secboot_set_shared_info
    blx     r1
    pop     {r14}
    ; ------------------------------------------------------------------
    ; Verify our load address.
    ; ------------------------------------------------------------------
    adr     r0, tzbsp_entry_handler
    ldr     r1, =tzbsp_entry_handler
    cmp     r0, r1
invalid_addr_trap
    bne     invalid_addr_trap
    ; ------------------------------------------------------------------
    ; Wait until we get exclusive access to monitor/TZBSP. This is a
    ; last line of defense preventing multiple CPUs doing the init
    ; simultaneously.
    ; ------------------------------------------------------------------
    WaitForAccess r0, r1, r2, tzbsp_smc_lock
    ; We got the lock, TZ is now exclusively ours.
    ; ------------------------------------------------------------------
    ; Build the non secure context using the secure context.
    ; Parameters: r0 - return address of non secure world
    ; Parameters: r1 - sp for supervisor mode of non-secure world
    ; ------------------------------------------------------------------
    mov     r0, r14
    mov     r1, r13
    ldr     r2, =tzbsp_save_spbl_context
    blx     r2
    ; SPBL context is saved, we can corrupt any register now.
    ; Update CPU0 boot status.
    ldr     r0, =tzbsp_has_cpu_cold_booted
    mov     r1, #1                  ; Cold boot done
    str     r1, [r0]                ; Save boot status
    ; -----------------------------------------------------------------------
    ; Set up a stack pointer in Supervisor, Undefined, Abort mode.
    ; -----------------------------------------------------------------------
    ; Set the Stack Pointer address for supervisor mode
    ldr     r0, =|Image$$TZBSP_STACK$$ZI$$Limit|
    mov     r13, r0
    ; Set the Stack Pointer address for undefined mode
    ldr     r0, =tzbsp_abt_stack
    add     r0, #0x400              ; Size is defined below.
    sub     r0, r0, #0x8
    msr     CPSR_c, #TZBSP_Mode_UND:OR:TZBSP_I_Bit:OR:TZBSP_F_Bit
    mov     r13, r0                 ; set Undefined SP
    ; Set the Stack Pointer address for abort mode
    msr     CPSR_c, #TZBSP_Mode_ABT:OR:TZBSP_I_Bit:OR:TZBSP_F_Bit
    mov     r13, r0                 ; set Abort SP
    ; Switch back to Supervisor mode
    msr     CPSR_c, #TZBSP_Mode_SVC:OR:TZBSP_I_Bit:OR:TZBSP_F_Bit
    ; ------------------------------------------------------------------
    ; Setup the Vector Base Address Register (VBAR) to our vectors
    ; ------------------------------------------------------------------
    ldr     r0, =tzbsp_reset_vector
    mcr     p15, 0, r0, c12, c0, 0
    ; Force flush/refetch of prefetch pipeline
    misb
    blx     sysini_wrapper_cold_init
    ; Jump to TZBSP C initializer
    ldr     r0, =tzbsp_cold_init
    blx     r0
    ; CPUs 1,2,3,...,n are initially in low power mode, hence the
    ; initial value of n - 1.  The uncached region is zero-initialized
    ; in cold_init, so set the proper value here
    ldr     r0, =tzbsp_cpus_in_lpm
    mov     r1, #(4 - 1)
    str     r1, [r0]
    ; Everything is done, jump to TZBSP message loop (it will
    ; SMC to non-secure side).
    ldr     r0, =tzbsp_switch_execute_smc ; Start the TZBSP message loop
    bx      r0                            ; Never returns
;======================================================================
; TZBSP data area
;======================================================================
    AREA    TZBSP_DATA, DATA, READWRITE
tzbsp_abt_stack                  SPACE  0x400
tzbsp_shared_imem_slp_ticks_0    DCD    ((0xFE805000 + 0x734) + (0x04))
; Uncached data is for items that are read/written both when the MMU
; is on and off.  This area is enforced ZI by memset in init.
    AREA    TZBSP_UNCACHED_ZI_PC_DATA, DATA, READWRITE, ALIGN=2
; Array of values, nonzero indicating CPU has not booted. This needs to be 
; 4-Byte Aligned
tzbsp_has_cpu_cold_booted        SPACE   (4 * 4)
tzbsp_cpus_in_lpm                DCD     0x0 ; TZBSP tracks PCed CPU count
tzbsp_do_l2_init                 DCD     0x0 ; saved HLOS flag from PC
tzbsp_was_l2_pc                  DCD     0x0 ; L2 power collapse status copy.
tzbsp_was_gdhs_pc                DCD     0x0 ; L2 GDHS collapse status copy.
tzbsp_cpu_pc_state               SPACE   4
; Boot stack is place do uncached memory as it is being used before and after
; MMU enablement.
    AREA   TZBSP_UNCACHED_ZI_BOOT_STACK, DATA, READWRITE, ALIGN=3
tzbsp_boot_stack                 SPACE   0x200
    END

;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;     T R U S T Z O N E   A S M
;
; GENERAL DESCRIPTION
;   Assembly functions
;
; Copyright (c) 2011 by QUALCOMM Technologies, Incorporated.All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                           EDIT HISTORY FOR FILE
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
; $Header: 
;
; when       who     what, where, why
; --------   ---     --------------------------------------------------------
; 03/28/11   tk      Initial version
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;============================================================================
;
;                            MODULE INCLUDES
;
;============================================================================
    GET tzbsp_libarm_asm.inc
    GET tzbsp_asm.inc
;============================================================================
;
;                             MODULE DEFINES
;
;============================================================================
;============================================================================
;
;                             MODULE IMPORTS
;
;============================================================================
    ; Import the external symbols that are referenced in this module.
    IMPORT tzbsp_syscall
    IMPORT Workspace_N_Begin
    IMPORT |Image$$TZBSP_STACK$$ZI$$Limit|
    IMPORT |Image$$TZBSP_STACK$$ZI$$Base|
    IMPORT tzbsp_dcache_flush_all
;============================================================================
;
;                             MODULE EXPORTS
;
;============================================================================
    EXPORT tzbsp_loop_here
    EXPORT tzbsp_switch_execute_smc
;============================================================================
;
;                             MODULE DATA AREA
;
;============================================================================
    PRESERVE8
    CODE32
    AREA    TZBSP_ASM, CODE, READONLY, align=2
;======================================================================
; Called by tzbsp_error_hanlder only. We perform
; Clean up the registers and loop here until JTAG is connected.
;======================================================================
tzbsp_loop_here FUNCTION
    mov r0,#0
    mov r1,#0
    mov r2,#0
    mov r3,#0
    mov r4,#0
    mov r5,#0
    mov r6,#0
    mov r7,#0
    mov r8,#0
    mov r9,#0
    mov r10,#0
    mov r11,#0
    mov r12,#0
1   b   %b1
    ENDFUNC
;======================================================================
; The main message loop. Each incoming syscall will be handled by this
; loop.
;======================================================================
tzbsp_switch_execute_smc FUNCTION
    SMI  0x0                    ; After init just go to non-secure side.
1   ldr  r0,=Workspace_N_Begin
    blx  tzbsp_syscall          ; Every subsequent call is to msg processing.
    SMI  0x0                    ; Msg handled, return to non-secure side.
    b    %b1                    ; Back to secure side, loop to msg processing.
    ENDFUNC
; This area is used currently by DAL for heap.  It is defined here
; rather than the DAL layer so we can use the base address in our SCL
; file and MMU pagetable configurations
    AREA    TZBSP_HEAP_AREA, DATA, READWRITE, align=12
tzbsp_heap    % 0x3000
    END


;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;     T R U S T Z O N E   W R A P P E R
;
; GENERAL DESCRIPTION
;   This file contains the TZBSP main entry
;
; EXTERNALIZED SYMBOLS
;   __main
;   _main
;
; INITIALIZATION AND SEQUENCING REQUIREMENTS
;
;
; Copyright (c) 2011-2012 by Qualcomm Technologies, Incorporated.All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                           EDIT HISTORY FOR FILE
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
; $Header: 
;
; when        who        what, where, why
; --------   ------     --------------------------------------------------------
; 12/21/12   ah         Initial version
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
; ---------------------
; MACRO: Mov32
; ---------------------
; Moves a 32 bit value into a register
  MACRO
  Mov32 $r, $val_l
    movw    $r, #($val_l & 0xFFFF)
    movt    $r, #(($val_l >> 16) & 0xFFFF)
  MEND
; ---------------------
; MACRO: Read32R
; ---------------------
; Reads a given word where the address is in a register
  MACRO
  Read32R $val_r, $addr_r, $work_r
    ldr     $val_r, [$addr_r, $work_r]
  MEND
; ---------------------
; MACRO: Read32L
; ---------------------
; Reads a given word where the address is a literal
  MACRO
  Read32L $val_r, $addr_l, $work_r
    Mov32   $val_r, $addr_l
    Read32R $val_r, $val_r, $work_r
  MEND
; ---------------------
; MACRO: Write32R
; ---------------------
; Writes a given word where the address is in a register
  MACRO
  Write32R $val_r, $addr_r, $work_r
    str     $val_r, [$addr_r, $work_r]
  MEND
; ---------------------
; MACRO: Write32L
; ---------------------
; Writes a given word where the address is a literal
  MACRO
  Write32L $val_r, $addr_l, $work_r, $addr_r
    Mov32    $addr_r, $addr_l
    Write32R $val_r, $addr_r, $work_r
  MEND
;============================================================================
;
;                            MODULE INCLUDES
;
;============================================================================
    GET qctps_common_macros.inc
;============================================================================
;
;                             MODULE IMPORTS
;
;============================================================================
    IMPORT a7_sysini
    IMPORT L2_INIT_SEMAPHORE_LOC
    IMPORT tzbsp_diable_l2_as_tcm
    IMPORT tzbsp_hyp_regs_init
    IMPORT tzbsp_do_l2_init
    IMPORT tzbsp_was_gdhs_pc
    IMPORT tzbsp_dcache_clean_inv_lx
    IMPORT tzbsp_dcache_flush_all
    IMPORT tzbsp_ftr_cfg_fuse
    IMPORT tzbsp_wa_after
    IMPORT tzbsp_chip_is_dino_v1
;============================================================================
;
;                             MODULE EXPORTS
;
;============================================================================
    EXPORT sysini_wrapper
    EXPORT sysini_wrapper_cold_init
    EXPORT tzbsp_reset_vtimer_cntvoff
    EXPORT tzbsp_cpu_power_down_sequence
    EXPORT tzbsp_pc_fall_through
    EXPORT l2_shmoo_hole_workaround
    EXPORT sysini_pre_warm_boot
    EXPORT tzbsp_cpu_invalidate_done_flag
; SCR (Secure Configuration Register) bits
SCR_NS_BIT                  EQU     0x01    ; Non-Secure (NS) bit
SCR_IRQ_BIT                 EQU     0x02    ; IRQ bit
SCR_FIQ_BIT                 EQU     0x04    ; FIQ bit
SCR_EA_BIT                  EQU     0x08    ; EA bit
SCR_FW_BIT                  EQU     0x10    ; F Bit writable (FW) bit
SCR_AW_BIT                  EQU     0x20    ; A Bit writable (AW) bit
; CPSR Processing Modes M[4:0] (CPSR[M]) bits
CPSR_MODE_USR               EQU     0x10
CPSR_MODE_FIQ               EQU     0x11
CPSR_MODE_IRQ               EQU     0x12
CPSR_MODE_SVC               EQU     0x13
CPSR_MODE_MON               EQU     0x16
CPSR_MODE_ABT               EQU     0x17
CPSR_MODE_UND               EQU     0x1b
CPSR_MODE_SYS               EQU     0x1f
CPSR_MODE_MASK              EQU     0x1f
QFPROM_CORR_FEAT_CONFIG_ROW0_MSB EQU 0xFC4BC104
;------------------------------------------------------------------------------
    PRESERVE8
    CODE32
    AREA wrapper, CODE
;------------------------------------------------------------------------------
  MACRO
    switch_to_ns_mode $cpyreg, $tmp
    mrc     p15, 0, $cpyreg, c1, c1, 0 ; Read the current SCR
    orr     $tmp, $cpyreg, #SCR_NS_BIT ; Set SCR.NS=1
    mcr     p15, 0, $tmp, c1, c1, 0    ; write SCR
  MEND
  MACRO
    restore_cpsr_mode $reg 
    mcr     p15, 0, $reg, c1, c1, 0    ; write SCR
  MEND
; void sysini_wrapper(uint32 inval)
; Arguments:
;  r0: 1 to invalidate L2 cache, 0 to not invalidate L2 cache.


sysini_wrapper FUNCTION
    push    {r12, lr}
       push    {r0}
    blx     tzbsp_diable_l2_as_tcm
    blx     tzbsp_hyp_regs_init ; Init CP15 VTimer Regs if needed
    pop     {r0}
    cmp     r0, #1      ; Check L2 invalidation argument.
    bne     %f1         ; Skip L2 invalidation if argument not set.
    ; Set up the L2 semaphore to do the full L2 init.
    mrc     p15, 0, r0, c0, c0, 5       ; Current MPIDR.
    and     r0, r0, #0xFF               ; AFFL0 is the CPU number.
    mov     r1, #1
    lsl     r1, r1, r0                  ; Set the current CPU bit high.
    ldr     r0, =L2_INIT_SEMAPHORE_LOC  ; L2 semaphore address.
    str     r1, [r0]                    ; Allow this CPU to do full L2 i
1   blx     a7_sysini
    pop     {r12, pc}
    ENDFUNC


; This function applies A7 Errata 802022. 
; This is called from primary and secondary cpus both.
; This routine can use all GP registers for scratch calc
; CPU MMU and caches disabled already
; Arguments:
;   r0 - 1 indicates secondary, 0 indicates primary
;   r1 - indicates the SGI number that should be used
sysini_pre_warm_boot FUNCTION
    cmp r0, #0x0 
    bne errata_secondary_cores
    ldr     r0, =tzbsp_cpu_invalidate_done_flag
    mov     r2, #0x0    
    ;str     r2, [r0, #0x0]                     ; Clear invalidate done
    ;str     r2, [r0, #0x4]                     ; Clear invalidate done
    ;str     r2, [r0, #0x8]                     ; Clear invalidate done
    ;str     r2, [r0, #0xC]                     ; Clear invalidate done
    mov     r2, #0x1
    mrc     p15, 0, r3, c0, c0, 5       ; Current MPIDR.
    and     r3, r3, #0xFF               ; AFFL0 is the CPU number.
    str     r2, [r0, r3, LSL #2]        ; Set self invalidate done flag
    DSB
    movw    r0, #0x0000                 ; secure interrupt
    movt    r0, #0x0100                 ; target to others
    orr     r0, r1, r0                  ; construct SGIR using number
    movw    r2, #0x0F00
    movt    r2, #0xF900                    ; Base Address of SGIR0
    str     r0, [r2]                      ; Write correct SGIR  
    ldr     r2, =tzbsp_cpu_invalidate_done_flag
    DSB
wfe_loop
    wfe
    ldr     r1, [r2, #0x0]        ; Check all invalidate done flags
    cmp     r1, #0x1
    bne     wfe_loop
    ldr     r1, [r2, #0x4]        ; Check all invalidate done flags
    cmp     r1, #0x1
    bne     wfe_loop
    ; Check Dual Core vs. Quad Core chip
    ldr     r1, =tzbsp_ftr_cfg_fuse
    ldr     r1, [r1]
    tst     r1, #(1<<20)           ; if set, Dual Core
    bne     %f1
    ldr     r1, [r2, #0x8]        ; Check all invalidate done flags
    cmp     r1, #0x1
    bne     wfe_loop
    ldr     r1, [r2, #0xC]        ; Check all invalidate done flags
    cmp     r1, #0x1
    bne     wfe_loop
1   DSB
    bx r14
errata_secondary_cores
    mov     r0, #0x0
    TLBIMVAIS  r0                       ; Any address. use 0x0
    DSB
    lsr     r0, r1, #0x2                ; get sgi Pend register number
    lsl     r0, r0, #0x2
    and     r4,  r1, #0x3              ; get
    lsl     r4, r4, #0x3
    mrc     p15, 0, r5, c0, c0, 5       ; Current MPIDR.
    and     r5, r5, #0xFF               ; AFFL0 is the CPU number.
    ;add     r4, r4, r5                  ; got the correct bit pos
    mov     r6, #0xF
    lsl     r6, r6, r4                  ;got the mask to ICPENDSGI
    movw    r2, #0x0F10
    movt    r2, #0xF900               ; Base Address od ICPENDSGI0
    str     r6, [r2, r0]              ; Write correct ICPENDSGI
    DSB     
    mrc     p15, 0, r0, c0, c0, 5       ; Current MPIDR.
    and     r0, r0, #0xFF               ; AFFL0 is the CPU number.
    ldr     r2, =tzbsp_cpu_invalidate_done_flag
    mov     r3, #0x1    
    str     r3, [r2, r0, LSL #2]        ; Set invalidate done flag
    mrc     p15, 0, r0, c0, c0, 5       ; Current MPIDR.
    and     r0, r0, #0xFF               ; AFFL0 is the CPU number.
    ldr     r2, =tzbsp_wa_after
    ldr     r3, [r2, r0, LSL #2]        ; Set 
    add     r3, #0x1
    str     r3, [r2, r0, LSL #2]        ; Set 
    DSB
    SEV
    DSB
    ISB
    wfi
errata_wfi_fall_through     
    b       errata_wfi_fall_through    ; never happen? or just return? 
    ENDFUNC
sysini_wrapper_cold_init FUNCTION
    ;Cold Init being called; Unlock Cache lines in milestone
    ldr     r0, =tzbsp_l2cc_unlock
    mov     r1, #0x1 ;store value in l2cc unlock flag.
    str     r1, [r0]
    ; Read QFPROM Feature Config fuse
    MOV32   r0, #QFPROM_CORR_FEAT_CONFIG_ROW0_MSB
    ldr     r0, [r0]
    ldr     r1, =tzbsp_ftr_cfg_fuse
    str     r0, [r1]
    bx      lr
    ENDFUNC
tzbsp_reset_vtimer_cntvoff FUNCTION
    push    {r0-r3, lr}
    cps     #CPSR_MODE_MON              ; Switch to Monitor Mode
    mrc     p15, 0, r2, c1, c1, 0       ; Read SCR
    mov     r0, #SCR_FW_BIT:OR:SCR_AW_BIT:OR:SCR_NS_BIT
    mcr     p15, 0, r0, c1, c1, 0       ; Switch to NS
    mov     r0, #0
    mov     r1, #0
    MCRR    p15, 4, r0, r1, c14         ; set CNTVOFF = 0  
    mcr     p15, 0, r2, c1, c1, 0       ; Restore secure mode
    cps     #CPSR_MODE_SVC              ; Restore SVC mode
    pop     {r0-r3, pc}
    ENDFUNC
;=======================================================================
; l2_shmoo_hole_workaround
;
;  DESCRIPTION - The very first write into L2 cache line (upper or lower 
;                bank) leaves corrupted data in the cache line when L2 
;                is powered up (Wake from L2 collapse sleep).
;                Reading directly into L2 cache lines after L2 power 
;                collapse will fix this issue (AKA. Shmoo hole).
;=======================================================================
l2_shmoo_hole_workaround FUNCTION
    push    {r0-r3, lr}
    ; only applicable to 8x10 v1
    bl      tzbsp_chip_is_dino_v1
    cmp     r0, #1
    bne     l2_ret
    ; Check for L2 power collapse flag or GDHS L2 Collapse flag
    ldr     r0, =tzbsp_do_l2_init
    ldr     r0, [r0]
    ldr     r1, =tzbsp_was_gdhs_pc
    ldr     r2, [r1]
    orr     r0, r0, r2
    cmp     r0, #1
    bne     l2_ret
    ; Clear GDHS L2 Collapse Flag
    mov     r0, #0
    str     r0, [r1]
    ; Enable L2 as TCM
    MOV32   r0, #0xF9010010        ; APCS_TCM_START_ADDR
    MOV32   r1, #0xF8000000        ; TCM_START_ADDR
    orr     r1, r1, #1             ; Set TCM_REDIRECT_EN_0
    str     r1, [r0]
    bic     r1, r1, #1             ; TCM_START_ADDR (Lower 512KB)
    mov     r2, #0x40              ; Cache Line Size (64B)
l2_loop_lower_bank
    ldr     r3, [r1]               ; Read into L2 Lower Bank
    add     r1, r1, #0x4           ; Advance lower bank pointer
    subs    r2, r2, #0x4           ; Cache line counter
    bne     l2_loop_lower_bank
    ; Make sure L2 upper bank is enable
    ldr     r1, =tzbsp_ftr_cfg_fuse
    ldr     r1, [r1]
    tst     r1, #(1<<21)           ; if set, upper bank disable
    bne     l2_disable_tcm
    MOV32   r1, #0xF8080000        ; TCM_START_ADDR (Upper 512KB)
    orr     r1, r1, #1             ; Set TCM_REDIRECT_EN_0
    str     r1, [r0]
    bic     r1, r1, #1             ; TCM_START_ADDR (Upper 512KB)
    mov     r2, #0x40              ; Cache Line Size (64B)
l2_loop_upper_bank
    ldr     r3, [r1]               ; Read into L2 Upper Bank
    add     r1, r1, #0x4           ; Advance upper bank pointer
    subs    r2, r2, #0x4           ; Cache line counter
    bne     l2_loop_upper_bank
l2_disable_tcm  
    ; Disable L2 as TCM
    MOV32   r1, #0                 ; Disable L2 TCM mode
    str     r1, [r0]
l2_ret
    pop     {r0-r3, pc}
    ENDFUNC
;=======================================================================
; tzbsp_cpu_power_down_sequence
;
;  DESCRIPTION - Power Down Sequence
;  1. Clear SCTLR.C
;  2. Clean & invalidate sec. & non-sec. L1 D-Cache
;  3. Clean & invalidate sec. & non-sec. L2 Cache if L2 is collapsing
;  4. Execute CLREX instruction
;  5. Clear ACTLR.SMP (switch to AMP mode)
;  6. Execute ISB
;  7. Execute DSB
; 
;=======================================================================
tzbsp_cpu_power_down_sequence FUNCTION
    push    {r0-r3, lr}
    DSB
    ; 1. Clear SCTLR.C & SCTLR.I 
    mrc     p15, 0, r1, c1, c0, 0   ; Read SCTLR, Control register
    bic     r1, r1, #(1 << 2)       ; C bit - Disable D-Cache
    mcr     p15, 0, r1, c1, c0, 0   ; Write SCTLR, Control register
    ISB
    switch_to_ns_mode r2, r3    
    DSB
    mrc     p15, 0, r1, c1, c0, 0   ; Read SCTLR, Control register
    bic     r1, r1, #(1 << 2)       ; C bit - Disable D-Cache
    mcr     p15, 0, r1, c1, c0, 0   ; Write SCTLR, Control register
    ISB
    restore_cpsr_mode r2
    ; Cortex A7 Errata - 794322
    mov r0, #0x0
    TLBIMVA r0
    DSB
    ; Check for L2 power collapse flag
    ldr     r0, =tzbsp_do_l2_init
    ldr     r1, [r0]
    cmp     r1, #1
       beq     l2_collapse
    ; 2. Clean & Invalidate L1 D-Cache only if L2 is not collapsing
    mov     r0, #0                  ; L1 cache
    blx     tzbsp_dcache_clean_inv_lx
       b       bp1
l2_collapse 
    ; 3. Clean & Invalidate Secure L1 & L2 D-Cache if L2 is collapsing
       blx     tzbsp_dcache_flush_all
bp1
    DSB
    ; 4. Execute CLREX instruction
    clrex
    ; 5. Clear ACTLR.SMP (switch to AMP mode)
    mrc     p15, 0, r0, c1, c0, 1
    bic     r0, r0, #(1 << 6)       ; Clear SMP Bit
    mcr     p15, 0, r0, c1, c0, 1
    ; 6. Execute ISB
    ISB
    ; 7. Execute DSB
    DSB
    pop     {r0-r3, pc}
    ENDFUNC
;=======================================================================
; tzbsp_pc_fall_through
;
;  DESCRIPTION - Power Collapse Fall Through Sequence
;  1. Set ACTLR.SMP (switch to SMP mode)
;  2. Set SCTLR.C (Enable D-Cache)
;  3. Execute ISB
;  4. Execute DSB
; 
;  NOTE: You must ensure the ACTLR.SMP bit is set to 1 before 
;        the caches and MMU or any cache and TLB maintenance operations
;        are performed. The only time this bit is set to 0 is during a
;        processor power-down sequence.
;=======================================================================
tzbsp_pc_fall_through FUNCTION
    push    {r0-r1, lr}
    ; Make sure, cache and MMU are disable
    mrc     p15, 0, r1, c1, c0, 0   ; Read SCTLR, Control register
    mov     r0, r1                  ; keep a copy
    bic     r1, r1, #0x5            ; Clear M & C bits
    mcr     p15, 0, r1, c1, c0, 0   ; Write SCTLR, Control register
    ; 1. Set ACTLR.SMP (switch to SMP mode)
    mrc     p15, 0, r1, c1, c0, 1
    orr     r1, r1, #(1 << 6)       ; Set SMP Bit
    mcr     p15, 0, r1, c1, c0, 1
    ISB
    DSB
    ; 2. Set SCTLR.C (Enable D-Cache)
    orr     r0, #(1 << 2)           ; set C bit
    mcr     p15, 0, r0, c1, c0, 0   ; Write SCTLR, Control register
    switch_to_ns_mode r2, r3    
    mrc     p15, 0, r0, c1, c0, 0   ; Read SCTLR, Control register
    orr     r0, #(1 << 2)           ; set C bit
    mcr     p15, 0, r0, c1, c0, 0   ; Write SCTLR, Control register
    restore_cpsr_mode r2
    ; 3. Execute ISB
    ISB
    ; 4. Execute DSB
    DSB
    ; Invalidate TLB
    MOV   r0, #0
    MCR   p15, 0, r0, c8, c7, 0
    ISB
    DSB
    ; Invalidate Instruction cache
    ; MOV   r0, #0
    ; MCR   p15, 0, r0, c7, c5, 0
    ISB
    DSB
    pop     {r0-r1, pc}
    ENDFUNC
;======================================================================
; TZBSP data area
;======================================================================
    AREA    TZBSP_9k_DATA, DATA, READWRITE
tzbsp_l2cc_unlock                DCD     0x0 ; should TZ unlock L2CC-310?
tzbsp_cpu_invalidate_done_flag   SPACE   (4 * 4)
    END

    ;============================================================================
;
;             Scorpion architecture assembly language MMU support
;
; GENERAL DESCRIPTION
;   This file contains the Scorpion hierarchichal cache and mmu control functions.
;   File originaly taken from: 
;
; Copyright 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved
;============================================================================
;============================================================================
;
;                       EDIT HISTORY FOR MODULE
;
; $Header: 
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
;
; when      who     what, where, why
; --------  ---     ---------------------------------------------------------
; 01/25/11  pre     Initial Version
;============================================================================
;============================================================================
;                             MODULE DEFINES
;============================================================================
;============================================================================
;                             MODULE INCLUDES
;============================================================================
    GET tzbsp_libarm_asm.inc
;============================================================================
;                             MODULE IMPORTS
;============================================================================
;============================================================================
;                             MODULE EXPORTS
;============================================================================
    EXPORT  tzbsp_cache_flush
    EXPORT  tzbsp_dcache_enable
    EXPORT  tzbsp_dcache_disable
    EXPORT  tzbsp_dcache_is_enabled
    EXPORT  tzbsp_dcache_clean_all
    EXPORT  tzbsp_dcache_inval_all
    EXPORT  tzbsp_dcache_flush_all
    EXPORT  tzbsp_dcache_clean_region
    EXPORT  tzbsp_dcache_inval_region
    EXPORT  tzbsp_dcache_flush_region
    EXPORT  tzbsp_dcache_inval_l1
    EXPORT  tzbsp_dcache_inval_l2
    EXPORT  tzbsp_dcache_clean_lx
    EXPORT  tzbsp_dcache_clean_inv_lx
    EXPORT  tzbsp_icache_enable
    EXPORT  tzbsp_icache_disable
    EXPORT  tzbsp_icache_is_enabled
    EXPORT  tzbsp_icache_inv
    EXPORT  tzbsp_icache_inv_area
;============================================================================
;
;                        MACROS AND FUNCTIONS
;
;============================================================================
   AREA    ARM_CACHE, CODE, READONLY
   PRESERVE8
   CODE32
;============================================================================
; MACRO dcache_granule_size
;
; ARGS
;   $reg - cache granule size in bytes
;   $tmp - cache granule size in words used for calculation
;          of the size in bytes
;
; DESCRIPTION
;   cache_granule_size - get the size of the smallest dcache/unified
;   cache (available on ARMv7+). Note that the value of $reg and $tmp
;   will be modified.
;============================================================================
  MACRO
    dcache_granule_size $reg, $tmp
    mrc     p15, 0, $tmp, c0, c0, 1 ; read CTR
    lsr     $tmp, #16
    and     $tmp, #0xF              ; $tmp = CTR[DminLine]
    mov     $reg, #4                ; CTR[DminLine] = log2 num words
    lsl     $reg, $reg, $tmp        ; cache writeback granule size in bytes
  MEND
; void tzbsp_dcache_enable(void)
tzbsp_dcache_enable FUNCTION
    mdsb
    misb
    mrc     p15, 0, r0, c1, c0, 0   ; Read SCTLR.
    orr     r0, r0, #(1 << 2)       ; SCTLR[C]=1.
    mcr     p15, 0, r0, c1, c0, 0   ; Write SCTLR.
    mdsb
    misb
    bx      lr
    ENDFUNC
; void tzbsp_dcache_disable(void)
tzbsp_dcache_disable FUNCTION
    mdsb
    misb
    mrc     p15, 0, r0, c1, c0, 0   ; Read SCTLR.
    and     r0, r0, #~(1 << 2)      ; SCTLR[C]=0.
    mcr     p15, 0, r0, c1, c0, 0   ; Write SCTLR.
    mdsb
    misb
    bx      lr
    ENDFUNC
; boolean tzbsp_dcache_is_enabled(void)
tzbsp_dcache_is_enabled FUNCTION
    misb
    mrc     p15, 0, r0, c1, c0, 0   ; Read SCTLR.
    and     r0, r0, #(1 << 2)       ; Extract SCTLR[C].
    lsr     r0, r0, #2              ; Move bit to position 0.
    bx      lr
    ENDFUNC
;============================================================================
; tzbsp_dcache_iterate_set_way
;
; DESCRIPTION -
;   Iterates through the set/way and calls function passed in
;============================================================================
tzbsp_dcache_iterate_set_way FUNCTION
    stmfd   sp!, {r0-r12, lr}
    mov     r6, r0
    DSB                             ; ensure ordering with previous
                                    ; memory  accesses
    mrc     p15, 1, r11, c0, c0, 1  ; read clidr
    ands    r3, r11, #0x7000000     ; extract loc from clidr
    mov     r3, r3, lsr #23         ; left align loc bit field
    beq     iterate_finished        ; if loc is 0, then no need to clean
    mov     r10, #0                 ; start clean at cache level 0
iterate_loop1
    add     r2, r10, r10, lsr #1    ; work out 3x current cache level
    mov     r1, r11, lsr r2         ; extract cache type bits from clidr
    and     r1, r1, #7              ; mask of the bits for current cache only
    cmp     r1, #2                  ; see what cache we have at this level
    blt     iterate_skip            ; skip if no cache, or just i-cache
    mcr     p15, 2, r10, c0, c0, 0  ; select current cache level in cssr
    ISB                             ; isb to sych the new cssr&csidr
    mrc     p15, 1, r1, c0, c0, 0   ; read the new csidr
    and     r2, r1, #7              ; extract the length of the cache lines
    add     r2, r2, #4              ; add 4 (line length offset)
    ldr     r4, =0x3ff
    ands    r4, r4, r1, lsr #3      ; find maximum number on the way size
    clz     r5, r4                  ; find bit position of way size increment
    ldr     r7, =0x7fff
    ands    r7, r7, r1, lsr #13     ; extract max number of the index size
iterate_loop2
    mov     r9, r4                  ; create working copy of max way size
iterate_loop3
    orr     r0, r10, r9, lsl r5     ; factor way and cache number into r0
    orr     r0, r0, r7, lsl r2      ; factor index number into r0
    blx     r6                      ; Jump to function for given cache line
    subs    r9, r9, #1              ; decrement the way
    bge     iterate_loop3
    subs    r7, r7, #1              ; decrement the index
    bge     iterate_loop2
iterate_skip
    add     r10, r10, #2            ; increment cache number
    cmp     r3, r10
    bgt     iterate_loop1
iterate_finished
    mov     r10, #0                 ; swith back to cache level 0
    mcr     p15, 2, r10, c0, c0, 0  ; select current cache level in cssr
    DSB
    ISB
    ldmfd   sp!, {r0-r12, pc}
    ENDFUNC
;============================================================================
; tzbsp_cache_flush
;
; DESCRIPTION -
;  Flush the entire cache system.  The data cache flush is achieved
;  using atomic clean / invalidates working outwards from L1
;  cache. This is done using Set/Way based cache maintainance
;  instructions.  The instruction cache can still be invalidated back
;  to the point of unification in a single instruction.  This also
;  flushes the branch target cache.
;============================================================================
tzbsp_cache_flush FUNCTION
    push    {lr}
    bl      tzbsp_dcache_flush_all
    mov     r0, #0
    mcr     p15, 0, r0, c7, c5, 0   ; ICIALLU: instruction & branch target
                                    ; invalidate
    ldmfd   sp!, {pc}
    ENDFUNC
;============================================================================
; tzbsp_dcache_clean_line
;
; DESCRIPTION -
;   Flush a single cache line by set/way
;============================================================================
tzbsp_dcache_clean_line FUNCTION
    mcr     p15, 0, r0, c7, c10, 2 ; clean by set/way
    bx      lr
    ENDFUNC
;============================================================================
; tzbsp_dcache_clean_all
;
; DESCRIPTION -
;   Clean the whole D-cache.
;============================================================================
tzbsp_dcache_clean_all FUNCTION
    push    {lr}
    DSB
    ldr     r0, =tzbsp_dcache_clean_line
    bl      tzbsp_dcache_iterate_set_way
    ldmfd   sp!, {pc}
    ENDFUNC
;============================================================================
; tzbsp_dcache_flush_line
;
; DESCRIPTION -
;   Flush a single cache line by set/way
;============================================================================
tzbsp_dcache_flush_line FUNCTION
    mcr     p15, 0, r0, c7, c14, 2 ; clean & invalidate by set/way
    bx      lr
    ENDFUNC
;============================================================================
; tzbsp_dcache_flush_all
;
; DESCRIPTION -
;   Flush the whole D-cache.
;============================================================================
tzbsp_dcache_flush_all FUNCTION
    push    {lr}
    DSB
    ldr     r0, =tzbsp_dcache_flush_line
    bl      tzbsp_dcache_iterate_set_way
    ldmfd   sp!, {pc}
    ENDFUNC
;============================================================================
; tzbsp_dcache_inval_line
;
; DESCRIPTION -
;   Invalidate a single cache line by set/way
;============================================================================
tzbsp_dcache_inval_line FUNCTION
    mcr     p15, 0, r0, c7, c6, 2  ; invalidate by set/way
    bx      lr
    ENDFUNC
;============================================================================
; tzbsp_dcache_inval_all
;
; DESCRIPTION -
;   Invalidate the whole D-cache.
;============================================================================
tzbsp_dcache_inval_all FUNCTION
    push    {lr}
    DSB
    ldr     r0, =tzbsp_dcache_inval_line
    bl      tzbsp_dcache_iterate_set_way
    ldmfd   sp!, {pc}
    ENDFUNC
;============================================================================
; tzbsp_dcache_inval_l1
;
; DESCRIPTION -
;   Invalidate the l1 section of the D-cache.
;============================================================================
tzbsp_dcache_inval_l1 FUNCTION
    DSB
    stmfd   sp!, {r0-r9}
    mrc     p15, 1, r6, c0, c0, 1   ; read clidr
    ands    r3, r6, #0x7000000      ; extract loc from clidr
    mov     r3, r3, lsr #23         ; left align loc bit field
    beq     il1_finished            ; if loc is 0, then no need to clean
    mov     r8, #0                  ; start clean at cache level 0
    add     r2, r8, r8, lsl #1      ; work out 3x current cache level
    mov     r1, r6, lsr r2          ; extract cache type bits from clidr
    and     r1, r1, #7              ; mask of the bits for current cache only
    cmp     r1, #2                  ; see what cache we have at this level
    blt     il1_finished            ; skip if no cache, or just i-cache
    mcr     p15, 2, r8, c0, c0, 0   ; select current cache level in cssr
    ISB                             ; isb to sych the new cssr&csidr
    mrc     p15, 1, r1, c0, c0, 0   ; read the new csidr
    and     r2, r1, #7              ; extract the length of the cache lines
    add     r2, r2, #4              ; add 4 (line length offset)
    ldr     r4, =0x3ff
    ands    r4, r4, r1, lsr #3      ; find maximum number on the way size
    clz     r5, r4                  ; find bit position of way size increment
    ldr     r7, =0x7fff
    ands    r7, r7, r1, lsr #13     ; extract max number of the index size
il1_loop2
    mov     r9, r4                  ; create working copy of max way size
il1_loop3
    orr     r0, r8, r9, lsl r5      ; factor way and cache number into r0
    orr     r0, r0, r7, lsl r2      ; factor index number into r0
    mcr     p15, 0, r0, c7, c6, 2   ; invalidate by set/way
    subs    r9, r9, #1              ; decrement the way
    bge     il1_loop3
    subs    r7, r7, #1              ; decrement the index
    bge     il1_loop2
il1_finished
    mov     r8, #0                 ; swith back to cache level 0
    mcr     p15, 2, r8, c0, c0, 0  ; select current cache level in cssr
    DSB
    ISB
    ldmfd   sp!, {r0-r9}
    bx      lr
    ENDFUNC
;=======================================================================
; tzbsp_dcache_inval_l2
;
; DESCRIPTION -
;   Invalidate the l2 section of the D-cache.
;=======================================================================
tzbsp_dcache_inval_l2 FUNCTION
    DSB
    stmfd   sp!, {r0-r9}
    mrc     p15, 1, r6, c0, c0, 1   ; read clidr
    ands    r3, r6, #0x7000000      ; extract loc from clidr
    mov     r3, r3, lsr #23         ; left align loc bit field
    beq     il2_finished            ; if loc is 0, then no need to clean
    mov     r8, #1                  ; start clean at cache level 1
    add     r2, r8, r8, lsl #1      ; work out 3x current cache level
    mov     r1, r6, lsr r2          ; extract cache type bits from clidr
    and     r1, r1, #7              ; mask of the bits for current cache
    cmp     r1, #2                  ; see what cache we have at this lev
    blt     il2_finished            ; skip if no cache, or just i-cache
    mcr     p15, 2, r8, c0, c0, 0   ; select current cache level in cssr
    ISB                             ; isb to sych the new cssr&csidr
    mrc     p15, 1, r1, c0, c0, 0   ; read the new csidr
    and     r2, r1, #7              ; extract the length of the cache li
    add     r2, r2, #4              ; add 4 (line length offset)
    ldr     r4, =0x3ff
    ands    r4, r4, r1, lsr #3      ; find maximum number on the way siz
    clz     r5, r4                  ; find bit position of way size incr
    ldr     r7, =0x7fff
    ands    r7, r7, r1, lsr #13     ; extract max number of the index si
il2_loop2
    mov     r9, r4                  ; create working copy of max way siz
il2_loop3
    orr     r0, r8, r9, lsl r5      ; factor way and cache number into r
    orr     r0, r0, r7, lsl r2      ; factor index number into r0
    mcr     p15, 0, r0, c7, c6, 2   ; invalidate by set/way
    subs    r9, r9, #1              ; decrement the way
    bge     il2_loop3
    subs    r7, r7, #1              ; decrement the index
    bge     il2_loop2
il2_finished
    mov     r8, #0                 ; swith back to cache level 0
    mcr     p15, 2, r8, c0, c0, 0  ; select current cache level in cssr
    DSB
    ISB
    ldmfd   sp!, {r0-r9}
    bx      lr
    ENDFUNC
;=======================================================================
; tzbsp_dcache_clean_lx
;
; DESCRIPTION -
;   Clean L1/L2 D-cache.
;   r0: 0 for L1 & 1 for L2
;=======================================================================
tzbsp_dcache_clean_lx FUNCTION
    DSB
    stmfd   sp!, {r0-r9}
    mrc     p15, 1, r6, c0, c0, 1   ; read clidr
    ands    r3, r6, #0x7000000      ; extract loc from clidr
    mov     r3, r3, lsr #23         ; left align loc bit field
    beq     ilx_finished            ; if loc is 0, then no need to clean
    mov     r8, r0                  ; start clean at cache level r0
    add     r2, r8, r8, lsl #1      ; work out 3x current cache level
    mov     r1, r6, lsr r2          ; extract cache type bits from clidr
    and     r1, r1, #7              ; mask of the bits for current cache
    cmp     r1, #2                  ; see what cache we have at this lev
    blt     ilx_finished            ; skip if no cache, or just i-cache
    mcr     p15, 2, r8, c0, c0, 0   ; select current cache level in cssr
    ISB                             ; isb to sych the new cssr&csidr
    mrc     p15, 1, r1, c0, c0, 0   ; read the new csidr
    and     r2, r1, #7              ; extract the length of the cache li
    add     r2, r2, #4              ; add 4 (line length offset)
    ldr     r4, =0x3ff
    ands    r4, r4, r1, lsr #3      ; find maximum number on the way siz
    clz     r5, r4                  ; find bit position of way size incr
    ldr     r7, =0x7fff
    ands    r7, r7, r1, lsr #13     ; extract max number of the index si
ilx_loop2
    mov     r9, r4                  ; create working copy of max way siz
ilx_loop3
    orr     r0, r8, r9, lsl r5      ; factor way and cache number into r
    orr     r0, r0, r7, lsl r2      ; factor index number into r0
    mcr     p15, 0, r0, c7, c10, 2  ; clean by set/way
    subs    r9, r9, #1 
    bge     ilx_loop3
    subs    r7, r7, #1              ; decrement the index
    bge     ilx_loop2
ilx_finished
    mov     r8, #0                 ; swith back to cache level 0
    mcr     p15, 2, r8, c0, c0, 0  ; select current cache level in cssr
    DSB
    ISB
    ldmfd   sp!, {r0-r9}
    bx      lr
    ENDFUNC
;=======================================================================
; tzbsp_dcache_clean_inv_lx
;
; DESCRIPTION -
;   Clean and invalidate L1/L2 D-cache.
;   r0: 0 for L1 & 1 for L2
;=======================================================================
tzbsp_dcache_clean_inv_lx FUNCTION
    DSB
    stmfd   sp!, {r0-r9}
    mrc     p15, 1, r6, c0, c0, 1   ; read clidr
    ands    r3, r6, #0x7000000      ; extract loc from clidr
    mov     r3, r3, lsr #23         ; left align loc bit field
    beq     ilx_finished            ; if loc is 0, then no need to clean
    mov     r8, r0                  ; start clean at cache level r0
    add     r2, r8, r8, lsl #1      ; work out 3x current cache level
    mov     r1, r6, lsr r2          ; extract cache type bits from clidr
    and     r1, r1, #7              ; mask of the bits for current cache
    cmp     r1, #2                  ; see what cache we have at this lev
    blt     inv_ilx_finished            ; skip if no cache, or just i-cache
    mcr     p15, 2, r8, c0, c0, 0   ; select current cache level in cssr
    ISB                             ; isb to sych the new cssr&csidr
    mrc     p15, 1, r1, c0, c0, 0   ; read the new csidr
    and     r2, r1, #7              ; extract the length of the cache li
    add     r2, r2, #4              ; add 4 (line length offset)
    ldr     r4, =0x3ff
    ands    r4, r4, r1, lsr #3      ; find maximum number on the way siz
    clz     r5, r4                  ; find bit position of way size incr
    ldr     r7, =0x7fff
    ands    r7, r7, r1, lsr #13     ; extract max number of the index si
inv_ilx_loop2
    mov     r9, r4                  ; create working copy of max way siz
inv_ilx_loop3
    orr     r0, r8, r9, lsl r5      ; factor way and cache number into r
    orr     r0, r0, r7, lsl r2      ; factor index number into r0
    mcr     p15, 0, r0, c7, c14, 2  ; clean & invalidate by set/way
    subs    r9, r9, #1 
    bge     inv_ilx_loop3
    subs    r7, r7, #1              ; decrement the index
    bge     inv_ilx_loop2
inv_ilx_finished
    mov     r8, #0                 ; swith back to cache level 0
    mcr     p15, 2, r8, c0, c0, 0  ; select current cache level in cssr
    DSB
    ISB
    ldmfd   sp!, {r0-r9}
    bx      lr
    ENDFUNC
;============================================================================
; tzbsp_dcache_flush_region
;
; PROTOTYPE
;   void tzbsp_dcache_flush_region(void *addr, unsigned int length);
;
; ARGS
;   void *addr            - Start address of memory region
;   unsigned int length   - Length of memory region
;
; DESCRIPTION
;   Cleans and invalidates a memory region in the cache. Please note that the
;   data in the cache would be written back to main memory if dirty and the
;   region invalidated. So any further access to the data would result in a
;   cache-miss.
;============================================================================
tzbsp_dcache_flush_region FUNCTION
    stmfd   sp!, {r0, lr}
    DSB                             ; data barrier before flushing
    add     r1, r0, r1              ; get the end address
    dcache_granule_size r2, r3      ; r2 = cache granule in bytes
    sub     r3, r2, #1              ; r3 = (cache granule size - 1)
    bic     r0, r0, r3              ; r0 = start address with cache granule
                                    ;      size bits removed
flush_loop
    mcr     p15, 0, r0, c7, c14, 1  ; clean & invalidate line
    add     r0, r0, r2              ; r0 = (r0 + cache granule size)
    cmp     r0, r1                  ; r0 > "end of region" ?
    blt     flush_loop
    DSB                             ; ensure all memory operations are complete
    ldmfd   sp!, {r0, pc}
    ENDFUNC
;============================================================================
; tzbsp_dcache_inval_region
;
; PROTOTYPE
;   void tzbsp_dcache_inval_region(void *addr, unsigned int length);
;
; ARGS
;   void *addr            - Start address of memory region
;   unsigned int length   - Length of memory region
;
; DESCRIPTION
;   Invalidate a memory region in the cache. Please note that the data in
;   the cache will not be written back to the main memory. However, any
;   further accesses to data in this region would result in a cache-miss.
;============================================================================
tzbsp_dcache_inval_region FUNCTION
    stmfd   sp!, {lr}
    DSB                             ; data barrier before flushing
    add     r1, r0, r1              ; r1 = end address
    dcache_granule_size r2, r3      ; r2 = cache granule size in bytes
    sub     r3, r2, #1              ; r3 = (cache granule size - 1)
    bic     r0, r0, r3              ; r0 = start address with cache granule
                                    ;      size bits removed
inval_loop
    mcr     p15, 0, r0, c7, c6, 1   ; invalidate dcache granule
    add     r0, r0, r2              ; r0 += dcache granule size
    cmp     r0, r1
    blt     inval_loop
    DSB                             ; ensure all memory operations are complete
    ldmfd   sp!, {pc}
    ENDFUNC
;============================================================================
; tzbsp_dcache_clean_region
;
; PROTOTYPE
;   void tzbsp_dcache_clean_region(void *addr, unsigned int length);
;
; ARGS
;   void *addr            - Start address of memory region
;   unsigned int length   - Length of memory region
;
; DESCRIPTION
;   Cleans a memory region in the cache. Please note that this will write
;   back any data that is dirty but it will not invalidate the cache region.
;   So any further accesses to data in this region would result in a cache-hit.
;============================================================================
tzbsp_dcache_clean_region FUNCTION
    stmfd   sp!, {lr}
    DSB                             ; data barrier before flushing
    add     r1, r0, r1              ; get the end address
    dcache_granule_size r2, r3
    sub     r3, r2, #1
    bic     r0, r0, r3
clean_loop
    mcr     p15, 0, r0, c7, c10, 1  ; clean D / U line
    add     r0, r0, r2
    cmp     r0, r1
    blt     clean_loop
    DSB                             ; ensure all memory operations are complete
    ldmfd   sp!, {pc}
    ENDFUNC
; void tzbsp_icache_enable(void)
tzbsp_icache_enable FUNCTION
    mdsb
    misb
    mrc     p15, 0, r0, c1, c0, 0   ; Read from CP15 Control Register
    orr     r0, r0, #(1 << 12)      ; Enable L1 Instruction Cache
    mcr     p15, 0, r0, c1, c0, 0   ; Write back to CP15 Control Register
    mdsb
    misb
    bx      lr
    ENDFUNC
; void tzbsp_icache_disable(void)
tzbsp_icache_disable FUNCTION
    mdsb
    misb
    mrc     p15, 0, r0, c1, c0, 0   ; Read from CP15 Control Register
    and     r0, r0, #~(1 << 12)     ; Disable L1 Instruction Cache
    mcr     p15, 0, r0, c1, c0, 0   ; Write back to CP15 Control Register
    mdsb
    misb
    bx      lr
    ENDFUNC
; boolean tzbsp_icache_is_enabled(void)
tzbsp_icache_is_enabled FUNCTION
    misb
    mrc     p15, 0, r0, c1, c0, 0   ; Read SCTLR.
    and     r0, r0, #(1 << 12)      ; Extract SCTLR[I].
    lsr     r0, r0, #12             ; Move bit to position 0.
    bx      lr
    ENDFUNC
; void tzbsp_icache_inv(void)
tzbsp_icache_inv FUNCTION
    mdsb
    misb
    ; ICIALLUIS (PoU, Inner Shareable)
    mcr     p15, 0, r0, c7, c1, 0   ; r0 is ignored by ICIALLUIS
    mdsb
    misb
    bx      lr
    ENDFUNC
; void tzbsp_icache_inv_area(void* start, void* end)
tzbsp_icache_inv_area FUNCTION
    mdsb
    misb
    cmp     r0, r1
    bls     %f0                     ; start <= end
    mov     r2, r0                  ; start > end, need to swap
    mov     r0, r1                  ; Not swapping by XORing, plenty of regs
    mov     r1, r2
    ; ICIMVAU addresses in range [start, end] (PoU)
0   mcr     p15, 0, r0, c7, c5, 1
    add     r0, r0, #4
    cmp     r0, r1
    bls     %b0                     ; if(r0 <= r1) then invalidate(r0)
    mdsb
    misb
    bx      lr
    ENDFUNC
    END
