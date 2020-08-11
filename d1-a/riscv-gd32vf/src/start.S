.section .vector_table
.global entry
entry:
j reset_handler
j reset_handler


.align 4

.global reset_handler
.global __global_pointer$

.section .text
uart_wait:
	/* wait for tx register empty */
	li t0, 0x40004400
1:	lw t1, 0(t0)
	andi t1, t1, 0x80
	beq t1, zero, 1b
	ret

uart_send_str:
	addi sp, sp, -12
	sw s0, 0(sp)
	sw ra, 4(sp)
	sw s1, 8(sp)

	addi s1, a0, 0
	li s0, 0x40004400
1:	jal uart_wait

	/* get next char, return if zero */
	lbu t1, 0(s1)
	addi s1, s1, 1
	beq t1, zero, 2f
	sb t1, 0x4(s0)
	j 1b
2:
	lw s0, 0(sp)
	lw ra, 4(sp)
	lw s1, 8(sp)
	addi sp, sp, 12
	ret


uart_send_char:
	addi sp, sp, -8
	sw ra, 4(sp)
	sw a0, 0(sp)

	jal uart_wait

	/* get next char, return if zero */
	lbu t1, 0(sp)
	li t0, 0x40004400
	sb t1, 0x4(t0)

	lw ra, 4(sp)
	addi sp, sp, 8
	ret



uart_send_hex:
	addi sp, sp, -12
	sw s0, 0(sp)
	sw ra, 4(sp)
	sw s1, 8(sp)
	addi s1, a0, 0

	la a0, hexstart
	jal uart_send_str

	li s0, 28
1:	srl t2, s1, s0
	addi s0, s0, -4
	andi t2, t2, 0x0F

	la a0, hexchars
	add a0, a0, t2
	lbu a0, 0(a0)
	jal uart_send_char

	bge s0, zero, 1b

	lw s0, 0(sp)
	lw ra, 4(sp)
	lw s1, 8(sp)
	addi sp, sp, 12
	ret


flash_dump:
	addi sp, sp, -0xc
	sw s0, 0(sp)
	sw ra, 4(sp)
	sw s1, 8(sp)

	addi s0, a0, 0
	addi s1, a1, 0

	la a0, rangestart
	jal uart_send_str

	addi a0, s0, 0
	jal uart_send_hex

	la a0, rangeto
	jal uart_send_str

	addi a0, s0, 0
	addi a1, s1, 0
	add a0, a0, a1
	addi a0, a0, -1
	jal uart_send_hex

	la a0, nl
	jal uart_send_str

	add s1, s0, s1

	/* print next word */
1:	lw a0, 0(s0)
	addi s0, s0, 4
	jal uart_send_hex

	/* print space */
	li a0, 0x20
	jal uart_send_char
	bne s0, s1, 1b

	la a0, completed
	jal uart_send_str

	lw s0, 0(sp)
	lw ra, 4(sp)
	lw s1, 8(sp)
	addi sp, sp, 0xc
	ret

/* Print a nice and warm welcome message */
print_welcome:
	addi sp, sp, -4
	sw ra, 0(sp)

	la a0, welcomestr
	jal uart_send_str

	lw ra, 0(sp)
	addi sp, sp, 4
	ret

reset_handler:
	la sp, _stack

	/* enable UART2 clock and GPIOA clock */
	li t0, 0x40021000
	li t1, 0x00020000
	lw t2, 0x1C(t0)
	or t1, t1, t2
	sw t1, 0x1C(t0)
	li t1, 0x00000004
	lw t2, 0x18(t0)
	or t1, t1, t2
	sw t1, 0x18(t0)

	/* Configure GPIO: USART2 TX (PA2) on */
	li t0, 0x40010800
	li t1, 0xFFFFF0FF
	lw t2, 0x0(t0)
	li t3, 0x00000A00
	and t1, t1, t2
	or t1, t1, t3
	sw t1, 0x0(t0)


	/* configure UART (freq=8MHz, baud rate=115200, mantissa=4, fraction=5) */
	li t0, 0x40004400
	li t1, 0x00000045
	li t2, 0x0000200C
	sw t1, 0x8(t0)
	sw t2, 0xC(t0)

	jal print_welcome


	la a0, _flash_start
	la a1, _flash_size
	jal flash_dump

	li a0, 0x1ffff800
	li a1, 0x10
	jal flash_dump



endsw:
	j endsw

welcomestr:
.string "Flash Dump Assistant\r\n"

rangestart:
.string "Dumping from "

rangeto:
.string " to "

nl:
.string "\r\n"

hexstart:
.string "0x"

hexchars:
.string "0123456789ABCDEF"

completed:
.string "\r\ndone!\r\n"
