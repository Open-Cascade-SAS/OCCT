// File:        WNT_MFTDraw.cxx
// Created:     March 1998
// Author:      CHABROVSKY Dmitry
// Copyright:   Matra Datavision 1998

#define MFT // Study S3553

#ifdef MFT

#define STRICT
#include <windows.h>
#include <windowsx.h>
#include "WNT_MFTDraw.hxx"
#include "W32_Allocator.hxx"
#include "W95_Allocator.hxx"
#include "WNT_Allocator.hxx"
#include <WNT_WDriver.hxx>
#include <WNT_DDriver.hxx>
#include <WNT_TextManager.hxx>

#ifdef DrawText
# undef DrawText
#endif // DrawText

#include <MFT_FontManager.hxx>
#include <TCollection_ExtendedString.hxx>

#define ROUNDL( d ) ( ( long )( ( d ) + ( ( d ) > 0 ? 0.5 : -0.5 ) ) )
#define TRANSFORM(i,X,Y,f95) \
  { \
    Standard_Real x = (X),y = (Y); \
    pts[(i)].x          = P( ROUNDL((double)myPT.x + x*Ca - y*Sa) );\
    if (f95) pts[(i)].y = sz.cy - P( ROUNDL((double)myPT.y + x*Sa + y*Ca) );\
    else     pts[(i)].y = P( ROUNDL((double)myPT.y + x*Sa + y*Ca) );\
  }
#define SET_DRAW_DATA(aData,aDC,aSZ) \
  { aData.theHDC       = aDC;    \
    aData.theDevWidth  = aSZ.cx; \
    aData.theDevHeight = aSZ.cy; }

void __cdecl _Do_MFTDrawText_ (PW32_FCALLPARAM lpParam)
{
  // Declare all needed variables
  PMFT_DRAWTEXT  lpDraw = (PMFT_DRAWTEXT)  lpParam;
  PW32_Allocator lpW32  = (PW32_Allocator) lpDraw->theParam.pAllocator;
  WNT_WDriver*   lpWDrv = (WNT_WDriver*)   lpDraw->theParam.pDriver;
  WNT_DDriver*   lpDDrv = (WNT_DDriver*)   lpDraw->theParam.pDriver;
  int            aFont  = (int)lpW32->myTextFont;
  double         myPixelToUnit = lpDraw->theP2U;
  XFORM          xf;
  HDC       MY_HDC;
  HPEN      hTextPen,   hOldPen,   hPolyPen;
  HBRUSH    hTextBrush, hOldBrush, hPolyBrush;
  float     Ca = (float)Cos(lpW32->myAngle),
            Sa = (float)Sin(lpW32->myAngle);
  SIZE      sz;
  // Starting point to draw
  POINT     myPT = {
    ROUNDL(
      float(
        Ca*lpW32->myScaleX * (lpDraw->theXPos - float( U(lpW32->myPivot.x)) ) -
        Sa*lpW32->myScaleY * (lpDraw->theYPos - float( U(lpW32->myPivot.y)) ) +
        float( U( (lpW32->myPivot.x + lpW32->myMove.x) ) )
      )
    ),
    ROUNDL(
      float(
        Sa*lpW32->myScaleX * (lpDraw->theXPos - float( U(lpW32->myPivot.x)) ) +
        Ca*lpW32->myScaleY * (lpDraw->theYPos - float( U(lpW32->myPivot.y)) ) +
        float( U( (lpW32->myPivot.y + lpW32->myMove.y) ) )
      )
    )
  };
  double    uPos;
  Handle(WNT_TextManager) theTextManager;
  Handle(MFT_FontManager) theFontManager;
  Standard_ShortReal      theTextSize;
  //============ Use driver to get some data ============
  if (lpW32->myFlags & W32F_DVDRV) {
//    theTextManager = lpDDrv->TextManager ();
//    theFontManager = lpDDrv->MFT_Font    (aFont);
//    theTextSize    = lpDDrv->MFT_Size    (aFont);
  } else {
    theTextManager = lpWDrv->TextManager ();
    theFontManager = lpWDrv->MFT_Font    (aFont);
    theTextSize    = lpWDrv->MFT_Size    (aFont);
  }
  //============== Set FontManager and TextManager attributes =============
  theFontManager->SetFontAttribs (
    Abs(theTextSize)*lpW32->myTextHScale*lpW32->myScaleX,
    Abs(theTextSize)*lpW32->myTextVScale*lpW32->myScaleY,
    lpW32->myTextSlant, 0.,
    ((theTextSize > 0.) ? Standard_False : Standard_True)
  );
  uPos = (lpW32->myFlags & W32F_TULIN ?
            theFontManager->UnderlinePosition() : 0.);
  theTextManager->SetTextAttribs (
    0, Aspect_TypeOfText(lpDraw->theTextType), uPos
  );
  //=============== Calculate UpdateRect  ================
  Ca = (float)Cos(lpDraw->theAngle + lpW32->myAngle);
  Sa = (float)Sin(lpDraw->theAngle + lpW32->myAngle);
  Standard_ShortReal theHeight, theWidth, theXoffset, theYoffset;
  if (lpW32->myFlags & W32F_DVDRV) {
    if (!lpDraw->isTextWide)
      lpDDrv->TextSize ((char*)lpDraw->theText, theWidth, theHeight, theXoffset,
                        theYoffset, aFont);
    else
      lpDDrv->TextSize ((short*)lpDraw->theText, theWidth, theHeight, theXoffset,
                        theYoffset, aFont);
  } else {
    if (!lpDraw->isTextWide)
      lpWDrv->TextSize ((char*)lpDraw->theText, theWidth, theHeight, theXoffset,
                        theYoffset, aFont);
    else
      lpWDrv->TextSize ((short*)lpDraw->theText, theWidth, theHeight, theXoffset,
                        theYoffset, aFont);
  }
  //================= Get some attributes ===============
  int uWidth = P(theHeight/32.);
  MFT_TEXTMAN_DATA aData = { 0, TRUE, 0, 0, uWidth, lpW32->myFlags & W32F_MONO };
  if (lpW32->myFlags & W32F_WIN95) {
    // Get necessary attributes from WIN95 Allocator
    PW95_Allocator lpW95 = (PW95_Allocator)lpW32;
    lpW95->myHDC.Extent(&sz);
    MY_HDC = lpW95->myHDC.Hdc ();
    SET_DRAW_DATA (aData, MY_HDC, sz);
    theTextManager->SetDrawAttribs     ( (Standard_Address)&aData );
    hTextPen     = CreatePen           ( PS_SOLID, 1, lpW95->myTextColor );
    hTextBrush   = CreateSolidBrush    ( lpW95->myTextColor );
    hPolyPen     = ((lpW32->myFlags & W32F_POUTL) ?
                      CreatePen (PS_SOLID, lpW95->myLinePenWidth,
                                 lpW95->myLineBrush.lbColor):
                      (HPEN) GetStockObject (NULL_PEN));
    hPolyBrush   = ((lpW32->myFlags & W32F_NOFIL) ?
                      (HBRUSH) GetStockObject (NULL_BRUSH):
                      CreateBrushIndirect (&lpW95->myPolyBrush));
  } else {
    // Get necessary attributes from WINNT Allocator
    PWNT_Allocator lpWNT = (PWNT_Allocator)lpW32;
    lpWNT->GetExtent(&sz);
    MY_HDC         = lpWNT->myHDC;
    aData.theWin95 = FALSE;
    SET_DRAW_DATA (aData, MY_HDC, sz);
    theTextManager->SetDrawAttribs ( (Standard_Address)&aData );
    GetWorldTransform              ( MY_HDC, &xf );
    ModifyWorldTransform           ( MY_HDC, NULL, MWT_IDENTITY );
    hTextPen     = lpWNT->myTextPen;
    hTextBrush   = lpWNT->myTextBrush;
    hPolyPen     = ((lpW32->myFlags & W32F_POUTL) ?
                      lpWNT->myLinePen :
                      (HPEN)   GetStockObject (NULL_PEN)  );
    hPolyBrush   = ((lpW32->myFlags & W32F_NOFIL) ?
                      (HBRUSH) GetStockObject (NULL_BRUSH):
                      lpWNT->myPolyBrush);
  }
  ////////////////////////////////////////////////////////
  Standard_ShortReal marge =
    Standard_ShortReal(theHeight * lpDraw->theMargin);
  theWidth   += 2.F*marge;   theHeight  += 2.F*marge;
  theXoffset -= marge;       theYoffset += marge;
  ////////////////////////////////////////////////////////
  BOOL  fWin95 = lpW32->myFlags & W32F_WIN95;
  POINT pts[4];
  TRANSFORM (0, theXoffset,                      - theYoffset, fWin95);
  TRANSFORM (1, theXoffset + theWidth,           - theYoffset, fWin95);
  TRANSFORM (2, theXoffset + theWidth, theHeight - theYoffset, fWin95);
  TRANSFORM (3, theXoffset,            theHeight - theYoffset, fWin95);
  //============ Draw POLYGON around the text ===========
  if (lpDraw->isTextPoly) {
    hOldPen   =  SelectPen   (MY_HDC, hPolyPen  );
    hOldBrush =  SelectBrush (MY_HDC, hPolyBrush);
    Polygon      (MY_HDC, pts, 4);
    SelectBrush  (MY_HDC, hOldBrush);
    SelectPen    (MY_HDC, hOldPen  );
  }
  //================= Draw text itself ==================
  hOldPen      = SelectPen   (MY_HDC, hTextPen);
  hOldBrush    = SelectBrush (MY_HDC, hTextBrush);
  if (lpDraw->isTextWide) {
    theFontManager->DrawText (
      theTextManager, (short*)lpDraw->theText,
      myPT.x, myPT.y, (lpDraw->theAngle + lpW32->myAngle)
    );
  } else {
    theFontManager->DrawText (
      theTextManager, (char*)lpDraw->theText,
      myPT.x, myPT.y, (lpDraw->theAngle + lpW32->myAngle)
    );
  }
  SelectBrush (MY_HDC, hOldBrush);
  SelectPen   (MY_HDC, hOldPen  );
  //========= Register points as an UpdatedRect =========
  if (lpW32->myFlags & W32F_WIN95) {
    PW95_Allocator lpW95 = (PW95_Allocator)lpW32;
    lpW95->myHDC.Register  ( pts, 4 );
    DeletePen              ( hTextPen   );
    DeleteBrush            ( hTextBrush );
    DeletePen              ( hPolyPen   );
    DeleteBrush            ( hPolyBrush );
  } else {
    PWNT_Allocator lpWNT = (PWNT_Allocator)lpW32;
    lpWNT->Register        ( pts, 4 );
    SetWorldTransform      ( MY_HDC, &xf );
  }
  //=====================================================
/*
  TCollection_ExtendedString extText;
  if (lpDraw->isTextWide) {
    extText = TCollection_ExtendedString((short*)lpDraw->theText);
  } else {
    extText = TCollection_AsciiString   ((char*) lpDraw->theText);
  }
  cout << "DRAW_TEXT: " << (int)lpW32->myTextFont << ", "
       << lpDraw->theXPos << ", " << lpDraw->theYPos << "), A="
       << lpDraw->theAngle << ", M=" << lpDraw->theMargin << ", T="
       << lpDraw->theTextType << ","
       << (lpDraw->isTextPoly ? "'Poly'" : "'Simple'") << ","
       << (lpDraw->isTextWide ? "'WCHAR'" : "'CHAR'")
       << ", L=" << lpDraw->theTextLength << ", ('" << extText
       << "'), SZ: " << theTextSize << ", VS=" << lpW32->myTextVScale
       << ", HS=" << lpW32->myTextHScale << ", SL=" << lpW32->myTextSlant
       << endl << flush;
*/
}

#endif  // MFT
