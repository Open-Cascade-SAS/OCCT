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

IMPLEMENT_STANDARD_RTTIEXT(IntSurf_LineOn2S, Standard_Transient)

IntSurf_LineOn2S::IntSurf_LineOn2S(const IntSurf_Allocator& /*theAllocator*/)
{
  // The allocator hint is ignored: std::vector uses the default allocator.
  // Profiles show this is a net win because random access through the line is
  // O(1) instead of O(N) (NCollection_BaseSequence::Find).
  myBuv1.SetWhole();
  myBuv2.SetWhole();
  myBxyz.SetWhole();
}

occ::handle<IntSurf_LineOn2S> IntSurf_LineOn2S::Split(const int Index)
{
  occ::handle<IntSurf_LineOn2S> NS = new IntSurf_LineOn2S();
  if (Index >= 1 && Index <= static_cast<int>(mySeq.size()))
  {
    for (auto it = mySeq.begin() + (Index - 1); it != mySeq.end(); ++it)
    {
      NS->Add(*it);
    }
    mySeq.erase(mySeq.begin() + (Index - 1), mySeq.end());
  }
  return NS;
}

void IntSurf_LineOn2S::InsertBefore(const int index, const IntSurf_PntOn2S& P)
{
  if (index > static_cast<int>(mySeq.size()))
  {
    mySeq.push_back(P);
  }
  else
  {
    mySeq.insert(mySeq.begin() + (index - 1), P);
  }

  if (!myBxyz.IsWhole())
  {
    myBxyz.Add(P.Value());
  }

  if (!myBuv1.IsWhole())
  {
    myBuv1.Add(P.ValueOnSurface(true));
  }

  if (!myBuv2.IsWhole())
  {
    myBuv2.Add(P.ValueOnSurface(false));
  }
}

void IntSurf_LineOn2S::RemovePoint(const int index)
{
  if (index >= 1 && index <= static_cast<int>(mySeq.size()))
  {
    mySeq.erase(mySeq.begin() + (index - 1));
  }
  myBuv1.SetWhole();
  myBuv2.SetWhole();
  myBxyz.SetWhole();
}

bool IntSurf_LineOn2S::IsOutBox(const gp_Pnt& Pxyz)
{
  if (NbPoints() == 0)
  {
    return false;
  }

  if (myBxyz.IsWhole())
  {
    int n = NbPoints();
    myBxyz.SetVoid();
    for (const auto& pt : mySeq)
    {
      myBxyz.Add(pt.Value());
    }
    (void)n;
    double x0, y0, z0, x1, y1, z1;
    myBxyz.Get(x0, y0, z0, x1, y1, z1);
    x1 -= x0;
    y1 -= y0;
    z1 -= z0;
    if (x1 > y1)
    {
      if (x1 > z1)
      {
        myBxyz.Enlarge(x1 * 0.01);
      }
      else
      {
        myBxyz.Enlarge(z1 * 0.01);
      }
    }
    else
    {
      if (y1 > z1)
      {
        myBxyz.Enlarge(y1 * 0.01);
      }
      else
      {
        myBxyz.Enlarge(z1 * 0.01);
      }
    }
  }
  bool out = myBxyz.IsOut(Pxyz);
  return (out);
}

bool IntSurf_LineOn2S::IsOutSurf1Box(const gp_Pnt2d& P1uv)
{
  if (NbPoints() == 0)
  {
    return false;
  }

  if (myBuv1.IsWhole())
  {
    double pu1, pu2, pv1, pv2;
    myBuv1.SetVoid();
    for (const auto& pt : mySeq)
    {
      pt.Parameters(pu1, pv1, pu2, pv2);
      myBuv1.Add(gp_Pnt2d(pu1, pv1));
    }
    myBuv1.Get(pu1, pv1, pu2, pv2);
    pu2 -= pu1;
    pv2 -= pv1;
    if (pu2 > pv2)
    {
      myBuv1.Enlarge(pu2 * 0.01);
    }
    else
    {
      myBuv1.Enlarge(pv2 * 0.01);
    }
  }
  bool out = myBuv1.IsOut(P1uv);
  return (out);
}

bool IntSurf_LineOn2S::IsOutSurf2Box(const gp_Pnt2d& P2uv)
{
  if (NbPoints() == 0)
  {
    return false;
  }

  if (myBuv2.IsWhole())
  {
    double pu1, pu2, pv1, pv2;
    myBuv2.SetVoid();
    for (const auto& pt : mySeq)
    {
      pt.Parameters(pu1, pv1, pu2, pv2);
      myBuv2.Add(gp_Pnt2d(pu2, pv2));
    }
    myBuv2.Get(pu1, pv1, pu2, pv2);
    pu2 -= pu1;
    pv2 -= pv1;
    if (pu2 > pv2)
    {
      myBuv2.Enlarge(pu2 * 0.01);
    }
    else
    {
      myBuv2.Enlarge(pv2 * 0.01);
    }
  }
  bool out = myBuv2.IsOut(P2uv);
  return (out);
}

//=================================================================================================

void IntSurf_LineOn2S::Add(const IntSurf_PntOn2S& P)
{
  mySeq.push_back(P);
  if (!myBxyz.IsWhole())
  {
    myBxyz.Add(P.Value());
  }

  if (!myBuv1.IsWhole())
  {
    myBuv1.Add(P.ValueOnSurface(true));
  }

  if (!myBuv2.IsWhole())
  {
    myBuv2.Add(P.ValueOnSurface(false));
  }
}

//=================================================================================================

void IntSurf_LineOn2S::SetUV(const int Index, const bool OnFirst, const double U, const double V)
{
  mySeq[Index - 1].SetValue(OnFirst, U, V);

  if (OnFirst && !myBuv1.IsWhole())
  {
    myBuv1.Add(gp_Pnt2d(U, V));
  }
  else if (!OnFirst && !myBuv2.IsWhole())
  {
    myBuv2.Add(gp_Pnt2d(U, V));
  }
}
