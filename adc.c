#include "adc.h"
#include "nvic.h"

#include "stm32f042k6.h"

// Module-level pointer to an ADC callback function that is provided
// by the user. This function is called from the ADC interrupt 
// handler when a conversion completes. Set to 0 until registered
// in the adc_init() function below.
static void(*adc_cbfn_ptr)(ADC_CHANNEL_t channel, uint16_t data) = 0;

// enable the ADC and any IO pins used by the ADC using the bus clock (8 Mhz)
// configure for 12-bit sampling mode
void adc_init(void(*adc_cbfn)(ADC_CHANNEL_t channel, uint16_t data)) {

    // Registering the callback function pointer that is passed. 
    adc_cbfn_ptr = adc_cbfn;
    
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

    // Enable ADC peripheral's interrupts. 
    ADC->IER |= ADC_IER_EOCIE;
    
    // Enabling the ADC interrupts in NVIC Interrupt set-enable register
    NVIC_ISER |= NVIC_ISER_SETENA_ADC_COMP;

    // looking up these defined address values in the .h files made it 
    // very easy 
} 

// A modular-level static variable that will store the channel that was passed
// in the adc_convert function. 
static ADC_CHANNEL_t active_channel;

// initiate a conversion from the selected channel and wait for the result
ret_val_t adc_convert(ADC_CHANNEL_t channel) {

    // register the channel
    active_channel = channel;
    
    // Make sure the ADC is ready
    if( !(ADC->ISR & ADC_ISR_ADRDY ) )
        return RET_ERROR;

    // Configure the channel selection register 
    ADC->CHSELR = channel;

    // Start the conversion
    ADC->CR |= ADC_CR_ADSTART;

    return RET_SUCCESS;
}

// ADC interrupt handler
void ADC1_IRQHandler(void){
    // declaring a variable to store the converted data 
    uint16_t adc_data;
    // First, check if the interrupt was initiated by EOC interrupt
    if (ADC->ISR & ADC_ISR_EOC){
        // reading the converted data and clearing EOC flag at the same time. 
        adc_data = ADC->DR;

        // if the function pointer we defined at the start is valid, pass the
        // converted data and the channel it was converted from
        if (adc_cbfn_ptr){
            adc_cbfn_ptr(active_channel, adc_data);
        }
    }
}
