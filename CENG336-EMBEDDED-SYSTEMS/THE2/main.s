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
GLOBAL paused
GLOBAL bar_length
GLOBAL speed
GLOBAL beat
GLOBAL interrupt_rb
; Define space for the variables in RAM
PSECT udata_acs
paused:
    DS 1
    
bar_length:
    DS 1
    
bartest:
    DS 1
speed:
    DS 1
beat:
    DS 1
bar_begin_flag:
    DS 1
var1:
    DS 1
interrupt_rb:
    DS 1

PSECT CODE
org 0x0000
  goto main


org 0x0008
interrupt_service_routine:
  btfsc INTCON, 0 ; check RB interrupts
  goto rb_interrupt
  btfsc INTCON, 2 ; check if timer0 has overloaded
  goto timer0_interrupt ; if interrupt occured -> check TMROIF
  btfsc PIR1, 0 ; check if timer1 has overloaded
  goto timer1_interrupt
  retfie  1                 ; No match
    
rb_interrupt:
    movff PORTB, interrupt_rb
    nop
    bcf INTCON, 0 ; clear interrupt
    
    movlw 0
    cpfseq paused ; if paused == 0 (not paused) skip next instruction
    goto rb_paused ; if it is paused
    goto rb_metronome ; if it is not paused
    retfie 1
    
rb_paused:
    ;bcf INTCON, 0 ; clear interrupt
    btfsc interrupt_rb, 4 ; if RB4 is pressed
    goto paused_RB4_pressed
    btfsc interrupt_rb, 5 ; if RB5 is pressed
    goto paused_RB5_pressed
    btfsc interrupt_rb, 6 ; if RB6 is pressed
    goto paused_RB6_pressed
    btfsc interrupt_rb, 7 ; if RB7 is pressed
    goto paused_RB7_pressed
    retfie 1

paused_RB4_pressed:
    clrf LATC
    clrf paused ; it is not paused anymore -> paused = 0
    call timer_start ; timer started according to speed -> continuing case started.
    call first_bar
    retfie 1
paused_RB5_pressed:
    movlw 9
    cpfslt speed ; if speed < 9, still can be increased (skip next instruction-> return)/ else return since it's nine.
    retfie 1
    incf speed ; speed += 1
    retfie 1
paused_RB6_pressed:
    movlw 1
    cpfsgt speed ; if speed > 1, still can be decreased (skip next instruction-> return)/ else return since it's 1
    retfie 1
    decf speed
    retfie 1
paused_RB7_pressed:
    movlw 6
    movwf speed ; initial = 6, min = 1, max = 9 -> speed reset
    retfie 1
    
;---rb_metronome is CONTINUING---
rb_metronome:
    ;bcf INTCON, 0 ; clear interrupt
    btfsc interrupt_rb, 4 ; if RB4 is pressed
    goto continuing_RB4_pressed
    btfsc interrupt_rb, 5 ; if RB5 is pressed
    goto continuing_RB5_pressed
    btfsc interrupt_rb, 6 ; if RB6 is pressed
    goto continuing_RB6_pressed
    btfsc interrupt_rb, 7 ; if RB7 is pressed
    goto continuing_RB7_pressed
    retfie 1
    
continuing_RB4_pressed:
    call display_empty_4
    nop
    call display_empty_3
    nop
    call display_empty_2
    nop
    call display_empty_1
    nop
    
    movlw 1
    movwf beat
    movlw 4
    movwf bar_length
    bcf T0CON, 7 ; stopped timer
    bcf T1CON, 0
    movlw 1
    movwf paused; it is not paused anymore -> paused = 0
    retfie 1
continuing_RB5_pressed:
    movlw 8
    cpfslt bar_length ; if >8, continue
    retfie 1
    incf bar_length ; increase bar_length
    retfie 1
continuing_RB6_pressed:
    movlw 2
    cpfsgt bar_length ; if <2, do nothing
    retfie 1
    decf bar_length
    movlw 1
    movwf beat
    retfie 1
continuing_RB7_pressed:
    movlw 4
    movwf bar_length
    retfie 1
    

main:
  clrf INTCON
  clrf PIE1
  clrf PIR1
  clrf TMR0
  clrf TMR1H
  clrf TMR1L
  
  ;SETTING INPUT/OUTPUT PORTS, CLEARING LATAX FOR ALL:
  
  clrf LATD ; segments a to g
  clrf TRISD ; output
  clrf LATA ; RA0-RA3 function as multiplexer
  clrf TRISA ;output 
  clrf LATC ; RC0 -> beat led, RC1 -> LED indicating start of a bar
  clrf TRISC ;output
  bcf RCON, 7
  
  ;input (interrupt souces):
  
  movlw 11110000B ; this will be the value in TRISB
  movwf TRISB ; RB4, RB5, RB6, RB7 -> interrupt sources
  clrf LATB ; to make it guaranteed everything is initialized before interrupt.
  
  ;INITIALIZING OTHER VARIABLES
  movlw 1
  movwf paused ;program shall start in paused state.
  movlw 4
  movwf bar_length ; the initial value = 4, min = 2, max = 8
  movlw 6
  movwf speed ; initial = 6, min = 1, max = 9
  movlw 1
  movwf bar_begin_flag;
  movlw 1
  movwf beat ;The current beat shall start at 1. boyle dedigi icin degistirdim NOT SURE
  
  ;INTERRUPT ENABLE
  bsf PIE1, 1 ; enabled TIMER2 interrupts. TIMER2 for display wait.
  bcf INTCON2, 7 ; cleared all PORTB pull-ups.
  bsf PIE1, 0 ; enabled TIMER1 interrupts. TIMER1 for led duration.
  clrf INTCON
  movlw   01101000B 
  movwf INTCON ; enabled peripheral, TIMER0, RB interrupts. TIMER0 for speed
  bsf INTCON, 7 ; enabled GIE
  
  call speed_display ; display p and speed

main_loop:
  movlw 1
  cpfseq paused ; if paused, skip next instruction
  call check_buttons_continuing
  movlw 0 
  cpfseq paused ; if not paused, skip next instruction
  call check_buttons_paused
  goto main_loop
    
check_buttons_paused: ; JUST TO DISPLAY
    call speed_display ; DISPLAY SPEED AT THE END OF CHECK PART.
    return
    
; --- CHECK BUTTONS CONTINUING ---
check_buttons_continuing: ;  not in ISR, for displaying
 
    call display_beat_value ;DIS2
    call wait_loop
    call display_dash ;DIS3
    ;call start_timer1
    call wait_loop
    call display_barlength_value ;DIS4
    call wait_loop
    call display_empty_4
    return
    
    
wait_loop:
    movlw 15
    addwf var1
    incfsz var1
    goto wait_loop
    return
    

   
speed_display:
    ;DISPLAYING P
    
    movlw 9
    subwf speed, 0
    btfss STATUS, 2
    goto check_8_speed_display
    call display_p
    call wait_loop
    call display_empty_2
    call display_empty_3
    call display_9_speed ;speed= 8

    check_8_speed_display: ; Check if speed is equal to 7
	movlw 8
	subwf speed, 0
	btfss STATUS, 2
	goto check_7_speed_display
	call display_p
	call wait_loop
	call display_empty_2
	call display_empty_3
	call display_8_speed ;speed= 8

	
    check_7_speed_display: ; Check if speed is equal to 7
	movlw 7
	subwf speed, 0
	btfss STATUS, 2
	goto check_6_speed_display
	call display_p
	call wait_loop
	call display_empty_2
	call display_empty_3
	call display_7_speed ;speed= 8
	
	
    check_6_speed_display: ; Check if speed is equal to 7
	movlw 6
	subwf speed, 0
	btfss STATUS, 2
	goto check_5_speed_display
	call display_p
	call wait_loop
	call display_empty_2
	call display_empty_3
	call display_6_speed ;speed= 8

    check_5_speed_display: ; Check if speed is equal to 7
	movlw 5
	subwf speed, 0
	btfss STATUS, 2
	goto check_4_speed_display
	call display_p
	call wait_loop
	call display_empty_2
	call display_empty_3
	call display_5_speed ;speed= 8

    check_4_speed_display: ; Check if speed is equal to 7
	movlw 4
	subwf speed, 0
	btfss STATUS, 2
	goto check_3_speed_display
	call display_p
	call wait_loop
	call display_empty_2
	call display_empty_3
	call display_4_speed ;speed= 8

    check_3_speed_display: ; Check if speed is equal to 7
	movlw 3
	subwf speed, 0
	btfss STATUS, 2
	goto check_2_speed_display
	call display_p
	call wait_loop
	call display_empty_2
	call display_empty_3
	call display_3_speed ;speed= 8

    check_2_speed_display: ; Check if speed is equal to 7
	movlw 2
	subwf speed, 0
	btfss STATUS, 2
	goto check_1_speed_display
	call display_p
	call wait_loop
	call display_empty_2
	call display_empty_3
	call display_2_speed ;speed= 8
	
    check_1_speed_display: ; Check if speed is equal to 7
	movlw 1
	subwf speed, 0
	btfss STATUS, 2
	return
	call display_p
	call wait_loop
	call display_empty_2
	call display_empty_3
	call display_1_speed ;speed= 8
	
    return

 
;timer 50 must overflow after 50ms
start_timer1:
    ;LIT UP LEDS HERE
    movlw 0
    cpfseq paused
    return
    clrf TMR1H
    clrf TMR1L
    
    movlw 1
    btfsc bar_begin_flag, 0 ;RC1 -> bar start, RC0 -> beat
    call lit_up_two
    bsf LATC, 0
    
    movlw 0x003C
    movwf TMR1H
    movlw 0x00B0 ;3CB0
    movwf TMR1L ; initialized timer1 to 15536.
    clrf PIR1, 0
    bsf PIE1, 0
    movlw 11001001B
    movwf T1CON
    return
    
lit_up_two:
    movlw 00000011B ;TO LIGHT UP LEDS 
    movwf LATC
    clrf bar_begin_flag
    return
  
    
timer0_interrupt:
    bcf INTCON, 2 ; clear the timer0 interrupt flag
    
    call timer_start
    movff bar_length, WREG
    subwf beat,0  ; Compare bar_length with beat
    btfsc STATUS, 2
    goto set_beat_to_1 ; If bar_length == beat, set beat to 1
    
    incf beat ; Else, increment beat
    
    call start_timer1
    retfie 1
   
set_beat_to_1:
    movlw 1
    movwf beat
    movlw 1
    movwf bar_begin_flag
    call start_timer1
    retfie 1
    
timer1_interrupt:
    bcf PIR1, 0 ; clear the timer1 interrupt flag
    ;lit off leds
    movlw 00000000B
    movwf LATC
    bcf T1CON, 0
    retfie 1 


first_bar:
    movlw 1
    movwf beat
    movlw 1
    movwf bar_begin_flag
    
    call start_timer1
    call display_empty_4
    call display_dash ;DIS3
    
    call display_beat_value ;DIS2
    call wait_loop
    call display_barlength_value ;DIS4
    
    
    return
    
;STARTING TIMER0   
timer_start:
   
    movlw 9
    subwf speed, 0
    btfss STATUS, 2
    goto check_8_speed
    call start_200 ;speed=9

    check_8_speed: ; 
	movlw 8
	subwf speed, 0
	btfss STATUS, 2
	goto check_7_speed
	call start_300 ;speed=8

	
    check_7_speed: ; 
	movlw 7
	subwf speed, 0
	btfss STATUS, 2
	goto check_6_speed
	call start_400 ;speed=7
	
	
    check_6_speed: ; 
	movlw 6
	subwf speed, 0
	btfss STATUS, 2
	goto check_5_speed
	call start_500 ;speed=6

    check_5_speed: ;
	movlw 5
	subwf speed, 0
	btfss STATUS, 2
	goto check_4_speed
	call start_600 ;speed=5

    check_4_speed: 
	movlw 4
	subwf speed, 0
	btfss STATUS, 2
	goto check_3_speed
	call start_700 ;speed=4

    check_3_speed: 
	movlw 3
	subwf speed, 0
	btfss STATUS, 2
	goto check_2_speed
	call start_800 ;speed=3

    check_2_speed: ;
	movlw 2
	subwf speed, 0
	btfss STATUS, 2
	goto check_1_speed
	call start_900 ;speed=2
	
    check_1_speed: ;
	movlw 1
	subwf speed, 0
	btfss STATUS, 2
	return
	call start_1s ;speed = 1
    return
    
    
    
    
    
;STARTING TIMER0: for arranging speed levels
;speed level 1
    
start_1s:
    clrf TMR0H
    clrf TMR0L
    
    movlw 0x0B
    movwf TMR0H ; initialized timer0 to 3036. 0x0BDC
    
    movlw 0xDC
    movwf TMR0L
    
    movlw 10000011B ; T0ON is 1, it's a 16 bit counter. Prescaler set to 1:16 -> 011
    movwf T0CON
    
    return
    
;speed level 2
start_900:
    clrf TMR0H
    clrf TMR0L
    
    movlw 0x24
    movwf TMR0H ; initialized timer0 to 9286. 0x2446
    
    movlw 0x46
    movwf TMR0L
    
    movlw 10000011B ; T0ON is 1, it's a 16 bit counter. Prescaler set to 1:16 -> 011
    movwf T0CON
    
    return
;speed level 3
start_800:
    clrf TMR0H
    clrf TMR0L
    
    movlw 0x3C
    movwf TMR0H ; initialized timer0 to 15536. 0x3CB0
    
    movlw 0xB0
    movwf TMR0L
    
    movlw 10000011B ; T0ON is 1, it's a 16 bit counter. Prescaler set to 1:16 -> 011
    movwf T0CON
    
    return
;speed level 4
start_700:
    clrf TMR0H
    clrf TMR0L
    
    movlw 0x55
    movwf TMR0H ; initialized timer0 to 21786.0x551A
    
    movlw 0x1A
    movwf TMR0L ; initialized timer0 to 21786.0x551A
    
    movlw 10000011B ; T0ON is 1, it's a 16 bit counter. Prescaler set to 1:16 -> 011
    movwf T0CON
    
    return
;speed level 5
start_600:
    clrf TMR0H
    clrf TMR0L
    
    movlw 0x6D
    movwf TMR0H ; initialized timer0 to 28036. 0x6D84
    
    movlw 0x84
    movwf TMR0L ; initialized timer0 to 28036. 0x6D84
    
    movlw 10000011B ; T0ON is 1, it's a 16 bit counter. Prescaler set to 1:16 -> 011
    movwf T0CON
    
    return
;speed level 6
start_500:
    clrf TMR0H
    clrf TMR0L
    
    movlw 0x0B
    movwf TMR0H ; initialized timer0 to 3036.0X0BDC
    
    movlw 0xDC
    movwf TMR0L ; initialized timer0 to 3036.0X0BDC
    
    movlw 10000010B ; T0ON is 1, it's a 16 bit counter. Prescaler set to 1:8 -> 010
    movwf T0CON
    
    return
;speed level 7
start_400:
    clrf TMR0H
    clrf TMR0L
    
    movlw 0x3C
    movwf TMR0H ; initialized timer0 to 15536.0x3CB0
    
    movlw 0xB0
    movwf TMR0L ; initialized timer0 to 15536.0x3CB0
    
    movlw 10000010B ; T0ON is 1, it's a 16 bit counter. Prescaler set to 1:8 -> 010
    movwf T0CON
    
    return
;speed level 8
start_300:
    clrf TMR0H
    clrf TMR0L
    
    movlw 0x6D
    movwf TMR0H ; initialized timer0 to 28036. 0x06D84
    
    movlw 0x84
    movwf TMR0L ; initialized timer0 to 28036. 0x06D84
    
    movlw 10000010B ; T0ON is 1, it's a 16 bit counter. Prescaler set to 1:8 -> 010
    movwf T0CON
    
    return
;speed level 9
start_200:
    clrf TMR0H
    clrf TMR0L
    
    movlw 0x3C
    movwf TMR0H ; initialized timer0 to 15536. 0x3CB0
    
    movlw 0xB0
    movwf TMR0L ; initialized timer0 to 15536. 0x3CB0
    
    movlw 10000001B ; T0ON is 1, it's a 16 bit counter. Prescaler set to 1:4 -> 001
    movwf T0CON
    
    return
    

    
;TO DISPLAY BEAT AND BAR LENGTH

display_beat_value:
    movlw 8
    subwf beat, 0
    btfss STATUS, 2
    goto check_7_beat
    call display_8 ;beat= 8

    check_7_beat: ; Check if beat is equal to 7
	movlw 7
	subwf beat, 0
	btfss STATUS, 2
	goto check_6_beat
	call display_7 ;beat= 7

    check_6_beat: ; Check if beat is equal to 6
	movlw 6
	subwf beat, 0
	btfss STATUS, 2
	goto check_5_beat
	call display_6  ;bar_length= 6

    check_5_beat: ; Check if beat is equal to 5
	movlw 5
	subwf beat, 0
	btfss STATUS, 2
	goto check_4_beat
	call display_5  ;beat= 5

    check_4_beat: ; Check if beat is equal to 4
	movlw 4
	subwf beat, 0
	btfss STATUS, 2
	goto check_3_beat
	call display_4 ;beat= 4

    check_3_beat: ; Check if beat is equal to 3
	movlw 3
	subwf beat, 0
	btfss STATUS, 2
	goto check_2_beat
	call display_3  ;beat= 3

    check_2_beat:    ; Check if beatis equal to 2
	movlw 2
	subwf beat, 0
	btfss STATUS, 2
	goto check_1_beat
	call display_2  ;beat= 2
	
    check_1_beat:    ; Check if beat is equal to 2
	movlw 1
	subwf beat, 0
	btfss STATUS, 2
	return
	call display_1;beat=1
	
    return

 display_barlength_value:
    ; Check if bar_length is equal to 8
    movlw 8
    subwf bar_length, 0
    btfss STATUS, 2
    goto check_7
    call display_8_bar ;bar_length= 8
    return

    check_7: ; Check if bar_length is equal to 7
	movlw 7
	subwf bar_length, 0
	btfss STATUS, 2
	goto check_6
	call display_7_bar ;bar_length= 7
	return

    check_6: ; Check if bar_length is equal to 6
	movlw 6
	subwf bar_length, 0
	btfss STATUS, 2
	goto check_5
	call display_6_bar  ;bar_length= 6
	return

    check_5: ; Check if bar_length is equal to 5
	movlw 5
	subwf bar_length, 0
	btfss STATUS, 2
	goto check_4
	call display_5_bar  ;bar_length= 5
	return

    check_4: ; Check if bar_length is equal to 4
	movlw 4
	subwf bar_length, 0
	btfss STATUS, 2
	goto check_3
	call display_4_bar  ;bar_length= 4
	return

    check_3: ; Check if bar_length is equal to 3
	movlw 3
	subwf bar_length, 0
	btfss STATUS, 2
	goto check_2
	call display_3_bar  ;bar_length= 3
	return

    check_2:    ; Check if bar_length is equal to 2
	movlw 2
	subwf bar_length, 0
	btfss STATUS, 2
	goto check_1
	call display_2_bar  ;bar_length= 2
	return
	
    check_1:    ; Check if bar_length is equal to 2
	movlw 1
	subwf bar_length, 0
	btfss STATUS, 2
	return
	call display_1_bar;bar_length=1
	
    return


    
    
; ---TO DISPLAY BEAT:
    
display_p:
    movlw 00000001B
    movwf LATA 
    clrf LATD
    movlw 01110011B
    movwf LATD
    return
    
display_dash:
    movlw 00000100B
    movwf LATA
    clrf LATD
    movlw 01000000B
    movwf LATD
    return
    
display_1:
    movlw 00000010B
    movwf LATA
    clrf LATD
    movlw 00000110B
    movwf LATD
    return
    
display_2:
    movlw 00000010B
    movwf LATA
    clrf LATD
    movlw 01011011B
    movwf LATD
    return
    
display_3:
    movlw 00000010B
    movwf LATA
    clrf LATD
    movlw 01001111B
    movwf LATD
    return
    
display_4:
    movlw 00000010B
    movwf LATA
    clrf LATD
    movlw 01100110B
    movwf LATD
    return
    
display_5:
    movlw 00000010B
    movwf LATA
    ;01101101B
    clrf LATD
    movlw 001101101B
    movwf LATD
    return
    
display_6:
    movlw 00000010B
    movwf LATA
    clrf LATD
    movlw 01111101B
    movwf LATD
    return
    
display_7:
    movlw 00000010B
    movwf LATA
    clrf LATD
    movlw 00000111B
    movwf LATD
    return
    
display_8:
    movlw 00000010B
    movwf LATA
    clrf LATD
    movlw 01111111B
    movwf LATD
    return
    
;DISPLAY FOR BAR
display_1_bar:
    movlw 00001000B
    movwf LATA
    clrf LATD
    movlw 00000110B
    movwf LATD
    return
    
display_2_bar:
    movlw 00001000B
    movwf LATA
    clrf LATD
    movlw 01011011B
    movwf LATD
    return
    
display_3_bar:
    movlw 00001000B
    movwf LATA
    clrf LATD
    movlw 01001111B
    movwf LATD
    return
    
display_4_bar:
    movlw 00001000B
    movwf LATA
    clrf LATD
    movlw 01100110B
    movwf LATD
    return
    
display_5_bar:
    movlw 00001000B
    movwf LATA
    clrf LATD
    movlw 01101101B
    movwf LATD
    return
    
display_6_bar:
    movlw 00001000B
    movwf LATA
    clrf LATD
    movlw 01111101B
    movwf LATD
    return
    
display_7_bar:
    movlw 00001000B
    movwf LATA
    clrf LATD
    movlw 00000111B
    movwf LATD
    return
    
display_8_bar:
    movlw 00001000B
    movwf LATA
    clrf LATD
    movlw 01111111B
    movwf LATD
    return
    
;DISPLAY FOR SPEED
display_1_speed:
    movlw 00001000B
    movwf LATA 
    clrf LATD
    movlw 00000110B
    movwf LATD
    return
display_2_speed:
    movlw 00001000B
    movwf LATA 
    clrf LATD
    movlw 01011011B
    movwf LATD
    return
display_3_speed:
    movlw 00001000B
    movwf LATA 
    clrf LATD
    movlw 01001111B
    movwf LATD
    return
display_4_speed:
    movlw 00001000B
    movwf LATA 
    clrf LATD
    movlw 01100110B
    movwf LATD
    return
display_5_speed:
    movlw 00001000B
    movwf LATA
    clrf LATD
    movlw 01101101B
    movwf LATD
    return
    
display_6_speed:
    movlw 00001000B
    movwf LATA 
    clrf LATD
    movlw 01111101B
    movwf LATD
    return
    
display_7_speed:
    movlw 00001000B
    movwf LATA 
    clrf LATD
    movlw 00000111B
    movwf LATD
    return
    
display_8_speed:
    movlw 00001000B
    movwf LATA 
    clrf LATD
    movlw 01111111B
    movwf LATD
    return
    
display_9_speed:
    movlw 00001000B
    movwf LATA 
    clrf LATD
    movlw 01101111B
    movwf LATD
    return
    
display_empty_1:
    movlw 00000001B ; DIS4
    movwf LATA
    clrf LATD
    movlw 00000000B
    movwf LATD
    return
    
display_empty_2:
    movlw 00000010B ; DIS3
    movwf LATA
    clrf LATD
    movlw 00000000B
    movwf LATD
    return
    
display_empty_3:
    movlw 00000100B ; DIS2
    movwf LATA
    clrf LATD
    movlw 00000000B
    movwf LATD
    return
    
display_empty_4:
    movlw 00001000B ;DIS1
    movwf LATA
    clrf LATD
    movlw 00000000B
    movwf LATD
    return
    
    
