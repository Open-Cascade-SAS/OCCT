// File:        WNT_DDriver.cxx
// Created:     Mon Mar 17 17:28:42 1997
// Author:      EugenyPLOTNIKOV
// Modified:    MAR-98 MAY-98 (EUG & DCB) - study S3553
// DCB: 15-05-98 Changing the parent of WNT_DDriver from Aspect_Driver
//               to Aspect_PlotterDriver
// DCB: SEP-98  Avoid memory crash when color indices do not follow
//              each other or do not begin with '1'
// DCB: OCT-98  Changing the implementation of the constructors to
//              the standard for plotter drivers (constructor takes
//              the file name as a Standard_CString).
//              Changing the implementation of _dd_fill_buff() to fill
//              the EMF file description according to documentation:
//              <APP_Name\0Picture_Name\0\0> - see CreateEnhMetaFile() API.
//              It is possible now to create two files: EMF and PRN.
// Copyright:   Matra Datavision 1993

// include windows.h first to have all definitions available
#include <windows.h>

#define MFT // S3553 study

#define PRO17334        //GG_060199
//              SetXxxAttrib() must raise when index is wrong
//              Idem for DrawMarker()

#define PRO17381        //GG_080199
//                      Protection : Do nothing when NULL string is found in TextSize

#include <WNT_DDriver.ixx>
#include <Aspect_LineStyle.hxx>
#include <Aspect_FontStyle.hxx>
#include <Aspect_ColorMapEntry.hxx>
#include <Aspect_FontMapEntry.hxx>
#include <Aspect_TypeMapEntry.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <Aspect_MarkerStyle.hxx>
#include <Aspect_MarkMapEntry.hxx>
#include <Aspect_WidthMapEntry.hxx>
#include <TColQuantity_Array1OfLength.hxx>
#include <TColStd_Array1OfBoolean.hxx>
#include <InterfaceGraphic_WNT.hxx>
#include <Quantity_Color.hxx>
#include <Resource_Unicode.hxx>
#include <OSD_Path.hxx>
#include <W32_Allocator.hxx>

#include <winspool.h>
#include <windowsx.h>
#include <time.h>
#include <stdio.h>

#pragma comment( lib, "winspool.lib" )
#pragma comment( lib, "advapi32.lib" )

#define FLAG_DISPLAY 0x00000001
#define FLAG_DELETE  0x00000002
#define FLAG_DIRTY   0x00000004
#define FLAG_BANDW   0x00000008
#define FLAG_ROTATE  0x00000010
#define FLAG_WINDOW  0x00000040
#define FLAG_BITMAP  0x00000080

#define CSF_SIGNATURE 0x43465343  // CSFC
#define MAX_DESC_LEN  128

/*====================== MFT ====================*/
#ifdef MFT
#include <Aspect_Units.hxx>
#include "WNT_MFTDraw.hxx"
#endif
/*====================== MFT ====================*/

typedef struct _m_param {
                 Aspect_MarkerStyle* s;
                 int                 w;
                 int                 h;
               } MPARAM, *PMPARAM;

typedef struct _Pixel2D {
                 float r;
                 float g;
                 float b;
               } PIXEL2D, *PPIXEL2D;
typedef struct _reg_key {
                 HKEY   hKey;
                 LPTSTR keyPath;
               } REG_KEY;
typedef struct _csf_sign {
                 DWORD dwSign;
                 DWORD dwLen;
                 DWORD fRotate;
               } CSF_SIGN, *PCSF_SIGN;

extern double  W32_TextFactor;
static PW32_Allocator __fastcall _FindAllocator ( Standard_Address, Standard_Integer );
static char*          __fastcall _ConvertU2F    ( const TCollection_ExtendedString&  );
static void WINAPI _SetPoint ( int, int, int, LPPOINT, int*, void* );

HBITMAP LoadImageFromFile (  Handle( WNT_GraphicDevice )&, char*, HDC = NULL  );
static void __fastcall         _dd_fill_buff ( char* );
static int  __fastcall         _dd_dev_size  ( HDC, int );

static int CALLBACK _dd_enum_proc (
                     HDC, HANDLETABLE FAR*, CONST ENHMETARECORD FAR*,
                     int, LPARAM
                    );

static Standard_ShortReal s_xOff, s_yOff;
static BOOL               s_fUnicode;

#define A    (  ( PW32_Allocator )myAllocator  )

//=====================================================================
WNT_DDriver::WNT_DDriver(const Standard_CString aDeviceName,
                         const Standard_CString aFileName,
                         const WNT_OrientationType anOrientation,
                         const Quantity_Factor aScale,
                         const Standard_Integer aCopies)
                       : PlotMgt_PlotterDriver (aFileName),
                         myFlags (0), myOrientation (anOrientation),
                         myScale (aScale), myNCopies (aCopies), myHDC (NULL)
{
  if (!lstrcmpi (aDeviceName, "DISPLAY" )) {
    myHDC    = CreateDC ( "DISPLAY", NULL, NULL, NULL );
    myFlags |= FLAG_DISPLAY;
  } else if ( *aDeviceName == '\x01'&& !IsBadReadPtr(aDeviceName+1, sizeof(::HDC))) {
    CopyMemory (  &myHDC, aDeviceName + 1, sizeof ( ::HDC )  );
    myFlags |= FLAG_BITMAP;
  } else if ( *aDeviceName != '\x00' ) {
    myHDC = CreateDC ( "WINSPOOL", aDeviceName, NULL, NULL );
  } else if (   !IsBadReadPtr (  aDeviceName + 1, sizeof ( HWND )  )   ) {
    HWND hwnd;
    CopyMemory (  &hwnd, aDeviceName + 1, sizeof ( HWND )  );
    myHDC = GetDC ( hwnd );
    myFlags |= FLAG_WINDOW;
  }  // end else

  if (myHDC == NULL)
    Aspect_DriverDefinitionError :: Raise ( "Invalid device name" );

  myDevice = new WNT_GraphicDevice ( Standard_False, myHDC );
  myRect   = new RECT;

  if (myDevice -> NumColors () == 2)
    myFlags |= FLAG_BANDW;

  ZeroMemory (myRect, sizeof (RECT));

  int                     w, h;
  double                  dw, dh;
  RECT                    r;
  OSD_Path                path (myFileName);
  TCollection_AsciiString ext = path.Extension ();

  if (path.Name() != "" && ext == "")
    path.SetExtension (".PRN");
  path.SystemName (ext);

  myDevice -> DisplaySize (  w,  h );
  myDevice -> DisplaySize ( dw, dh );
  myPixelToUnit = ( dw / w + dh / h ) / 2.0;

  ((PRECT)myRect)->right  = w;
  ((PRECT)myRect)->bottom = h;
  r.left   = 0;
  r.top    = 0;
  r.right  = _dd_dev_size (  ( ::HDC )myHDC, HORZSIZE  ) * 100;
  r.bottom = _dd_dev_size (  ( ::HDC )myHDC, VERTSIZE  ) * 100;

  switch ( myOrientation ) {
    case WNT_OT_PORTRAIT:
      if ( w > h )
    doRotate:
        myFlags |= FLAG_ROTATE;
        break;
    case WNT_OT_LANDSCAPE:
      if ( w < h ) goto doRotate;
      break;
  }  // end switch

  char buffer[ MAX_DESC_LEN ];
  ZeroMemory (  buffer, sizeof ( buffer )  );
  _dd_fill_buff (buffer);

/*
  if (ext.IsEmpty ()) {
    myFlags |= FLAG_DELETE;
    ext = _dd_temp_name ();
  }  // end if
*/
  // Set name for PRN file
  myPrnName = ext;
  // Set name for EMF file
  path.SetExtension (".EMF");
  path.SystemName   (ext   );
  myEmfName = ext;
  // Create EMF file
  if ((myHDCMeta = CreateEnhMetaFile (
                     ( ::HDC )myHDC, myEmfName.ToCString (), &r, buffer )
      ) == NULL )
     Aspect_DriverError :: Raise ( "Could not create enhanced metafile" );

  myImage       = NULL;
  myImageName   = new TCollection_HAsciiString ( "" );
  myAllocators  = myAllocator = ( *W32_GetAllocator ) ( 0, NULL );
}

//=====================================================================
WNT_DDriver::WNT_DDriver(const Standard_CString aFileName,
                         const Standard_Integer aCopies)
                       : PlotMgt_PlotterDriver (aFileName)
{
  myAllocator   = NULL;
  myAllocators  = NULL;
  myImage       = NULL;
  myHDC         = NULL;
  myRect        = NULL;
  myHMetaFile   = NULL;
  myNCopies     = aCopies;
  // Set filenames
  OSD_Path path (myFileName);
  if (path.Extension () == "")
    path.SetExtension (".PRN");
  path.SystemName   (myPrnName);
  path.SetExtension (".EMF");
  path.SystemName   (myEmfName);
}

//=====================================================================
void WNT_DDriver::Close ()
{
  if (  myHDC       != NULL && !( myFlags & FLAG_BITMAP )  )
    ( myFlags & FLAG_WINDOW )                                           ?
    ReleaseDC (   WindowFromDC (  ( ::HDC )myHDC  ), ( ::HDC )myHDC   ) :
    DeleteDC  (                                      ( ::HDC )myHDC   );
  if ( myRect      != NULL )
    delete ( PRECT )myRect;
  if ( myImage     != NULL )
    DeleteObject ( myImage );
  if ( myHMetaFile != NULL )
    DeleteEnhMetaFile (  ( HENHMETAFILE )myHMetaFile  );
  if (myAllocators != NULL) {
    do {
      myAllocator = (  ( PW32_Allocator )myAllocators  ) -> myNext;
      delete ( PW32_Allocator )myAllocators;
      myAllocators = ( PW32_Allocator )myAllocator;
    } while ( myAllocators != NULL );
  }
  // Set value to NULL
  myHDC        = NULL;
  myRect       = NULL;
  myImage      = NULL;
  myHMetaFile  = NULL;
/*
  // Delete enhanced metafile
  if (GetFileAttributes(myEmfName.ToCString ()) != 0xFFFFFFFF)
    DeleteFile (  myEmfName.ToCString ()  );
*/
}

//=====================================================================
void WNT_DDriver::BeginDraw() 
{
  // Text management
  myNTextManager = new WNT_TextManager (myPixelToUnit);

  PW32_Allocator a = _FindAllocator (myAllocators, 0);
  if (a != NULL) {
    a -> ClearBlocks ();
    a -> myFlags |=  W32F_START;
    a -> myFlags |=  W32F_DVDRV;
    a -> myFlags &= ~W32F_DBUFF;

    if (myFlags & FLAG_DIRTY) {
      RECT                    r    = { 0, 0, 0, 0 };
      char                    buffer[ MAX_DESC_LEN ];

      r.right  = _dd_dev_size (  ( ::HDC )myHDC, HORZSIZE  ) * 100;
      r.bottom = _dd_dev_size (  ( ::HDC )myHDC, VERTSIZE  ) * 100;

      ZeroMemory (  buffer, sizeof ( buffer )  );
      _dd_fill_buff ( buffer );
      DeleteEnhMetaFile ( (HENHMETAFILE)myHMetaFile );

      if (/*myFlags & FLAG_DELETE && */
          GetFileAttributes(myEmfName.ToCString ()) != 0xFFFFFFFF)
        DeleteFile ( myEmfName.ToCString () );

      myHDCMeta = CreateEnhMetaFile ((::HDC)myHDC, myEmfName.ToCString (), &r, buffer);
      if ( myImage != NULL ) {
        DeleteObject ( myImage );
        myImage = NULL;
      }
      myImageName = new TCollection_HAsciiString ("");
    } else myFlags |= FLAG_DIRTY;

    Quantity_Length width, height;
    WorkSpace (width, height);
    if (myFlags & FLAG_ROTATE) {
      a -> myAngle   = Standard_PI / 2;
      a -> myPivot.x = Convert (height / 2);
      a -> myPivot.y = Convert (width  / 2);
      a -> myMove.x  =
      a -> myMove.y  = (int)((a->myPivot.x - a->myPivot.y) * myScale);
    } else {
      a -> myMove.x = (int)Convert((width  / 2) * (1 - myScale));
      a -> myMove.y = (int)Convert((height / 2) * (1 - myScale));
    }

    a -> myScaleX = a -> myScaleY = myScale;
  }

  CSF_SIGN cs = {
            CSF_SIGNATURE, sizeof ( CSF_SIGN ), myFlags & FLAG_ROTATE ? 'T' : 'F'
           };

  GdiComment (
   ( ::HDC )myHDCMeta, sizeof ( CSF_SIGN ), ( CONST BYTE* )&cs
  );
}

//=====================================================================
void WNT_DDriver::EndDraw(const Standard_Boolean fSynchronize)
{
  ((PW32_Allocator)myAllocator) -> myFlags &= ~W32F_START;
  PW32_Allocator a = _FindAllocator (myAllocators, 0);
  if (a != NULL) {
    SIZE     sz;
    HPALETTE hpo = NULL;

    if (myDevice -> IsPaletteDevice ())
      hpo = SelectPalette ((::HDC) myHDCMeta,
                           (HPALETTE)myDevice->HPalette (), FALSE);

    sz.cx = ((PRECT)myRect)->right;
    sz.cy = ((PRECT)myRect)->bottom;
    a -> Play ( (::HDC) myHDCMeta, &sz );
    if (myFlags & FLAG_DISPLAY)
      a -> Play ((::HDC) myHDC, &sz);

    if (hpo != NULL)
      SelectPalette((::HDC) myHDCMeta, hpo, FALSE);
    a -> myFlags |= W32F_DRAWN;
    myHMetaFile = CloseEnhMetaFile (  ( ::HDC )myHDCMeta  );
  }
/*
  if (( myFlags & FLAG_PRINT ) || aPrintFlag)
    DoSpool (Standard_False);
  if (myFlags & FLAG_DELETE && myHMetaFile != NULL) {
    TCollection_AsciiString fName;
    myPath.SystemName ( fName );
    DeleteEnhMetaFile (  ( HENHMETAFILE )myHMetaFile  );
    myHMetaFile = NULL;
    if (GetFileAttributes (fName.ToCString ()) != 0xFFFFFFFF)
      DeleteFile (  fName.ToCString ()  );
  }  // end if
*/
}

//=====================================================================
Standard_Boolean WNT_DDriver::Spool(const Aspect_PlotMode aPlotMode,
                                    const Standard_CString aDeviceName,
                                    const Standard_Boolean anOriginalSize)
{
  Standard_Boolean fOrigSize = anOriginalSize;
  if ( aDeviceName != NULL ) {
    if ( myHDC != NULL ) Close ();
    if ( !lstrcmpi ( aDeviceName, "DISPLAY" ) ) {
      myHDC    = CreateDC ( "DISPLAY", NULL, NULL, NULL );
      myFlags |= FLAG_DISPLAY;
    } else if ( *aDeviceName == '\x01'&& !IsBadReadPtr(aDeviceName+1, sizeof(::HDC))) {
      CopyMemory (  &myHDC, aDeviceName + 1, sizeof ( ::HDC )  );  
      myFlags |= FLAG_BITMAP;
    } else if ( *aDeviceName != '\x00' ) {
      myHDC = CreateDC ( "WINSPOOL", aDeviceName, NULL, NULL );
    } else if (   !IsBadReadPtr ( aDeviceName + 1, sizeof ( HWND )  )   ) {
      HWND hwnd;
      CopyMemory (  &hwnd, aDeviceName + 1, sizeof ( HWND )  );
      myHDC = GetDC ( hwnd );
      myFlags |= FLAG_WINDOW;
    }  // end else

    if ( myHDC == NULL )
      return Standard_False;
    myDevice = new WNT_GraphicDevice ( Standard_False, myHDC );
    myRect   = new RECT;
    ZeroMemory (  myRect, sizeof ( RECT )  );
    int                     w, h;
    myDevice -> DisplaySize ( w,   h );
    (  ( PRECT )myRect  ) -> right  = w;
    (  ( PRECT )myRect  ) -> bottom = h;
    if ((myHMetaFile = GetEnhMetaFile (myEmfName.ToCString ())) == NULL)
      return Standard_False;
  } else fOrigSize = Standard_False;
  Standard_Boolean fSpoolRes1 = DoSpool ( fOrigSize, aPlotMode );
  Standard_Boolean fSpoolRes2 = (aPlotMode == Aspect_PM_FILEONLY ?
                                  Standard_True :
                                  DoSpool ( fOrigSize, Aspect_PM_FILEONLY )
                                );
  return (fSpoolRes1 & fSpoolRes2);
}

//=====================================================================
Standard_Boolean WNT_DDriver::DoSpool
                 (const Standard_Boolean anOriginalSize,
                  const Aspect_PlotMode aPlotMode) const
{
  Standard_Integer i;
  UINT             n;
  char             buffer[ MAX_DESC_LEN ]; 
  DOCINFO          di;
  PLOGPALETTE      pLogPal;
  HPALETTE         hPal;
  Standard_Boolean retVal = Standard_True;

  if (GetEnhMetaFileDescription ((HENHMETAFILE )myHMetaFile, MAX_DESC_LEN, buffer)
      == GDI_ERROR)
    return Standard_False;
  if (!(myFlags & (FLAG_DISPLAY|FLAG_WINDOW|FLAG_BITMAP))) {
    di.cbSize      = sizeof (DOCINFO);
    di.lpszDocName = buffer;
    if (aPlotMode == Aspect_PM_FILEONLY)
      di.lpszOutput = myPrnName.ToCString ();
    else
      di.lpszOutput = NULL;
    i = 0;
    if ( StartDoc  ( ( ::HDC )myHDC, &di ) <= 0 )
      return Standard_False;
  nextPage:
    if ( StartPage ( ( ::HDC )myHDC      ) <= 0 )
      return Standard_False;
  }  // end if
  n = GetEnhMetaFilePaletteEntries ( ( HENHMETAFILE )myHMetaFile, 0, NULL );
  if ( n && n != GDI_ERROR ) {
    pLogPal = ( PLOGPALETTE )new char[ sizeof ( DWORD ) + sizeof ( PALETTEENTRY ) * n ];
    pLogPal -> palVersion    = 0x300;
    pLogPal -> palNumEntries = ( WORD )n;
    GetEnhMetaFilePaletteEntries (
      ( HENHMETAFILE )myHMetaFile, n,
      ( PPALETTEENTRY )(   (  ( PBYTE )pLogPal  ) + 8   )
    );
    hPal = CreatePalette ( pLogPal );
    delete [] ( char* )pLogPal;
    SelectPalette  (  ( ::HDC )myHDC, hPal, FALSE  );
    RealizePalette (  ( ::HDC )myHDC               );
  }  // end if
  if (anOriginalSize) {
    float         pX, pY, mmX, mmY;
    ENHMETAHEADER emh;
    ZeroMemory (  &emh, sizeof ( ENHMETAHEADER )  );
    emh.nSize = sizeof ( ENHMETAHEADER );
    if (  GetEnhMetaFileHeader (
            ( HENHMETAFILE )myHMetaFile, sizeof ( ENHMETAHEADER ), &emh)
          == 0
       ) goto spoolError;
    pX  = ( float )_dd_dev_size (  ( ::HDC )myHDC, HORZRES   );
    pY  = ( float )_dd_dev_size (  ( ::HDC )myHDC, VERTRES   );
    mmX = ( float )_dd_dev_size (  ( ::HDC )myHDC, HORZSIZE  );
    mmY = ( float )_dd_dev_size (  ( ::HDC )myHDC, VERTSIZE  );
    *(    ( PLONG )(   &(  ( LPRECT )myRect  ) -> top      )    ) =
      ( LONG )(  ( float )( emh.rclFrame.top    ) * pY / ( mmY * 100.0F )  );
    *(    ( PLONG )(   &(  ( LPRECT )myRect  ) -> left     )    ) =
      ( LONG )(  ( float )( emh.rclFrame.left   ) * pX / ( mmX * 100.0F )  );
    *(    ( PLONG )(   &(  ( LPRECT )myRect  ) -> right    )    ) =
      ( LONG )(  ( float )( emh.rclFrame.right  ) * pX / ( mmX * 100.0F )  );
    *(    ( PLONG )(   &(  ( LPRECT )myRect  ) -> bottom   )    ) =
      ( LONG )(  ( float )( emh.rclFrame.bottom ) * pY / ( mmY * 100.0F )  );
  }  // end if
  if (!PlayEnhMetaFile ((::HDC)myHDC, (HENHMETAFILE)myHMetaFile, (LPRECT)myRect))
spoolError:
    retVal = Standard_False;

  if (n && n != GDI_ERROR)
    DeleteObject ( hPal );

  if (!( myFlags & (FLAG_DISPLAY|FLAG_WINDOW|FLAG_BITMAP))) {
    if (EndPage (  ( ::HDC )myHDC  ) <= 0)
      retVal = Standard_False;
    if (++i < myNCopies && retVal)
      goto nextPage;
    if (EndDoc  (  ( ::HDC )myHDC  ) <= 0)
      retVal = Standard_False;
  } // end if
  return retVal;
}

//=====================================================================
void WNT_DDriver::SetLineAttrib(const Standard_Integer ColorIndex,
                                const Standard_Integer TypeIndex,
                                const Standard_Integer WidthIndex) 
{
#ifdef PRO17334
  if (myColors.IsNull() ||
        ColorIndex < myColors->Lower() || ColorIndex > myColors->Upper())
                        Aspect_DriverError::Raise ("Bad Color Index");
  if (myTypeIdxs.IsNull() ||
        TypeIndex < myTypeIdxs->Lower() || TypeIndex > myTypeIdxs->Upper())
                        Aspect_DriverError::Raise ("Bad Type Index");
  if (myWidthIdxs.IsNull() ||
        WidthIndex < myWidthIdxs->Lower() || WidthIndex > myWidthIdxs->Upper())
                        Aspect_DriverError::Raise ("Bad Width Index");

  Aspect_LineStyle    Entry = TypeMap () -> Entry ( myTypeIdxs -> Value(TypeIndex) ).Type ();
#else
  Standard_Integer theIndex = TypeIndex;
  if (TypeIndex < myTypeIdxs->Lower() || TypeIndex > myTypeIdxs->Upper())
    theIndex = myTypeIdxs->Lower();

  Aspect_LineStyle    Entry = TypeMap () -> Entry ( myTypeIdxs -> Value(theIndex) ).Type ();
#endif
  Standard_Integer    type  = ((Entry.Style() == Aspect_TOL_SOLID) ? 0 : PS_USERSTYLE);
  double              width = ( double )ProcessWidthIndex ( WidthIndex );
  Standard_Boolean    fUser = ( type & PS_USERSTYLE );
  LOGBRUSH            lb    = { BS_SOLID, ProcessColorIndex ( ColorIndex ), 0 };
  if ( fUser ) {
    Standard_Integer Lower   = Entry.Values ().Lower ();
    Standard_Integer Count   = Entry.Values ().Length ();
    PDWORD           dwStyle = ( PDWORD )HeapAlloc (
                                          GetProcessHeap (),
                                          HEAP_GENERATE_EXCEPTIONS,
                                          Count * sizeof ( DWORD )
                                         );
     for ( Standard_Integer i = 0; i < Count; ++i ) {
       dwStyle[ i ] = DWORD (  Entry.Values ().Value ( i + Lower ) / myPixelToUnit + 0.5  );
       if ( dwStyle[ i ] == 0.0 ) dwStyle[ i ] = 1;
     }  // end for
     A -> LineAttrib (  DWORD( width + 0.5 ), &lb, Count, dwStyle  );
    HeapFree (  GetProcessHeap (), 0, ( LPVOID )dwStyle  );
  } else A -> LineAttrib (  DWORD( width + 0.5 ), &lb  );
}

//=====================================================================
void WNT_DDriver::SetTextAttrib(const Standard_Integer ColorIndex,
                                const Standard_Integer FontIndex) 
{
  SetTextAttrib ( ColorIndex, FontIndex, 0.0, 1.0, 0.0,Standard_False );
}

//=====================================================================
void WNT_DDriver::SetTextAttrib(const Standard_Integer ColorIndex,
                                const Standard_Integer FontIndex,
                                const Quantity_PlaneAngle aSlant,
                                const Quantity_Factor aHScale,
                                const Quantity_Factor aWScale,
                                const Standard_Boolean isUnderlined) 
{
#ifdef PRO17334
  if (myColors.IsNull() ||
        ColorIndex < myColors->Lower() || ColorIndex > myColors->Upper())
                        Aspect_DriverError::Raise ("Bad Color Index");
  if (myFonts.IsNull() ||
        FontIndex < myFonts->Lower() || FontIndex > myFonts->Upper())
                        Aspect_DriverError::Raise ("Bad Font Index");
#endif

  HFONT                      hFont;
  LOGFONT                    lf;
  BOOL                       fFree = FALSE;
  Handle( WNT_FontMapEntry ) fmEntry = myFonts -> Value ( FontIndex );
/*====================== MFT ====================*/
#ifdef MFT
  if ( UseMFT() ) {
    //============== Set some font attributes =============
    Handle(MFT_FontManager) theFontManager =  MFT_Font(FontIndex);
    Standard_ShortReal      theTextSize    =  MFT_Size(FontIndex);
    theFontManager->SetFontAttribs (
      Abs(theTextSize)*(aWScale > 0.0 ? aWScale : aHScale)*A->myScaleX,
      Abs(theTextSize)*aHScale                            *A->myScaleY,
      aSlant, 0., ((theTextSize > 0.) ? Standard_False : Standard_True)
    );
    A -> TextAttrib (
      (HFONT)FontIndex, ProcessColorIndex ( ColorIndex ), aSlant,
      aHScale,
      ( aWScale > 0.0 ? aWScale : aHScale ),
      isUnderlined, Standard_False, Standard_True
    );
  } else {
#endif
/*====================== MFT ====================*/
    GetObject (
      hFont = ( HFONT )fmEntry-> HFont (),
      sizeof ( LOGFONT ), &lf
    );
    if ( isUnderlined && !lf.lfUnderline ) {
      FONT_DATA fd = { TRUE };
      hFont = ( HFONT )fmEntry -> SetAttrib ( faUnderlined, &fd );
      fFree = TRUE;
    }  // end if
    A -> TextAttrib (
      hFont, ProcessColorIndex ( ColorIndex ), aSlant,
      aHScale / W32_TextFactor,
      ( aWScale > 0.0 ? aWScale : aHScale ) / W32_TextFactor,
      isUnderlined, fFree
    );
/*====================== MFT ====================*/
#ifdef MFT
  }
#endif
/*====================== MFT ====================*/
}

//=====================================================================
void WNT_DDriver::SetPolyAttrib(const Standard_Integer ColorIndex,
                                const Standard_Integer TileIndex,
                                const Standard_Boolean DrawEdge) 
{
#ifdef PRO17334
  if (myColors.IsNull() ||
        ColorIndex < myColors->Lower() || ColorIndex > myColors->Upper())
                        Aspect_DriverError::Raise ("Bad Color Index");
#endif
  LOGBRUSH lb = { TileIndex < 0 ? BS_NULL : BS_SOLID, ProcessColorIndex ( ColorIndex ), 0 };
  A -> PolyAttrib ( &lb, DrawEdge );
}

//=====================================================================
void WNT_DDriver::SetMarkerAttrib(const Standard_Integer ColorIndex,
                                  const Standard_Integer WidthIndex,
                                  const Standard_Boolean FillMarker) 
{
#ifdef PRO17334
  if (myColors.IsNull() ||
        ColorIndex < myColors->Lower() || ColorIndex > myColors->Upper())
                        Aspect_DriverError::Raise ("Bad Color Index");
  if (myWidthIdxs.IsNull() ||
        WidthIndex < myWidthIdxs->Lower() || WidthIndex > myWidthIdxs->Upper())
                        Aspect_DriverError::Raise ("Bad EdgeWidth Index");
#endif
  A -> MarkerAttrib (
        ProcessColorIndex ( ColorIndex ),
        DWORD(  ProcessWidthIndex ( WidthIndex ) + 0.5  ),
        FillMarker
       );
}

//=====================================================================
Standard_Boolean WNT_DDriver::IsKnownImage
                 (const Handle(Standard_Transient)& anImage) 
{
  return Standard_False;
}

//=====================================================================
Standard_Boolean WNT_DDriver::SizeOfImageFile
                 (const Standard_CString anImageFile,
                  Standard_Integer& aWidth,
                  Standard_Integer& aHeight) const
{
#ifndef BUG     //Sets the size to 0 when the image file is not found.
 aWidth = 0; aHeight = 0;
#endif

  Handle( TCollection_HAsciiString ) iName = myImageName;
  Handle( WNT_GraphicDevice        ) gDev  = myDevice;
  if (  myImage == NULL || iName -> String () != anImageFile  ) {
    *(  ( Aspect_Handle* )&myImage  ) =
      LoadImageFromFile (  gDev, (Standard_PCharacter)anImageFile, ( ::HDC )myHDCMeta  );
    if ( myImage == NULL ) return Standard_False;
    iName -> Clear ();
    iName -> AssignCat ( anImageFile );
  }  // end if
  BITMAP bmp;
  GetObject (  ( HBITMAP )myImage, sizeof ( BITMAP ), &bmp  );
  aWidth  = bmp.bmWidth;
  aHeight = bmp.bmHeight;
  return Standard_True;
}

//=====================================================================
void WNT_DDriver::ClearImage(const Handle(Standard_Transient)& anImageId) 
{
  if (myImage != NULL) {
    DeleteObject ( myImage );
    myImage     =  NULL;
    myImageName -> Clear ();
  }  // end if
}

//=====================================================================
void WNT_DDriver::ClearImageFile(const Standard_CString anImageFile) 
{
  if ( myImage != NULL && myImageName -> String () == anImageFile )
    ClearImage ( Handle( Standard_Transient ) () );
}

//=====================================================================
void WNT_DDriver::DrawImage(const Handle(Standard_Transient)& anImageId,
                            const Standard_ShortReal aX,
                            const Standard_ShortReal aY) 
{
  if (myImage != NULL) {
    PW32_Bitmap pwbmp = ( PW32_Bitmap )HeapAlloc (
      GetProcessHeap (), HEAP_GENERATE_EXCEPTIONS, sizeof ( W32_Bitmap )
    );
    pwbmp -> nUsed = 0;
    pwbmp -> hBmp  = (HBITMAP)myImage;
    A -> Image ( P( aX ), P( aY ), pwbmp, 1.0 * Convert(myPixelSize) );
  }
}

//=====================================================================
void WNT_DDriver::DrawImageFile(const Standard_CString anImageFile,
                                const Standard_ShortReal aX,
                                const Standard_ShortReal aY,
                                const Quantity_Factor aScale) 
{
  int Width, Height;
  if (SizeOfImageFile ( anImageFile, Width, Height )) {
    PW32_Bitmap pwbmp = ( PW32_Bitmap )HeapAlloc (
      GetProcessHeap (), HEAP_GENERATE_EXCEPTIONS, sizeof ( W32_Bitmap )
    );
    pwbmp -> nUsed = 0;
    pwbmp -> hBmp  = ( HBITMAP )myImage;
    A -> Image ( P( aX ), P( aY ), pwbmp, aScale * Convert(myPixelSize) );
  }
}

//=====================================================================
void WNT_DDriver::FillAndDrawImage
                  (const Handle(Standard_Transient)& anImageId,
                   const Standard_ShortReal aX,
                   const Standard_ShortReal aY,
                   const Standard_Integer aWidth,
                   const Standard_Integer aHeight,
                   const Standard_Address anArrayOfPixels) 
{
  int                         Width, Height;
  ::HDC                       hdcMem;
  HBITMAP                     hbo;
  HPALETTE                    hpo = NULL;
  Quantity_Color              color;
  PPIXEL2D                    p = ( PPIXEL2D )anArrayOfPixels;

  if ( !SizeOfImageFile (myImageName->ToCString(), Width, Height) ||
       Width  != aWidth                                           ||
       Height != aHeight) {
    Width  = aWidth;
    Height = aHeight;
    ClearImage ( anImageId );
    myImage = CreateCompatibleBitmap ((::HDC)myHDCMeta, Width, Height);
  }  // end if
  hdcMem = CreateCompatibleDC ( (::HDC)myHDCMeta );

  if (  myDevice -> IsPaletteDevice ()  )
    hpo = SelectPalette (hdcMem, (HPALETTE)myDevice->HPalette (), FALSE);

  if (Width > 0 && Height > 0) {
    hbo = ( HBITMAP )SelectObject (hdcMem, ( HBITMAP )myImage);
    for ( int i = 0; i < Height; ++i )
      for ( int j = 0; j < Width;  ++j, ++p ) {
        color.SetValues ( p->r, p->g, p->b, Quantity_TOC_RGB );
        SetPixel (hdcMem, j, i, myDevice->SetColor(color));
      }  // end for
    SelectObject ( hdcMem, hbo );
  }  // end if

  if ( hpo != NULL ) SelectPalette ( hdcMem, hpo, FALSE );

  DeleteDC  ( hdcMem );
  DrawImage ( anImageId, aX, aY );
}

//=====================================================================
void WNT_DDriver::FillAndDrawImage
                  (const Handle(Standard_Transient)& anImageId,
                   const Standard_ShortReal aX,
                   const Standard_ShortReal aY,
                   const Standard_Integer anIndexOfLine,
                   const Standard_Integer aWidth,
                   const Standard_Integer aHeight,
                   const Standard_Address anArrayOfPixels) 
{
  int                         Width, Height;
  ::HDC                       hdcMem;
  HBITMAP                     hbo;
  HPALETTE                    hpo = NULL;
  Quantity_Color              color;
  PPIXEL2D                    p = ( PPIXEL2D )anArrayOfPixels;

  if ( !SizeOfImageFile (myImageName->ToCString(), Width, Height) ||
       Width  != aWidth                                           ||
       Height != aHeight) {
    Width  = aWidth;
    Height = aHeight;
    ClearImage ( anImageId );
    myImage = CreateCompatibleBitmap ((::HDC)myHDCMeta, Width, Height);
  }  // end if
  hdcMem = CreateCompatibleDC ( (::HDC)myHDCMeta );

  if (  myDevice -> IsPaletteDevice ()  )
    hpo = SelectPalette (hdcMem, (HPALETTE)myDevice->HPalette (), FALSE);

  if (Width > 0 && Height > 0) {
    hbo = ( HBITMAP )SelectObject (hdcMem, ( HBITMAP )myImage);
    for ( int j = 0; j < Width;  ++j, ++p ) {
      color.SetValues ( p->r, p->g, p->b, Quantity_TOC_RGB );
      SetPixel (hdcMem, j, anIndexOfLine, myDevice->SetColor(color));
    }  // end for
    SelectObject ( hdcMem, hbo );
  }  // end if

  if ( hpo != NULL ) SelectPalette ( hdcMem, hpo, FALSE );

  DeleteDC  ( hdcMem );
  if (anIndexOfLine == Height - 1)
    DrawImage ( anImageId, aX, aY );
}

//=====================================================================
Standard_Boolean WNT_DDriver::PlotPolyline(const Standard_Address xArray,
                                           const Standard_Address yArray,
                                           const Standard_Address nPts,
                                           const Standard_Integer nParts)
{
/*
  Standard_Integer nPts = aListX.Length ();
  Standard_Integer lX = aListX.Lower ();
  Standard_Integer lY = aListY.Lower ();
  if ( A -> myPrimitive != zzNone )
    ClosePrimitive ();
  W32_PolygonNote* p = ( W32_PolygonNote* )(  A -> Polyline ( nPts )  );
  for ( Standard_Integer i = 0; i < nPts; ++i )
    p -> Add (
      P(  aListX.Value ( lX + i )  ),
      P(  aListY.Value ( lY + i )  )
    );
*/
  return Standard_True;
}

//=====================================================================
Standard_Boolean WNT_DDriver::PlotPolygon(const Standard_Address xArray,
                                          const Standard_Address yArray,
                                          const Standard_Address nPts,
                                          const Standard_Integer nParts)
{
/*
  Standard_Integer nPts = aListX.Length ();
  if (  nPts != aListY.Length ()  )
    Aspect_DriverError :: Raise ( "DrawPolygon: incorrect argument" );
  if ( nPts > 0 ) {
    Standard_Integer lX = aListX.Lower ();
    Standard_Integer lY = aListY.Lower ();
    if ( A -> myPrimitive != zzNone )
      ClosePrimitive ();
    W32_PolygonNote* p = ( W32_PolygonNote* )(  A -> Polygon ( nPts )  );
    for ( Standard_Integer i = 0; i < nPts; ++i )
      p -> Add (
        P(  aListX.Value ( lX + i )  ),
        P(  aListY.Value ( lY + i )  )
      );
  }  // end if
*/
  return Standard_True;
}

//=====================================================================
Standard_Boolean WNT_DDriver::PlotSegment(const Standard_ShortReal X1,
                                          const Standard_ShortReal Y1,
                                          const Standard_ShortReal X2,
                                          const Standard_ShortReal Y2) 
{
  A -> Line (  P( X1 ), P( Y1 ), P( X2 ), P( Y2 )  );
  return Standard_True;
}

//=====================================================================
Standard_Boolean WNT_DDriver::PlotText(const TCollection_ExtendedString& aText,
                                       const Standard_ShortReal Xpos,
                                       const Standard_ShortReal Ypos,
                                       const Standard_ShortReal anAngle,
                                       const Aspect_TypeOfText aType) 
{
/*====================== MFT ====================*/
#ifdef MFT
  if ( UseMFT() ) {
/*
    MFT_DRAWTEXT   dt = {
      myAllocator, this, myPixelToUnit,
      Xpos, Ypos, anAngle, 0., (int)aType,
      FALSE, TRUE, aText.Length(), NULL
    };
    CopyMemory (dt.theText, aText.ToExtString(),
                aText.Length()*sizeof(short));
    A -> FunCall ( _Do_MFTDrawText_, sizeof(MFT_DRAWTEXT),
                   PW32_FCALLPARAM(&dt) );
*/
  } else {
#endif
/*====================== MFT ====================*/
    char* str = _ConvertU2F ( aText );
    A -> Text (
      P( Xpos ), P( Ypos ),
      anAngle, str, FALSE,
      ( aType == Aspect_TOT_SOLID ) ? FALSE : TRUE
    );
    HeapFree (  GetProcessHeap (), 0, str  );
/*====================== MFT ====================*/
#ifdef MFT
  }
#endif
/*====================== MFT ====================*/
  return Standard_True;
}

//=====================================================================
Standard_Boolean WNT_DDriver :: PlotText (
                     const Standard_CString      Text,
                     const Standard_ShortReal    Xpos,
                     const Standard_ShortReal    Ypos,
                     const Standard_ShortReal anAngle,
                     const Aspect_TypeOfText    aType
                    )
{
/*====================== MFT ====================*/
#ifdef MFT
  if ( UseMFT() ) {
/*
    MFT_DRAWTEXT   dt = {
      myAllocator, this, myPixelToUnit,
      Xpos, Ypos, anAngle, 0., (int)aType,
      FALSE, FALSE, lstrlen(Text), NULL
    };
    CopyMemory (dt.theText, Text, lstrlen(Text));
    A -> FunCall ( _Do_MFTDrawText_, sizeof(MFT_DRAWTEXT),
                   PW32_FCALLPARAM(&dt) );
*/
  } else
#endif
/*====================== MFT ====================*/
    A -> Text (
           P( Xpos ), P( Ypos ),
           anAngle, (Standard_PCharacter)Text, FALSE,
           ( aType == Aspect_TOT_SOLID ) ? FALSE : TRUE
         );
  return Standard_True;
}

//=====================================================================
Standard_Boolean WNT_DDriver::PlotPolyText(const TCollection_ExtendedString& aText,
                                           const Standard_ShortReal Xpos,
                                           const Standard_ShortReal Ypos,
                                           const Quantity_Ratio aMarge,
                                           const Standard_ShortReal anAngle,
                                           const Aspect_TypeOfText aType) 
{
/*====================== MFT ====================*/
#ifdef MFT
  if ( UseMFT() ) {
/*
    MFT_DRAWTEXT   dt = {
      myAllocator, this, myPixelToUnit,
      Xpos, Ypos, anAngle, aMarge, (int)aType,
      TRUE, TRUE, aText.Length(), NULL
    };
    CopyMemory (dt.theText, aText.ToExtString(),
                aText.Length()*sizeof(short));
    A -> FunCall ( _Do_MFTDrawText_, sizeof(MFT_DRAWTEXT),
                   PW32_FCALLPARAM(&dt) );
*/
  } else {
#endif
/*====================== MFT ====================*/
    char* str = _ConvertU2F ( aText );
    A -> Polytext (
          P( Xpos ), P( Ypos ),
          anAngle, aMarge, str, FALSE,
          ( aType == Aspect_TOT_SOLID ) ? FALSE : TRUE
         );
    HeapFree (  GetProcessHeap (), 0, str  );
/*====================== MFT ====================*/
#ifdef MFT
  }
#endif
/*====================== MFT ====================*/
  return Standard_True;
}

//=====================================================================
Standard_Boolean WNT_DDriver :: PlotPolyText (
                     const Standard_CString     aText,
                     const Standard_ShortReal    Xpos,
                     const Standard_ShortReal    Ypos,
                     const Quantity_Ratio      aMarge,
                     const Standard_ShortReal anAngle,
                     const Aspect_TypeOfText    aType
                    )
{
/*====================== MFT ====================*/
#ifdef MFT
  if ( UseMFT() ) {
/*
    MFT_DRAWTEXT   dt = {
      myAllocator, this, myPixelToUnit,
      Xpos, Ypos, anAngle, aMarge, (int)aType,
      TRUE, FALSE, lstrlen(aText), NULL
    };
    CopyMemory (dt.theText, aText, lstrlen(aText));
    A -> FunCall ( _Do_MFTDrawText_, sizeof(MFT_DRAWTEXT),
                   PW32_FCALLPARAM(&dt) );
*/
  } else
#endif
/*====================== MFT ====================*/
    A -> Polytext (
          P( Xpos ), P( Ypos ),
          anAngle, aMarge, (Standard_PCharacter)aText, FALSE,
          ( aType == Aspect_TOT_SOLID ) ? FALSE : TRUE
         );
  return Standard_True;
}

//=====================================================================
Standard_Boolean WNT_DDriver::PlotPoint(const Standard_ShortReal X,
                                        const Standard_ShortReal Y) 
{
  switch ( A -> myPrimitive ) {
    case zzPolygon:
      (  ( W32_PolygonNote* )( A -> myNote )  ) -> Add (  P( X ), P( Y )  );
      break;
    case zzPolyline:
      (  ( W32_PolylineNote* )( A -> myNote )  ) -> Add (  P( X ), P( Y )  );
      break;
    default:
      A -> Point (  P( X ), P( Y )  );
  }  // end switch
  return Standard_True;
}

//=====================================================================
Standard_Boolean WNT_DDriver::PlotMarker(const Standard_Integer aMarker,
                                         const Standard_ShortReal Xpos,
                                         const Standard_ShortReal Ypos,
                                         const Standard_ShortReal Width,
                                         const Standard_ShortReal Height,
                                         const Standard_ShortReal Angle) 
{
  if ( A -> myPrimitive != zzNone && A -> myPrimitive != zzMarker )
    ClosePrimitive ();

  int aWidth  = P( Width  );
  int aHeight = P( Height );
  Standard_Integer theIndex = -1;
#ifdef PRO17334
  if (!myMarkerIdxs.IsNull() &&
         (aMarker >= myMarkerIdxs->Lower() && aMarker <= myMarkerIdxs->Upper()))
    theIndex = myMarkerIdxs->Value(aMarker);
  if (theIndex < 0) Aspect_DriverError::Raise ("Bad Marker Index");

  if (aWidth && aHeight && (aMarker > 0)) {
#else
  if (aMarker >= myMarkerIdxs->Lower() || aMarker <= myMarkerIdxs->Upper())
    theIndex = myMarkerIdxs->Value(aMarker);

  if (aWidth && aHeight && (theIndex >= 0)) {
#endif
    A -> BeginMarker (  P( Xpos ), P( Ypos ), aWidth, aHeight, Angle  );
    Aspect_MarkerStyle Style = MarkMap () -> Entry ( theIndex ).Style ();
    int MarkerSize = Style.Length ();
    int DrawCount  = 0;
    int PrevPoint  = 0;
    int i          = 2;
    MPARAM mp = { &Style, aWidth, aHeight };
    for (; i <= MarkerSize; ++i ) {
      if (  Style.SValues ().Value ( i )  ) {
        if (  Style.SValues ().Value ( i - 1 )  )
          ++DrawCount;
        else {
          PrevPoint = i - 1;
          DrawCount = 2;
        }  // end else
      } else {
        if (  Style.SValues().Value ( i - 1 )  ) {
          if (   (  Style.XValues ().Value ( i ) ==
                    Style.XValues ().Value ( PrevPoint )  ) &&
                 (  Style.YValues ().Value ( i ) ==
                    Style.YValues ().Value ( PrevPoint )  )
            )
            A -> PolyMarker2 ( DrawCount, &_SetPoint, PrevPoint, &mp );
          else
            A -> PolyMarker1 ( DrawCount, &_SetPoint, PrevPoint, &mp );
        }  // end if
        DrawCount = 0;
      }  // end else
    }  // end for
    --i;
    if ( DrawCount ) {
      if (   (  Style.XValues ().Value ( i ) ==
                Style.XValues ().Value ( PrevPoint )  ) &&
             (  Style.YValues ().Value ( i ) ==
                Style.YValues ().Value ( PrevPoint )  )
        )
        A -> PolyMarker2 ( DrawCount, &_SetPoint, PrevPoint, &mp );
      else
        A -> PolyMarker1 ( DrawCount, &_SetPoint, PrevPoint, &mp );
    }  // end if
    A -> EndMarker ();
  } else A -> MarkerPoint (  P( Xpos ), P( Ypos )  );
  return Standard_True;
}

//=====================================================================
Standard_Boolean WNT_DDriver::PlotArc(const Standard_ShortReal X,
                                      const Standard_ShortReal Y,
                                      const Standard_ShortReal anXradius,
                                      const Standard_ShortReal anYradius,
                                      const Standard_ShortReal aStartAngle,
                                      const Standard_ShortReal anOpenAngle) 
{
  if ( A -> myPrimitive != zzNone && A -> myPrimitive != zzArc )
    ClosePrimitive ();
  if ( anOpenAngle >= 6.28318 )
    A -> Ellipse (  P( X ), P( Y ), P( anXradius ), P( anYradius )  );
  else
    A -> Arc (
      P( X ), P( Y ), P( anXradius ), P( anYradius ),
      aStartAngle, anOpenAngle
    );
  return Standard_True;
}

//=====================================================================
Standard_Boolean WNT_DDriver::PlotPolyArc(const Standard_ShortReal X,
                                          const Standard_ShortReal Y,
                                          const Standard_ShortReal anXradius,
                                          const Standard_ShortReal anYradius,
                                          const Standard_ShortReal aStartAngle,
                                          const Standard_ShortReal anOpenAngle) 
{
  if ( A -> myPrimitive != zzNone && A -> myPrimitive != zzPolySector )
    ClosePrimitive ();
  if ( anOpenAngle >= 6.28318 )
    A -> PolyEllipse (  P( X ), P( Y ), P( anXradius ), P( anYradius )  );
  else
    A -> PolySector (
      P( X ), P( Y ), P( anXradius ), P( anYradius ),
      aStartAngle, anOpenAngle
    );
  return Standard_True;
}

//=====================================================================
void WNT_DDriver::BeginPolyline(const Standard_Integer aNumber) 
{
  A -> myPrimitive = zzPolyline;
  A -> myNote      = A -> Polyline ( aNumber );
}

//=====================================================================
void WNT_DDriver::BeginPolygon(const Standard_Integer aNumber) 
{
  A -> myPrimitive = zzPolygon;
  A -> myNote      = A -> Polygon ( aNumber );
}

//=====================================================================
void WNT_DDriver::BeginSegments() 
{
  A -> myPrimitive = zzLine;
  A -> myNote      = NULL;  
}

//=====================================================================
void WNT_DDriver::BeginArcs() 
{
  A -> myPrimitive = zzArc;
  A -> myNote      = NULL;  
}

//=====================================================================
void WNT_DDriver::BeginPolyArcs() 
{
  A -> myPrimitive = zzPolySector;
  A -> myNote      = NULL;  
}

//=====================================================================
void WNT_DDriver::BeginMarkers() 
{
  A -> myPrimitive = zzMarker;
  A -> myNote      = NULL;  
}

//=====================================================================
void WNT_DDriver::BeginPoints() 
{
  A -> myPrimitive = zzPoint;
  A -> myNote      = NULL;  
}

//=====================================================================
void WNT_DDriver::ClosePrimitive() 
{
  A -> myPrimitive = zzNone;
  A -> myNote      = NULL;  
}

//=====================================================================
void WNT_DDriver::InitializeColorMap(const Handle(Aspect_ColorMap)& aColorMap) 
{
  Aspect_ColorMapEntry entry;
  Standard_Integer i, minindex = IntegerLast(), maxindex = -minindex;
  for (i = 1; i <= aColorMap->Size (); i++) {
    entry = aColorMap->Entry (i);
    maxindex = Max(maxindex, entry.Index ());
    minindex = Min(minindex, entry.Index ());
  }

  if (myColors.IsNull () || myColors -> Length () != aColorMap -> Size ())
    myColors = new WNT_HColorTable ( minindex, maxindex, (WNT_ColorRef)0x00000000 );
  myDevice -> MapColors ( aColorMap, myColors );
}

//=====================================================================
void WNT_DDriver::InitializeTypeMap(const Handle(Aspect_TypeMap)& aTypeMap) 
{
  Aspect_TypeMapEntry entry;
  Standard_Integer i, minindex = IntegerLast(), maxindex = -minindex;
  for (i = 1; i <= aTypeMap->Size (); i++) {
    entry = aTypeMap->Entry (i);
    maxindex = Max(maxindex, entry.Index ());
    minindex = Min(minindex, entry.Index ());
  }
  
  myTypeIdxs = new TColStd_HArray1OfInteger (minindex, maxindex, 1);
  for (i = 1; i <= aTypeMap->Size (); i++) {
    entry = aTypeMap->Entry (i);
    myTypeIdxs -> SetValue (entry.Index(), i);
  }
}

//=====================================================================
void WNT_DDriver::InitializeWidthMap(const Handle(Aspect_WidthMap)& aWidthMap) 
{
  Aspect_WidthMapEntry entry;
  Standard_Integer i, minindex = IntegerLast(), maxindex = -minindex;
  for (i = 1; i <= aWidthMap->Size (); i++) {
    entry = aWidthMap->Entry (i);
    maxindex = Max(maxindex, entry.Index ());
    minindex = Min(minindex, entry.Index ());
  }
  
  myWidthIdxs = new TColStd_HArray1OfInteger (minindex, maxindex, 1);
  for (i = 1; i <= aWidthMap->Size (); i++) {
    entry = aWidthMap->Entry (i);
    myWidthIdxs -> SetValue (entry.Index(), i);
  }
}

//=====================================================================
void WNT_DDriver::InitializeFontMap(const Handle(Aspect_FontMap)& aFontMap) 
{
  Aspect_FontStyle           fontStyle;
  Handle( WNT_FontMapEntry ) fmEntry;
  Standard_Character         left[ 260 ], right[ 250 ];
  Standard_PCharacter           pSize;
  Standard_Integer           height, mapSize = aFontMap -> Size ();
  Standard_Integer           i;
  HFONT                      hfo =
    SelectFont((::HDC)myHDC, GetStockObject(DEFAULT_GUI_FONT));
/*====================== MFT ====================*/
#ifdef MFT
  Aspect_FontMapEntry     entry;
  Aspect_FontStyle        style;
  Quantity_Length         size;
  Quantity_PlaneAngle     slant;
  TCollection_AsciiString aname;
  if ( UseMFT() ) {
    Handle(MFT_FontManager) theFontManager;
    myMFTFonts  = new WNT_HListOfMFTFonts (0, mapSize, theFontManager);
    myMFTSizes  = new TShort_HArray1OfShortReal (0, mapSize, 0.F);
  }
#endif
/*====================== MFT ====================*/
  myFonts = new WNT_HFontTable ( 0, mapSize - 1 );
  for ( i = 0; i < mapSize; ++i ) {
/*====================== MFT ====================*/
#ifdef MFT
  if ( UseMFT() ) {
    entry  = aFontMap->Entry(i+1);
    style  = entry.Type ();
    size   = (Quantity_Length) TOMILLIMETER (style.Size());
    slant  = style.Slant ();
    if (style.CapsHeight())  size = -size;
    aname  = style.AliasName ();
    Standard_Boolean found = MFT_FontManager::IsKnown (aname.ToCString());
    if (!found) {
      cout << " WNT_DDriver::SetFontMap can't find the MFT font name '"
        << aname << "', using 'Defaultfont'" << endl << flush;
      aname = TCollection_AsciiString("Defaultfont");
      found = Standard_True;
    }
    if (found) {
      Handle(MFT_FontManager) theFontManager;
      Aspect_FontStyle        theStyle = MFT_FontManager::Font (aname.ToCString());
      found = Standard_False;
      for (int k = 0; k < mapSize; k++) {
        theFontManager = myMFTFonts -> Value (k);
        if (!theFontManager.IsNull()) {
          if (theStyle == theFontManager->Font()) {
            found = Standard_True;
            break;
          }
        }
      }
      if (!found)
        theFontManager = new MFT_FontManager (aname.ToCString());
        theFontManager->SetFontAttribs (
          Abs(size), Abs(size), slant, 0., Standard_Boolean(size < 0.)
        );
      myMFTFonts->SetValue (i, theFontManager);
      myMFTSizes->SetValue (i, (Standard_ShortReal)size);
    }
  }
  if (UseMFT()  && !myMFTFonts->Value(i).IsNull())
    continue;
#endif
/*====================== MFT ====================*/
  fontStyle = aFontMap -> Entry ( i + 1 ).Type ();
  height    = Standard_Integer (  fontStyle.Size () / myPixelToUnit );
  lstrcpyA ( right,"-*-*-*-*-*-*-*-*-*-*-*-*-" );
  switch (  fontStyle.Style ()  ) {

  case Aspect_TOF_HELVETICA:
    
    lstrcatA ( right,"Arial" );
    
    break;

  case Aspect_TOF_DEFAULT:
  case Aspect_TOF_COURIER:
    
    lstrcatA ( right, "Courier New" );
    
    break;

  case Aspect_TOF_TIMES:

    lstrcatA ( right,"Times New Roman" );
    
    break;

  case Aspect_TOF_USERDEFINED:

    lstrcpyA (  left, fontStyle.Value ()  );

    if (   strchr (  fontStyle.Value (), '-'  )   ) {

      height = atol (  pSize = strtok ( left, "-" )  );

      if ( height == 0 )
        
        height = Standard_Integer (  atof ( pSize ) / myPixelToUnit  );

      if ( height == 0 )
        
        height = Standard_Integer (  fontStyle.Size () / myPixelToUnit  );

      pSize  = left + lstrlenA ( left );
      *pSize = '-';
      lstrcpyA ( right, pSize );
      
    } else lstrcatA ( right,left );
    
  }  // end switch

  height = Standard_Integer ( height * W32_TextFactor );
  ltoa ( height, left, 10 );
  lstrcatA ( left, right );
  
  fmEntry = new WNT_FontMapEntry ( left );

  if (  fontStyle.CapsHeight ()  ) {
    
    double     r;
    TEXTMETRIC tm;
    
    SelectFont(  (::HDC)myHDC, fmEntry -> HFont ()  );

    GetTextMetrics ( (::HDC)myHDCMeta, &tm );

    r = tm.tmAscent * tm.tmHeight / ( tm.tmAscent - tm.tmInternalLeading );
    lstrcpy (  right, strchr ( left, '-' )  );
    ltoa (  LONG( r + 0.5 ), left, 10  );
    lstrcatA ( left, right );
    
    fmEntry = new WNT_FontMapEntry ( left );
    
  }  // end if

  myFonts -> SetValue ( i, fmEntry );

 }  // end for

 SelectFont( (::HDC)myHDC, hfo );
}

//=====================================================================
void WNT_DDriver::InitializeMarkMap(const Handle(Aspect_MarkMap)& aMarkMap) 
{
  Aspect_MarkMapEntry entry;
  Standard_Integer i, minindex = IntegerLast(), maxindex = -minindex;
  for (i = 1; i <= aMarkMap->Size (); i++) {
    entry = aMarkMap->Entry (i);
    maxindex = Max(maxindex, entry.Index ());
    minindex = Min(minindex, entry.Index ());
  }
  
  myMarkerIdxs = new TColStd_HArray1OfInteger (minindex, maxindex, -1);
  for (i = 1; i <= aMarkMap->Size (); i++) {
    entry = aMarkMap->Entry (i);
    myMarkerIdxs -> SetValue (entry.Index(), i);
  }
}

//=====================================================================
void WNT_DDriver::WorkSpace(Quantity_Length& Width,
                            Quantity_Length& Heigth) const
{
  Width  = U ( ((PRECT)myRect) -> right );
  Heigth = U ( ((PRECT)myRect) -> bottom);

  if ( myFlags & FLAG_ROTATE ) {
    Quantity_Length tlen;
    tlen   = Heigth;
    Heigth = Width;
    Width  = tlen;
  }  // end if
}

//=====================================================================
Quantity_Length WNT_DDriver::Convert(const Standard_Integer PV) const
{
  return U( PV );
}

//=====================================================================
Standard_Integer WNT_DDriver::Convert(const Quantity_Length DV) const
{
  return P( DV );
}

//=====================================================================
void WNT_DDriver::Convert(const Standard_Integer PX,
                          const Standard_Integer PY,
                          Quantity_Length& DX,
                          Quantity_Length& DY) const
{
  DX = U( PX                           );
  DY = U( ((PRECT)myRect)->bottom - PY );
}

//=====================================================================
void WNT_DDriver::Convert(const Quantity_Length DX,
                          const Quantity_Length DY,
                          Standard_Integer& PX,
                          Standard_Integer& PY) const
{
  PX = P( DX );
  PY = P( DY );
}

//=====================================================================
void WNT_DDriver::TextSize(const TCollection_ExtendedString& aText,
                           Standard_ShortReal& aWidth,
                           Standard_ShortReal& aHeight,
                           const Standard_Integer aFontIndex) const
{
#ifdef PRO17381
  if (aText.Length() <= 0) {
    aWidth = aHeight = 0.;
    return;
  }
#endif

#ifdef MFT
  if ( UseMFT() ) {
    Standard_ShortReal xoffset, yoffset;
    TextSize (aText, aWidth, aHeight, xoffset, yoffset, aFontIndex);
  } else {
#endif
    char* str = _ConvertU2F ( aText );
    TextSize ( str, aWidth, aHeight, aFontIndex );
    HeapFree (  GetProcessHeap (), 0, str  );
#ifdef MFT
  }
#endif
}

//=====================================================================
void WNT_DDriver::TextSize(const TCollection_ExtendedString& aText,
                           Standard_ShortReal& aWidth,
                           Standard_ShortReal& aHeight,
                           Standard_ShortReal& anXoffset,
                           Standard_ShortReal& anYoffset,
                           const Standard_Integer aFontIndex) const
{
#ifdef PRO17381
  if (aText.Length() <= 0) {
    aWidth = aHeight = 0.;
    return;
  }
#endif

#ifdef MFT
  if ( UseMFT() ) {
    Standard_Integer aFont = (aFontIndex < 0 ?
      (int)A->myTextFont : aFontIndex);
    Handle(MFT_FontManager) theFontManager =
      ((WNT_DDriver *const)(this))->MFT_Font(aFont);
    Standard_Real theWidth = 0.,theAscent = 0.,
                  theLbearing = 0.,theDescent = 0.;
    if (aText.IsAscii()) {
      TCollection_AsciiString ascii (aText,'?');
      theFontManager->TextSize(ascii.ToCString(), theWidth,
                               theAscent, theLbearing, theDescent);
    } else if (theFontManager->IsComposite()) {
      theFontManager->TextSize(aText.ToExtString(), theWidth,
                               theAscent, theLbearing, theDescent);
    }
    theDescent = Max(
      theDescent, (A->myFlags & W32F_TULIN ?
                   theFontManager->UnderlinePosition() : 0.));
    aWidth    = Standard_ShortReal (theWidth              );
    aHeight   = Standard_ShortReal (theAscent + theDescent);
    anXoffset = Standard_ShortReal (theLbearing           );
    anYoffset = Standard_ShortReal (theDescent            );
  } else {
#endif
    char* str = ( char* )_ConvertU2F ( aText );
    HFONT      hfo;
    SIZE       sz;
    TEXTMETRIC tm;
    int        len;
    double     tanVal = tan ( A -> myTextSlant );
    hfo = SelectFont(
      (::HDC)myHDC,
      aFontIndex < 0 ? A -> myTextFont : ( HFONT )myFonts -> Value ( aFontIndex ) -> HFont ()
    );
    GetTextExtentPoint32A (
      (::HDC)myHDC, str, len = lstrlenA ( str ), &sz
    );
    GetTextMetrics ( (::HDC)myHDC, &tm );
    sz.cy -= tm.tmInternalLeading;
    aWidth  = Standard_ShortReal (  U( sz.cx * A -> myTextHScale )  );
    aHeight = Standard_ShortReal (  U( sz.cy * A -> myTextVScale )  );
    aWidth += Standard_ShortReal ( tanVal * aHeight );
    anXoffset = Standard_ShortReal ( 0.0 );
    anYoffset = Standard_ShortReal ( 0.0 );
    SelectFont( (::HDC)myHDC, hfo );
    HeapFree (  GetProcessHeap (), 0, str  );
#ifdef MFT
  }
#endif
}

//=====================================================================
void WNT_DDriver::TextSize(const Standard_CString aText,
                           Standard_ShortReal& aWidth,
                           Standard_ShortReal& aHeight,
                           Standard_ShortReal& anXoffset,
                           Standard_ShortReal& anYoffset,
                           const Standard_Integer aFontIndex) const
{
#ifdef PRO17381
  if (!aText || (strlen(aText) <= 0)) {
    aWidth = aHeight = anXoffset = anYoffset = 0.;
    return;
  }
#endif

#ifdef MFT
  if ( UseMFT() ) {
    TCollection_ExtendedString extText (aText);
    TextSize (extText, aWidth, aHeight, anXoffset, anYoffset, aFontIndex);
  } else {
#endif
    HFONT hfo;
    SIZE  sz;
    hfo = SelectFont(
      (::HDC)myHDC,
      aFontIndex < 0 ? A -> myTextFont : ( HFONT )myFonts -> Value ( aFontIndex ) -> HFont ()
    );
    GetTextExtentPoint32A (
      (::HDC)myHDC, aText, lstrlenA ( aText ), &sz
    );
    aWidth  = Standard_ShortReal ( U( sz.cx ) * A -> myTextHScale );
    aHeight = Standard_ShortReal ( U( sz.cy ) * A -> myTextVScale );
    aWidth  = Standard_ShortReal (  aWidth + tan ( A -> myTextSlant ) * aHeight  );
    SelectFont( (::HDC)myHDC, hfo );
#ifdef MFT
  }
#endif
}

//=====================================================================
Aspect_Handle WNT_DDriver::HDC() const
{
  return myHDCMeta;
}

//=====================================================================
void WNT_DDriver::ClientRect(Standard_Integer& aWidth,
                             Standard_Integer& aHeigth) const
{
  aWidth  = (  ( PRECT )myRect  ) -> right;
  aHeigth = (  ( PRECT )myRect  ) -> bottom;
}

//=====================================================================
Handle(WNT_GraphicDevice) WNT_DDriver::GraphicDevice() const
{
  return myDevice;
}

//=====================================================================
static BOOL IsWindows95 () {
  OSVERSIONINFO os;
  os.dwOSVersionInfoSize = sizeof ( OSVERSIONINFO );
  GetVersionEx (&os);
  return (os.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS);
}

//=====================================================================
Handle(TColStd_HSequenceOfAsciiString) WNT_DDriver::DeviceList() 
{
  char                                     buff[ MAX_PATH ];
  DWORD                                    dwLen, dwLenRet;
  TCollection_AsciiString                  dev, defDev;
  Handle( TColStd_HSequenceOfAsciiString ) retVal =
    new TColStd_HSequenceOfAsciiString ();
  buff[ dwLen =
    GetProfileString ( "Windows", "Device", ",,,", buff, MAX_PATH ) ] = '\x00';
  defDev = TCollection_AsciiString ( buff );
  defDev = defDev.Token ( ",", 1 );
  dwLen  = 0;
  if (IsWindows95()) {
    ///////////////////////// W95 ////////////////////////////
    PRINTER_INFO_5*                           ppi = NULL;
    if (!EnumPrinters (
           PRINTER_ENUM_LOCAL,
           NULL, 5, NULL, 0, &dwLen, &dwLenRet
         ))
    {
      if (GetLastError () == ERROR_INSUFFICIENT_BUFFER)
      {
        ppi = ( PRINTER_INFO_5* )HeapAlloc (  GetProcessHeap (), HEAP_GENERATE_EXCEPTIONS, dwLen  );
        EnumPrinters (
          PRINTER_ENUM_LOCAL,
          NULL, 5, ( LPBYTE )ppi, dwLen, &dwLen, &dwLenRet
        );
      } else {
        wsprintf ( buff, "W95_DDriver::Could not obtain device list (%d)", GetLastError () );
//        Aspect_DriverError :: Raise ( buff );
        dwLenRet = 0;
      }  // end else
    }
    for (dwLen = 0; dwLen < dwLenRet; ++ dwLen) {
      dev = TCollection_AsciiString ( ppi[ dwLen ].pPrinterName );
      if (  dev.IsEqual (defDev)  )
        retVal -> Prepend ( dev );
      else
        retVal -> Append ( dev );
    }  // end for
//    retVal -> Append (TCollection_AsciiString ( "DISPLAY" ));
    if (ppi != NULL)
      HeapFree (  GetProcessHeap (), 0, ( LPVOID )ppi  );
  } else {
    ///////////////////////// WNT ////////////////////////////
    PRINTER_INFO_4*                           ppi = NULL;
    if (!EnumPrinters (
           PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS,
           NULL, 4, NULL, 0, &dwLen, &dwLenRet
         ))
    {
      if (GetLastError () == ERROR_INSUFFICIENT_BUFFER)
      {
        ppi = ( PRINTER_INFO_4* )HeapAlloc (  GetProcessHeap (), HEAP_GENERATE_EXCEPTIONS, dwLen  );
        EnumPrinters (
          PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS,
          NULL, 4, ( LPBYTE )ppi, dwLen, &dwLen, &dwLenRet
        );
      } else {
        wsprintf ( buff, "WNT_DDriver::Could not obtain device list (%d)", GetLastError () );
//        Aspect_DriverError :: Raise ( buff );
        dwLenRet = 0;
      }  // end else
    }
    for (dwLen = 0; dwLen < dwLenRet; ++ dwLen) {
      dev = TCollection_AsciiString ( ppi[ dwLen ].pPrinterName );
      if (  dev.IsEqual (defDev)  )
        retVal -> Prepend ( dev );
      else
        retVal -> Append ( dev );
    }  // end for
//    retVal -> Append (TCollection_AsciiString ( "DISPLAY" ));
    if (ppi != NULL)
      HeapFree (  GetProcessHeap (), 0, ( LPVOID )ppi  );
  } // WINNT
  return retVal;
}

//=====================================================================
void WNT_DDriver::DeviceSize(const TCollection_AsciiString& aDevice,
                             Standard_Real& aWidth,
                             Standard_Real& aHeight)
{
  ::HDC hdc;
  aWidth = aHeight = 0.0;

  if (!lstrcmpi (aDevice.ToCString(), "DISPLAY"))
    hdc = CreateDC ("DISPLAY", NULL, NULL, NULL);
  else
    hdc = CreateDC ("WINSPOOL", aDevice.ToCString(), NULL, NULL);
  if (hdc != NULL) {
    aWidth  = ((Standard_Real)_dd_dev_size (hdc, HORZSIZE) / 10.);
    aHeight = ((Standard_Real)_dd_dev_size (hdc, VERTSIZE) / 10.);
    DeleteDC (hdc);
  }
}

//=====================================================================
Standard_Real WNT_DDriver :: EMFDim (
                              Standard_Integer& aWidth,
                              Standard_Integer& aHeight,
                              Standard_Integer& aSwap
                             ) {

 ENHMETAHEADER emh;
 Standard_Real retVal = -1.0;

 aWidth = aHeight = -1;
 aSwap  = 2;

 if ( myHMetaFile == NULL ) {

  TCollection_AsciiString fName;

  if (    (   myHMetaFile = GetEnhMetaFile (  myFileName.ToCString ()  )   ) == NULL    )

   goto ret;

 }  // end if

 if (  GetEnhMetaFileHeader (
        ( HENHMETAFILE )myHMetaFile, sizeof ( ENHMETAHEADER ), &emh
       ) == 0
 ) goto ret;

 aWidth  = emh.rclFrame.right  - emh.rclFrame.left;
 aHeight = emh.rclFrame.bottom - emh.rclFrame.top;

 retVal = Standard_Real ( aWidth ) / Standard_Real ( aHeight );

 {  // begin block

  ::HDC hdc = GetDC ( NULL );
   DWORD d   ='U';
   RECT  r   = { 0, 0, GetDeviceCaps ( hdc, HORZRES ), GetDeviceCaps ( hdc, VERTRES ) };

    EnumEnhMetaFile (
     hdc, ( HENHMETAFILE )myHMetaFile, &_dd_enum_proc, &d, &r
    );

   switch ( d ) {

    case 'T':

     aSwap = 1;

    break;

    case 'F':

     aSwap = 0;

    break;

   }  // end switch

  ReleaseDC ( NULL, hdc );

 }  // end block
ret:
 return retVal;

}  // end WNT_DDriver :: EMFDim

//=====================================================================
Standard_Integer WNT_DDriver::ProcessColorIndex
                 (const Standard_Integer ColorIndex) const
{
  if ( (     A -> myFlags & W32F_MONO  ) &&
       (  !( A -> myFlags & W32F_MINIT ) || ColorIndex <= 0 )
     )
    return A -> myPointColor;
  if (  myColors.IsNull ()  )
    return ( Standard_Integer )RGB( 0, 0, 0 );
  if (ColorIndex <= 0) {
    LOGBRUSH lb;
    GetObject (  GetStockObject ( NULL_BRUSH ), sizeof ( LOGBRUSH ), &lb  );
    return lb.lbColor;
  } else {
    if (myFlags & FLAG_BANDW)
      return 0;
    if (ColorIndex < myColors->Lower() || ColorIndex > myColors->Upper())
      return myColors -> Value ( myColors->Lower() );
    return myColors -> Value ( ColorIndex );
  }
}

//=====================================================================
Quantity_Length WNT_DDriver::ProcessWidthIndex
                (const Standard_Integer WidthIndex) const
{
  Standard_Integer theIndex = WidthIndex;
  if (WidthIndex < myWidthIdxs->Lower() || WidthIndex > myWidthIdxs->Upper())
    theIndex = myWidthIdxs->Lower();

  Aspect_WidthMapEntry Entry = WidthMap () -> Entry ( myWidthIdxs -> Value(theIndex) );
  return (  Entry.Type () == Aspect_WOL_THIN  ) ?
         ( Quantity_Length )0.0 : Entry.Width () / myPixelToUnit;
}

//=====================================================================
Standard_Integer WNT_DDriver::ProcessTypeIndex
                 (const Standard_Integer TypeIndex) const
{
 return (TypeMap()->Entry(TypeIndex+1).Type().Style() == Aspect_TOL_SOLID) ?
         0 : PS_USERSTYLE;
}

//=====================================================================
const Handle(WNT_TextManager)& WNT_DDriver :: TextManager ()
{
  return (myNTextManager);
}

//=====================================================================
const Handle(MFT_FontManager)& WNT_DDriver :: MFT_Font
                               (const Standard_Integer anIndex)
{
  return myMFTFonts->Value(anIndex);
}

//=====================================================================
Standard_ShortReal WNT_DDriver :: MFT_Size
                               (const Standard_Integer anIndex)
{
  return myMFTSizes->Value(anIndex);
}

//=====================================================================
//=====================================================================
//=====================================================================
static void WINAPI _SetPoint ( int s, int m, int set, LPPOINT p, int* n, void* param ) {

 PMPARAM pmp = ( PMPARAM )param;

 *n = 0;

 for (  int i = 0; i < m; ++i, ++( *n ), ++s  ) {

  p[ i ].x = LONG (  pmp -> s -> XValues ().Value ( s ) * pmp -> w / 2.0 + 0.5  );
  p[ i ].y = LONG (  pmp -> s -> YValues ().Value ( s ) * pmp -> h / 2.0 + 0.5  );

 }  // end for

}  // end _SetPoint

static PW32_Allocator __fastcall _FindAllocator ( Standard_Address p, Standard_Integer id ) {

 while (   p != NULL && (  ( PW32_Allocator )p  ) -> myID != id   )

  p = (  ( PW32_Allocator )p  ) -> myNext;

 return ( PW32_Allocator )p;

}  // end _FindAllocator

static char* __fastcall _ConvertU2F ( const TCollection_ExtendedString& str ) {

 char*            retVal;
 Standard_Integer szBuff = (  str.Length () << 2  );

 retVal = ( char* )HeapAlloc (
                    GetProcessHeap (), HEAP_GENERATE_EXCEPTIONS,
                    szBuff
                   );

 while (  !Resource_Unicode :: ConvertUnicodeToFormat ( str, retVal, szBuff )  ) {
 
  szBuff <<= 1;
  retVal = ( char* )HeapReAlloc (
                     GetProcessHeap (), HEAP_GENERATE_EXCEPTIONS,
                     retVal, szBuff
                    );
 
 }  // end while

 return retVal;

}  // end _ConvertU2F

static void __fastcall _dd_fill_buff ( char* buffer ) {

 time_t t;
 struct tm* lt;
 char*  p;
 DWORD  dwSize = 32;

 lstrcpy ( buffer, "CAS.CADE driver <" );
 
 GetUserName (  &buffer[ lstrlen ( buffer ) ], &dwSize  );

 lstrcat ( buffer, "@" );

 dwSize = 24;
 GetComputerName (  &buffer[ lstrlen ( buffer ) ], &dwSize  );

 lstrcat ( buffer, ">" );

 t  = time ( NULL );
 lt = localtime ( &t );
 p  = asctime ( lt );
 CopyMemory (
  &buffer[ lstrlen ( buffer ) + 1 ], p, lstrlen ( p ) - 1
 );
 
}  // end _dd_fill_buff

static int __fastcall _dd_dev_size ( HDC hdc, int index ) {

 int  retVal = -1;
 HWND hwnd   = WindowFromDC ( hdc );

 if ( hwnd != NULL ) {

  HDC  hdcDisplay = GetDC ( NULL );
  RECT r;

  GetClientRect ( hwnd, &r );

  switch ( index ) {

   case HORZRES:

    retVal = r.right;

   break;

   case VERTRES:

    retVal = r.bottom;

   break;

   case HORZSIZE:

    retVal = GetDeviceCaps ( hdcDisplay, HORZSIZE ) * r.right /
             GetDeviceCaps ( hdcDisplay, HORZRES  );

   break;

   case VERTSIZE:

    retVal = GetDeviceCaps ( hdcDisplay, VERTSIZE ) * r.bottom /
             GetDeviceCaps ( hdcDisplay, VERTRES  );

   break;

  }  // end switch

  ReleaseDC ( NULL, hdcDisplay );

 } else retVal = GetDeviceCaps ( hdc, index );

 return retVal;

}  // end _dd_dev_size

static int CALLBACK _dd_enum_proc (
                     HDC                         hDC,
                     HANDLETABLE FAR*       lpHTable,
                     CONST ENHMETARECORD FAR* lpEMFR,
                     int                        nObj,
                     LPARAM                   lpData
                    ) {

 int retVal = 1;

 if (  lpEMFR -> iType      == EMR_GDICOMMENT      &&
       lpEMFR -> dParm[ 0 ] == sizeof ( CSF_SIGN ) &&
       lpEMFR -> dParm[ 1 ] == CSF_SIGNATURE
 ) {

  *(  ( DWORD* )lpData  ) = lpEMFR -> dParm[ 3 ];
  retVal = 0;

 }  // end if

 return retVal;

}  // end _dd_enum_proc

/*
static TCollection_AsciiString _dd_temp_name ( void ) {

 HKEY    hKey;
 TCHAR   tmpPath[ MAX_PATH ];
 BOOL    fOK = FALSE;
 REG_KEY regKey[ 2 ] = {
 
                 { HKEY_LOCAL_MACHINE,
                   TEXT( "SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment" )
                 },
                 { HKEY_USERS,
                   TEXT( ".DEFAULT\\Environment" )
                 }
 
                };
 
 for ( int i = 0; i < 2; ++i ) {

  if (  RegOpenKeyEx (
         regKey[ i ].hKey, regKey[ i ].keyPath, 0, KEY_QUERY_VALUE, &hKey
       ) == ERROR_SUCCESS
  ) {
  
   DWORD dwType;
   DWORD dwSize = 0;

   if (  RegQueryValueEx (
          hKey, TEXT( "TEMP" ), NULL, &dwType, NULL, &dwSize
         ) == ERROR_SUCCESS
   ) {
  
    LPTSTR kVal = ( LPTSTR )HeapAlloc (
                             GetProcessHeap (), HEAP_ZERO_MEMORY | HEAP_GENERATE_EXCEPTIONS,
                             dwSize + sizeof ( TCHAR )
                            );

     RegQueryValueEx (  hKey, TEXT( "TEMP" ), NULL, &dwType, ( LPBYTE )kVal, &dwSize  );

     if ( dwType == REG_EXPAND_SZ )
    
      ExpandEnvironmentStrings ( kVal, tmpPath, MAX_PATH );

     else

      lstrcpy ( tmpPath, kVal );

    HeapFree (  GetProcessHeap (), 0, ( LPVOID )kVal  );
    fOK = TRUE;

   }  // end if

   RegCloseKey ( hKey );

  }  // end if

  if ( fOK ) break;

 }  // end for

 if ( !fOK ) lstrcpy (  tmpPath, TEXT( "./" )  );
 
 GetTempFileName ( tmpPath, "CSF", 0, tmpPath );

 return tmpPath;

}  // end _dd_temp_name
*/
