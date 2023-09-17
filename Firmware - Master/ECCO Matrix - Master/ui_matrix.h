#ifndef UI_MATRIX_H_
#define UI_MATRIX_H_

#include <stdio.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

//
#define FLAG_DARK				0x01
#define FLAG_FLUOR_SPECIAL		0x02

// 0xF0 für relais reserviert
#define FLAG_CANACTIVATE		0x10

#define UIM_GETRELAIS(x)		((x & 0xE0) >> 5)

void uim_start(void);
void uim_input(void);
void uim_overlay(void);


#endif /* UI_MATRIX_H_ */