// Copyright (c) 1995-1999 Matra Datavision
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


#include <IntSurf_LineOn2S.hxx>
#include <IntSurf_PntOn2S.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IntSurf_LineOn2S,Standard_Transient)

IntSurf_LineOn2S::
        IntSurf_LineOn2S(const IntSurf_Allocator& theAllocator) : mySeq(theAllocator)
{
  myBuv1.SetWhole();
  myBuv2.SetWhole();
  myBxyz.SetWhole();
}


Handle(IntSurf_LineOn2S) IntSurf_LineOn2S::Split (const Standard_Integer Index)
{
  IntSurf_SequenceOfPntOn2S SS;
  mySeq.Split(Index,SS);
  Handle(IntSurf_LineOn2S) NS = new IntSurf_LineOn2S ();
  Standard_Integer i;
  Standard_Integer leng = SS.Length();
  for (i=1; i<=leng; i++) {
    NS->Add(SS(i));
  }
  return NS;
}


void IntSurf_LineOn2S::InsertBefore(const Standard_Integer index, const IntSurf_PntOn2S& P) { 
  if(index>mySeq.Length()) { 
    mySeq.Append(P);
  }
  else { 
    mySeq.InsertBefore(index,P);
  }

  if (!myBxyz.IsWhole())
  {
    myBxyz.Add(P.Value());
  }

  if (!myBuv1.IsWhole())
  {
    myBuv1.Add(P.ValueOnSurface(Standard_True));
  }

  if (!myBuv2.IsWhole())
  {
    myBuv2.Add(P.ValueOnSurface(Standard_False));
  }
}

void IntSurf_LineOn2S::RemovePoint(const Standard_Integer index) { 
  mySeq.Remove(index);
  myBuv1.SetWhole();
  myBuv2.SetWhole();
  myBxyz.SetWhole();
}

Standard_Boolean IntSurf_LineOn2S::IsOutBox(const gp_Pnt& Pxyz)
{
  if (myBxyz.IsWhole())
  {
    Standard_Integer n = NbPoints();
    myBxyz.SetVoid();
    for (Standard_Integer i = 1; i <= n; i++)
    {
      gp_Pnt P = mySeq(i).Value();
      myBxyz.Add(P);
    }
    Standard_Real x0, y0, z0, x1, y1, z1;
    myBxyz.Get(x0, y0, z0, x1, y1, z1);
    x1 -= x0; y1 -= y0; z1 -= z0;
    if (x1>y1)
    {
      if (x1>z1)
      {
        myBxyz.Enlarge(x1*0.01);
      }
      else
      {
        myBxyz.Enlarge(z1*0.01);
      }
    }
    else
    {
      if (y1>z1)
      {
        myBxyz.Enlarge(y1*0.01);
      }
      else
      {
        myBxyz.Enlarge(z1*0.01);
      }
    }
  }
  Standard_Boolean out = myBxyz.IsOut(Pxyz);
  return(out);
}

Standard_Boolean IntSurf_LineOn2S::IsOutSurf1Box(const gp_Pnt2d& P1uv)
{
  if (myBuv1.IsWhole())
  {
    Standard_Integer n = NbPoints();
    Standard_Real pu1, pu2, pv1, pv2;
    myBuv1.SetVoid();
    for (Standard_Integer i = 1; i <= n; i++)
    {
      mySeq(i).Parameters(pu1, pv1, pu2, pv2);
      myBuv1.Add(gp_Pnt2d(pu1, pv1));
    }
    myBuv1.Get(pu1, pv1, pu2, pv2);
    pu2 -= pu1;
    pv2 -= pv1;
    if (pu2>pv2)
    {
      myBuv1.Enlarge(pu2*0.01);
    }
    else
    {
      myBuv1.Enlarge(pv2*0.01);
    }
  }
  Standard_Boolean out = myBuv1.IsOut(P1uv);
  return(out);
}

Standard_Boolean IntSurf_LineOn2S::IsOutSurf2Box(const gp_Pnt2d& P2uv)
{
  if (myBuv2.IsWhole())
  {
    Standard_Integer n = NbPoints();
    Standard_Real pu1, pu2, pv1, pv2;
    myBuv2.SetVoid();
    for (Standard_Integer i = 1; i <= n; i++)
    {
      mySeq(i).Parameters(pu1, pv1, pu2, pv2);
      myBuv2.Add(gp_Pnt2d(pu2, pv2));
    }
    myBuv2.Get(pu1, pv1, pu2, pv2);
    pu2 -= pu1;
    pv2 -= pv1;
    if (pu2>pv2)
    {
      myBuv2.Enlarge(pu2*0.01);
    }
    else
    {
      myBuv2.Enlarge(pv2*0.01);
    }
  }
  Standard_Boolean out = myBuv2.IsOut(P2uv);
  return(out);
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================
void IntSurf_LineOn2S::Add(const IntSurf_PntOn2S& P)
{
  mySeq.Append(P);
  if (!myBxyz.IsWhole())
  {
    myBxyz.Add(P.Value());
  }

  if (!myBuv1.IsWhole())
  {
    myBuv1.Add(P.ValueOnSurface(Standard_True));
  }

  if (!myBuv2.IsWhole())
  {
    myBuv2.Add(P.ValueOnSurface(Standard_False));
  }
}

//=======================================================================
//function : SetUV
//purpose  : 
//=======================================================================
void IntSurf_LineOn2S::SetUV(const Standard_Integer Index,
                             const Standard_Boolean OnFirst,
                             const Standard_Real U,
                             const Standard_Real V)
{
  mySeq(Index).SetValue(OnFirst, U, V);

  if (OnFirst && !myBuv1.IsWhole())
  {
    myBuv1.Add(gp_Pnt2d(U, V));
  }
  else if (!OnFirst && !myBuv2.IsWhole())
  {
    myBuv2.Add(gp_Pnt2d(U, V));
  }
}
