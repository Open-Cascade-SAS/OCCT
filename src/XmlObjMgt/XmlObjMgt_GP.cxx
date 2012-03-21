// Created on: 2001-08-02
// Created by: Alexander GRIGORIEV
// Copyright (c) 2001-2012 OPEN CASCADE SAS
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


#include <XmlObjMgt_GP.ixx>
#include <stdio.h>
#include <errno.h>

static const char * Translate (const char * theStr, gp_Mat& M);
static const char * Translate (const char * theStr, gp_XYZ& P);

// STORE

//=======================================================================
//function : Translate
//purpose  : 
//=======================================================================
XmlObjMgt_DOMString XmlObjMgt_GP::Translate (const gp_Trsf& aTrsf)
{
  char buf [256];
  XmlObjMgt_DOMString S1 (Translate(aTrsf.HVectorialPart())),
                      S2 (Translate(aTrsf.TranslationPart()));
  sprintf (buf, "%.17g %d %s %s", aTrsf.ScaleFactor(), aTrsf.Form(),
           S1.GetString(), S2.GetString());

  return XmlObjMgt_DOMString (buf);
}

//=======================================================================
//function : Translate
//purpose  : 
//=======================================================================
XmlObjMgt_DOMString XmlObjMgt_GP::Translate (const gp_Mat& aMat)
{
  char buf[128];
  XmlObjMgt_DOMString S1 (Translate(aMat.Row(1))),
                      S2 (Translate(aMat.Row(2))),
                      S3 (Translate(aMat.Row(3)));
  sprintf (buf, "%s %s %s", S1.GetString(), S2.GetString(), S3.GetString());
  return XmlObjMgt_DOMString (buf);
}

//=======================================================================
//function : Translate
//purpose  : 
//=======================================================================
XmlObjMgt_DOMString XmlObjMgt_GP::Translate (const gp_XYZ& anXYZ)
{
  char buf [64];
  sprintf (buf, "%.17g %.17g %.17g", anXYZ.X(), anXYZ.Y(), anXYZ.Z());
  return XmlObjMgt_DOMString (buf);
}

// RETRIEVE

//=======================================================================
//function : Translate
//purpose  : 
//=======================================================================
Standard_Boolean XmlObjMgt_GP::Translate
                        (const XmlObjMgt_DOMString& theStr, gp_Trsf& T)
{
  Standard_Boolean aResult = Standard_False;
  const char * aStr = theStr.GetString();
  char * ptr;
  errno = 0;
  Standard_Real aScaleFactor = Standard_Real(strtod (aStr, &ptr));
  if (ptr != aStr && errno != ERANGE && errno != EINVAL)
  {
    T._CSFDB_Setgp_Trsfscale(aScaleFactor);
    aStr = ptr;
    Standard_Integer aForm = Standard_Integer(strtol(aStr, &ptr, 10));
    if (ptr != aStr && errno != ERANGE && errno != EINVAL) {
      T._CSFDB_Setgp_Trsfshape((gp_TrsfForm)aForm);
      aStr = ptr;

//  gp_Mat aMatr;
      aStr = ::Translate(aStr, (gp_Mat&)T._CSFDB_Getgp_Trsfmatrix());
      if (aStr) {

//  gp_XYZ aTransl;
        ::Translate(aStr, (gp_XYZ&)T._CSFDB_Getgp_Trsfloc());
        aResult = Standard_True;
      }
    }
  }
  return aResult;
}

//=======================================================================
//function : Translate
//purpose  : 
//=======================================================================
Standard_Boolean XmlObjMgt_GP::Translate
                        (const XmlObjMgt_DOMString& theStr, gp_Mat& M)
{
  return (::Translate (theStr.GetString(), M) != 0);
}

//=======================================================================
//function : Translate
//purpose  : 
//=======================================================================
Standard_Boolean XmlObjMgt_GP::Translate
                        (const XmlObjMgt_DOMString& theStr, gp_XYZ& P)
{
  return (::Translate (theStr.GetString(), P) != 0);
}

//=======================================================================
//function : Translate
//purpose  : 
//=======================================================================
static const char * Translate (const char * theStr, gp_Mat& M)
{
  gp_XYZ aC;

  theStr = Translate(theStr, aC);
  if (theStr) {
    M.SetRow(1, aC);
    theStr = Translate(theStr, aC);
    if (theStr) {
      M.SetRow(2, aC);
      theStr = Translate(theStr, aC);
      if (theStr)
        M.SetRow(3, aC);
    }
  }
  return theStr;
}

//=======================================================================
//function : Translate
//purpose  : 
//=======================================================================
static const char * Translate (const char * theStr, gp_XYZ& P)
{
  char * ptr;
  if (theStr) {
    errno = 0;
    Standard_Real aC = strtod (theStr, &ptr);
    if (ptr != theStr && errno != ERANGE && errno != EINVAL) {
      P.SetX(aC);
      theStr = ptr;
      aC = strtod (theStr, &ptr);
      if (ptr != theStr && errno != ERANGE && errno != EINVAL) {
        P.SetY(aC);
        theStr = ptr;
        aC = strtod (theStr, &ptr);
        if (ptr != theStr && errno != ERANGE && errno != EINVAL) {
          P.SetZ(aC);
          theStr = ptr;
        } else
	  theStr = 0;
      } else
	theStr = 0;
    } else
      theStr = 0;
  }
  return theStr;
}
