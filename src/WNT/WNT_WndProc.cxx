// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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
