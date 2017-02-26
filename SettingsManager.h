#include <iostream>
#include <windows.h>
#include <fstream>
#include <string>
#include "resource.h"



#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

class SettingsManager{

public:
	/**Costruttore di default*/
	SettingsManager();
	/**Carica tutte le opzioni relative al programma.
	@param hwnd Handle della finestra del programma.
	*/
	void loadOptions(HWND hwnd);
	/**Salva tutte le opzioni relative al programma.
	@param hwnd Handle della finestra del programma.
	*/
	void saveOptions(HWND hwnd);
	
};

#endif