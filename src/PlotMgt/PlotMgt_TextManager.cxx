// File:      PlotMgt_TextManager.cdl
// Created:   DEC-98
// Author:    DCB
// Copyright: Matra Datavision 1993

#include <PlotMgt_TextManager.ixx>
#include <Aspect_Units.hxx>

#ifdef WNT
#pragma warning (disable : 4244)
#endif

//#define TRACE
#define MAXPATHPOINTS  800
#define MAXCHARPATHS   20
#define MAXCHARPOINTS  MAXPATHPOINTS*MAXCHARPATHS

#define CONVERT(v) (v)*72./(0.0254005 METER)

#define TRANSFORM(X,Y)                 \
  { Standard_Real x = (X), y = (Y);    \
    X = x*theCosAngle - y*theSinAngle; \
    Y = x*theSinAngle + y*theCosAngle; \
  }

#define ADD_POINT(X,Y,aX,aY)                       \
  { theXPath[thePoint] = myDriver->MapX((X)+(aX)); \
    theYPath[thePoint] = myDriver->MapY((Y)+(aY)); \
    thePoint++;                                    \
  }

//static Standard_Integer   thePaintType, theNchar, theNpoly, theNpath;
static Standard_Integer   thePaintType, theNchar;
static Standard_ShortReal theX, theY, thePX, thePY;
static Standard_ShortReal theXmin, theXmax, theSinAngle, theCosAngle;
static Standard_Real      theOrientation, theSlant, theWidth, theHeight;
static Standard_Integer   theTextColor;
static Standard_Integer   theTypeOfText;
static Quantity_Length    theUnderlinePos;
static Standard_ShortReal theXPath     [MAXCHARPOINTS];
static Standard_ShortReal theYPath     [MAXCHARPOINTS];
static Standard_Integer   theCPath     [MAXCHARPATHS ];
static Standard_Integer   thePoint     = 0;
static Standard_Integer   thePathCount = 0;
// Driver's graphic attributes
static Standard_Integer   lci, lti, lwi, pci, pti;
static Standard_Boolean   pef;

//==========================================================
PlotMgt_TextManager::PlotMgt_TextManager (const Handle(PlotMgt_PlotterDriver)& aDriver)
{
  myDriver        = aDriver.operator->();
  theTypeOfText   = Aspect_TOT_OUTLINE;
  theUnderlinePos = 0.;
  theTextColor    = 0;
}

//==========================================================
Standard_Boolean PlotMgt_TextManager::SetCharBoundingBox (const Quantity_Length X1,
                                                          const Quantity_Length Y1,
                                                          const Quantity_Length X2,
                                                          const Quantity_Length Y2,
                                                          const Quantity_Length X3,
                                                          const Quantity_Length Y3,
                                                          const Quantity_Length X4,
                                                          const Quantity_Length Y4)
{
#ifdef TRACE
  cout << " PlotMgt_TextManager::SetCharBoundingBox(" << X1 << "," << Y1
       << "," << X2 << "," << Y2 << "," << X3 << "," << Y3 << "," << X4
       << "," << Y4 << ")" << endl << flush;
#endif
  if (theUnderlinePos > 0.) {
    if (!theNchar)
      theXmin = Standard_ShortReal (X1);
    theXmax = Sqrt(X2*X2 + Y2*Y2);
  }
  return Standard_True;
}

//==========================================================
Standard_Boolean PlotMgt_TextManager::SetCharEncoding (const Standard_CString anEncoding)
{
#ifdef TRACE
  cout << " PlotMgt_TextManager::SetCharEncoding('" << anEncoding << "')" << endl;
#endif
  return Standard_True;
}

//==========================================================
void PlotMgt_TextManager::BeginString (const Quantity_Length X,
                                       const Quantity_Length Y,
                                       const Quantity_PlaneAngle anOrientation,
                                       const Quantity_Length aWidth,
                                       const Quantity_Length aHeight,
                                       const Quantity_PlaneAngle aSlant,
                                       const Standard_Integer aPaintType)
{
#ifdef TRACE
  cout << " PlotMgt_TextManager::BeginString(" << X << "," << Y
       << "," << anOrientation << "," << aWidth << "," << aHeight
       << "," << aSlant << "," << aPaintType << ")" << endl;
#endif
  thePaintType   = aPaintType;
  theOrientation = (float) anOrientation;
  // Force to OUTLINE if required
  if (!thePaintType && (theTypeOfText == Aspect_TOT_OUTLINE))
    thePaintType = 2;
  theX = X; theY = Y;
  theXmin = theXmax = 0.;
  theNchar = 0;
  theSlant = aSlant;
  theWidth = aWidth;
  theHeight = aHeight;
  // Remember the old attributes and set new ones
  myDriver -> LineAttrib (lci, lti, lwi);
  myDriver -> PolyAttrib (pci, pti, pef);
  myDriver -> SetLineAttrib  (theTextColor, 0, 0);
  myDriver -> SetPolyAttrib  (theTextColor,
    (theTypeOfText == Aspect_TOT_OUTLINE ? -1 : 0), Standard_False);
  // Force plotting of graphic attributes at the begin of a string
  myDriver -> PlotLineAttrib (theTextColor, 0, 0);
  myDriver -> PlotPolyAttrib (theTextColor,
    (theTypeOfText == Aspect_TOT_OUTLINE ? -1 : 0), Standard_False);
}

//==========================================================
Standard_Boolean PlotMgt_TextManager::BeginChar (const Standard_Integer aCharCode,
                                                 const Quantity_Length X,
                                                 const Quantity_Length Y)
{
#ifdef TRACE
  cout << " PlotMgt_TextManager::BeginChar(" << aCharCode << ","
       << X << "," << Y << ")" << endl << flush;
#endif
  thePoint     = 0;
  thePathCount = 0;
  theCPath[0]  = 0;
  thePX        = X;
  thePY        = Y;
  return Standard_True;
}

//==========================================================
Standard_Boolean PlotMgt_TextManager::Moveto (const Quantity_Length X,
                                              const Quantity_Length Y)
{
#ifdef TRACE
  cout << " PlotMgt_TextManager::Moveto(" << X << "," << Y << ")" << endl << flush;
#endif
  if (theCPath[thePathCount] > 1)
    thePathCount++;
  else if (theCPath[thePathCount] > 0)
    --thePoint;
  theCPath[thePathCount] = 1;
  thePX = X; thePY = Y;
  ADD_POINT (theX, theY, X, Y);
  return Standard_True;
}

//==========================================================
Standard_Boolean PlotMgt_TextManager::Lineto (const Quantity_Length X,
                                              const Quantity_Length Y)
{
#ifdef TRACE
  cout << " PlotMgt_TextManager::Lineto(" << X << "," << Y << ")" << endl << flush;
#endif
  theCPath[thePathCount]++;
  ADD_POINT (theX, theY, X, Y);
  return Standard_True;
}

//==========================================================
Standard_Boolean PlotMgt_TextManager::Curveto (const Quantity_Length X1,
                                               const Quantity_Length Y1,
                                               const Quantity_Length X2,
                                               const Quantity_Length Y2,
                                               const Quantity_Length X3,
                                               const Quantity_Length Y3,
                                               const Quantity_Length X4,
                                               const Quantity_Length Y4)
{
#ifdef TRACE
  cout << " PlotMgt_TextManager::Curveto(" << X1 << "," << Y1 << ","
       << X2 << "," << Y2 << "," << X3 << "," << Y3 << "," << X4
       << "," << Y4 << ")" << endl << flush;
#endif
  if (myDriver -> DrawCurveCapable()) {
    if (theCPath[thePathCount] > 0)
      thePathCount++;
    theCPath[thePathCount] = -1;
    ADD_POINT (theX, theY, X2, Y2);
    ADD_POINT (theX, theY, X3, Y3);
    ADD_POINT (theX, theY, X4, Y4);
    thePathCount++;
    theCPath[thePathCount] = 0;
    return Standard_True;
  }
  return Standard_False;
}

//==========================================================
void PlotMgt_TextManager::ClosePath ()
{
#ifdef TRACE
  cout << " PlotMgt_TextManager::ClosePath()" << endl << flush;
#endif
  if (thePaintType == 2)
    Lineto (thePX, thePY);
  thePathCount++;
  theCPath[thePathCount] = 0;
}

//==========================================================
Standard_Boolean PlotMgt_TextManager::EndChar (const Quantity_Length X,
                                               const Quantity_Length Y)
{
#ifdef TRACE
  cout << "PlotMgt_TextManager::EndChar(" << X << "," << Y << ")\n"
       << "Paths: " << thePathCount << ", PaintType: " << thePaintType << endl << flush;
  for (int j = 0; j < thePathCount; j++)
    cout << "Path[" << j << "] is: " << theCPath[j] << endl << flush;
#endif
  int N = 0, i;
  theNchar++;
  switch (thePaintType) {
    /////////////////////////////////////////// Euclid3 font
    case 1:
      for (i = 0; i <= thePathCount; i++) {
        myDriver -> PlotPolyline (&theXPath[N], &theYPath[N], &theCPath[i], 1);
        N += theCPath [i];
      }
      break;
    /////////////////////////////////////////// Outlined font
    case 2:
      myDriver -> PlotPolyline (theXPath, theYPath, theCPath, thePathCount);
      break;
    /////////////////////////////////////////// Solid font
    default:
      myDriver -> PlotPolygon (theXPath, theYPath, theCPath, thePathCount);
      break;
  }
  thePathCount = 0;
  memset (theCPath, 0, sizeof(theCPath));
  return Standard_True;
}

//==========================================================
void PlotMgt_TextManager::EndString ()
{
#ifdef TRACE
  cout << " PlotMgt_TextManager::EndString()" << endl << flush;
#endif
  if (theUnderlinePos > 0.) {
    Standard_ShortReal theX1 = theXmin;
    Standard_ShortReal theY1 = (float)-theUnderlinePos;
    Standard_ShortReal theX2 = theXmax;
    Standard_ShortReal theY2 = theY1;
    theSinAngle = (float)Sin(theOrientation);
    theCosAngle = (float)Cos(theOrientation);
    TRANSFORM(theX1, theY1);
    TRANSFORM(theX2, theY2);
    myDriver -> PlotSegment (
      myDriver->MapX(theX+theX1), myDriver->MapY(theY+theY1),
      myDriver->MapX(theX+theX2), myDriver->MapY(theY+theY2)
    );
  }
  // Restore old attributes
  myDriver -> SetLineAttrib (lci, lti, lwi);
  myDriver -> SetPolyAttrib (pci, pti, pef);
}

//==========================================================
void PlotMgt_TextManager::SetTextAttribs (const Standard_Integer aTextColor,
                                          const Aspect_TypeOfText aTypeOfText,
                                          const Quantity_Length anUnderlinePosition)
{
  theTextColor    = aTextColor;
  theTypeOfText   = aTypeOfText;
  theUnderlinePos = anUnderlinePosition;
}
