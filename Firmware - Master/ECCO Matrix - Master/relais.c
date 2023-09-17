#include <avr/io.h>
#include <stdlib.h>

#include "master.h"
#include "relais.h"

#define RELAIS_NONE			0xFF

uint8_t relais_state;			// ist-zustand; 1 - aktiv

struct {
	uint8_t relais_num;
	uint32_t time_off;
} relais_list[RELAIS_MAX_ACTIVE];

// init
void relais_init(void)
{
	uint8_t i;
	
	cbi(SR_PORT, (1 << SR_CLOCK) | (1 << SR_CLOCK) | (1 << SR_DATA));
	
	relais_state = 0;
	
	for(i = 0; i < RELAIS_MAX_ACTIVE; i++)
	{
		relais_list[i].relais_num = RELAIS_NONE;
		relais_list[i].time_off = 0;
	}
	
	relais_update();
	
	cbi(PORTD, (1 << PD7));
}

// schickt daten raus
void relais_update(void)
{
	uint8_t mask;
	
	mask = 1;
	while(mask) 
	{
		if(isset(relais_state, mask))
			sbi(SR_PORT, (1 << SR_DATA));
		else
			cbi(SR_PORT, (1 << SR_DATA));
		
		sbi(SR_PORT, (1 << SR_CLOCK));
		cbi(SR_PORT, (1 << SR_CLOCK));
		
		mask <<= 1;
	}

	sbi(SR_PORT, (1 << SR_LATCH));
	cbi(SR_PORT, (1 << SR_LATCH));
}

// relais einschalten; rel_num 0...7, sec zeit in sek bis es aus geht
void relais_activate(uint8_t rel_num, uint16_t sec)
{
	uint8_t i = 0, pos = 0xFF;
	uint32_t last_tick;
	
	if(rel_num > 7)
		return;
	
	// wenn bereits aktiv, zeit verlängern
	if(relais_isActive(rel_num))
	{
		for(i = 0; i < RELAIS_MAX_ACTIVE; i++)
		{
			if(relais_list[i].relais_num == rel_num)
			{
				relais_list[i].time_off = timestamp + (sec * 1000UL);
				return;
			}
		}		
	}
	
	// freien platz suchen
	for(i = 0; i < RELAIS_MAX_ACTIVE; i++)
	{
		if(relais_list[i].relais_num == RELAIS_NONE)
		{
			pos = i;
			break;
		}
	}
	
	if(pos == 0xFF)		// alle plätze belegt -> relais suchen dass als nächstes automatisch ausgeht
	{
		last_tick = (uint32_t)(relais_list[0].time_off - timestamp);
		pos = 0;
		for(i = 1; i < RELAIS_MAX_ACTIVE; i++)
		{
			if(relais_list[i].time_off - timestamp < last_tick)
			{
				last_tick = relais_list[i].time_off - timestamp;
				pos = i;
			}
		}
		
		relais_state &= ~(1 << relais_list[pos].relais_num);
	}
	
	relais_list[pos].relais_num = rel_num;
	relais_list[pos].time_off = timestamp + (sec * 1000UL);
	
	relais_state |= (1 << rel_num);
	
	relais_update();
}

// relais abschalten
void relais_deactivate(uint8_t rel_num)
{
	uint8_t i;
	
	if(rel_num > 7)
		return;
	
	for(i = 0; i < RELAIS_MAX_ACTIVE; i++)
	{
		if(relais_list[i].relais_num == rel_num)
		{
			relais_list[i].relais_num = RELAIS_NONE;
			break;
		}
	}
	
	relais_state &= ~(1 << rel_num);
	relais_update();
}

// alle relais abschalten
void relais_deactivate_all(void)
{
	uint8_t i;
	
	for(i = 0; i < RELAIS_MAX_ACTIVE; i++)
	{
		if(relais_list[i].relais_num != RELAIS_NONE)
		{
			relais_state &= ~(1 << relais_list[i].relais_num);
			relais_list[i].relais_num = RELAIS_NONE;
		}
	}

	relais_update();
}

// relais prüfen
uint8_t relais_isActive(uint8_t rel_num)
{
	if(rel_num > 7)
		return 0;
	
	return (relais_state & (1 << rel_num));
}

// periodisch aufrufen
void relais_tick(void)
{
	uint8_t i;
	
	for(i = 0; i < RELAIS_MAX_ACTIVE; i++)
	{
		if(relais_list[i].relais_num != RELAIS_NONE)
		{
			if(relais_list[i].time_off <= timestamp)
			{
				relais_state &= ~(1 << relais_list[i].relais_num);
				relais_list[i].relais_num = RELAIS_NONE;
				relais_update();
				return;
			}
		}
	}
}