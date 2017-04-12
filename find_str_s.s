.global find_str_s
.func find_str_s


find_str_s:
	sub SP, SP, #4
	str r4, [SP]

	sub SP, SP, #4
	str r5, [SP]

	sub SP, SP, #4
	str r6, [SP]

	sub SP, SP, #4
	str r7, [SP]

	sub SP, SP, #4
	str r8, [SP]

	sub SP, SP, #4
	str r9, [SP]

	sub SP, SP, #4
	str r10, [SP]

	/* return val */
	mov r2, #0
	sub r2, r2, #1
	/*outer loop */
	mov r3, #0
	/*inner loop */
	mov r4, #0
	/*outer+inner*/
	mov r5, #0
	
	mov r10, #0

loop1:
	/* string */
	ldrb r6, [r0, r3]
	cmp r6, #0
	beq not_found

	/* substring */
	ldrb r7, [r1, r10]

	cmp r6, r7
	moveq r4, #1
	beq loop2

	add r3, r3, #1
	b loop1



loop2:
	add r5, r3, r4
	/* string[i + j]) */
	ldrb r8, [r0, r5]
	ldrb r9, [r1, r4]

	cmp r9, #0
	beq found

	cmp r8, r9
	addeq r4, r4, #1
	beq loop2

	addne r3, r3, #1
	bne loop1


found:
	mov r0, r3

	ldr r10, [SP]
	add SP, SP, #4
	ldr r9, [SP]
	add SP, SP, #4
	ldr r8, [SP]
	add SP, SP, #4
	ldr r7, [SP]
	add SP, SP, #4
	ldr r6, [SP]
	add SP, SP, #4
	ldr r5, [SP]
	add SP, SP, #4
	ldr r4, [SP]
	add SP, SP, #4

	bx lr

not_found:
	mov r0, r2

	ldr r10, [SP]
	add SP, SP, #4
	ldr r9, [SP]
	add SP, SP, #4
	ldr r8, [SP]
	add SP, SP, #4
	ldr r7, [SP]
	add SP, SP, #4
	ldr r6, [SP]
	add SP, SP, #4
	ldr r5, [SP]
	add SP, SP, #4
	ldr r4, [SP]
	add SP, SP, #4
	
	bx lr
