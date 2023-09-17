#ifndef LED_H_
#define LED_H_

typedef struct
{
	uint8_t r, g, b;
} led_data_t;

// max. farbwert
#define LED_COL_MAX		0x1F

// max. länge string für lauftext
#define LED_FONT_MAX_LEN	12
#define LED_FONT_DELAY		10
typedef struct {
	led_data_t col;					// farbe
	char text[LED_FONT_MAX_LEN];	// text
	uint8_t pos, subpos;			// char position (0 ... len), position in char (0 ... font_width + 1)
	uint8_t del;
} led_font_data_t;

extern led_data_t led_data_buffer[5][5], led_data[5][5];

// init
void led_init(void);

// buffer in ausgabe kopieren
void led_copy_buff(void);

// komplettes feld auf RGB setzen
void led_setRGB(uint8_t r, uint8_t g, uint8_t b);
// kopiert farbe src nach dest
void led_copyCol(led_data_t *dest, led_data_t *src);
// alles nach 1 links
void led_shiftLeft(void);
// alles nach 1 rechts
void led_shiftRight(void);
// alles nach 1 oben
void led_shiftUp(void);
// alles nach 1 unten
void led_shiftDown(void);

// (mehr oder weniger) kräftige zufallsfarbe
void led_getCol(led_data_t *col);

// hue in rgb umwandeln
void led_hue(uint8_t hue, led_data_t *led);

// nachbearbeitung vor ausgabe
// farben abdunkeln after-effect
void led_ae_darken(void);

void led_font_merge();
uint8_t led_font_step(void);
void led_font_start(char *text, uint8_t r, uint8_t g, uint8_t b);
void led_font_start_random();

#endif /* LED_H_ */