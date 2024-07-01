PROCESSOR    18F4620

#include <xc.inc>

; CONFIGURATION (DO NOT EDIT)
CONFIG OSC = HSPLL      ; Oscillator Selection bits (HS oscillator, PLL enabled (Clock Frequency = 4 x FOSC1))
CONFIG FCMEN = OFF      ; Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
CONFIG IESO = OFF       ; Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)
; CONFIG2L
CONFIG PWRT = ON        ; Power-up Timer Enable bit (PWRT enabled)
CONFIG BOREN = OFF      ; Brown-out Reset Enable bits (Brown-out Reset disabled in hardware and software)
CONFIG BORV = 3         ; Brown Out Reset Voltage bits (Minimum setting)
; CONFIG2H
CONFIG WDT = OFF        ; Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
; CONFIG3H
CONFIG PBADEN = OFF     ; PORTB A/D Enable bit (PORTB<4:0> pins are configured as digital I/O on Reset)
CONFIG LPT1OSC = OFF    ; Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
CONFIG MCLRE = ON       ; MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)
; CONFIG4L
CONFIG LVP = OFF        ; Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
CONFIG XINST = OFF      ; Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

; GLOBAL SYMBOLS
; You need to add your variables here if you want to debug them.
GLOBAL var1
GLOBAL var2
GLOBAL var3 
GLOBAL rb0_pressed
GLOBAL rb1_pressed
GLOBAL rb2_pressed
GLOBAL rb3_pressed
GLOBAL rb4_pressed
GLOBAL rb0_past
GLOBAL rb1_past
GLOBAL rb2_past
GLOBAL rb3_past
GLOBAL rb4_past
GLOBAL speed2x
GLOBAL bar_count
GLOBAL howm_bars
GLOBAL paused
GLOBAL lata_prev
GLOBAL reduced
GLOBAL resumed
; Define space for the variables in RAM
PSECT udata_acs
var1:
    DS 1 
var2:
    DS 1
var3:
    DS 1
rb0_pressed:
    DS 1
rb1_pressed:
    DS 1
rb2_pressed:
    DS 1
rb3_pressed:
    DS 1
rb4_pressed:
    DS 1

rb0_past:
    DS 1
rb1_past:
    DS 1
rb2_past:
    DS 1
rb3_past:
    DS 1
rb4_past:
    DS 1
bar_count:
    DS 1
howm_bars:
    DS 1
speed2x:
    DS 1
paused:
    DS 1
lata_prev:
    DS 1
reduced:
    DS 1
resumed:
    DS 1

PSECT resetVec,class=CODE,reloc=2
resetVec:
    goto       main

PSECT CODE
main:
    ;clearing past values.clrf rb0_past
    movlw 0x0f
    movwf ADCON1
    clrf rb0_pressed
    clrf rb1_pressed
    clrf rb2_pressed
    clrf rb3_pressed
    clrf rb4_pressed
    
    clrf rb0_past
    clrf rb1_past
    clrf rb2_past
    clrf rb3_past
    clrf rb4_past
    clrf speed2x
    clrf paused
    clrf reduced
    clrf resumed
    clrf lata_prev
    movlw 4
    movwf howm_bars
    clrf bar_count
    ;clearing
  clrf TRISA ; make PORTA an output
  setf TRISB; make PORTB an input
  clrf var1 ; var1 = 0 f means a memory location w-working register
  setf var2 ; set all bits to 1 -> 11111111 -> 255
  clrf var3
  movwf var3; now 42 is in var3.
  movf var1 ; var1 = var1 sets status codes.
  ;BUSY WAITING
  ;for(var1=0; var1< 99999; var1++)
  ;incf var1 ; var1++
  ;incfsz var1 ; var+1 if (var1 == 0) skip next
  
  ;TODO: Light up LEDs.
  ;sleep(1) -> schedule your program after 1 sec.
  
  movlw 00000111B ; first three pins to light up -> RA1, RA2, RA3  ;for initialization part.
  movwf LATA;
  call busy_wait
  clrf LATA;
  clrf var1
  clrf var2
  
main_loop:
  call check_buttons
  call metronome_update
  call check_buttons
  call main_loop

  
busy_wait: ;986ms
clrf var2 ; var2 = 255
clrf var1
	outer_loop_start:
	    loop_start:
		;call check_RB0
		incfsz var1
		goto loop_start
		
		;it'll skip goto when var1 == 0.
	incfsz var2
	goto outer_loop_start ;continue as long as var2 is not 0.
	outer_loop_start1:
	    loop_start1:
		;call check_RB0
		incfsz var1
		goto loop_start1
		;it'll skip goto when var1 == 0.
	incfsz var2
	goto outer_loop_start1 ;continue as long as var2 is not 0.
	outer_loop_start2:
	    loop_start2:
		;call check_RB0
		incfsz var1
		goto loop_start2
		;it'll skip goto when var1 == 0.
	incfsz var2
	goto outer_loop_start2 ;continue as long as var2 is not 0.
	outer_loop_start3:
	    loop_start3:
		;call check_RB0
		incfsz var1
		goto loop_start3
		;it'll skip goto when var1 == 0.
	incfsz var2
	goto outer_loop_start3 ;continue as long as var2 is not 0.
	outer_loop_start4:
	    loop_start4:
		;call check_RB0
		incfsz var1
		goto loop_start4
		;it'll skip goto when var1 == 0.
	incfsz var2
	goto outer_loop_start4 ;continue as long as var2 is not 0.
return
    
check_buttons:
    call check_RB0  ;might be error
    ;we will execute next instr if RB0=1
    movlw 0
    cpfseq paused ; if paused = 0, skip next_instr
    call check_RB0
    
    call check_RB1  ;there might be error
    
    call check_RB2  ;this & below were very simple, but check in case
    
    call check_RB3 ; doesnt work
    
    call check_RB4 ; works
    
    
    return
    
check_RB0:
    movff PORTB, rb0_pressed
    movlw 1
    cpfseq rb0_pressed ;if = 1, skip next instr.
    call RB0_pressed
    movff rb0_pressed, rb0_past ; move the value in rb0 to rb0_past.
    movlw 0
    cpfseq paused ; if paused = 0, skip next_instr
    call check_RB0
    return
    
RB0_pressed:
    ;movff PORTB, rb0_pressed
    movlw 1
    cpfseq rb0_past ;if rb0_past == 1, skip next instr means clicked
    return
    movff LATA, lata_prev ; lata_prev = LATA
    movlw 0
    cpfseq paused ; if paused == 0, skip next instr
    call resume ;call pause
    movlw 0
    cpfseq resumed ; if resumed == 0, skip next instr
    return
    clrf LATA
    movlw 00000100B ; RA2
    movwf LATA;
    ;movlw 0000111
    ;xorwf LATA
    movlw 1
    movwf paused ; paused = 1
    movlw 0
    movwf resumed ; resumed = 0
    
    return
    
resume:
    movlw 0
    movwf paused ; paused = 0
    ;movff lata_prev, LATA ; LATA = lata_prev
    ;movff PORTB, rb0_past ; move the value in rb0 to rb0_past.
    movlw 1
    movwf resumed ; resumed = 1
    return ; to make things faster, resume from the left check_button part.
    
check_RB1:
    ;btfss PORTB, 1 ; check RB1, if 1 skip next instr
    movff PORTB, rb1_pressed
    movlw 2
    cpfseq rb1_pressed
    call RB1_pressed
    movff rb1_pressed, rb1_past ; move the value in rb0 to rb0_past.
    return

RB1_pressed:
    ;movff PORTB, rb1_pressed
    movlw 2
    cpfseq rb1_past ;if rb1_past == 1, skip next instr means clicked
    return
    movlw 0
    cpfseq speed2x ; if speed2x == 0, skip next instr means we need to make it faster
    call reduce ;below makes speed2x
    movlw 0
    cpfseq reduced ; if reduced == 0, skip next instr
    return
    movlw 1
    movwf speed2x ; speed = 2x from now -> speed2x = 1
    movlw 0
    movwf reduced
    return
    
reduce:
    movlw 0
    movwf speed2x ; speed = 1x from now -> speed2x = 0
    ;movff PORTB, rb1_past ; move the value in rb0 to rb0_past.
    movlw 1
    movwf reduced
    return
    
check_RB2:
    ;btfss PORTB, 2 ; check RB2, if 1 skip next instr
    movff PORTB, rb2_pressed
    movlw 4
    cpfseq rb2_pressed
    call RB2_pressed
    movff rb2_pressed, rb2_past ; move the value in rb0 to rb0_past.
    return
    
RB2_pressed:
    ;movff PORTB, rb2_pressed ;Initialization LED correct bits: 7/8 (Grade += 2.2)
    movlw 4
    cpfseq rb2_past ;if rb2_past == 1, skip next instr means clicked
    return
    movlw 4
    movwf howm_bars ; howm_bars = 4
    return
    
check_RB3:
    ;btfss PORTB, 3 ; check RB3, if 1 skip next instr
    movff PORTB, rb3_pressed
    movlw 8
    cpfseq rb3_pressed
    call RB3_pressed
    movff rb3_pressed, rb3_past ; move the value in rb0 to rb0_past.
    return
    
RB3_pressed:
    ;movff PORTB, rb3_pressed
    movlw 8
    cpfseq rb3_past ;if rb3_past == 1, skip next instr means clicked
    return
    decf howm_bars ; howm_bars--
    return
    
check_RB4:
    movff PORTB, rb4_pressed
    movlw 16
    cpfseq rb4_pressed
    call RB4_pressed
    movff rb4_pressed, rb4_past ; move the value in rb0 to rb0_past.
    return
    
RB4_pressed:
    ;movff PORTB, rb4_pressed
    movlw 16
    cpfseq rb4_past ;if rb3_past == 1, skip next instr means clicked
    return
    incf howm_bars ; howm_bars++
    return
    
metronome_update:
    ;incf var1;;;but if u want to add 2:
    clrf bar_count
    movlw 00000011B ; light up RA0 and RA1
    movwf LATA;
    call bar_start  ;lighted RA0 RA1
    return
    
bar_start:
    incf bar_count ; bar count++
    
    movff howm_bars, WREG
    cpfslt bar_count ; skip if f<w
    return
    ;goto metronome_update
    ;incf bar_count ; bar count++
    
    call note_wait ;wait -NOT YET ARRANGED- 500ms OR 250ms
    call note_end ; turn off all leds
    call note_wait
    
    call note_start
    return
    
note_start:
    
    
    movff howm_bars, WREG
    cpfslt bar_count ; skip if f<w
    ;goto metronome_update
    return
    incf bar_count
    
    movlw 00000001B ; light up RA0
    movwf LATA;
    
    call note_wait ;wait -NOT YET ARRANGED- 500ms OR 250ms
    call note_end ; turn off all leds
    call note_wait
    
    ;goto note_start
    call note_start
    return
    
note_end:
    clrf LATA
    return
    
note_wait: ;508.114ms
    movlw 1
    cpfseq speed2x ;if speed2x == 1, skip next instruction
    call one_note1x
    movlw 0
    cpfseq speed2x ; if speed2x == 0, skip next instruction
    call one_note2x
    return
    
one_note1x: ;has to wait for 500ms
    clrf var1
    clrf var2
    loop1:   ;without check_buttons it's 197.399ms
	loop2:
	    call check_buttons
	    movlw 1
	    addwf var1
	    incfsz var1
	    goto loop2
	incf var2
	movlw 41
	cpfsgt var2 ; skip if f<w
	goto loop1
	
    clrf var1
    clrf var2
    return ; otherwise return

one_note2x: ;has to wait for 250ms
    clrf var1
    clrf var2
    loop12:   ;without check_buttons it's 197.399ms
	loop22:
	    call check_buttons
	    movlw 1
	    addwf var1
	    incfsz var1
	    goto loop22
	incf var2
	movlw 16
	cpfsgt var2 ; skip if f<w
	goto loop12
    clrf var1
    clrf var2
    return ; otherwise return

end resetVec
