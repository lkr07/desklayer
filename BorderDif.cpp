#include"BorderDif.h"
#include<iostream>
using namespace std;

BorderDif::BorderDif(){
height=GetSystemMetrics(SM_CYMAXIMIZED);//devmode.dmPelsHeight;//vedere se getsystemmetrics da anche la dim meno barra start
width=GetSystemMetrics(SM_CXMAXIMIZED);//devmode.dmPelsWidth;
this->minX=::GetSystemMetrics(SM_CXMIN);
this->minY=::GetSystemMetrics(SM_CYMIN);
this->weightEx=0.3;
this->weightWx=0.3;
this->weightSy=0.2;
this->weightNy=0.2;

}

BorderDif::BorderDif(double weightEx,double weightWx,double weightNy, double weightSy){
height=GetSystemMetrics(SM_CYMAXIMIZED);//devmode.dmPelsHeight;//vedere se getsystemmetrics da anche la dim meno barra start
width=GetSystemMetrics(SM_CXMAXIMIZED);//devmode.dmPelsWidth;
this->minX=::GetSystemMetrics(SM_CXMIN);
this->minY=::GetSystemMetrics(SM_CYMIN);
this->weightEx=weightEx;
this->weightWx=weightWx;
this->weightSy=weightSy;
this->weightNy=weightNy;
}

bool BorderDif::anchorElement(HWND hwnd,int position,int listPos){
if(this->checkFull(position))
	return false;
	
	RECT temp;
	layWND nuov;
	std::list<layWND>::iterator iter;
	int i=0;
	bool added=false;
	nuov.hwnd=hwnd;
	for(iter=this->wndBuffer.begin();iter!=wndBuffer.end();iter++)
		if(*iter==nuov){
			nuov=*iter;
		}
		wndBuffer.remove(nuov);
	switch (position){
	case TOP:
		for(iter=north.begin();iter!=north.end() && !added;iter++){
			if(i==listPos){
				north.insert(iter,nuov);
				added=true;
			}
			i++;
		}
		if(!added){
			north.push_back(nuov);
		}

		//	this->refreshLayout();
	break;
	case LEFT:
		for(iter=west.begin();iter!=west.end() && !added;iter++){
			if(i==listPos){
				west.insert(iter,nuov);
				added=true;
			}
			i++;
		}	//	this->refreshLayout();		
		if(!added){
			west.push_back(nuov);
		}

	break;
	case RIGHT:
		for(iter=east.begin();iter!=east.end()&& !added;iter++){
			if(i==listPos){
				east.insert(iter,nuov);
				added=true;
			}
			i++;
		}
			//	this->refreshLayout();
		if(!added){
			east.push_back(nuov);
		}

	break;
	case BOTTOM:
		for(iter=south.begin();iter!=south.end() && !added;iter++){
			if(i==listPos){
				south.insert(iter,nuov);
				added=true;
			}
			i++;
		}
		if(!added ){
			south.push_back(nuov);
		}

		break;
	case CENTER:
		if(central.hwnd!=NULL)
			return false;
		else{
			central=nuov;
		//	this->refreshLayout();
		}
	break;
	default:	
		return false;
	}
		if(nuov.hwnd!=NULL){
			if((nuov.weightNSx>0 && (position==TOP||position==BOTTOM))||(nuov.weightWEy>0 &&(position==LEFT || position==RIGHT)))
				this->ricalcolateWeight(nuov,position,true);
			else 
				this->assignWeight(nuov,position,true);
		}
	
return true;
}



bool BorderDif::anchorElement(HWND hwnd,int position){
	//int i;
	if(checkFull(position))
		return false;
	RECT temp;
	layWND nuov;
	switch (position){
	case TOP:
			nuov.hwnd=hwnd;
			north.push_back(nuov);		
			//	this->refreshLayout();
	break;
	case LEFT:
			nuov.hwnd=hwnd;
			west.push_back(nuov);
		//	this->refreshLayout();		
	break;
	case RIGHT:
			nuov.hwnd=hwnd;
			east.push_back(nuov);
			//	this->refreshLayout();
	break;
	case BOTTOM:
			nuov.hwnd=hwnd;
			south.push_back(nuov);
		//	this->refreshLayout();
		break;
	case CENTER:
		if(central.hwnd!=NULL)
			return false;
		else{
			nuov.hwnd=hwnd;
			central=nuov;
		//	this->refreshLayout();
		}
	break;
	default:
		return false;
	}
//	::GetWindowRect(hwnd,&temp);
	HRGN hrgn=CreateRectRgn(0,0,0,0);
	RGNDATA rgnData;
	int regionType=::GetWindowRgn(hwnd,hrgn);
	if(regionType==SIMPLEREGION){
		::GetRegionData(hrgn,48,& rgnData);
		::CopyRect(& temp,& rgnData.rdh.rcBound);
	}
	else
		::GetWindowRect(hwnd,&temp);
	if(!(temp.top<0 && temp.right<0 && temp.left<0 && temp.bottom<0))
		initPositions.insertKeyElement(hwnd,&temp);
	switch(position){
	case TOP:
		this->assignWeight(*north.rbegin(),position,true);
		break;
	case BOTTOM:
				this->assignWeight(*south.rbegin(),position,true);
		break;
	case LEFT:
			this->assignWeight(*west.rbegin(),position,true);
		break;
	case RIGHT:
			this->assignWeight(*east.rbegin(),position,true);
		break;
	case CENTER:
			this->assignWeight(central,position,true);
		break;
	default:break;
	}
return true;
}



int BorderDif::getListPosition(HWND hwnd,int bord){
	std::list<layWND>::iterator iter;
	int i=0;
	layWND conf;
	conf.hwnd=hwnd;
	switch (bord){
	case TOP:
		if(north.size()==MAXZONEWINDOWS)
			return -1;
		for(iter=north.begin();iter!=north.end();iter++){
			if(*iter==conf){
				return i;
			}
			i++;
		}
		//	this->refreshLayout();
	break;
	case LEFT:
		if(west.size()==MAXZONEWINDOWS)
			return -1;
		for(iter=west.begin();iter!=west.end();iter++){
			if(*iter==conf){
				return i;
			}
			i++;
		}	//	this->refreshLayout();		
	break;
	case RIGHT:
		if(east.size()==MAXZONEWINDOWS)
			return -1;	
		for(iter=east.begin();iter!=east.end();iter++){
			if(*iter==conf){
				return i;
			}
			i++;
		}
			//	this->refreshLayout();
	break;
	case BOTTOM:
		if(south.size()==MAXZONEWINDOWS)
			return -1;
		for(iter=south.begin();iter!=south.end();iter++){
			if(conf==*iter){
				return i;
			}
			i++;
		}
		break;
	default:
		return -1;
	}
	return -1;
}


std::list<layWND> BorderDif::getWindowsInPos(int pos){
		std::list<layWND> temp;
		layWND nuov;
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
		return temp;
	}
}

int BorderDif::isInLayout(HWND toCheck){
	std::list<layWND>::iterator iter;
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

bool BorderDif::isMoved(HWND hwnd){
	RECT newPos,oldPos;
	HRGN hrgn=CreateRectRgn(0,0,0,0);
	RGNDATA rgnData;
	int regionType=::GetWindowRgn(hwnd,hrgn);
	if(regionType==SIMPLEREGION){
		::GetRegionData(hrgn,48,& rgnData);
		::CopyRect(& newPos,& rgnData.rdh.rcBound);
	}
	else
		::GetWindowRect(hwnd,&newPos);
	this->regPositions.findElement(hwnd,& oldPos);
	if(oldPos.bottom!=newPos.bottom && oldPos.left!=newPos.left && oldPos.right!=newPos.right && oldPos.top!=newPos.top)
		return true;
	return false;
}

void BorderDif::refreshScreenSize(){
	bool refresh=false;
	if(GetSystemMetrics(SM_CYMAXIMIZED)!=height || GetSystemMetrics(SM_CXMAXIMIZED)!=width)
		refresh=true;
	height=GetSystemMetrics(SM_CYMAXIMIZED);//devmode.dmPelsHeight;//vedere se getsystemmetrics da anche la dim meno barra start
	width=GetSystemMetrics(SM_CXMAXIMIZED);//devmode.dmPelsWidth;
	this->minX=::GetSystemMetrics(SM_CXMIN);
	this->minY=::GetSystemMetrics(SM_CYMIN);
	if(refresh)
	this->refreshLayout();
}

bool BorderDif::removeElement(HWND toRemove,int pos,bool complete,bool futuRestore){
	layWND toRem;
	std::list<layWND>::iterator iter;
	switch (pos){
		case TOP:
			if(north.size()>0){
				for(iter=north.begin();iter!=north.end();iter++)
					if(*iter==toRemove)
						toRem=*iter;
				north.remove(toRem);
			}
			break;
		case LEFT:
			if(west.size()>0){
				for(iter=west.begin();iter!=west.end();iter++)
					if(*iter==toRemove)
						toRem=*iter;
				west.remove(toRem);
			}
			break;
		case RIGHT:
			if(east.size()>0){
				for(iter=east.begin();iter!=east.end();iter++)
					if(*iter==toRemove)
						toRem=*iter;
				east.remove(toRem);
			}
			break;
		case BOTTOM:
			if(south.size()>0){
				for(iter=south.begin();iter!=south.end();iter++)
					if(*iter==toRemove)
						toRem=*iter;
				south.remove(toRem);
			}
			break;
		case CENTER:
			toRem=central;
			central.hwnd=NULL;
			central.weightNSx=0;
			central.weightWEy=0;
			refreshLayout();
			break;
		default:
			break;
		}
	if(pos==TOP || pos==BOTTOM)
		ridistribuiteWeight(pos,toRem.weightNSx,true);
	else if(pos==LEFT || pos==RIGHT)
		ridistribuiteWeight(pos,toRem.weightWEy,true);
	if(complete){
	this->initPositions.delKeyElement(toRemove);
	this->regPositions.delKeyElement(toRemove);
	}
	if(futuRestore){
		this->wndBuffer.push_back(toRem);
	}
	else{
		this->wndBuffer.remove(toRem);
	}
	return true;
}

bool BorderDif::removeElement(HWND toRemove,int pos,bool complete,bool futuRestore,bool refresh){
	layWND toRem;
	std::list<layWND>::iterator iter;
	switch (pos){
		case TOP:
			if(north.size()>0){
				for(iter=north.begin();iter!=north.end();iter++)
					if(*iter==toRemove)
						toRem=*iter;
				north.remove(toRem);
			}
			break;
		case LEFT:
			if(west.size()>0){
				for(iter=west.begin();iter!=west.end();iter++)
					if(*iter==toRemove)
						toRem=*iter;
				west.remove(toRem);
			}
			break;
		case RIGHT:
			if(east.size()>0){
				for(iter=east.begin();iter!=east.end();iter++)
					if(*iter==toRemove)
						toRem=*iter;
				east.remove(toRem);
			}
			break;
		case BOTTOM:
			if(south.size()>0){
				for(iter=south.begin();iter!=south.end();iter++)
					if(*iter==toRemove)
						toRem=*iter;
				south.remove(toRem);
			}
			break;
		case CENTER:
			toRem=central;
			central.hwnd=NULL;
			central.weightNSx=0;
			central.weightWEy=0;
			refreshLayout();
			break;
		default:
			break;
		}
	if(complete){
	this->initPositions.delKeyElement(toRemove);
	this->regPositions.delKeyElement(toRemove);
	}
	if(futuRestore){
		this->wndBuffer.push_back(toRem);
	}
	else{
		this->wndBuffer.remove(toRem);
	}
	return true;
}

void BorderDif::regLayPosition(){
	std::list<layWND>::iterator iterNorth,iterSouth,iterEast,iterWest;	
	iterNorth=north.begin();
	iterSouth=south.begin();
	iterEast=east.begin();
	iterWest=west.begin();	
	RECT temp;
	layWND cur;
	int regionType;
	HRGN hrgn;
	RGNDATA rgnData;
	for(;iterNorth!=north.end();iterNorth++){
		cur=*iterNorth;
				hrgn=CreateRectRgn(0,0,0,0);
				regionType=::GetWindowRgn(cur.hwnd,hrgn);
				if(regionType==SIMPLEREGION){
				::GetRegionData(hrgn,48,& rgnData);
				::CopyRect(& temp,& rgnData.rdh.rcBound);
				}
			else
				::GetWindowRect(cur.hwnd,&temp);
		this->regPositions.insertKeyElement(cur.hwnd,& temp);
	}
	for(;iterWest!=west.end();iterWest++){
		cur=*iterWest;
			hrgn=CreateRectRgn(0,0,0,0);
			regionType=::GetWindowRgn(cur.hwnd,hrgn);
			if(regionType==SIMPLEREGION){
				::GetRegionData(hrgn,48,& rgnData);
				::CopyRect(& temp,& rgnData.rdh.rcBound);
				}
			else
				::GetWindowRect(cur.hwnd,&temp);
		this->regPositions.insertKeyElement(cur.hwnd,& temp);
	}
	for(;iterEast!=east.end();iterEast++){
		cur=*iterEast;
				hrgn=CreateRectRgn(0,0,0,0);
				regionType=::GetWindowRgn(cur.hwnd,hrgn);
				if(regionType==SIMPLEREGION){
				::GetRegionData(hrgn,48,& rgnData);
				::CopyRect(& temp,& rgnData.rdh.rcBound);
				}
			else
				::GetWindowRect(cur.hwnd,&temp);
		this->regPositions.insertKeyElement(cur.hwnd,& temp);
	}
	for(;iterSouth!=south.end();iterSouth++){
		cur=*iterSouth;
				hrgn=CreateRectRgn(0,0,0,0);
				regionType=::GetWindowRgn(cur.hwnd,hrgn);
				if(regionType==SIMPLEREGION){
				::GetRegionData(hrgn,48,& rgnData);
				::CopyRect(& temp,& rgnData.rdh.rcBound);
				}
			else
				::GetWindowRect(cur.hwnd,&temp);
		this->regPositions.insertKeyElement(cur.hwnd,& temp);
	}
	if(central.hwnd!=NULL){
				hrgn=CreateRectRgn(0,0,0,0);
				regionType=::GetWindowRgn(central.hwnd,hrgn);
				if(regionType==SIMPLEREGION){
				::GetRegionData(hrgn,48,& rgnData);
				::CopyRect(& temp,& rgnData.rdh.rcBound);
				}
			else
				::GetWindowRect(central.hwnd,&temp);
		this->regPositions.insertKeyElement(central.hwnd,& temp);
	}

}

void BorderDif::regLayPosition(HWND added){
	RECT temp;
	HRGN hrgn=CreateRectRgn(0,0,0,0);
	RGNDATA rgnData;
	int regionType=::GetWindowRgn(added,hrgn);
		if(regionType==SIMPLEREGION){
			::GetRegionData(hrgn,48,& rgnData);
			::CopyRect(& temp,& rgnData.rdh.rcBound);
			}
		else
			::GetWindowRect(added,&temp);
		if(!this->regPositions.insertKeyElement(added,&temp))
			this->regPositions.modifyElement(added,&temp);
}

void BorderDif::restorePositions(){
	std::list<layWND>::iterator iterNorth,iterSouth,iterEast,iterWest;	
	iterNorth=north.begin();
	iterSouth=south.begin();
	iterEast=east.begin();
	iterWest=west.begin();	
	RECT temp;
	layWND cur;
	WINDOWPLACEMENT wndplc;//finire restore....allungare le finestre quando possibile....
	for(;iterNorth!=north.end();iterNorth++){
		cur=*iterNorth;
		if(initPositions.findElement(cur.hwnd,&temp)){
			if(!::IsRectEmpty(&temp)){
				::GetWindowPlacement(cur.hwnd,& wndplc);
				CopyRect(& wndplc.rcNormalPosition,& temp);
				wndplc.showCmd=SW_SHOWNORMAL;
				::SetWindowPlacement(cur.hwnd,& wndplc);
			SetWindowPos(cur.hwnd,0,temp.left,temp.top,temp.right-temp.left,temp.bottom-temp.top,0);
			}
		}
	}
	for(;iterWest!=west.end();iterWest++){
		cur=*iterWest;
		if(initPositions.findElement(cur.hwnd,&temp)){
			if(!::IsRectEmpty(&temp)){
				::GetWindowPlacement(cur.hwnd,& wndplc);
				CopyRect(& wndplc.rcNormalPosition,& temp);
				wndplc.showCmd=SW_SHOWNORMAL;
				::SetWindowPlacement(cur.hwnd,& wndplc);
				SetWindowPos(cur.hwnd,0,temp.left,temp.top,temp.right-temp.left,temp.bottom-temp.top,0);
			}
		}
	}
	for(;iterEast!=east.end();iterEast++){
		cur=*iterEast;
		if(initPositions.findElement(cur.hwnd,&temp)){
			if(!::IsRectEmpty(&temp)){
				::GetWindowPlacement(cur.hwnd,& wndplc);
				CopyRect(& wndplc.rcNormalPosition,& temp);
				wndplc.showCmd=SW_SHOWNORMAL;
				::SetWindowPlacement(cur.hwnd,& wndplc);
				SetWindowPos(cur.hwnd,0,temp.left,temp.top,temp.right-temp.left,temp.bottom-temp.top,0);
			}
		}	
	}
	for(;iterSouth!=south.end();iterSouth++){
		cur=*iterSouth;
		if(initPositions.findElement(cur.hwnd,&temp)){
			if(!::IsRectEmpty(&temp)){
				::GetWindowPlacement(cur.hwnd,& wndplc);
				CopyRect(& wndplc.rcNormalPosition,& temp);
				wndplc.showCmd=SW_SHOWNORMAL;
				::SetWindowPlacement(cur.hwnd,& wndplc);
				SetWindowPos(cur.hwnd,0,temp.left,temp.top,temp.right-temp.left,temp.bottom-temp.top,0);
			}
		}
	}
	if(central.hwnd!=NULL){
		if(initPositions.findElement(central.hwnd,&temp)){
			if(!::IsRectEmpty(&temp)){
				::GetWindowPlacement(central.hwnd,&wndplc);
				CopyRect(&wndplc.rcNormalPosition, &temp);
				wndplc.showCmd=SW_SHOWNORMAL;
				::SetWindowPlacement(central.hwnd,& wndplc);
				SetWindowPos(central.hwnd,0,temp.top,temp.left,temp.right-temp.left,temp.bottom-temp.top,0);
			}
		}
	}

}

bool BorderDif::toCenter(layWND toMove,int pos){
	if(toMove==central)
		return true;
	layWND sended;
	std::list<layWND>::iterator iter;
	int direction=-1 ;
	switch(pos){
	case TOP:
		if(north.size()>0)
		for(iter=north.begin();iter!=north.end();iter++){ 
			if(*iter==toMove){
				toMove=*iter;
				direction=0;
				if(central.hwnd!=NULL){
					north.insert(iter,central);
					sended=central;
				}
				central=toMove;
				}
	}
		break;
	case LEFT:
		
	if(west.size()>0)
	for(iter=west.begin();iter!=west.end();iter++){
		if(*iter==toMove){
			toMove=*iter;
			direction=1;
			if(central.hwnd!=NULL){
					west.insert(iter,central);
					sended=central;
				}
				central=toMove;
			}
	}
	break;
	case RIGHT:
		if(east.size()>0)
	for(iter=east.begin();iter!=east.end();iter++){ 
		if(*iter==toMove){
			toMove=*iter;
			direction=2;
			if(central.hwnd!=NULL){
				east.insert(iter,central);
				sended=central;
				}
				central=toMove;
		}
	}
	break;
	case BOTTOM:
		if(south.size()>0)
	for(iter=south.begin();iter!=south.end();iter++){ 
		if(*iter==toMove){
			toMove=*iter;
			direction=3;
			if(central.hwnd!=NULL){
					south.insert(iter,central);
					sended=central;
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
	if(direction>-1){
		if(sended.hwnd!=NULL){
			if((sended.weightNSx>0 && (direction==TOP||direction==BOTTOM))||(sended.weightWEy>0 &&(direction==LEFT || direction==RIGHT)))
				this->ricalcolateWeight(sended,direction,false);
			else 
				this->assignWeight(sended,direction,false);
		}
		removeElement(toMove.hwnd,direction,false,false);
	}
	return true;
}

void BorderDif::moveElement(HWND activeWnd,int oldPos,int newPos){
	layWND cur;
	std::list<layWND>::iterator iter;
	if(checkFull(newPos))
		return;
	switch (oldPos){
		case TOP:
			if(north.size()>0){
				for(iter=north.begin();iter!=north.end();iter++)
					if(*iter==activeWnd)
						cur=*iter;
				north.remove(cur);
			}
			break;
		case LEFT:
			if(west.size()>0){
				for(iter=west.begin();iter!=west.end();iter++)
					if(*iter==activeWnd)
						cur=*iter;
				west.remove(cur);
			}
			break;
		case RIGHT:
			if(east.size()>0){
				for(iter=east.begin();iter!=east.end();iter++)
					if(*iter==activeWnd)
						cur=*iter;
				east.remove(cur);
			}
			break;
		case BOTTOM:
			if(south.size()>0){
				for(iter=south.begin();iter!=south.end();iter++)
					if(*iter==activeWnd)
						cur=*iter;
				south.remove(cur);
			}
			break;
		case CENTER:
			if(central.hwnd!=NULL && central.hwnd==activeWnd){
				cur=central;
				central.hwnd=NULL;
				central.weightNSx=0;
				central.weightWEy=0;
			}
			break;
		default:
			return;
		}
	if(oldPos!=CENTER){
		if(oldPos==TOP || oldPos==BOTTOM)
			this->ridistribuiteWeight(oldPos,cur.weightNSx,true);
		else if(oldPos==RIGHT || oldPos==LEFT)
			this->ridistribuiteWeight(oldPos,cur.weightWEy,true);
	}
	switch (newPos){
	case TOP:
		if(north.size()==MAXZONEWINDOWS)
			return;
			north.push_back(cur);
		//	this->refreshLayout();
	break;
	case LEFT:
		if(west.size()==MAXZONEWINDOWS)
			return;
			west.push_back(cur);
		//	this->refreshLayout();		
	break;
	case RIGHT:
		if(east.size()==MAXZONEWINDOWS)
			return;
			east.push_back(cur);
			//	this->refreshLayout();
	break;
	case BOTTOM:
		if(south.size()==MAXZONEWINDOWS)
			return;
			south.push_back(cur);
		//	this->refreshLayout();
		break;
	default:
		return ;
	}

	if((cur.weightNSx>0 && (newPos==TOP||newPos==BOTTOM))||(cur.weightWEy>0 &&(newPos==LEFT || newPos==RIGHT)))
		this->ricalcolateWeight(cur,newPos,true);
	else 	
		this->assignWeight(cur,newPos,true);
	bringToTop(cur);
}


void BorderDif::pushToCenter(HWND toPush){
	if(this->central.hwnd==NULL){
		this->anchorElement(toPush,CENTER);
		return;
	}
	srand(time(0));
	this->moveElement(central.hwnd,CENTER,rand()%4);
	this->anchorElement(toPush,CENTER);
}

void BorderDif::refreshLayout(){
	
	int tempX,tempY,i=0;
	int virtBord;
	WINDOWPLACEMENT wndplc;
	RECT newRect;	
	centerTempTop=height*weightNy;
	centerTempRight=width-(width*this->weightEx);
	centerTempLeft=width*weightWx;
	centerTempBottom=height-(height*this->weightSy);
	std::list<layWND>::iterator iter;
	layWND cur;
	int plus;
	checkSum();
	HWND wind;
	if(north.size()>0){
		i=0;
		tempX=0;
		tempY=0;
		if(east.size()>0)
			virtBord=centerTempRight;
		else
			virtBord=width;
		for(iter=north.begin();iter!=north.end();iter++){
			//fare funzione per provare ad assegare la posizione con setwindowPlacement
			cur=*iter;
			wind=cur.hwnd;
			GetWindowPlacement(cur.hwnd,& wndplc);
			newRect.left=tempX;
			newRect.top=tempY;
			newRect.right=(virtBord*cur.weightNSx)+tempX;
			newRect.bottom=centerTempTop;
			wndplc.rcNormalPosition=newRect;
			wndplc.showCmd=SW_SHOWNORMAL;
			SetWindowPlacement(cur.hwnd,& wndplc);
			//::SetWindowPos(cur.hwnd,NULL,tempX,tempY,(virtBord*cur.weightNSx),centerTempTop,0);
			tempX=tempX+(virtBord * cur.weightNSx);
			this->regLayPosition(cur.hwnd);
			i++;
		}
	}
	else
		centerTempTop=0;
//	if(north.size()>0)
//		centerTempRight=tempX;
	if(east.size()>0){
		i=0;
		tempX=centerTempRight;
		tempY=0;
		if(south.size()>0)
			virtBord=centerTempBottom;
		else
			virtBord=height;
		for(iter=east.begin();iter!=east.end();iter++){
			cur=*iter;
			wind=cur.hwnd;
			GetWindowPlacement(wind,& wndplc);
			newRect.left=tempX;
			newRect.top=(tempY);
			newRect.right=width;
			newRect.bottom=(tempY)+(virtBord*cur.weightWEy);
			wndplc.rcNormalPosition=newRect;
			wndplc.showCmd=SW_SHOWNORMAL;
			SetWindowPlacement(wind,& wndplc);
			//::SetWindowPos(wind,NULL,tempX,tempY,width-tempX,(virtBord*cur.weightWEy),0);
			tempY=tempY+(virtBord*cur.weightWEy);
			this->regLayPosition(wind);
			i++;
		}
		
	}
	else
		centerTempRight=width;
//	if(east.size()>0)
//		centerTempBottom=tempY;
	if(south.size()>0){
		i=0;
		if(west.size()>0)
			tempX=(width*weightWx);
		else
			tempX=0;
		tempY=centerTempBottom;
		virtBord=width-tempX;
		for(iter=south.begin();iter!=south.end();iter++){
			cur=*iter;
			wind=cur.hwnd;
			GetWindowPlacement(cur.hwnd,& wndplc);
			newRect.left=tempX;
			newRect.top=tempY;
			newRect.right=(virtBord*cur.weightNSx)+tempX;
			newRect.bottom=height;
			wndplc.rcNormalPosition=newRect;
			wndplc.showCmd=SW_SHOWNORMAL;
			SetWindowPlacement(wind,& wndplc);
			//::SetWindowPos(wind,NULL,tempX,tempY,virtBord*cur.weightNSx,height-tempY,0);
			tempX=tempX+(virtBord*cur.weightNSx);
			this->regLayPosition(wind);
			i++;	
		}
		
	}
	else
		centerTempBottom=height;
	if(west.size()>0){
		i=0;
		tempX=0;
		tempY=centerTempTop;
		virtBord=height-centerTempTop;
		for(iter=west.begin();iter!=west.end();iter++){
			cur=*iter;
			wind=cur.hwnd;
			GetWindowPlacement(wind,& wndplc);
			newRect.left=tempX;
			newRect.top=tempY;
			newRect.right=centerTempLeft;
			newRect.bottom=(virtBord*cur.weightWEy)+tempY;
			wndplc.rcNormalPosition=newRect;
			wndplc.showCmd=SW_SHOWNORMAL;
			SetWindowPlacement(wind,& wndplc);
			//::SetWindowPos(wind,NULL,tempX,tempY,centerTempLeft,virtBord*cur.weightWEy,0);
			tempY=tempY+(virtBord*cur.weightWEy);
			this->regLayPosition(wind);
			i++;
		}
		
	}
	else
		centerTempLeft=0;


	if(this->central.hwnd != NULL){
		GetWindowPlacement(central.hwnd,& wndplc);
		newRect.left=centerTempLeft;
		newRect.top=centerTempTop;
		newRect.right=centerTempRight;
		newRect.bottom=centerTempBottom;
		wndplc.rcNormalPosition=newRect;
		wndplc.showCmd=SW_SHOWNORMAL;
		SetWindowPlacement(central.hwnd,& wndplc);
		//::SetWindowPos(central.hwnd,NULL,centerTempLeft,centerTempTop,centerTempRight-centerTempLeft,centerTempBottom-centerTempTop,0);			
		this->regLayPosition(central.hwnd);	
	}
	if(east.size()>0)
	this->weightEx=(width-centerTempRight)/width;
	if(north.size()>0)
	this->weightNy=centerTempTop/height;
	if(south.size()>0)
	this->weightSy=(height-centerTempBottom)/height;
	if(west.size()>0)
	this->weightWx=centerTempLeft/width;
	this->checkIntegrity();
}
//ridistribuire pesi alla rimozione...

bool BorderDif::regulateLayout(HWND moved){
	if(isMoved(moved)){
		this->removeElement(moved,this->isInLayout(moved),true,false);
		::SetActiveWindow(moved);
		return false;
	}
	double sum=0;
	RECT movedRect;
	RGNDATA rgnData;
	HRGN hrgn=CreateRectRgn(0,0,0,0);
	int regionType=::GetWindowRgn(moved,hrgn);
	if(regionType==SIMPLEREGION){
			::GetRegionData(hrgn,48,& rgnData);
			::CopyRect(& movedRect,& rgnData.rdh.rcBound);
		}
		else
			::GetWindowRect(moved,&movedRect);
	int pos=this->isInLayout(moved);
	RECT prevRect, tmp;
	std::list<layWND>::iterator iter,found;
	layWND cur,other,adjust;
	int index;
	bool scrolled=false;
	double difference,temp;
	switch(pos){
		case TOP:
			index=this->getListPosition(moved,TOP);
			if(index==(north.size()-1) && east.size()>0){
					other=*east.begin();
					hrgn=CreateRectRgn(0,0,0,0);
					regionType=::GetWindowRgn(other.hwnd,hrgn);
					if(regionType==SIMPLEREGION){
						::GetRegionData(hrgn,48,& rgnData);
						::CopyRect(& prevRect,& rgnData.rdh.rcBound);
					}
					else
						::GetWindowRect(other.hwnd,& prevRect);
					if(prevRect.left>movedRect.right || prevRect.left<movedRect.right){
						centerTempRight=movedRect.right;
						this->weightEx=((width-movedRect.right)/width);
						scrolled=true;
					}
			}
			for(iter=north.begin();iter!=north.end();iter++){
				if(*iter==moved){
					cur=*iter;
					found=iter;
					if(!scrolled)
						break;
				}
				else if(scrolled){
					adjust=*iter;
					hrgn=CreateRectRgn(0,0,0,0);
					regionType=::GetWindowRgn(adjust.hwnd,hrgn);
					if(regionType==SIMPLEREGION){
						::GetRegionData(hrgn,48,& rgnData);
						::CopyRect(& tmp,& rgnData.rdh.rcBound);
					}
					else
						::GetWindowRect(adjust.hwnd,& tmp);
					adjust.weightNSx=(tmp.right-tmp.left)/(centerTempRight);
					sum+=adjust.weightNSx;
					*iter=adjust;
				}

			}
			temp=cur.weightNSx;
			if(cur==*north.begin())
				cur.weightNSx=(movedRect.right-0)/(centerTempRight);
			else
				cur.weightNSx=(movedRect.right-movedRect.left)/(centerTempRight);
			difference=temp-cur.weightNSx;
			sum+=cur.weightNSx;
				if((movedRect.bottom/height)!=this->weightNy){
					this->weightNy=movedRect.bottom/height;
					centerTempTop=movedRect.bottom;
				}
			*found=cur;
			if(!scrolled)
				this->ridistribuiteWeight(TOP,difference,cur,true);
			else if(sum>1)
				this->ridistribuiteWeight(TOP,1-sum,cur,true);
			break;
		case BOTTOM:
			index=this->getListPosition(moved,BOTTOM);
			if(index==0 && west.size()>0){
					other=*west.rbegin();
					hrgn=CreateRectRgn(0,0,0,0);
					regionType=::GetWindowRgn(other.hwnd,hrgn);
					if(regionType==SIMPLEREGION){
						::GetRegionData(hrgn,48,& rgnData);
						::CopyRect(& prevRect,& rgnData.rdh.rcBound);
					}
					else
						::GetWindowRect(other.hwnd,& prevRect);
					if(prevRect.right>movedRect.left || prevRect.right<movedRect.left){
						centerTempLeft=movedRect.left;
						this->weightWx=movedRect.left/width;
						scrolled=true;
					}
			}
			for(iter=south.begin();iter!=south.end();iter++){
				if(*iter==moved){
					cur=*iter;
					found=iter;
					if(!scrolled)
						break;
				}
				else if(scrolled){
					adjust=*iter;
					hrgn=CreateRectRgn(0,0,0,0);
					regionType=::GetWindowRgn(adjust.hwnd,hrgn);
					if(regionType==SIMPLEREGION){
						::GetRegionData(hrgn,48,& rgnData);
						::CopyRect(& tmp,& rgnData.rdh.rcBound);
					}
					else
						::GetWindowRect(adjust.hwnd,& tmp);
					adjust.weightNSx=(tmp.right-tmp.left)/(width-centerTempLeft);
					sum+=adjust.weightNSx;
					*iter=adjust;
				}

			}
			temp=cur.weightNSx;
			if(cur==*south.begin())
				cur.weightNSx=(movedRect.right-centerTempLeft)/(width-centerTempLeft);
			else
				cur.weightNSx=(movedRect.right-movedRect.left)/(width-centerTempLeft);
			difference=temp-cur.weightNSx;
			sum+=cur.weightNSx;
				if(((height-movedRect.top)/height)!=this->weightSy){
					this->weightSy=((height-movedRect.top)/height);
					centerTempBottom=movedRect.top;
				}
			*found=cur;
			if(!scrolled)
			this->ridistribuiteWeight(BOTTOM,difference,cur,true);
			else if(sum>1)
				this->ridistribuiteWeight(BOTTOM,1-sum,cur,true);
			break;
		case LEFT:
			index=this->getListPosition(moved,LEFT);
			if(index==0 && north.size()>0){
					other=*north.begin();
					hrgn=CreateRectRgn(0,0,0,0);
					regionType=::GetWindowRgn(other.hwnd,hrgn);
					if(regionType==SIMPLEREGION){
						::GetRegionData(hrgn,48,& rgnData);
						::CopyRect(& prevRect,& rgnData.rdh.rcBound);
					}
					else
						::GetWindowRect(other.hwnd,& prevRect);
					if(prevRect.bottom>movedRect.top || prevRect.bottom<movedRect.top){
						centerTempTop=movedRect.top;
						this->weightNy=movedRect.top/height;
						scrolled=true;
					}
			}
			for(iter=west.begin();iter!=west.end();iter++){
				if(*iter==moved){
					cur=*iter;
					found=iter;
					if(!scrolled)
						break;
				}
				else if(scrolled){
					adjust=*iter;
					hrgn=CreateRectRgn(0,0,0,0);
					regionType=::GetWindowRgn(adjust.hwnd,hrgn);
					if(regionType==SIMPLEREGION){
						::GetRegionData(hrgn,48,& rgnData);
						::CopyRect(& tmp,& rgnData.rdh.rcBound);
					}
					else
						::GetWindowRect(adjust.hwnd,& tmp);
					adjust.weightWEy=(tmp.bottom-tmp.top)/(height-centerTempTop);
					sum+=adjust.weightWEy;
					*iter=adjust;
				}
			}
			temp=cur.weightWEy;
			if(cur==*west.begin())
					cur.weightWEy=(movedRect.bottom-centerTempTop)/(height-centerTempTop);
				else
					cur.weightWEy=(movedRect.bottom-movedRect.top)/(height-centerTempTop);
			difference=temp-cur.weightWEy;
			sum+=cur.weightWEy;
				if((movedRect.right/width)!=this->weightWx){
					this->weightWx=(movedRect.right/width);//attenzione...se ridimensiono a destra tutto sud si deve regolare....lo stesso er altri lati..
					centerTempLeft=movedRect.right;
				}
			*found=cur;
			if(!scrolled)
			this->ridistribuiteWeight(LEFT,difference,cur,true);
			else if(sum>1)
				this->ridistribuiteWeight(LEFT,1-sum,cur,true);
			break;
		case RIGHT:
			index=this->getListPosition(moved,RIGHT);
			if(index==east.size()-1 && south.size()>0){
					other=*south.begin();
					hrgn=CreateRectRgn(0,0,0,0);
					regionType=::GetWindowRgn(other.hwnd,hrgn);
					if(regionType==SIMPLEREGION){
						::GetRegionData(hrgn,48,& rgnData);
						::CopyRect(& prevRect,& rgnData.rdh.rcBound);
					}
					else
						::GetWindowRect(other.hwnd,& prevRect);
					if(prevRect.top>movedRect.bottom || prevRect.top<movedRect.bottom){
						centerTempBottom=movedRect.bottom;
						this->weightSy=(height-movedRect.bottom)/height;
						scrolled=true;
					}
			}
			for(iter=east.begin();iter!=east.end();iter++){
				if(*iter==moved){
					cur=*iter;
					found=iter;
					if(!scrolled)
						break;
				}
				else if(scrolled){
					adjust=*iter;
					hrgn=CreateRectRgn(0,0,0,0);
					regionType=::GetWindowRgn(adjust.hwnd,hrgn);
					if(regionType==SIMPLEREGION){
						::GetRegionData(hrgn,48,& rgnData);
						::CopyRect(& tmp,& rgnData.rdh.rcBound);
					}
					else
						::GetWindowRect(adjust.hwnd,& tmp);
					adjust.weightWEy=(tmp.bottom-tmp.top)/(centerTempBottom);
					sum+=adjust.weightWEy;
					*iter=adjust;
				}
			}
			temp=cur.weightWEy;
			if(cur==*east.begin())
				cur.weightWEy=(movedRect.bottom-0)/(centerTempBottom);
			else
				cur.weightWEy=(movedRect.bottom-movedRect.top)/(centerTempBottom);
			difference=temp-cur.weightWEy;
			sum+=cur.weightWEy;
				if(((width-movedRect.left)/width)!=this->weightEx){
					this->weightEx=((width-movedRect.left)/width);
					centerTempRight=movedRect.left;
				/*	for(iter=north.begin();iter!=north.end();iter++){
						adjust=*iter;
						::GetWindowRect(adjust.hwnd,& tmp);
						adjust.weightNSx=(tmp.right-tmp.left)/centerTempRight;
						*iter=adjust;
					}
					*/
				}
				*found=cur;
				if(!scrolled)
				this->ridistribuiteWeight(RIGHT,difference,cur,true);
				else if(sum>1 )//&& cur.weightWEy<1) per evitare problemi con finestre grandi già in principio più della dimensione della cella sennò eliminare cur da ridist successivo(se sum>1)
					this->ridistribuiteWeight(RIGHT,1-sum,cur,true);
			break;
		case CENTER:
			if(movedRect.right!=centerTempRight && east.size()!=0){
				this->weightEx=(width-movedRect.right)/width;
				centerTempRight=movedRect.right;
				/*	for(iter=north.begin();iter!=north.end();iter++){
						adjust=*iter;
						::GetWindowRect(adjust.hwnd,& tmp);
						adjust.weightNSx=(tmp.right-tmp.left)/centerTempRight;
						*iter=adjust;
					}
					*/
			}
			if(movedRect.left!=centerTempLeft && west.size()>0){
				this->weightWx=(movedRect.left)/width;
				centerTempLeft=movedRect.left;
		/*		for(iter=south.begin();iter!=south.end();iter++){
					adjust=*iter;
					::GetWindowRect(adjust.hwnd,& tmp);
					adjust.weightNSx=(tmp.right-tmp.left)/(width-centerTempLeft);
					*iter=adjust;
				}*/
			}
			if(movedRect.bottom!=centerTempBottom && south.size()>0){
				this->weightSy=(height-movedRect.bottom)/height;
				centerTempBottom=movedRect.bottom;
			/*	for(iter=east.begin();iter!=east.end();iter++){
					adjust=*iter;
					::GetWindowRect(adjust.hwnd,& tmp);
					adjust.weightWEy=(tmp.bottom-tmp.top)/(centerTempBottom);
					*iter=adjust;
				}
				*/
			}
			if(movedRect.top!=centerTempTop && north.size()>0){
				this->weightNy=movedRect.top/height;
				centerTempTop=movedRect.top;
			/*	for(iter=west.begin();iter!=west.end();iter++){
						adjust=*iter;
						::GetWindowRect(adjust.hwnd,& tmp);
						adjust.weightWEy=(tmp.bottom-tmp.top)/(height-centerTempTop);
						*iter=adjust;
				}
				*/
			}
			break;
	}
	adjustWeight();
	this->checkSum(moved);
	return true;
}



void BorderDif::ridistribuiteWeight(int pos,double weighToGive,bool refresh){
	std::list<layWND>::iterator iter;
	layWND cur;
	switch (pos){
	case TOP:
		for(iter=north.begin();iter!=north.end();iter++){
			cur=*iter;
			cur.weightNSx+=weighToGive/north.size();
			*iter=cur;
		}
	break;
	case LEFT:
		for(iter=west.begin();iter!=west.end() ;iter++){
			cur=*iter;
			cur.weightWEy+=weighToGive/west.size();
			*iter=cur;
		}
	break;
	case RIGHT:
		for(iter=east.begin();iter!=east.end();iter++){
			cur=*iter;
			cur.weightWEy+=weighToGive/east.size();
			*iter=cur;
		}
	break;
	case BOTTOM:
		for(iter=south.begin();iter!=south.end() ;iter++){
			cur=*iter;
			cur.weightNSx+=weighToGive/south.size();
			*iter=cur;
		}
		break;
	default:	
		return ;
	}
	if(refresh)
	refreshLayout();

}

void BorderDif::ridistribuiteWeight(int pos,double weighToGive,layWND exc,bool refresh){
	std::list<layWND>::iterator iter;
	layWND cur;
	switch (pos){
	case TOP:
		for(iter=north.begin();iter!=north.end();iter++){
			cur=*iter;
			if(cur.hwnd!=exc.hwnd)
				cur.weightNSx+=weighToGive/(north.size()-1);
			*iter=cur;
		}
	break;
	case LEFT:
		for(iter=west.begin();iter!=west.end() ;iter++){
			cur=*iter;
			if(cur.hwnd!=exc.hwnd)
				cur.weightWEy+=weighToGive/(west.size()-1);
			*iter=cur;
		}
	break;
	case RIGHT:
		for(iter=east.begin();iter!=east.end();iter++){
			cur=*iter;
			if(cur.hwnd!=exc.hwnd)
				cur.weightWEy+=weighToGive/(east.size()-1);
			*iter=cur;
		}
	break;
	case BOTTOM:
		for(iter=south.begin();iter!=south.end() ;iter++){
			cur=*iter;
			if(cur.hwnd!=exc.hwnd)
				cur.weightNSx+=weighToGive/(south.size()-1);
			*iter=cur;
		}
		break;
	default:	
		return ;
	}
	if(refresh)
	refreshLayout();
}


//per l'aggiunta di una nuova finestra che è di default con peso= a 1.....
void BorderDif::assignWeight(layWND nuova,int pos,bool refresh){
	double averageWeight;
	std::list<layWND>::iterator iter;
	layWND cur;
	switch (pos){
	case TOP:
		if(north.size()>1){
			averageWeight=1.0/north.size();
			for(iter=north.begin();iter!=north.end();iter++){
				cur=*iter;
				if(cur.hwnd==nuova.hwnd){
					cur.weightNSx=averageWeight;
					*iter=cur;
				}
				else{
					cur.weightNSx-=(averageWeight*cur.weightNSx);
					*iter=cur;
				}
			}
		}
		else if(north.size()==1){
			iter=north.begin();
			cur=*iter;
			cur.weightNSx=1;
			*iter=cur;
		}
	break;
	case LEFT:
		if(west.size()>1){
			averageWeight=1.0/west.size();
			for(iter=west.begin();iter!=west.end() ;iter++){
				cur=*iter;
				if(cur.hwnd==nuova.hwnd){
					cur.weightWEy=averageWeight;
					*iter=cur;
				}
				else{
					cur.weightWEy-=(averageWeight*cur.weightWEy);
					*iter=cur;
				}
			}
		}
		else if(west.size()!=0){
			iter=west.begin();
			cur=*iter;
			cur.weightWEy=1;
			*iter=cur;
		}
	break;
	case RIGHT:
		if(east.size()>1){
			averageWeight=1.0/east.size();
			for(iter=east.begin();iter!=east.end() ;iter++){
				cur=*iter;
				if(cur.hwnd==nuova.hwnd){
					cur.weightWEy=averageWeight;
					*iter=cur;
				}
				else{
					cur.weightWEy-=(averageWeight*cur.weightWEy);
					*iter=cur;
				}
			}
		}
		else if(east.size()!=0){
			iter=east.begin();
			cur=*iter;
			cur.weightWEy=1;
			*iter=cur;
		}
		break;
	case BOTTOM:
		if(south.size()>1){
			averageWeight=1.0/south.size();
			for(iter=south.begin();iter!=south.end() ;iter++){
				cur=*iter;
				if(cur.hwnd==nuova.hwnd){
					cur.weightNSx=averageWeight;
					*iter=cur;
				}
				else{
					cur.weightNSx-=(averageWeight*cur.weightNSx);
					*iter=cur;
				}
			}
		}
		else if(south.size()!=0){
			iter=south.begin();
			cur=*iter;
			cur.weightNSx=1;
			*iter=cur;
		}
		break;
	default:	
		break;
	}
	adjustWeight();
	if(refresh)
	refreshLayout();
}



void BorderDif::ricalcolateWeight(layWND moved,int pos,bool refresh){
	double additionalWeight=1;
	std::list<layWND>::iterator iter;
	layWND cur;
	std::list<HWND> minimumSize;
	double origiNSx=moved.weightNSx,origiWEy=moved.weightWEy;
	double averageWeight=0;
	double temp;
	double movedWeight=0,lime=0;
	switch (pos){
	case TOP:
		if(north.size()>2){
			averageWeight=1.0/north.size();
			movedWeight=(averageWeight/2)+(averageWeight*moved.weightNSx);
			for(iter=north.begin();iter!=north.end();iter++){
				if(*iter==moved){
					cur=*iter;
					cur.weightNSx=movedWeight;
					*iter=cur;
				}
				else{
					cur=*iter;
					cur.weightNSx-=((movedWeight*cur.weightNSx));
//					if(cur.weightNSx<=this->minXweight)
//						minimumSize.push_back(cur.hwnd);
					*iter=cur;
				}
			}
		
		}
		else if (north.size()==1){
			cur=*north.begin();
			cur.weightNSx=1;
			*north.begin()=cur;
		}
		else if(north.size()==2){
				if(origiNSx==1)
						temp=0.5;
					else
						temp=1-origiNSx;
			for(iter=north.begin();iter!=north.end();iter++){
				cur=*iter;	
				if(cur==moved){
					cur.weightNSx=(1-temp);
					*iter=cur;
				}
				else{
					cur.weightNSx=temp;
					*iter=cur;
				}
			}
		}
	break;
	case LEFT:
		if(west.size()>2){
			averageWeight=1.0/west.size();
			movedWeight=(averageWeight/2)+(averageWeight*moved.weightWEy);
			for(iter=west.begin();iter!=west.end();iter++){
				if(*iter==moved){
					cur=*iter;
					cur.weightWEy=movedWeight;
					*iter=cur;
				}
				else{
					cur=*iter;
					cur.weightWEy-=((movedWeight*cur.weightWEy));
//					if(cur.weightWEy<=this->minYweight)
//						minimumSize.push_back(cur.hwnd);
					*iter=cur;
				}
			}

		}
		else if (west.size()==1){
			cur=*west.begin();
			cur.weightWEy=1;
			*west.begin()=cur;
		}
		else if(west.size()==2){
				if(origiWEy==1)
						temp=0.5;
					else
						temp=1-origiWEy;
			for(iter=west.begin();iter!=west.end();iter++){
				cur=*iter;	
				if(cur==moved){
					cur.weightWEy=(1-temp);
					*iter=cur;
				}
				else{
					cur.weightWEy=temp;
					*iter=cur;
				}
			}
		}
	break;
	case RIGHT:
		if(east.size()>2){
			averageWeight=1.0/east.size();
			movedWeight=(averageWeight/2)+(averageWeight*moved.weightWEy);
			for(iter=east.begin();iter!=east.end();iter++){
				if(*iter==moved){
					cur=*iter;
					cur.weightWEy=movedWeight;
					*iter=cur;
				}
				else{
					cur=*iter;
					cur.weightWEy-=((movedWeight*cur.weightWEy));
//					if(cur.weightWEy<=this->minYweight)
//						minimumSize.push_back(cur.hwnd);					
					*iter=cur;
				}
			}
		
		}
		else if (east.size()==1){
			cur=*east.begin();
			cur.weightWEy=1;
			*east.begin()=cur;
		}
		else if(east.size()==2){
				if(origiWEy==1)
						temp=0.5;
					else
						temp=1-origiWEy;
			for(iter=east.begin();iter!=east.end();iter++){
				cur=*iter;	
				if(cur==moved){
					cur.weightWEy=(1-temp);
					*iter=cur;
				}
				else{
					cur.weightWEy=temp;
					*iter=cur;
				}
			}
		}
		break;
	case BOTTOM:
		if(south.size()>2){
			averageWeight=1.0/south.size();
			movedWeight=(averageWeight/2)+(averageWeight*moved.weightNSx);
			for(iter=south.begin();iter!=south.end();iter++){
				if(*iter==moved){
					cur=*iter;
					cur.weightNSx=movedWeight;
					*iter=cur;
				}
				else{
					cur=*iter;
					cur.weightNSx-=((movedWeight*cur.weightNSx));
//					if(cur.weightNSx<=this->minXweight)
//						minimumSize.push_back(cur.hwnd);
					*iter=cur;
				}
			}
		
		}
		else if (south.size()==1){
			cur=*south.begin();
			cur.weightNSx=1;
			*south.begin()=cur;
		}
		else if(south.size()==2){
				if(origiNSx==1)
						temp=0.5;
					else
						temp=1-origiNSx;
			for(iter=south.begin();iter!=south.end();iter++){
				cur=*iter;	
				if(cur==moved){
					cur.weightNSx=(1-temp);
					*iter=cur;
				}
				else{
					cur.weightNSx=temp;
					*iter=cur;
				}
			}
		}
		break;
	default:	
		return ;
	}
	std::list<HWND>::iterator iterHwnd;
//	for(iterHwnd=minimumSize.begin();iterHwnd!=minimumSize.end();iterHwnd++)
//		this->regulateLayout(*iterHwnd);
	adjustWeight();
//	adjustWeight();
	if(refresh)
	refreshLayout();

}


//non è utile perchè le finestre troppo piccole vengono spostate automaticamente.
void BorderDif::adjustWeight(){

	std::list<layWND>::iterator iter;
	std::list<bool>::iterator iteRemov;
	RECT temp;
	double toRemove=0;
	layWND cur;
	int count=0;
	std::list<bool> removable;
	double minWeight;
	minWeight=this->minX/centerTempRight;
	for(iter=north.begin();iter!= north.end();iter++){
		cur=*iter;
		if(minWeight>=cur.weightNSx){
			toRemove+=(minWeight-cur.weightNSx);
			cur.weightNSx=minWeight;
			removable.push_back(false);
			count++;
			*iter=cur;
		}
		else 
			removable.push_back(true);
	}
	iteRemov=removable.begin();
	if(toRemove>0){
		for(iter=north.begin();iter!= north.end();iter++){
			cur=*iter;
			if(*iteRemov==true){
				cur.weightNSx-=toRemove/(north.size()-count);
				*iter=cur;
			}
			iteRemov++;
		}
	}
	count=0;
	removable.clear();
	toRemove=0;
	minWeight=this->minY/(height-centerTempTop);
	for(iter=west.begin();iter!=west.end();iter++){
		cur=*iter;
		if(minWeight>=cur.weightWEy){
			toRemove+=(minWeight-cur.weightWEy);
			cur.weightWEy=minWeight;
			removable.push_back(false);
			count++;
			*iter=cur;
		}
		else 
			removable.push_back(true);
	}
	iteRemov=removable.begin();
	if(toRemove>0){
		for(iter=west.begin();iter!=west.end();iter++){
			cur=*iter;
			if(*iteRemov==true){
				cur.weightWEy-=toRemove/(west.size()-count);
				*iter=cur;
			}
			iteRemov++;
		}
	}
	count=0;
	removable.clear();
	toRemove=0;
	minWeight=this->minY/centerTempBottom;
	for(iter=east.begin();iter!=east.end();iter++){
		cur=*iter;
		if(minWeight>=cur.weightWEy){
			toRemove+=(minWeight-cur.weightWEy);
			cur.weightWEy=minWeight;
			removable.push_back(false);
			count++;
			*iter=cur;
		}
		else 
			removable.push_back(true);
	}
	iteRemov=removable.begin();
	if(toRemove>0){
		for(iter=east.begin();iter!=east.end();iter++){
			cur=*iter;
			if(*iteRemov==true){
				cur.weightWEy-=toRemove/(east.size()-count);
				*iter=cur;
			}
			iteRemov++;
		}
	}
	count=0;
	removable.clear();
	toRemove=0;
	minWeight=this->minX/(width-centerTempLeft);
	for(iter=south.begin();iter!= south.end();iter++){
		cur=*iter;
		if(minWeight>=cur.weightNSx){
			toRemove+=(minWeight-cur.weightNSx);
			cur.weightNSx=minWeight;
			removable.push_back(false);
			count++;
			*iter=cur;
		}
		else 
			removable.push_back(true);
	}
	iteRemov=removable.begin();
	if(toRemove>0){
		for(iter=south.begin();iter!= south.end();iter++){
			cur=*iter;
			if(*iteRemov==true){
				cur.weightNSx-=toRemove/(south.size()-count);
				*iter=cur;
			}
			iteRemov++;
		}
	}
//refreshLayout();
}

void BorderDif::getRegion(HWND hwnd,LPRECT rect){

}


bool BorderDif::checkFull(int pos){
	switch (pos){
	case TOP:
		if(north.size()==floor((width-(weightEx*width))/minX))
			return true;
	return false;
	case LEFT:
		if(west.size()==floor((height-(height*weightNy))/minY))
			return true;
	return false;
	case RIGHT:
		if(east.size()==floor((height-(height*weightSy))/minY))
			return true;
	return false;
	case BOTTOM:
		if(south.size()==floor((width-(weightWx*width))/minX))
			return true;
		return false;
	default:
		return false;
	}
}


void BorderDif::checkIntegrity(){
	std::list<layWND>::iterator iter;
	std::list<bool>::iterator iteRemov;
	RECT temp;
	layWND cur;
	double weightMove=0;
	RGNDATA rgnData;
	HRGN hrgn=CreateRectRgn(0,0,0,0);
	int regionType;
	bool someModify=false;
	for(iter=north.begin();iter!= north.end();iter++){
		cur=*iter;
	regionType=::GetWindowRgn(cur.hwnd,hrgn);
	if(regionType==SIMPLEREGION){
			::GetRegionData(hrgn,48,& rgnData);
			::CopyRect(& temp,& rgnData.rdh.rcBound);
	}
	else
		GetWindowRect(cur.hwnd,&temp);
	if(((temp.bottom-temp.top)/height)>weightNy){
		weightNy=((temp.bottom-temp.top)/height);
		someModify=true;
	}
	}
	for(iter=west.begin();iter!=west.end();iter++){
		cur=*iter;
		regionType=::GetWindowRgn(cur.hwnd,hrgn);
	if(regionType==SIMPLEREGION){
			::GetRegionData(hrgn,48,& rgnData);
			::CopyRect(& temp,& rgnData.rdh.rcBound);
	}
	else
		GetWindowRect(cur.hwnd,&temp);
	if(((temp.right-temp.left)/width)>weightWx){
		weightWx=((temp.right-temp.left)/width);
		someModify=true;
	}
	}
	for(iter=east.begin();iter!=east.end();iter++){
		cur=*iter;
			regionType=::GetWindowRgn(cur.hwnd,hrgn);
	if(regionType==SIMPLEREGION){
			::GetRegionData(hrgn,48,& rgnData);
			::CopyRect(& temp,& rgnData.rdh.rcBound);
	}
	else
		GetWindowRect(cur.hwnd,&temp);
	if(((temp.right-temp.left)/width)>weightEx){
		weightEx=((temp.right-temp.left)/width);
		someModify=true;
	}
	}
	for(iter=south.begin();iter!= south.end();iter++){
		cur=*iter;
		regionType=::GetWindowRgn(cur.hwnd,hrgn);
	if(regionType==SIMPLEREGION){
			::GetRegionData(hrgn,48,& rgnData);
			::CopyRect(& temp,& rgnData.rdh.rcBound);
	}
	else
		GetWindowRect(cur.hwnd,&temp);
	if(((temp.bottom-temp.top)/height)>weightSy){
		weightSy=((temp.bottom-temp.top)/height);
		someModify=true;
	}
	}
	if(someModify)
		refreshLayout();
}

void BorderDif::bringToTop(layWND top){
	
	int tempX,tempY,i=0;
	int virtBord;
	WINDOWPLACEMENT wndplc;
	RECT newRect;	
	centerTempTop=height*weightNy;
	centerTempRight=width-(width*this->weightEx);
	centerTempLeft=width*weightWx;
	centerTempBottom=height-(height*this->weightSy);
	std::list<layWND>::iterator iter;
	layWND cur;
	int plus;
	HWND wind;
	if(north.size()>0){
		i=0;
		tempX=0;
		tempY=0;
		if(east.size()>0)
			virtBord=centerTempRight;
		else
			virtBord=width;
		for(iter=north.begin();iter!=north.end();iter++){
			//fare funzione per provare ad assegare la posizione con setwindowPlacement
			cur=*iter;
			wind=cur.hwnd;
			if(top==cur)
				::SetWindowPos(cur.hwnd,HWND_TOP,tempX,tempY,(virtBord*cur.weightNSx),centerTempTop,0);
			tempX=tempX+(virtBord * cur.weightNSx);
			//this->regLayPosition(cur.hwnd);
			i++;
		}
	}
	else
		centerTempTop=0;
	if(north.size()>0)
		centerTempRight=tempX;

	if(east.size()>0){
		i=0;
		tempX=centerTempRight;
		tempY=0;
		if(south.size()>0)
			virtBord=centerTempBottom;
		else
			virtBord=height;
		for(iter=east.begin();iter!=east.end();iter++){
			cur=*iter;
			if(top==cur)
				::SetWindowPos(cur.hwnd,HWND_TOP,tempX,tempY,width-tempX,(virtBord*cur.weightWEy),0);
			tempY=tempY+(virtBord*cur.weightWEy);
			//this->regLayPosition(wind);
			i++;
		}
		
	}
	else
		centerTempRight=width;
	if(east.size()>0)
		centerTempBottom=tempY;
	if(south.size()>0){
		i=0;
		if(west.size()>0)
			tempX=(width*weightWx);
		else
			tempX=0;
		tempY=centerTempBottom;
		virtBord=width-tempX;
		for(iter=south.begin();iter!=south.end();iter++){
			cur=*iter;
			if(cur==top)
				::SetWindowPos(cur.hwnd,NULL,tempX,tempY,virtBord*cur.weightNSx,height-tempY,0);
			tempX=tempX+(virtBord*cur.weightNSx);
			//this->regLayPosition(wind);
			i++;	
		}
		
	}
	else
		centerTempBottom=height;
	if(west.size()>0){
		i=0;
		tempX=0;
		tempY=centerTempTop;
		virtBord=height-centerTempTop;
		for(iter=west.begin();iter!=west.end();iter++){
			cur=*iter;
			if(cur==top)
				::SetWindowPos(cur.hwnd,NULL,tempX,tempY,centerTempLeft,virtBord*cur.weightWEy,0);
			tempY=tempY+(virtBord*cur.weightWEy);
			//this->regLayPosition(wind);
			i++;
		}
		
	}
	else
		centerTempLeft=0;


	if(this->central.hwnd != NULL){
		GetWindowPlacement(central.hwnd,& wndplc);
		if(central==top)
			::SetWindowPos(central.hwnd,NULL,centerTempLeft,centerTempTop,centerTempRight-centerTempLeft,centerTempBottom-centerTempTop,0);			
		//this->regLayPosition(central.hwnd);	
	}
	//this->checkIntegrity();
}


std::list<HWND> BorderDif::minimizeAll(HWND noMin){
	std::list<HWND>toRet;
	std::list<layWND>::iterator iter;
	layWND cur;
	for(iter=north.begin();iter!=north.end();iter++){
		cur=*iter;
		if(!::IsIconic(cur.hwnd))
			toRet.push_back(cur.hwnd);
		}
	for(iter=east.begin();iter!=east.end();iter++){
		cur=*iter;
		if(!::IsIconic(cur.hwnd))
			toRet.push_back(cur.hwnd);
	}
	for(iter=west.begin();iter!=west.end();iter++){
		cur=*iter;
		if(!::IsWindowVisible(cur.hwnd))
			toRet.push_back(cur.hwnd);
	}
	for(iter=south.begin();iter!=south.end();iter++){
		cur=*iter;
		if(!::IsIconic(cur.hwnd))
			toRet.push_back(cur.hwnd);
		}
	if(::IsIconic(central.hwnd))
		toRet.push_back(central.hwnd);
return toRet;
}

void BorderDif::checkSum(){
	std::list<layWND>::iterator iter;
	double sum=0;
	layWND cur;
	for(iter=north.begin();iter!=north.end();iter++){
			cur=*iter;
			sum+=cur.weightNSx;
		}
	if(sum<1)
		this->ridistribuiteWeight(TOP,1-sum,false);
	sum=0;
	for(iter=east.begin();iter!=east.end();iter++){
			cur=*iter;
			sum+=cur.weightWEy;
		}
	if(sum<1)
		this->ridistribuiteWeight(RIGHT,1-sum,false);
	sum=0;	
	for(iter=south.begin();iter!=south.end();iter++){
			cur=*iter;
			sum+=cur.weightNSx;
		}
	if(sum<1)
		this->ridistribuiteWeight(BOTTOM,1-sum,false);
	sum=0;
		for(iter=west.begin();iter!=west.end();iter++){
			cur=*iter;
			sum+=cur.weightWEy;
		}
	if(sum<1)
		this->ridistribuiteWeight(LEFT,1-sum,false);


}

void BorderDif::checkSum(HWND moved){
	std::list<layWND>::iterator iter;
	double sum=0;
	layWND cur;
	layWND exc;
	bool toExclude=false;
	for(iter=north.begin();iter!=north.end();iter++){
			cur=*iter;
			sum+=cur.weightNSx;
			if(cur.hwnd==moved){
				exc=*iter;
				toExclude=true;
			}
		}
	if(sum<1){
		if(!toExclude)
			this->ridistribuiteWeight(TOP,1-sum,false);
		else
			this->ridistribuiteWeight(TOP,1-sum,exc,false);
	}
	sum=0;
	toExclude=false;
	for(iter=east.begin();iter!=east.end();iter++){
			cur=*iter;
			sum+=cur.weightWEy;
			if(cur.hwnd==moved){
				exc=*iter;
				toExclude=true;
			}
		}
	if(sum<1){
		if(!toExclude)
			this->ridistribuiteWeight(RIGHT,1-sum,false);
		else
			this->ridistribuiteWeight(RIGHT,1-sum,exc,false);
	}
	sum=0;	
	toExclude=false;
	for(iter=south.begin();iter!=south.end();iter++){
			cur=*iter;
			sum+=cur.weightNSx;
			if(cur.hwnd==moved){
				exc=*iter;
				toExclude=true;
			}
		}
	if(sum<1){
		if(!toExclude)
			this->ridistribuiteWeight(BOTTOM,1-sum,false);
		else
			this->ridistribuiteWeight(BOTTOM,1-sum,exc,false);
	}
	sum=0;
	toExclude=false;
		for(iter=west.begin();iter!=west.end();iter++){
			cur=*iter;
			sum+=cur.weightWEy;
			if(cur.hwnd==moved){
				exc=*iter;
				toExclude=true;
			}
		}
	if(sum<1){
		if(!toExclude)
			this->ridistribuiteWeight(LEFT,1-sum,false);
		else 
			this->ridistribuiteWeight(LEFT,1-sum,exc,false);
	}
}