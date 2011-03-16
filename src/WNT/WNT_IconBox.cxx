// include windows.h first to have all definitions available
#include <windows.h>
#include <windowsx.h>

#include <WNT_IconBox.ixx>

#include <WNT_WClass.hxx>
#include <WNT_ImageManager.hxx>
#include <WNT_Icon.hxx>

//***//
static void __fastcall _get_filename ( char*, int, char*, char* );
//***//
HBITMAP LoadImageFromFile (  Handle( WNT_GraphicDevice )&, char*, HDC = NULL  );
int     SaveBitmapToFile (
         Handle( WNT_GraphicDevice )& gDev,
         HBITMAP, char*, int, int, int, int
	    );
LRESULT CALLBACK WNT_IconBoxWndProc ( HWND, UINT, WPARAM, LPARAM );
//***//
//*************************** Constructor *******************************//
//***//
WNT_IconBox :: WNT_IconBox (
                const Handle( WNT_GraphicDevice )& aDevice,
                const Standard_CString             aName,
                const WNT_Dword&                   aStyle,
                const Standard_Real                Xc,
                const Standard_Real                Yc,
                const Quantity_NameOfColor         aBkColor
               ) : WNT_Window (
                    aDevice, aName,
                    new WNT_WClass (
                         "WNT_IconBoxClass", WNT_IconBoxWndProc,
                         CS_HREDRAW | CS_VREDRAW
                        ),
                    aStyle | WS_VSCROLL,
                    Xc, Yc, 0.5, 0.2, aBkColor
                   ) {

  if (  LoadIcons ( aName ) == 0  )

   Aspect_WindowDefinitionError :: Raise ( "Unable to load icons" );

  myFont = NULL;
  SetDim ( 70, 70 );
  myStart = 1;

  myPen = CreatePen (  PS_SOLID, 3, RGB( 0, 255, 0 )  );

}  // end constructor
//***//
//********************************* Destroy *****************************//
//***//
void WNT_IconBox :: Destroy () {

 if ( myFont != NULL )

  DeleteObject ( myFont );

 if ( myPen != NULL )

  DeleteObject ( myPen );

}  // end WNT_IconBox :: Destroy
//***//
//******************************* LoadIcons *****************************//
//***//
Standard_Integer WNT_IconBox :: LoadIcons (
                                 const Standard_CString Name
                                ) {

 int                len;
 Standard_Integer   retVal = 0;
 ifstream           is;
 char               fileName[ MAX_PATH ];
 char               fName[ MAX_PATH ];
 char               iName[ MAX_PATH ];
 Standard_Integer   iHCode;
 HBITMAP            hBmp;
 Handle( WNT_Icon )	icon;

 Handle( WNT_GraphicDevice ) gDev = Handle( WNT_GraphicDevice ) ::
                                     DownCast (  GraphicDevice ()  );

 _get_filename ( fileName, MAX_PATH, (Standard_PCharacter)Name, "ifd" );

 is.open ( fileName );

 if ( is ) {

  while (  !is.eof ()  ) {

   is >> fName >> iName;

   if (  is.bad ()  ) break;

   if ( fName[ 0 ] == '\x00' || iName[ 0 ] == '\x00' ) continue;

   if (  fName[ 0 ] == '#' ) continue;

   _get_filename ( fileName, MAX_PATH, fName, "" );

   iHCode = myImages -> StringHashCode ( fileName );
   len    = myImages -> Size ();
   int i;

   for ( i = 1; i <= len; ++i )

    if (  myImages -> HashCode ( i ) == iHCode  ) {

     icon = Handle( WNT_Icon ) :: DownCast (  myImages -> Image ( i )  );

     if (  lstrcmp ( iName, icon -> myName )  ) {

      i = len + 1;
      break;

     }  /* end if */

    }  /* end if */

   if ( i > len ) {

    hBmp = LoadImageFromFile ( gDev, fileName );

    if ( hBmp ) {

     icon = new WNT_Icon ( iName, hBmp, iHCode );
     myImages -> Add ( icon );

    }  // end if

   }  // end if ( i > len . . . )

   fName[ 0 ] = iName[ 0 ] = '\x00';

  }  // end while

  retVal = myImages -> Size ();

 }  // end if

 return retVal;

}  // end WNT_IconBox :: LoadIcons
//***//
//********************************** Show *******************************//
//***//
void WNT_IconBox :: Show () const {

 Map ();

}  // end WNT_IconBox :: Show
//***//
//******************************* UnloadIcons ***************************//
//***//
Standard_Integer WNT_IconBox :: UnloadIcons (
                                 const Standard_CString Name
                                ) {

 Standard_Integer   retVal = 0;
 ifstream           is;
 char               fileName[ MAX_PATH ];
 char               fName[ MAX_PATH ];
 char               iName[ MAX_PATH ];
 Handle( WNT_Icon )	icon;

 Handle( WNT_GraphicDevice ) gDev = Handle( WNT_GraphicDevice ) ::
                                     DownCast (  GraphicDevice ()  );

 _get_filename ( fileName, MAX_PATH, (Standard_PCharacter)Name, "ifd" );

 is.open ( fileName );

 if ( is ) {

  while (  !is.eof ()  ) {

   is >> fName >> iName;

   if (  is.bad ()  ) break;

   if (  fName[ 0 ] == '#' ) continue;

   for ( int i = 1; i <= myImages -> Size (); ++i ) {

    icon = Handle( WNT_Icon ) :: DownCast (  myImages -> Image ( i )  );

	if (  icon.IsNull ()  ) continue;

	if (  lstrcmp ( icon -> myName, iName )  ) continue;

	myImages -> Delete ( i );
	++retVal;

   }  // end for

  }  // end while

 }  // end if

 return retVal;

}  // end WNT_IconBox :: UnloadIcons
//***//
//********************************* AddIcon *****************************//
//***//
void WNT_IconBox :: AddIcon (
                     const Handle( WNT_Window )& W,
                     const Standard_CString      Name,
                     const Standard_Integer      aWidth,
                     const Standard_Integer      aHeight
                    ) {

  Handle( WNT_Icon ) icon = W -> myIcon;

  if (  !icon.IsNull ()  )

   myImages -> Add ( W -> myIcon );

}  // end WNT_IconBox :: AddIcon
//***//
//********************************* SaveIcons ***************************//
//***//
Standard_Integer WNT_IconBox :: SaveIcons () const {

 int                w, h;
 Standard_Integer   retVal = 0;
 char               ext[ 5 ];
 char               fName[ MAX_PATH ];
 Handle( WNT_Icon )	icon;

 Handle( WNT_GraphicDevice ) gDev = Handle( WNT_GraphicDevice ) ::
                                     DownCast (  GraphicDevice ()  );

 switch ( myFormat ) {

  case WNT_TOI_XWD:

   strcpy ( ext, ".xwd"	);

  break;

  case WNT_TOI_BMP:

   strcpy ( ext, ".bmp" );

  break;

  case WNT_TOI_GIF:

   strcpy ( ext, ".gif" );

 }  // end switch

 for ( int i = 1; i <= myImages -> Size (); ++i ) {

  icon = Handle( WNT_Icon ) :: DownCast (  myImages -> Image ( i )  );

  if (  icon.IsNull ()  ) continue;

  strcpy ( fName, icon -> myName );

  if (  strchr ( fName, '.' ) == NULL  )

   strcat ( fName, ext );

  myImages -> Dim ( i, w, h );

  if (  SaveBitmapToFile (
         gDev, ( HBITMAP )(  myImages -> ImageHandle ( i )  ), fName,
		 0, 0, w, h
		)
  )

   ++retVal;

 }  // end for

 return retVal;

}  // end WNT_IconBox :: SaveIcons
//***//
//********************************* IconNumber **************************//
//***//
Standard_Integer WNT_IconBox :: IconNumber () const {

 return myImages -> Size ();

}  // end WNT_IconBox :: IconNumber
//***//
//********************************* IconName ****************************//
//***//
Standard_CString WNT_IconBox :: IconName (
                                 const Standard_Integer Index
                                ) const {

 Handle( WNT_Icon ) icon = Handle( WNT_Icon ) :: DownCast (
                                                  myImages -> Image ( Index )
												 );

 return icon -> myName;

}  // end WNT_IconBox :: IconName
//***//
//********************************* IconSize ****************************//
//***//
Standard_Boolean WNT_IconBox :: IconSize (
                                 const Standard_CString Name,
                                 Standard_Integer&      Width,
                                 Standard_Integer&      Height
                                ) const {

 int                i, len = myImages -> Size ();
 Handle( WNT_Icon ) icon;

 for ( i = 1; i <= len; ++i ) {

  icon = Handle( WNT_Icon ) :: DownCast (  myImages -> Image ( i )  );

  if (  !strcmp ( icon -> myName, Name )  ) break;

 }  // end for

 return ( i > len ) ? Standard_False :
                      myImages -> Dim ( i, Width, Height ), Standard_True;

}  // end WNT_IconBox :: IconSize
//***//
//********************************* IconPixmap (1) **********************//
//***//
Aspect_Handle WNT_IconBox :: IconPixmap (
                            const Standard_CString Name
                          ) const {

 int                i, len = myImages -> Size ();
 Handle( WNT_Icon ) icon;

 for ( i = 1; i <= len; ++i ) {

  icon = Handle( WNT_Icon ) :: DownCast (  myImages -> Image ( i )  );

  if (  !strcmp ( icon -> myName, Name )  ) break;

 }  // end for

 return ( i > len ) ? NULL : myImages -> ImageHandle ( i );

}  // end WNT_IconBox :: IconPixmap
//***//
//********************************* IconPixmap (2) **********************//
//***//
Aspect_Handle WNT_IconBox :: IconPixmap (
                           const Standard_CString Name,
                           const Standard_Integer Width,
                           const Standard_Integer Height
                          ) const {

 HBITMAP            retVal = NULL;
 int                len = myImages -> Size ();
 int                i, w, h;
 Handle( WNT_Icon ) icon;
 HPALETTE           hOldPal;

 Handle( WNT_GraphicDevice ) gDev = Handle( WNT_GraphicDevice ) ::
                                     DownCast (  GraphicDevice ()  );

 for ( i = 1; i <= len; ++i ) {

  icon = Handle( WNT_Icon ) :: DownCast (  myImages -> Image ( i )  );

  if (  !strcmp ( icon -> myName, Name )  ) break;

 }  // end for

 if ( i <= len ) {

  HDC hDC, hDCmemSrc, hDCmemDst;

  hDC = GetDC ( NULL );

   if (  gDev -> IsPaletteDevice ()  ) {

   	hOldPal = SelectPalette ( hDC, ( HPALETTE )(  gDev -> HPalette ()  ), FALSE );
	RealizePalette ( hDC );

   }  // end if

   hDCmemSrc = CreateCompatibleDC ( hDC );
   hDCmemDst = CreateCompatibleDC ( hDC );

   retVal = CreateCompatibleBitmap ( hDC, Width, Height );

   if ( retVal != NULL ) {

   	myImages -> Dim ( i, w, h );

	SelectBitmap (  hDCmemSrc, myImages -> ImageHandle ( i )  );
	SelectBitmap (  hDCmemDst, retVal );

	BitBlt (
	 hDCmemDst, 0, 0, Width, Height, hDCmemSrc,
	 w / 2 - Width / 2, h / 2 - Height / 2, SRCCOPY
	);

   }  // end if

   DeleteDC ( hDCmemDst );
   DeleteDC ( hDCmemSrc );

   if (  gDev -> IsPaletteDevice ()  )

    SelectPalette ( hDC, hOldPal, FALSE );

  ReleaseDC ( NULL, hDC );

 }  // end if

 return retVal;

}  // end WNT_IconBox :: IconPixmap
//***//
//******************************* SetDim ********************************//
//***//
void WNT_IconBox :: SetDim (
                     const Standard_Integer aWidth,
                     const Standard_Integer aHeight
                    ) {

 LOGFONT lf;

 myIconWidth  = aWidth;
 myIconHeight = aHeight;

 ZeroMemory (  &lf, sizeof ( LOGFONT )  );

 lf.lfHeight = myIconHeight / 5;
 strcpy ( lf.lfFaceName, "Modern" );

 if ( myFont != NULL ) DeleteObject ( myFont );

 myFont = CreateFontIndirect ( &lf );

}  // end WNT_IconBox :: SetDim
//***//
//***********************************************************************//
//***//
static void __fastcall _get_filename ( char* retVal, int len, char* name, char* ext ) {

 char* ptr, *ptr1;
 char  eNameVal[ MAX_PATH ];
 char  fNameVal[ MAX_PATH ];
 DWORD eNameLen;

 len = Min ( MAX_PATH, len );

 ZeroMemory (  ( PVOID )eNameVal, sizeof ( eNameVal )  );
 ZeroMemory (  ( PVOID )fNameVal, sizeof ( fNameVal )  );

 if ( name != NULL ) {

  strncpy ( fNameVal, name, len );

  if ( *name == '$' ) {

   ptr = strpbrk ( fNameVal, "\\/" );

   if ( ptr != NULL ) {

   	*ptr++ = '\x00';
	ptr1 = fNameVal + 1;

	if (
	 (  eNameLen = GetEnvironmentVariable ( ptr1, eNameVal, MAX_PATH )  ) != 0
	)

	 strncpy ( retVal, eNameVal, len );

	strncat ( retVal, "\\", len );

   } else

    ptr = fNameVal;

   strncat ( retVal, ptr, len );

  } else

   strncpy ( retVal, fNameVal, len );

  if (  strpbrk ( retVal, "." ) == NULL  ) {

   strncat ( retVal, ".", len );
   strncat ( retVal, ext, len );

  }  // end if

  ptr = retVal;

  for ( int i = 0; i < ( int )(  strlen ( retVal )  ); ++i )

   if ( ptr[ i ] == '/' ) ptr[ i ] = '\\';

 }  // end if

}  // end _get_filename
//***//
//***********************************************************************//
//***//
LRESULT CALLBACK WNT_IconBoxWndProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  WINDOW_DATA* wd = (WINDOW_DATA* )GetWindowLongPtr (hwnd, GWLP_USERDATA);
  if (wd != NULL)
  {
    WNT_IconBox* ib = (WNT_IconBox* )(wd->WNT_Window_Ptr);
    return ib->HandleEvent (hwnd, uMsg, wParam, lParam);
  }
  else
  {
    return DefWindowProc (hwnd, uMsg, wParam, lParam);
  }
}  // end WNT_IconBoxWndProc
//***//
//***********************************************************************//
