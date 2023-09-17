#ifndef RELAIS_H_
#define RELAIS_H_

#define RELAIS_MAX_ACTIVE		3		// wieviele max. aktiv sein dürfen
#define RELAIS_ACT_TIME			60		// default zeit [s] aktiv

// init
void relais_init(void);

// schickt daten raus
void relais_update(void);

// periodisch aufrufen
void relais_tick(void);

// relais prüfen
uint8_t relais_isActive(uint8_t rel_num);

// relais einschalten; rel_num 0...7, sec zeit in sek bis es aus geht
void relais_activate(uint8_t rel_num, uint16_t sec);

// relais abschalten
void relais_deactivate(uint8_t rel_num);
void relais_deactivate_all(void);

#endif /* RELAIS_H_ */