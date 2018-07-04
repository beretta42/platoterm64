;;; A C runtime start file

	export	start
	export  _ashlhi3
	export  _di
	export  _ei
	import  _kpoll
	import  main

	section .text

olds	rmb	2
	
start
	;; setup C stack
	sts	olds
	lds	#$8000
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


interrupt:
	lda	$ff02		; clear vsync pia
	jsr	_kpoll		; go poll keyboard
	jsr	poll		; go poll and add drivewire
a@	rti			; return from interrupt

_di:
	orcc	#$50		; stop interrupts
	rts

_ei:
	andcc	#~$50		; start interrupts
	rts