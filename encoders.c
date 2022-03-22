/**********************************************************************/
// ENGR-2350 Activity-Encoder
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
void Encoder_ISR();


// Add global variables here, as needed.
Timer_A_ContinuousModeConfig Timer;
Timer_A_CaptureModeConfig left_encoder;

uint32_t enc_total; // Keep track of total encoder events, useful in measuring distance traveled.
int32_t enc_counts_track; // Keep track the timer counts since the capture event (must be signed!).
int32_t enc_counts; // Final value of timer counts between capture events.
uint8_t enc_flag; // A flag to denote that a capture event has occurred.

uint16_t timer_reset_count = 0; // variable to count timer interrupts


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
        if(enc_flag){ // Check to see if capture occurred
            enc_flag = 0; // reset capture flag
            uint16_t distance = enc_total * 70 * 3.1415926 / 360; // Calculate distance travelled in mm
            uint16_t speed_rpm = 24000000 * 60 / (360 * enc_counts); // Calculate the instantaneous wheel speed in rpm
            uint16_t speed_mm = 70 * 3.1415926 * 24000000/(360 * enc_counts); // Calculate the instantaneous car speed in mm/s
            printf("%5u mm\t%5u rpm\t%5u mm/s\r\n",distance,speed_rpm,speed_mm);
        }
    }
}

// Add function declarations here as needed
void GPIO_Init(){
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P10, GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION); // TA3.CCI1A
}

void Timer_Init(){
    //PWM
    Timer.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;// Frequency 24MHz
    Timer.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1; //Divider 1
    Timer.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_ENABLE; // Timer interrupt enable

    Timer_A_configureContinuousMode(TIMER_A3_BASE, &Timer);
    Timer_A_registerInterrupt(TIMER_A3_BASE,TIMER_A_CCR0_INTERRUPT,Encoder_ISR);

    // CCR1 - left_encoder
    left_encoder.captureRegister = TIMER_A_CAPTURECOMPARE_REGISTER_1; // CCR1
    left_encoder.captureMode = TIMER_A_CAPTUREMODE_RISING_AND_FALLING_EDGE; // Both rising and falling edge
    left_encoder.captureInputSelect = TIMER_A_CAPTURE_INPUTSELECT_CCIxA;
    left_encoder.synchronizeCaptureSource = TIMER_A_CAPTURE_SYNCHRONOUS;
    left_encoder.captureInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE;
//    left_encoder.captureOutputMode = TIMER_A_OUTPUTMODE_TOGGLE_RESET;
    Timer_A_initCapture(TIMER_A3_BASE, &left_encoder);
    Timer_A_registerInterrupt(TIMER_A3_BASE,TIMER_A_CCRX_AND_OVERFLOW_INTERRUPT,Encoder_ISR);

    Timer_A_startCounter(TIMER_A3_BASE, TIMER_A_CONTINUOUS_MODE);
}

// Add interrupt functions last so they are easy to find
void Encoder_ISR(){
    if (Timer_A_getInterruptStatus(TIMER_A3_BASE)){
        Timer_A_clearInterruptFlag(TIMER_A3_BASE);
        enc_counts_track += 65536;
    }
    if (Timer_A_getCaptureCompareEnabledInterruptStatus(TIMER_A3_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1) == TIMER_A_CAPTURECOMPARE_INTERRUPT_FLAG){
        Timer_A_clearCaptureCompareInterrupt(TIMER_A3_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1);
        enc_total ++;
        uint16_t capture_value = Timer_A_getCaptureCompareCount(TIMER_A3_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1);
        enc_counts = enc_counts_track + capture_value;
        enc_counts_track = -(capture_value);
        enc_flag = 1;
    }
}

