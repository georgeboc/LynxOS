#include "LY_Timer.hpp"
#include "LY_Ports.hpp"
#include "LY_Screen.hpp"
#include "LY_IDT.hpp"
#include "LY_ISR.hpp"
#include "LY_IRQ.hpp"

int LY_TIMER_TICKS=0;

int LY_TIMERS[100];
int LY_TIMERS_IDs[100];
int LY_TIMERS_IDs_Pointer=0;
int LY_TIMERS_BUFFER[100];
int LY_TIMERS_BUFFER_Pointer=0;
void LY::Timer::Phase(int hz)
{
	int divisor=1193180/hz;
	LY::Ports::OutputB(0x43,0x36);
	LY::Ports::OutputB(0x40, divisor & 0xFF);
	LY::Ports::OutputB(0x40, divisor >> 8);
}
void LY::Timer::Wait(int ticks)
{
	unsigned long eticks;
	eticks=LY_TIMER_TICKS+ticks;
	while(LY_TIMER_TICKS < eticks)
	{
		
	}
}
void LY::Timer::Setup()
{
	LY::Screen::SetColor(LY_SIDE_FOREGROUND, LY_COLOR_WHITE);
	LY::Screen::PutString("\nSetup timer...");
	
	LY_IRQ_InstallHandler(0,&LY_Timer_Handler);
	
	LY::Screen::SetColor(LY_SIDE_FOREGROUND,LY_COLOR_WHITE);
	LY::Screen::PutString("done");
}

void LY::Timer::Create(int id,int expire)
{
	LY_TIMERS[id]=expire;
	LY_TIMERS_IDs[LY_TIMERS_IDs_Pointer]=id;
	LY_TIMERS_IDs_Pointer++;
}

void LY::Timer::Destroy(int id)
{
	for(int l=0;l<=(LY_TIMERS_IDs_Pointer-id);l++)
	{
		if(l==LY_TIMERS_IDs_Pointer-id)
		{
			LY_TIMERS_IDs[id+l]=0;
		}
		else
		{
			LY_TIMERS_IDs[id+l]=LY_TIMERS_IDs[id+l+1];
		}
	}
	LY_TIMERS_IDs_Pointer--;
}

extern "C"
void LY_Timer_Handler(struct regs* r)
{
	LY_TIMER_TICKS++;
	if(LY_TIMER_TICKS % 18 == 0 && LY_TIMERS_IDs_Pointer>0)
	{
		for(int i=0;i<=LY_TIMERS_IDs_Pointer;i++)
		{
			LY_TIMERS[LY_TIMERS_IDs[i]]--;	
			if(LY_TIMERS[LY_TIMERS_IDs[i]]==0)
			{
				LY::Screen::SetColor(LY_SIDE_FOREGROUND,LY_COLOR_BROWN);
				LY::Screen::PutString("\nThe timer has expired id:"); //WE SHOULD DO A REFRESH SCREEN
				LY::Screen::itoa(LY_TIMERS_IDs[i]);
				LY_TIMERS_BUFFER[LY_TIMERS_BUFFER_Pointer]=i;
				LY_TIMERS_BUFFER_Pointer++;
				LY::Screen::SetColor(LY_SIDE_FOREGROUND,LY_COLOR_WHITE);
				LY::Screen::PutString("\n");
				//LY::Timer::Destroy(i);
			}		
		}
		for(int i=0;i<LY_TIMERS_BUFFER_Pointer;i++)
		{
			LY::Timer::Destroy(LY_TIMERS_BUFFER[LY_TIMERS_BUFFER_Pointer]);	
			LY_TIMERS_BUFFER[LY_TIMERS_BUFFER_Pointer]=0;
		}
		LY_TIMERS_BUFFER_Pointer=0;
		//LY::Screen::SetColor(LY_SIDE_FOREGROUND,LY_COLOR_BROWN);
		//LY::Screen::PutString("\nOne more second"); //WE SHOULD DO A REFRESH SCREEN
	}
}

