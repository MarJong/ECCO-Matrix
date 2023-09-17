#ifndef UI_MATRIX_TEXTE_H_
#define UI_MATRIX_TEXTE_H_

#include <avr/io.h>
#include <avr/pgmspace.h>

#include "ui_matrix.h"


/*
	texte für projekte
	position
	1	2	3	4	5
	6	7	8	9	10
	11	12	13	14	15
	16	17	18	19	20
	21	22	23	24	25
	
	flags
	7 6 5 4 3 2 1 0
	R R R R?
*/

typedef struct {
	char title[21];
	uint8_t flags;
} proj_info_t;

const char PROGMEM projekt_texte[25][6][22] = 
	{
		// 1
		{
		//012345678901234567890
		{"Beitr\x84ge zum Motoren-"},
		{"wettbewerb in ET1."},
		{0},
		{0},
		{0},
		{0}
		},
		// 2
		{
		//012345678901234567890
		{"Linke H\x84lfte des"},
		{"Armaturenbretts"},
		{0},
		{0},
		{0},
		{0}
		},
		// 3
		{
		//012345678901234567890
		{"Steuereinheit und"},
		{"Motorsimulation zum"},
		{"testen"},
		{0},
		{0},
		{0}
		},
		// 4
		{
		//012345678901234567890
		{"Rechte H\x84lfte des"},
		{"Armaturenbretts"},
		{0},
		{0},
		{0},
		{0}
		},
		// 5
		{
		//012345678901234567890
		{"Metall, z.B. einen"},
		{"Schl\x81sselbund vor die"},
		{"Spule halten und"},
		{"bewegen."},
		{"Mit den Drehkn\x94pfen"},
		{"feinabstimmen"}
		},
		
		// 6
		{
		//012345678901234567890
		{"Dieser Lautsprecher"},
		{"ist lauter als der"},
		{"Rechte"},
		{0},
		{0},
		{0}
		},
		// 7
		{
		//012345678901234567890
		{"     Fachschaft"},
		{"   Elektrotechnik"},
		{"        und"},
		{"    Maschinenbau"},
		{0},
		{0}
		},
		// 8
		{
		//012345678901234567890
		{"   Elektronikclub"},
		{"       Cottbus"},
		{0},
		{0},
		{0},
		{0}
		},
		// 9
		{
		//012345678901234567890
		{"     Fachschaft"},
		{"    Informations"},
		{"        und"},
		{"    Medientechnik"},
		{0},
		{0}
		},
		// 10
		{
		//012345678901234567890
		{"Dieser Lautsprecher"},
		{"ist lauter als der"},
		{"Linke"},
		{0},
		{0},
		{0}
		},
		
		// 11
		{
		//012345678901234567890
		{"Die Idee wurde in"},
		{"einer Sitzung vorge-"},
		{"stellt und weiter-"},
		{"entwickelt"},
		{0},
		{0}
		},
		// 12
		{
		//012345678901234567890
		{"Arbeitsschritte"},
		{"Konzept entwickeln"},
		{"Spezifikationen fest-"},
		{" legen"},
		{"Bauteile suchen"},
		{"Schaltplan erstellen"}
		},
		// 13
		{
		//012345678901234567890
		{"Die Hauptplatine ist"},
		{"das Herz der Matrix."},
		{"Sie verwaltet alle"},
		{"Ein- und Ausgabe Op-"},
		{"erationen und verar-"},
		{"beitet diese."}
		},
		// 14
		{
		//012345678901234567890
		{"Platine ..."},
		{"  belichten"},
		{"  \x84tzen"},
		{"  verzinnen"},
		{"  best\x81""cken"},
		{"Prototyp fertig"}
		},
		// 15
		{
		//012345678901234567890
		{"Prototypen testen"},
		{"Fehler finden"},
		{"alles nochmal"},
		{0},
		{0},
		{0}
		},
		
		// 16
		{
		//012345678901234567890
		{"Misslungene Platinen"},
		{"der LED-Treiber und"},
		{"aktulle LED Platinen."},
		{0},
		{0},
		{0}
		},
		// 17
		{
		//012345678901234567890
		{"Mit diesen Folien"},
		{"werden die Platinen"},
		{"belichtet, um den"},
		{"Fotolack zu l\x94sen"},
		{0},
		{0}
		},
		// 18
		{
		//012345678901234567890
		{"Tastenbelegung"},
		{"\xEA Steuerkreuz"},
		{"\xE6\xE7\xE8\xE9 Tasten rechts"},
		{"\xEF Start"},
		{0},
		{0}
		},
		// 19
		{
		//012345678901234567890
		{"Erste Version der"},
		{"Hauptplatine."},
		{"Die fehlende Leitung"},
		{"wurde durch einen"},
		{"Kurzschluss gebraten"},
		{0}
		},
		// 20
		{
		//012345678901234567890
		{"Prototypen und auch"},
		{"finale Version der"},
		{"LED-Treiber Platinen"},
		{"und die alte Version"},
		{"der LED Platinen"},
		{0}
		},
		
		// 21
		{
		//012345678901234567890
		{"Ein handels\x81""bliches"},
		{"ATX Netzteil"},
		{0},
		{0},
		{0},
		{0}
		},
		// 22
		{
		//012345678901234567890
		{"Du willst eine"},
		{"Projektidee umsetzen?"},
		{"Von Holz- und Metall-"},
		{"bearbeitung bis"},
		{"3D Druck ist alles"},
		{"m\x94glich."}
		},
		// 23
		{
		//012345678901234567890
		{"Ruhmeshalle der"},
		{"Matrixbauer"},
		{0},
		{"Danke an alle,"},
		{"die geholfen haben"},
		{0}
		},
		// 24
		{
		//012345678901234567890
		{"Der Beitrag"},
		{"des Fablab"},
		{"der Fahrradwerkstatt"},
		{"und des ECCO"},
		{"zur Stadtrallye 2013"},
		{0}
		},
		// 25
		{
		//012345678901234567890
		{"Eine Studie in Fluo-"},
		{"reszenz"},
		{"Eine UV-LED bringt"},
		{"Fluorescein aus einem"},
		{"gelben Textmarker"},
		{"zum leuchten"}
		}
	};

const proj_info_t projekt_info[25] PROGMEM = {
	{"Motorenwettbewerb", 0},
	{"Armaturen Links", 0},
	{"Melete", FLAG_CANACTIVATE | (2 << 5)},
	{"Armaturen Rechts", 0},
	{"Metalldetektor", FLAG_CANACTIVATE | (6 << 5)},
		
	{"Lautsprecher Links", 0},
	{"Sponsor FEM", 0},
	{"ECCO", 0},
	{"Sponsor IMT", 0},
	{"Lautsprecher Rechts", 0},
		
	{"Die Idee", 0},
	{"Die Planung", 0},
	{"Hauptplatine", 0},
	{"Die Umsetzung", 0},
	{"Fertig?!", 0},
		
	{"Platinen", 0},
	{"Belichtungsvorlagen", 0},
	{"Anleitung", 0},
	{"Prototyp Hauptplatine", 0},
	{"Prototyp LED-Platine", 0},
		
	{"Netzteil", 0},
	{"FabLabCb", 0},
	{"Ruhmeshalle", 0},
	{"Die Bombe", 0},
	{"Fluoreszenz", FLAG_CANACTIVATE | (7 << 5) | FLAG_FLUOR_SPECIAL}
};


#endif /* UI_MATRIX_TEXTE_H_ */