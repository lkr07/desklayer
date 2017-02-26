#include <windows.h>
#include <list>
#include <cassert>
#include "HashMap.h"
#ifndef GRIDLAYOUT_H
#define GRIDLAYOUT_H




/** Classe che definisce il classico layout a griglia.*/
class GridLayout{

private:
	/**Numero di righe.
	*/
	int rows;
	/**Numero di colonne.
	*/
	int columns;
	/**Altezza dello schermo.
	*/
	int height;
	/**Larghezza dello schermo.
	*/
	int width;
	/**Lista contenente gli elementi del layout.
	*/
	std::list<HWND> matrix;
	/**Struttura per la memorizzazione delle posizioni iniziali delle finestre.*/
	HashMap initPositions;
	/**Struttura per la memorizzazione delle posizioni attuali delle finestre.*/
	HashMap regPositions;
	/**Numero di righe corrente.*/
	int curRows;
	/**Numero di colonne corrente.*/
	int curColumns;

public:
	/**Costruttore di default.*/
	GridLayout();
	/**Resetta tutti i parametri del layout*/
	void reset();
	/**Determina se una finestra fa parte del layout o meno
	@param toCheck Handle della finestra da ricercare nel layout.
	@return True se la finestra è nel layout, false altrimenti.
	*/
	bool isInLayout(HWND toCheck);
//	void setDimensions(int rows,int columns);
	/**Verifica se il layout è attualmente vuoto
	@return True se il layout è vuoto, false altrimenti.
	*/
	bool isNull();
	/**Trova l'handle di una finestra a partire dalle sue coordinate.
	@param row Riga dell'elemento desiderato.
	@param column Colonna dell'elemento desiderato.
	@return Handle della finestra desiderata.
	*/
	HWND getElement(int row, int column);
	/**Rimuove un elemento dall layout.
	@param hwnd Handle della finestra da rimuovere.
	@param completely Indica se la finestra deve essere rimossa da tutte le strutture dati del programma.
	*/
	void removeElement(HWND hwnd,bool completely);
	/**Aggiunge una finestra al layout.
	@param hwnd Handle alla finestra da aggiungere.
	@return True se la finestra viene aggiunta correttamente al layout, false altrimenti.
	*/
	bool anchorElement(HWND hwnd);
	/**Ritorna l'altezza delllo schermo.
	*/
	int getHeight();
	/**Ritorna la larghezza dello schermo.*/
	int getWidth();
	/**Ritorna il numero di righe della matrice.*/
	int getRows();
	/**Ritorna il numero di colonne della matrice.*/
	int getColumn();
	/**Disegna il layout assegnando posizioni e dimensioni alle finestre ricalcolandoli ad ogni chiamata.*/
	void drawLayout();
	/**Regola il layout dopo lo spostamento di una finestra ottimizzandone lo spazio sfruttabile.
	@param moved Handle della finestra mossa.
	@param recursiveBock Flag che indica di fermare la ricorsione tra questa funzione e searchSpace().
	@see searchSpace(HWND moved).
	*/
	void regulateLayout(HWND moved,bool recursiveBlock);
	/**Verifica se tra due rettangoli rappresentanti l'area di una finestra c'è una intersezione e la risolve ridimensionando il rettangolo analizzato.
	@param moved Puntatore al rettangolo della finestra mossa, questo rettangolo non cambia dimensioni dopo la chiamata a quasta funzione.
	@param analyzing Puntatore al rettangolo della finestra analizzata, in caso di intersezione viene ridimensionato in modo da eliminarla.
	@return True se c'è una intersezione, false altrimenti.
	*/
	bool checkInterseption(RECT * moved,RECT * analyzing);
	/**Adatta il layout in caso di cambio di risoluzione.
	*/
	void changedResolution();
	/**Ricerca tutto lo spazio disponibile in modo da non lasciare spazi vuoti nel layout ove possibile.
	@param moved Handle della finestra mossa e quindi che uscirà non modificata dalla chiamata della funzione.
	*/
	void searchSpace(HWND moved);
	/**Ripristina posizioni e dimensioni delle finestre a come erano prima dell'attivazione del layout ove possibile.
	*/
	void restorePositions();
	/**Verifica se una finestra è stata mossa completamente o ridimensionata.
	@param hwnd Handle della finestra mossa o ridimensionata.
	@return True se la finestra si è mossa completamente rispetto alla posizione precedente, false altrimenti.
	*/
	bool isMoved(HWND hwnd);
	/**Registra la posizione di tutte le finestre del layout.*/
	void regLayPosition();
	/**Registra la posizione della finestra specificata.
	@param added Handle della finestra di cui registrare la posizione.
	*/
	void regLayPosition(HWND added);
	/**Inverte le posizioni della griglia.
	*/
	void revertLayout();
};

#endif