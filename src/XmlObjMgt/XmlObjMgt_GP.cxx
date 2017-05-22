// Created on: 2001-08-02
// Created by: Alexander GRIGORIEV
// Copyright (c) 2001-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <gp_Mat.hxx>
#include <gp_Trsf.hxx>
#include <gp_XYZ.hxx>
#include <XmlObjMgt_GP.hxx>

#include <errno.h>
#include <stdio.h>
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
  Sprintf (buf, "%.17g %d %s %s", aTrsf.ScaleFactor(), aTrsf.Form(),
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
  Sprintf (buf, "%s %s %s", S1.GetString(), S2.GetString(), S3.GetString());
  return XmlObjMgt_DOMString (buf);
}

//=======================================================================
//function : Translate
//purpose  : 
//=======================================================================
XmlObjMgt_DOMString XmlObjMgt_GP::Translate (const gp_XYZ& anXYZ)
{
  char buf [75]; // (24 + 1) * 3
  Sprintf (buf, "%.17g %.17g %.17g", anXYZ.X(), anXYZ.Y(), anXYZ.Z());
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
  Standard_Real aScaleFactor = Standard_Real(Strtod (aStr, &ptr));
  if (ptr != aStr && errno != ERANGE && errno != EINVAL)
  {
    T.SetScaleFactor(aScaleFactor);
    aStr = ptr;
    Standard_Integer aForm = Standard_Integer(strtol(aStr, &ptr, 10));
    if (ptr != aStr && errno != ERANGE && errno != EINVAL) {
      T.SetForm((gp_TrsfForm)aForm);
      aStr = ptr;

//  gp_Mat aMatr;
      aStr = ::Translate(aStr, (gp_Mat&)T.HVectorialPart());
      if (aStr) {

//  gp_XYZ aTransl;
        ::Translate(aStr, (gp_XYZ&)T.TranslationPart());
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
    Standard_Real aC = Strtod (theStr, &ptr);
    if (ptr != theStr && errno != ERANGE && errno != EINVAL) {
      P.SetX(aC);
      theStr = ptr;
      aC = Strtod (theStr, &ptr);
      if (ptr != theStr && errno != ERANGE && errno != EINVAL) {
        P.SetY(aC);
        theStr = ptr;
        aC = Strtod (theStr, &ptr);
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
