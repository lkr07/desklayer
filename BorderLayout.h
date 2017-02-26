
#include<windows.h>
#include <list>
#include<vector>
#include<iostream>
#include<cstdlib>
#include<ctime>
#include "HashMap.h"
#include "BorderDif.h"

#ifndef BORDERLAYOUT_H
#define BORDERLAYOUT_H

/**Costante che indica il bordo superiore del layout.*/
//const int TOP=0;
/**Costante che indica il bordo inferiore del layout.*/
//const int BOTTOM=3;
/**Costante che indica il bordo di sinistra del layout.*/
//const int LEFT=1;
/**Costante che indica il bordo di destra del layout.*/
//const int RIGHT=2;
/**Costante che indica la cella centrale del layout*/
//const int CENTER=4;
//const int MAXZONEWINDOWS=15;


/**Classe che implementa le funzionalità per creare e gestire un layout di tipo border*/
class BorderLayout{

private:
	/** Larghezza dello schermo e del layout.*/
	int width;
	/**Altezza dello schermo e del layout.*/
	int height;
	int centerBorderLeft;
	int centerBorderRight;
	int centerBorderTop;
	int centerBorderBottom;
	/**Lista degli elemnti a nord nel layout*/
	std::list<HWND> north;
	/**Lista degli elementi a sud nel layout*/
	std::list<HWND> south;
	/**Lista degli elementi a destra nel layout.*/
	std::list<HWND> east;
	/**Lista degli elementi a sinistra nel layout.*/
	std::list<HWND> west;
	/** Struttura dati di tipo hashmap per la memorizzazione delle posizioni iniziali delle finestre del layout.*/
	HashMap initPositions;
	/**Struttura dati di tipo hashmap per la memorizzazione della posizione corrente di ogni finestra nel layout*/
	HashMap regPositions;
	/**Handle alla finestra centrale*/
	HWND central;
public:
	/**Costruttore di default che inizializza le dimensioni del layout.
	*/
	BorderLayout();
	/**Controlla se è avvenuto un  cambio di risoluzione e regola il layout di conseguenza.
	*/
	void refreshScreenSize();
	/**Setta le dimensioni della cella centrale del layout e di conseguenza tutte le dimensione dei bordi circostanti.
	@param top Coordinata del lato alto della cella.
	@param left Coordinata del lato sinistro della cella.
	@param right Coordinata del lato destro della cella.
	@param bottom Coordinata del lato basso della cella.
	*/
	void setCenterCellDimension(int top,int left,int right,int bottom);
	/**Ingrandisce le dimensioni della cella centrale da 1/2 dell'altezza e larghezza totali a 3/5 e viceversa.
	@param zoomIn True se si vuole ingrandire la cella, false altrimenti.
	*/
	void zoomCenter(bool zoomIn);
	/**Ritorna la coordinata del lato sinistro della cella centrale.
	*/
	int getBorderLeft ();
	/**Ritorna la coordinata del lato destro della cella centrale.
	*/
	int getBorderRight ();
	/**Ritorna la coordinata del lato superiore della cella centrale.
	*/
	int getBorderTop ();
	/**Verifica se il bordo indicato o la cella centrale sono vuoti
	@param position indica la posizione nel layout dove si andrà a verificare se vi sono finestre.
	@return True se nella posizione indicata non ci sono finestre, false altrimenti.
	*/
	bool checkEmpty(int position);
	/**Ritorna la coordinata inferiore della cella centrale.
	*/
	int getBorderBottom ();
	/**Sposta la finestra indicata dalla sua posizione originaria nel layout alla cella centrale.
	@param toMove Handle alla finestra da spostare al centro.
	@param pos Posizione della finestra da spostare al centro all'interno del layout.
	*/
	bool toCenter(HWND toMove,int pos);
	/**Aggiunge la finestra al layout registrandone la posizione.
	@param hwnd Handle alla finestra da aggiungere al layout.
	@param position Posizione nella quale aggiungere la finestra.
	@return True se la finestra è stata aggiunta correttamente al layout, false altrimenti.
	*/
	bool anchorElement(HWND hwnd,int position);
	bool anchorElement(HWND hwnd,int position,int listPos);
	/**Ridisegna tutto il layout sullo schermo ricalcolando tutte la dimensioni e posizioni in base al numero di finestre.
	*/
	bool refreshLayout();
	/**Restituisce la finestra o le finestre in una determinata posizione.
	@param pos La posizione della quale si desiderano gli handle di finestra.
	@return Una lista con tutti gli handle delle finestre nella posizione indicata.
	*/
	std::list<HWND> getWindowsInPos(int pos);
	/**Regola le posizioni delle finestre dopo la registrazione di un movimento in modo da ricavare spazio per quelle spostate e non.
	@param moved Handle della finestra mossa.
	*/
	void regulateLayout(HWND moved);
	/**Individua la posizione di una finestra nel layout se questa ne fa parte.
	@param toCheck Handle della finestra da ricercare nel layout.
	@return Posizione della finestra nel layout, se la finestra non fa parte del layout viene restituito -1.
	*/
	int isInLayout(HWND toCheck);
	/**Verifica se due rettangoli si intersecano e li ridimensiona di conseguenza.
	@param moved Puntatore al rettangolo della finestra mossa, rimane lo stesso dopo la chiamata a questa funzione.
	@param analyzing Puntatore al rettangolo sotto analisi che viene ridimensionato in caso di intersezione con il rettangolo a cui punta moved.
	@return Viene restituito true se avviene una intersezione, false altrimenti.
	*/
	bool checkInterseption(RECT * moved,RECT * analyzing);
	/**Provvede alla rimozione di una finestra dalle strutture dati del programma.
	@param toRemove Handle alla finestra da rimuovere.
	@param pos Posizione dela finestra da rimuovere.
	@param complete Indica se la finestra deve essere rimossa anche dalle strutture dati contenenti le posizioni della finestra(iniziale ed attuale).
	@return True se la finestra viene rimossa correttamente, false altrimenti.
	*/
	bool removeElement(HWND toRemove,int pos,bool complete);
	/**Provvede a riportare le finestre in posizione e dimensioni precedenti all'attivazione del layout ove possibile.
	*/
	void restorePositions();
	/**Cerca lo spazio disponibile per sfruttare gli spazi vuoti creati dal movimento di una finestra.
	@param moved Handle alla finestra mossa la cui dimensione non verrà mai cambiata.
	*/
	void searchSpace(HWND moved);
	/**Cerca lo spazio disponibile per sfruttare gli spazi vuoti creati dal movimento di una finestra.
	@param moved Handle alla finestra mossa la cui dimensione non verrà mai cambiata.
	@param exam Handle ad un altra finestra che rimane deve rimanere delle stesse dimensioni dopo la chiamata alla funzione.
	*/
	void searchSpace(HWND exam,HWND moved);
	/**Sposta una finestra da una posizione ad un altra tra i bordi del layout.
	@param activeWnd Handle della finestra da muovere.
	@param oldPos Posizione attuale della finestra.
	@param newPos Nuova posizione della finestra.
	*/
	void moveElement(HWND activeWnd,int oldPos,int newPos);
	/**Sposta una finestra al centro anche se questa non fa parte del layout richiamando altre funzioni della classe.
	@param toPush Handle alla finestra da muovere al centro.
	*/
	void pushToCenter (HWND toPush);
	/**Determina se una finestra è stata mossa completamente o è stata solo ridimensionata(movimento di più lati della finestra, ma non di tutti).
	@param moved Handle della finestra da contrallare.
	@return True se la finestra si è mossa completamente, false altrimenti.
	*/
	bool isMoved(HWND hwnd);	
	/**Registra la posizione di tutte le finestre del layout.
	*/
	void regLayPosition();
	/**Registra la posizione di una finestra in una struttura dati.
	@param added Handle della finestra di cui registrare la posizione.
	*/
	void regLayPosition(HWND added);
	void registerNewBorder();
	bool getRegisteredPosition(HWND hwnd,LPRECT position);
	int getListPosition(HWND hwnd,int bord);
};
#endif