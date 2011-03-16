#include <WNT_FontMapEntry.ixx>

#include <InterfaceGraphic_WNT.hxx>

#include <stdlib.h>
#include <string.h>						 

// character for field separator
#define FS "-"
// character for default value
#define DV "*"

WNT_FontMapEntry :: WNT_FontMapEntry ( const Standard_CString aFontName )
{
  int   i;
  char* p;
  BOOL  fUseDefault = FALSE;

  char* fName = new char[ strlen ( aFontName ) + 1 ];

  strcpy ( fName, aFontName );
  ZeroMemory (  &myLogFont, sizeof ( WNT_LogFont )  );

  myLogFont.lfCharSet        = DEFAULT_CHARSET;
  myLogFont.lfOutPrecision   = OUT_DEFAULT_PRECIS;
  myLogFont.lfClipPrecision  = CLIP_DEFAULT_PRECIS;
  myLogFont.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
  myLogFont.lfQuality        = PROOF_QUALITY;

  p = strtok ( fName, FS );
  if (  p && strcmp ( p, DV )  )
    myLogFont.lfHeight = atol ( p );

  p = strtok ( NULL, FS );
  if (  p && strcmp ( p, DV )  )
    myLogFont.lfWidth = atol ( p );

  p = strtok ( NULL, FS );
  if (  p && strcmp ( p, DV )  )
    myLogFont.lfEscapement = atol ( p );

  p = strtok ( NULL, FS );
  if (  p && strcmp ( p, DV )  )
    myLogFont.lfOrientation = atol ( p );

  p = strtok ( NULL, FS );
  if (  p && strcmp ( p, DV )  )
    myLogFont.lfWeight = atol ( p );

  p = strtok ( NULL, FS );
  if (  p && strcmp ( p, DV )  )
    myLogFont.lfItalic = ( BYTE )atoi ( p );

  p = strtok ( NULL, FS );
  if (  p && strcmp ( p, DV )  )
    myLogFont.lfUnderline = ( BYTE )atoi ( p );

  p = strtok ( NULL, FS );
  if (  p && strcmp ( p, DV )  )
    myLogFont.lfStrikeOut = ( BYTE )atoi ( p );

  p = strtok ( NULL, FS );
  if (  p && strcmp ( p, DV )  )
    myLogFont.lfCharSet = ( BYTE )atoi ( p );

  p = strtok ( NULL, FS );
  if (  p && strcmp ( p, DV )  )
    myLogFont.lfOutPrecision = ( BYTE )atoi ( p );

  p = strtok ( NULL, FS );
  if (  p && strcmp ( p, DV )  )
    myLogFont.lfClipPrecision = ( BYTE )atoi ( p );

  p = strtok ( NULL, FS );
  if (  p && strcmp ( p, DV )  )
    myLogFont.lfQuality = ( BYTE )atoi ( p );

  p = strtok ( NULL, FS );
  if (  p && strcmp ( p, DV )  )
    myLogFont.lfPitchAndFamily = ( BYTE )atoi ( p );

  p = strtok ( NULL, FS );
  if (  p && strcmp ( p, DV )  ) {
    i = strlen ( p );
    strncpy (  myLogFont.lfFaceName, p, ( i < LF_FACESIZE ) ? i : LF_FACESIZE  );
  }  // end if

  myLogFont.lfOutPrecision |= OUT_TT_ONLY_PRECIS;
  myHandle = CreateFontIndirect ( &myLogFont );
  delete [] fName;
  if ( !myHandle )
    WNT_FontMapEntryDefinitionError :: Raise ( "Unable to load font" );
}  // end constructor

void WNT_FontMapEntry :: Destroy () {

 DeleteObject ( myHandle );

}  // end WNT_FontMapEntry :: Destroy 

Aspect_Handle WNT_FontMapEntry :: HFont () const {

 return myHandle;

}  // WNT_FontMapEntry :: HFont

Aspect_Handle WNT_FontMapEntry :: SetAttrib (
                                const WNT_Dword& aFlags,
                                const Standard_Address aData,
                                const Standard_Boolean aRepl
                               ) {

 HFONT      hFont;
 LOGFONT    lf = myLogFont;
 FONT_DATA* fd = ( FONT_DATA* )aData;

 if ( aFlags & faUnderlined )

  lf.lfUnderline = fd -> fdUnderlined;

 if ( aFlags & faItalic )

  lf.lfItalic = fd -> fdItalic;

 if ( aFlags & faStrikeOut )

  lf.lfStrikeOut = fd -> fdStrikeOut;

 if ( aFlags & faBold )

  lf.lfWeight = fd -> fdBold;

 if ( aFlags & faHeight )

  lf.lfHeight = fd -> fdHeight;

 if ( aFlags & faAngle ) {

  lf.lfEscapement  = fd -> fdOrientation;
  lf.lfOrientation = fd -> fdOrientation;

 }  // end if

 if ( aFlags & faWidth )

  lf.lfWidth = fd -> fdWidth;

 if ( aFlags & faSlant )

  lf.lfOrientation -= ( fd -> fdSlant * 10 );

 hFont = CreateFontIndirect ( &lf );

 if ( hFont != NULL && aRepl ) {
 
  DeleteObject ( myHandle );
  myHandle  = hFont;
  myLogFont = lf;

 }  // end if

 return hFont;

}  // WNT_FontMapEntry :: SetAttrib
