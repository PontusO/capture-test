
#include <stdio.h>
#include <sam.h>
#include "compiler.h"
#include "main.h"


#define ACTIVITY_LED	PORT_PA16
#define BUTTON          PIN_PA14
#define BUTTON_MASK     (1U << (BUTTON & 0x1f))
volatile PortGroup *port = 0;

int main(void)
{
    /* -- Initialize GPIO (PORT) */

    port = (volatile PortGroup *)&PORT->Group[0];
    port->DIRCLR.reg = (BUTTON_MASK);
    port->PINCFG[BUTTON & 0x1F].reg = PORT_PINCFG_INEN | PORT_PINCFG_PULLEN;
    port->OUTSET.reg = BUTTON_MASK;
    port->DIRSET.reg = (ACTIVITY_LED);
    port->OUTSET.reg = (ACTIVITY_LED);

    // - Set PB30 (LED) as TCC0 Waveform out (PMUX : E = 0x03)
    PORT->Group[1].WRCONFIG.reg = (uint32_t)(PORT_WRCONFIG_HWSEL | PORT_WRCONFIG_WRPINCFG | 
		PORT_WRCONFIG_WRPMUX | 1 << (30 - 16) | PORT_WRCONFIG_PMUXEN | (0x4 << PORT_WRCONFIG_PMUX_Pos) );

    

    /* -- Enable TCC0 Bus Clock and Generic clock*/
    // - Enable TCC0 Bus clock (Timer counter control clock)
    PM->APBCMASK.reg |= PM_APBCMASK_TCC0;

    //Enable GCLK for TCC0 (timer counter input clock)
    GCLK->CLKCTRL.reg = (uint16_t) ((GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | ( 0x1A << GCLK_CLKCTRL_ID_Pos)));

    /* -- Initialize TCC0 */
    // - DISABLE TCC0
    TCC0->CTRLA.reg &=~(TCC_CTRLA_ENABLE);

    // - Set TCC0 in waveform mode Normal PWM
    TCC0->WAVE.reg |= TCC_WAVE_WAVEGEN_NPWM;

    // - Set PER to maximum counter value (resolution : 0xFF)
    TCC0->PER.reg = 0xFF;

    // - Set WO[0] compare register to 0xFF (PWM duty cycle = 100%)
    TCC0->CC[0].reg = 0xFF;

    // - ENABLE TCC0
    TCC0->CTRLA.reg |= TCC_CTRLA_ENABLE;

    /* UART is enabled in all cases */
    usart_open();

    while(1) {
	if (port->IN.reg & BUTTON_MASK) {
            port->OUTTGL.reg = ACTIVITY_LED; 
            for(int i=0;i<100000;i++);
       } else {
            port->OUTCLR.reg = ACTIVITY_LED;
       }
    }
}
