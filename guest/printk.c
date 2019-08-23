/*
 * Copyright (c) 2019 Jie Zheng
 */
#include <printk.h>
#include <serial.h>


#define DEFAULT_RESOLVE_STACK 128

static void
resolve_qword(uint64_t qword)
{
    uint8_t stack[DEFAULT_RESOLVE_STACK];
    int iptr = 0;
    uint8_t mod = 0x0;
    if (qword < 0) {
        return;   
    }
    while (qword && iptr < DEFAULT_RESOLVE_STACK) {
        mod = qword % 10;
        stack[iptr++] = '0' + mod;
        qword /= 10;
    }
    if (!iptr) {
        stack[iptr++] = '0';
    }
    while (iptr > 0) {
        write_serial(stack[--iptr]);
    }
}

static void
resolve_hex_qword(uint64_t qword, uint8_t is_lowercase)
{
   uint8_t stack[DEFAULT_RESOLVE_STACK];
   uint8_t lower[] = "0123456789abcdef";
   uint8_t upper[] = "0123456789ABCDEF";
   int iptr = 0;
   int mod;
   while (qword && iptr < DEFAULT_RESOLVE_STACK) {
       mod = qword & 0xf;
       stack[iptr++] = is_lowercase ? lower[mod] : upper[mod];
       qword = qword >> 4;
   }

   if (!iptr) {
       stack[iptr++] = '0';
   }
   while (iptr > 0) {
       write_serial(stack[--iptr]);
   }
}

void
printk(const char *fmt, ...)
{
   va_list args;
   va_start(args, fmt);
   printk_mp_raw(fmt, args);
   va_end(args);
}

void
printk_mp_raw(const char * fmt, va_list arg_ptr)
{
    const char * ptr = fmt;
    
    for (; *ptr; ptr++) {
        if (*ptr != '%') {
            write_serial(*ptr);            
        } else {
            ptr++;
            switch (*ptr)
            {
                case 's':
                    {
                        char * string_ptr = va_arg(arg_ptr, char *);
                        for (; *string_ptr; string_ptr++) {
                            write_serial(*string_ptr);
                        }
                    }
                    break;
                case 'c':
                    {
                        char char_arg = (char)va_arg(arg_ptr, uint64_t);
                        write_serial(char_arg);
                    }
                    break;
                case 'd':
                    {
                        int32_t dword_arg = va_arg(arg_ptr, uint64_t);
                        if (dword_arg < 0) {
                            write_serial('-');
                            dword_arg = - dword_arg;
                        }
                        resolve_qword(dword_arg);
                    }
                    break;
                case 'q': // This is a new notation for quad word integer type
                    {
                        int64_t qword_arg = va_arg(arg_ptr, uint64_t);
                        if (qword_arg < 0) {
                            write_serial('-');
                            qword_arg = -qword_arg;
                        }
                        resolve_qword(qword_arg);
                    }
                    break;
                case 'x':
                    {
                        uint64_t qword_arg = va_arg(arg_ptr, uint64_t);
                        resolve_hex_qword(qword_arg, 1);
                    }
                    break;
                case 'X':
                    {
                        uint64_t qword_arg = va_arg(arg_ptr, uint64_t);
                        resolve_hex_qword(qword_arg, 0);
                    }
                    break;
                default:
                    break;
            }
        }
    }
}
