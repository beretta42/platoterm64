;;; A C runtime start file

	export	start
	export  _ashlhi3
	export  _kbhit
	export  _cgetc
	import  main

	section .text

olds	rmb	2
	
start
	;; setup C stack
	sts	olds
	lds	#$6000
	;; replace irq vector
	orcc	#$50		; turn off both firq and irq
	ldx	#interrupt
	stx	$10d
	andcc	#~$10		; turn on irq
	;; go do main
	jsr	_main
	;; restore stack, return
	lds	olds
	rts


_ashlhi3:
        pshs    x
_ashlhi3_1:
        leax    -1,x
        cmpx    #-1
        beq     _ashlhi3_2
        aslb
        rola
        bra     _ashlhi3_1
_ashlhi3_2:
        puls    x,pc

key_hack:
	.db	0

_kbhit:
	jsr	$a1cb		; A = byte
	sta	key_hack
	clrb
	tfr	d,x
	rts

_cgetc:
	ldb	key_hack
	rts

interrupt:
	lda	$ff02		; clear vsync pia
	jsr	poll		; go poll and add drivewire
a@	rti			; return from interrupt