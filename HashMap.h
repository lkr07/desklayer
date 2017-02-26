#include <vector>
//#include <list>
#include <windows.h>


#ifndef HASHMAP_H
#define HASHMAP_H
/**Struttura di supporto che contiene la rappresentazione di una finestra ovvero il suo handle ed le coordinate del rettangolo occupato.
*/
struct WND{
	RECT rect;
	HWND hwnd;
	bool operator==(HWND other){
		if(other==hwnd)
			return true;
		return false;
	}

	bool operator==(WND other){
		if(other.hwnd==hwnd)
			return true;
		return false;
	}

};


/**Struttura dati di tipo HashMap basata sulla struttura WND.
@see WND.
*/
class HashMap{
private:
	/**Struttura di supporto che contiene i dati della "tabella hash".*/
	std::vector<WND> map;
public:
	/**Costruttore di default.*/
	HashMap();
	/**Cerca un elemento all'interno della struttura dati.
	@param key Handle alla finestra che fa da chiave nella tabella hash.
	@param element Puntatore al rettangolo che risulta essere il valore nella tabella hash.
	*/
	bool findElement(HWND key,LPRECT element);
	/**Cancella una coppia chiave-valore dalla struttura.
	@param key L'handle della finestra da cancellare dalla struttura che fa da chiave per la tabella hash.
	*/
	bool delKeyElement(HWND key);
	/**Inserisce una coppia chiave-valore nella struttura.
	@param key Handle di finestra che fa da chiave nella tabella della struttura.
	@param element Puntatore al rettangolo che rappresenta il valore nella tabella hash.
	*/
	bool insertKeyElement(HWND key,LPRECT element);
	/** Permette di associare un nuovo valore corrispondente ad una determinata chiave.
	@param key Handle a finestra e chiave nella tabella della struttura.
	@param newValue Nuovo valore da associare alla chiave, è un puntatore ad una struttura di tipo RECT
	*/
	bool modifyElement(HWND key,LPRECT newValue);
};

#endif