#define SBI_CONSOLE_PUTCHAR 0x01
#define SBI_CONSOLE_GETCHAR 0x02

#define SBI_ERR_FAILED -1

struct sbiret {
  long error;
  long value;
};