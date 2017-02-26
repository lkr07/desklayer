#include "SettingsManager.h"

SettingsManager::SettingsManager(){
	
}

void SettingsManager::saveOptions(HWND hwnd){
	remove("settings.dat");
	std::ofstream myfile("settings.dat");
	char buffer[50];
	int i=0;
	if(myfile.is_open()){
		while(SendDlgItemMessage(hwnd, IDC_LIST, LB_GETTEXT, i, (LPARAM)buffer)!=LB_ERR){
			myfile<<buffer<<std::endl;
			i++;
		}
	if(SendDlgItemMessage(hwnd,IDC_AUTO,BM_GETSTATE,0,0)==BST_CHECKED)
		myfile<<"auto"<<std::endl;
	if(SendDlgItemMessage(hwnd,IDC_STARTUP,BM_GETSTATE,0,0)==BST_CHECKED){
		myfile<<"nostart"<<std::endl;
	}
	myfile.close();
	}
}

void SettingsManager::loadOptions(HWND hwnd){
char bannedExe[50]="";
double a;
std::ifstream myfile("settings.dat");
if(myfile.is_open()){
	while(!myfile.eof()){
		myfile>>bannedExe;
		if(strcmp(bannedExe,"")!=0 && strcmp(bannedExe,"auto")!=0 && strcmp(bannedExe,"nostart")!=0)
		SendDlgItemMessage(hwnd, IDC_LIST, LB_ADDSTRING, 0, (LPARAM)bannedExe);
		else if(strcmp("auto",bannedExe)==0)
			SendDlgItemMessage(hwnd,IDC_AUTO,BM_SETCHECK,BST_CHECKED,0);
		else if(strcmp("nostart",bannedExe)==0){
			SendDlgItemMessage(hwnd,IDC_STARTUP,BM_SETCHECK,BST_CHECKED,0);
			HKEY hkey;
			RegOpenKey(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\Windows\\CurrentVersion\\Run",&hkey);
			RegSetValueEx(hkey,"DeskLayer",0,(DWORD)REG_SZ,(LPBYTE)"",sizeof(TCHAR));		
		
		}
	}
}

}