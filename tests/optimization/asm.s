
test:     file format elf32-littlearm


Disassembly of section .text:

00008000 <main>:
    8000:	4b08      	ldr	r3, [pc, #32]	; (8024 <main+0x24>)
    8002:	4809      	ldr	r0, [pc, #36]	; (8028 <main+0x28>)
    8004:	6819      	ldr	r1, [r3, #0]
    8006:	4001      	ands	r1, r0
    8008:	6019      	str	r1, [r3, #0]
    800a:	6819      	ldr	r1, [r3, #0]
    800c:	f441 41a0 	orr.w	r1, r1, #20480	; 0x5000
    8010:	6019      	str	r1, [r3, #0]
    8012:	681a      	ldr	r2, [r3, #0]
    8014:	4002      	ands	r2, r0
    8016:	601a      	str	r2, [r3, #0]
    8018:	681a      	ldr	r2, [r3, #0]
    801a:	2000      	movs	r0, #0
    801c:	f442 42e0 	orr.w	r2, r2, #28672	; 0x7000
    8020:	601a      	str	r2, [r3, #0]
    8022:	4770      	bx	lr
    8024:	20003000 	.word	0x20003000
    8028:	ffe00fff 	.word	0xffe00fff
