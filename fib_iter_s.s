.global fib_iter_s
.func fib_iter_s

.text

fib_iter_s:
	sub SP, SP, #4
	str r4, [SP]
	/* first */
	mov r1, #0
	/* second */
	mov r2, #1
	/* next */
	mov r3, #0 //0, 1
	/* i */
	mov r4, #0 //1, 2

loop:
	cmp r4, r0
	bgt endloop
	cmp r4, #1
	ble if
	/* next = first + second */
	add r3, r2, r1
	/* first = second */
	mov r1, r2
	/* second = next */
	mov r2, r3
	add r4, r4, #1

	b loop

if:
	mov r3, r4
	add r4, r4, #1
	b loop

endloop:
	ldr r4, [SP]
	add SP, SP, #4
	mov r0, r3
	bx lr

