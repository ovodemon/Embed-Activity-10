/**********************************************************************/
// ENGR-2350 Avtivity-PWM
// Name: Yijia Zhou
// RIN: 661995479
// This is the base project for several activities and labs throughout
// the course.  The outline provided below isn't necessarily *required*
// by a C program; however, this format is required within ENGR-2350
// to ease debugging/grading by the staff.
/**********************************************************************/

// We'll always add this include statement. This basically takes the
// code contained within the "engr_2350_msp432.h" file and adds it here.
#include "engr2350_msp432.h"

// Add function prototypes here, as needed.
void GPIO_Init();
void Timer_Init();

// Add global variables here, as needed.
Timer_A_UpModeConfig Timer;
Timer_A_CompareModeConfig CCR3;
Timer_A_CompareModeConfig CCR4;
uint16_t rightspeed = 0;
uint16_t leftspeed = 0;


int main(void) /* Main Function */
{
   // Add local variables here, as needed.

    // We always call the "SysInit()" first to set up the microcontroller
    // for how we are going to use it.
    SysInit();
    GPIO_Init();
    Timer_Init();
    // Place initialization code (or run-once) code here

    while(1){
        // Place code that runs continuously in here
        uint8_t cmd = getchar();
        if (cmd == 'a'){
            leftspeed += 50;
            if (leftspeed >= 0.9*65535){
                leftspeed = 0.9*65535;
            }
            Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_4, leftspeed);
        }else if (cmd == 'z'){
            if (leftspeed <= 50){
                leftspeed = 0;
            }else{
                leftspeed -= 50;
            }
            Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_4, leftspeed);
        }else if (cmd == 's'){
            rightspeed += 50;
            if (rightspeed >= 0.9*65535){
                rightspeed = 0.9*65535;
            }
            Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_3, rightspeed);
        }else if (cmd == 'x'){
            if (rightspeed <= 50){
                rightspeed = 0;
            }else{
                rightspeed -= 50;
            }
            Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_3, rightspeed);
        }
    }
}
// Add function declarations here as needed
void GPIO_Init(){
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN7|GPIO_PIN6);//Enable Pin  P3.7, P3.6
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN4|GPIO_PIN5);// Direction Pin  P5.4, P5.5
    GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN7|GPIO_PIN6);
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN4|GPIO_PIN5);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION);// PWM Pin  P2.7, P2.6
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN6, GPIO_PRIMARY_MODULE_FUNCTION);// PWM Pin  P2.7, P2.6
}

void Timer_Init(){
    //PWM
    Timer.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;// Frequency 24MHz
    Timer.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1; //Divider 1
    Timer.timerPeriod = 799; // 30kHz
    Timer_A_configureUpMode(TIMER_A0_BASE, &Timer);

    //CCR3
    CCR3.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_3; //CCR3  P2.6 Right
    CCR3.compareInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE;
    CCR3.compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET;
    CCR3.compareValue = 0; // %0
    Timer_A_initCompare(TIMER_A0_BASE, &CCR3);

    //CCR4
    CCR4.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_4; //CCR4  P2.7 Left
    CCR4.compareInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE;
    CCR4.compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET;
    CCR4.compareValue = 0; // %0
    Timer_A_initCompare(TIMER_A0_BASE, &CCR4);

    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);
}




// Add interrupt functions last so they are easy to find
