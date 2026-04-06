/* kernel.ld — 链接脚本（Lab1 任务1）
 *
 * 任务：告诉链接器把内核放在物理地址 0x80000000，
 *       并确保 _entry 是整个程序的第一条指令。
 */

OUTPUT_ARCH( "riscv" )
ENTRY( _entry )

SECTIONS
{
  . = 0x80000000;   /* <-- 改成正确的内核物理起始地址 */

  .text : {
    /* *(.text.entry) 确保 entry.S 中 _entry 的代码排在最前面 */
    *(.text.entry)
    *(.text .text.*)
    etext = .;
  }

  . = ALIGN(0x1000);
  trampoline = .;
  trampsec : {
    *(trampsec)
  }
  . = ALIGN(0x1000);
  

  . = ALIGN(0x1000);
  .rodata : {
    *(.rodata .rodata.*)
  }

  . = ALIGN(0x1000);
  .data : {
    *(.data .data.*)
  }

  . = ALIGN(0x1000);
  .bss : {
    /* end_address 标记内核数据结束位置，kalloc 从这里开始管理空闲内存（Lab3用） */
    end_address = .;
    *(.bss .bss.*)
  }

  . = ALIGN(0x1000);
  kernel_end = .;
}
