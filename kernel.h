#define SBI_CONSOLE_PUTCHAR 0x01
#define SBI_CONSOLE_GETCHAR 0x02
#define SBI_EXT_HSM 0x48534D
#define SBI_EXT_SRST 0x53525354
#define SBI_EXT_BASE 0x10

#define SBI_ERR_FAILED -1

struct sbiret {
  long error;
  long value;
};