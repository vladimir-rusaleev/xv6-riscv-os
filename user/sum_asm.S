.data

#define BUF_SIZE 25

msg_string:  .asciz "|%s|\n"
msg_error:   .asciz "Ошибка: %s\n"
msg_result:  .asciz "Сумма: %d\n"
no_first_msg:.asciz "Нет первого числа"
no_second_msg: .asciz "Нет второго числа"
overflow_msg: .asciz "Слишком большая суммарная длина чисел"
buf:      .space BUF_SIZE

.text
.global main

main:
	la a0, buf
	li a1, BUF_SIZE
	call gets
	
	li t2, 0
        li t3, BUF_SIZE
        addi t3, t3, -1

	li t0, ' '
	lbu t1, (a0)
	addi t2, t2, 1
        beq t1, t0, error_no_first

	la a1, buf
	li t0, '\n'

find_newline:
	lbu t1, (a1)
	beq t1, zero, split
	beq t1, t0, replace
	beq t2, t3, error_overflow
	addi a1, a1, 1
	addi t2, t2, 1
	j find_newline

replace:
	sb zero, (a1)

	la a0, msg_string
	la a1, buf
	call printf

split:
	la a1, buf
	li t0, ' '


find_space:
	lbu t1, (a1)
	beq t1, t0, replace_space
	beq t1, zero, error_no_second
	addi a1, a1, 1
	j find_space

replace_space:
	sb zero, (a1)
	addi a1, a1, 1
	
	lbu t1, (a1)
	beq t1, zero, error_no_second
	
	la a0, buf
	call atoi
	mv s3, a0
	
	mv a0, a1
	call atoi
	mv s4, a0
	
	mv a1, zero
	add a1, s3, s4
	la a0, msg_result
	call printf

	li a7, 2
	li a0, 0
	ecall

error_no_first:
	li a0, 2
        la a1, msg_error
        la a2, no_first_msg
        call fprintf
        li a7, 2
        li a0, 1
        ecall
error_no_second:
	li a0, 2
	la a1, msg_error
	la a2, no_second_msg
	call fprintf
	li a7, 2
	li a0, 1
	ecall
error_overflow:
        li a0, 2
        la a1, msg_error
        la a2, overflow_msg
        call fprintf
        li a7, 2
        li a0, 1
        ecall
