// Copyright (c) 2008-2012 OPEN CASCADE SAS
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


#include <windows.h>
#include <WNT_IconBox.hxx>

#include <WNT_GraphicDevice.hxx>
#include <WNT_ImageManager.hxx>
#include <WNT_Icon.hxx>

#define X_SPACING 10
#define Y_SPACING 10

//=======================================================================
//function : HandleEvent
//purpose  :
//=======================================================================

WNT_Long WNT_IconBox::HandleEvent (
          const Aspect_Handle  hwnd,
          const WNT_Uint&      uMsg,
          const WNT_Dword&     wParam,
          const WNT_Dword&     lParam
         )
{

  switch ( uMsg ) {

  case WM_SIZE : {

   int        len;
   RECT       r;
   SCROLLINFO si;

   GetClientRect (  ( HWND )hwnd, &r  );

   myIncX = myIconWidth + X_SPACING + 6;
   myNX   = r.right  / myIncX;

   if ( myNX == 0 ) ++myNX;

   myIncY = myIconHeight + Y_SPACING + 11 + myIconHeight / Y_SPACING;
   myNY   = r.bottom / myIncY;

   if ( myNY == 0 ) ++myNY;

   len = myImages -> Size ();

   if ( myNX * myNY >= len )

    ShowScrollBar (  ( HWND )hwnd, SB_VERT, FALSE  );

   else {

    si.cbSize = sizeof ( SCROLLINFO );
	si.fMask  = SIF_POS | SIF_RANGE;
	si.nPos   = myNPos = 0;
	si.nMin   = 0;
	si.nMax   = myMaxPos = len / myNX;

    SetScrollInfo (  ( HWND )hwnd, SB_VERT, &si, TRUE );
   	ShowScrollBar (  ( HWND )hwnd, SB_VERT, TRUE  );
	myStart = 1;

   }  // end else

   return 0;

  }  // WM_SIZE

  case WM_VSCROLL : {

   int        nCurPos;
   int        nScrollCode;
   int        newStart;
   int        len;
   SCROLLINFO si;

   nScrollCode = ( Standard_Integer )LOWORD( wParam );
   len         = myImages -> Size ();

   switch ( nScrollCode ) {

   	case SB_LINEUP :
	case SB_PAGEUP :

	 newStart = myStart - myNX;
	 myStart    = newStart >= 1 ? --myNPos, newStart : (  ( myNPos = 0 ), 1  );

	break;

	case SB_LINEDOWN :
	case SB_PAGEDOWN :

	 newStart = myStart + myNX;
	 myStart    = ( newStart <= len - myNX + 1 ) ? ( ++myNPos, newStart ) :
	                                               (  ( myNPos = myMaxPos ),
	                                                  ( len - myNX + 1  )
	                                               );

	break;

	case SB_THUMBTRACK :

     myNPos  = ( Standard_Integer )HIWORD( wParam );
     myStart = myNPos * myNX;

	break;

	case SB_ENDSCROLL :

	return 0;

   }  // end switch

   if ( myStart == 0 ) ++myStart;

   nCurPos = GetScrollPos (  ( HWND )hwnd, SB_VERT  );

   if ( nCurPos == myNPos ) return 0;

   si.cbSize = sizeof ( SCROLLINFO );
   si.fMask  = SIF_POS;
   si.nPos   = myNPos;

   SetScrollInfo (  ( HWND )hwnd, SB_VERT, &si, TRUE  );
   InvalidateRect (  ( HWND )hwnd, NULL, TRUE  );

   return 0;

  }  // WM_VSCROLL

  case WM_PAINT : {

   HDC         hDCsrc, hDCdst;
   PAINTSTRUCT ps;
   HPEN        hOldPen;
   HFONT       hOldFont;
   HPALETTE    hOldPal;
   int         i, j, x, y, ow, oh, cnt, len;

   Handle( WNT_Icon ) icon;

   Handle( WNT_GraphicDevice ) gDev = Handle( WNT_GraphicDevice ) ::
                                       DownCast (  GraphicDevice ()  );

   if ( !myDragging ) {

    hDCdst = BeginPaint (  ( HWND )hwnd, &ps  );
    hDCsrc = CreateCompatibleDC ( hDCdst );

    if (  gDev -> IsPaletteDevice ()  ) {

   	 hOldPal = SelectPalette ( hDCdst, ( HPALETTE )(  gDev -> HPalette ()  ), FALSE );
	 RealizePalette ( hDCdst );

    }  // end if

     len      = myImages -> Size ();
	 hOldPen  = SelectPen ( hDCdst, myPen );
	 hOldFont = SelectFont ( hDCdst, myFont );
	 SetTextAlign ( hDCdst, TA_CENTER | TA_TOP );
	 SetStretchBltMode ( hDCdst, COLORONCOLOR );

	 for ( i = 0, y = Y_SPACING, cnt = myStart; i < myNY; ++i, y += myIncY )

	  for ( j = 0, x = X_SPACING; j < myNX; ++j, ++cnt, x += myIncX ) {

       if ( cnt > len ) goto exit_display;

	   icon = Handle( WNT_Icon ) :: DownCast (  myImages -> Image ( cnt )  );

	   if (  icon.IsNull ()  ) continue;

	   myImages -> Dim ( cnt, ow, oh );
	   SelectBitmap ( hDCsrc, icon -> myImage );

       Rectangle ( hDCdst, x, y, x + myIconWidth + 4, y + myIconHeight + 4 );
       TextOut  (
        hDCdst, x + myIconWidth / 2, y + myIconHeight + 10,
        icon -> myName, strlen ( icon -> myName )
       );

       StretchBlt (
        hDCdst, x + 2, y + 2, myIconWidth, myIconHeight,
        hDCsrc, 0,     0,     ow,          oh,
        SRCCOPY
       );

	  }  // end for

exit_display:

     SelectFont ( hDCdst, hOldFont );
	 SelectPen ( hDCdst, hOldPen );

    if (  gDev -> IsPaletteDevice ()  )

     SelectPalette ( hDCdst, hOldPal, FALSE );

    DeleteDC ( hDCsrc );
    EndPaint (  ( HWND )hwnd, &ps  );

    return 0;

   } else

    return DefWindowProc (  ( HWND )hwnd, uMsg, wParam, lParam  );

  }  // WM_PAINT

  case WM_ENTERSIZEMOVE :

   myDragging = TRUE;

   return 0;

  case WM_EXITSIZEMOVE :

   myDragging = FALSE;
   InvalidateRect (  ( HWND )hwnd, NULL, TRUE  );

   return 0;

  case WM_QUERYNEWPALETTE: {

   HDC          hDC = GetDC (  ( HWND )hwnd  );
   WINDOW_DATA* wd  = (WINDOW_DATA* )GetWindowLongPtr ((HWND )hwnd, GWLP_USERDATA);

   SelectPalette ( hDC, wd -> hPal, FALSE );

//   if (  RealizePalette ( hDC )  );
   if (  RealizePalette ( hDC )  )
    InvalidateRect (  ( HWND )hwnd, NULL, FALSE  );

   ReleaseDC (  ( HWND )hwnd, hDC  );

   return TRUE;

  }  // WM_QUERYNEWPALETTE

  case WM_PALETTECHANGED:

   if (  hwnd != ( HWND )wParam  ) {

    HDC          hDC = GetDC (  ( HWND )hwnd  );
    WINDOW_DATA* wd  = (WINDOW_DATA* )GetWindowLongPtr ((HWND )hwnd, GWLP_USERDATA);
    SelectPalette ( hDC, wd -> hPal, TRUE );

    if (  RealizePalette ( hDC )  )

     UpdateColors ( hDC );

    ReleaseDC (  ( HWND )hwnd, hDC  );

    return 0;

   }  // end if

  default:

   return DefWindowProc (  ( HWND )hwnd, uMsg, wParam, lParam  );

 }  // end switch

}  // end WNT_IconBox :: HandleEvent
