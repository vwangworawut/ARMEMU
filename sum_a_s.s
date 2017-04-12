.text

.balign 4

.global sum_a_s
.func sum_a_s

sum_a_s:
	//i
	mov r2, #0
	//sum
	mov r5, #0
loop:
	cmp r2, r1
	beq endloop
	mov r3, r0
	ldr r4, [r3]
	add r5, r5, r4
	add r2, r2, #1
	add r0, r0, #4
	b loop

endloop:
	mov r0, r5
	bx lr

