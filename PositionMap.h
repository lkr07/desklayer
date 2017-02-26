#include <vector>
//#include <list>
#include <windows.h>


#ifndef POSITIONMAP_H
#define POSITIONMAP_H
/**Struttura di supporto che associa un handle di finestra ad un intero che rappresenta una posizione nel layout.*/
struct WNDPOS{
	int  pos;
	HWND hwnd;
	bool operator==(HWND other){
		if(other==hwnd)
			return true;
		return false;
	}

	bool operator==(WNDPOS other){
		if(other.hwnd==hwnd)
			return true;
		return false;
	}

};


/**Struttura dati di tipo HasMap basata sulla struttura dati WNDPOS.*/
class PositionMap{
private:
	/**Vettore che rappresenta la "tabella hash".
	*/
	std::vector<WNDPOS> map;
public:
	/**Costruttore di default.*/
	PositionMap();
	/**Ricerca di un elemento nella struttura in base ad una chiave.
	@param key Handle di finestra che identifica  un valore nella struttura.
	@return Un intero che rappresenta la posizione della finestra nel layout.
	*/
	int findElement(HWND key);
	/**Cancella la coppia chiave-valore identificata da una chiave dalla struttura.
	@param key Handle della finestra che identifica la coppia chiave-valore da cancellare.
	@return True se la coppia è stata cancellata, false altrimenti.
	*/
	bool delKeyElement(HWND key);
	/**Inserisce una coppia chiave-valore.
	@param key L'handle di finestra che fa da chiave nella struttura.
	@param element Valore da registrare in coppia con la chiave specificata in key,
	@return True se la coppia è stata inserita, false altrimenti.
	*/
	bool insertKeyElement(HWND key,int element);
	/**Modifica il valore associato ad una chiave già esistente nella struttura.
	@param key Handle di finestra al quale si vuole associare un nuovo valore.
	@param newValue Nuovo valore intero da associare alla chiave.
	*/
	bool modifyElement(HWND key,int newValue);
	void clear();
};

#endif