#include"GridLayout.h"


GridLayout::GridLayout(){
	this->columns=4;
	this->rows=3;
	this->curRows=0;
	this->curColumns=0;
//	DEVMODE devmode;
//	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devmode);
	height=GetSystemMetrics(SM_CYMAXIMIZED);//devmode.dmPelsHeight;//vedere se getsystemmetrics da anche la dim meno barra start
	width=GetSystemMetrics(SM_CXMAXIMIZED);//devmode.dmPelsWidth;
}


int GridLayout::getColumn(){
return columns;
}

int GridLayout::getRows(){
return rows;
}

int GridLayout::getHeight(){
return height;
}

int GridLayout::getWidth(){
return width;
}

bool GridLayout::isNull(){
return matrix.size()==0;
}


HWND GridLayout::getElement(int row,int column){
	if(row>rows || column>columns)
		return NULL;
	std::list<HWND>::iterator iterMatrix;
	int rowIndex=0;
	int columnIndex=0;
	for(iterMatrix=matrix.begin();iterMatrix!=matrix.end();iterMatrix++){
		if(rowIndex==row && columnIndex==column)
			return *iterMatrix;
		if(columnIndex==columns-1)
			rowIndex++;
		columnIndex=(columnIndex+1)%columns;
	}
	return NULL;
}


void GridLayout::removeElement(HWND hwnd,bool completely){//la funzione remove va kiamata quando si ha un WM_CLOSE...o HCB_DESTROYWND
	matrix.remove(hwnd);
	if(completely){
		initPositions.delKeyElement(hwnd);
		regPositions.delKeyElement(hwnd);
	}
	this->drawLayout();
}

void GridLayout::drawLayout(){
	if(matrix.size()>0){
	bool odd=false;
	bool oddLine=false;
	if((matrix.size()/columns)<1){
		this->curRows=1;
		this->curColumns=matrix.size()/curRows;
	}
	else if((matrix.size()/columns)<=2){
		this->curRows=2;
		this->curColumns=matrix.size()/curRows;
		if(matrix.size()>(curRows * curColumns))
			odd=true;
	}
	else {
		this->curRows=3;
		this->curColumns=matrix.size()/curRows;
		if(matrix.size()>(curRows * curColumns))
			odd=true;
	}
	int indexR=0,indexC=0,drawed=0;
	int deltaX=width/curColumns;
	RECT newRect;
	WINDOWPLACEMENT wndplc;
	int deltaY=height/curRows;
	int deltaOdd=width/(curColumns+1);
	std::list<HWND>::iterator iterMatrix;
	for(iterMatrix=matrix.begin();iterMatrix!=matrix.end();iterMatrix++){
		if(indexR==curRows-1 && odd){
			oddLine=true;
		GetWindowPlacement(*iterMatrix,& wndplc);
		newRect.left=deltaOdd*indexC;
		newRect.top=deltaY*indexR;
		newRect.right=(deltaOdd*indexC)+deltaOdd;
		newRect.bottom=(deltaY*indexR)+deltaY;
		wndplc.rcNormalPosition=newRect;
		wndplc.showCmd=SW_SHOWNORMAL;
		SetWindowPlacement(*iterMatrix,& wndplc);
		::SetWindowPos(*iterMatrix,NULL,newRect.left,newRect.top,newRect.right-newRect.left,newRect.bottom-newRect.top,0);		
		this->regLayPosition(*iterMatrix);
		}
		else{
		GetWindowPlacement(*iterMatrix,& wndplc);
		newRect.left=deltaX*indexC;
		newRect.top=deltaY*indexR;
		newRect.right=(deltaX*indexC)+deltaX;
		newRect.bottom=(deltaY*indexR)+deltaY;
		wndplc.rcNormalPosition=newRect;
		wndplc.showCmd=SW_SHOWNORMAL;
		SetWindowPlacement(*iterMatrix,& wndplc);
		::SetWindowPos(*iterMatrix,NULL,newRect.left,newRect.top,newRect.right-newRect.left,newRect.bottom-newRect.top,0);		
		this->regLayPosition(*iterMatrix);
		}
		if(!oddLine)
			indexC=(indexC+1)%curColumns;
		else
			indexC=(indexC+1)%(curColumns+1);
		if(indexC==0 && matrix.size()>3 && !oddLine)
			indexR++;
	}
	}
}

void GridLayout::reset(){
	this->columns=0;
	this->rows=0;
}

void GridLayout::regulateLayout(HWND moved,bool recursiveBlock){
	std::list<HWND>::iterator iterMatrix;
	RECT rect;
	RECT movedRect;
	::GetWindowRect(moved,&movedRect);
	for(iterMatrix=matrix.begin();iterMatrix!=matrix.end();iterMatrix++){
	::GetWindowRect(*iterMatrix,& rect);
	if(*iterMatrix!=moved)
		if(checkInterseption(& movedRect,& rect)){ 
			::SetWindowPos(*iterMatrix,0,rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,0);
			this->regLayPosition(*iterMatrix);
		}
	}
//tipo search space..solo che gli passo una lista di neighbors(fare funzione di individuazione vicinato)
	if(!recursiveBlock)
		this->searchSpace(moved);

}

bool GridLayout::isInLayout(HWND toCheck){
	std::list<HWND>::iterator iterMatrix;
	for(iterMatrix=matrix.begin();iterMatrix!=matrix.end();iterMatrix++)
		if(*iterMatrix==toCheck)
			return true;
		return false;
}

bool GridLayout::anchorElement(HWND hwnd){
	RECT tmp;
	if(matrix.size()<(rows*columns)){
		matrix.push_back(hwnd);
		::GetWindowRect(hwnd,& tmp);
		if(!(tmp.top<0 && tmp.right<0 && tmp.left<0 && tmp.bottom<0))
		initPositions.insertKeyElement(hwnd,&tmp);
		return true;
	}
	return false;
}

bool GridLayout::checkInterseption(RECT * moved,RECT * analyzing){
	RECT inter;

	if(::IntersectRect(& inter,moved,analyzing)){

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

		if(analyzing->bottom-analyzing->top<150 ||analyzing->right-analyzing->left<150)
			return false;	

	return true;
	}
	else
		return false;
}

void GridLayout::changedResolution(){
	//DEVMODE devmode;
	//EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devmode);
	bool refresh=false;
	if(GetSystemMetrics(SM_CYMAXIMIZED)!=height || GetSystemMetrics(SM_CXMAXIMIZED)!=width)
		refresh=true;
	height=GetSystemMetrics(SM_CYMAXIMIZED);//devmode.dmPelsHeight;
	width=GetSystemMetrics(SM_CXMAXIMIZED);//devmode.dmPelsWidth;	
	if(refresh)
	this->drawLayout();
}


void GridLayout::restorePositions(){
	std::list<HWND>::iterator iterMatrix;
	iterMatrix=matrix.begin();	
	RECT temp;
	WINDOWPLACEMENT wndplc;
	for(;iterMatrix!=matrix.end();iterMatrix++){
		if(initPositions.findElement(*iterMatrix,&temp)){
		::GetWindowPlacement(*iterMatrix,& wndplc);
		CopyRect(& wndplc.rcNormalPosition,& temp);
		wndplc.showCmd=SW_SHOWNORMAL;
		::SetWindowPlacement(*iterMatrix,& wndplc);
		SetWindowPos(*iterMatrix,0,temp.left,temp.top,temp.right-temp.left,temp.bottom-temp.top,0);
		}
	}




}

//NON FUNGE.....
void GridLayout::searchSpace(HWND moved){
	std::list<HWND>::iterator iterMatrix;
	bool odd=false;
	if(matrix.size()>curRows*curColumns)
		odd=true;
	int indexR=0;
	int indexC=0;
	int rowMov=-1;
	int columnMov=-1;
	bool oddLine=false;
	RECT tempRect;
	RECT movRect;
	RECT cuRect;
	RGNDATA rgnData;
	HRGN hrgn=CreateRectRgn(0,0,0,0);
	int regionType=::GetWindowRgn(moved,hrgn);
	if(regionType==SIMPLEREGION){
			::GetRegionData(hrgn,48,& rgnData);
			::CopyRect(& movRect,& rgnData.rdh.rcBound);
		}
		else
			::GetWindowRect(moved,&movRect);
	for(iterMatrix=matrix.begin();iterMatrix!=matrix.end();iterMatrix++){
		if(moved==*iterMatrix){
			rowMov=indexR;
			columnMov=indexC;
			if(columnMov!=0){
				iterMatrix--;
				hrgn=CreateRectRgn(0,0,0,0);
				regionType=::GetWindowRgn(*iterMatrix,hrgn);
				if(regionType==SIMPLEREGION){
					::GetRegionData(hrgn,48,& rgnData);
					::CopyRect(&tempRect,& rgnData.rdh.rcBound);
					}
				else
					::GetWindowRect(*iterMatrix,&tempRect);
				if(movRect.left>tempRect.right){
					tempRect.right=movRect.left;
					::SetWindowPos(*iterMatrix,0,tempRect.left,tempRect.top,tempRect.right-tempRect.left,tempRect.bottom-tempRect.top,0);
					regPositions.modifyElement(*iterMatrix,&tempRect);
				}
				iterMatrix++;
			}
			if((columnMov!=curColumns-1 && !oddLine)||(columnMov==curColumns && oddLine)){
				iterMatrix++;
				hrgn=CreateRectRgn(0,0,0,0);
				regionType=::GetWindowRgn(*iterMatrix,hrgn);
				if(regionType==SIMPLEREGION){
					::GetRegionData(hrgn,48,& rgnData);
					::CopyRect(&tempRect,& rgnData.rdh.rcBound);
					}
				else
					::GetWindowRect(*iterMatrix,&tempRect);
				if(movRect.right<tempRect.left){
					tempRect.left=movRect.right;
					::SetWindowPos(*iterMatrix,0,tempRect.left,tempRect.top,tempRect.right-tempRect.left,tempRect.bottom-tempRect.top,0);
					regPositions.modifyElement(*iterMatrix,&tempRect);
				}
				iterMatrix--;
			}
		}
		if(odd && indexR==curRows-1)
				oddLine=true;
		if(!oddLine)
				indexC=(indexC+1)%curColumns;
		else
			indexC=(indexC+1)%(curColumns+1);
		if(indexC==0 && matrix.size()>3 && !oddLine)
			indexR++;
		}

	//cerco spazio sopra e sotto
	indexR=0;
	indexC=0;
//	int tempCoord;
	oddLine=false;
	if(columnMov!=-1 && rowMov!=-1 && curRows>1){
		for(iterMatrix=matrix.begin();iterMatrix!=matrix.end();iterMatrix++){
			if(indexR==rowMov-1 ){//&& columnMov==indexC){
				hrgn=CreateRectRgn(0,0,0,0);
				regionType=::GetWindowRgn(*iterMatrix,hrgn);
				if(regionType==SIMPLEREGION){
					::GetRegionData(hrgn,48,& rgnData);
					::CopyRect(&tempRect,& rgnData.rdh.rcBound);
					}
				else
					::GetWindowRect(*iterMatrix,&tempRect);
				if(tempRect.bottom<movRect.top){
						tempRect.bottom=movRect.top;
						::SetWindowPos(*iterMatrix,0,tempRect.left,tempRect.top,tempRect.right-tempRect.left,tempRect.bottom-tempRect.top,0);
						this->regLayPosition(*iterMatrix);
						this->regulateLayout(*iterMatrix,true);
				}
			}
			else if(indexR==rowMov+1){// && columnMov==indexC){
				hrgn=CreateRectRgn(0,0,0,0);
				regionType=::GetWindowRgn(*iterMatrix,hrgn);
				if(regionType==SIMPLEREGION){
					::GetRegionData(hrgn,48,& rgnData);
					::CopyRect(&tempRect,& rgnData.rdh.rcBound);
					}
				else
					::GetWindowRect(*iterMatrix,&tempRect);
				if(tempRect.top>movRect.bottom){
						tempRect.top=movRect.bottom;
						::SetWindowPos(*iterMatrix,0,tempRect.left,tempRect.top,tempRect.right-tempRect.left,tempRect.bottom-tempRect.top,0);
						this->regulateLayout(*iterMatrix,true);
						this->regLayPosition(*iterMatrix);
				}				
			}
			else if(rowMov==indexR && indexC!=columnMov){
				hrgn=CreateRectRgn(0,0,0,0);
				regionType=::GetWindowRgn(*iterMatrix,hrgn);
				if(regionType==SIMPLEREGION){
					::GetRegionData(hrgn,48,& rgnData);
					::CopyRect(&tempRect,& rgnData.rdh.rcBound);
					}
				else
					::GetWindowRect(*iterMatrix,&tempRect);
				::SetWindowPos(*iterMatrix,0,tempRect.left,movRect.top,tempRect.right-tempRect.left,movRect.bottom-movRect.top,0);
				this->regulateLayout(*iterMatrix,true);	
				this->regLayPosition(*iterMatrix);
			
			}
			if(indexR==rowMov+1 && oddLine ){//&& columnMov+1==indexC){
				hrgn=CreateRectRgn(0,0,0,0);
				regionType=::GetWindowRgn(*iterMatrix,hrgn);
				if(regionType==SIMPLEREGION){
					::GetRegionData(hrgn,48,& rgnData);
					::CopyRect(&tempRect,& rgnData.rdh.rcBound);
					}
				else
					::GetWindowRect(*iterMatrix,&tempRect);
				if(tempRect.top>movRect.bottom){
						tempRect.top=movRect.bottom;
						::SetWindowPos(*iterMatrix,0,tempRect.left,tempRect.top,tempRect.right-tempRect.left,tempRect.bottom-tempRect.top,0);
						this->regulateLayout(*iterMatrix,true);
						this->regLayPosition(*iterMatrix);
				}				
				
			}
			if(indexR==rowMov-1 && odd && columnMov-1==indexC && !oddLine){
				hrgn=CreateRectRgn(0,0,0,0);
				regionType=::GetWindowRgn(*iterMatrix,hrgn);
				if(regionType==SIMPLEREGION){
					::GetRegionData(hrgn,48,& rgnData);
					::CopyRect(&tempRect,& rgnData.rdh.rcBound);
					}
				else
					::GetWindowRect(*iterMatrix,&tempRect);
				if(tempRect.bottom<movRect.top){
						tempRect.bottom=movRect.top;
						::SetWindowPos(*iterMatrix,0,tempRect.left,tempRect.top,tempRect.right-tempRect.left,tempRect.bottom-tempRect.top,0);
						this->regulateLayout(*iterMatrix,true);
						this->regLayPosition(*iterMatrix);
				}				
				
			}
			if(!oddLine)
				indexC=(indexC+1)%curColumns;	
			else
				indexC=(indexC+1)%(curColumns+1);

			if(indexC==0 && matrix.size()>3 && !oddLine)
				indexR++;
			if(odd && indexR==curRows-1)
				oddLine=true;
		}
	}
	
	indexR=0;
	indexC=0;
//	int tempCoord;
	oddLine=false;

		for(iterMatrix=matrix.begin();iterMatrix!=matrix.end();iterMatrix++){
		if(indexC!=columnMov && indexC!=0 && indexR != rowMov){
				hrgn=CreateRectRgn(0,0,0,0);
				regionType=::GetWindowRgn(*iterMatrix,hrgn);
				if(regionType==SIMPLEREGION){
					::GetRegionData(hrgn,48,& rgnData);
					::CopyRect(&cuRect,& rgnData.rdh.rcBound);
					}
				else
					::GetWindowRect(*iterMatrix,&cuRect);
				iterMatrix--;
				hrgn=CreateRectRgn(0,0,0,0);
				regionType=::GetWindowRgn(*iterMatrix,hrgn);
				if(regionType==SIMPLEREGION){
					::GetRegionData(hrgn,48,& rgnData);
					::CopyRect(&tempRect,& rgnData.rdh.rcBound);
					}
				else
					::GetWindowRect(*iterMatrix,&tempRect);
			if(cuRect.left>tempRect.right){
				tempRect.right=cuRect.left;
				::SetWindowPos(*iterMatrix,0,tempRect.left,tempRect.top,tempRect.right-tempRect.left,tempRect.bottom-tempRect.top,0);
				this->regLayPosition(*iterMatrix);
			}
			iterMatrix++;
		}
		if((indexC!=curColumns-1 && !oddLine)||(indexC==curColumns && oddLine)&& (indexC!=columnMov && indexR!=rowMov)){
				hrgn=CreateRectRgn(0,0,0,0);
				regionType=::GetWindowRgn(*iterMatrix,hrgn);
				if(regionType==SIMPLEREGION){
					::GetRegionData(hrgn,48,& rgnData);
					::CopyRect(&cuRect,& rgnData.rdh.rcBound);
					}
				else
					::GetWindowRect(*iterMatrix,&cuRect);
			iterMatrix++;
				hrgn=CreateRectRgn(0,0,0,0);
				regionType=::GetWindowRgn(*iterMatrix,hrgn);
				if(regionType==SIMPLEREGION){
					::GetRegionData(hrgn,48,& rgnData);
					::CopyRect(&tempRect,& rgnData.rdh.rcBound);
					}
				else
					::GetWindowRect(*iterMatrix,&tempRect);
				if(cuRect.right<tempRect.left){
				tempRect.left=cuRect.right;
				::SetWindowPos(*iterMatrix,0,tempRect.left,tempRect.top,tempRect.right-tempRect.left,tempRect.bottom-tempRect.top,0);
				this->regLayPosition(*iterMatrix);
				}
				iterMatrix--;
			}
		if(odd && indexR==curRows-1)
			oddLine=true;
		if(!oddLine)
			indexC=(indexC+1)%curColumns;
		else
			indexC=(indexC+1)%(curColumns+1);
		if(indexC==0 && matrix.size()>3 && !oddLine)
			indexR++;
	}

}


//IL RESTORE NN SI RICORDA SE LA FINESTRA ERA MASSIMIZZATA....



bool GridLayout::isMoved(HWND hwnd){
	RECT newPos,oldPos;
	RGNDATA rgnData;
	HRGN hrgn=CreateRectRgn(0,0,0,0);
	int regionType=::GetWindowRgn(hwnd,hrgn);
	if(regionType==SIMPLEREGION){
		::GetRegionData(hrgn,48,& rgnData);
		::CopyRect(&newPos,& rgnData.rdh.rcBound);
		}
	else
		::GetWindowRect(hwnd,& newPos);
	this->regPositions.findElement(hwnd,& oldPos);
	if(oldPos.bottom!=newPos.bottom && oldPos.left!=newPos.left && oldPos.right!=newPos.right && oldPos.top!=newPos.top)
		return true;
	return false;
}

void GridLayout::regLayPosition(){
	std::list<HWND>::iterator iterMatrix;
	iterMatrix=matrix.begin();	
	RECT temp;
	HRGN hrgn;
	int regionType;
	RGNDATA rgnData;
	for(;iterMatrix!=matrix.end();iterMatrix++){
			hrgn=CreateRectRgn(0,0,0,0);
			regionType=::GetWindowRgn(*iterMatrix,hrgn);
			if(regionType==SIMPLEREGION){
				::GetRegionData(hrgn,48,& rgnData);
				::CopyRect(&temp,& rgnData.rdh.rcBound);
			}
			else
				::GetWindowRect(*iterMatrix,&temp);
		regPositions.insertKeyElement(*iterMatrix,&temp);
	}
	
}


void GridLayout::regLayPosition(HWND added){
	RECT temp;
	HRGN hrgn=CreateRectRgn(0,0,0,0);
	int regionType=::GetWindowRgn(added,hrgn);
	RGNDATA rgnData;
	if(regionType==SIMPLEREGION){
		::GetRegionData(hrgn,48,& rgnData);
		::CopyRect(&temp,& rgnData.rdh.rcBound);
	}
	else
		::GetWindowRect(added,&temp);
	if(!this->regPositions.insertKeyElement(added,&temp))
		this->regPositions.modifyElement(added, &temp);
}	

void GridLayout::revertLayout(){
	if(matrix.size()>0){
		this->matrix.reverse();
		this->drawLayout();
	}
}
