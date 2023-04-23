#ifndef __ASM_OFFSETS_H__
#define __ASM_OFFSETS_H__
/*
 * DO NOT MODIFY.
 *
 * This file was generated by Kbuild
 */

#define PV_CPU_usergs_sysret64 248 /* offsetof(struct paravirt_patch_template, cpu.usergs_sysret64) */

#define KVM_STEAL_TIME_preempted 16 /* offsetof(struct kvm_steal_time, preempted) */

#define pt_regs_bx 40 /* offsetof(struct pt_regs, bx) */
#define pt_regs_cx 88 /* offsetof(struct pt_regs, cx) */
#define pt_regs_dx 96 /* offsetof(struct pt_regs, dx) */
#define pt_regs_sp 152 /* offsetof(struct pt_regs, sp) */
#define pt_regs_bp 32 /* offsetof(struct pt_regs, bp) */
#define pt_regs_si 104 /* offsetof(struct pt_regs, si) */
#define pt_regs_di 112 /* offsetof(struct pt_regs, di) */
#define pt_regs_r8 72 /* offsetof(struct pt_regs, r8) */
#define pt_regs_r9 64 /* offsetof(struct pt_regs, r9) */
#define pt_regs_r10 56 /* offsetof(struct pt_regs, r10) */
#define pt_regs_r11 48 /* offsetof(struct pt_regs, r11) */
#define pt_regs_r12 24 /* offsetof(struct pt_regs, r12) */
#define pt_regs_r13 16 /* offsetof(struct pt_regs, r13) */
#define pt_regs_r14 8 /* offsetof(struct pt_regs, r14) */
#define pt_regs_r15 0 /* offsetof(struct pt_regs, r15) */
#define pt_regs_flags 144 /* offsetof(struct pt_regs, flags) */

#define saved_context_cr0 200 /* offsetof(struct saved_context, cr0) */
#define saved_context_cr2 208 /* offsetof(struct saved_context, cr2) */
#define saved_context_cr3 216 /* offsetof(struct saved_context, cr3) */
#define saved_context_cr4 224 /* offsetof(struct saved_context, cr4) */
#define saved_context_gdt_desc 267 /* offsetof(struct saved_context, gdt_desc) */


#define stack_canary_offset 40 /* offsetof(struct fixed_percpu_data, stack_canary) */


#define TASK_threadsp 5016 /* offsetof(struct task_struct, thread.sp) */
#define TASK_stack_canary 2240 /* offsetof(struct task_struct, stack_canary) */

#define crypto_tfm_ctx_offset 24 /* offsetof(struct crypto_tfm, __crt_ctx) */

#define pbe_address 0 /* offsetof(struct pbe, address) */
#define pbe_orig_address 8 /* offsetof(struct pbe, orig_address) */
#define pbe_next 16 /* offsetof(struct pbe, next) */

#define IA32_SIGCONTEXT_ax 44 /* offsetof(struct sigcontext_32, ax) */
#define IA32_SIGCONTEXT_bx 32 /* offsetof(struct sigcontext_32, bx) */
#define IA32_SIGCONTEXT_cx 40 /* offsetof(struct sigcontext_32, cx) */
#define IA32_SIGCONTEXT_dx 36 /* offsetof(struct sigcontext_32, dx) */
#define IA32_SIGCONTEXT_si 20 /* offsetof(struct sigcontext_32, si) */
#define IA32_SIGCONTEXT_di 16 /* offsetof(struct sigcontext_32, di) */
#define IA32_SIGCONTEXT_bp 24 /* offsetof(struct sigcontext_32, bp) */
#define IA32_SIGCONTEXT_sp 28 /* offsetof(struct sigcontext_32, sp) */
#define IA32_SIGCONTEXT_ip 56 /* offsetof(struct sigcontext_32, ip) */

#define IA32_RT_SIGFRAME_sigcontext 164 /* offsetof(struct rt_sigframe_ia32, uc.uc_mcontext) */

#define PV_IRQ_irq_disable 296 /* offsetof(struct paravirt_patch_template, irq.irq_disable) */
#define PV_IRQ_irq_enable 304 /* offsetof(struct paravirt_patch_template, irq.irq_enable) */
#define PV_CPU_iret 256 /* offsetof(struct paravirt_patch_template, cpu.iret) */
#define PV_MMU_read_cr2 376 /* offsetof(struct paravirt_patch_template, mmu.read_cr2) */

#define XEN_vcpu_info_mask 1 /* offsetof(struct vcpu_info, evtchn_upcall_mask) */
#define XEN_vcpu_info_pending 0 /* offsetof(struct vcpu_info, evtchn_upcall_pending) */
#define XEN_vcpu_info_arch_cr2 16 /* offsetof(struct vcpu_info, arch.cr2) */

#define BP_scratch 484 /* offsetof(struct boot_params, scratch) */
#define BP_secure_boot 492 /* offsetof(struct boot_params, secure_boot) */
#define BP_loadflags 529 /* offsetof(struct boot_params, hdr.loadflags) */
#define BP_hardware_subarch 572 /* offsetof(struct boot_params, hdr.hardware_subarch) */
#define BP_version 518 /* offsetof(struct boot_params, hdr.version) */
#define BP_kernel_alignment 560 /* offsetof(struct boot_params, hdr.kernel_alignment) */
#define BP_init_size 608 /* offsetof(struct boot_params, hdr.init_size) */
#define BP_pref_address 600 /* offsetof(struct boot_params, hdr.pref_address) */

#define PTREGS_SIZE 168 /* sizeof(struct pt_regs) */
#define TLB_STATE_user_pcid_flush_mask 22 /* offsetof(struct tlb_state, user_pcid_flush_mask) */
#define CPU_ENTRY_AREA_entry_stack 4096 /* offsetof(struct cpu_entry_area, entry_stack_page) */
#define SIZEOF_entry_stack 4096 /* sizeof(struct entry_stack) */
#define MASK_entry_stack -4096 /* (~(sizeof(struct entry_stack) - 1)) */
#define TSS_sp0 4 /* offsetof(struct tss_struct, x86_tss.sp0) */
#define TSS_sp1 12 /* offsetof(struct tss_struct, x86_tss.sp1) */
#define TSS_sp2 20 /* offsetof(struct tss_struct, x86_tss.sp2) */

#endif
