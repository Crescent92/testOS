#include "exception.h"
#include <mem/fault.h>

static void divide_by_zero_fault(registers_t *regs) {
    KPANIC("DIVIDE BY ZERO EXCEPTION!!\n");
    for(;;) {

    }
}

static void single_step_trap(registers_t *regs) {
    KPANIC("SINGLE STEP TRAP!!\n");
    for(;;) {

    }
}

static void nmi_trap(registers_t *regs) {
    KPANIC("NMI!!\n");
    for(;;) {

    }
}

static void breakpoint_trap(registers_t *regs) {
    KPANIC("Breakpoint trap!\n");
    for(;;) {

    }
}

static void overflow_trap(registers_t *regs) {
    KPANIC("OVERFLOW TRAP!!\n");
    for(;;) {

    }
}

static void bound_check_fault(registers_t *regs) {
    KPANIC("BOUND CHECK FAULT!!\n");
    for(;;) {

    }
}

static void invalid_opcode_fault(registers_t *regs) {
    KPANIC("INVALID OPCODE FAULT!!\n");
    for(;;) {

    }
}

static void no_device_fault(registers_t *regs) {
    KPANIC("NO DEVICE FAULT!!\n");
    for(;;) {

    }
}

static void double_fault_abort(registers_t *regs) {
    KPANIC("DOUBLE FAULT!\n");
    for(;;) {

    }
}

static void invalid_tss_fault(registers_t *regs) {
    KPANIC("INVALID TSS FAULT!!\n");
    for(;;) {

    }
}

static void no_segment_fault(registers_t *regs) {
    KPANIC("Invalid segment!!\n");
    for(;;) {

    }
}

static void stack_fault(registers_t *regs) {
    KPANIC("Stack fault!!\n");
    for(;;) {

    }
}

static void general_prot_fault(registers_t *regs) {
    KPANIC("GENERAL PROTECTION FAULT!!\n");
    for(;;) {

    }
}

static void fpu_fault(registers_t *regs) {
    KPANIC("FPU FAULT!!\n");
    for(;;) {

    }
}

static void alignment_check_fault(registers_t *regs) {
    KPANIC("ALIGNMENT CHECK FAULT!!\n");
    for(;;) {

    }
}

static void machine_check_abort(registers_t *regs) {
    KPANIC("MACHINE CHECK ABORT!!\n");
    for(;;) {

    }
}

static void simd_fpu_fault(registers_t *regs) {
    KPANIC("SIMD FPU FAULT!!\n");
    for(;;) {

    }
}

void exception_init() {
    irq_install_handler (0, divide_by_zero_fault);
    irq_install_handler (1, single_step_trap);
    irq_install_handler (2, nmi_trap);
    irq_install_handler (3, breakpoint_trap);
    irq_install_handler (4, overflow_trap);
    irq_install_handler (5, bound_check_fault);
    irq_install_handler (6, invalid_opcode_fault);
    irq_install_handler (7, no_device_fault);
    irq_install_handler (8, double_fault_abort);
    irq_install_handler (10, invalid_tss_fault);
    irq_install_handler (11, no_segment_fault);
    irq_install_handler (12, stack_fault);
    irq_install_handler (13, general_prot_fault);
    irq_install_handler (16, fpu_fault);
    irq_install_handler (17, alignment_check_fault);
    irq_install_handler (18, machine_check_abort);
    irq_install_handler (19, simd_fpu_fault); 
}
