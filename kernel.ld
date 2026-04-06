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
    . = ALIGN(0x1000);
    /*trampsec单独放一个区域*/
    _trampoline = .;
    *(trampsec)
    . = ALIGN(0x1000);
    ASSERT(. - _trampoline == 0x1000, "error: trampoline larger than one page");
    PROVIDE(etext = .);
  }
  
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
    *(.bss .bss.*)
  }

  . = ALIGN(0x1000);
  end_address = .;
  kernel_end = .;
}
