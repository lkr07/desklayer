#include"BorderLayout.h"
#include<iostream>
using namespace std;
BorderLayout::BorderLayout(){

//DEVMODE devmode;
//EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devmode);
height=GetSystemMetrics(SM_CYMAXIMIZED);//devmode.dmPelsHeight;//vedere se getsystemmetrics da anche la dim meno barra start
width=GetSystemMetrics(SM_CXMAXIMIZED);//devmode.dmPelsWidth;
this->centerBorderBottom=(height/4)*3;
this->centerBorderLeft=(width/4);
this->centerBorderRight=(width/4)*3;
this->centerBorderTop=(height/4);
central=NULL;
}

bool BorderLayout::anchorElement(HWND hwnd,int position){
	//int i;
	RECT temp;
	switch (position){
	case TOP:
		if(north.size()==MAXZONEWINDOWS)
			return false;
			north.push_back(hwnd);		
		//	this->refreshLayout();
	break;
	case LEFT:
		if(west.size()==MAXZONEWINDOWS)
			return false;
			west.push_back(hwnd);
		//	this->refreshLayout();		
	break;
	case RIGHT:
		if(east.size()==MAXZONEWINDOWS)
			return false;
			east.push_back(hwnd);
			//	this->refreshLayout();
	break;
	case BOTTOM:
		if(south.size()==MAXZONEWINDOWS)
			return false;
			south.push_back(hwnd);
		//	this->refreshLayout();
		break;
	case CENTER:
		if(central!=NULL)
			return false;
		else{
			central=hwnd;
		//	this->refreshLayout();
		}
	break;
	default:	return false;
	}
			::GetWindowRect(hwnd,&temp);
			if(!(temp.top<0 && temp.right<0 && temp.left<0 && temp.bottom<0))
			initPositions.insertKeyElement(hwnd,&temp);
			
return true;
}

bool BorderLayout::checkEmpty(int position){

	//int i;
	switch (position){
	case TOP:
			if(north.size()!=0)
				return false;
	return true;
	case LEFT:
			if(west.size()!=0)
				return false;		
	return true;
	case RIGHT:
			if(east.size()!=0)
				return false;
	return true;
	case BOTTOM:
			if(south.size()!=0)
				return false;
	case CENTER:
		if(central==NULL)
			return true;
	return false;
	default:
		return true;
	}

}

void BorderLayout::refreshScreenSize(){
	//DEVMODE devmode;
	//EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devmode);
	bool refresh=false;
	if(GetSystemMetrics(SM_CYMAXIMIZED)!=height || GetSystemMetrics(SM_CXMAXIMIZED)!=width)
		refresh=true;
	height=GetSystemMetrics(SM_CYMAXIMIZED);//devmode.dmPelsHeight;//vedere se getsystemmetrics da anche la dim meno barra start
	width=GetSystemMetrics(SM_CXMAXIMIZED);//devmode.dmPelsWidth;
	this->centerBorderBottom=(height/4)*3;
	this->centerBorderLeft=(width/4);
	this->centerBorderRight=(width/4)*3;
	this->centerBorderTop=(height/4);
	if(refresh)
	this->refreshLayout();   
}

int BorderLayout::getBorderBottom(){
	return centerBorderBottom;
}

int BorderLayout::getBorderTop(){
	return centerBorderTop;
}

int BorderLayout::getBorderLeft(){
	return centerBorderLeft;
}

int BorderLayout::getBorderRight(){
	return centerBorderRight;
}

std::list<HWND> BorderLayout::getWindowsInPos(int pos){
	std::list<HWND> temp;
	switch (pos){
	case TOP:
		return north;
	case LEFT:
		return west;
	case RIGHT:
		return east;
	case BOTTOM:
		return south;
case CENTER:
			temp.push_back(central);
		return temp;
	default:
		return NULL;
	}


}

void BorderLayout::setCenterCellDimension(int top,int left,int right,int bottom){
	this->centerBorderBottom=bottom;
	this->centerBorderTop=top;
	this->centerBorderLeft=left;
	this->centerBorderRight=right;
}

bool BorderLayout::toCenter(HWND toMove,int pos){
	if(toMove==central)
		return true;
	//MessageBox(toMove,"centrare","centrare",0);
	//vedere il caso se al centro ci metto grid
	std::list<HWND>::iterator iter;
	int direction=-1 ;
	switch(pos){
	case TOP:
		if(north.size()>0)
		for(iter=north.begin();iter!=north.end();iter++){ 
			if(*iter==toMove){
				direction=0;
				if(central!=NULL){
					north.insert(iter,central);
				}
				central=toMove;
				}
	}
		break;
	case LEFT:
		
	if(west.size()>0)
	for(iter=west.begin();iter!=west.end();iter++){
		if(*iter==toMove){
			direction=1;
			if(central!=NULL){
					west.insert(iter,central);
				}
				central=toMove;
			}
	}
	break;
	case RIGHT:
		if(east.size()>0)
	for(iter=east.begin();iter!=east.end();iter++){ 
		if(*iter==toMove){
			direction=2;
			if(central!=NULL){
				east.insert(iter,central);
				}
				central=toMove;
		}
	}
	break;
	case BOTTOM:
		if(south.size()>0)
	for(iter=south.begin();iter!=south.end();iter++){ 
		if(*iter==toMove){
			direction=3;
			if(central!=NULL){
					south.insert(iter,central);
				}
			central=toMove;
		}
	}
	break;
	default:
		return false;
	}
//	MessageBox(toMove,"toMove","MOVE",0);
//	MessageBox(central,"central","CENTRE",0);
	if(direction>-1)
		removeElement(toMove,direction,false);
	return true;

}


bool BorderLayout::refreshLayout(){

	int tempX,tempY,i=0;
	WINDOWPLACEMENT wndplc;
	RECT newRect;	int centerTempTop=centerBorderTop,centerTempRight=centerBorderRight,centerTempLeft=centerBorderLeft,centerTempBottom=centerBorderBottom;
	std::list<HWND>::iterator iter;	
	if(north.size()>0){
		i=0;
		tempX=width/north.size();
		tempY=0;
		for(iter=north.begin();iter!=north.end();iter++){
			//fare funzione per provare ad assegare la posizione con setwindowPlacement
			GetWindowPlacement(*iter,& wndplc);
			newRect.left=tempX*i;
			newRect.top=tempY;
			newRect.right=tempX;
			newRect.bottom=centerBorderTop;
			wndplc.rcNormalPosition=newRect;
			wndplc.showCmd=SW_SHOWNORMAL;
			SetWindowPlacement(*iter,& wndplc);
			::SetWindowPos(*iter,NULL,tempX*i,tempY,tempX,this->centerBorderTop,0);
			this->regLayPosition(*iter);
			i++;
		}
	}
	else
		centerTempTop=0;
	if(south.size()>0){
		i=0;
		tempX=width/south.size();
		tempY=this->centerBorderBottom;
		for(iter=south.begin();iter!=south.end();iter++){
			GetWindowPlacement(*iter,& wndplc);
			newRect.left=tempX*i;
			newRect.top=tempY;
			newRect.right=tempX;
			newRect.bottom=height;
			wndplc.rcNormalPosition=newRect;
			wndplc.showCmd=SW_SHOWNORMAL;
			SetWindowPlacement(*iter,& wndplc);
			::SetWindowPos(*iter,NULL,tempX*i,tempY,tempX,height-tempY,0);
			this->regLayPosition(*iter);
			i++;	
		}
		
	}
	else
		centerTempBottom=height;

	if(west.size()>0){
		i=0;
		tempX=0;
		tempY=(height-centerTempTop-(height-centerTempBottom))/west.size();
		for(iter=west.begin();iter!=west.end();iter++){
			GetWindowPlacement(*iter,& wndplc);
			newRect.left=tempX;
			newRect.top=(tempY*i)+centerTempTop;
			newRect.right=centerBorderLeft;
			newRect.bottom=(tempY*i)+centerTempTop+tempY;
			wndplc.rcNormalPosition=newRect;
			wndplc.showCmd=SW_SHOWNORMAL;
			SetWindowPlacement(*iter,& wndplc);
			::SetWindowPos(*iter,NULL,tempX,(tempY*i)+centerTempTop,this->centerBorderLeft,tempY,0);
			this->regLayPosition(*iter);
			i++;
		}
		
	}
	else
		centerTempLeft=0;
	if(east.size()>0){
		i=0;
		tempX=centerBorderRight;
		tempY=(height-centerTempTop-(height-centerTempBottom))/east.size();
		for(iter=east.begin();iter!=east.end();iter++){
			GetWindowPlacement(*iter,& wndplc);
			newRect.left=tempX;
			newRect.top=(tempY*i)+centerTempTop;
			newRect.right=width-tempX;
			newRect.bottom=(tempY*i)+centerTempTop+tempY;
			wndplc.rcNormalPosition=newRect;
			wndplc.showCmd=SW_SHOWNORMAL;
			SetWindowPlacement(*iter,& wndplc);
				::SetWindowPos(*iter,NULL,tempX,(tempY*i)+centerTempTop,width-tempX,tempY,0);
				this->regLayPosition(*iter);
				i++;
		}
		
	}
	else
		centerTempRight=width;

	if(this->central != NULL){
		GetWindowPlacement(central,& wndplc);
		newRect.left=centerTempLeft;
		newRect.top=centerTempTop;
		newRect.right=centerTempRight-centerTempLeft;
		newRect.bottom=centerTempBottom-centerTempTop;
		wndplc.rcNormalPosition=newRect;
		wndplc.showCmd=SW_SHOWNORMAL;
		SetWindowPlacement(central,& wndplc);
		::SetWindowPos(central,NULL,centerTempLeft,centerTempTop,centerTempRight-centerTempLeft,centerTempBottom-centerTempTop,0);			
		this->regLayPosition(central);	
	}
	return true;
	}


void BorderLayout::regulateLayout(HWND moved){
	if(isMoved(moved)){
		this->removeElement(moved,this->isInLayout(moved),true);
		::SetActiveWindow(moved);
		return;
	}
	this->regLayPosition(moved);
	std::list<HWND>::iterator iterNorth,iterSouth,iterEast,iterWest;	
	iterNorth=north.begin();
	iterSouth=south.begin();
	iterEast=east.begin();
	iterWest=west.begin();
	RECT rect;
	RECT movedRect;
	::GetWindowRect(moved,&movedRect);
	bool finishW=false,finishE=false,finishS=false,finishN=false;
	//funzione di verifica se ho mosso la finestra o l'ho spostata....
		for(;iterNorth!=north.end();iterNorth++){
			::GetWindowRect(*iterNorth,& rect);
			if(*iterNorth!=moved)
			if(checkInterseption(& movedRect,& rect)){
				::SetWindowPos(*iterNorth,0,rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,0);
				searchSpace(*iterNorth,moved);
				this->regLayPosition(*iterNorth);
			//	if(rect.bottom>centerBorderTop)
			//		scrollWindows(TOP);
			}
		}
		for(;iterWest!=west.end();iterWest++){
			::GetWindowRect(*iterWest,& rect);
			if(*iterWest!=moved)
			if(checkInterseption(& movedRect,& rect)){ 
				::SetWindowPos(*iterWest,0,rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,0);
				searchSpace(*iterWest,moved);
				this->regLayPosition(*iterWest);
			}
		}
		for(;iterEast!=east.end();iterEast++){
			::GetWindowRect(*iterEast,& rect);
			if(*iterEast!=moved)
			if(checkInterseption(& movedRect,& rect)){ 
				::SetWindowPos(*iterEast,0,rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,0);
				searchSpace(*iterEast,moved);
				this->regLayPosition(*iterEast);
			}
		}
		for(;iterSouth!=south.end();iterSouth++){
			::GetWindowRect(*iterSouth,& rect);
			if(*iterSouth!=moved)
			if(checkInterseption(& movedRect,& rect)){
				::SetWindowPos(*iterSouth,0,rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,0);
				searchSpace(*iterSouth,moved);
				this->regLayPosition(*iterSouth);
			}
		}
		if(central!=NULL && central!=moved){
			::GetWindowRect(central,& rect);
			if(checkInterseption(& movedRect,& rect)){
			::SetWindowPos(central,0,rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,0);
			searchSpace(central,moved);
			this->regLayPosition(central);
			}
		}
		//funzione di verifica degli spazi vuoti......
		this->searchSpace(moved);
		this->registerNewBorder();
}

int BorderLayout::isInLayout(HWND toCheck){
	std::list<HWND>::iterator iter;
	for(iter=north.begin();iter!=north.end();iter++) 
		if(*iter==toCheck)
			return TOP;
	for(iter=west.begin();iter!=west.end();iter++) 
		if(*iter==toCheck)
			return LEFT;
	for(iter=east.begin();iter!=east.end();iter++)
		if(*iter==toCheck)
			return RIGHT;
	for(iter=south.begin();iter!=south.end();iter++) 
			if(*iter==toCheck)
			return BOTTOM;
	if(central==toCheck)
		return CENTER;
return -1;
}

//RIPORTARE LE FINESTRE ALLA POSIZIONE ORIGINARIA SALVANDO LE POSIZIONI...così posso pure vedere se è un move o un size

bool BorderLayout::checkInterseption(RECT * moved,RECT * analyzing){	
	RECT inter;//capire quando resize...nn fare niente quando muove....

	if(::IntersectRect(& inter,moved,analyzing)){
	//	MessageBox(*north.begin(),"fas","af",0);
		//::CopyRect(&temp,analyzing);
		//::SubtractRect(analyzing,&temp,& inter);
	
		if((analyzing->top==inter.top && analyzing->bottom==inter.bottom)){
			if(analyzing->left==inter.left)
				analyzing->left=inter.right;
			else if(analyzing->right==inter.right)
				analyzing->right=inter.left;

		}
		else if((analyzing->right==inter.right && analyzing->left==inter.left)){
			if(analyzing->top==inter.top)//aggiustare  un poco le condizioni
				analyzing->top=inter.bottom;
			else if(analyzing->bottom==inter.bottom)
				analyzing->bottom=inter.top;
		}
		else if(analyzing->top==inter.top && analyzing->left==inter.left){
			if(inter.right-inter.left>inter.bottom-inter.top)
				analyzing->top=inter.bottom;
			else 
				analyzing->left=inter.right;
		}
		else if(analyzing->top==inter.top && analyzing->right==inter.right){
			if(inter.right-inter.left>inter.bottom-inter.top)
				analyzing->top=inter.bottom;
			else 
				analyzing->right=inter.left;
		}
		else if(analyzing->bottom==inter.bottom && analyzing->left==inter.left){
			if(inter.right-inter.left>inter.bottom-inter.top)
				analyzing->bottom=inter.top;
			else 
				analyzing->left=inter.right;
		}
		else if(analyzing->bottom==inter.bottom && analyzing->right==inter.right){
			if(inter.right-inter.left>inter.bottom-inter.top)
				analyzing->bottom=inter.top;
			else 
				analyzing->right=inter.left;
		}
		else if(analyzing->top==inter.top)
			analyzing->top=inter.bottom;
		else if(analyzing->bottom==inter.bottom)
			analyzing->bottom=inter.top; 
		else if(analyzing->right==inter.right)
			analyzing->right=inter.left;
		else if(analyzing->left==inter.left)
			analyzing->left=inter.right;

		if(analyzing->bottom-analyzing->top<150 ||analyzing->right-analyzing->left<150)
			return false;	

	return true;
	}
	else
		return false;
}

bool BorderLayout::removeElement(HWND toRemove,int pos,bool complete){
	switch (pos){
		case TOP:
			if(north.size()>0){
				north.remove(toRemove);
				refreshLayout();
			}
			break;
		case LEFT:
			if(west.size()>0){
				west.remove(toRemove);
				refreshLayout();
			}
			break;
		case RIGHT:
			if(east.size()>0){
				east.remove(toRemove);
				refreshLayout();
			}
			break;
		case BOTTOM:
			if(south.size()>0){
				south.remove(toRemove);
				refreshLayout();
			}
			break;
		case CENTER:
			central=NULL;
		    refreshLayout();  
			break;
		default:
			break;
		}
	if(complete){
	this->initPositions.delKeyElement(toRemove);
	this->regPositions.delKeyElement(toRemove);
	}
	return true;
}

void BorderLayout::restorePositions(){
	std::list<HWND>::iterator iterNorth,iterSouth,iterEast,iterWest;	
	iterNorth=north.begin();
	iterSouth=south.begin();
	iterEast=east.begin();
	iterWest=west.begin();	
	RECT temp;
	WINDOWPLACEMENT wndplc;//finire restore....allungare le finestre quando possibile....
	for(;iterNorth!=north.end();iterNorth++){
		if(initPositions.findElement(*iterNorth,&temp)){
			if(!::IsRectEmpty(&temp)){
				::GetWindowPlacement(*iterNorth,& wndplc);
				CopyRect(& wndplc.rcNormalPosition,& temp);
				wndplc.showCmd=SW_SHOWNORMAL;
				::SetWindowPlacement(*iterNorth,& wndplc);
			SetWindowPos(*iterNorth,0,temp.left,temp.top,temp.right-temp.left,temp.bottom-temp.top,0);
			}
		}
	}
	for(;iterWest!=west.end();iterWest++){
		if(initPositions.findElement(*iterWest,&temp)){
			if(!::IsRectEmpty(&temp)){
				::GetWindowPlacement(*iterWest,& wndplc);
				CopyRect(& wndplc.rcNormalPosition,& temp);
				wndplc.showCmd=SW_SHOWNORMAL;
				::SetWindowPlacement(*iterWest,& wndplc);
				SetWindowPos(*iterWest,0,temp.left,temp.top,temp.right-temp.left,temp.bottom-temp.top,0);
			}
		}
	}
	for(;iterEast!=east.end();iterEast++){
		if(initPositions.findElement(*iterEast,&temp)){
			if(!::IsRectEmpty(&temp)){
				::GetWindowPlacement(*iterEast,& wndplc);
				CopyRect(& wndplc.rcNormalPosition,& temp);
				wndplc.showCmd=SW_SHOWNORMAL;
				::SetWindowPlacement(*iterEast,& wndplc);
				SetWindowPos(*iterEast,0,temp.left,temp.top,temp.right-temp.left,temp.bottom-temp.top,0);
			}
		}	
	}
	for(;iterSouth!=south.end();iterSouth++){
		if(initPositions.findElement(*iterSouth,&temp)){
			if(!::IsRectEmpty(&temp)){
				::GetWindowPlacement(*iterSouth,& wndplc);
				CopyRect(& wndplc.rcNormalPosition,& temp);
				wndplc.showCmd=SW_SHOWNORMAL;
				::SetWindowPlacement(*iterSouth,& wndplc);
				SetWindowPos(*iterSouth,0,temp.left,temp.top,temp.right-temp.left,temp.bottom-temp.top,0);
			}
		}
	}
	if(central!=NULL){
		if(initPositions.findElement(central,&temp)){
			if(!::IsRectEmpty(&temp)){
				::GetWindowPlacement(central,&wndplc);
				CopyRect(&wndplc.rcNormalPosition, &temp);
				wndplc.showCmd=SW_SHOWNORMAL;
				::SetWindowPlacement(central,& wndplc);
				SetWindowPos(central,0,temp.top,temp.left,temp.right-temp.left,temp.bottom-temp.top,0);
			}
		}
	}//FARE UNA FUNZIONE KE KONTIENE STA ROBBA.....

}


//posso fare myrect per memorizzare le coordinate originali e wnd per capire se ho spostato oppure ho resaizzato
//questo mi vede se ho rimpicciolito  una finestra..
void BorderLayout::searchSpace(HWND moved){
	RECT tmp1,tmp2,tmp3,tmp4;
	RECT next;
	int positionMov=this->isInLayout(moved);
	std::list<HWND>::iterator iter;
	int tmpCoord=0;
	if(central!=NULL)
	::GetWindowRect(central,&tmp2);
	::GetWindowRect(*east.begin(),& tmp3);
	::GetWindowRect(*west.begin(),&tmp4);
	bool resizable=true;
	for(iter=north.begin();iter!=north.end();iter++){
		::GetWindowRect(*iter,& tmp1);
		if((tmp1.bottom>=tmp2.top && central!=NULL) || (tmp1.bottom>tmp3.top && east.size()>0)  || (tmp1.bottom>tmp4.top && west.size()>0))
			resizable=false;
		if(tmp1.bottom>tmpCoord)
			tmpCoord=tmp1.bottom;
		iter++;
		::GetWindowRect(*iter,& next);
		iter--;
		if(next.left>tmp1.right && *iter!= moved){
			SetWindowPos(*iter,0,tmp1.left,tmp1.top,next.left-tmp1.left,tmp1.bottom-tmp1.top,0);
			this->regLayPosition(*iter);
		}
		else if(next.left>tmp1.right && *iter== moved){
			iter++;
			SetWindowPos(*iter,0,tmp1.right,next.top,next.right-tmp1.right,next.bottom-next.top,0);
			this->regLayPosition(*iter);
			iter--;
		}
	}
	if(resizable && north.size()>0){
		if(central!=moved){
			SetWindowPos(central,0,tmp2.left,tmpCoord,tmp2.right-tmp2.left,tmp2.bottom-tmpCoord,0);
			this->regLayPosition(central);
		}
		if(*east.begin()!=moved){
			SetWindowPos(*east.begin(),0,tmp3.left,tmpCoord,tmp3.right-tmp3.left,tmp3.bottom-tmpCoord,0);
			this->regLayPosition(*east.begin());
		}
		if(*west.begin()!=moved){
			SetWindowPos(*west.begin(),0,tmp4.left,tmpCoord,tmp4.right-tmp4.left,tmp4.bottom-tmpCoord,0);
			this->regLayPosition(*west.begin());
		}
		if(positionMov==LEFT || positionMov==RIGHT){
			if(central==NULL && east.size()==0 && tmpCoord<tmp4.top)
				for(iter=north.begin();iter!=north.end();iter++){
					::GetWindowRect(*iter,& tmp1);
					SetWindowPos(*iter,0,tmp1.left,tmp1.top,tmp1.right-tmp1.left,tmp4.top-tmp1.top,0);
				}
			else if(central==NULL && west.size()==0 && tmpCoord<tmp3.top)
				for(iter=north.begin();iter!=north.end();iter++){
					::GetWindowRect(*iter,& tmp1);
					SetWindowPos(*iter,0,tmp1.left,tmp1.top,tmp1.right-tmp1.left,tmp3.top-tmp1.top,0);
				}
		}
	}
	//ripetere per tutti i lati ed il centro
	::GetWindowRect(central,&tmp2);
	::GetWindowRect(*east.rbegin(),& tmp3);
	::GetWindowRect(*west.rbegin(),&tmp4);
	resizable=true;
	tmpCoord=height;
	for(iter=south.begin();iter!=south.end();iter++){
		::GetWindowRect(*iter,& tmp1);
		if((tmp1.top<=tmp2.bottom && central!=NULL) || (tmp1.top<tmp3.bottom && east.size()>0)  || (tmp1.top<tmp4.bottom && west.size()>0))
			resizable=false;
		if(tmp1.top<tmpCoord)
			tmpCoord=tmp1.top;
		iter++;
		::GetWindowRect(*iter,& next);
		iter--;
		if(next.left>tmp1.right && *iter!= moved){
			SetWindowPos(*iter,0,tmp1.left,tmp1.top,next.left-tmp1.left,tmp1.bottom-tmp1.top,0);
			this->regLayPosition(*iter);
		}
		else if(next.left>tmp1.right && *iter== moved){
			iter++;
			SetWindowPos(*iter,0,tmp1.right,next.top,next.right-tmp1.right,next.bottom-next.top,0);
			this->regLayPosition(*iter);
			iter--;
		}
	}
	if(resizable && south.size()>0){
		if(central!=moved){
			SetWindowPos(central,0,tmp2.left,tmp2.top,tmp2.right-tmp2.left,tmpCoord-tmp2.top,0);
			this->regLayPosition(central);
		}
		if(*east.rbegin()!=moved){
			SetWindowPos(*east.rbegin(),0,tmp3.left,tmp3.top,tmp3.right-tmp3.left,tmpCoord-tmp3.top,0);
			this->regLayPosition(*east.rbegin());
		}
		if(*west.rbegin()!=moved){
			SetWindowPos(*west.rbegin(),0,tmp4.left,tmp4.top,tmp4.right-tmp4.left,tmpCoord-tmp4.top,0);
			this->regLayPosition(*west.rbegin());
		}
		if(positionMov==LEFT || positionMov==RIGHT){
			if(central==NULL && east.size()==0 && tmpCoord>tmp4.bottom)
				for(iter=south.begin();iter!=south.end();iter++){
					::GetWindowRect(*iter,& tmp1);
					SetWindowPos(*iter,0,tmp1.left,tmp4.bottom,tmp1.right-tmp1.left,tmp1.bottom-tmp4.bottom,0);
				}
			else if(central==NULL && west.size()==0 && tmpCoord>tmp3.bottom)
				for(iter=south.begin();iter!=south.end();iter++){
					::GetWindowRect(*iter,& tmp1);
					SetWindowPos(*iter,0,tmp1.left,tmp3.bottom,tmp1.right-tmp1.left,tmp1.bottom-tmp3.bottom,0);
				}
		}
	}


	::GetWindowRect(central,&tmp2);
	resizable=true;
	tmpCoord=0;
	for(iter=west.begin();iter!=west.end();iter++){
		::GetWindowRect(*iter,& tmp1);
		if((tmp1.right>=tmp2.left && central!=NULL))
			resizable=false;
		if(tmp1.right>tmpCoord)
			tmpCoord=tmp1.right;
		iter++;
		::GetWindowRect(*iter,& next);
		iter--;
		if(next.top>tmp1.bottom && *iter!= moved){
			SetWindowPos(*iter,0,tmp1.left,tmp1.top,tmp1.right-tmp1.left,next.top-tmp1.top,0);
			this->regLayPosition(*iter);
		}
		else if(next.top>tmp1.bottom && *iter== moved){
			iter++;
			SetWindowPos(*iter,0,next.left,tmp1.bottom,next.right-next.left,next.bottom-tmp1.bottom,0);
			this->regLayPosition(*iter);
			iter--;
		}

	}
	if(resizable && west.size()>0){
		if(central!=moved){
			SetWindowPos(central,0,tmpCoord,tmp2.top,tmp2.right-tmpCoord,tmp2.bottom-tmp2.top,0);
			this->regLayPosition(central);
		}
		for(iter=west.begin();iter!=west.end();iter++)
			if(*iter!= moved){
				::GetWindowRect(*iter,&tmp3);
				SetWindowPos(*iter,0,tmp3.left,tmp3.top,tmpCoord-tmp3.left,tmp3.bottom-tmp3.top,0);	
				this->regLayPosition(*iter);
			}
	}

	::GetWindowRect(central,&tmp2);
	resizable=true;
	tmpCoord=width;
	for(iter=east.begin();iter!=east.end();iter++){
		::GetWindowRect(*iter,& tmp1);
		if((tmp1.left<=tmp2.right && central!=NULL))
			resizable=false;
		if(tmp1.left<tmpCoord)
			tmpCoord=tmp1.left;
		iter++;
		::GetWindowRect(*iter,& next);
		iter--;
		if(next.top>tmp1.bottom && *iter!= moved){
			SetWindowPos(*iter,0,tmp1.left,tmp1.top,tmp1.right-tmp1.left,next.top-tmp1.top,0);
			this->regLayPosition(*iter);
		}
		else if(next.top>tmp1.bottom && *iter== moved){
			iter++;
			SetWindowPos(*iter,0,next.left,tmp1.bottom,next.right-next.left,next.bottom-tmp1.bottom,0);
			this->regLayPosition(*iter);
			iter--;
		}
	}
	if(resizable && east.size()>0){
		if(central!=moved){
			SetWindowPos(central,0,tmp2.left,tmp2.top,tmpCoord-tmp2.left,tmp2.bottom-tmp2.top,0);
			this->regLayPosition(central);
		}
		for(iter=east.begin();iter!=east.end();iter++)
			if(*iter!= moved){
				::GetWindowRect(*iter,&tmp3);
				SetWindowPos(*iter,0,tmpCoord,tmp3.top,tmp3.right-tmpCoord,tmp3.bottom-tmp3.top,0);	
				this->regLayPosition(*iter);
			}
	}

	if(central==moved){
		for(iter=north.begin();iter!=north.end();iter++){
			::GetWindowRect(*iter,& tmp1);
			SetWindowPos(*iter,0,tmp1.left,tmp1.top,tmp1.right-tmp1.left,tmp2.top-tmp1.top,0);
			this->regLayPosition(*iter);
		}
		for(iter=south.begin();iter!=south.end();iter++){
			::GetWindowRect(*iter,& tmp1);
			SetWindowPos(*iter,0,tmp1.left,tmp2.bottom,tmp1.right-tmp1.left,tmp1.bottom-tmp2.bottom,0);
			this->regLayPosition(*iter);
			}
		for(iter=west.begin();iter!=west.end();iter++){
			::GetWindowRect(*iter,& tmp1);
			if(west.size()==1){
				SetWindowPos(*west.begin(),0,tmp1.left,tmp2.top,tmp2.left-tmp1.left,tmp2.bottom-tmp2.top,0);
				this->regLayPosition(*west.begin());
			}
			else if(*iter==*west.begin()){
				SetWindowPos(*west.begin(),0,tmp1.left,tmp2.top,tmp2.left-tmp1.left,tmp1.bottom-tmp2.top,0);
				this->regLayPosition(*west.begin());
			}
			else if(*iter==*west.rbegin()){
				SetWindowPos(*west.rbegin(),0,tmp1.left,tmp1.top,tmp2.left-tmp1.left,tmp2.bottom-tmp1.top,0);
				this->regLayPosition(*west.rbegin());
			}
			else{
				SetWindowPos(*iter,0,tmp1.left,tmp1.top,tmp2.left-tmp1.left,tmp1.bottom-tmp1.top,0);
				this->regLayPosition(*iter);
				}
			}
		for(iter=east.begin();iter!=east.end();iter++){
			::GetWindowRect(*iter,& tmp1);
			if(east.size()==1){
				SetWindowPos(*east.begin(),0,tmp2.right,tmp2.top,tmp1.right-tmp2.right,tmp2.bottom-tmp2.top,0);			
				this->regLayPosition(*east.begin());
			}
			else if(*iter==*east.begin()){
				SetWindowPos(*east.begin(),0,tmp2.right,tmp2.top,tmp1.right-tmp2.right,tmp1.bottom-tmp2.top,0);
				this->regLayPosition(*east.begin());
			}
			else if(*iter==*east.rbegin()){
				SetWindowPos(*east.rbegin(),0,tmp2.right,tmp1.top,tmp1.right-tmp2.right,tmp2.bottom-tmp1.top,0);
				this->regLayPosition(*east.rbegin());
			}
			else{
				SetWindowPos(*iter,0,tmp2.right,tmp1.top,tmp1.right-tmp2.right,tmp1.bottom-tmp1.top,0);
				this->regLayPosition(*iter);	
				}
			}
		::GetWindowRect(*east.begin(),&tmp3);
		::GetWindowRect(*west.begin(),&tmp4);		
	}


	//TROVA TUTTO LO SPAZIO TRANNE TRA LE GRIGLIE NEI BORDI....
}


void BorderLayout::zoomCenter(bool zoomIn){
	if(zoomIn){
		this->centerBorderBottom=(height/5)*4;
		this->centerBorderLeft=(width/5);
		this->centerBorderRight=(width/5)*4;	
		this->centerBorderTop=(height/5);
	}
	else{
		this->centerBorderBottom=(height/4)*3;
		this->centerBorderLeft=(width/4);
		this->centerBorderRight=(width/4)*3;
		this->centerBorderTop=(height/4);
		}

	this->refreshLayout();
	this->regLayPosition(central);
}

void BorderLayout::moveElement(HWND activeWnd,int oldPos,int newPos){
	switch (oldPos){
		case TOP:
			if(north.size()>0){
				north.remove(activeWnd);
			}
			break;
		case LEFT:
			if(west.size()>0){
				west.remove(activeWnd);
			}
			break;
		case RIGHT:
			if(east.size()>0){
				east.remove(activeWnd);
			}
			break;
		case BOTTOM:
			if(south.size()>0){
				south.remove(activeWnd);
			}
			break;
		case CENTER:
			if(central!=NULL)
			central=NULL;
			break;
		default:
			return;
		}

	switch (newPos){
	case TOP:
		if(north.size()==MAXZONEWINDOWS)
			return;
			north.push_back(activeWnd);
		//	this->refreshLayout();
	break;
	case LEFT:
		if(west.size()==MAXZONEWINDOWS)
			return;
			west.push_back(activeWnd);
		//	this->refreshLayout();		
	break;
	case RIGHT:
		if(east.size()==MAXZONEWINDOWS)
			return;
			east.push_back(activeWnd);
			//	this->refreshLayout();
	break;
	case BOTTOM:
		if(south.size()==MAXZONEWINDOWS)
			return;
			south.push_back(activeWnd);
		//	this->refreshLayout();
		break;
	default:
		return ;
	}


}


void BorderLayout::pushToCenter(HWND toPush){
	if(this->central==NULL){
		this->anchorElement(toPush,CENTER);
		return;
	}
	srand(time(0));
	this->moveElement(central,CENTER,rand()%4);
	this->anchorElement(toPush,CENTER);
}




void BorderLayout::searchSpace(HWND exam,HWND moved){
	RECT tmp1,tmp2,tmp3,tmp4;
	std::list<HWND>::iterator iter;
	int tmpCoord=0;
	if(central!=NULL)
	::GetWindowRect(central,&tmp2);
	::GetWindowRect(*east.begin(),& tmp3);
	::GetWindowRect(*west.begin(),&tmp4);
	bool resizable=true;
	for(iter=north.begin();iter!=north.end();iter++){
		::GetWindowRect(*iter,& tmp1);
		if((tmp1.bottom>=tmp2.top && central!=NULL) || (tmp1.bottom>tmp3.top && east.size()>0)  || (tmp1.bottom>tmp4.top && west.size()>0))
			resizable=false;
		if(tmp1.bottom>tmpCoord)
			tmpCoord=tmp1.bottom;
	}
	if(resizable && north.size()>0){
		if(central!=moved && central!=exam){
			SetWindowPos(central,0,tmp2.left,tmpCoord,tmp2.right-tmp2.left,tmp2.bottom-tmpCoord,0);
			this->regLayPosition(central);
		}	
		if(*east.begin()!=moved && *east.begin()!=exam){
			SetWindowPos(*east.begin(),0,tmp3.left,tmpCoord,tmp3.right-tmp3.left,tmp3.bottom-tmpCoord,0);
			this->regLayPosition(*east.begin());
		}
		if(*west.begin()!=moved && *west.begin()!=exam){
			SetWindowPos(*west.begin(),0,tmp4.left,tmpCoord,tmp4.right-tmp4.left,tmp4.bottom-tmpCoord,0);
			this->regLayPosition(*west.begin());
		}
	}
	//ripetere per tutti i lati ed il centro
	::GetWindowRect(central,&tmp2);
	::GetWindowRect(*east.rbegin(),& tmp3);
	::GetWindowRect(*west.rbegin(),&tmp4);
	resizable=true;
	tmpCoord=height;
	for(iter=south.begin();iter!=south.end();iter++){
		::GetWindowRect(*iter,& tmp1);
		if((tmp1.top<=tmp2.bottom && central!=NULL) || (tmp1.top<tmp3.bottom && east.size()>0)  || (tmp1.top<tmp4.bottom && west.size()>0))
			resizable=false;
		if(tmp1.top<tmpCoord)
			tmpCoord=tmp1.top;
		}
	if(resizable && south.size()>0){
		if(central!=moved && central!=exam){
			SetWindowPos(central,0,tmp2.left,tmp2.top,tmp2.right-tmp2.left,tmpCoord-tmp2.top,0);
			this->regLayPosition(central);
		}
		if(*east.rbegin()!=moved && *east.rbegin()!=exam){
			SetWindowPos(*east.rbegin(),0,tmp3.left,tmp3.top,tmp3.right-tmp3.left,tmpCoord-tmp3.top,0);
			this->regLayPosition(*east.begin());
		}
		if(*west.rbegin()!=moved && *west.rbegin()!=exam){
			SetWindowPos(*west.rbegin  (),0,tmp4.left,tmp4.top,tmp4.right-tmp4.left,tmpCoord-tmp4.top,0);
			this->regLayPosition(*west.begin());
		}
	}


	::GetWindowRect(central,&tmp2);
	resizable=true;
	tmpCoord=0;
	for(iter=west.begin();iter!=west.end();iter++){
		::GetWindowRect(*iter,& tmp1);
		if((tmp1.right>=tmp2.left && central!=NULL))
			resizable=false;
		if(tmp1.right>tmpCoord)
			tmpCoord=tmp1.right;

	}
	if(resizable && west.size()>0){
		if(central!=moved && central != exam){
			SetWindowPos(central,0,tmpCoord,tmp2.top,tmp2.right-tmpCoord,tmp2.bottom-tmp2.top,0);
			this->regLayPosition(*iter);
		}
		for(iter=west.begin();iter!=west.end();iter++)
			if(*iter!= moved && *iter!=exam){
				::GetWindowRect(*iter,&tmp3);
				SetWindowPos(*iter,0,tmp3.left,tmp3.top,tmpCoord-tmp3.left,tmp3.bottom-tmp3.top,0);	
				this->regLayPosition(*iter);
			}
	}

	::GetWindowRect(central,&tmp2);
	resizable=true;
	tmpCoord=width;
	for(iter=east.begin();iter!=east.end();iter++){
		::GetWindowRect(*iter,& tmp1);
		if((tmp1.left<=tmp2.right && central!=NULL))
			resizable=false;
		if(tmp1.left<tmpCoord)
			tmpCoord=tmp1.left;
	}
	if(resizable && east.size()>0){
		if(central!=moved && central!=exam){
			SetWindowPos(central,0,tmp2.left,tmp2.top,tmpCoord-tmp2.left,tmp2.bottom-tmp2.top,0);
			this->regLayPosition(central);
		}
		for(iter=east.begin();iter!=east.end();iter++)
			if(*iter!= moved && *iter!=exam){
				::GetWindowRect(*iter,&tmp3);
				SetWindowPos(*iter,0,tmpCoord,tmp3.top,tmp3.right-tmpCoord,tmp3.bottom-tmp3.top,0);	
				this->regLayPosition(*iter);
			}
	}

	if(central==exam){
		for(iter=north.begin();iter!=north.end();iter++){
			if(*iter!=moved){
			::GetWindowRect(*iter,& tmp1);
			SetWindowPos(*iter,0,tmp1.left,tmp1.top,tmp1.right-tmp1.left,tmp2.top-tmp1.top,0);
			this->regLayPosition(*iter);
			}
		}
		for(iter=south.begin();iter!=south.end();iter++){
			if(*iter!=moved){
			::GetWindowRect(*iter,& tmp1);
			SetWindowPos(*iter,0,tmp1.left,tmp2.bottom,tmp1.right-tmp1.left,tmp1.bottom-tmp2.bottom,0);
			this->regLayPosition(*iter);
			}
		}
		for(iter=west.begin();iter!=west.end();iter++){
			if(*iter!=moved){
			::GetWindowRect(*iter,& tmp1);
			if(west.size()==1){
				SetWindowPos(*west.begin(),0,tmp1.left,tmp2.top,tmp2.left-tmp1.left,tmp2.bottom-tmp2.top,0);
				this->regLayPosition(*west.begin());
			}
			else if(*iter==*west.begin()){
				SetWindowPos(*west.begin(),0,tmp1.left,tmp2.top,tmp2.left-tmp1.left,tmp1.bottom-tmp2.top,0);
				this->regLayPosition(*west.begin());
			}
			else if(*iter==*west.rbegin()){
				SetWindowPos(*west.rbegin(),0,tmp1.left,tmp1.top,tmp2.left-tmp1.left,tmp2.bottom-tmp1.top,0);
				this->regLayPosition(*west.rbegin());
			}
			else{
				SetWindowPos(*iter,0,tmp1.left,tmp1.top,tmp2.left-tmp1.left,tmp1.bottom-tmp1.top,0);
				this->regLayPosition(*iter);	
				}
			}
		}
		for(iter=east.begin();iter!=east.end();iter++){
			if(*iter!=moved){
			::GetWindowRect(*iter,& tmp1);
			if(east.size()==1){
				SetWindowPos(*east.begin(),0,tmp2.right,tmp2.top,tmp1.right-tmp2.right,tmp2.bottom-tmp2.top,0);			
				this->regLayPosition(*east.begin());
			}
			else if(*iter==*east.begin()){
				SetWindowPos(*east.begin(),0,tmp2.right,tmp2.top,tmp1.right-tmp2.right,tmp1.bottom-tmp2.top,0);
				this->regLayPosition(*east.begin());
			}
			else if(*iter==*east.rbegin()){
				SetWindowPos(*east.rbegin(),0,tmp2.right,tmp1.top,tmp1.right-tmp2.right,tmp2.bottom-tmp1.top,0);
				this->regLayPosition(*east.rbegin());
			}
			else{
				SetWindowPos(*iter,0,tmp2.right,tmp1.top,tmp1.right-tmp2.right,tmp1.bottom-tmp1.top,0);
				this->regLayPosition(*iter);
				}
			}
		}
		::GetWindowRect(*east.begin(),&tmp3);
		::GetWindowRect(*west.begin(),&tmp4);
		
	}
	//TROVA TUTTO LO SPAZIO TRANNE TRA LE GRIGLIE NEI BORDI....
}


bool BorderLayout::isMoved(HWND hwnd){
	RECT newPos,oldPos;
	::GetWindowRect(hwnd,& newPos);
	this->regPositions.findElement(hwnd,& oldPos);
	if(oldPos.bottom!=newPos.bottom && oldPos.left!=newPos.left && oldPos.right!=newPos.right && oldPos.top!=newPos.top)
		return true;
	return false;
}

void BorderLayout::regLayPosition(){
	std::list<HWND>::iterator iterNorth,iterSouth,iterEast,iterWest;	
	iterNorth=north.begin();
	iterSouth=south.begin();
	iterEast=east.begin();
	iterWest=west.begin();	
	RECT temp;
	for(;iterNorth!=north.end();iterNorth++){
		::GetWindowRect(*iterNorth,& temp);
		this->regPositions.insertKeyElement(*iterNorth,& temp);
	}
	for(;iterWest!=west.end();iterWest++){
		::GetWindowRect(*iterWest,& temp);
		this->regPositions.insertKeyElement(*iterWest,& temp);
	}
	for(;iterEast!=east.end();iterEast++){
		::GetWindowRect(*iterEast,& temp);
		this->regPositions.insertKeyElement(*iterEast,& temp);
	}
	for(;iterSouth!=south.end();iterSouth++){
		::GetWindowRect(*iterNorth,& temp);
		this->regPositions.insertKeyElement(*iterSouth,& temp);
	}
	if(central!=NULL){
		::GetWindowRect(central,& temp);
		this->regPositions.insertKeyElement(central,& temp);
	}//FARE UNA FUNZIONE KE KONTIENE STA ROBBA.....

}


void BorderLayout::regLayPosition(HWND added){
	RECT temp;
	::GetWindowRect(added,&temp);
	
		if(!this->regPositions.insertKeyElement(added,&temp))
			this->regPositions.modifyElement(added,&temp);
}



void BorderLayout::registerNewBorder(){
	std::list<HWND>::iterator iterNorth,iterSouth,iterEast,iterWest;	
	iterNorth=north.begin();
	iterSouth=south.begin();
	iterEast=east.begin();
	iterWest=west.begin();	
	RECT temp;
	int maxBord=0;
	for(;iterNorth!=north.end();iterNorth++){
		::GetWindowRect(*iterNorth,& temp);
		if(temp.bottom>maxBord)
			maxBord=temp.bottom;
	}
	if(maxBord!=0)
		this->centerBorderTop=maxBord;
	maxBord=0;
	for(;iterWest!=west.end();iterWest++){
		::GetWindowRect(*iterWest,& temp);
		if(temp.right>maxBord)
			maxBord=temp.right;
	}
	if(maxBord!=0)
		this->centerBorderLeft=maxBord;
	maxBord=width;
	for(;iterEast!=east.end();iterEast++){
		::GetWindowRect(*iterEast,& temp);
		if(temp.left<maxBord)
			maxBord=temp.left;
	}
	if(maxBord!=width)
	this->centerBorderRight=maxBord;
	maxBord=height;
	for(;iterSouth!=south.end();iterSouth++){
		::GetWindowRect(*iterSouth,& temp);
		if(temp.top<maxBord)
			maxBord=temp.top;
	}
	if(maxBord!=height)
		this->centerBorderBottom=maxBord;
}


bool BorderLayout::getRegisteredPosition(HWND hwnd,LPRECT position){
	if(regPositions.findElement(hwnd,position))	
		return true;
	return false;
}

bool BorderLayout::anchorElement(HWND hwnd,int position,int listPos){
	RECT temp;
	std::list<HWND>::iterator iter;
	int i=0;
	bool added=false;
	switch (position){
	case TOP:
		if(north.size()==MAXZONEWINDOWS)
			return false;
		for(iter=north.begin();iter!=north.end() && !added;iter++){
			if(i==listPos){
				north.insert(iter,hwnd);
				added=true;
			}
			i++;
		}
		if(!added && listPos==north.size())
			north.push_back(hwnd);

		//	this->refreshLayout();
	break;
	case LEFT:
		if(west.size()==MAXZONEWINDOWS)
			return false;
		for(iter=west.begin();iter!=west.end() && !added;iter++){
			if(i==listPos){
				west.insert(iter,hwnd);
				added=true;
			}
			i++;
		}	//	this->refreshLayout();		
		if(!added && listPos==west.size())
			west.push_back(hwnd);

	break;
	case RIGHT:
		if(east.size()==MAXZONEWINDOWS)
			return false;	
		for(iter=east.begin();iter!=east.end()&& !added;iter++){
			if(i==listPos){
				east.insert(iter,hwnd);
				added=true;
			}
			i++;
		}
			//	this->refreshLayout();
		if(!added && listPos==east.size())
			east.push_back(hwnd);

	break;
	case BOTTOM:
		if(south.size()==MAXZONEWINDOWS)
			return false;
		for(iter=south.begin();iter!=south.end() && !added;iter++){
			if(i==listPos){
				south.insert(iter,hwnd);
				added=true;
			}
			i++;
		}
		if(!added && listPos==south.size())
			south.push_back(hwnd);

		break;
	case CENTER:
		if(central!=NULL)
			return false;
		else{
			central=hwnd;
		//	this->refreshLayout();
		}
	break;
	default:	return false;
	}
return true;
}

int BorderLayout::getListPosition(HWND hwnd,int bord){
	std::list<HWND>::iterator iter;
	int i=0;
	switch (bord){
	case TOP:
		if(north.size()==MAXZONEWINDOWS)
			return false;
		for(iter=north.begin();iter!=north.end();iter++){
			if(*iter==hwnd){
				return i;
			}
			i++;
		}
		//	this->refreshLayout();
	break;
	case LEFT:
		if(west.size()==MAXZONEWINDOWS)
			return false;
		for(iter=west.begin();iter!=west.end();iter++){
			if(*iter==hwnd){
				return i;
			}
			i++;
		}	//	this->refreshLayout();		
	break;
	case RIGHT:
		if(east.size()==MAXZONEWINDOWS)
			return false;	
		for(iter=east.begin();iter!=east.end();iter++){
			if(*iter==hwnd){
				return i;
			}
			i++;
		}
			//	this->refreshLayout();
	break;
	case BOTTOM:
		if(south.size()==MAXZONEWINDOWS)
			return false;
		for(iter=south.begin();iter!=south.end();iter++){
			if(hwnd==*iter){
				return i;
			}
			i++;
		}
		break;
	default:
		return -1;
	}

	return-1;
}