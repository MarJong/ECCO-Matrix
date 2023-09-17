#ifndef PS2CONTROLLER_H_
#define PS2CONTROLLER_H_

typedef struct {
	// daten vom controller active low
	uint8_t btn1, btn2;					// digitale buttons
	uint8_t last_btn1, last_btn2;		// letzter zustand
	
	// nach auswertung active high
	uint8_t btn1_up, btn2_up;			// gerade losgelassen
	uint8_t btn1_down, btn2_down;		// gerade gedrückt
	
	uint8_t lx, ly; // linker analog stick
	uint8_t rx, ry; // rechter analog stick
	uint8_t vib1, vib2;	// kleiner vibrator (an/aus), grosser vibrator (0 ... 255)
} ps2_data_t;

extern ps2_data_t ps2_data;

// btn1
#define PS2_SELECT		0x01
#define PS2_R3			0x02
#define PS2_L3			0x04
#define PS2_START		0x08
#define PS2_UP			0x10
#define PS2_RIGHT		0x20
#define PS2_DOWN		0x40
#define PS2_LEFT		0x80

// btn2
#define PS2_L2			0x01
#define PS2_R2			0x02
#define PS2_L1			0x04
#define PS2_R1			0x08
#define PS2_TRIANGLE	0x10
#define PS2_CIRCLE		0x20
#define PS2_CROSS		0x40
#define PS2_SQUARE		0x80

// analog-stick-wert mittelstellung
// < links/unten; > rechts/oben
#define PS2_ANALOG_MID	0x7B

// init
void ps2_init(void);

/*
	daten pollen
	return 0 - fehler; 1 - erfolg
	nach erfolgreichem aufruf daten in ps2_data
*/
uint8_t ps2_poll();

/*
	vibration einstellen
	v1 - kleiner motor an/aus (1/0)
	v2 - grosser motor geschwindigkeit 0 ... 255
	steps - zeit in 1/100s
*/
void ps2_set_vibration(uint8_t v1, uint8_t v2, uint8_t steps);
void ps2_vibration_step(void);

#endif /* PS2CONTROLLER_H_ */