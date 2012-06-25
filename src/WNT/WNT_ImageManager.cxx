// Copyright (c) 1998-1999 Matra Datavision
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

// Modifications: PLOTNIKOV Eugeny at July 1998 (BUC60286)

// include windows.h first to have all definitions available
#include <windows.h>
#include <windowsx.h>

#include <WNT_ImageManager.ixx>

#include <WNT_Image.hxx>
#include <WNT_GraphicDevice.hxx>

#include <WNT_Bitmap.h>

#define TRASH_SIZE 8

#define PWND (  ( WNT_WindowPtr )myWindow  )

HBITMAP LoadImageFromFile (  Handle( WNT_GraphicDevice )&, char*, HDC = NULL  );
int     SaveWindowToFile  (
         Handle( WNT_GraphicDevice )&, HWND, char*, int,  int,  int, int
        );
int     SaveBitmapToFile  (
         Handle( WNT_GraphicDevice )&, HBITMAP, char*, int,  int,  int, int
		);
void    SetOutputFormat ( WNT_TypeOfImage );
//***//
//***************************** Constructor ******************************//
//***//
WNT_ImageManager :: WNT_ImageManager (  const WNT_WindowPtr& aWindow  ) {

 myWindow    = aWindow;
 myLastIndex = 0;

}  // end constructor
//***//
//******************************* Destroy ********************************//
//***//
void WNT_ImageManager :: Destroy () {


}  // end WNT_ImageManager :: Destroy
//***//
//******************************* SetFormat ******************************//
//***//
void WNT_ImageManager :: SetFormat ( const WNT_TypeOfImage aFormat ) {

 myFormat = aFormat;
 SetOutputFormat ( aFormat );

}  // end WNT_ImageManager :: SetFormat
//***//
//********************************** Load ********************************//
//***//
Standard_Integer WNT_ImageManager :: Load ( const Standard_CString aFileName ) {

 Handle( WNT_Image ) image;
 Standard_Integer    i, iHCode, len, retVal = 0;
 HBITMAP             hBmp;

 Handle( WNT_GraphicDevice ) gDev = Handle( WNT_GraphicDevice ) ::
                                     DownCast (  PWND -> GraphicDevice ()  );

 iHCode = StringHashCode ( aFileName );

 if ( myLastIndex && myLastImage -> myHashCode == iHCode ) return myLastIndex;

 for ( i = 1; i <= myTrash.Length (); ++i )

  if (  myTrash.Value ( i ) -> myHashCode == iHCode  ) {

   myLastImage = myTrash.Value ( i );
   retVal = -i;

   goto end;
  
  }  /* end if */

 len = myImages.Length ();

 for ( i = 1; i <= len; ++i )

  if (  myImages.Value ( i ) -> myHashCode == iHCode  ) {

   myLastImage = myImages.Value ( i );
   retVal = i;

   goto end;

  }  // end if

 hBmp = LoadImageFromFile ( gDev,(Standard_PCharacter) aFileName );

 if ( hBmp ) {
  
  myLastImage = new WNT_Image ( hBmp, iHCode );
  myImages.Append ( myLastImage );
  retVal = myImages.Length ();
  
 }  // end if
end:
 return myLastIndex = retVal;   
 
}  // end WNT_ImageManager :: Load
//***//
//********************************** Save ********************************//
//***//
Standard_Boolean WNT_ImageManager :: Save (
                                      const Standard_CString aFileName,
                                      const Standard_Integer aX,
                                      const Standard_Integer aY,
                                      const Standard_Integer aWidth,
                                      const Standard_Integer aHeight
                                     ) const {

 Standard_Boolean retVal;

 Handle( WNT_GraphicDevice ) gDev = Handle( WNT_GraphicDevice ) ::
                                     DownCast (  PWND -> GraphicDevice ()  );

 retVal = SaveWindowToFile (
           gDev, ( HWND )(  PWND -> HWindow ()  ),
          (Standard_PCharacter) aFileName, aX, aY, aWidth, aHeight
          );

 return retVal;

}  // end WNT_ImageManager :: Save
//***//
//****************************** SaveBuffer ******************************//
//***//
Standard_Boolean WNT_ImageManager :: SaveBuffer (
                                      const Standard_CString aFileName,
                                      const Standard_Integer aX,
                                      const Standard_Integer aY,
                                      const Standard_Integer aWidth,
                                      const Standard_Integer aHeight
                                     ) const {

 Standard_Boolean retVal;
 Handle( WNT_GraphicDevice ) gDev = Handle( WNT_GraphicDevice ) ::
                                     DownCast (  PWND -> GraphicDevice ()  );

 retVal = SaveBitmapToFile (
           gDev, ( HBITMAP )(  PWND -> HPixmap ()  ),
           (Standard_PCharacter)aFileName, aX, aY, aWidth, aHeight
          );

 return retVal;
								
}  // end WNT_ImageManager :: SaveBuffer
//***//
//********************************** Draw ********************************//
//***//
void WNT_ImageManager :: Draw (
                          const Standard_Integer anIndex,
                          const Standard_Integer Xc,
                          const Standard_Integer Yc,
			        	  const Standard_Integer aWidth,
				          const Standard_Integer aHeight,
                          const Standard_Real    anAngle
                         ) {

 HDC              hDC, hDCmemSrc, hDCmemDst = 0;
 HPALETTE         hPal, hOldPal;
 Standard_Integer iw, ih;

 if ( myLastIndex != anIndex ) {
  
  myLastIndex = anIndex;
  myLastImage = anIndex < 0 ? myTrash.Value ( -anIndex ) : myImages.Value ( anIndex );

 }  // end if

 Handle( WNT_GraphicDevice ) gDev = Handle( WNT_GraphicDevice ) ::
                                     DownCast (  PWND -> GraphicDevice ()  );
 
 Dim ( anIndex, iw, ih );

 hDC = GetDC (   ( HWND )(  PWND -> HWindow ()  )   );

  if (  gDev -> IsPaletteDevice ()  ) {
  
   hOldPal = SelectPalette (
              hDC, hPal = ( HPALETTE )(  gDev -> HPalette ()  ), FALSE
             );

   if (  RealizePalette ( hDC )  )

    UpdateColors ( hDC );
  
  }  // end if

  if (  PWND -> DoubleBuffer ()  ) {
  
   hDCmemDst = CreateCompatibleDC ( hDC );
   SelectBitmap (  hDCmemDst, PWND -> HPixmap ()  );
  
  } else
  
   hDCmemDst = hDC;  
  
  hDCmemSrc = CreateCompatibleDC ( hDC );
  SetStretchBltMode ( hDCmemDst, COLORONCOLOR );

   SelectBitmap (
    hDCmemSrc, (  ( PWNT_Bitmap )myLastImage -> myImage  ) -> hBmp
   );

   if ( aWidth == iw && aHeight == ih && anAngle == 0.0F )
 
 	BitBlt (
 	 hDCmemDst, Xc - aWidth / 2 - ( aWidth & 1 ),
 	 Yc - aHeight / 2, aWidth, aHeight,
	 hDCmemSrc, 0, 0, SRCCOPY
	);
 
   else if ( anAngle == 0.0F )
   
	StretchBlt (
     hDCmemDst, Xc - aWidth / 2,
     Yc - aHeight / 2 + ( aHeight & 1 ), aWidth, aHeight,
     hDCmemSrc, 0, 0, iw, ih, SRCCOPY
    );        
   
   else {
   
    XFORM  x;
    POINT  pivot;
   	POINT  pts[ 3 ];
	double sinVal, cosVal, angle;

	SetGraphicsMode ( hDCmemDst, GM_ADVANCED );

    angle = ( double )anAngle * ( M_PI / 180. );
	cosVal = Cos ( angle );
	sinVal = Sin ( angle );

    pts[ 0 ].x = Xc - aWidth  / 2;
	pts[ 0 ].y = Yc - aHeight / 2 + ( aHeight & 1 );

	pts[ 1 ].x = Xc + aWidth / 2;
	pts[ 1 ].y = pts[ 0 ].y;

	pts[ 2 ].x = pts[ 0 ].x;
	pts[ 2 ].y = Yc + aHeight / 2 + ( aHeight & 1 );

	pivot.x = ( pts[ 1 ].x + pts[ 2 ].x ) / 2;
	pivot.y = ( pts[ 1 ].y + pts[ 2 ].y ) / 2;

	x.eM11 = 1.0F; x.eM12 = 0.0F;
	x.eM21 = 0.0F; x.eM22 = 1.0F;
	x.eDx  = ( float )-pivot.x;
	x.eDy  = ( float )-pivot.y;
	ModifyWorldTransform ( hDCmemDst, &x, MWT_RIGHTMULTIPLY );

	x.eM11 = ( float )cosVal; x.eM12 = ( float )-sinVal;
	x.eM21 = ( float )sinVal; x.eM22 = ( float ) cosVal;
	x.eDx  = 0.0F;
	x.eDy  = 0.0F;
	ModifyWorldTransform ( hDCmemDst, &x, MWT_RIGHTMULTIPLY );

	x.eM11 = 1.0F; x.eM12 = 0.0F;
	x.eM21 = 0.0F; x.eM22 = 1.0F;
	x.eDx  = ( float )pivot.x;
	x.eDy  = ( float )pivot.y;
	ModifyWorldTransform ( hDCmemDst, &x, MWT_RIGHTMULTIPLY );

	PlgBlt ( hDCmemDst, pts, hDCmemSrc, 0, 0, iw, ih, NULL, 0, 0 );

   }  // end else

  DeleteDC ( hDCmemSrc );

  if (  gDev -> IsPaletteDevice ()  )

   SelectPalette ( hDC, hOldPal, FALSE );

  if (  PWND -> DoubleBuffer ()  )

   DeleteDC ( hDCmemDst );

 ReleaseDC (   ( HWND )(  PWND -> HWindow ()  ), hDC   );

}  // end WNT_ImageManager :: Draw
//***//
//********************************** Scale *******************************//
//***//
Aspect_Handle WNT_ImageManager :: Scale (
                                    const Standard_Integer anIndex,
                                    const Standard_Real    aScaleX,
                                    const Standard_Real    aScaleY,
                                    const Standard_Boolean aReplace
                                   ) {

 Standard_Integer iw, ih, iNw, iNh;
 HDC              hDCmemSrc, hDCmemDst, hDC;
 HPALETTE         hOldPal = NULL;
 HBITMAP          hBitmap, hOldBitmap, hBmp;

 if ( myLastIndex != anIndex ) {
 
  myLastIndex = anIndex;
  myLastImage = anIndex < 0 ? myTrash.Value ( -anIndex ) : myImages.Value ( anIndex );

 }  // end if

 Handle( WNT_GraphicDevice ) gDev = Handle( WNT_GraphicDevice ) ::
                                     DownCast (  PWND -> GraphicDevice ()  );

 Dim ( anIndex, iw, ih );

 iNw = ( Standard_Integer )( iw * aScaleX );
 iNh = ( Standard_Integer )( ih * aScaleY );

 hDC = GetDC ( NULL );

  if (  gDev -> IsPaletteDevice ()  )
  
   hOldPal = SelectPalette (   hDC, ( HPALETTE )(  gDev -> HPalette ()  ), FALSE   );
   
  hDCmemSrc = CreateCompatibleDC ( hDC );
  hDCmemDst = CreateCompatibleDC ( hDC );

  SetStretchBltMode ( hDCmemDst, COLORONCOLOR );

  hBitmap = CreateCompatibleBitmap ( hDC, iNw, iNh );

  if ( hBitmap ) {

   hBmp = (  ( PWNT_Bitmap )myLastImage -> myImage  ) -> hBmp;
   hOldBitmap = SelectBitmap ( hDCmemSrc, hBmp );
   SelectBitmap ( hDCmemDst, hBitmap );

   StretchBlt (
    hDCmemDst,  0, 0, iNw, iNh,
    hDCmemSrc, 0, 0, iw, ih, SRCCOPY
   );

   SelectBitmap ( hDCmemSrc, hOldBitmap );

   if ( aReplace ) {

    DeleteObject ( hBmp );
    (  ( PWNT_Bitmap )myLastImage -> myImage  ) -> hBmp = hBitmap;

   }  // end if
    
  }  // end if

  DeleteDC ( hDCmemDst );
  DeleteDC ( hDCmemSrc );

  if ( hOldPal != NULL ) SelectPalette ( hDC, hOldPal, FALSE );

 ReleaseDC ( NULL, hDC );

 return ( Aspect_Handle )hBitmap;

}  // end WNT_ImageManager :: Scale
//***//
//******************************** HashCode ******************************//
//***//
Standard_Integer WNT_ImageManager :: StringHashCode (
                                      const Standard_CString aString
                                     ) {

 Standard_Integer i, n, aHashCode = 0;

 union {
 
  char charPtr[ 80 ];
  int  intPtr[ 20 ];

 } u;

 n = (Standard_Integer) strlen ( aString );

 if ( n > 0 ) {

  if ( n < 80 ) {

   n = ( n + 3 ) / 4;
   u.intPtr[ n - 1 ] = 0;
   strcpy ( u.charPtr, aString );

  } else {

   n = 20;
   strncpy ( u.charPtr, aString, 80 );
    
  }  // end else

  for ( i = 0; i < n; ++i )

   aHashCode = aHashCode ^ u.intPtr[ i ];

 }  // end if

 return Abs ( aHashCode ) + 1;

}  // end WNT_ImageManager :: HashCode
//***//
//**************************** ImageHandle *******************************//
//***//
Aspect_Handle WNT_ImageManager :: ImageHandle (
                                   const Standard_Integer anIndex
                                  ) {

 if ( myLastIndex == anIndex ) return (  ( PWNT_Bitmap )myLastImage -> myImage  ) -> hBmp;

 myLastIndex = anIndex;
 myLastImage = anIndex < 0 ? myTrash.Value ( -anIndex ) : myImages.Value ( anIndex );

 return (  ( PWNT_Bitmap )myLastImage -> myImage  ) -> hBmp;

}  // end WNT_ImageManager :: ImageHandle
//***//
//******************************** Dim ***********************************//
//***//
void WNT_ImageManager :: Dim (
                          const Standard_Integer  anIndex,
                                Standard_Integer& aWidth,
                                Standard_Integer& aHeight
                         ) {

 BITMAP bmp;

 if ( myLastIndex != anIndex ) {

  myLastIndex = anIndex;
  myLastImage = anIndex < 0 ? myTrash.Value ( -anIndex ) : myImages.Value ( anIndex );

 }  // end if

 GetObject (
  (  ( PWNT_Bitmap )myLastImage -> myImage  ) -> hBmp,
  sizeof ( BITMAP ), ( LPVOID )&bmp
 );

 aWidth  = bmp.bmWidth;
 aHeight = bmp.bmHeight;

}  // WNT_ImageManager :: Dim
//***//
//****************************** HashCode ********************************//
//***//
Standard_Integer WNT_ImageManager :: HashCode (
                                      const Standard_Integer anIndex
                                     ) {

 if ( myLastIndex == anIndex ) return myLastImage -> myHashCode;

 myLastIndex = anIndex;
 myLastImage = anIndex < 0 ? myTrash.Value ( -anIndex ) : myImages.Value ( anIndex );

 return myLastImage -> myHashCode;

}  // end WNT_ImageManager :: HashCode
//***//
//****************************** Index ***********************************//
//***//
Standard_Integer WNT_ImageManager :: Index (
                   const Standard_Integer aHashCode
                  ) {

 Standard_Integer retVal = 0;

 if ( myLastImage -> myHashCode == aHashCode ) return myLastIndex;

 for ( int i = 1; i <= myImages.Length (); ++i )

  if (  myImages.Value ( i ) -> myHashCode == aHashCode  ) {
  
   myLastImage = myImages.Value ( retVal = myLastIndex = i );
   break;
  
  }  // end if

 if ( retVal == 0 )
  
 for ( int i = 1; i <= myTrash.Length (); ++i )

  if (  myTrash.Value ( i ) -> myHashCode == aHashCode  ) {
  
   myLastImage = myImages.Value ( i );
   retVal = myLastIndex = -i;
   break;
  
  }  // end if

 return retVal;

}  // end WNT_ImageManager :: Index
//***//
//****************************** Delete **********************************//
//***//
void WNT_ImageManager :: Delete ( const Standard_Integer anIndex ) {

 myImages.Remove ( anIndex );

 if ( myLastIndex == anIndex ) myLastIndex = 0;

}  // end WNT_ImageManager :: Delete
//***//
//****************************** Delete **********************************//
//***//
void WNT_ImageManager :: Discard ( const Standard_Integer anIndex ) {

 if ( anIndex > 0 ) {

  int len = myTrash.Length ();

  if ( len == TRASH_SIZE ) myTrash.Remove ( len );

  myTrash.Prepend (  myImages.Value ( anIndex )  );

  myImages.Remove ( anIndex );

  if ( myLastIndex == anIndex ) myLastIndex = 0;

 }  // end if

}  // end WNT_ImageManager :: Delete
//***//
//******************************** Size **********************************//
//***//
Standard_Integer WNT_ImageManager :: Size () const {

 return myImages.Length ();

}  // end WNT_ImageManager :: Size
//***//
//******************************** Add ***********************************//
//***//
void WNT_ImageManager :: Add ( const Handle_WNT_Image& anImage ) {

 myImages.Append ( myLastImage = anImage );

 myLastIndex = myImages.Length ();

}  // end WNT_ImageManager :: Add
//***//
//******************************** Image *********************************//
//***//
Handle( WNT_Image ) WNT_ImageManager :: Image (
                                         const Standard_Integer anIndex
                                        ) {

 if ( myLastIndex == anIndex ) return myLastImage;

 myLastIndex = anIndex;
 myLastImage = anIndex < 0 ? myTrash.Value ( -anIndex ) : myImages.Value ( anIndex ); 

 return myLastImage;

}  // end WNT_ImageManager :: Image
//***//
//******************************** Open **********************************//
//***//
Standard_Integer WNT_ImageManager :: Open (
                                      const Aspect_Handle    aDC,
                                      const Standard_Integer aWidth,
                                      const Standard_Integer aHeight,
                                      const Standard_Integer aHashCode
                                     ) {

 HDC     hdc  = ( HDC )aDC;
 HBITMAP hBmp = CreateCompatibleBitmap ( hdc, aWidth, aHeight );

 myImages.Append (  myLastImage = new WNT_Image ( hBmp, aHashCode )  );

 return myLastIndex = myImages.Length ();
                                     
}  // end WNT_ImageMagager :: Open
//***//
//************************************************************************//
