// File:      XmlObjMgt_GP.cxx
// Created:   02.08.01 21:10:37
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2001
// History:

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
