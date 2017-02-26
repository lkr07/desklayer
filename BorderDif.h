
#include<windows.h>
#include <list>
#include<vector>
#include<iostream>
#include<cstdlib>
#include<ctime>
#include <cmath>
#include<winuser.h>
#include "HashMap.h"


#ifndef BORDERDIF_H
#define BORDERDIF_H

/**Costante che indica il bordo superiore del layout.*/
const int TOP=0;
/**Costante che indica il bordo inferiore del layout.*/
const int BOTTOM=3;
/**Costante che indica il bordo di sinistra del layout.*/
const int LEFT=1;
/**Costante che indica il bordo di destra del layout.*/
const int RIGHT=2;
/**Costante che indica la cella centrale del layout*/
const int CENTER=4;
const int MAXZONEWINDOWS=6;
//REGOLARA DOPO IL RICALCOLO EL PESO...

/**Struttura contenente l'handle di una finestra e il suo relativo peso per i vari bordi.*/
struct layWND{
	HWND hwnd;
	double weightNSx;
	double weightWEy;
	layWND::layWND(){
		hwnd=NULL;
		weightNSx=0;//se rimane a zero vuol dire che la finestra non è mai stata messa ai bordi...
		weightWEy=0;
	}
	bool operator==(layWND other){
		if(other.hwnd==hwnd)
			return true;
		return false;
	}
	bool operator==(HWND other){
		if(other==hwnd)
			return true;
		return false;
	}
	const layWND & operator=(const layWND & other){
		hwnd=other.hwnd;
		 weightNSx=other.weightNSx;
		 weightWEy=other.weightWEy;
		 return other;
	}
};


/**Classe che implementa le funzionalità per creare e gestire un layout di tipo border*/
class BorderDif{
private:
	/**Lista delle finestre nel lato nord.*/
	std::list<layWND> north;
	/**Lista delle finestre nel lato sud.*/
	std::list<layWND> south;
	/**Lista delle finestre nel lato est.*/
	std::list<layWND> east;
	/**Lista delle finesre nel lato ovest.*/
	std::list<layWND> west;
	/**Finestra centrale.*/
	layWND central;
	/**Struttura dati contenente le posizioni delle finestre precedenti all'aggiunta al layout.*/
	HashMap initPositions;
	/**Struttura dati contenente la posizione corrente di tutti gli elementi del layout.*/
	HashMap regPositions;
	/**Coordinate temporanee della cella centrale.*/
	double centerTempRight,centerTempTop,centerTempBottom,centerTempLeft;
	/**Struttura dati contenente handle e pesi delle finestre momentaneamente fuori dal layout.*/
	std::list<layWND> wndBuffer;
	/**Altezza del desktop.*/
	double height;
	/**Larghezza del desktop.*/
	double width;
	/**Peso orizzontale delle finestre ancorate ad est.*/
	double weightEx;
	/**Peso orizzontale delle finestre ancorate ad ovest.*/
	double weightWx;
	/**Peso verticale delle finestre ancorate a nord.*/
	double weightNy;
	/**Peso verticale delle finestre ancorate a sud.*/
	double weightSy;
	double minX;
	double minY;
public:
	/**Costruttore di default.*/
	BorderDif();
	BorderDif(double weightEx,double weightWx,double weightNy, double weightSy);
	/**Funzione di disegno del layout.*/
	void refreshLayout();
	/**Funzione che regola il layout in conseguenza uno spostamento di una finestra.
	@param moved Handle della finestra mossa.
	*/
	bool regulateLayout(HWND moved);
	/**Funzione che restituisce l'indice nella lista di una finestra che si trova in posizione laterale.
	@param hwnd Handle alla finestra del quale si vuole scoprire l'indice.
	@param bord Identificatore del bordo nel quale si trova la finestra.
	@return L'intero che rappresenta la posizione della finestra nella lista, se la funzione non va a buon fine viene ritornato -1.
	*/
	int getListPosition(HWND hwnd,int bord);
	/**Viene registrata la posizione di una particolare finestra.
	@param added Handle della finestra di cui si vuole registrare la posizione.
	*/
	void regLayPosition(HWND added);
	/**Registra la posizione di tutte le finestre presenti nel layout*/
	void regLayPosition();
	/**Verifica se una finestra è stata mossa.
	@return True se la finestra si è mossa, false altrimenti.
	*/
	bool isMoved(HWND hwnd);
	/**Spinge la finestra al centro anche se vi si trovano altre finestre
	@param toPush Handle della finestra da spingere al centro.
	*/
	void pushToCenter (HWND toPush);
	/**Sposta una finestra da una posizione ad un lato all'interno del layout.
	@param activeWnd Handel della finestra da muovere.
	@param oldPos Identificativo del bordo di provenienza della finestra.
	@param newPos Identificativo del bordo di destinazione della finestra.
	*/
	void moveElement(HWND activeWnd,int oldPos,int newPos);
	/**Riporta alla posizione precedente all'aggiunta al layout tutte le finestre.
	*/
	void restorePositions();
	/**Rimuove una finestra dal layout.
	@param toRemove Handle della finestra da rimuovere dal layout.
	@param pos Identificativo del bordo dove si trova la finestra da rimuovere.
	@param complete Se è posto a true indica che tutto ciò che è memorizzato della finestra deve essere rimosso.
	@param futuRestore Se è posto a true vengono salvati i pesi della finestra nonostante sia rimossa dal layout.
	@return True se l'elemento è stato rimosso correttamente, false altrimenti.
	*/
	bool removeElement(HWND toRemove,int pos,bool complete,bool futuRestore);
	bool removeElement(HWND toRemove,int pos,bool complete,bool futuRestore,bool refresh);
	/**Verifica se una finestra fa parte del layout.
	@param toCheck Handle della finestra da ricercare nel layout.
	@return Identificativo del bordo a cui appartiene la finestra, -1 se la finestra non fa parte del layout.
	*/
	int isInLayout(HWND toCheck);
	/**Restituisce tutte le finestre facenti parti di una posizione del layout.
	@param pos La posizione della quale si vogliono ottenere le finestre
	@return Una lista di strutture di tipo layWND contenente tutti gli elementi, se ce ne sono, della data posizione. 
	*/
	std::list<layWND> getWindowsInPos(int pos);
	/**Aggiunge una finestra al layout.
	@param hwnd La finestra da aggiungere al layout.
	@param position La posizione nella quale va aggiunta la finestra.
	@return True se l'inserimento è andato a buon fine, false altrimenti.
	*/
	bool anchorElement(HWND hwnd,int position);
	/**Aggiunge una finestra al layout.
	@param hwnd La finestra da aggiungere al layout.
	@param position La posizione nella quale va aggiunta la finestra.
	@param listPos Specifica la posizione all'interno del lato in cui viene aggiunta la finestra.
	@return True se l'inserimento è andato a buon fine, false altrimenti.
	*/	
	bool anchorElement(HWND hwnd,int position,int listPos);
	/**Spsota una finestra presente nel layout al centro.
	@param toMove Riferimento alla finestra da spostare al centro.
	@param pos Posizione della finestra da spostare al centro.
	@return True se lo spostamento è andato a buon fine, false altrimenti.
	*/
	bool toCenter(layWND toMove,int pos);
	/**Cambia i parametri e ridisegna il layout dopo un evento di cambiamento di risoluzione dello schermo.*/
	void refreshScreenSize();
	/**Assegna un peso ad una finestra che non lo ha.
	@param nuova Riferimento alla finestra alla quale va settato il peso.
	@param pos Posizione nella quale la finestra si trova.
	*/
	void assignWeight(layWND nuova,int pos,bool refresh);
	/**Regola i pesi in conseguenza di uno spostamento di una finestra.
	@param moved Riferimento alla finestra spostata.
	@param pos Nuova posizione della finestra. 
	*/
	void ricalcolateWeight(layWND moved,int pos,bool refresh);
	/**Ridistribuisce il peso dopo che una finestra ha abbandonato al sua posizione.
	@param pos Posizione nella quale i pesi vanno ridistribuiti.
	@param weighToGive Peso della finestra che ha lasciato la posizione che andrà ridistribuito.
	*/
	void ridistribuiteWeight(int pos,double weighToGive,bool refresh);
	void ridistribuiteWeight(int pos,double weighToGive, layWND exc,bool refresh);
	void adjustWeight();
	void getRegion(HWND hwnd,LPRECT rect);
	bool checkFull(int pos);
	void checkIntegrity();
	void bringToTop(layWND top);
	std::list<HWND> minimizeAll(HWND noMin);
	void checkSum();
	void checkSum(HWND moved);
};
#endif