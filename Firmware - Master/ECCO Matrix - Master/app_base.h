#ifndef APP_BASE_H_
#define APP_BASE_H_

#define APP_DATA_SIZE		256

extern uint8_t app_data[APP_DATA_SIZE];

typedef struct {
	char name[21];
	void (*func_init)(void);
	void (*func_input)(void);
	void (*func_draw)(void);
} app_list_t;

// app num starten
void app_start(uint8_t num);

// app beenden
void app_end(void);

// app eingabe event
void app_input(void);

// app ausgabe
void app_draw(void);

// menü
void app_menuDraw(void);
void app_menuInput(void);
void app_menuStart(void);

#endif /* APP_BASE_H_ */