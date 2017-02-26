// msghook.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
//#define _COMPILING_44E531B1_14D3_11d5_A025_006067718D04
#include "msghook.h"

#pragma data_seg(".LUCA")
HWND hWndServer = NULL;
#pragma data_seg()
#pragma comment(linker, "/section:.LUCA,rws")

HHOOK hook;
HHOOK hook2;
HHOOK hook3;
HINSTANCE hInst;
//UINT UWM_MOUSEMOVE;
//UINT UWM_SIZING;

static LRESULT CALLBACK msghook(UINT nCode, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK createhook(UINT nCode, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK invisiblehook(UINT nCode, WPARAM wParam, LPARAM lParam);

BOOL APIENTRY DllMain( HINSTANCE hInstance, 
                       DWORD  Reason, 
                       LPVOID Reserved
					 )
{
 switch(Reason)
    { /* reason */
     case DLL_PROCESS_ATTACH:
	hInst = hInstance;
	return TRUE;
     case DLL_PROCESS_DETACH:
	if(hWndServer != NULL)
	   clearMyHook(hWndServer);
	return TRUE;
    } /* reason */
    return TRUE;
}


/****************************************************************************
*                                 setMyHook
* Inputs:
*       HWND hWnd: Window to notify
* Result: BOOL
*       TRUE if successful
*	FALSE if error
* Effect: 
*       Sets the hook
****************************************************************************/

__declspec(dllexport) BOOL setMyHook(HWND hWnd)
    {
     if(hWndServer != NULL)
	return FALSE; // already hooked!
     hook = SetWindowsHookEx(WH_CBT,//WH_GETMESSAGE,WH_CALLWNDPROC,//
			    (HOOKPROC)msghook,
			    hInst,
			    0);
     hook2 = SetWindowsHookEx(WH_SHELL,
			    (HOOKPROC)createhook,
			    hInst,
			    0);
	 hook3 = SetWindowsHookEx(WH_CALLWNDPROC,
			    (HOOKPROC)invisiblehook,
			    hInst,
			    0);


     if(hook != NULL && hook2 != NULL && hook3!=NULL)
	{ /* success */
	 hWndServer = hWnd;
	 return TRUE;
	} /* success */
     return FALSE; // failed to set hook
    } // setMyHook

/****************************************************************************
*                                 clearMyHook
* Inputs:
*       HWND hWnd: Window hook
* Result: BOOL
*       TRUE if successful
*	FALSE if error
* Effect: 
*       Removes the hook that has been set
****************************************************************************/

__declspec(dllexport) BOOL clearMyHook(HWND hWnd)
    {
     if(hWnd != hWndServer || hWnd == NULL)
	return FALSE;
     BOOL unhooked = UnhookWindowsHookEx(hook);
	 BOOL unhooked2 = UnhookWindowsHookEx(hook2);
     BOOL unhooked3 = UnhookWindowsHookEx(hook3);
	 if(unhooked && unhooked2 && unhooked3)
		 hWndServer = NULL;
     return unhooked && unhooked2 && unhooked3;
    } // clearMyHook

/****************************************************************************
*                                   msghook
* Inputs:
*       int nCode: Code value
*	WPARAM wParam:
*	LPARAM lParam:
* Result: LRESULT
*       Either 0 or the result of CallNextHookEx
* Effect: 
*       Hook processing function. If the message is a resizing window message,
*		create window message or destroy window message posts the event to the
*		parent window
****************************************************************************/


//hook per WH_GETMESSAGE

static LRESULT CALLBACK createhook(UINT nCode, WPARAM wParam, LPARAM lParam)
 {
//	MessageBox(hwndServer,"efaf","affs",0);
//	PostMessage(hWndServer,WM_CLOSE,0,0);
     if(nCode < 0)
	{ // pass it on 
	 
	 return CallNextHookEx(hook, nCode, wParam, lParam);;
	} // pass it on 
	 if(nCode=HSHELL_WINDOWCREATED){
		PostMessage(hWndServer,UWM_CREATE,wParam,lParam);
		return 0;
	 }
	 
	 return 0;
    } // msghook

//hook per WH_CBT

static LRESULT CALLBACK msghook(UINT nCode,WPARAM wParam,LPARAM lParam){
	if(nCode<0)
		return CallNextHookEx(hook,nCode,wParam,lParam);
	if(nCode==HCBT_MOVESIZE)
		//MessageBox(hwndServer,"efaf","affs",0);
		PostMessage(hWndServer,UWM_SIZING,wParam,lParam);
		//PostMessage(hWndServer,WM_CLOSE,0,0);		
//	if(nCode==HCBT_CREATEWND)
//		PostMessage(hWndServer,UWM_CREATE,wParam,lParam);
	else if(nCode==HCBT_DESTROYWND)
		PostMessage(hWndServer,UWM_CLOSE,wParam,lParam);
	else if(nCode==HCBT_ACTIVATE)
		PostMessage(hWndServer,UWM_FOCUS,wParam,lParam);
	else if(nCode==HCBT_MINMAX)
			PostMessage(hWndServer,UWM_MINMAX,wParam,lParam);
			
	return 0;

}


static LRESULT CALLBACK invisiblehook(UINT nCode,WPARAM wParam,LPARAM lParam){

	if(nCode<0)
		return CallNextHookEx(hook,nCode,wParam,lParam);
		CWPSTRUCT* strut=(CWPSTRUCT*)lParam;
		//MessageBox(hwndServer,"efaf","affs",0);
		if(strut->message==WM_SHOWWINDOW){
			if(strut->wParam==FALSE && strut->lParam==0)
				PostMessage(hWndServer,UWM_CLOSE,(UINT)strut->hwnd,strut->lParam);
		}
	//	else if(strut->message==WM_MENUCOMMAND){
	//			PostMessage(hWndServer,UWM_SYSMENU,wParam,lParam);
	//	}

//			else if(strut->wParam==TRUE && strut->lParam==0){
//				PostMessage(hWndServer,UWM_MINMAX,(UINT)strut->hwnd,MAKELPARAM(0,SW_RESTORE));//vedere se funziona...	
	//		}
	return CallNextHookEx(hook,nCode,wParam,lParam);
 
}

