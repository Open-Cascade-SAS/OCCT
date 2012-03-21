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

#define PRO15231	//GG 080998
//			De maniere indirect on corrige le retour
//			de SetCharBoundingBox() de maniere a activer 
//			l'optimisation importante pour myFontManager::TextSize()

#include <MFT_TextManager.ixx>
#include <TCollection_AsciiString.hxx>
#define TRACE 0

static Quantity_Length theXmin,theYmin,theXmax,theYmax;
static Quantity_Length theWidth,theHeight;
static Quantity_PlaneAngle theOrientation,theSlant;
static TCollection_AsciiString theEncoding;
static Standard_Boolean theSkipFlag = Standard_False;

MFT_TextManager::MFT_TextManager() {
}

void MFT_TextManager::BeginString( const Quantity_Length X, const Quantity_Length Y, const Quantity_PlaneAngle anOrientation, const Quantity_Length aWidth, const Quantity_Length aHeight, const Quantity_PlaneAngle aSlant, const Standard_Integer aPaintType) {
#if TRACE > 0
  cout << " MFT_TextManager::BeginString(" << 
			X << "," << Y << "," << anOrientation << ","
			<< aWidth << "," << aHeight << "," aSlant << "," 
			<< aPaintType << ")" << endl;
#endif
  theOrientation = anOrientation;
  theSlant = aSlant;
  theWidth = aWidth;
  theHeight = aHeight;
  theXmin = theYmin = theXmax = theYmax = 0.;
}

Standard_Boolean MFT_TextManager::BeginChar(const Standard_Integer aCharCode, const Standard_Real X, const Standard_Real Y) {
#if TRACE > 0
  cout << " MFT_TextManager::BeginChar(" << aCharCode << "," << 
					X << "," << Y << ")" << endl;
#endif
  theXmin = Min(theXmin,X); theXmax = Max(theXmax,X);
  theYmin = Min(theYmin,Y); theYmax = Max(theYmax,Y);
  theSkipFlag = Standard_False;
  theEncoding.Clear();
  return Standard_True;
}

Standard_Boolean MFT_TextManager::SetCharBoundingBox(const Quantity_Length X1, const Quantity_Length Y1, const Quantity_Length X2, const Quantity_Length Y2, const Quantity_Length X3, const Quantity_Length Y3, const Quantity_Length X4, const Quantity_Length Y4) {

#if TRACE > 0
  cout << " MFT_TextManager::SetCharBoundingBox(" << X1 << "," << Y1 << "," << 
	X2 << "," << Y2 << "," << X3 << "," << Y3 << "," << 
	X4 << "," << Y4 << ")" << endl;
#endif

  if( theOrientation != 0. ) {	//NOT IMPLEMENTED,because not used
#ifdef PRO15231
    theSkipFlag = Standard_False;
#endif
  }
  if( X2 > X1 ) {
    theXmin = Min(theXmin,X1); theXmin = Min(theXmin,X4); 
    theYmin = Min(theYmin,Y1); theYmax = Max(theYmax,Y3);
    theXmax = Max(theXmax,X2); theXmax = Max(theXmax,X3);
    theSkipFlag = Standard_True;
  } else {
    theSkipFlag = Standard_False;
  }
#ifdef PRO15231
  return !theSkipFlag;	
#else
  return Standard_True;
#endif
}

Standard_Boolean MFT_TextManager::SetCharEncoding(const Standard_CString anEncoding) {

#if TRACE > 0
  cout << " MFT_TextManager::SetCharEncoding('" << anEncoding << "')" << endl;
#endif

   theEncoding = anEncoding;

   return !theSkipFlag;
}

Standard_Boolean MFT_TextManager::Moveto(const Quantity_Length X, const Quantity_Length Y) {
#if TRACE > 1
  cout << " MFT_TextManager::Moveto(" << X << "," << Y << ")" << endl;
#endif
  theXmin = Min(theXmin,X); theXmax = Max(theXmax,X);
  theYmin = Min(theYmin,Y); theYmax = Max(theYmax,Y);
  return Standard_True;
}

Standard_Boolean MFT_TextManager::Lineto(const Quantity_Length X, const Quantity_Length Y) {
#if TRACE > 1
  cout << " MFT_TextManager::Lineto(" << X << "," << Y << ")" << endl;
#endif
  theXmin = Min(theXmin,X); theXmax = Max(theXmax,X);
  theYmin = Min(theYmin,Y); theYmax = Max(theYmax,Y);
  return Standard_True;
}

Standard_Boolean MFT_TextManager::Curveto(const Quantity_Length X1, const Quantity_Length Y1, const Quantity_Length X2, const Quantity_Length Y2, const Quantity_Length X3, const Quantity_Length Y3, const Quantity_Length X4, const Quantity_Length Y4) {

#if TRACE > 0
  cout << " MFT_TextManager::Curveto(" << X1 << "," << Y1 << "," << 
	X2 << "," << Y2 << "," << X3 << "," << Y3 << "," << 
	X4 << "," << Y4 << ")" << endl;
#endif

  return Standard_False;	
}

void MFT_TextManager::ClosePath() {
#if TRACE > 0
  cout << " MFT_TextManager::ClosePath()" << endl;
#endif
}

Standard_Boolean MFT_TextManager::EndChar(const Standard_Real X, const Standard_Real Y) {
#if TRACE > 0
  cout << " MFT_TextManager::EndChar(" << X << "," << Y << ")" << endl;
#endif
  theXmin = Min(theXmin,X); theXmax = Max(theXmax,X);
  theYmin = Min(theYmin,Y); theYmax = Max(theYmax,Y);
  return Standard_True;
}

void MFT_TextManager::EndString() {
#if TRACE > 0
  cout << " MFT_TextManager::EndString()" << endl;
#endif
}

void MFT_TextManager::MinMax(Quantity_Length& Xmin, Quantity_Length& Ymin, Quantity_Length& Xmax, Quantity_Length& Ymax) {

  Xmin = theXmin;
  Ymin = theYmin;
  Xmax = theXmax;
  Ymax = theYmax;
#if TRACE > 0
  cout << " MFT_TextManager::MinMax(" << theXmin << "," << theYmin << 
			"," << theXmax << "," << theYmax << ")" << endl;
#endif
 theXmin = theYmin = theXmax = theYmax = 0.;
}

Standard_CString MFT_TextManager::Encoding() {

  return theEncoding.ToCString();
}
