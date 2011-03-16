// include windows.h first to have all definitions available
#include <windows.h>

#include <WNT_Window.hxx>

//***//
//*** This window procedure provides management of the window background.   ***//
//*** Background belongs to the window class but we need that windows which ***//
//*** are based on the same class have different backgrounds. So, we are    ***//
//*** using	window subclassing technique to provide this ability.           ***//
//***//
LRESULT CALLBACK WNT_WndProc (
                  HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam
                 ) {

 HDC          hDC;
 HPALETTE     hOldPal;
 WNDPROC      lpfnWndProc;
 WINDOW_DATA* wd;
 WNT_Window*  win;
 RECT         invRect;

 wd          = (WINDOW_DATA* )GetWindowLongPtr (hwnd, GWLP_USERDATA);
 win         = (WNT_Window* )wd->WNT_Window_Ptr;
 lpfnWndProc = (WNDPROC )win->WndProc();

 if (  msg == WM_ERASEBKGND && !( wd -> dwFlags & WDF_NOERASEBKGRND )  ) {

  hDC = ( HDC )wParam;

  if ( wd -> hPal ) {

   hOldPal = SelectPalette (  hDC, wd -> hPal, FALSE  );

   if (  RealizePalette ( hDC )  )

    UpdateColors ( hDC );

  }  // end if

  GetClipBox ( hDC, &invRect );
  FillRect (   hDC, &invRect, ( HBRUSH )( win -> HBackground ()  )   );

  if ( wd -> hPal )

   SelectPalette ( hDC, hOldPal, FALSE );

  return TRUE;

 } else if ( msg == WM_MOVE ) {

  WINDOWPLACEMENT wp;

  wp.length = sizeof ( WINDOWPLACEMENT );
  GetWindowPlacement ( hwnd, &wp );

  win -> SetPos (
          wp.rcNormalPosition.left,  wp.rcNormalPosition.top,
          wp.rcNormalPosition.right, wp.rcNormalPosition.bottom
         );

 } 	// end else

 return CallWindowProc ( lpfnWndProc, hwnd, msg, wParam, lParam );

}  // end WNT_WndProc
//***//
