.section .text
// 异常向量表-异常处理（未完成）
.global __vector_undefined
.global __vector_swi
.global __vector_prefetch_abort
.global __vector_data_abort
.global __vector_reserved
.global __vector_irq
.global __vector_fiq

//.extern irq_handler
.text

__vector_undefined:
    nop
__vector_swi:
    nop
__vector_prefetch_abort:
    nop
__vector_data_abort:
    nop
__vector_reserved:
    nop
__vector_irq:
    nop
__vector_fiq:
    nop