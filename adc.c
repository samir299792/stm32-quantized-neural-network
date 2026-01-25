#include "adc.h"
#include "nvic.h"

#include "stm32f042k6.h"

// enable the ADC and any IO pins used by the ADC using the bus clock (8 Mhz)
// configure for 12-bit sampling mode
void adc_init(void) {

    // Set the correct sampling time for the input signal's impedance
    ADC->SMPR = ADC_SMPR_13_5;

    // Calibrate the ADC
    ADC->CR |= ADC_CR_ADCAL;
    // hardware clears the bit when calibration completes
    // if your program hangs and you hit break & are sitting on this 
    // line something is wrong with your request to calibrate... 
    // check the requirements in the user manual!
    while( ADC->CR & ADC_CR_ADCAL);

    // Enable the ADC
    ADC->CR |= ADC_CR_ADEN;

    // Wait for the ADC to become ready
    while( !(ADC->ISR & ADC_ISR_ADRDY) );
} 

// initiate a conversion from the selected channel and wait for the result
ret_val_t adc_convert(ADC_CHANNEL_t channel, uint16_t *result) {
    // Make sure the ADC is ready
    if( !(ADC->ISR & ADC_ISR_ADRDY ) )
        return RET_ERROR;

    // Configure the channel selection register 
    ADC->CHSELR = channel;

    // Start the conversion
    ADC->CR |= ADC_CR_ADSTART;

    // Wait for the result
    while( !(ADC->ISR & ADC_ISR_EOC) ); 
    *result = ADC->DR; // clears EOC flag

    return RET_SUCCESS;
}