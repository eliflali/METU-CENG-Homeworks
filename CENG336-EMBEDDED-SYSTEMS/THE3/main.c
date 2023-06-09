/*
 * File:   main.c
 * Author: elifl
 *
 * Created on May 23, 2023, 7:01 PM
 */


#include "Pragmas.h" //some necessary configurations
#include "ADC.h"
#include "LCD.h"
#include "the3.h"
#include <string.h>
#include <stdio.h>

/* The definitions for 7 segment letters */
#define LETTER_0 0x3F
#define LETTER_1 0x06
#define LETTER_2 0x5B
#define LETTER_3 0x4F
#define LETTER_4 0x66
#define LETTER_5 0x6D
#define LETTER_6 0x7D
#define LETTER_NONE 0x00
#define DASH 0x40

unsigned char number_letters[6] = {
    LETTER_0,
    LETTER_1,
    LETTER_2,
    LETTER_3,
    LETTER_4,
    LETTER_5,
    
};

int isactive = 0; //INITIALLY NOT ACTIVE
int ishold = 0;
int isthrown = 0;
int target_on = 0;
int frisbee_count = 0;

int timer2_overflow = 0;
int timer3_overflow = 0;

int selected = 1; // if one holds frisbee, it has to be selected.
int holds = 0; // who holds the frisbee, if 0 -> no one holds the frisbee

unsigned char A1[2] = {3, 2};
unsigned char A2[2] = {3, 3};
unsigned char B1[2] = {14, 2};
unsigned char B2[2] = {14, 3};
unsigned char frisbee_position[2] = {9, 2};
unsigned char target_position[2] = {0,0};
unsigned short step_number;
unsigned char* player_positions[4] = {A1, A2, B1, B2};

unsigned short adc_value;
    

int team_a_score=0;
int team_b_score=0;
int LCD_flag = 0;

void timer0_start(void);
void timer1_start(void);
void timer2_start(void);
void timer3_start(void);

void rb0_interrupt(void);
void rb1_interrupt(void);
void rb4_interrupt(void);
void rb5_interrupt(void);
void rb6_interrupt(void);
void rb7_interrupt(void);

void indicate_movement(void);

void timer0_interrupt(void);
void timer1_interrupt(void);
void timer2_interrupt(void);
void timer3_interrupt(void);

void display_LCD(void);
void display_LED(void);

void init(void);


void throw_frisbee(void);
void blink_frisbee(void);
void set_cursor(void);
void check_hold(void);
void player_next_move(int player);

void delete_player(int player);
void delete_frisbee(unsigned char x, unsigned char y);
void delete_targetfrisbee(void);

void move_up(int player);
void move_right(int player);
void move_down(int player);
void move_left(int player);

void move_up_random(int player);
void move_right_random(int player);
void move_down_random(int player);
void move_left_random(int player);

void move_upright(int player);
void move_downright(int player);
void move_downleft(int player);
void move_upleft(int player);


void __interrupt() FNC()
{
    if(PIE1bits.TMR1IE && PIR1bits.TMR1IF)
    {
        PIR1bits.TMR1IF = 0; //interrupt flag cleaned
        timer1_start();
        return;
    }
    
    if(PIR1bits.ADIF && PIE1bits.ADIE)
    {
        PIR1bits.ADIF = 0; //CLEANED INTERRUPT
        adc_value = (unsigned short)((ADRESH << 8)+ADRESL);
    }
    
    if(INTCONbits.RBIE && INTCONbits.RBIF)
    {
        int rb4 = PORTBbits.RB4;
        int rb5 = PORTBbits.RB5;
        int rb6 = PORTBbits.RB6;
        int rb7 = PORTBbits.RB7;
        
        rb7 = rb7;
        INTCONbits.RBIF = 0;
        
        if(!rb4)
        {
            rb4_interrupt();
            check_hold();
        }
        else if(!rb5)
        {
           rb5_interrupt();
           check_hold();
        }
        else if(!rb6)
        {
            rb6_interrupt();
            check_hold();
        }
        else if(!rb7)
        {
            rb7_interrupt();
            check_hold();
        }
        LCD_flag = 1;
        return;
    }
    if(INTCONbits.TMR0IE && INTCONbits.TMR0IF)
    {
        INTCONbits.TMR0IF = 0;//CLEANED INTERRUPT
        timer0_interrupt();
        LCD_flag = 1;
        return;
    }
    if(PIE2bits.TMR3IE && PIR2bits.TMR3IF)
    {
        PIR2bits.TMR3IF = 0; //CLEANED INTERRUPT
        timer3_interrupt();
        return;
    }
    if(PIE1bits.TMR2IE && PIR1bits.TMR2IF)
    {
        PIR1bits.TMR2IF = 0; //CLEANED INTERRUPT
        timer2_interrupt();
        LCD_flag = 1;
        return;
    }

    if(INTCON3bits.INT1F && INTCON3bits.INT1E)
    {
        INTCON3bits.INT1F = 0;
        rb1_interrupt();
        LCD_flag = 1;
        return;
    }
   
    if(INTCONbits.INT0IF && INTCONbits.INT0IE)
    {
        INTCONbits.INT0IF = 0;
        rb0_interrupt();
        LCD_flag = 1;
        return;
    }
    
    return;
}
//THIS FUNC IS ABOUT MOVEMENT: NOT IMPLEMENTED YET
void timer0_interrupt()
{
    delete_frisbee(frisbee_position[0],frisbee_position[1]);
    frisbee_position[0] = frisbee_steps[frisbee_count][0];
    frisbee_position[1] = frisbee_steps[frisbee_count][1];
    
    
    if(frisbee_position[0] == target_position[0] && frisbee_position[1] == target_position[1])
    {
        frisbee_count = 0;
        isactive = 0;
        isthrown = 0;
        //target_on = 0;
        //delete_targetfrisbee();
        T2CONbits.TMR2ON = 0;
        T0CONbits.TMR0ON = 0;
        check_hold();
        return;
    }
    
    timer0_start();
    throw_frisbee();
    check_hold();
    frisbee_count++;
    return;
}

//SHOULD START TIMER 1 AGAIN. THIS FUNC IS TO GENERATE RANDOM VALUES
void timer1_interrupt()
{
    timer1_start();
    return;
}

//THIS FUNCTION IS ABOUT BLINKING TIME, HAS TO OVERFLOW 10 TIMES! 
//INCREASING TIMER2_OVERFLOW EVERY TIME I OVERFLOWS.
void timer2_interrupt() //COMPLETE
{
    while(timer2_overflow != 6)
    {
        timer2_overflow++;
        timer2_start();
        return;
    }
    timer2_overflow = 0;
    timer2_start();
    blink_frisbee();
    return;
}

void timer3_interrupt()
{
    if(timer3_overflow < 25)
    {
        timer3_overflow++;
        timer3_start();
        return;
    }
    else
    {
        timer3_overflow = 0;
        T3CONbits.TMR3ON = 0;
        INTCONbits.INT0IE = 1;
        INTCON3bits.INT1IE = 1;
        INTCONbits.RBIE = 1; 
    }
    
    return;
}
void rb0_interrupt() //NOT COMPLETED YET
{
    timer3_start();
    if(!isactive && ishold) //IF FRISBEE IS BEING HOLDED AND GAME IS NOT ACTIVE 
    {
        
        isactive = 1;
        ishold = 0;
        isthrown = 1;
        step_number = compute_frisbee_target_and_route(frisbee_position[0], frisbee_position[1]);
        frisbee_position[0] = frisbee_steps[0][0];
        frisbee_position[1] = frisbee_steps[1][1];
        frisbee_count++;
        target_position[0] = frisbee_steps[step_number][0];
        target_position[1] = frisbee_steps[step_number][1];
        timer2_start();
        timer0_start();
        blink_frisbee();
        throw_frisbee();
    }
    
    return;
    
}
void throw_frisbee() //NOT COMPLETED YET
{
    for(int i = 0; i<4; i++)
    {
        if(i+1 != selected)
        {
            player_next_move(i+1);
        }
    }
    return;
}

//START TIMER2
//display targeted point for frisbee
void blink_frisbee()
{
    if(isactive)
    {
        /*if(frisbee_position[0] == target_position[0] && frisbee_position[1] == target_position[1])
        {
            
            isthrown = 0;
            isactive = 0;
            target_on = 0;
            T0CONbits.TMR0ON = 0;
            T2CONbits.TMR2ON = 0;
            return;
        }*/
        if(!target_on)
        {
            target_on = 1; //LIGHT UP FRISBEE
        }
        else
        {
            target_on = 0; //DELETE FRISBEE
        }
        
    }
    
    return;
}


void rb1_interrupt() //IF THE FRISBEE IS NOT BEING HOLDED
{
    
    INTCONbits.INT0IE = 0;
    INTCON3bits.INT1IE = 0;
    INTCONbits.RBIE = 0; 
    timer3_start();
    if(!ishold)
    {
        if(selected<4)
        {
            selected++;
        }
        else
        {
            selected = 1;
        } 
    }
    return;
}

void rb4_interrupt() //COMPLETE
{
    timer3_start();
    move_up(selected);
    check_hold();
    return;
    
}
void rb5_interrupt() //COMPLETE
{
    timer3_start();
    move_right(selected);
    check_hold();
    return;
    
}
void rb6_interrupt() //COMPLETE
{
    timer3_start();
    move_down(selected);
    check_hold();
    return;
    
}
void rb7_interrupt() //COMPLETE  -> DONT FORGET TO MAKE HOLDS = 0 WHEN MOVE. -- DONE IN MOVE_ FUNCTIONS.
{
    timer3_start();
    move_left(selected);
    check_hold();
    return;
    
}

void check_hold()
{
    if(player_positions[selected-1][0] == frisbee_position[0]
            && player_positions[selected-1][1] == frisbee_position[1])
    {
        if(isthrown)
        {
            
            if(selected<=2)
            {
                team_a_score++;
            }
            else if(selected>2)
            {
                team_b_score++;
            }
            T2CONbits.TMR2ON = 0;
        }
        
        ishold = 1;
        holds = selected;
    }
    return;
}

//will be used for tracking time
void timer0_start() 
{
   INTCONbits.TMR0IF = 0;
   readADCChannel(0);
    //unsigned short adc_value = readADCChannel(0);
    
    T0CON = 0;
    TMR0H = 11;
    TMR0L = 220;
    T0CONbits.T08BIT = 0;
    T0CONbits.T0CS = 0;
    T0CONbits.T0SE = 0;
    T0CONbits.PSA = 0;
    T0CONbits.T0PS2 = 0;
    T0CONbits.T0PS1 = 1;
    T0CONbits.T0PS2 = 1;
    
    if(!adc_value || adc_value<255) //400 ms -> default value 
    {
        //T0ON is 1, it's a 16 bit counter. Prescaler set to 1:16 -> 011
        TMR0H = 11;
        TMR0L = 220; //TMR0->3036
        T0CONbits.T0PS2 = 0;
        T0CONbits.T0PS1 = 1;
        T0CONbits.T0PS2 = 1;
        
    }
    else if(adc_value<511) //800 ms
    {
      //T0ON is 1, it's a 16 bit counter. Prescaler set to 1:32 -> 100
        TMR0H = 11;
        TMR0L = 220; //TMR0->3036
        T0CONbits.T0PS2 = 1;
        T0CONbits.T0PS1 = 0;
        T0CONbits.T0PS2 = 0;
    }
    else if(adc_value<767) //1200 ms
    {
        //T0ON is 1, it's a 16 bit counter. Prescaler set to 1:64 -> 101
        TMR0H = 72;
        TMR0L = 229; //TMR0->18661
        T0CONbits.T0PS2 = 1;
        T0CONbits.T0PS1 = 0;
        T0CONbits.T0PS2 = 1;
    }
    else //1600 ms
    {
        //T0ON is 1, it's a 16 bit counter. Prescaler set to 1:64 -> 101
        TMR0H = 11;
        TMR0L = 220; //TMR0->3036
        T0CONbits.T0PS2 = 1;
        T0CONbits.T0PS1 = 0;
        T0CONbits.T0PS2 = 1;
    }
    T0CONbits.TMR0ON = 1;
    INTCONbits.TMR0IE = 1;
    return;
}
//will be used for random generator
void timer1_start() //COMPLETE
{
    PIE1bits.TMR1IE = 0; 
    T1CON = 0;
    TMR1H = 0;
    TMR1L = 0;
    T1CONbits.RD16 = 1;
    T1CONbits.T1RUN = 1;
    T1CONbits.T1CKPS1 = 0;
    T1CONbits.T1CKPS0 = 0;
    T1CONbits.T1OSCEN = 0;
    T1CONbits.T1SYNC = 0;
    T1CONbits.TMR1CS = 0;
    PIE1bits.TMR1IE = 1; //start timer1 again
    T1CONbits.TMR1ON = 1;
    return;
    
}
//TO MANAGE BLINKING -> 100 MS
void timer2_start()
{
    PIR1bits.TMR2IF = 0;
    T2CON = 0;
    
    TMR2 = 241;            
    PR2 = 255;             
    T2CONbits.T2CKPS = 2;  // Set Timer2 prescaler to 1:16
    T2CONbits.TOUTPS = 9;  // Set Timer2 postscaler to 1:10
    PIR1bits.TMR2IF = 0;   // Clear Timer2 interrupt flag
    PIE1bits.TMR2IE = 1;   // Enable Timer2 overflow interrupt
    T2CONbits.TMR2ON = 1;  // Turn on Timer2
    
    PIE1bits.TMR2IE = 1;
    return;
}
//TO STOP BOUNCING -> 400 ms
void timer3_start()
{
    PIR2bits.TMR3IF = 0;
    
    TMR3H = 0xf1;
    TMR3L = 0x15; //TMR0->3036
    T3CONbits.RD16 = 1;
    T3CONbits.T3CCP1 = 0;
    T3CONbits.T3CCP2 = 0;
    T3CONbits.TMR3CS = 0;
    T3CONbits.T3CKPS0 = 1; //-> prescaler = 1:8
    T3CONbits.T3CKPS1 = 1;
    
    T3CONbits.TMR3ON = 1;
    INTCONbits.INT0IE = 0;
    INTCON3bits.INT1IE = 0;
    INTCONbits.RBIE = 0;
    INTCON3bits.INT1F = 0;
    INTCONbits.RBIF = 0;
    PIE2bits.TMR3IE = 1;
    return;
}

void main(void)
{
    init();
    timer1_start(); //SHOW LEDS
    display_LCD();
    set_cursor();
    
    while(1)
    {
        display_LED();
        if(LCD_flag)
        {
            check_hold();
            display_LCD();
            set_cursor();
            LCD_flag = 0;
        }
    }
    
}

void set_cursor()
{
    LCDGoto(player_positions[(selected-1)][0], player_positions[(selected-1)][1]); //set the cursor on selected player.
    return;
}



void indicate_movement()
{
    return;
}

void display_LED()
{
    __delay_us(3);
    switch (team_a_score)
    {
        case 4:
            LATA=0b00001000; //disp2=ra3
            LATD = number_letters[4];
            break;
        case 0:
            LATA=0b00001000; //disp2=ra3
            LATD = number_letters[0];
            break;
        case 1:
            LATA=0b00001000; //disp2=ra3
            LATD = number_letters[1];
            break;
        case 2:
            LATA=0b00001000; //disp2=ra3
            LATD = number_letters[2];
            break;
        case 3:
            LATA=0b00001000; //disp2=ra3
            LATD = number_letters[3];
            break;
        case 5:
            LATA=0b00001000; //disp2=ra3
            LATD = number_letters[5];
            break;
        default:
            break;

    }
    __delay_us(3);
    int a = 1;
    switch(a)
    {
        case 1:
            LATA=0b00010000; //set for next display (dash))
            LATD=DASH;
            break;
        case 2:
            LATA=0b00010000; //set for next display (dash))
            LATD=DASH;
            break;
        case 3:
            LATA=0b00010000; //set for next display (dash))
            LATD=DASH;
            break;
        case 4:
            LATA=0b00010000; //set for next display (dash))
            LATD=DASH;
            break;
        default:
            LATA=0b00010000; //set for next display (dash))
            LATD=DASH;
            break;
    }
    
    __delay_us(3);
    switch (team_b_score)
    {
        case 0:
            LATA=0b00100000; //disp4=ra5 
            LATD = number_letters[0];
            break;
        case 2:
            LATA=0b00100000; //disp4=ra5 
            LATD = number_letters[2];
            break;
        case 1:
            LATA=0b00100000; //disp4=ra5 
            LATD = number_letters[1];
            break;
        
        case 3:
            LATA=0b00100000; //disp4=ra5 
            LATD = number_letters[3];
            break;
        case 4:
            LATA=0b00100000; //disp4=ra5 
            LATD = number_letters[4];
            break;
        case 5:
            LATA=0b00100000; //disp4=ra5 
            LATD = number_letters[5];
            break;
        default:
            break;   
            
     }
    
    return;
}

void display_LCD()
{
    // it happens only in inactive mode & who holds frisbee is selected
    // also means targeted fri is not blinked and displayed
    // frisbee is not being displayed
    if(ishold == 1)  //IF THE FRISBEE IS HELD, DO NOT SHOW FRISBEE OR TARGETED POINT 
    {
        for(int i = 0; i<4; i++)
        {
            if(holds == (i+1))
            {
                if(i<2)
                {
                    LCDGoto(player_positions[i][0], player_positions[i][1]);
                    LCDDat(4); //TEAM A PLAYER WITH FRISBEE
                }
                else
                {
                    LCDGoto(player_positions[i][0], player_positions[i][1]);
                    LCDDat(5); //TEAM B PLAYER WITH FRISBEE
                } 
            }
            else
            {
                if(i<2)
                {
                    LCDGoto(player_positions[i][0], player_positions[i][1]);
                    LCDDat(0); //TEAM A PLAYER WITHOUT FRISB & NOT SELECTED
                }
                else
                {
                    LCDGoto(player_positions[i][0], player_positions[i][1]);
                    LCDDat(1); //TEAM B PLAYER WITHOUT FRISB & NOT SELECTED
                } 
            }
        }  
    }
    // it may happen in either active or inactive mode. -- CASES MUST BE THE SAME 
    //frisbee has to be displayed
    //targeted frisbee is displayed if "thrown"
    else
    {
        for(int i = 0; i<4; i++)
        {
            
            if(selected == (i+1))
            {
                if(i<2)
                {
                    
                    LCDGoto(player_positions[i][0], player_positions[i][1]);
                    LCDDat(2); //TEAM A PLAYER SELECTED
                }
                else
                {
                    LCDGoto(player_positions[i][0], player_positions[i][1]);
                    LCDDat(3); //TEAM B PLAYER SELECTED
                } 
            }
            else
            {
                if(i<2)
                {
                    
                    LCDGoto(player_positions[i][0], player_positions[i][1]);
                    LCDDat(0); //TEAM A PLAYER WITHOUT FRISB & NOT SELECTED
                }
                else
                {
                    
                    LCDGoto(player_positions[i][0], player_positions[i][1]);
                    LCDDat(1); //TEAM B PLAYER WITHOUT FRISB & NOT SELECTED
                } 
            }
        }
        
        LCDGoto(frisbee_position[0], frisbee_position[1]); //DISPLAY FRISBEE
        LCDDat(6);
        
        if(isactive)
        {
            if(!target_on)
            {
                LCDGoto(target_position[0], target_position[1]);
                LCDDat(7);
            }
            else
            {
                delete_targetfrisbee();
            }
        }
    }
    return;
}

void delete_player(int player)
{
    LCDGoto(player_positions[player-1][0], player_positions[player-1][1]);
    LCDStr(" ");
    
    return;
}

void delete_frisbee(unsigned char x, unsigned char y)
{
    LCDGoto(x, y);
    LCDStr(" ");
    return;
}

void delete_targetfrisbee()
{
    LCDGoto(target_position[0], target_position[1]);
    LCDStr(" ");
    return;
}

//MOVE TO A DIRECTION
//CHECKED: IS THERE ANYONE IN RELATED CELL
//CHECKED: DOES PLAYER HOLD FRISBEE -> IF YES THEN HOLDS = 0 MEANS NOBODY HOLDS FRISBEE ANYMORE
void move_up(int player)
{
    
    if(player_positions[(player-1)][1]>1)
    {
        int equals = 0;
        for(int i = 0; i<4; i++) //CHECKING IF THERE IS ANYBODY IN RELATED CELL
        {
            if(player-1 != i) // DON'T CHECK THE PLAYER WHICH IS ABOUT TO BE MOVED.
            {
                if(player_positions[(player-1)][0] == player_positions[i][0] &&
                        player_positions[player-1][1]-1 == player_positions[i][1])
                {
                    equals = 1; //IF THERE IS SOMEBODY, EQUALS = 1
                }
            }
        }
        if(!equals) //NOBODY IN RELATED CELL
        {
            delete_player(player);       
            player_positions[(player-1)][1]--; //GO UP MEANS y-1
            if(holds == player)  //DOES THE SELECTED PLAYER HOLD FRISBEE?
            {
                holds = 0; 
                ishold = 0;
            }
        }
        return;
    }
    return;
}

void move_right(int player)
{
    
    if(player_positions[(player-1)][0]<16)
    {
        int equals = 0;
        for(int i = 0; i<4; i++) //CHECKING IF THERE IS ANYBODY IN RELATED CELL
        {
            if(player-1 != i)
            {
                if(player_positions[(player-1)][1] == player_positions[i][1] &&
                        player_positions[player-1][0]+1 == player_positions[i][0])
                {
                    equals = 1; //IF THERE IS SOMEBODY, EQUALS = 1
                }
            }
        }
        if(!equals)
        {
            delete_player(player);  
            player_positions[(player-1)][0]++; //GO RIGHT MEANS x+1 
            
            if(holds == player)
            {
                holds = 0;
                ishold = 0;
            }
        }
        return;
    }
    
    return;
}

void move_down(int player)
{
    
    if(player_positions[(player-1)][1]<4)
    {
        int equals = 0;
        for(int i = 0; i<4; i++) //CHECKING IF THERE IS ANYBODY IN RELATED CELL
        {
            if(player-1 != i)
            {
                if(player_positions[(player-1)][0] == player_positions[i][0] &&
                        player_positions[player-1][1]+1 == player_positions[i][1])
                {
                    equals = 1; //IF THERE IS SOMEBODY, EQUALS = 1
                }
            }
        }
        if(!equals)
        {
            delete_player(player);  
            player_positions[(player-1)][1]++; //GO DOWN MEANS y+1
            
            if(holds == player)
            {
                holds = 0;
                ishold = 0;
            }
        }
    }
    
    return;
}

void move_left(int player)
{
    if(player_positions[(player-1)][0]>1)
    {
        int equals = 0;
        for(int i = 0; i<4; i++) //CHECKING IF THERE IS ANYBODY IN RELATED CELL
        {
            if(player-1 != i)
            {
                if(player_positions[(player-1)][1] == player_positions[i][1] &&
                        player_positions[player-1][0]-1 == player_positions[i][0])
                {
                    equals = 1; //IF THERE IS SOMEBODY, EQUALS = 1
                }
            }
        }
        if(!equals)
        {
            delete_player(player);  
            player_positions[(player-1)][0]--; //GO LEFT MEANS x-1
            if(holds == player)
            {
                holds = 0;
                ishold = 0;
            }
        } 
    }
    
    return;
}

void move_up_random(int player)
{
    if(player_positions[(player-1)][1]>1)
    {
        int equals = 0;
        for(int i = 0; i<4; i++) //CHECKING IF THERE IS ANYBODY IN RELATED CELL
        {
            if(player-1 != i) // DON'T CHECK THE PLAYER WHICH IS ABOUT TO BE MOVED.
            {
                if(player_positions[(player-1)][0] == player_positions[i][0] &&
                        player_positions[player-1][1]-1 == player_positions[i][1])
                {
                    equals = 1; //IF THERE IS SOMEBODY, EQUALS = 1
                }
                if(player_positions[(player-1)][1]-1== frisbee_position[1] &&
                        player_positions[player-1][0] == frisbee_position[0])
                {
                    equals = 1;
                }
                if(player_positions[(player-1)][1]-1 == target_position[1] &&
                        player_positions[player-1][0] == target_position[0])
                {
                    equals = 1;
                }
            }
        }
        if(!equals) //NOBODY IN RELATED CELL
        {
            delete_player(player);  
            player_positions[(player-1)][1]--; //GO UP MEANS y-1
            if(holds == player)  //DOES THE SELECTED PLAYER HOLD FRISBEE?
            {
                holds = 0; 
                ishold = 0;
            }
            
        }
        return;
    }
    return;
}

void move_right_random(int player)
{
    if(player_positions[(player-1)][0]<16)
    {
        int equals = 0;
        for(int i = 0; i<4; i++) //CHECKING IF THERE IS ANYBODY IN RELATED CELL
        {
            if(player-1 != i)
            {
                if(player_positions[(player-1)][1] == player_positions[i][1] &&
                        player_positions[player-1][0]+1 == player_positions[i][0])
                {
                    equals = 1; //IF THERE IS SOMEBODY, EQUALS = 1
                }
                if(player_positions[(player-1)][1] == frisbee_position[1] &&
                        player_positions[player-1][0]+1 == frisbee_position[0])
                {
                    equals = 1;
                }
                if(player_positions[(player-1)][1] == target_position[1] &&
                        player_positions[player-1][0]+1 == target_position[0])
                {
                    equals = 1;
                }
            }
        }
        if(!equals)
        {
            delete_player(player);  
            player_positions[(player-1)][0]++; //GO RIGHT MEANS x+1  
            if(holds == player)
            {
                holds = 0;
                ishold = 0;
            }
        }
        return;
    }
    
    return;
}
void move_down_random(int player)
{
    if(player_positions[(player-1)][1]<4)
    {
        int equals = 0;
        for(int i = 0; i<4; i++) //CHECKING IF THERE IS ANYBODY IN RELATED CELL
        {
            if(player-1 != i)
            {
                if(player_positions[(player-1)][0] == player_positions[i][0] &&
                        player_positions[player-1][1]+1 == player_positions[i][1])
                {
                    equals = 1; //IF THERE IS SOMEBODY, EQUALS = 1
                }
                if(player_positions[(player-1)][1]+1 == frisbee_position[1] &&
                        player_positions[player-1][0] == frisbee_position[0])
                {
                    equals = 1;
                }
                if(player_positions[(player-1)][1]+1 == target_position[1] &&
                        player_positions[player-1][0] == target_position[0])
                {
                    equals = 1;
                }
            }
        }
        if(!equals)
        {
            delete_player(player);  
            player_positions[(player-1)][1]++; //GO DOWN MEANS y+1
            
            if(holds == player)
            {
                holds = 0;
                ishold = 0;
            }
        }
    }
    
    return;
}

void move_left_random(int player)
{
    if(player_positions[(player-1)][0]>1)
    {
        int equals = 0;
        for(int i = 0; i<4; i++) //CHECKING IF THERE IS ANYBODY IN RELATED CELL
        {
            if(player-1 != i)
            {
                if(player_positions[(player-1)][1] == player_positions[i][1] &&
                        player_positions[player-1][0]-1 == player_positions[i][0])
                {
                    equals = 1; //IF THERE IS SOMEBODY, EQUALS = 1
                }
                if(player_positions[(player-1)][1] == frisbee_position[1] &&
                        player_positions[player-1][0]-1 == frisbee_position[0])
                {
                    equals = 1;
                }
                if(player_positions[(player-1)][1] == target_position[1] &&
                        player_positions[player-1][0]-1 == target_position[0])
                {
                    equals = 1;
                }
            }
        }
        if(!equals)
        {
            delete_player(player);  
            player_positions[(player-1)][0]--; //GO LEFT MEANS x-1
            if(holds == player)
            {
                holds = 0;
                ishold = 0;
            }
        } 
    }
    
    return;
}

void move_upright(int player)
{
    if(player_positions[(player-1)][0]<16 && player_positions[(player-1)][1]>1)
    {
        int equals = 0;
        for(int i = 0; i<4; i++) //CHECKING IF THERE IS ANYBODY IN RELATED CELL
        {
            if(player-1 != i)
            {
                if(player_positions[(player-1)][1]-1 == player_positions[i][1] &&
                        player_positions[player-1][0]+1 == player_positions[i][0])
                {
                    equals = 1; //IF THERE IS SOMEBODY, EQUALS = 1
                }
                if(player_positions[(player-1)][1]-1 == frisbee_position[1] &&
                        player_positions[player-1][0]+1 == frisbee_position[0])
                {
                    equals = 1;
                }
                if(player_positions[(player-1)][1]-1 == target_position[1] &&
                        player_positions[player-1][0]+1 == target_position[0])
                {
                    equals = 1;
                }
            }
        }
        if(!equals)
        {
            delete_player(player);  
            player_positions[(player-1)][0]++; //GO RIGHT MEANS x+1  
            player_positions[player-1][1]--;
        }
    }
    return;
}

void move_downright(int player)
{
    if(player_positions[(player-1)][0]<16 && player_positions[(player-1)][1]<16)
    {
        int equals = 0;
        for(int i = 0; i<4; i++) //CHECKING IF THERE IS ANYBODY IN RELATED CELL
        {
            if(player-1 != i)
            {
                if(player_positions[(player-1)][1]+1 == player_positions[i][1]+1 &&
                        player_positions[player-1][0]+1 == player_positions[i][0])
                {
                    equals = 1; //IF THERE IS SOMEBODY, EQUALS = 1
                }
                if(player_positions[(player-1)][1]+1 == frisbee_position[1]+1 &&
                        player_positions[player-1][0]+1 == frisbee_position[0])
                {
                    equals = 1;
                }
                if(player_positions[(player-1)][1]+1 == target_position[1]-1 &&
                        player_positions[player-1][0]+1 == target_position[0])
                {
                    equals = 1;
                }
            }
        }
        if(!equals)
        {
            delete_player(player);  
            player_positions[(player-1)][0]++; //GO RIGHT MEANS x+1  
            player_positions[player-1][1]++;
        }
    }
    
    return;
}

void move_downleft(int player)
{
    if(player_positions[(player-1)][0]>1 && player_positions[(player-1)][1]<16)
    {
        int equals = 0;
        for(int i = 0; i<4; i++) //CHECKING IF THERE IS ANYBODY IN RELATED CELL
        {
            if(player-1 != i)
            {
                if(player_positions[(player-1)][1]+1 == player_positions[i][1] &&
                        player_positions[player-1][0]-1 == player_positions[i][0])
                {
                    equals = 1; //IF THERE IS SOMEBODY, EQUALS = 1
                }
                if(player_positions[(player-1)][1]+1 == frisbee_position[1] &&
                        player_positions[player-1][0]-1 == frisbee_position[0])
                {
                    equals = 1;
                }
                if(player_positions[(player-1)][1]+1 == target_position[1] &&
                        player_positions[player-1][0]-1 == target_position[0])
                {
                    equals = 1;
                }
            }
        }
        if(!equals)
        {
            delete_player(player);  
            player_positions[(player-1)][0]--; //GO RIGHT MEANS x+1 
            player_positions[player-1][1]++;
        }
    }
    
    return;
}

void move_upleft(int player)
{
    if(player_positions[(player-1)][0]>1 && player_positions[(player-1)][1]>1)
    {
        int equals = 0;
        for(int i = 0; i<4; i++) //CHECKING IF THERE IS ANYBODY IN RELATED CELL
        {
            if(player-1 != i)
            {
                if(player_positions[(player-1)][1]-1 == player_positions[i][1] &&
                        player_positions[player-1][0]-1 == player_positions[i][0])
                {
                    equals = 1; //IF THERE IS SOMEBODY, EQUALS = 1
                }
                if(player_positions[(player-1)][1]-1 == frisbee_position[1] &&
                        player_positions[player-1][0]-1 == frisbee_position[0])
                {
                    equals = 1;
                }
                if(player_positions[(player-1)][1]-1 == target_position[1] &&
                        player_positions[player-1][0]-1 == target_position[0])
                {
                    equals = 1;
                }
            }
        }
        if(!equals)
        {
            delete_player(player);  
            player_positions[(player-1)][0]--; //GO RIGHT MEANS x+1  
            player_positions[player-1][1]--;
        }
    }
    
    return;
}

void player_next_move(int player)
{
    int number = random_generator(9);
    if(number == 0)
    {
        move_left_random(player);
    }
    else if(number == 1)
    {
        move_right_random(player);
    }
    else if(number == 2)
    {
        move_up_random(player);
    }
    else if(number == 3)
    {
        move_down_random(player);
    }
    else if(number == 4)
    {
        move_upleft(player);
    }
    else if(number == 5)
    {
        move_downleft(player);
    }
    else if(number == 6)
    {
        move_upright(player);
    }
    else if(number == 7)
    {
        move_downright(player);
    }
    else if(number == 8)
    {
        return;
    }
    return;
}

void init()
{
    initADC();
    InitLCD();
    LATA=0;
    LATD=0;
    INTCON2bits.RBPU = 0; //0 = PORTB pull-ups are enabled by individual port latch values 
    TRISBbits.TRISB0 = 1;  // RB0 as digital input
    TRISBbits.TRISB1 = 1;  // RB1 as digital input
    TRISBbits.TRISB4 = 1;  // RB4 as digital input
    TRISBbits.TRISB5 = 1;  // RB5 as digital input
    TRISBbits.TRISB6 = 1;  // RB6 as digital input
    TRISBbits.TRISB7 = 1;  // RB7 as digital input
    TRISAbits.TRISA0=1;
    TRISAbits.TRISA1=1;
    TRISAbits.TRISA2=1;
    TRISAbits.TRISA3=0;
    TRISAbits.TRISA4=0;
    TRISAbits.TRISA5=0;
    
    LCDAddSpecialCharacter(0, teamA_player);
    LCDAddSpecialCharacter(1, teamB_player);
    LCDAddSpecialCharacter(2, selected_teamA_player);
    LCDAddSpecialCharacter(3, selected_teamB_player);
    LCDAddSpecialCharacter(4, selected_teamA_player_with_frisbee);
    LCDAddSpecialCharacter(5, selected_teamB_player_with_frisbee);
    LCDAddSpecialCharacter(6, frisbee);
    LCDAddSpecialCharacter(7, frisbee_target);

    //INITIALIZATION OF VARIABLES JUST IN CASE.
    selected = 1;
    ishold = 0;
    isthrown = 0;
    isactive = 0;
    holds = 0;
    
    
    
    RCONbits.IPEN = 0;          // IPEN is cleared for enabling all peripheral interrupts in INTCON register.
    
    //PIE1bits.ADIE = 1;          // A/D interrupt enabled. 
    //GERI AC -> BUNU INTCON'UN USTUNE ALDIM GIE'DEN ONCE ENABLE ETMEK ICIN.
    
    INTCONbits.TMR0IF = 0;
    PIR1bits.TMR1IF = 0;
    PIR1bits.TMR2IF = 0;
    INTCONbits.INT0IF = 0;  
    INTCONbits.INT0IE = 1; //Enable INT0 pin interrupt
    INTCON3bits.INT1F = 0;
    INTCON3bits.INT1IE = 1; //Enable INT1 pin interrupt
    
    //MERVE HOCA'NIN SOYLEDIGINI DENEDIM.
    INTCONbits.TMR0IE = 1;
    INTCONbits.RBIE = 0;
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    PORTB = PORTB;
    INTCONbits.RBIF = 0;
    INTCONbits.RBIE = 1;
    
    return;
}
