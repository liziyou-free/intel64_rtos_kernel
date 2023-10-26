#include "../startup/multiboot.h"
#include <cpuid.h>
#include "../x86_driver/x86_dt.h"


static int count = 0;


uint8_t stack[1024*1024] __attribute__((section("kernel_stack")));
uint8_t system_stack[1024*2] __attribute__((section("kernel_stack")));

uint32_t *gp_system_stack = (uint32_t *)(system_stack + sizeof(system_stack));
uint32_t  gp_task_stack = 0;


void (*gp_x86_isr[256])(void *);


void devided_error_isr(void *p_arg) {
    x86_serial_send(p_arg);
}


void general_protection_isr(uint32_t err_code) {

    int a = 10;
    while(a--);
}


void stack_exception_isr(uint32_t err_code) {

    int a = 10;
    while(a--);
}

extern void x86_exit0_isr(void *p);


void x86_common_isr (uint32_t inum, uint32_t err_code){

    const char *p_str = "This is x86 common ISR handle without error code!, divided error!";
    gp_x86_isr[0] = devided_error_isr;
    gp_x86_isr[12] = stack_exception_isr;
    gp_x86_isr[13] = general_protection_isr;
    gp_x86_isr[32] = general_protection_isr;
    gp_x86_isr[36] = x86_exit0_isr;

    if (inum > 255) {
        /* Error! */
        for (;;);
    }
    switch (inum) {

    /* Without error code exxceptiont handle */
    case divide_error:
    case debug_exceptions:
    case nomaskable:
    case breakpoint:
    case overflow:
    case bounds_check:
    case invalid_opcode:
    case coprocessor_not_available:
    case coprocessor_segment_overrun:
    case coprecessor_error:
        (gp_x86_isr[inum])((void*)p_str);
    break;

    /* With error code exxceptiont handle */
    case double_fault:
    case invalid_tss:
    case segment_not_present:
    case stack_exception:
    case general_protection:
    case page_fault:
        (gp_x86_isr[inum])((void*)err_code);
        break;

    /* Extern interrupt */
    default:
        (gp_x86_isr[inum])((void*)p_str);
        break;
    }
}




/* Example: Get CPU's model number */
static int get_model(void)
{
    int ebx, unused;
    __cpuid(0, unused, ebx, unused, unused);
    return ebx;
}


/* Example: Check for builtin local APIC. */
static int check_apic(void)
{
    unsigned int eax, unused, edx;
    __get_cpuid(1, &eax, &unused, &unused, &edx);
    return edx & (1 << 9);//CPUID_FEAT_EDX_APIC;
}




int main(int argc, char**arg) {

    uint8_t array[1024*1024*1024];
    static int is_apic = 0;
//    is_apic = check_apic();
//    int a = 3 / 0;    // devid error except
    __asm__ __volatile__ ("mov  $0x60000000, %esp \n\t"
                          "push $12;");

    array[sizeof(array)-10] = 128;    // stack segment except
    for(;;) {
        count++;
        printf("Count %d\n", count);
        if(count > 1) {
            break;
        }
    }
    return is_apic;
}






/******************************************************************************/

void task0 (void *p_arg) {
    static int j = 0;
    while(1){
        j++;
        if (j == 500) {
            x86_serial_send_str((char*)p_arg);
            break;
        }
    }
    return;
}


void task1 (void *p_arg) {

    static int j = 0;
    while(1){
//        x86_serial_send(*(char*)p_arg);
        j++;
        if (j < 300) {
            x86_serial_send_str((char*)p_arg);
        } else {
             break;
        }
    }
    return;
}


void task_exit_hook(void *arg, void *arg1) {

    x86_serial_send_str("Task Exit! %s\r\n");
    x86_serial_send_str((char *)arg);
    x86_serial_send_str((char *)arg1);
    for(;;);

}


/*
 *                     Performance Considerations
 * Because the hardware mechanism saves almost all of the CPU state it can be
 * slower than is necessary.For example,when the CPU loads new segment registers
 * it does all of the access and permission checks that are involved. As most
 * modern operating systems don't use segmentation,loading the segment registers
 * during context switches may be not be required, so for performance reasons t-
 * hese operating systems tend not to use the hardware context switching mechan-
 * ism. Due to it not being used as much CPU manufacturers don't optimize CPUs
 * for this method anymore (AFAIK). In addition the new 64 bit CPU's do not sup-
 * port hardware context switches when in 64 bit/long mode.
 */

uint32_t *task_init_list_stack(uint32_t *stack_top,
                               void(*p_func)(void*),
                               void* p_arg,
                               void(*p_exit)(void*, void*),
                               void *p_exit_arg,
                               void *p_exit_arg1)
{

    uint32_t    code_segment;

    *stack_top = (uint32_t)p_exit_arg1;
    stack_top--;

    *stack_top = (uint32_t)p_exit_arg;
    stack_top--;

    /* First parameter of the task */
    *stack_top = (uint32_t *)p_arg;
    stack_top--;

    *stack_top = (uint32_t)p_exit;
    stack_top--;

    /* EFLAG REGISTER (default value : 0x200 ==> 'eable interrupt') */
    *stack_top = 0x200;
    stack_top--;

    /* Use the same code segment as the kernel. */
    __asm__ __volatile__ ("movl  %%cs, %0":"=r"(code_segment));
    *stack_top = code_segment;
    stack_top--;

    /* First instruction */
    *stack_top = (uint32_t)p_func;
    stack_top--;

    /*all general registers */
    *stack_top = 0xea;
    stack_top--;

    *stack_top = 0xec;
    stack_top--;

    *stack_top = 0xed;
    stack_top--;

    *stack_top = 0xeb;
    stack_top--;

    /* ESP */
    *stack_top = 0xa5a5a5a5;
    stack_top--;

    /* EBP */
    *stack_top = 0x00;
    stack_top--;

    /* ESI */
    *stack_top = 0xea;
    stack_top--;

    /* EDI */
    *stack_top = 0x55555555;

    return stack_top;
}

uint32_t  *p_tcb0;
uint32_t  *p_tcb1;

volatile uint32_t task_stack0[1024];
volatile uint32_t task_stack1[1024];

void task_init() {
    p_tcb0 = task_init_list_stack((&task_stack0[0] + sizeof(task_stack0) - 4), task0, "task0\n", task_exit_hook, "freedom\r\n", "@me!");
    p_tcb1 = task_init_list_stack((&task_stack1[0] + sizeof(task_stack1) - 4), task1, "task1\n", task_exit_hook, "FREEDOM\r\n", "@ME!");
}


int flag = 0;

void x86_exit_timer_isr (void) {

    flag = !flag;
    if(!flag) {
        __asm__ __volatile__ ("mov  %0,  %%rsp"::"r"(p_tcb0));
    } else {
        __asm__ __volatile__ ("mov  %0,  %%rsp"::"r"(p_tcb1));
    }
    __asm__ __volatile__ ("popa");
    __asm__ __volatile__ ("iret");
}





