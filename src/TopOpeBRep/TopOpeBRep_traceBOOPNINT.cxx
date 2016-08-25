// Created on: 1997-10-22
// Created by: Jean Yves LEBEY
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifdef OCCT_DEBUG

#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TopOpeBRepTool_define.hxx>

#define AS(x) (Standard_PCharacter)TCollection_AsciiString((x)).ToCString();
#define I 10
#define J 10
#define OK(x,y) (x)<I&&(y)<J
#define T(x,y) myT[(x)][(y)]
#define L0(x,y) for((x)=0;(x)<(y);(x)++)
#define A(v) (atoi(a[(v)]))

class BOOPNINTL{
public:BOOPNINTL();
Standard_Boolean Get(Standard_Integer n,char**a);
Standard_Integer Set(const Standard_Boolean b,Standard_Integer n,char**a);
private: 
  TColStd_ListOfInteger myTL[I];
  Standard_Boolean myTB[I];
  Standard_Integer mynl;
};

BOOPNINTL::BOOPNINTL(){Set(Standard_False,0,NULL);}

Standard_Integer BOOPNINTL::Set(const Standard_Boolean b,Standard_Integer n,char**a)
{
  if(n == 0) { 
    Standard_Integer i; L0(i,I) { myTB[i] = Standard_False; myTL[i].Clear(); }
    mynl = 0;
    return 0;
  }
  else if (mynl + 1 < I) {
    myTB[mynl] = b; Standard_Integer i; L0(i,n) myTL[mynl].Append(atoi(a[i]));
    mynl++;
    return 0;
  }
  else {
    return 1;
  }
}

Standard_Boolean BOOPNINTL::Get(Standard_Integer n,char**a){
  if(!n)return Standard_False;
  Standard_Integer il;
  L0(il,mynl) {
    const TColStd_ListOfInteger& L = myTL[il];
    if (L.IsEmpty()) continue;
    TColStd_ListIteratorOfListOfInteger itL(L);
    Standard_Integer ia = 0;
    Standard_Boolean found = Standard_True;
    for (;itL.More() && (ia < n); itL.Next(),ia++) {
      Standard_Integer Lval = itL.Value();
      Standard_Integer aval = atoi(a[ia]);
      if (Lval != aval) {
	found = Standard_False;
	break;
      }
    }
    if (found) {
      return myTB[il];
    }
  }
  return Standard_False;
}

// ===========
static Standard_Boolean TopOpeBRep_traceEEFF = Standard_False;
Standard_EXPORT void TopOpeBRep_SettraceEEFF(const Standard_Boolean b) { TopOpeBRep_traceEEFF = b; }
Standard_EXPORT Standard_Boolean TopOpeBRep_GettraceEEFF() { return TopOpeBRep_traceEEFF; }

BOOPNINTL BOOPEEFF;
Standard_EXPORT Standard_Integer TopOpeBRep_SettraceEEFF(const Standard_Boolean b,Standard_Integer n,char**a)
{
  Standard_Integer err = BOOPEEFF.Set(b,n,a); if (n==0) TopOpeBRep_SettraceEEFF(b); return err;
}

Standard_EXPORT Standard_Boolean TopOpeBRep_GettraceEEFF(Standard_Integer n,char**a)
{ Standard_Boolean b = BOOPEEFF.Get(n,a); return b; }

Standard_EXPORT Standard_Boolean TopOpeBRep_GettraceEEFF(const Standard_Integer i1,const Standard_Integer i2,const Standard_Integer i3,const Standard_Integer i4)
{
  char* t[4]; t[0]=AS(i1);t[1]=AS(i2);t[2]=AS(i3);t[3]=AS(i4);
  Standard_Boolean b = BOOPEEFF.Get(4,t); return b;
}

// ===========
static Standard_Boolean TopOpeBRep_traceNVP = Standard_False;
Standard_EXPORT void TopOpeBRep_SettraceNVP(const Standard_Boolean b) { TopOpeBRep_traceNVP = b; }
Standard_EXPORT Standard_Boolean TopOpeBRep_GettraceNVP() { return TopOpeBRep_traceNVP; }

BOOPNINTL BOOPNVP;
Standard_EXPORT Standard_Integer TopOpeBRep_SettraceNVP(const Standard_Boolean b,Standard_Integer n,char**a)
{ 
  Standard_Integer err = BOOPNVP.Set(b,n,a); if (n==0) TopOpeBRep_SettraceNVP(b); return err;
}

Standard_EXPORT Standard_Boolean TopOpeBRep_GettraceNVP(Standard_Integer n,char**a)
{
  return BOOPNVP.Get(n,a);
}

Standard_EXPORT Standard_Boolean TopOpeBRep_GettraceNVP(Standard_Integer i1,Standard_Integer i2,Standard_Integer i3,Standard_Integer i4,Standard_Integer i5)
{
  char* t[5]; t[0]=AS(i1);t[1]=AS(i2);t[2]=AS(i3);t[3]=AS(i4);t[4]=AS(i5);
  Standard_Boolean b = BOOPNVP.Get(5,t);return b;
}

// ===========
static Standard_Boolean TopOpeBRep_traceSHA = Standard_False;
Standard_EXPORT void TopOpeBRep_SettraceSHA(const Standard_Boolean b) { TopOpeBRep_traceSHA = b; }
Standard_EXPORT Standard_Boolean TopOpeBRep_GettraceSHA() { return TopOpeBRep_traceSHA; }

BOOPNINTL BOOPSHA;
Standard_EXPORT Standard_Integer TopOpeBRep_SettraceSHA(const Standard_Boolean b,Standard_Integer n,char**a)
{ 
  Standard_Integer err = BOOPSHA.Set(b,n,a); if (n==0) TopOpeBRep_SettraceSHA(b); return err;
}

Standard_EXPORT Standard_Boolean TopOpeBRep_GettraceSHA(Standard_Integer n,char**a)
{
  return BOOPSHA.Get(n,a);
}

Standard_EXPORT Standard_Boolean TopOpeBRep_GettraceSHA(const Standard_Integer i1)
{
  char* t[1]; t[0]=AS(i1);
  return BOOPSHA.Get(1,t);
}

// #ifdef OCCT_DEBUG
#endif
