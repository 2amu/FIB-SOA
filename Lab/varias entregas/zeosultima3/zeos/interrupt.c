/*
 * interrupt.c -
 */
#include <types.h>
#include <interrupt.h>
#include <segment.h>
#include <hardware.h>
#include <io.h>

#include <zeos_interrupt.h>

Gate idt[IDT_ENTRIES];
Register    idtR;
int varx = 60;
//int zeos_ticks = 0;
char hexa[24];

char char_map[] =
{
  '\0','\0','1','2','3','4','5','6',
  '7','8','9','0','\'','¡','\0','\0',
  'q','w','e','r','t','y','u','i',
  'o','p','`','+','\0','\0','a','s',
  'd','f','g','h','j','k','l','ñ',
  '\0','º','\0','ç','z','x','c','v',
  'b','n','m',',','.','-','\0','*',
  '\0','\0','\0','\0','\0','\0','\0','\0',
  '\0','\0','\0','\0','\0','\0','\0','7',
  '8','9','-','4','5','6','+','1',
  '2','3','0','\0','\0','\0','<','\0',
  '\0','\0','\0','\0','\0','\0','\0','\0',
  '\0','\0'
};

void setInterruptHandler(int vector, void (*handler)(), int maxAccessibleFromPL)
{
  /***********************************************************************/
  /* THE INTERRUPTION GATE FLAGS:                          R1: pg. 5-11  */
  /* ***************************                                         */
  /* flags = x xx 0x110 000 ?????                                        */
  /*         |  |  |                                                     */
  /*         |  |   \ D = Size of gate: 1 = 32 bits; 0 = 16 bits         */
  /*         |   \ DPL = Num. higher PL from which it is accessible      */
  /*          \ P = Segment Present bit                                  */
  /***********************************************************************/
  Word flags = (Word)(maxAccessibleFromPL << 13);
  flags |= 0x8E00;    /* P = 1, D = 1, Type = 1110 (Interrupt Gate) */

  idt[vector].lowOffset       = lowWord((DWord)handler);
  idt[vector].segmentSelector = __KERNEL_CS;
  idt[vector].flags           = flags;
  idt[vector].highOffset      = highWord((DWord)handler);
}

void setTrapHandler(int vector, void (*handler)(), int maxAccessibleFromPL)
{
  /***********************************************************************/
  /* THE TRAP GATE FLAGS:                                  R1: pg. 5-11  */
  /* ********************                                                */
  /* flags = x xx 0x111 000 ?????                                        */
  /*         |  |  |                                                     */
  /*         |  |   \ D = Size of gate: 1 = 32 bits; 0 = 16 bits         */
  /*         |   \ DPL = Num. higher PL from which it is accessible      */
  /*          \ P = Segment Present bit                                  */
  /***********************************************************************/
  Word flags = (Word)(maxAccessibleFromPL << 13);

  //flags |= 0x8F00;    /* P = 1, D = 1, Type = 1111 (Trap Gate) */
  /* Changed to 0x8e00 to convert it to an 'interrupt gate' and so
     the system calls will be thread-safe. */
  flags |= 0x8E00;    /* P = 1, D = 1, Type = 1110 (Interrupt Gate) */

  idt[vector].lowOffset       = lowWord((DWord)handler);
  idt[vector].segmentSelector = __KERNEL_CS;
  idt[vector].flags           = flags;
  idt[vector].highOffset      = highWord((DWord)handler);
}

void kb_routine() {
   unsigned char a = inb(0x60);
  
   if (!(a & 0x80)) {
   	printc_xy(varx,20,char_map[a&0x7f]);
   	++varx;
   }
 }

void clk_routine() {
   ++zeos_ticks;
   zeos_show_clock();
   
}

void pgfault_routine(unsigned long a, unsigned long b) {
   hex(b, hexa);
   itoa(b, hexa);
   printk("\n Process generates a PAGE FAULT exception at EIP: 0x");
   printk(hexa);
   while(1);
   
}

void kb_handler(void);
void clk_handler(void);
void pgfault_handler(void);
void syscall_handler_sysenter(void);
void writeMSR(unsigned long a, unsigned long b);

void setIdt()
{
  /* Program interrups/exception service routines */
  idtR.base  = (DWord)idt;
  idtR.limit = IDT_ENTRIES * sizeof(Gate) - 1;
  
  set_handlers();

  /* ADD INITIALIZATION CODE FOR INTERRUPT VECTOR */
  setInterruptHandler(33, kb_handler ,0);
  setInterruptHandler(32, clk_handler ,0);
  setInterruptHandler(14, pgfault_handler ,0);
  set_idt_reg(&idtR);
  
  writeMSR(0x174,__KERNEL_CS);
  writeMSR(0X175,INITIAL_ESP);
  writeMSR(0X176,(unsigned long)syscall_handler_sysenter);
  
}

void hex(int b, char* hex) {
  int i = 0;
  while (b > 0) {
  	int newnum = b%16;
  	if(newnum < 10) {
  	  hex[i] = newnum + '0';
  	}
  	else {
  	  hex[i] = newnum + 'A' - 10;
  	}
  	b /= 16;
  	i++;
  }
  hex[i] = '/0';
   int j = 0, k = i-1;
   while(j < k) {
   	char aux = hex[j];
   	hex[j] = hex[k];
   	hex[k] = aux;
   	j++;
   	k--;
   }
}


