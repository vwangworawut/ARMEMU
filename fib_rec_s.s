.global fib_rec_s
.func fib_rec_s

.text

fib_rec_s:
	sub SP, SP, #4
	str r4, [SP]
	sub SP, SP, #4
	str r5, [SP]
	sub SP, SP, #4
	str lr, [SP]

	cmp r0, #0
	beq fib_exit
	cmp r0, #1
	beq fib_exit
	mov r4, r0
	bl fib

fib:
	/* n - 1 */
	sub r0, r0, #1
	bl fib_rec_s
	/* n - 2 */
	mov r5, r0
	
	mov r0, r4
	sub r0, r0, #2
	bl fib_rec_s
	mov r4, r0
	add r0, r4, r5
	
fib_exit:
	ldr lr, [SP]
	add SP, SP, #4
	ldr r5, [SP]
	add SP, SP, #4
	ldr r4, [SP]
	add SP, SP, #4

	bx lr
