/*
 Copyright (c) 1991-1999 Matra Datavision
 Copyright (c) 1999-2012 OPEN CASCADE SAS

 The content of this file is subject to the Open CASCADE Technology Public
 License Version 6.5 (the "License"). You may not use the content of this file
 except in compliance with the License. Please obtain a copy of the License
 at http://www.opencascade.org and read it completely before using this file.

 The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
 main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.

 The Original Code and all software distributed under the License is
 distributed on an "AS IS" basis, without warranty of any kind, and the
 Initial Developer hereby disclaims all such warranties, including without
 limitation, any warranties of merchantability, fitness for a particular
 purpose or non-infringement. Please see the License for the specific terms
 and conditions governing the rights and limitations under the License.

*/

#ifndef WNT

int _a_horreur_du_vide_a_la_compilation;

#else 

#define STRICT
#include <windows.h>

#pragma comment( lib, "user32.lib" )
#pragma comment( lib, "gdi32.lib"  )

#if WINVER >= 0x0400
# define NB_STATIC_COLORS ( COLOR_INFOBK - COLOR_SCROLLBAR + 1 )
#else
# define NB_STATIC_COLORS ( COLOR_BTNHIGHLIGHT - COLOR_SCROLLBAR + 1 )
#endif  /* WINVER . . . */

#define BLACK RGB(   0,   0,   0 )
#define WHITE RGB( 255, 255, 255 )

static int s_sysPalIdx[ NB_STATIC_COLORS ] = {

            COLOR_SCROLLBAR, 
            COLOR_BACKGROUND,
            COLOR_ACTIVECAPTION,
            COLOR_INACTIVECAPTION,
            COLOR_MENU,
            COLOR_WINDOW,
            COLOR_WINDOWFRAME,
            COLOR_MENUTEXT,
            COLOR_WINDOWTEXT,
            COLOR_CAPTIONTEXT,
            COLOR_ACTIVEBORDER,
            COLOR_INACTIVEBORDER,
            COLOR_APPWORKSPACE,
            COLOR_HIGHLIGHT,
            COLOR_HIGHLIGHTTEXT,
            COLOR_BTNFACE,
            COLOR_BTNSHADOW,
            COLOR_GRAYTEXT,
            COLOR_BTNTEXT,
            COLOR_INACTIVECAPTIONTEXT,
            COLOR_BTNHIGHLIGHT
#if WINVER >= 0x0400
            ,COLOR_3DDKSHADOW,
             COLOR_3DLIGHT,
             COLOR_INFOTEXT,
             COLOR_INFOBK
#endif  /* WINVER . . . */
};

static COLORREF s_sysPalBW[ NB_STATIC_COLORS ] = {

                 WHITE,  /* COLOR_SCROLLBAR           */
                 BLACK,  /* COLOR_BACKGROUND          */
                 BLACK,  /* COLOR_ACTIVECAPTION       */
                 WHITE,  /* COLOR_INACTIVECAPTION     */
                 WHITE,  /* COLOR_MENU                */
                 WHITE,  /* COLOR_WINDOW              */
                 BLACK,  /* COLOR_WINDOWFRAME         */
                 BLACK,  /* COLOR_MENUTEXT            */
                 BLACK,  /* COLOR_WINDOWTEXT          */
                 WHITE,  /* COLOR_CAPTIONTEXT         */
                 WHITE,  /* COLOR_ACTIVEBORDER        */
                 WHITE,  /* COLOR_INACTIVEBORDER      */
                 WHITE,  /* COLOR_APPWORKSPACE        */
                 BLACK,  /* COLOR_HIGHLIGHT           */
                 WHITE,  /* COLOR_HIGHLIGHTTEXT       */
                 WHITE,  /* COLOR_BTNFACE             */
                 BLACK,  /* COLOR_BTNSHADOW           */
                 BLACK,  /* COLOR_GRAYTEXT            */
                 BLACK,  /* COLOR_BTNTEXT             */
                 BLACK,  /* COLOR_INACTIVECAPTIONTEXT */
                 BLACK   /* COLOR_BTNHIGHLIGHT        */
#if WINVER >= 0x0400
                 ,BLACK,  /* BLACKCOLOR_3DDKSHADOW    */
                  BLACK,  /* COLOR_3DLIGHT            */
                  BLACK,  /* COLOR_INFOTEXT           */
                  BLACK   /* COLOR_INFOBK             */
#endif  /* WINVER . . . */
};

static BOOL     s_sysUse;
static COLORREF s_sysPal[ NB_STATIC_COLORS ];

__declspec( dllexport ) long InterfaceGraphic_RealizePalette ( HDC hdc, HPALETTE hPal, BOOL fBkg, BOOL fUseStatic ) {

 int i;
 long retVal = 0;

 if ( fUseStatic ) {

  if ( !s_sysUse )
   
   for ( i = 0; i < NB_STATIC_COLORS; ++i ) s_sysPal[ i ] = GetSysColor ( s_sysPalIdx[ i ] );

  SetSystemPaletteUse ( hdc, fBkg ? SYSPAL_STATIC : SYSPAL_NOSTATIC );

  if (  UnrealizeObject ( hPal ) && SelectPalette ( hdc, hPal, fBkg )  ) retVal = RealizePalette ( hdc );

  if ( fBkg ) {
 
   SetSysColors ( NB_STATIC_COLORS, s_sysPalIdx, s_sysPal );
   s_sysUse = FALSE;
 
  } else {
  
   SetSysColors ( NB_STATIC_COLORS, s_sysPalIdx, s_sysPalBW );
   s_sysUse = TRUE;
  
  }  // end else
 
  PostMessage ( HWND_BROADCAST, WM_SYSCOLORCHANGE, 0, 0 );

 } else {
 
  SelectPalette ( hdc, hPal, FALSE );
  retVal = RealizePalette ( hdc );
 
 }  // end else

 return retVal;
}  // end RealizePaletteNow

#endif /* WNT */
