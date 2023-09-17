#ifndef UI_LCD_H_
#define UI_LCD_H_

#define MENU_ITEM_NAME_MAX		21

typedef struct  {
	char name[MENU_ITEM_NAME_MAX];
	void (*start)();
	void (*input)();
	void *data;
} menu_entry_t;

typedef struct {
	char name[21];
	uint8_t pos, max;
	menu_entry_t *item;
} menu_t;

// starten/init
void uil_start(void);

// eingabe auswerten
void uil_input(void);

// func beenden und wieder in menü
void uil_endFunc(void);
void uil_startFunc(void);


#endif /* UI_LCD_H_ */