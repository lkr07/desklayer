#include "PositionMap.h"


PositionMap::PositionMap(){


}

int PositionMap::findElement(HWND key){
	
	for(int i=0;i<map.size();i++)
		if(map.at(i)==key){
			return map.at(i).pos;
		}
		return -1;
}


bool PositionMap::insertKeyElement(HWND key,int element){
	WNDPOS newPair;
	if(findElement(key)!=-1)
		return false;
	newPair.hwnd=key;
	newPair.pos=element;
	map.push_back(newPair);
	return true;
}


bool PositionMap::delKeyElement(HWND key){
	std::vector<WNDPOS>::iterator iter;
	for(iter=map.begin();iter!=map.end();iter++)
		if(*iter==key){
			map.erase(iter);
			return true;
		}
	return false;
}

bool PositionMap::modifyElement(HWND key,int newValue){
	for(int i=0;i<map.size();i++)
		if(map.at(i)==key){
			map.at(i).pos=newValue;
			return true;
		}
		return false;
}

void PositionMap::clear(){
	this->map.clear();
}