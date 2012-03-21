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
