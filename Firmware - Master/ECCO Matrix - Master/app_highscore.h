#ifndef APP_HIGHSCORE_H_
#define APP_HIGHSCORE_H_

#define HIGHSCORE_NAME_MAX		5
#define HIGHSCORE_MAX			5

typedef struct {
	char name[6];
	uint32_t punkte;
} app_highscore_t;

extern app_highscore_t highscore_snake[HIGHSCORE_MAX], highscore_spiel1[HIGHSCORE_MAX];

// aus eeprom ins ram
void app_highscore_init(void);

// ist neuer highscore?
uint8_t app_highscore_isHS(app_highscore_t *l, uint32_t p);

// ausgabe top 5 in zeile 1 ... 5 (0, 6, 7 frei)
void app_highscore_print(app_highscore_t *l);

// punkte hinzufügen mit name in hs_name
void app_highscore_addHS(app_highscore_t *l, uint32_t punkte);

// daten ins eeprom
void app_highscore_save_spiel1(void);
void app_highscore_save_snake(void);

// init für step
void app_highscore_step_init(void);
// step zum namen einlesen return 1 wenn eingabe fertig, sonst 0
uint8_t app_highscore_step(void);

#endif /* APP_HIGHSCORE_H_ */