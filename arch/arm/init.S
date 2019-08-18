.section ".text.boot"

.global _start
.org 0x8000 // 加载地址0x8000
.extern __vector_reset
.extern __vector_undefined
.extern __vector_swi
.extern __vector_prefetch_abort
.extern __vector_data_abort
.extern __vector_reserved
.extern __vector_irq
.extern __vector_fiq
_start: // 异常向量表-开始
    // 操作系统启动入口点
    ldr pc,_vector_reset          // 设备重置
    ldr pc,_vector_undefined      // 未定义
    ldr pc,_vector_swi            // 软件中断
    ldr pc,_vector_prefetch_abort // 预取终止
    ldr pc,_vector_data_abort     // 数据终止
    ldr pc,_vector_reserved       // 保留
    ldr pc,_vector_irq            // 中断
    ldr pc,_vector_fiq            // 快速中断

_vector_reset:          .word __vector_reset
_vector_undefined:      .word __vector_undefined
_vector_swi:            .word __vector_swi
_vector_prefetch_abort: .word __vector_prefetch_abort
_vector_data_abort:     .word __vector_data_abort
_vector_reserved:       .word __vector_reserved
_vector_irq:            .word __vector_irq
_vector_fiq:            .word __vector_fiq