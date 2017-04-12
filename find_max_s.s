.global find_max_s
.func find_max_s

find_max_s:
	sub SP, SP, #4
	str r4, [SP]
	//i
	mov r2, #0
	//array incrementer
	mov r3, #0
	//max 
	ldr r4, [r0]

loop:
	cmp r2, r1
	beq endloop
	mov r3, r0
	ldr r5, [r3]

	add r2, r2, #1
	add r0, r0, #4

	cmp r4, r5
	movlt r4, r5

	b loop

endloop:
	mov r0, r4
	ldr r4, [SP]
	add SP, SP, #4
	bx lr
