#include "HashMap.h"


HashMap::HashMap(){


}

bool HashMap::findElement(HWND key,LPRECT element){
	
	for(int i=0;i<map.size();i++)
		if(map.at(i)==key){
			CopyRect(element,& map.at(i).rect);
			return true;
		}
		return false;
}


bool HashMap::insertKeyElement(HWND key,LPRECT element){
	WND newPair;
	if(findElement(key,& newPair.rect))
		return false;
	newPair.hwnd=key;
	CopyRect(& newPair.rect,element);
	map.push_back(newPair);
	return true;
}


bool HashMap::delKeyElement(HWND key){
	std::vector<WND>::iterator iter;
	for(iter=map.begin();iter!=map.end();iter++)
		if(*iter==key){
			map.erase(iter);
			return true;
		}
		return false;
}

bool HashMap::modifyElement(HWND key,LPRECT newValue){
	for(int i=0;i<map.size();i++)
		if(map.at(i)==key){
			CopyRect(& map.at(i).rect,newValue);
			return true;
		}
		return false;
}