// Created by: CHABROVSKY Dmitry
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


// include windows.h first to have all definitions available
#include <windows.h>

#define  MFT

#include <WNT_TextManager.ixx>
#include <WNT_MFTDraw.hxx>
#include <Aspect_Units.hxx>

#define CHAR_DESC_LEN 30000
#define MY_HDC (HDC)myDevContext
#define U2P(v) LONG( (v)/myPixelToUnit + 0.5 )
#define TRANSFORM(X,Y)                                           \
{ Standard_Real x = X,y = Y;                             \
          X = Standard_ShortReal(x*theCosAngle - y*theSinAngle); \
          Y = Standard_ShortReal(x*theSinAngle + y*theCosAngle); \
        }

enum TypeOfPoint {
  TOP_MOVETO, TOP_LINETO, TOP_CURVETO
};

static Standard_Integer   thePaintType;
static Standard_Integer   theTextColor;
static Aspect_TypeOfText  theTypeOfText;
static Standard_ShortReal theUnderlinePosition;
static Standard_ShortReal theXmin,theXmax;
static Standard_Integer   theNchar;
static Standard_Real      theSinAngle,theCosAngle;
static Standard_Real      theOrientation;
//////////////// Character description ///////////////
static int                myStrLen  = 0;
static POINT              myStrDesc [CHAR_DESC_LEN];
static TypeOfPoint        myStrInfo [CHAR_DESC_LEN];
static double             myBX, myBY;
static double             myCX, myCY;
static int                myCharWidth;
static int                myCharHeight;
static BOOL               myCharInside;

#define ADD_POINT(X,Y,aType)                                                         \
  myStrDesc[myStrLen].x = U2P( (X)+myBX );                                           \
  myStrDesc[myStrLen].y = ( myWin95 ? myDevHeight - U2P((Y)+myBY) : U2P((Y)+myBY) ); \
  myStrInfo[myStrLen]   = aType; myStrLen++;

#define CHECK_INSIDE(x,y)                                                 \
  if (!myCharInside)                                                      \
    if (myWin95) {                                                        \
      if ((x > -myCharWidth  && x < (myDevWidth  /*+ myCharWidth*/ )) &&  \
          (y > -myCharHeight && y < (myDevHeight + myCharHeight)))        \
        myCharInside = TRUE;                                              \
    } else {                                                              \
      if ((x > -myCharWidth  && x < (myDevWidth  /*+ myCharWidth */)) &&  \
          (y > -myCharHeight && y < (myDevHeight /*+ myCharHeight*/)))    \
        myCharInside = TRUE;                                              \
    }

#define CUR_X() myStrDesc[myStrLen-1].x
#define CUR_Y() myStrDesc[myStrLen-1].y

/*==================================================================*/
WNT_TextManager::WNT_TextManager(const Standard_Real aPixelToUnit)
{
  myPixelToUnit = aPixelToUnit;
  myDevContext  = 0;
  myDevHeight   = 0;
  // Clear the memory for STRING description
  ZeroMemory (myStrDesc, sizeof(myStrDesc));
  ZeroMemory (myStrInfo, sizeof(myStrInfo));
  myStrLen = 0;
}

/*==================================================================*/
void WNT_TextManager::BeginString(const Quantity_Length X,
                                  const Quantity_Length Y,
                                  const Quantity_PlaneAngle anOrientation,
                                  const Quantity_Length aWidth,
                                  const Quantity_Length aHeight,
                                  const Quantity_PlaneAngle aSlant,
                                  const Standard_Integer aPaintType)
{
  myStrLen       = 0;
  myCharWidth    = U2P(aWidth );
  myCharHeight   = U2P(aHeight);

  thePaintType   = aPaintType;
  theOrientation = anOrientation;
  myBX           = X;
  myBY           = Y;
  theNchar       = 0;
  theXmin        = theXmax = 0.F;
  if (!thePaintType) {
    if ((aHeight < (6. MILLIMETER)) || 
		    (theTypeOfText == Aspect_TOT_OUTLINE))
      thePaintType = 2;
  }
}

/*==================================================================*/
Standard_Boolean WNT_TextManager::BeginChar(const Standard_Integer aCharCode,
                                            const Standard_Real X,
                                            const Standard_Real Y)
{
  myCharInside = FALSE;
  return Standard_True;
}

/*==================================================================*/
Standard_Boolean WNT_TextManager::SetCharBoundingBox(const Quantity_Length X1,
                                                     const Quantity_Length Y1,
                                                     const Quantity_Length X2,
                                                     const Quantity_Length Y2,
                                                     const Quantity_Length X3,
                                                     const Quantity_Length Y3,
                                                     const Quantity_Length X4,
                                                     const Quantity_Length Y4)
{
  if (theUnderlinePosition > 0.) {
    if (!theNchar)
      theXmin = Standard_ShortReal (X1);
    theXmax   = Standard_ShortReal (Sqrt(X2*X2 + Y2*Y2));
  }
  return Standard_True;
}

/*==================================================================*/
Standard_Boolean WNT_TextManager::SetCharEncoding(const Standard_CString anEncoding)
{
  return Standard_True;
}

/*==================================================================*/
Standard_Boolean WNT_TextManager::Moveto(const Standard_Real X,
                                         const Standard_Real Y)
{
  ADD_POINT (X,Y,TOP_MOVETO);
  myCX = X; myCY = Y;
  CHECK_INSIDE (CUR_X(),CUR_Y());
  if (!myCharInside)
    myStrLen--;
  return myCharInside;
}

/*==================================================================*/
Standard_Boolean WNT_TextManager::Lineto(const Standard_Real X,
                                         const Standard_Real Y)
{
  ADD_POINT (X,Y,TOP_LINETO);
  CHECK_INSIDE (CUR_X(),CUR_Y());
  if (!myCharInside)
    myStrLen--;
  return myCharInside;
}

/*==================================================================*/
Standard_Boolean WNT_TextManager::Curveto(const Quantity_Length X1,
                                          const Quantity_Length Y1,
                                          const Quantity_Length X2,
                                          const Quantity_Length Y2,
                                          const Quantity_Length X3,
                                          const Quantity_Length Y3,
                                          const Quantity_Length X4,
                                          const Quantity_Length Y4)
{
  ADD_POINT (X2,Y2,TOP_CURVETO);
  ADD_POINT (X3,Y3,TOP_CURVETO);
  ADD_POINT (X4,Y4,TOP_CURVETO);
  return Standard_True;
}

/*==================================================================*/
void WNT_TextManager::ClosePath()
{
  ADD_POINT (myCX,myCY,TOP_LINETO);
  CHECK_INSIDE (CUR_X(),CUR_Y());
  if (!myCharInside)
    myStrLen--;
}

/*==================================================================*/
Standard_Boolean WNT_TextManager::EndChar(const Standard_Real X,
                                          const Standard_Real Y)
{
  theNchar++;
  return Standard_True;
}

/*==================================================================*/
void WNT_TextManager::EndString()
{
  static int i, j;

  // Draw text string
  i = 0;
  BeginPath (MY_HDC);
  do {
    switch (myStrInfo[i]) {
      case TOP_MOVETO:
        MoveToEx     (MY_HDC, myStrDesc[i].x, myStrDesc[i].y, NULL);
        break;
      case TOP_LINETO:
        j = i+1;
        while ((myStrInfo[j] == TOP_LINETO) && (j < myStrLen))
          j++;
        PolylineTo   (MY_HDC, &myStrDesc[i], j-i);
        i = j-1;
        break;
      case TOP_CURVETO:
        PolyBezierTo (MY_HDC, &myStrDesc[i], 3);
        i += 2;
        break;
      default:
        break;
    }
  } while (++i < myStrLen);
  EndPath (MY_HDC);
  if (thePaintType == 0 && !myMonoBuf) FillPath   (MY_HDC);
  else                                 StrokePath (MY_HDC);

  // Draw underline if necessary
  if (theUnderlinePosition > 0.) {
    Standard_ShortReal theX1 = theXmin;
    Standard_ShortReal theY1 = -theUnderlinePosition;
    Standard_ShortReal theX2 = theXmax;
    Standard_ShortReal theY2 = theY1;
    theSinAngle = Sin (theOrientation);
    theCosAngle = Cos (theOrientation);
    TRANSFORM (theX1, theY1);
    TRANSFORM (theX2, theY2);
    // Draw UNDERLINE
    if (myWin95) {
      MoveToEx     (MY_HDC, U2P(theX1 + myBX), myDevHeight - U2P(theY1 + myBY), NULL);
      LineTo       (MY_HDC, U2P(theX2 + myBX), myDevHeight - U2P(theY2 + myBY)      );
    } else {
      MoveToEx     (MY_HDC, U2P(theX1 + myBX), U2P(theY1 + myBY), NULL);
      LineTo       (MY_HDC, U2P(theX2 + myBX), U2P(theY2 + myBY)      );
    }
  }
}

/*==================================================================*/
void WNT_TextManager::SetTextAttribs(const Standard_Integer aTextColor,
                                     const Aspect_TypeOfText aTypeOfText,
                                     const Quantity_Length anUnderlinePosition)
{
  theTextColor         = aTextColor;
  theTypeOfText        = aTypeOfText;
  theUnderlinePosition = (Standard_ShortReal)anUnderlinePosition;
}

/*==================================================================*/
void WNT_TextManager::SetDrawAttribs(const Standard_Address aDrawData)
{
  PMFT_TEXTMAN_DATA aData = (PMFT_TEXTMAN_DATA)aDrawData;
  myDevContext = (int)aData->theHDC;
  myWin95      =      aData->theWin95;
  myDevWidth   =      aData->theDevWidth;
  myDevHeight  =      aData->theDevHeight;
  myUWidth     =      aData->theUWidth;
  myMonoBuf    =      aData->theMonoBuffer;
}
