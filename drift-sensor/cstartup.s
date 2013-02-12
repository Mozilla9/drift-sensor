;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Part one of the system initialization code,
;; contains low-level
;; initialization.
;;
;; Copyright 2006 IAR Systems. All rights reserved.
;;
;; Revision: 21638
;;
;; $Id$
;;

        MODULE  ?cstartup

        ;; Forward declaration of sections.
        SECTION IRQ_STACK:DATA:NOROOT(3)
        SECTION FIQ_STACK:DATA:NOROOT(3)
        SECTION SVC_STACK:DATA:NOROOT(3)
        SECTION ABT_STACK:DATA:NOROOT(3)
        SECTION UND_STACK:DATA:NOROOT(3)
        SECTION CSTACK:DATA:NOROOT(3)

;
; The module in this file are included in the libraries, and may be
; replaced by any user-defined modules that define the PUBLIC symbol
; __iar_program_start or a user defined start symbol.
;
; To override the cstartup defined in the library, simply add your
; modified version to the workbench project.

        SECTION .intvec:CODE:NOROOT(2)

        PUBLIC  __vector
        PUBLIC  __iar_program_start
        PUBLIC  __vector_0x14

        EXTERN	undef_handler, swi_handler, prefetch_handler
        EXTERN	data_handler, irq_handler, fiq_handler
        ARM	; Always ARM mode after reset

__vector:
		ldr	pc,[pc,#24]	; Absolute jump can reach 4 GByte

__undef_handler:
                ldr	pc,[pc,#24]	; Branch to undef_handler
                ;;B .

__swi_handler:
		ldr	pc,[pc,#24]	; Branch to swi_handler

__prefetch_handler:
		ldr	pc,[pc,#24]	; Branch to prefetch_handler
                ;;B .

__data_handler
		ldr	pc,[pc,#24]	; Branch to data_handler
                ;;B .


__vector_0x14
                dc32    0xFFFFFFFF

__irq_handler:
		ldr     pc,[pc, #-0x0120] ; Branch to irq_handler

__fiq_handler:
		ldr	pc,[pc,#24]	; Branch to fiq_handler

; ------------------------------------------------------------------------------
; Constant table entries (for ldr pc) will be placed at 0x20

      dc32	__iar_program_start
      dc32	0x00000000          ;__undef_handler
      dc32	0x00010200          ;__swi_handler
      dc32	0x00000000          ;__prefetch_handler
      dc32	0x00000000          ;__data_handler
      dc32	0xFFFFFFFF
      dc32	0xFFFFFFFF
      dc32	0x00010400          ;__fiq_handler

; ------------------------------------------------------------------------------
; Mode, correspords to bits 0-5 in CPSR

MODE_MSK DEFINE 0x1F            ; Bit mask for mode bits in CPSR

USR_MODE DEFINE 0x10            ; User mode
FIQ_MODE DEFINE 0x11            ; Fast Interrupt Request mode
IRQ_MODE DEFINE 0x12            ; Interrupt Request mode
SVC_MODE DEFINE 0x13            ; Supervisor mode
ABT_MODE DEFINE 0x17            ; Abort mode
UND_MODE DEFINE 0x1B            ; Undefined Instruction mode
SYS_MODE DEFINE 0x1F            ; System mode


CP_DIS_MASK DEFINE 0xFFFFFFF2
NOINT    DEFINE 0xc0
IRQ_BIT  DEFINE 0x80            ;; when IRQ bit is set, IRQ is disabled
FIQ_BIT  DEFINE 0x40            ;; when FIQ bit is set, FIQ is disabled


;-------------------------------------------------------------------------------
;
;-------------------------------------------------------------------------------
        SECTION .CSTARTUP:CODE:NOROOT(2)

        EXTERN  ?main
        REQUIRE __vector
        EXTERN  low_level_init

        ARM

__iar_program_start:

          ?cstartup:

I_Bit    DEFINE 0x80            ; when I bit is set, IRQ is disabled
F_Bit    DEFINE 0x40            ; when F bit is set, FIQ is disabled

;; Disable all interrupts
                LDR   R0,=0xFFFFF014
                MOV   R1,#0xFFFFFFFF
                STR   R1,[R0]

;-------------------------------------------------------------------------------
; Execution starts here.
; After a reset, the mode is ARM, Supervisor, interrupts disabled.
; Initialize the stack pointers.
; The pattern below can be used for any of the exception stacks:
; FIQ, IRQ, SVC, ABT, UND, SYS.
; The USR mode uses the same stack as SYS.
; The stack segments must be defined in the linker command file,
;
; Set stack pointers
;-------------------------------------------------------------------------------

                MRS    R0, cpsr                            ;; Original PSR value
                BIC    R0, R0, #MODE_MSK                   ;; Clear the mode bits
                ORR    R0, R0, #FIQ_MODE                   ;; Set FIR mode bits
                MSR    CPSR_C, R0                          ;; Change the mode
                LDR    SP,=SFE(FIQ_STACK)                  ;; End of FIQ_STACK

                BIC    R0,R0,#MODE_MSK                     ;; Clear the mode bits
                ORR    R0,R0,#IRQ_MODE                     ;; Set IRQ mode bits
                MSR    CPSR_C, R0                          ;; Change the mode
                LDR    SP,=SFE(IRQ_STACK)                  ;; End of IRQ_STACK

                BIC    R0, R0, #MODE_MSK                   ;; Clear the mode bits
                ORR    R0, R0, #(SVC_MODE|IRQ_BIT|FIQ_BIT) ;; Set Supervisor mode bits
                                                           ;; and dis int
                MSR    CPSR_C, R0                          ;; Change the mode
                LDR    SP,=SFE(CSTACK)                     ;; End of CSTACK

;-------------------------------------------------------------------------------
;
;-------------------------------------------------------------------------------

#ifdef __ARMVFP__
; Enable the VFP coprocessor.
                mov     r0, #BASE_ARD_EIM                       ; Set EN bit in VFP
                fmxr    fpexc, r0                               ; FPEXC, clear others.

; Disable underflow exceptions by setting flush to zero mode.
; For full IEEE 754 underflow compliance this code should be removed
; and the appropriate exception handler installed.
                mov     r0, #0x01000000		                ; Set FZ bit in VFP
                fmxr    fpscr, r0                               ; FPSCR, clear others.
#endif

; Add more initialization here


; Continue to ?main for more IAR specific system startup

                ldr     r0,=?main
                bx      r0

                END


