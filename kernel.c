#include "kernel.h"
#include "common.h"
#include "common.h"

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef uint32_t size_t;

extern char __bss[], __bss_end[], __stack_top[];

void *memset(void *buf, char c, size_t n) {
    uint8_t *p = (uint8_t *) buf;
    while (n--)
        *p++ = c;
    return buf;
}

struct sbiret sbi_call(long arg0, long arg1, long arg2, long arg3, long arg4,
                       long arg5, long fid, long eid) {
    register long a0 __asm__("a0") = arg0;
    register long a1 __asm__("a1") = arg1;
    register long a2 __asm__("a2") = arg2;
    register long a3 __asm__("a3") = arg3;
    register long a4 __asm__("a4") = arg4;
    register long a5 __asm__("a5") = arg5;
    register long a6 __asm__("a6") = fid;
    register long a7 __asm__("a7") = eid;

    __asm__ __volatile__("ecall"
                         : "=r"(a0), "=r"(a1)
                         : "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5),
                           "r"(a6), "r"(a7)
                         : "memory");
    return (struct sbiret){.error = a0, .value = a1};
}

void putchar(char ch) {
    sbi_call(ch, 0, 0, 0, 0, 0, 0, SBI_CONSOLE_PUTCHAR);
}

int
getchar(void)
{
  struct sbiret ret;

  do
  {
    ret = sbi_call(0, 0, 0, 0, 0, 0, 0, SBI_CONSOLE_GETCHAR);
  } while (ret.error == SBI_ERR_FAILED);

  return (int) ret.error;
}

void put(const char *s) {
    while (*s) {
        putchar(*s++);
    }
}

void sbi_hart_stop(void) {
    sbi_call(0, 0, 0, 0, 0, 0, 1, 0x48534D);
    while(1);
}

void sbi_system_shutdown(void) {
    sbi_call(0, 0, 0, 0, 0, 0, 0, 0x53525354);
    while(1);
}


void kernel_main(void) {
    put("Choose an option: \n");
    put("1. Get SBI implementation version\n");
    put("2. Hart get status\n");
    put("3. Hart stop\n");
    put("4. System Shutdown\n");

    for(;;) {
        int option = getchar();
        put("\n");

        switch (option) {
            case '1': {
            struct sbiret result = sbi_call(0, 0, 0, 0, 0, 0, 2, 0x10);

            printf("SBI implementation version: %d.%d\n", (result.value >> 16) & 0xFFFF, result.value & 0xFFFF);
            break;
            }
            case '2':
                put("Enter hart ID: ");
                int hart_id;
                while ((hart_id = getchar()) == -1)
                    ;
                putchar(hart_id);
                put("\n");

                hart_id = hart_id - '0';

                struct sbiret res = sbi_call(hart_id, 0, 0, 0, 0, 0, 2, 0x48534D);

                if (res.error != 0) {
                    put("Hart status error");
                    put("\n");
                } else {
                    put("Hart status: ");
                    putchar(res.value + '0');
                    put("\n");
}
                break;
            case '3':
                put("Stopping hart...");
                sbi_hart_stop();
                break;
            case '4':
                put("Stopping system");
                sbi_system_shutdown();
                break;
            default:
                put("Invalid option. Please choose again.\n");
                break;  
        }  
    }     
}


__attribute__((section(".text.boot")))
__attribute__((naked))
void boot(void) {
    __asm__ __volatile__(
        "mv sp, %[stack_top]\n" // Устанавливаем указатель стека
        "j kernel_main\n"       // Переходим к функции main ядра
        :
        : [stack_top] "r" (__stack_top) // Передаём верхний адрес стека в виде %[stack_top]
    );
}

