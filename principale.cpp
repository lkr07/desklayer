#include <windows.h>
#include <psapi.h>
#include <shellapi.h>
#include <commctrl.h>
#include "BorderLayout.h"
#include "GridLayout.h"
#include "resource.h"
#include "msghook/msghook.h"
#include "SettingsManager.h"
#include "PositionMap.h"
#include "BorderDif.h"

#define _WIN32_IE 0x0600
#define NOLAYOUT 0
#define GRID	 2
#define BORDERDIF 1
#define WM_TRAY_MESSAGE (WM_APP+1)
#define ID_TRAY 100077
#define MENU_RESTORE 1000
#define MENU_MINIMIZE 1001
#define MENU_CLOSE 1002
#define INITBORD 0x0000
#define INITGRID 0x0001
#define RESTORE	0x0002
#define LTOC	0x0003
#define RTOC	0x0004
#define UTOC	0x0005	
#define DTOC	0x0006
#define SELLEFT	0x0007
#define SELRIGHT	0x0008
#define SELUP	0x0009
#define SELDOWN	0x000A
#define SELCENTER	0x000B

HINSTANCE hInst;
HWND prog,activeWnd=NULL;
bool layoutActive=false;
int  activeLayoutType=NOLAYOUT;
GridLayout *grid;
//RICORDARSI DI MEMORIZZARE SE LA FINESTRA ERA MASSIMIZZATA PRIMA DELL'AGGIUNTA
//vedere se si può disegnare nella dialog....
char buf[100],buf2[100];
int position=3 ;
int temp=-1;
bool collocated=false;
LPWINDOWPLACEMENT wndTmp;
int ThreadNr=1;
//DWORD WINAPI KeysProc( LPVOID pMyID );  
int index=0;
HINSTANCE hInst2;
int px;
HKEY hkey;
char progPath[MAX_PATH];
NOTIFYICONDATA nid;
HMENU h_menu;
POINT pt;
RECT temporaneo;
int len;
int count;
HWND hList;
SettingsManager setMan;
PositionMap minPos;
PositionMap borderPos;
HWND currMinimized;
BorderDif * borderDif;
double weightNy=0.2;
double weightSy=0.2;
double weightWx=0.2;
double weightEx=0.2;
double tmpN,tmpS,tmpW,tmpE;
int width,height;
std::list<HWND> showDesk;
std::list<HWND>::iterator deskIter;
int use,useTmp,temp2;
int idHot;
std::list<layWND> layTemp;
std::list<layWND>::iterator iterWnd;
int premuto;
int posTemp=-1;
char tempo[200];

BOOL CALLBACK settingProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam) {
	switch(message){
	case WM_INITDIALOG:
		width=::GetSystemMetrics(SM_CXMAXIMIZED);
		height=::GetSystemMetrics(SM_CYMAXIMIZED);
		::SetDlgItemInt(hwnd,IDC_WNY,(weightNy*height),FALSE);
		::SetDlgItemInt(hwnd,IDC_WSY,(weightSy*height),FALSE);
		::SetDlgItemInt(hwnd,IDC_WWX,(weightWx*width),FALSE);
		::SetDlgItemInt(hwnd,IDC_WEX,(weightEx*width),FALSE);
		break;
	case WM_CLOSE:
		::EndDialog(hwnd,0);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case IDOK:
		tmpN=::GetDlgItemInt(hwnd,IDC_WNY,NULL,FALSE);
		tmpS=::GetDlgItemInt(hwnd,IDC_WSY,NULL,FALSE);
		tmpW=::GetDlgItemInt(hwnd,IDC_WWX,NULL,FALSE);
		tmpE=::GetDlgItemInt(hwnd,IDC_WEX,NULL,FALSE);
		if(tmpN>120 && tmpS>120 &&tmpW>120 && tmpE>120 && (height-tmpN-tmpS)>120 && (width-tmpE-tmpW)>120){
			weightEx=tmpE/width;
			weightNy=tmpN/height;
			weightSy=tmpS/height;
			weightWx=tmpW/width;
			::EndDialog(hwnd,0);
		}
		else
			MessageBox(hwnd,"Please enter a valid value for all members!","Warning",0);
			break;
		case IDCANCEL:
			::EndDialog(hwnd,0);
			break;
		case IDC_RESET:
			weightNy=0.2;
			weightSy=0.2;
			weightWx=0.2;
			weightEx=0.2;
			break;
		default:
			break;
		}
		break;
		default:
			return false;
	}
	return true;
}
//! Funzione che setta nell'area di testo del programma il nome del layout corrente.
void setCurrentText(HWND hwnd){//da kiamare quando cambio layout..cambiare anke tooltip notifyinìcon..
	if(activeLayoutType==GRID)
		SetDlgItemText(hwnd,IDC_CURRENT,"Grid Layout");
	else if(activeLayoutType==BORDERDIF)
		SetDlgItemText(hwnd,IDC_CURRENT,"Border Layout");
	else
		SetDlgItemText(hwnd,IDC_CURRENT,"No Layout Selected");
}
/** Stabilisce in base al filtro sugli eseguibili se il nome dell'eseguibile in input è autorizzato ad essere aggiunto al layout.
@param exeName Stringa con il nome dell'eseguibile da testare.
@return True se l'eseguibile è autorizzato, false altrimenti.
*/
bool isAllowed(LPSTR exeName){
	strcat(exeName,"\0");
if(SendDlgItemMessage(prog, IDC_LIST, LB_FINDSTRINGEXACT, -1, (LPARAM)exeName)==LB_ERR)
	return true;
return false;
}

/** Trova il nome dell'eseguibile che ha generato una finestra.
@param hWnd L'handle della finestra del quale si vuole trovare l'eseguibile di origine.
@param lpszFileName Stringa di destinazione del nome dell'eseguibile.
@param dwSize Spazio che deve occupare la stringa di destinazione.
@return True se viene trovato il nome dell'eseguibile, false altrimenti.
*/
BOOL GetExeFromWindow (HWND hWnd, LPSTR lpszFileName, DWORD dwSize)
{
    DWORD   dwProcessId, dwModules, dwRet;
    HANDLE  hProcess;
    HMODULE hModule;

    if (hWnd == NULL || GetWindowThreadProcessId (hWnd, &dwProcessId) == 0)
        return FALSE;
    hProcess = OpenProcess (PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessId);

    if (hProcess == NULL) 
        return FALSE;

    if (EnumProcessModules (hProcess, &hModule, sizeof (hModule), &dwModules) == FALSE)
    {
        CloseHandle (hProcess);
        return FALSE;
    }

    dwRet = ::GetModuleBaseName(hProcess, hModule, lpszFileName, dwSize);

    CloseHandle (hProcess);

    return dwRet > 0;
}

//ALT+frecce sosto finestre o aggiungo in posizione della freccia....
/**Procedure richiamata dall'API EnumWindows per ogni finestra enumerata, nello specifico aggiunge tutte le finestre aperte al layout automaticamente.
@param hWnd Handle della finestra enumerata.
@param lParam Eventuali specifiche dell'applicazione chiamante.
@return True per continuare l'enumerazione, false altrimenti.
*/
BOOL CALLBACK EnumWindowsProc(HWND hWnd,LPARAM lParam){
	if(IsWindow(hWnd) && IsWindowVisible(hWnd) && !IsIconic(hWnd)){
		//MessageBox(prog, "Error getting selected item :(", "Warning", MB_OK);
		//bannare messenger se faccio funzione di banning programmi
		GetWindowText(hWnd,buf,sizeof(buf));
		GetExeFromWindow(hWnd,buf2,100);//funge
		GetWindowRect(hWnd,&temporaneo);
		HWND start=FindWindow("shell_traywnd",NULL);//boh
		HWND startBt=FindWindowEx(start,0,"button",NULL);
		if(strcmp(buf,"")!=0 && strcmp(buf2,"sidebar.exe")!=0)
			if(strcmp(buf,"Program Manager")!=0 && strcmp(buf,"Menu Avvio")!=0 &&strcmp(buf,"Start")!=0 &&strcmp(buf,"DeskLayer")!=0){
				//MessageBox(prog,buf,buf2,0);
				if(isAllowed(buf2) && !::IsRectEmpty(& temporaneo)){
					//		h_menu = GetSystemMenu(hWnd,false);
							//MessageBox(prog,tempo,tempo,0);
					//		if(GetMenuString(h_menu,12,tempo,200,MF_BYCOMMAND)==0 && strcmp(tempo,"PROVA")!=0){
					//			AppendMenu(h_menu,MF_SEPARATOR,0,0);
					//			AppendMenu(h_menu,MF_STRING,12,"PROVA");
					//		}
					if(activeLayoutType==BORDERDIF){
						if(hWnd==::GetForegroundWindow())
							borderDif->anchorElement(hWnd,4);
						else{
							borderDif->anchorElement(hWnd,position);
							position=(position+1)%4;
						}
					}
					else if(activeLayoutType==GRID){
						grid->anchorElement(hWnd);
					}
				}
		}
	}


return true;}





/** Procedura per il processamento dei messaggi rivolti alla dialog box.
@param hwnd Handle della dialog box.
@param message Specifica del messaggio.
@param wParam Specifica le informazioni addizionali del messaggio.
@param lParam Specifica le informazioni addizionali del messaggio.
@return True se il messaggio è stato processato dalla procedura, false altrimenti.
*/
BOOL CALLBACK DlgProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam) {
	prog=hwnd;
	if(activeLayoutType!=NOLAYOUT){
		//if(message==UWM_SYSMENU)
		//	MessageBox(prog,"MENUCLIKKATO","ATTENZIONE",0);
		if( message==UWM_SIZING){
			if(activeLayoutType==BORDERDIF){
				if(borderDif->isInLayout((HWND)wParam)!=-1){
					if(!borderDif->regulateLayout((HWND)wParam))
						minPos.delKeyElement((HWND)wParam);
					borderDif->refreshLayout();
				}
			}
			else if(activeLayoutType==GRID){
				if(grid->isInLayout((HWND)wParam)){
						if(grid->isMoved((HWND)wParam)){
							grid->removeElement((HWND)wParam,true);
							::SetActiveWindow((HWND)wParam);
						}
						else{
							grid->regLayPosition((HWND)wParam);
							grid->regulateLayout((HWND)wParam,false);	
						}
				}
			}
		return true;
		}
		else if(message==UWM_CLOSE){
			if(activeLayoutType==BORDERDIF){
					if((temp=borderDif->isInLayout((HWND)wParam))>  -1){
						borderDif->removeElement((HWND)wParam,temp,true,false);
					}
						if(minPos.findElement((HWND)wParam)!=-1)
							minPos.delKeyElement((HWND)wParam);
			}
			else if(activeLayoutType==GRID){
					if(grid->isInLayout((HWND)wParam))
						grid->removeElement((HWND)wParam,true);
			}
		//rimozione finestra da lay out
		return true; 
		}	else if(message==UWM_MINMAX){
			 if(activeLayoutType==GRID){
				if(LOWORD(lParam)==SW_MINIMIZE){
					if(grid->isInLayout((HWND)wParam)){
						grid->removeElement((HWND)wParam,false);
						grid->drawLayout();
					}
				}
				else if(LOWORD(lParam)==SW_RESTORE){
					//MessageBox(hwnd,"MAXXX","fass",0);
					if(!grid->isInLayout((HWND)wParam)){
					grid->anchorElement((HWND)wParam);
					grid->drawLayout();
					grid->regLayPosition((HWND)wParam);
					}
				}
				else if(LOWORD(lParam)==SW_MAXIMIZE){
					if(grid->isInLayout((HWND)wParam)){
						grid->removeElement((HWND)wParam,false);
						grid->drawLayout();
					}
				}
			}
			else if(activeLayoutType==BORDERDIF){
				if(LOWORD(lParam)==SW_MINIMIZE){
					//MessageBox(hwnd,"MINNNN","fass",0);
					if((temp=borderDif->isInLayout((HWND)wParam))>-1){
						if(!minPos.insertKeyElement((HWND)wParam,temp))
							minPos.modifyElement((HWND)wParam,temp);
						if(temp!=4)
						if(!borderPos.insertKeyElement((HWND)wParam,borderDif->getListPosition((HWND)wParam,temp)))
							borderPos.modifyElement((HWND)wParam,borderDif->getListPosition((HWND)wParam,temp));
						borderDif->removeElement((HWND)wParam,temp,false,true);
					}
				}
				else if(LOWORD(lParam)==SW_RESTORE){//risolvere il fatto del restore su cosi che erano minimizzati...
					if(borderDif->isInLayout((HWND)wParam)==-1){
						if((temp=minPos.findElement((HWND)wParam))!=-1){
							if(temp!=4 &&(px=borderPos.findElement((HWND)wParam))!=-1)
								borderDif->anchorElement((HWND)wParam,temp,px);
							else if(temp==4)
								borderDif->pushToCenter((HWND)wParam);
							else
								borderDif->anchorElement((HWND)wParam,temp);
//							else
//							border->anchorElement((HWND)wParam,position);
				//	position=(position+1)%4;
						}
					}
				/*else if((temp=borderDif->isInLayout((HWND)wParam))>-1){
						use=borderDif->getListPosition((HWND)wParam,temp);
						showDesk=borderDif->minimizeAll((HWND)wParam);
						for(deskIter=showDesk.begin();deskIter!=showDesk.end();deskIter++){
							if(*deskIter!=(HWND)wParam){
								temp2=borderDif->isInLayout(*deskIter);
								useTmp=borderDif->getListPosition(*deskIter,temp2);
								if(!minPos.insertKeyElement(*deskIter,temp2))
									minPos.modifyElement(*deskIter,temp2);
								if(temp!=4)
									if(!borderPos.insertKeyElement(*deskIter,useTmp))
										borderPos.modifyElement(*deskIter,useTmp);
							}
							borderDif->removeElement(*deskIter,temp2,false,true,false);
							
						}
						//aggiungere a minPos,a borderPos e rimuovere....
						borderDif->anchorElement((HWND)wParam,temp,use);
					}*/
				}
				else if(LOWORD(lParam)==SW_MAXIMIZE){
					if((temp=borderDif->isInLayout((HWND)wParam))>-1){
						if(!minPos.insertKeyElement((HWND)wParam,temp))
							minPos.modifyElement((HWND)wParam,temp);
						if(temp!=4)
						if(!borderPos.insertKeyElement((HWND)wParam,borderDif->getListPosition((HWND)wParam,temp)))
							borderPos.modifyElement((HWND)wParam,borderDif->getListPosition((HWND)wParam,temp));
						borderDif->removeElement((HWND)wParam,temp,false,true);
					
					}
				}
			}
		return true;
		}
	} 

	switch(message){
		case WM_INITDIALOG:
			hInst=::GetModuleHandle(NULL);
			::GetModuleFileName(hInst,progPath,MAX_PATH);
			RegOpenKey(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\Windows\\CurrentVersion\\Run",&hkey);
			RegSetValueEx(hkey,"DeskLayer",0,(DWORD)REG_SZ,(LPBYTE)progPath,(lstrlen(progPath)+1)*sizeof(TCHAR));	
			//inserire caricamento opzioni salvate (classe FileManager?per caricare e salvare..boh)
			setMyHook(hwnd);
			//	Thread di ascolto che si aggania ai tasti specificati
			//CreateThread( NULL,0,KeysProc,&ThreadNr,0,NULL );
			RegisterHotKey(hwnd,INITBORD,MOD_CONTROL | MOD_ALT,'B');//61
			RegisterHotKey(hwnd,INITGRID,MOD_CONTROL | MOD_ALT,'G');//66
			RegisterHotKey(hwnd,RESTORE,MOD_CONTROL,VK_BACK);
			RegisterHotKey(hwnd,LTOC,MOD_CONTROL,VK_LEFT);
			RegisterHotKey(hwnd,RTOC,MOD_CONTROL,VK_RIGHT);
			RegisterHotKey(hwnd,UTOC,MOD_CONTROL,VK_UP);
			RegisterHotKey(hwnd,DTOC,MOD_CONTROL,VK_DOWN);
			RegisterHotKey(hwnd,SELLEFT,MOD_ALT,VK_LEFT);
			RegisterHotKey(hwnd,SELRIGHT,MOD_ALT,VK_RIGHT);
			RegisterHotKey(hwnd,SELUP,MOD_ALT,VK_UP);
			RegisterHotKey(hwnd,SELDOWN,MOD_ALT,VK_DOWN);
			RegisterHotKey(hwnd,SELUP,MOD_ALT,VK_UP);
			RegisterHotKey(hwnd,SELCENTER,MOD_ALT,VK_F10);
			nid.cbSize=sizeof(NOTIFYICONDATA);
            nid.hWnd=hwnd;
			nid.uID=ID_TRAY;
			nid.uFlags=NIF_ICON | NIF_MESSAGE | NIF_TIP;
			strcpy(nid.szTip,"DeskLayer");
			nid.uCallbackMessage=WM_TRAY_MESSAGE;
			nid.hIcon = LoadIcon(hInst,MAKEINTRESOURCE(IDI_ICON));
			Shell_NotifyIcon(NIM_ADD,&nid);
			setMan.loadOptions(hwnd);
			setCurrentText(hwnd);
			break;
		case WM_CLOSE:
			//salvataggio impostazioni salvate
			ShowWindow(hwnd,SW_HIDE);
			::UpdateWindow(hwnd);
			break;
		  case WM_TRAY_MESSAGE:
                switch (lParam) {
			         case WM_LBUTTONDBLCLK:
                           ShowWindow(hwnd,SW_SHOW);
						   setCurrentText(hwnd);
                     break;
					 case WM_RBUTTONDOWN:
                            h_menu=CreatePopupMenu();
                            AppendMenu(h_menu,MF_STRING,MENU_RESTORE,"Settings");
                            AppendMenu(h_menu,MF_STRING,MENU_MINIMIZE,"Minimize");
                            AppendMenu(h_menu,MF_SEPARATOR,0,NULL);
                            AppendMenu(h_menu,MF_STRING,MENU_CLOSE,"Close");
                            GetCursorPos(&pt);
                            SetForegroundWindow(hwnd);    // sparisce il menu se clicco fuori da esso
                            TrackPopupMenu(h_menu,TPM_RIGHTALIGN,pt.x,pt.y,0,hwnd,NULL);//inserimento menu
							//DestroyMenu(h_menu);
							break;
                       }
               break;
case WM_HOTKEY:
			idHot=(int)wParam;
	switch(idHot){
		case INITBORD:
			if(activeLayoutType==NOLAYOUT){
				activeLayoutType=BORDERDIF;
				borderDif=new BorderDif(weightEx,weightWx,weightNy,weightSy);
				if(SendDlgItemMessage(prog,IDC_AUTO,BM_GETSTATE,0,0)==BST_CHECKED){
					EnumWindows(EnumWindowsProc,0);
				//	borderDif->refreshLayout();
				//borderDif->initializeLayout();//->refreshLayout();
				//borderDif->regLayPosition();
			}
			setCurrentText(prog);
		}
			break;
		case INITGRID:
			if(activeLayoutType==NOLAYOUT){
				activeLayoutType=GRID;
	 			grid=new GridLayout();
				if(SendDlgItemMessage(prog,IDC_AUTO,BM_GETSTATE,0,0)==BST_CHECKED)
					EnumWindows(EnumWindowsProc,0);
				if(!grid->isNull()){
					grid->drawLayout();
					grid->regLayPosition();
			}
			setCurrentText(prog);
			}
			break;
		case RESTORE:
			if(activeLayoutType==GRID){
				minPos.clear();
				borderPos.clear();
				grid->restorePositions();
				activeLayoutType=NOLAYOUT;
				grid=NULL;
			}
			else if(activeLayoutType==BORDERDIF){
				borderDif->restorePositions();
				activeLayoutType=NOLAYOUT;
				borderDif=NULL;
				minPos.clear();
				borderPos.clear();
			}
			setCurrentText(prog);
			break;
		case LTOC:
			premuto=1;
			if(activeLayoutType==BORDERDIF){
				layTemp=borderDif->getWindowsInPos(premuto);
				if(!layTemp.empty()){
				index=index%layTemp.size();
				iterWnd=layTemp.begin();
				for(int i=0;i!=index && iterWnd!=layTemp.end();i++)
	 				iterWnd++;  
				borderDif->toCenter(*iterWnd,premuto);
				index=(index+1)%layTemp.size();
				}			
			}
			break;
		case RTOC:
			premuto=2;
			if(activeLayoutType==BORDERDIF){
				layTemp=borderDif->getWindowsInPos(premuto);
				if(!layTemp.empty()){
				index=index%layTemp.size();
				iterWnd=layTemp.begin();
				for(int i=0;i!=index && iterWnd!=layTemp.end();i++)
	 				iterWnd++;  
				borderDif->toCenter(*iterWnd,premuto);
				index=(index+1)%layTemp.size();
				}			
			}
			break;
		case UTOC:
			premuto=0;
			if(activeLayoutType==BORDERDIF){
				layTemp=borderDif->getWindowsInPos(premuto);
				if(!layTemp.empty()){
				index=index%layTemp.size();
				iterWnd=layTemp.begin();
				for(int i=0;i!=index && iterWnd!=layTemp.end();i++)
	 				iterWnd++;  
				borderDif->toCenter(*iterWnd,premuto);
				index=(index+1)%layTemp.size();
				}			
			}
			break;
		case DTOC:
			premuto=3;
			if(activeLayoutType==BORDERDIF){
				layTemp=borderDif->getWindowsInPos(premuto);
				if(!layTemp.empty()){
				index=index%layTemp.size();
				iterWnd=layTemp.begin();
				for(int i=0;i!=index && iterWnd!=layTemp.end();i++)
	 				iterWnd++;  
				borderDif->toCenter(*iterWnd,premuto);
				index=(index+1)%layTemp.size();
				}			
			}
			break;
		case SELLEFT:
			premuto=1;
		 if(activeLayoutType==BORDERDIF){
			activeWnd=::GetForegroundWindow();
			GetWindowText(activeWnd,buf,200);
			GetWindowRect(activeWnd,&temporaneo);
			if(IsWindow(activeWnd) && IsWindowVisible(activeWnd))
				if(strcmp(buf,"Menu Avvio")!=0 && strcmp(buf,"")!=0 && strcmp("Program Manager",buf)!=0 && strcmp(buf,"DeskLayer")!=0 && !::IsRectEmpty(& temporaneo))
					if(borderDif->isInLayout(activeWnd)==-1){
						borderDif->anchorElement(activeWnd,premuto);
						//forse reglaypositions
					}
					else if((posTemp=borderDif->isInLayout(activeWnd))>-1){
						borderDif->moveElement(activeWnd,posTemp,premuto);
					}
		}
		else if(activeLayoutType==GRID){
			activeWnd=::GetForegroundWindow();
			GetWindowText(activeWnd,buf,200);
			GetWindowRect(activeWnd,&temporaneo);
			//attenzione a start...
			if(IsWindow(activeWnd) && IsWindowVisible(activeWnd))
				if(strcmp(buf,"Menu Avvio")!=0,strcmp(buf,"")!=0 && strcmp("Program Manager",buf)!=0  && strcmp("start",buf)!=0 && strcmp(buf,"DeskLayer")!=0 && !::IsRectEmpty(& temporaneo))
					if(!grid->isInLayout(activeWnd)){
						grid->anchorElement(activeWnd);
						grid->drawLayout();
						grid->regLayPosition(activeWnd);
					}
		}
			break;
		case SELRIGHT:
			premuto=2;
		 if(activeLayoutType==BORDERDIF){
			activeWnd=::GetForegroundWindow();
			GetWindowText(activeWnd,buf,200);
			GetWindowRect(activeWnd,&temporaneo);
			if(IsWindow(activeWnd) && IsWindowVisible(activeWnd))
				if(strcmp(buf,"Menu Avvio")!=0 && strcmp(buf,"")!=0 && strcmp("Program Manager",buf)!=0 && strcmp(buf,"DeskLayer")!=0 && !::IsRectEmpty(& temporaneo))
					if(borderDif->isInLayout(activeWnd)==-1){
						borderDif->anchorElement(activeWnd,premuto);
						//forse reglaypositions
					}
					else if((posTemp=borderDif->isInLayout(activeWnd))>-1){
						borderDif->moveElement(activeWnd,posTemp,premuto);
					}
		}
		else if(activeLayoutType==GRID){
			activeWnd=::GetForegroundWindow();
			GetWindowText(activeWnd,buf,200);
			GetWindowRect(activeWnd,&temporaneo);
			//attenzione a start...
			if(IsWindow(activeWnd) && IsWindowVisible(activeWnd))
				if(strcmp(buf,"Menu Avvio")!=0,strcmp(buf,"")!=0 && strcmp("Program Manager",buf)!=0  && strcmp("start",buf)!=0 && strcmp(buf,"DeskLayer")!=0 && !::IsRectEmpty(& temporaneo))
					if(!grid->isInLayout(activeWnd)){
						grid->anchorElement(activeWnd);
						grid->drawLayout();
						grid->regLayPosition(activeWnd);
					}
		}
			break;
		case SELUP:
						premuto=0;
		 if(activeLayoutType==BORDERDIF){
			activeWnd=::GetForegroundWindow();
			GetWindowText(activeWnd,buf,200);
			GetWindowRect(activeWnd,&temporaneo);
			if(IsWindow(activeWnd) && IsWindowVisible(activeWnd))
				if(strcmp(buf,"Menu Avvio")!=0 && strcmp(buf,"")!=0 && strcmp("Program Manager",buf)!=0 && strcmp(buf,"DeskLayer")!=0 && !::IsRectEmpty(& temporaneo))
					if(borderDif->isInLayout(activeWnd)==-1){
						borderDif->anchorElement(activeWnd,premuto);
						//forse reglaypositions
					}
					else if((posTemp=borderDif->isInLayout(activeWnd))>-1){
						borderDif->moveElement(activeWnd,posTemp,premuto);
					}
		}
		else if(activeLayoutType==GRID){
			activeWnd=::GetForegroundWindow();
			GetWindowText(activeWnd,buf,200);
			GetWindowRect(activeWnd,&temporaneo);
			//attenzione a start...
			if(IsWindow(activeWnd) && IsWindowVisible(activeWnd))
				if(strcmp(buf,"Menu Avvio")!=0,strcmp(buf,"")!=0 && strcmp("Program Manager",buf)!=0  && strcmp("start",buf)!=0 && strcmp(buf,"DeskLayer")!=0 && !::IsRectEmpty(& temporaneo))
					if(!grid->isInLayout(activeWnd)){
						grid->anchorElement(activeWnd);
						grid->drawLayout();
						grid->regLayPosition(activeWnd);
					}
		}
			break;
		case SELDOWN:
						premuto=3;
		 if(activeLayoutType==BORDERDIF){
			activeWnd=::GetForegroundWindow();
			GetWindowText(activeWnd,buf,200);
			GetWindowRect(activeWnd,&temporaneo);
			if(IsWindow(activeWnd) && IsWindowVisible(activeWnd))
				if(strcmp(buf,"Menu Avvio")!=0 && strcmp(buf,"")!=0 && strcmp("Program Manager",buf)!=0 && strcmp(buf,"DeskLayer")!=0 && !::IsRectEmpty(& temporaneo))
					if(borderDif->isInLayout(activeWnd)==-1){
						borderDif->anchorElement(activeWnd,premuto);
						//forse reglaypositions
					}
					else if((posTemp=borderDif->isInLayout(activeWnd))>-1){
						borderDif->moveElement(activeWnd,posTemp,premuto);
					}
		}
		else if(activeLayoutType==GRID){
			activeWnd=::GetForegroundWindow();
			GetWindowText(activeWnd,buf,200);
			GetWindowRect(activeWnd,&temporaneo);
			//attenzione a start...
			if(IsWindow(activeWnd) && IsWindowVisible(activeWnd))
				if(strcmp(buf,"Menu Avvio")!=0,strcmp(buf,"")!=0 && strcmp("Program Manager",buf)!=0  && strcmp("start",buf)!=0 && strcmp(buf,"DeskLayer")!=0 && !::IsRectEmpty(& temporaneo))
					if(!grid->isInLayout(activeWnd)){
						grid->anchorElement(activeWnd);
						grid->drawLayout();
						grid->regLayPosition(activeWnd);
					}
		}
			break;
		case SELCENTER:
			premuto=4;
			if(activeLayoutType==BORDERDIF){
			activeWnd=::GetForegroundWindow();
			GetWindowText(activeWnd,buf,200);
			GetWindowRect(activeWnd,&temporaneo);
			if(IsWindow(activeWnd) && IsWindowVisible(activeWnd))
				if(strcmp(buf,"Menu Avvio")!=0,strcmp(buf,"")!=0 && strcmp("Program Manager",buf)!=0 && strcmp("start",buf)!=0  &&!::IsRectEmpty(& temporaneo))			
					if((posTemp=borderDif->isInLayout(activeWnd))>-1){
						layWND mom;
						mom.hwnd=activeWnd;
						borderDif->toCenter(mom,posTemp);
					}
					else if((posTemp=borderDif->isInLayout(activeWnd))==-1){
						borderDif->pushToCenter(activeWnd);//vedere se va bene così
					}
			}
			break;
				default:
					break;
				}
			break;
		case WM_COMMAND:
			switch(LOWORD(wParam)){//analizzo controlli(caricare impostazioni default) inserire opzione avvio all'avvio del pc
			case IDC_BORDIF:
				if(activeLayoutType==NOLAYOUT){
					activeLayoutType=BORDERDIF;
					borderDif=new BorderDif(weightEx,weightWx,weightNy,weightSy);
					if(SendDlgItemMessage(prog,IDC_AUTO,BM_GETSTATE,0,0)==BST_CHECKED){
						EnumWindows(EnumWindowsProc,0);
					//	borderDif->refreshLayout();
						//borderDif->initializeLayout();//->refreshLayout();
					//borderDif->regLayPosition();
					}
					setCurrentText(prog);
				}
				break;
			case IDC_GRID:
				if(activeLayoutType==NOLAYOUT){
					activeLayoutType=GRID;
					grid=new GridLayout();
					if(SendDlgItemMessage(hwnd,IDC_AUTO,BM_GETSTATE,0,0)==BST_CHECKED)
						EnumWindows(EnumWindowsProc,0);
					grid->drawLayout();
					grid->regLayPosition();
					setCurrentText(hwnd);
				}
				break;
			case IDC_NOLAY:
				minPos.clear();
				borderPos.clear();
				grid=NULL;
				activeLayoutType=NOLAYOUT;
				borderDif=NULL;
				setCurrentText(hwnd);
				break;
			case IDOK:
				ShowWindow(prog,SW_HIDE);
				break;
			case IDC_STARTUP:
				if(SendDlgItemMessage(hwnd,IDC_STARTUP,BM_GETSTATE,0,0)==BST_CHECKED){
				RegOpenKey(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\Windows\\CurrentVersion\\Run",&hkey);
				RegSetValueEx(hkey,"DeskLayer",0,(DWORD)REG_SZ,(LPBYTE)"",sizeof(TCHAR));
				}
				else if(SendDlgItemMessage(hwnd,IDC_STARTUP,BM_GETSTATE,0,0)==BST_UNCHECKED){
					RegOpenKey(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\Windows\\CurrentVersion\\Run",&hkey);
					RegSetValueEx(hkey,"DeskLayer",0,(DWORD)REG_SZ,(LPBYTE)progPath,(lstrlen(progPath)+1)*sizeof(TCHAR));	
				}
				break;
			case IDCLOSE :
				clearMyHook(hwnd);
				Shell_NotifyIcon(NIM_DELETE,&nid);
			//	if(activeLayoutType==BORDER)
			//		border->restorePositions();
			//	else if(activeLayoutType==GRID)
			//		grid->restorePositions();
				setMan.saveOptions(hwnd);
			UnregisterHotKey(hwnd,INITBORD);//61
			UnregisterHotKey(hwnd,INITGRID);//66
			UnregisterHotKey(hwnd,RESTORE);
			UnregisterHotKey(hwnd,LTOC);
			UnregisterHotKey(hwnd,RTOC);
			UnregisterHotKey(hwnd,UTOC);
			UnregisterHotKey(hwnd,DTOC);
			UnregisterHotKey(hwnd,SELLEFT);
			UnregisterHotKey(hwnd,SELRIGHT);
			UnregisterHotKey(hwnd,SELUP);
			UnregisterHotKey(hwnd,SELDOWN);
			UnregisterHotKey(hwnd,SELUP);
			UnregisterHotKey(hwnd,SELCENTER);
				//::TerminateThread(KeysProc,0);
				::EndDialog(hwnd,0);
				::PostQuitMessage(0);
				break;
			case MENU_RESTORE:
				ShowWindow(hwnd,SW_SHOW);
				setCurrentText(hwnd);
				::UpdateWindow(hwnd);
                break;
            case MENU_MINIMIZE:
                ShowWindow(hwnd,SW_HIDE);
				::UpdateWindow(hwnd);
                break;
            case MENU_CLOSE:
				clearMyHook(hwnd);
				Shell_NotifyIcon(NIM_DELETE,&nid);
				//if(activeLayoutType==BORDER)
				//	border->restorePositions();
				//else if(activeLayoutType==GRID)
				//	grid->restorePositions();
				setMan.saveOptions(hwnd);//WM_DRAWITEM...
			UnregisterHotKey(hwnd,INITBORD);//61
			UnregisterHotKey(hwnd,INITGRID);//66
			UnregisterHotKey(hwnd,RESTORE);
			UnregisterHotKey(hwnd,LTOC);
			UnregisterHotKey(hwnd,RTOC);
			UnregisterHotKey(hwnd,UTOC);
			UnregisterHotKey(hwnd,DTOC);
			UnregisterHotKey(hwnd,SELLEFT);
			UnregisterHotKey(hwnd,SELRIGHT);
			UnregisterHotKey(hwnd,SELUP);
			UnregisterHotKey(hwnd,SELDOWN);
			UnregisterHotKey(hwnd,SELUP);
			UnregisterHotKey(hwnd,SELCENTER);
//				::TerminateThread(KeysProc,0);
				::EndDialog(hwnd,0);
				::PostQuitMessage(0);				
				break;
			case IDC_ADD:
					len = GetWindowTextLength(GetDlgItem(hwnd, IDC_TOADD));
						if(len > 0)
						{
							char* aggiungere,*sost;
							aggiungere=new char[len+1];						
							GetDlgItemText(hwnd, IDC_TOADD, aggiungere, len + 1);
							sost=new char[len+1];
							strcpy(sost,aggiungere);
							strtok(sost,".");
							char* exe=strtok(NULL,".");
							if(exe==NULL || strcmp(exe,"exe")!=0)
								MessageBox(hwnd,"Insert a valid exe name please","Warning",MB_OK);
							else {
								SendDlgItemMessage(hwnd, IDC_LIST, LB_ADDSTRING, 0, (LPARAM)aggiungere);
								SetDlgItemText(hwnd,IDC_TOADD,"");
							}
							delete[]sost;
							delete[]aggiungere;
						
						}
						else 
						{
							MessageBox(hwnd, "You didn't enter anything!", "Warning", MB_OK);
						}
			break;
			case IDC_REMOVE:
					hList = GetDlgItem(hwnd, IDC_LIST);
					count = SendMessage(hList, LB_GETCURSEL, 0, 0);
					if(count != LB_ERR)
						SendMessage(hList, LB_DELETESTRING, (WPARAM)count, 0);
						else 
							MessageBox(hwnd, "No items selected.", "Warning", MB_OK);
						break;
			case IDC_SET:
				DialogBox(hInst2,MAKEINTRESOURCE(IDD_DIALOG2),hwnd,settingProc);
				break;
			default:
				break;
			}
		case WM_DISPLAYCHANGE:
			//MessageBox(hwnd,"dasdsa","saffsadds",0);       
			if(activeLayoutType==BORDERDIF)
				borderDif->refreshScreenSize();
			else if(activeLayoutType==GRID)
				grid->changedResolution();    
			//catturo di nuovo risoluzione e la passo a border e poi refresho...
			break;
		default:
			return false;
		}
return true;
}

/**Procedura che definisce le azioni di un thread, nello specifico un thread per la ricezione di inpur system-wide da tastiera.
@param pMyID Dati per il thread creato.
@return Il codice di uscita del thread.
*/
/*
DWORD WINAPI KeysProc( LPVOID pMyID ){
//char pressed[255];
 //long scancode;
//short keyCode;
	std::list<layWND> layTemp;
	std::list<layWND>::iterator iterWnd;
	int premuto;
	int posTemp=-1;
	while(true){
	premuto=-1;
	

	if(GetKeyState(VK_CONTROL)<0){
	//	Sleep(50);
	if (GetKeyState(VK_LEFT)<0)
			premuto=1;
	else if(GetKeyState(VK_RIGHT)<0)
			premuto=2;
	else if(GetKeyState(VK_UP) <0)
		premuto=0;
	else if(GetKeyState(VK_DOWN)<0)
		premuto=3;
	else if(GetKeyState(VK_BACK)<0){
		if(activeLayoutType==GRID){
			minPos.clear();
			borderPos.clear();
			grid->restorePositions();
			activeLayoutType=NOLAYOUT;
			grid=NULL;
		}
		else if(activeLayoutType==BORDERDIF){
			borderDif->restorePositions();
			activeLayoutType=NOLAYOUT;
			borderDif=NULL;
			minPos.clear();
			borderPos.clear();
		}
		setCurrentText(prog);
	}
	else if(GetKeyState(66)<0 && GetKeyState(VK_MENU)<0){
		if(activeLayoutType==NOLAYOUT){
			activeLayoutType=BORDERDIF;
			borderDif=new BorderDif(weightEx,weightWx,weightNy,weightSy);
			if(SendDlgItemMessage(prog,IDC_AUTO,BM_GETSTATE,0,0)==BST_CHECKED){
				EnumWindows(EnumWindowsProc,0);
			//	borderDif->refreshLayout();
				//borderDif->initializeLayout();//->refreshLayout();
				//borderDif->regLayPosition();
			}
			setCurrentText(prog);
		}
	}
	else if(GetKeyState(71)<0 && GetKeyState(VK_MENU)<0){
		if(activeLayoutType==NOLAYOUT){
			activeLayoutType=GRID;
	 		grid=new GridLayout();
			if(SendDlgItemMessage(prog,IDC_AUTO,BM_GETSTATE,0,0)==BST_CHECKED)
				EnumWindows(EnumWindowsProc,0);
			if(!grid->isNull()){
			grid->drawLayout();
			grid->regLayPosition();
			}
			setCurrentText(prog);
		}
	}
	if(activeLayoutType==BORDERDIF && premuto>=0 && premuto<=3){
			layTemp=borderDif->getWindowsInPos(premuto);
			if(!layTemp.empty()){
				index=index%layTemp.size();
				iterWnd=layTemp.begin();
				for(int i=0;i!=index && iterWnd!=layTemp.end();i++)
	 				iterWnd++;  
				borderDif->toCenter(*iterWnd,premuto);
				index=(index+1)%layTemp.size();
				}
			premuto=0;			
		}
		if(GetKeyState(VK_SHIFT)<0)
		 if(activeLayoutType==GRID){
				grid->revertLayout();
			}
	}
	else if(GetKeyState(VK_MENU)<0){
	//	Sleep(50);
		if (GetKeyState(VK_LEFT)<0)
				premuto=1;
		else if(GetKeyState(VK_RIGHT)<0)
				premuto=2;
		else if(GetKeyState(VK_UP) <0)
				premuto=0;
		else if(GetKeyState(VK_DOWN)<0)
				premuto=3;
		else if(GetKeyState(VK_F10)<0)
	 			premuto=4;
		 if(activeLayoutType==BORDERDIF && premuto>=0 && premuto<=3){
			activeWnd=::GetForegroundWindow();
			GetWindowText(activeWnd,buf,200);
			GetWindowRect(activeWnd,&temporaneo);
			if(IsWindow(activeWnd) && IsWindowVisible(activeWnd))
				if(strcmp(buf,"Menu Avvio")!=0 && strcmp(buf,"")!=0 && strcmp("Program Manager",buf)!=0 && strcmp(buf,"DeskLayer")!=0 && !::IsRectEmpty(& temporaneo))
					if(borderDif->isInLayout(activeWnd)==-1){
						borderDif->anchorElement(activeWnd,premuto);
						//forse reglaypositions
					}
					else if((posTemp=borderDif->isInLayout(activeWnd))>-1){
						borderDif->moveElement(activeWnd,posTemp,premuto);
					}
		}
		else if(activeLayoutType==GRID && premuto>=0 && premuto<=3){
			activeWnd=::GetForegroundWindow();
			GetWindowText(activeWnd,buf,200);
			GetWindowRect(activeWnd,&temporaneo);
			//attenzione a start...
			if(IsWindow(activeWnd) && IsWindowVisible(activeWnd))
				if(strcmp(buf,"Menu Avvio")!=0,strcmp(buf,"")!=0 && strcmp("Program Manager",buf)!=0  && strcmp("start",buf)!=0 && strcmp(buf,"DeskLayer")!=0 && !::IsRectEmpty(& temporaneo))
					if(!grid->isInLayout(activeWnd)){
						grid->anchorElement(activeWnd);
						grid->drawLayout();
						grid->regLayPosition(activeWnd);
					}
		}
		//caso centrale
		else if(premuto==4 && activeLayoutType==BORDERDIF){
			activeWnd=::GetForegroundWindow();
			GetWindowText(activeWnd,buf,200);
			GetWindowRect(activeWnd,&temporaneo);
			if(IsWindow(activeWnd) && IsWindowVisible(activeWnd))
				if(strcmp(buf,"Menu Avvio")!=0,strcmp(buf,"")!=0 && strcmp("Program Manager",buf)!=0 && strcmp("start",buf)!=0  &&!::IsRectEmpty(& temporaneo))			
					if((posTemp=borderDif->isInLayout(activeWnd))>-1){
						layWND mom;
						mom.hwnd=activeWnd;
						borderDif->toCenter(mom,posTemp);
					}
					else if((posTemp=borderDif->isInLayout(activeWnd))==-1){
						borderDif->pushToCenter(activeWnd);//vedere se va bene così
					}

		}
	}

Sleep(100);
}


return 0;

}

*/

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine, int nCmdShow){
	MSG Msg;
	hInst2=hInstance;
	prog=CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
	///::ShowWindow(prog,SW_SHOW);
	::UpdateWindow(prog);
	while(GetMessage(&Msg, NULL, 0, 0) > 0)
	{	
		if(!IsDialogMessage(prog, &Msg))
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
	}
	return Msg.wParam;

}

//disabilitare ricezione tasti..
//fare pulsante e tasto per disabilitare senza restore...