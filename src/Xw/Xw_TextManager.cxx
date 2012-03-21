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

#define S3593	//GG_130398
//		OPTIMISATION MFT
#define PRO14351//GG_180698
//		OPTIMISATION MFT

#include <Xw_TextManager.ixx>
#include <Aspect_Units.hxx>
#define TRACE 0

#define MAXCHARPOINTS 512
#define MAXCHARPATHS 8
#define TRANSFORM(X,Y) \
{ Standard_Real x = X,y = Y; \
          X = x*theCosAngle - y*theSinAngle; \
          Y = x*theSinAngle + y*theCosAngle; \
        }

// Routines C a declarer en extern
//extern "C" {
#include <Xw_Cextern.hxx>
//}

static XW_ATTRIB thePolyCode,theLineCode;
static Standard_Integer thePaintType,theNchar,theNpoly,theNpath;
static Standard_Integer theTextColor,theLineColor,theLineType,theLineWidth;
static Standard_Integer thePolyColor,thePolyType,thePolyTile;
static XW_DRAWMODE theLineMode,thePolyMode;
static Aspect_TypeOfText theTypeOfText;
static Standard_ShortReal theUnderlinePosition;
static Standard_ShortReal theX,theY,thePX,thePY;
static Standard_ShortReal theXmin,theXmax,theSinAngle,theCosAngle;
static Standard_ShortReal theSlant,theWidth,theHeight;
static Standard_Real theOrientation;
static Standard_Boolean theClippingFlag;

Xw_TextManager::Xw_TextManager(const Standard_Address aDrawable, const Standard_Address aWidthMap) : myDrawable(aDrawable),myWidthMap(aWidthMap) {
}

void Xw_TextManager::BeginString( const Quantity_Length X, const Quantity_Length Y, const Quantity_PlaneAngle anOrientation, const Quantity_Length aWidth, const Quantity_Length aHeight, const Quantity_PlaneAngle aSlant, const Standard_Integer aPaintType) {
#if TRACE > 0
  cout << " Xw_TextManager::BeginString(" <<
                        X << "," << Y << "," << anOrientation << ","
                        << aWidth << "," << aHeight << "," aSlant << ","
                        << aPaintType << ")" << endl;
#endif

  thePaintType = aPaintType;
  theOrientation = anOrientation;
  theX = X; theY = Y;
  theXmin = theXmax = 0.;
  theNchar = 0;
  theSlant = aSlant;
  theWidth = aWidth;
  theHeight = aHeight;
  theNpoly = theNpath = 0;

#ifdef S3593
					// Set clipping off
  theClippingFlag = Xw_get_clipping(myDrawable);
  Xw_set_clipping(myDrawable,Standard_False);
#endif
                                        // Save line attribs
  thePolyCode = Xw_get_poly_attrib(myDrawable,
              	&thePolyColor,&thePolyType,&thePolyTile,&thePolyMode);
  theLineCode = Xw_get_line_attrib(myDrawable,
              	&theLineColor,&theLineType,&theLineWidth,&theLineMode);

  Standard_ShortReal thickness = theHeight/32.;
  Standard_Integer windex;
  Xw_get_width_index(myWidthMap,thickness,&windex) ;
					// Force to OUTLINE if required
  if( !thePaintType ) {
    if( (theHeight < (4. MILLIMETER)) || 
		(theTypeOfText == Aspect_TOT_OUTLINE) ) thePaintType = 2;
  }
					// Sets the MFT text attribs
  switch (thePaintType) {
    case 0:
      Xw_set_poly_attrib(myDrawable,
                        theTextColor,Standard_False,0,XW_REPLACE);
      Xw_set_line_attrib(myDrawable,
                        theTextColor,0,windex,XW_REPLACE);
      break;
    case 2:
      Xw_set_poly_attrib(myDrawable,
                        theTextColor,Standard_True,-1,XW_REPLACE);
      Xw_set_line_attrib(myDrawable,
                        theTextColor,0,windex,XW_REPLACE);
      break;
    case 1:
      Xw_set_line_attrib(myDrawable,
                        theTextColor,0,windex,XW_REPLACE);
#ifndef PRO14351
      Xw_begin_segments(myDrawable, 0);
#endif
      break;
  }

}

Standard_Boolean Xw_TextManager::BeginChar(const Standard_Integer aCharCode, const Standard_Real X, const Standard_Real Y) {
#if TRACE > 0
  cout << " Xw_TextManager::BeginChar(" << aCharCode << "," <<
                                        X << "," << Y << ")" << endl;
#endif

  thePX = X; thePY = Y;
#ifndef PRO14351
  if( thePaintType != 1 ) 
#endif
  theNpoly = theNpath = 0;
  return Standard_True;
}

Standard_Boolean Xw_TextManager::SetCharBoundingBox(const Quantity_Length X1, const Quantity_Length Y1, const Quantity_Length X2, const Quantity_Length Y2, const Quantity_Length X3, const Quantity_Length Y3, const Quantity_Length X4, const Quantity_Length Y4) {

#if TRACE > 0
  cout << " Xw_TextManager::SetCharBoundingBox(" << X1 << "," << Y1 << "," << 
	X2 << "," << Y2 << "," << X3 << "," << Y3 << "," << 
	X4 << "," << Y4 << ")" << endl;
#endif

  if( theUnderlinePosition > 0. ) {
    if( !theNchar ) {
      theXmin = Standard_ShortReal (X1);
    }
    theXmax = Sqrt(X2*X2 + Y2*Y2);
  }
  return Standard_True;
}

Standard_Boolean Xw_TextManager::SetCharEncoding(const Standard_CString anEncoding) {

#if TRACE > 0
  cout << " Xw_TextManager::SetCharEncoding('" << anEncoding << "')" << endl;
#endif

   return Standard_True;
}

Standard_Boolean Xw_TextManager::Moveto(const Standard_Real X, const Standard_Real Y) {
#if TRACE > 1
  cout << " Xw_TextManager::Moveto(" << X << "," << Y << ")" << endl;
#endif

#ifndef PRO14351
  if( thePaintType != 1 ) 
#endif
  this->ClosePath();
  thePX = X; thePY = Y;

  return Standard_True;
}

Standard_Boolean Xw_TextManager::Lineto(const Standard_Real X, const Standard_Real Y) {
#if TRACE > 1
  cout << " Xw_TextManager::Lineto(" << X << "," << Y << ")" << endl;
#endif
  switch (thePaintType) {
    case 0:
    case 2:
      if( theNpoly == 0 ) {
        Xw_begin_poly(myDrawable,MAXCHARPOINTS,MAXCHARPATHS);
      }
      if( theNpath == 0 ) {
        Xw_poly_point(myDrawable, theX+thePX, theY+thePY);
      }
      Xw_poly_point(myDrawable, theX+Standard_ShortReal(X), 
				theY+Standard_ShortReal(Y));
      theNpath++; theNpoly++;
      break;
    case 1:
#ifndef PRO14351
      Xw_draw_segment(myDrawable, theX+thePX, theY+thePY,
				  theX+Standard_ShortReal(X),
				  theY+Standard_ShortReal(Y));
#else
      if( theNpoly == 0 ) {
        Xw_begin_line(myDrawable,MAXCHARPOINTS);
        Xw_line_point(myDrawable, theX+thePX, theY+thePY);
      }
      Xw_line_point(myDrawable, theX+Standard_ShortReal(X),
                                theY+Standard_ShortReal(Y));
#endif
      thePX = X; thePY = Y;
      theNpoly++;
  }

  return Standard_True;
}

Standard_Boolean Xw_TextManager::Curveto(const Quantity_Length X1, const Quantity_Length Y1, const Quantity_Length X2, const Quantity_Length Y2, const Quantity_Length X3, const Quantity_Length Y3, const Quantity_Length X4, const Quantity_Length Y4) {

#if TRACE > 0
  cout << " Xw_TextManager::Curveto(" << X1 << "," << Y1 << "," << 
	X2 << "," << Y2 << "," << X3 << "," << Y3 << "," << 
	X4 << "," << Y4 << ")" << endl;
#endif

  return Standard_False;
}

void Xw_TextManager::ClosePath() {
#if TRACE > 0
  cout << " Xw_TextManager::ClosePath()" << endl;
#endif
  switch (thePaintType) {
    case 0:
    case 2:
      if( theNpath > 0 ) Xw_close_path(myDrawable);
      theNpath = 0;
      break;
    case 1:
#ifdef PRO14351
     if( theNpoly > 0 ) Xw_close_line(myDrawable);
     theNpoly = 0;
#endif
     break;
  }
}

Standard_Boolean Xw_TextManager::EndChar(const Standard_Real X, const Standard_Real Y) {
#if TRACE > 0
  cout << " Xw_TextManager::EndChar(" << X << "," << Y << ")" << endl;
#endif
  switch (thePaintType) {
    case 0:
    case 2:
      Xw_close_poly(myDrawable);
      break;
    case 1:
#ifdef PRO14351
     Xw_close_line(myDrawable);
     theNpoly = theNpath = 0;
#endif
     break;
  }
  theNchar++;

  return Standard_True;
}

void Xw_TextManager::EndString() {
#if TRACE > 0
  cout << " Xw_TextManager::EndString()" << endl;
#endif

  if( theUnderlinePosition > 0. ) {
    Standard_ShortReal theX1 = theXmin;
    Standard_ShortReal theY1 = -theUnderlinePosition;
    Standard_ShortReal theX2 = theXmax;
    Standard_ShortReal theY2 = theY1;
    theSinAngle = Sin(theOrientation);
    theCosAngle = Cos(theOrientation);
    TRANSFORM(theX1,theY1);
    TRANSFORM(theX2,theY2);
#ifndef PRO14351
    Xw_draw_segment(myDrawable, theX+theX1, theY+theY1,
                                theX+theX2, theY+theY2);
#else
    Xw_begin_line(myDrawable,2);
    Xw_line_point(myDrawable, theX+theX1, theY+theY1);
    Xw_line_point(myDrawable, theX+theX2, theY+theY2);
    Xw_close_line(myDrawable);
#endif
  }
                                        // Restore line attribs
  switch (thePaintType) {
    case 0:
    case 2:
      if( thePolyCode != ~0 ) Xw_set_poly_attrib(myDrawable,
              	thePolyColor,thePolyType,thePolyTile,thePolyMode);
    case 1:
#ifndef PRO14351
      Xw_close_segments(myDrawable);
      theNpoly = 0;
#endif
      if( theLineCode != ~0 ) Xw_set_line_attrib(myDrawable,
              	theLineColor,theLineType,theLineWidth,theLineMode);
      break;
  }
#ifdef S3593
					// Restore clipping
  Xw_set_clipping(myDrawable,theClippingFlag);
#endif
}

void Xw_TextManager::SetTextAttribs( const Standard_Integer aTextColor, const Aspect_TypeOfText aTypeOfText, const Quantity_Length anUnderlinePosition) {

  theTextColor = aTextColor;
  theTypeOfText = aTypeOfText;
  theUnderlinePosition = anUnderlinePosition;
}
