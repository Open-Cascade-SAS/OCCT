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
  #include <Standard_Integer.hxx>
  #include <NCollection_List.hxx>
  #include <TopAbs_ShapeEnum.hxx>
  #include <TopAbs_Orientation.hxx>
  #include <TopAbs_State.hxx>
  #include <TopoDS_Shape.hxx>
  #include <TopTools_ShapeMapHasher.hxx>
  #include <NCollection_Map.hxx>
  #include <NCollection_IndexedMap.hxx>
  #include <NCollection_DataMap.hxx>
  #include <NCollection_IndexedDataMap.hxx>
  #include <TopoDS_Face.hxx>
  #include <TopoDS_Edge.hxx>
  #include <TopoDS_Vertex.hxx>

  #define AS(x) (Standard_PCharacter) TCollection_AsciiString((x)).ToCString();
  #define I 10
  #define J 10
  #define OK(x, y) (x) < I && (y) < J
  #define T(x, y) myT[(x)][(y)]
  #define L0(x, y) for ((x) = 0; (x) < (y); (x)++)
  #define A(v) (atoi(a[(v)]))

class BOOPNINTL
{
public:
  BOOPNINTL();
  bool Get(int n, char** a);
  int  Set(const bool b, int n, char** a);

private:
  NCollection_List<int> myTL[I];
  bool                  myTB[I];
  int                   mynl;
};

BOOPNINTL::BOOPNINTL()
{
  Set(false, 0, NULL);
}

int BOOPNINTL::Set(const bool b, int n, char** a)
{
  if (n == 0)
  {
    int i;
    L0(i, I)
    {
      myTB[i] = false;
      myTL[i].Clear();
    }
    mynl = 0;
    return 0;
  }
  else if (mynl + 1 < I)
  {
    myTB[mynl] = b;
    int i;
    L0(i, n) myTL[mynl].Append(atoi(a[i]));
    mynl++;
    return 0;
  }
  else
  {
    return 1;
  }
}

bool BOOPNINTL::Get(int n, char** a)
{
  if (!n)
    return false;
  int il;
  L0(il, mynl)
  {
    const NCollection_List<int>& L = myTL[il];
    if (L.IsEmpty())
      continue;
    NCollection_List<int>::Iterator itL(L);
    int                             ia    = 0;
    bool                            found = true;
    for (; itL.More() && (ia < n); itL.Next(), ia++)
    {
      int Lval = itL.Value();
      int aval = atoi(a[ia]);
      if (Lval != aval)
      {
        found = false;
        break;
      }
    }
    if (found)
    {
      return myTB[il];
    }
  }
  return false;
}

// ===========
static bool TopOpeBRep_traceEEFF = false;

Standard_EXPORT void TopOpeBRep_SettraceEEFF(const bool b)
{
  TopOpeBRep_traceEEFF = b;
}

Standard_EXPORT bool TopOpeBRep_GettraceEEFF()
{
  return TopOpeBRep_traceEEFF;
}

BOOPNINTL BOOPEEFF;

Standard_EXPORT int TopOpeBRep_SettraceEEFF(const bool b, int n, char** a)
{
  int err = BOOPEEFF.Set(b, n, a);
  if (n == 0)
    TopOpeBRep_SettraceEEFF(b);
  return err;
}

Standard_EXPORT bool TopOpeBRep_GettraceEEFF(int n, char** a)
{
  bool b = BOOPEEFF.Get(n, a);
  return b;
}

Standard_EXPORT bool TopOpeBRep_GettraceEEFF(const int i1, const int i2, const int i3, const int i4)
{
  char* t[4];
  t[0]   = AS(i1);
  t[1]   = AS(i2);
  t[2]   = AS(i3);
  t[3]   = AS(i4);
  bool b = BOOPEEFF.Get(4, t);
  return b;
}

// ===========
static bool TopOpeBRep_traceNVP = false;

Standard_EXPORT void TopOpeBRep_SettraceNVP(const bool b)
{
  TopOpeBRep_traceNVP = b;
}

Standard_EXPORT bool TopOpeBRep_GettraceNVP()
{
  return TopOpeBRep_traceNVP;
}

BOOPNINTL BOOPNVP;

Standard_EXPORT int TopOpeBRep_SettraceNVP(const bool b, int n, char** a)
{
  int err = BOOPNVP.Set(b, n, a);
  if (n == 0)
    TopOpeBRep_SettraceNVP(b);
  return err;
}

Standard_EXPORT bool TopOpeBRep_GettraceNVP(int n, char** a)
{
  return BOOPNVP.Get(n, a);
}

Standard_EXPORT bool TopOpeBRep_GettraceNVP(int i1, int i2, int i3, int i4, int i5)
{
  char* t[5];
  t[0]   = AS(i1);
  t[1]   = AS(i2);
  t[2]   = AS(i3);
  t[3]   = AS(i4);
  t[4]   = AS(i5);
  bool b = BOOPNVP.Get(5, t);
  return b;
}

// ===========
static bool TopOpeBRep_traceSHA = false;

Standard_EXPORT void TopOpeBRep_SettraceSHA(const bool b)
{
  TopOpeBRep_traceSHA = b;
}

Standard_EXPORT bool TopOpeBRep_GettraceSHA()
{
  return TopOpeBRep_traceSHA;
}

BOOPNINTL BOOPSHA;

Standard_EXPORT int TopOpeBRep_SettraceSHA(const bool b, int n, char** a)
{
  int err = BOOPSHA.Set(b, n, a);
  if (n == 0)
    TopOpeBRep_SettraceSHA(b);
  return err;
}

Standard_EXPORT bool TopOpeBRep_GettraceSHA(int n, char** a)
{
  return BOOPSHA.Get(n, a);
}

Standard_EXPORT bool TopOpeBRep_GettraceSHA(const int i1)
{
  char* t[1];
  t[0] = AS(i1);
  return BOOPSHA.Get(1, t);
}

// #ifdef OCCT_DEBUG
#endif
