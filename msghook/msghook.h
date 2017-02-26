//#if _MSC_VER > 1000
//#pragma once
//#endif
//#ifdef __cplusplus
extern "C" {
//#endif // __cplusplus
	#define LIBSPEC __declspec(dllexport)
	/**Funzione che aggancia una finestra al sistema in modo da riceverne informazioni. 
	@param hWnd Handle della finestra che deve ricevere i messaggi intercettati dalla dll.
	@return True se l'aggancio al sistema è andato bene, false altrimenti.
	*/
   LIBSPEC BOOL setMyHook(HWND hWnd);
   /**Disinstalla l'hook dal sistema.
   @param hWnd Handle della finestra che aveva precedentemente installato l'hook.
   @return True se la disinstallazione è riuscita, false altrimenti.
   */
   LIBSPEC BOOL clearMyHook(HWND hWnd);
//LRESULT __declspec ( dllexport ) CALLBACK	msghook   ( int nCode, WPARAM wParam, LPARAM lParam );

#undef LIBSPEC
//#ifdef __cplusplus
}
//#endif // __cplusplus
//#define UWM_SIZING_MSG TEXT("UWM_SIZING_MSG-44E531B1_14D3_11d5_A025_006067718D04")
/**Macro per la creazione di messaggi personalizzati*/
#define DECLARE_REGISTERED_MESSAGE( name, name_s )  static const UINT	name = :: RegisterWindowMessage( name_s );

DECLARE_REGISTERED_MESSAGE( UWM_SIZING,     "UWM_SIZING"     )
DECLARE_REGISTERED_MESSAGE( UWM_CREATE,     "UWM_CREATE"     )
DECLARE_REGISTERED_MESSAGE( UWM_CLOSE,		"UWM_CLOSE"      )
DECLARE_REGISTERED_MESSAGE( UWM_FOCUS,		"UWM_FOCUS"      )
DECLARE_REGISTERED_MESSAGE( UWM_MINMAX, 	"UWM_MINMAX"	 )
DECLARE_REGISTERED_MESSAGE( UWM_SYSMENU, 	"UWM_SYSMENU"	 )

// These declarations are because the latest SDK isn't installed...
//#ifndef GET_X_LPARAM
//#define GET_X_LPARAM(p) ((int)(short)LOWORD(p))
//#define GET_Y_LPARAM(p) ((int)(short)HIWORD(p))
//#endif


