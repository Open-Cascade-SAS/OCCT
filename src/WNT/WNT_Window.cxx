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
//                TCL              at October 2000 (G002)
//                new method SetBackground(CString aName)

// include windows.h first to have all definitions available
#include <windows.h>

//                GG 07/03/00 Add MMSize() method

#include <WNT_Window.ixx>

#include <Image_PixMap.hxx>
#include <Aspect_Convert.hxx>

#include <stdio.h>
extern "C" void _debug_break ( char* );
//************************************************************************//
//***//
// callback function to manage window's background
extern LRESULT CALLBACK WNT_WndProc (
                         HWND, UINT, WPARAM, LPARAM
                        );
//***//
#define SELECT_PALETTE(hDC)                                  \
 HPALETTE hOldPal;                                           \
                                                             \
  if ( myExtraData.hPal  ) {                                 \
                                                             \
   hOldPal = SelectPalette ( hDC, myExtraData.hPal, FALSE ); \
                                                             \
   if (  RealizePalette ( hDC )  )                           \
                                                             \
    UpdateColors ( hDC );                                    \
                                                             \
  }  // end if
//***//
#define UNSELECT_PALETTE(hDC)            \
 if ( myExtraData.hPal )                 \
                                         \
  SelectPalette ( hDC, hOldPal, FALSE );

HBITMAP LoadImageFromFile (  Handle( WNT_GraphicDevice )&, char*, HDC = NULL  );

//***//
//************************* Constructor **********************************//
//***//
// TODO - Here is enough of duplicated code in 1st and second constructors!
WNT_Window::WNT_Window (const Handle(WNT_GraphicDevice)& theDevice,
                        const Standard_CString           theTitle,
                        const Handle(WNT_WClass)&        theClass,
                        const WNT_Dword&                 theStyle,
                        const Quantity_Parameter         theXc,
                        const Quantity_Parameter         theYc,
                        const Quantity_Parameter         theWidth,
                        const Quantity_Parameter         theHeight,
                        const Quantity_NameOfColor       theBackColor,
                        const Aspect_Handle              theParent,
                        const Aspect_Handle              theMenu,
                        const Standard_Address           theClientStruct)
: Aspect_Window (theDevice),
  myWClass (theClass)
{
  ZeroMemory (&myExtraData, sizeof (WNT_WindowData));
  DWORD dwStyle = theStyle;
  const Handle(WNT_GraphicDevice)& aDev = Handle(WNT_GraphicDevice)::DownCast (MyGraphicDevice);

  if (theXc < 0.0 || theXc > 1.0 || theYc < 0.0 || theYc > 1.0 ||
      theWidth <= 0.0 || theHeight <= 0.0)
  {
    Aspect_WindowDefinitionError::Raise ("Coordinate(s) out of range");
  }

  Standard_Integer aParentSizeX, aParentSizeY;
  if (theParent)
  {
    RECT aRect;
    GetClientRect ((HWND )theParent, &aRect);
    aParentSizeX = aRect.right;
    aParentSizeY = aRect.bottom;
  }
  else
  {
    aDev->DisplaySize (aParentSizeX, aParentSizeY);
  }

  Standard_Integer aPxWidth, aPxHeight;
  Aspect_Convert::ConvertCoordinates (aParentSizeX, aParentSizeY,
                                      theXc, theYc, theWidth, theHeight,
                                      aXLeft, aYTop, aPxWidth, aPxHeight);
  Aspect_Convert::FitIn (aParentSizeX, aParentSizeY,
                         aXLeft, aYTop, aPxWidth, aPxHeight);
  aXRight  = aXLeft + aPxWidth;
  aYBottom = aYTop + aPxHeight;

  if (theParent && !(theStyle & WS_CHILD))
  {
    dwStyle |= WS_CHILD | WS_CLIPSIBLINGS;
  }
  else if (!theParent && !(theStyle & WS_CLIPCHILDREN))
  {
    dwStyle |= WS_CLIPCHILDREN;
  }

  if (dwStyle & WS_OVERLAPPEDWINDOW)
  {
    // include decorations in the window dimensions
    // to reproduce same behaviour of Xw_Window.
    aXLeft   -= GetSystemMetrics(SM_CXSIZEFRAME);
    aYTop    -= GetSystemMetrics(SM_CYSIZEFRAME) + GetSystemMetrics(SM_CYCAPTION);
    aXRight  += GetSystemMetrics(SM_CXSIZEFRAME);
    aYBottom += GetSystemMetrics(SM_CYSIZEFRAME);
  }

  myHWindow = CreateWindow (
              myWClass->Name(),                 // window's class name
              theTitle,                         // window's name
              dwStyle,                          // window's style
              aXLeft, aYTop,                    // window's coordinates
              (aXRight - aXLeft), (aYBottom - aYTop),
              (HWND )theParent,                 // window's parent
              (HMENU )theMenu,                    // window's menu
              (HINSTANCE )myWClass->Instance(), // application's instance
              theClientStruct);                 // pointer to CLIENTCREATESTRUCT
  if (!myHWindow)
  {
    Aspect_WindowDefinitionError::Raise ("Unable to create window");
  }

  myDoubleBuffer  = Standard_False;
  myHPixmap       = 0;
  myHParentWindow = theParent;
  myWndProc       = myWClass->WndProc();
  SetBackground (theBackColor);
  SetWindowLongPtr ((HWND )myHWindow, GWLP_WNDPROC, (LONG_PTR )WNT_WndProc);
  myUsrData = (Standard_Address )SetWindowLongPtr ((HWND )myHWindow, GWLP_USERDATA, (LONG_PTR )&myExtraData);

  myExtraData.WNT_Window_Ptr = (void* )this;
  myExtraData.hPal           = (HPALETTE )aDev->HPalette();

  myImages = new WNT_ImageManager (this);
  myFormat = WNT_TOI_XWD;

	SetFlags(WDF_NOERASEBKGRND);
}

WNT_Window::WNT_Window (const Handle(WNT_GraphicDevice)& theDevice,
                        const Standard_CString           theTitle,
                        const Handle(WNT_WClass)&        theClass,
                        const WNT_Dword&                 theStyle,
                        const Standard_Integer           thePxLeft,
                        const Standard_Integer           thePxTop,
                        const Standard_Integer           thePxWidth,
                        const Standard_Integer           thePxHeight,
                        const Quantity_NameOfColor       theBackColor,
                        const Aspect_Handle              theParent,
                        const Aspect_Handle              theMenu,
                        const Standard_Address           theClientStruct)
: Aspect_Window (theDevice),
  aXLeft (thePxLeft),
  aYTop (thePxTop),
  aXRight (thePxLeft + thePxWidth),
  aYBottom (thePxTop + thePxHeight),
  myWClass (theClass)
{
  ZeroMemory (&myExtraData, sizeof (WNT_WindowData));
  DWORD dwStyle = theStyle;
  const Handle(WNT_GraphicDevice)& aDev = Handle(WNT_GraphicDevice)::DownCast (MyGraphicDevice);

  if (thePxWidth <= 0 || thePxHeight <= 0)
  {
    Aspect_WindowDefinitionError::Raise ("Coordinate(s) out of range");
  }

  if (theParent && !(theStyle & WS_CHILD))
  {
    dwStyle |= WS_CHILD | WS_CLIPSIBLINGS;
  }
  else if (!theParent && !(theStyle & WS_CLIPCHILDREN))
  {
    dwStyle |= WS_CLIPCHILDREN;
  }

  if (dwStyle & WS_OVERLAPPEDWINDOW)
  {
    // include decorations in the window dimensions
    // to reproduce same behaviour of Xw_Window.
    aXLeft   -= GetSystemMetrics(SM_CXSIZEFRAME);
    aYTop    -= GetSystemMetrics(SM_CYSIZEFRAME) + GetSystemMetrics(SM_CYCAPTION);
    aXRight  += GetSystemMetrics(SM_CXSIZEFRAME);
    aYBottom += GetSystemMetrics(SM_CYSIZEFRAME);
  }

  myHWindow = CreateWindow (
              myWClass->Name(),                 // window's class name
              theTitle,                         // window's name
              dwStyle,                          // window's style
              aXLeft, aYTop,                    // window's coordinates
              (aXRight - aXLeft), (aYBottom - aYTop),
              (HWND )theParent,                 // window's parent
              (HMENU )theMenu,                  // window's menu
              (HINSTANCE )myWClass->Instance(), // application's instance
              theClientStruct);                 // pointer to CLIENTCREATESTRUCT
  if (!myHWindow)
  {
    Aspect_WindowDefinitionError::Raise ("Unable to create window");
  }

  myDoubleBuffer  = Standard_False;
  myHPixmap       = 0;
  myHParentWindow = theParent;
  myWndProc       = myWClass->WndProc();
  SetBackground (theBackColor);
  SetWindowLongPtr ((HWND )myHWindow, GWLP_WNDPROC, (LONG_PTR )WNT_WndProc);
  myUsrData = (Standard_Address )SetWindowLongPtr ((HWND )myHWindow, GWLP_USERDATA, (LONG_PTR )&myExtraData);

  myExtraData.WNT_Window_Ptr = (void* )this;
  myExtraData.hPal = (HPALETTE )aDev->HPalette();

  myImages = new WNT_ImageManager (this);
  myFormat = WNT_TOI_XWD;

	SetFlags (WDF_NOERASEBKGRND);
}

//***//
//************************* Constructor **********************************//
//***//
WNT_Window :: WNT_Window (
               const Handle( WNT_GraphicDevice )& aDevice,
               const Aspect_Handle                aHandle,
               const Quantity_NameOfColor         aBackColor
              ) : Aspect_Window ( aDevice ) {

 doCreate ( aDevice, aHandle, aBackColor );

	/* Bug OCC20596 */
	SetFlags(WDF_NOERASEBKGRND);

}  // end constructor
//***//
//************************* Constructor **********************************//
//***//
WNT_Window :: WNT_Window (
               const Handle( WNT_GraphicDevice )& aDevice,
               const Standard_Integer             aPart1,
               const Standard_Integer             aPart2,
               const Quantity_NameOfColor         aBackColor
              ) : Aspect_Window ( aDevice ) {

 Aspect_Handle aHandle = ( Aspect_Handle )(  ( aPart1 << 16 ) + aPart2  );

 doCreate ( aDevice, aHandle, aBackColor );

 	/* Bug OCC20596 */
	SetFlags(WDF_NOERASEBKGRND);

}  // end constructor
//***//
//***************************** Destroy **********************************//
//***//
void WNT_Window :: Destroy () {

 if ( MyHBackground ) DeleteObject ( MyHBackground );  // delete background brush
 if ( myHPixmap     ) DeleteObject ( myHPixmap     );  // . . . and double buffer pixmap if exists

 if ( myHWindow ) {

  if (myUsrData != Standard_Address(-1))
  {
    SetWindowLongPtr ((HWND )myHWindow, GWLP_USERDATA, (LONG_PTR )myUsrData);
  }

  if (myWndProc != NULL)
  {
   SetWindowLongPtr ((HWND )myHWindow, GWLP_WNDPROC, (LONG_PTR )myWndProc);
  }

  if (  !( myExtraData.dwFlags & WDF_FOREIGN )  )

   DestroyWindow (  ( HWND )myHWindow  );

 }  // end if

}  // end WNT_Window :: Destroy
//***//
//************************ SetBackground (1) *****************************//
//***//
void WNT_Window :: SetBackground ( const Aspect_Background& Background ) {

 SetBackground (  Background.Color ().Name ()  );

}  // end WNT_Window :: SetBackground
//***//
//************************ SetBackground (2) *****************************//
//***//
void WNT_Window :: SetBackground ( const Quantity_NameOfColor BackColor ) {

 const Handle( WNT_GraphicDevice )& dev = Handle( WNT_GraphicDevice ) ::
                                           DownCast ( MyGraphicDevice );

 if ( MyHBackground )
   DeleteObject ( MyHBackground );

 MyBackground.SetColor ( BackColor );
 MyHBackground = CreateSolidBrush (
                  dev -> SetColor (  MyBackground.Color ()  )
                 );

}  // end WNT_Window :: SetBackground
//***//
//************************ SetBackground (3) *****************************//
//***//
void WNT_Window :: SetBackground ( const Aspect_Handle aBackPixmap ) {

 Aspect_Handle bitmap = CreatePatternBrush (  ( HBITMAP )aBackPixmap  );

 if( bitmap ) {
   if ( MyHBackground ) DeleteObject ( MyHBackground );
   MyHBackground = bitmap;
   MyBackgroundImage.Clear();
   MyBackgroundFillMethod = Aspect_FM_NONE;
 }

}  // end WNT_Window :: SetBackground
//***//
//************************ SetBackground (4) *****************************//
//***//
Standard_Boolean WNT_Window::SetBackground( const Standard_CString aName,
                                const Aspect_FillMethod aMethod ) {
 Standard_Boolean status = Standard_False;

 if( !aName ) return Standard_False;

 Handle( WNT_GraphicDevice ) gdev =
     *( ( Handle_WNT_GraphicDevice * )& MyGraphicDevice );
 if ( MyHBackground )
   DeleteObject( MyHBackground );
 MyHBackground = 0;

 HBITMAP hBmp = LoadImageFromFile( gdev, (Standard_PCharacter)aName );

 if( hBmp ) {
   status = Standard_True;
   MyBackgroundImage = aName;
   MyBackgroundFillMethod = aMethod;
   MyHBackground = CreatePatternBrush(  hBmp  );
 }

 return status;
}  // end WNT_Window :: SetBackground
    //***//
//************************ SetBackground (5) *****************************//
//***//
void WNT_Window::SetBackground( const Quantity_Color& color ) {

 const Handle( WNT_GraphicDevice )& dev =
   Handle( WNT_GraphicDevice )::DownCast ( MyGraphicDevice );

 if ( MyHBackground )
   DeleteObject( MyHBackground );

 MyBackground.SetColor( color );
 MyHBackground = CreateSolidBrush( dev->SetColor( color ) );

}  // end WNT_Window :: SetBackground

//************************ SetBackground (6) *****************************//
//***//
void WNT_Window::SetBackground (const Aspect_GradientBackground& GrBackground) {

  Quantity_Color Color1, Color2;
  GrBackground.Colors(Color1,Color2);
  SetBackground(Color1,Color2, GrBackground.BgGradientFillMethod());

} // end WNT_Window :: SetBackground

//************************ SetBackground (7) *****************************//
//***//
void WNT_Window::SetBackground( const Quantity_Color& color1,
                                const Quantity_Color& color2,
                                const Aspect_GradientFillMethod aMethod){

  Standard_Integer bcolor = Standard_Integer( color1.Name() ) + Standard_Integer( color2.Name() );
  if(  ( bcolor >= 0 ) )
    MyGradientBackground.SetColors( color1, color2, aMethod ) ;
}  // end WNT_Window :: SetBackground

//**************************** SetCursor *********************************//
//***//
void WNT_Window :: SetCursor ( const Aspect_Handle aCursor ) const {

#ifndef _WIN64
 SetClassLong (  ( HWND )myHWindow, GCL_HCURSOR, ( LONG )aCursor  );
#else
 SetClassLong (  ( HWND )myHWindow, GCLP_HCURSOR, ( LONG )aCursor  );
#endif

}  // end WNT_Window :: SetCursor
//***//
//**************************** SetIcon ***********************************//
//***//
void WNT_Window :: SetIcon (
                    const Aspect_Handle    anIcon,
                    const Standard_CString aName
                   ) {

 if (  !myIcon.IsNull ()  )

  myIcon.Nullify ();

 myIcon = new WNT_Icon ( aName, anIcon, 0 );

}  // end WNT_Window :: SetIcon
//***//
//**************************** SetIconName *******************************//
//***//
void WNT_Window :: SetIconName ( const Standard_CString aName ) {

 myIcon -> SetName ( aName );

}  // end WNT_Window :: SetIconName
//***//
//************************** SetDoubleBuffer *****************************//
//***//
void WNT_Window :: SetDoubleBuffer ( const Standard_Boolean DBmode ) {

 if ( DBmode )

  if (  !BackingStore ()  )

   Aspect_WindowError :: Raise ( "Unable to set double buffer" );

  else {

   myDoubleBuffer   = Standard_True;
   myExtraData.hBmp     = ( HBITMAP )myHPixmap;

  }  // end else

 else {

  myDoubleBuffer   = Standard_False;
  myExtraData.hBmp = NULL;

 }  // end else

}  // end WNT_Window :: SetDoubleBuffer
//***//
//***************************** Flush ************************************//
//***//
void WNT_Window :: Flush () const {

 if ( myDoubleBuffer ) Restore ();

}  // end WNT_Window :: Flush
//***//
//***************************** IsMapped *********************************//
//***//
Standard_Boolean WNT_Window :: IsMapped () const {
  if (IsVirtual()) {
    return Standard_True;
  }

  WINDOWPLACEMENT wp;

  wp.length = sizeof ( WINDOWPLACEMENT );
  GetWindowPlacement (  ( HWND )myHWindow, &wp  );

  return !(  wp.showCmd == SW_HIDE || wp.showCmd == SW_MINIMIZE );
}  // WNT_Window :: IsMapped
//***//
//***************************** Map (1) **********************************//
//***//
void WNT_Window :: Map () const {
  if (IsVirtual()) {
    return;
  }
  Map ( SW_SHOW );
}  // end WNT_Window :: Map
//***//
//***************************** Map (2) **********************************//
//***//
void WNT_Window :: Map ( const Standard_Integer aMapMode ) const {
  if (IsVirtual()) {
    return;
  }
  ShowWindow (  ( HWND )myHWindow, aMapMode  );
  UpdateWindow (  ( HWND )myHWindow  );

}  // end WNT_Window :: Map
//***//
//**************************** Unmap *************************************//
//***//
void WNT_Window :: Unmap () const {

 Map ( SW_HIDE );

}  // end WNT_Window :: Unmap
//***//
//**************************** DoResize **********************************//
//***//
Aspect_TypeOfResize WNT_Window :: DoResize () const {

 int                 mask = 0;
 Aspect_TypeOfResize mode = Aspect_TOR_UNKNOWN;
 WINDOWPLACEMENT     wp;

 wp.length = sizeof ( WINDOWPLACEMENT );
 GetWindowPlacement (  ( HWND )myHWindow, &wp  );

 if ( wp.showCmd != SW_SHOWMINIMIZED ) {

  if (  Abs ( wp.rcNormalPosition.left   - aXLeft   ) > 2  ) mask |= 1;
  if (  Abs ( wp.rcNormalPosition.right  - aXRight  ) > 2  ) mask |= 2;
  if (  Abs ( wp.rcNormalPosition.top    - aYTop    ) > 2  ) mask |= 4;
  if (  Abs ( wp.rcNormalPosition.bottom - aYBottom ) > 2  ) mask |= 8;

  switch ( mask ) {

   case 0:

     mode = Aspect_TOR_NO_BORDER;

    break;

   case 1:

     mode = Aspect_TOR_LEFT_BORDER;

    break;

   case 2:

     mode = Aspect_TOR_RIGHT_BORDER;

    break;

   case 4:

     mode = Aspect_TOR_TOP_BORDER;

    break;

   case 5:

     mode = Aspect_TOR_LEFT_AND_TOP_BORDER;

    break;

   case 6:

     mode = Aspect_TOR_TOP_AND_RIGHT_BORDER;

    break;

   case 8:

     mode = Aspect_TOR_BOTTOM_BORDER;

    break;

   case 9:

     mode = Aspect_TOR_BOTTOM_AND_LEFT_BORDER;

    break;

   case 10:

     mode = Aspect_TOR_RIGHT_AND_BOTTOM_BORDER;

    break;

   default:

    break;

  }  // end switch

  *(  ( Standard_Integer* )&aXLeft   ) = wp.rcNormalPosition.left;
  *(  ( Standard_Integer* )&aXRight  ) = wp.rcNormalPosition.right;
  *(  ( Standard_Integer* )&aYTop    ) = wp.rcNormalPosition.top;
  *(  ( Standard_Integer* )&aYBottom ) = wp.rcNormalPosition.bottom;

  if (  myDoubleBuffer  ) {

   DeleteObject ( myHPixmap );
   *(  ( Aspect_Handle* )&myHPixmap  ) = 0;

   if (  !BackingStore ()  )

    Aspect_WindowError :: Raise ( "WNT_Window::DoResize (): Unable to set double buffer" );

  }  // end if

 }  // end if

 return mode;

}  // end WNT_Window :: DoResize

//***//
//**************************** DoMapping **********************************//
//***//
Standard_Boolean WNT_Window :: DoMapping () const {
// DO nothing on WNT.
  return Standard_True;
}

//***//
//**************************** Clear *************************************//
//***//
void WNT_Window :: Clear () const {

 RECT r;

 GetClientRect (  ( HWND )myHWindow, &r  );
 ClearArea ( r.right / 2, r.bottom / 2, r.right, r.bottom );

}  // end WNT_Window :: Clear
//***//
//**************************** ClearArea *********************************//
//***//
void WNT_Window :: ClearArea (
                    const Standard_Integer Xc,
                    const Standard_Integer Yc,
                    const Standard_Integer Width,
                    const Standard_Integer Height
                   ) const {

 RECT r;
 HDC  hDC = GetDC (  ( HWND )myHWindow  );

  r.left   = Xc - Width / 2;
  r.right  = r.left + Width;
  r.top    = Yc - Height / 2;
  r.bottom = r.top + Height;

  if ( myDoubleBuffer && myHPixmap ) {

   HDC hDCmem = CreateCompatibleDC ( hDC );

    SELECT_PALETTE( hDCmem );

     SelectObject ( hDCmem, myHPixmap );
     FillRect (  hDCmem, &r, ( HBRUSH )MyHBackground  );

        UNSELECT_PALETTE( hDCmem );

   DeleteDC ( hDCmem );

  } else {

   SELECT_PALETTE( hDC );

    FillRect (  hDC, &r, ( HBRUSH )MyHBackground  );

   UNSELECT_PALETTE( hDC );

  }  // end else

 ReleaseDC (  ( HWND )myHWindow, hDC  );

}  // end WNT_Window :: ClearArea
//***//
//**************************** Restore ***********************************//
//***//
void WNT_Window :: Restore () const {

 RECT r;

 if ( myHPixmap ) {

  GetClientRect (  ( HWND )myHWindow, &r  );
  RestoreArea (
   r.right / 2, r.bottom - r.bottom / 2, r.right, r.bottom
  );

 }  // end if

}  // end WNT_Window :: Restore
//***//
//************************* Restore Area *********************************//
//***//
void WNT_Window :: RestoreArea (
                    const Standard_Integer Xc,
                    const Standard_Integer Yc,
                    const Standard_Integer Width,
                    const Standard_Integer Height
                   ) const {

 HDC hDC, hDCmem;
 int x, y;

 if ( myHPixmap ) {

  hDC     = GetDC (  ( HWND )myHWindow  );
   hDCmem = CreateCompatibleDC ( hDC );

    x = Xc - Width / 2;
    y = Yc - Height / 2;

    SelectObject ( hDCmem, myHPixmap );
    BitBlt ( hDC, x, y, Width, Height, hDCmem, x, y, SRCCOPY );

   DeleteDC ( hDCmem );
  ReleaseDC (  ( HWND )myHWindow, hDC  );

 }  // end if

}  // end WNT_Window :: RestoreArea
//***//
//****************************** Dump ************************************//
//***//
Standard_Boolean WNT_Window::Dump (const Standard_CString theFilename,
                                   const Standard_Real theGammaValue) const
{
  return ToPixMap()->Dump (theFilename, theGammaValue);
}  // end WNT_Window :: Dump
//***//
//*************************** DumpArea ***********************************//
//***//
Standard_Boolean WNT_Window::DumpArea (const Standard_CString theFilename,
                                       const Standard_Integer Xc, const Standard_Integer Yc,
                                       const Standard_Integer Width, const Standard_Integer Height,
                                       const Standard_Real aGammaValue) const
{
// *** Gamma correction must be implemented also under WNT ...
 Standard_Integer x = Xc - Width  / 2;
 Standard_Integer y = Yc - Height / 2;
 return (myDoubleBuffer) ?
         myImages->SaveBuffer (theFilename, x, y, Width, Height) :
         myImages->Save       (theFilename, x, y, Width, Height);

}  // end WNT_Window :: DumpArea
//***//

static Handle(Image_PixMap) ConvertBitmap (HBITMAP theHBitmap)
{
  Handle(Image_PixMap) anImagePixMap;
  // Copy data from HBITMAP
  BITMAP aBitmap;

  // Get informations about the bitmap
  GetObject (theHBitmap, sizeof(BITMAP), (LPSTR )&aBitmap);
  Standard_Integer aWidth  = aBitmap.bmWidth;
  Standard_Integer aHeight = aBitmap.bmHeight;

  // Setup image data
  BITMAPINFOHEADER aBitmapInfo;
  memset (&aBitmapInfo, 0, sizeof(BITMAPINFOHEADER));
  aBitmapInfo.biSize = sizeof(BITMAPINFOHEADER);
  aBitmapInfo.biWidth = aWidth;
  aBitmapInfo.biHeight = aHeight; // positive means bottom-up!
  aBitmapInfo.biPlanes = 1;
  aBitmapInfo.biBitCount = 32;
  aBitmapInfo.biCompression = BI_RGB;

  Standard_Integer aBytesPerLine = aWidth * 4;
  Standard_Byte* aDataPtr = new Standard_Byte[aBytesPerLine * aHeight];

  // Copy the pixels
  HDC aDC = GetDC (NULL);
  Standard_Boolean isSuccess
    = GetDIBits (aDC,             // handle to DC
                 theHBitmap,      // handle to bitmap
                 0,               // first scan line to set
                 aHeight,         // number of scan lines to copy
                 aDataPtr,        // array for bitmap bits
                 (LPBITMAPINFO )&aBitmapInfo, // bitmap data info
                 DIB_RGB_COLORS   // RGB
                 ) != 0;

  if (isSuccess)
  {
    anImagePixMap = new Image_PixMap (aDataPtr,
                                      aWidth, aHeight,
                                      aBytesPerLine,
                                      aBitmapInfo.biBitCount,
                                      Standard_False); // bottom-up!
  }
  // Release dump memory
  delete[] aDataPtr;
  ReleaseDC (NULL, aDC);
  return anImagePixMap;
}

Handle(Aspect_PixMap) WNT_Window::ToPixMap() const
{
  if (myDoubleBuffer && myHPixmap)
  {
    return ConvertBitmap ((HBITMAP )myHPixmap);
  }

  RECT aRect;
  GetClientRect ((HWND )myHWindow, &aRect);
  int aWidth  = aRect.right  - aRect.left;
  int aHeight = aRect.bottom - aRect.top;

  // Prepare the DCs
  HDC aDstDC = GetDC (NULL);
  HDC aSrcDC = GetDC ((HWND )myHWindow); // we copy only client area
  HDC aMemDC = CreateCompatibleDC (aDstDC);

  // Copy the screen to the bitmap
  HBITMAP anHBitmapDump = CreateCompatibleBitmap (aDstDC, aWidth, aHeight);
  HBITMAP anHBitmapOld = (HBITMAP )SelectObject (aMemDC, anHBitmapDump);
  BitBlt (aMemDC, 0, 0, aWidth, aHeight, aSrcDC, 0, 0, SRCCOPY);

  Handle(Image_PixMap) anImagePixMap = ConvertBitmap (anHBitmapDump);

  // Free objects
  DeleteObject (SelectObject (aMemDC, anHBitmapOld));
  DeleteDC (aMemDC);

  return anImagePixMap;
}

//****************************** Load ************************************//
//***//
Standard_Boolean WNT_Window :: Load ( const Standard_CString aFilename ) const {

 RECT r;

 GetClientRect (  ( HWND )myHWindow, &r  );

 return LoadArea ( aFilename, r.right / 2, r.bottom / 2, r.right, r.bottom );

}  // end WNT_Window :: Load
//***//
//*************************** LoadArea ***********************************//
//***//
Standard_Boolean WNT_Window :: LoadArea (
                                const Standard_CString aFilename,
                                const Standard_Integer Xc,
                                const Standard_Integer Yc,
                                const Standard_Integer Width,
                                const Standard_Integer Height
                               ) const {

 Standard_Integer idx;
 Standard_Boolean status;

 status = (  idx = myImages -> Load ( aFilename )  ) ? Standard_True : Standard_False;

 if ( status ) myImages -> Draw ( idx, Xc, Yc, Width, Height );

 return status;

}  // end WNT_Window :: LoadArea
//***//
//*************************** BackingStore *******************************//
//***//
Standard_Boolean WNT_Window :: BackingStore () const {

 Standard_Boolean retVal = Standard_False;;
 HDC              hDC;
 RECT             r;

 if ( myHPixmap )

  retVal = Standard_True;

 else {

  hDC = GetDC (  ( HWND )myHWindow  );

   GetClientRect (  ( HWND )myHWindow, &r  );
   *(  ( Aspect_Handle* )&myHPixmap  ) = CreateCompatibleBitmap ( hDC, r.right, r.bottom );

   if ( myHPixmap ) {

    HDC hDCmem;

         hDCmem = CreateCompatibleDC ( hDC );

       SELECT_PALETTE( hDCmem );

            SelectObject ( hDCmem, myHPixmap );
        FillRect (  hDCmem, &r, ( HBRUSH )MyHBackground  );

       UNSELECT_PALETTE( hDCmem );

         DeleteDC ( hDCmem );

    retVal = Standard_True;

   }  // end if

  ReleaseDC (  ( HWND )myHWindow, hDC  );

 }  // end else

 return retVal;

}  // end WNT_Window :: BackingStore
//***//
//******************************* Ratio **********************************//
//***//
Quantity_Ratio WNT_Window :: Ratio () const {

 RECT r;

 GetClientRect (  ( HWND )myHWindow, &r  );

 return ( Quantity_Ratio )(  ( Quantity_Ratio )r.right / ( Quantity_Ratio )r.bottom  );

}  // end WNT_Window :: Ratio
//***//
//**************************** Position (1) ******************************//
//***//
void WNT_Window :: Position (
                    Quantity_Parameter& X1, Quantity_Parameter& Y1,
                    Quantity_Parameter& X2, Quantity_Parameter& Y2
                   ) const {

 POINT ptl, ptr;
 RECT  pr, cr;
 const Handle( WNT_GraphicDevice )& dev = Handle( WNT_GraphicDevice ) ::
                                           DownCast ( MyGraphicDevice );

 if ( myHParentWindow )

  GetClientRect (  ( HWND )myHParentWindow, &pr  );

 else {

  int w, h;

  dev -> DisplaySize ( w, h );

  pr.right  = w;
  pr.bottom = h;

 }  // end else

 GetClientRect (  ( HWND )myHWindow, &cr  );

 ptl.x = ptl.y = 0;
 ClientToScreen (  ( HWND )myHWindow, &ptl  );
 cr.left = ptl.x;
 cr.top  = ptl.y;

 ptr.x = cr.right;
 ptr.y = cr.bottom;
 ClientToScreen (  ( HWND )myHWindow, &ptr  );
 cr.right  = ptr.x;
 cr.bottom = ptr.y;

 if ( myHParentWindow ) {

  ScreenToClient (      ( HWND )myHParentWindow, &ptl  );
  ScreenToClient (  ( HWND )myHParentWindow, &ptr  );
  cr.left   = ptl.x;
  cr.top    = ptl.y;
  cr.right  = ptr.x;
  cr.bottom     = ptr.y;

 }  // end if

 X1 = ( Quantity_Parameter )cr.left   / ( Quantity_Parameter )pr.right;
 X2 = ( Quantity_Parameter )cr.right  / ( Quantity_Parameter )pr.right;
 Y1 = 1. - ( Quantity_Parameter )cr.top    / ( Quantity_Parameter )pr.bottom;
 Y2 = 1. - ( Quantity_Parameter )cr.bottom / ( Quantity_Parameter )pr.bottom;

}  // end WNT_Window :: Position
//***//
//**************************** Position (2) ******************************//
//***//
void WNT_Window :: Position (
                    Standard_Integer& X1, Standard_Integer& Y1,
                    Standard_Integer& X2, Standard_Integer& Y2
                   ) const {

 POINT ptl, ptr;
 RECT  r;

 GetClientRect (  ( HWND )myHWindow, &r  );

 ptl.x = ptl.y = 0;
 ClientToScreen (  ( HWND )myHWindow, &ptl  );
 ptr.x = r.right;
 ptr.y = r.bottom;
 ClientToScreen (  ( HWND )myHWindow, &ptr  );

 if ( myHParentWindow ) {

  ScreenToClient (      ( HWND )myHParentWindow, &ptl  );
  ScreenToClient (  ( HWND )myHParentWindow, &ptr  );

 }  // end if

 X1 = ptl.x;
 X2 = ptr.x;
 Y1 = ptl.y;
 Y2 = ptr.y;

}  // end WNT_Window :: Position
//***//
//******************************* Size (1) *******************************//
//***//
void WNT_Window :: Size (
                    Quantity_Parameter& Width, Quantity_Parameter& Height
                   ) const {

 RECT pr, wr;
 const Handle( WNT_GraphicDevice )& dev = Handle( WNT_GraphicDevice ) ::
                                           DownCast ( MyGraphicDevice );

 if ( myHParentWindow )

  GetClientRect (  ( HWND )myHParentWindow, &pr  );

 else {

  int w, h;

  dev -> DisplaySize ( w, h );

  pr.right  = w;
  pr.bottom     = h;

 }  // end else

 GetClientRect (  ( HWND )myHWindow, &wr  );

 Width  = ( Quantity_Parameter )wr.right  / ( Quantity_Parameter )pr.right;
 Height = ( Quantity_Parameter )wr.bottom / ( Quantity_Parameter )pr.bottom;

}  // end WNT_Window :: Size
//***//
//******************************* Size (2) *******************************//
//***//
void WNT_Window :: Size (
                    Standard_Integer& Width, Standard_Integer& Height
                   ) const {

 RECT r;

 GetClientRect (  ( HWND )myHWindow, &r  );

 Width  = r.right;
 Height = r.bottom;

}  // end WNT_Window :: Size
//***//
//******************************* MMSize (1) *******************************//
//***//
void WNT_Window :: MMSize (
                    Standard_Real& Width, Standard_Real& Height
                   ) const {

 const Handle( WNT_GraphicDevice )& dev = Handle( WNT_GraphicDevice ) ::
                                           DownCast ( MyGraphicDevice );

 int w, h;
 dev -> DisplaySize ( w, h );
 Standard_Real mmw,mmh;
 dev -> DisplaySize ( mmw, mmh );
 Standard_Real mmratio = (mmw/w + mmh/h)/2.;

 RECT wr;
 GetClientRect (  ( HWND )myHWindow, &wr  );

 Width  = mmratio * wr.right;
 Height = mmratio * wr.bottom;

}  // end WNT_Window :: MMSize
//***//
//**************************** Convert (1) *******************************//
//***//
Quantity_Parameter WNT_Window :: Convert (
                                  const Standard_Integer PV
                                 ) const {

 int w, h;
 const Handle( WNT_GraphicDevice )& dev = Handle( WNT_GraphicDevice ) ::
                                           DownCast ( MyGraphicDevice );

 dev -> DisplaySize ( w, h );

 return ( Quantity_Parameter )(  PV / Min ( w, h )  );

}  // end WNT_Window :: Convert
//***//
//**************************** Convert (2) *******************************//
//***//
Standard_Integer WNT_Window :: Convert (
                                const Quantity_Parameter DV
                               ) const {

 int w, h;
 const Handle( WNT_GraphicDevice )& dev = Handle( WNT_GraphicDevice ) ::
                                           DownCast ( MyGraphicDevice );

 dev -> DisplaySize ( w, h );

 return ( Standard_Integer )(  DV * Min ( w, h )  );

}  // end WNT_Window :: Convert
//***//
//**************************** Convert (3) *******************************//
//***//

void WNT_Window :: Convert (
                    const Standard_Integer PX, const Standard_Integer PY,
                    Quantity_Parameter& DX, Quantity_Parameter& DY
                   ) const {

 int w, h;
 const Handle( WNT_GraphicDevice )& dev = Handle( WNT_GraphicDevice ) ::
                                           DownCast ( MyGraphicDevice );

 dev -> DisplaySize ( w, h );

 DX = ( Quantity_Parameter )PX / ( Quantity_Parameter )w;
 DY = 1. - ( Quantity_Parameter )PY / ( Quantity_Parameter )h;

}  // end WNT_Window :: Convert
//***//
//**************************** Convert (4) *******************************//
//***//
void WNT_Window :: Convert (
                    const Quantity_Parameter DX, const Quantity_Parameter DY,
                    Standard_Integer& PX, Standard_Integer& PY
                   ) const {

 int w, h;
 const Handle( WNT_GraphicDevice )& dev = Handle( WNT_GraphicDevice ) ::
                                           DownCast ( MyGraphicDevice );

 dev -> DisplaySize ( w, h );

 PX = ( Standard_Integer )(  DX * ( Quantity_Parameter )w           );
 PY = ( Standard_Integer )(  ( 1. - DY ) * ( Quantity_Parameter )h  );

}  // end WNT_Window :: Convert
//***//
//******************************* SetPos *********************************//
//***//
void WNT_Window :: SetPos (
                    const Standard_Integer X,  const Standard_Integer Y,
                    const Standard_Integer X1, const Standard_Integer Y1
                   ) {

 aXLeft   = X;
 aYTop    = Y;
 aXRight  = X1;
 aYBottom = Y1;

}  // end WNT_Window :: SetPos
//***//
//*************************** SetOutputFormat ****************************//
//***//
void WNT_Window :: SetOutputFormat ( const WNT_TypeOfImage aFormat ) {

 myFormat = aFormat;
 myImages -> SetFormat ( aFormat );

}  // end WNT_Window :: SetOutputFormat
//***//
//**************************** SetFlags **********************************//
//***//
void WNT_Window :: SetFlags ( const Standard_Integer aFlags ) {

 myExtraData.dwFlags |= aFlags;

}  // end WNT_Window :: SetFlags
//***//
//*************************** ResetFlags *********************************//
//***//
void WNT_Window :: ResetFlags ( const Standard_Integer aFlags ) {

 myExtraData.dwFlags &= ~aFlags;

}  // end WNT_Window :: ResetFlags
//***//
//*************************** doCreate **********************************//
//***//
void WNT_Window :: doCreate (
                    const Handle( WNT_GraphicDevice )& aDevice,
                    const Aspect_Handle                aHandle,
                    const Quantity_NameOfColor         aBackColor
                   ) {

 LONG            uData;
 WNDPROC         wProc;
 WINDOWPLACEMENT wp;

 const Handle( WNT_GraphicDevice )& dev = Handle( WNT_GraphicDevice ) ::
                                           DownCast ( MyGraphicDevice );

 ZeroMemory (  &myExtraData, sizeof ( WNT_WindowData )  );

 myHWindow        = aHandle;
 myHParentWindow  = GetParent (  ( HWND )aHandle  );
 myDoubleBuffer   = Standard_False;
 myHPixmap        = NULL;
 wProc            = (WNDPROC )GetWindowLongPtr ((HWND )aHandle, GWLP_WNDPROC);
 uData            = GetWindowLongPtr ((HWND )aHandle, GWLP_USERDATA);
 myUsrData        = Standard_Address(-1);
 myWndProc        = NULL;

 if ( wProc != &WNT_WndProc ) {

  myWndProc = wProc;

  SetBackground ( aBackColor );

  myExtraData.WNT_Window_Ptr = ( void* )this;
  myExtraData.hPal           = ( HPALETTE )dev -> HPalette ();

  if (  uData != ( LONG )&myExtraData  )
  {
    myUsrData = (Standard_Address )SetWindowLongPtr ((HWND )myHWindow, GWLP_USERDATA, (LONG_PTR )&myExtraData);
  }

  if (myWndProc != NULL)
  {
    SetWindowLongPtr ((HWND )myHWindow, GWLP_WNDPROC, (LONG_PTR )WNT_WndProc);
  }

 }  // end if

 myExtraData.dwFlags = WDF_FOREIGN;

 myImages = new WNT_ImageManager ( this );
 myFormat = WNT_TOI_XWD;

 wp.length = sizeof ( WINDOWPLACEMENT );
 GetWindowPlacement (  ( HWND )myHWindow, &wp  );

 aXLeft   = wp.rcNormalPosition.left;
 aYTop    = wp.rcNormalPosition.top;
 aXRight  = wp.rcNormalPosition.right;
 aYBottom = wp.rcNormalPosition.bottom;

}  // end WNT_Window :: doCreate
//***//
//************************************************************************//
