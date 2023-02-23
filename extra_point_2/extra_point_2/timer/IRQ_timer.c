/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include "lpc17xx.h"
#include "../tamagotchi.h"
#include "../GLCD/GLCD.h" 
#include "../TouchPanel/TouchPanel.h"

extern void do_tamagotchi_life_interrupt(void);
extern void do_tamagotchi_custom_animation(void);
extern void do_user_events_interrupt_touch_panel(void);
extern void do_play_interrupt(void);

/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

void TIMER0_IRQHandler (void)
{
	do_tamagotchi_custom_animation();
  LPC_TIM0->IR = 1;			/* clear interrupt flag */
}

/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER1_IRQHandler (void)
{
  do_tamagotchi_life_interrupt();
	LPC_TIM1->IR = 1;			/* clear interrupt flag */
}

void TIMER2_IRQHandler (void)
{
  do_user_events_interrupt_touch_panel();
	LPC_TIM2->IR = 1;			/* clear interrupt flag */
}

void TIMER3_IRQHandler (void)
{
  do_play_interrupt();
	LPC_TIM3->IR = 1;			/* clear interrupt flag */
}

/******************************************************************************
**                            End Of File
******************************************************************************/
