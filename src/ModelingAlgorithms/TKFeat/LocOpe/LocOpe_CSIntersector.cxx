// Created on: 1996-06-11
// Created by: Jacques GOUSSARD
// Copyright (c) 1996-1999 Matra Datavision
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

#include <Geom_Circle.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <IntCurvesFace_Intersector.hxx>
#include <LocOpe_CSIntersector.hxx>
#include <LocOpe_PntFace.hxx>
#include <NCollection_Sequence.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>

static bool LocAfter(const NCollection_Sequence<LocOpe_PntFace>&,
                     const double,
                     const double,
                     TopAbs_Orientation&,
                     int&,
                     int&);

static bool LocBefore(const NCollection_Sequence<LocOpe_PntFace>&,
                      const double,
                      const double,
                      TopAbs_Orientation&,
                      int&,
                      int&);

static bool LocAfter(const NCollection_Sequence<LocOpe_PntFace>&,
                     const int,
                     const double,
                     TopAbs_Orientation&,
                     int&,
                     int&);

static void AddPoints(IntCurvesFace_Intersector&,
                      NCollection_Sequence<LocOpe_PntFace>&,
                      const TopoDS_Face&);

//=================================================================================================

void LocOpe_CSIntersector::Init(const TopoDS_Shape& S)
{
  myDone  = false;
  myShape = S;
  delete[] (NCollection_Sequence<LocOpe_PntFace>*)myPoints;
  myPoints = nullptr;
  myNbelem = 0;
}

//=================================================================================================

void LocOpe_CSIntersector::Perform(const NCollection_Sequence<gp_Lin>& Slin)
{
  if (myShape.IsNull() || Slin.Length() <= 0)
  {
    throw Standard_ConstructionError();
  }
  myDone = false;

  myNbelem = Slin.Length();
  delete[] (NCollection_Sequence<LocOpe_PntFace>*)myPoints;
  myPoints =
    (NCollection_Sequence<LocOpe_PntFace>*)new NCollection_Sequence<LocOpe_PntFace>[myNbelem];

  constexpr double binf = RealFirst();
  constexpr double bsup = RealLast();
  TopExp_Explorer  exp(myShape, TopAbs_FACE);
  for (; exp.More(); exp.Next())
  {
    const TopoDS_Face&        theface = TopoDS::Face(exp.Current());
    IntCurvesFace_Intersector theInt(theface, Precision::PConfusion());
    for (int i = 1; i <= myNbelem; i++)
    {
      theInt.Perform(Slin(i), binf, bsup);
      if (theInt.IsDone())
      {
        AddPoints(theInt, (((NCollection_Sequence<LocOpe_PntFace>*)myPoints)[i - 1]), theface);
      }
    }
  }
  myDone = true;
}

//=================================================================================================

void LocOpe_CSIntersector::Perform(const NCollection_Sequence<gp_Circ>& Scir)
{
  if (myShape.IsNull() || Scir.Length() <= 0)
  {
    throw Standard_ConstructionError();
  }
  myDone = false;

  myNbelem = Scir.Length();
  delete[] (NCollection_Sequence<LocOpe_PntFace>*)myPoints;
  myPoints =
    (NCollection_Sequence<LocOpe_PntFace>*)new NCollection_Sequence<LocOpe_PntFace>[myNbelem];

  TopExp_Explorer                exp(myShape, TopAbs_FACE);
  occ::handle<GeomAdaptor_Curve> HC   = new GeomAdaptor_Curve();
  double                         binf = 0.;
  double                         bsup = 2. * M_PI;

  for (; exp.More(); exp.Next())
  {
    const TopoDS_Face&        theface = TopoDS::Face(exp.Current());
    IntCurvesFace_Intersector theInt(theface, 0.);
    for (int i = 1; i <= myNbelem; i++)
    {

      HC->Load(new Geom_Circle(Scir(i)));
      theInt.Perform(HC, binf, bsup);
      if (theInt.IsDone())
      {
        AddPoints(theInt, (((NCollection_Sequence<LocOpe_PntFace>*)myPoints)[i - 1]), theface);
      }
    }
  }
  myDone = true;
}

//=================================================================================================

void LocOpe_CSIntersector::Perform(const NCollection_Sequence<occ::handle<Geom_Curve>>& Scur)
{
  if (myShape.IsNull() || Scur.Length() <= 0)
  {
    throw Standard_ConstructionError();
  }
  myDone = false;

  myNbelem = Scur.Length();
  delete[] (NCollection_Sequence<LocOpe_PntFace>*)myPoints;
  myPoints =
    (NCollection_Sequence<LocOpe_PntFace>*)new NCollection_Sequence<LocOpe_PntFace>[myNbelem];

  TopExp_Explorer                exp(myShape, TopAbs_FACE);
  occ::handle<GeomAdaptor_Curve> HC = new GeomAdaptor_Curve();
  for (; exp.More(); exp.Next())
  {
    const TopoDS_Face&        theface = TopoDS::Face(exp.Current());
    IntCurvesFace_Intersector theInt(theface, 0.);
    for (int i = 1; i <= myNbelem; i++)
    {
      if (Scur(i).IsNull())
      {
        continue;
      }
      HC->Load(Scur(i));
      double binf = HC->FirstParameter();
      double bsup = HC->LastParameter();
      theInt.Perform(HC, binf, bsup);
      if (theInt.IsDone())
      {
        AddPoints(theInt, (((NCollection_Sequence<LocOpe_PntFace>*)myPoints)[i - 1]), theface);
      }
    }
  }
  myDone = true;
}

//=================================================================================================

int LocOpe_CSIntersector::NbPoints(const int I) const
{
  if (!myDone)
  {
    throw StdFail_NotDone();
  }
  if (I <= 0 || I > myNbelem)
  {
    throw Standard_OutOfRange();
  }
  return ((NCollection_Sequence<LocOpe_PntFace>*)myPoints)[I - 1].Length();
}

//=================================================================================================

const LocOpe_PntFace& LocOpe_CSIntersector::Point(const int I, const int Index) const
{
  if (!myDone)
  {
    throw StdFail_NotDone();
  }
  if (I <= 0 || I > myNbelem)
  {
    throw Standard_OutOfRange();
  }
  return ((NCollection_Sequence<LocOpe_PntFace>*)myPoints)[I - 1](Index);
}

//=================================================================================================

void LocOpe_CSIntersector::Destroy()
{
  delete[] (NCollection_Sequence<LocOpe_PntFace>*)myPoints;
  myPoints = nullptr;
}

//=================================================================================================

bool LocOpe_CSIntersector::LocalizeAfter(const int           I,
                                         const double        From,
                                         const double        Tol,
                                         TopAbs_Orientation& Or,
                                         int&                IndFrom,
                                         int&                IndTo) const
{
  if (!myDone)
  {
    throw StdFail_NotDone();
  }
  if (I <= 0 || I > myNbelem)
  {
    throw Standard_OutOfRange();
  }
  return LocAfter((((NCollection_Sequence<LocOpe_PntFace>*)myPoints)[I - 1]),
                  From,
                  Tol,
                  Or,
                  IndFrom,
                  IndTo);
}

//=================================================================================================

bool LocOpe_CSIntersector::LocalizeBefore(const int           I,
                                          const double        From,
                                          const double        Tol,
                                          TopAbs_Orientation& Or,
                                          int&                IndFrom,
                                          int&                IndTo) const
{
  if (!myDone)
  {
    throw StdFail_NotDone();
  }
  if (I <= 0 || I > myNbelem)
  {
    throw Standard_OutOfRange();
  }
  return LocBefore(((NCollection_Sequence<LocOpe_PntFace>*)myPoints)[I - 1],
                   From,
                   Tol,
                   Or,
                   IndFrom,
                   IndTo);
}

//=================================================================================================

bool LocOpe_CSIntersector::LocalizeAfter(const int           I,
                                         const int           FromInd,
                                         const double        Tol,
                                         TopAbs_Orientation& Or,
                                         int&                IndFrom,
                                         int&                IndTo) const
{
  if (!myDone)
  {
    throw StdFail_NotDone();
  }
  if (I <= 0 || I > myNbelem)
  {
    throw Standard_OutOfRange();
  }
  return LocAfter(((NCollection_Sequence<LocOpe_PntFace>*)myPoints)[I - 1],
                  FromInd,
                  Tol,
                  Or,
                  IndFrom,
                  IndTo);
}

//=================================================================================================

bool LocOpe_CSIntersector::LocalizeBefore(const int           I,
                                          const int           FromInd,
                                          const double        Tol,
                                          TopAbs_Orientation& Or,
                                          int&                IndFrom,
                                          int&                IndTo) const
{
  if (!myDone)
  {
    throw StdFail_NotDone();
  }
  if (I <= 0 || I > myNbelem)
  {
    throw Standard_OutOfRange();
  }
  return LocBefore(((NCollection_Sequence<LocOpe_PntFace>*)myPoints)[I - 1],
                   FromInd,
                   Tol,
                   Or,
                   IndFrom,
                   IndTo);
}

//=================================================================================================

static bool LocAfter(const NCollection_Sequence<LocOpe_PntFace>& Spt,
                     const double                                From,
                     const double                                Tol,
                     TopAbs_Orientation&                         Or,
                     int&                                        IndFrom,
                     int&                                        IndTo)
{

  double param, FMEPS = From - Tol;
  int    i, ifirst, nbpoints = Spt.Length();
  for (ifirst = 1; ifirst <= nbpoints; ifirst++)
  {
    if (Spt(ifirst).Parameter() >= FMEPS)
    {
      break;
    }
  }
  bool RetVal = false;
  if (ifirst <= nbpoints)
  {
    i          = ifirst;
    IndFrom    = ifirst;
    bool found = false;
    while (!found)
    {
      Or    = Spt(i).Orientation();
      param = Spt(i).Parameter();
      i     = i + 1;
      while (i <= nbpoints)
      {
        if (Spt(i).Parameter() - param <= Tol)
        {
          if (Or != TopAbs_EXTERNAL && Or != Spt(i).Orientation())
          {
            Or = TopAbs_EXTERNAL;
          }
          i++;
        }
        else
        {
          break;
        }
      }
      if (Or == TopAbs_EXTERNAL)
      {
        found   = (i > nbpoints);
        IndFrom = i;
      }
      else
      { // on a une intersection franche
        IndTo  = i - 1;
        found  = true;
        RetVal = true;
      }
    }
  }

  return RetVal;
}

//=================================================================================================

static bool LocBefore(const NCollection_Sequence<LocOpe_PntFace>& Spt,
                      const double                                From,
                      const double                                Tol,
                      TopAbs_Orientation&                         Or,
                      int&                                        IndFrom,
                      int&                                        IndTo)
{
  double param, FPEPS = From + Tol;
  int    i, ifirst, nbpoints = Spt.Length();
  for (ifirst = nbpoints; ifirst >= 1; ifirst--)
  {
    if (Spt(ifirst).Parameter() <= FPEPS)
    {
      break;
    }
  }
  bool RetVal = false;
  if (ifirst >= 1)
  {
    i          = ifirst;
    IndTo      = ifirst;
    bool found = false;
    while (!found)
    {
      Or    = Spt(i).Orientation();
      param = Spt(i).Parameter();
      i     = i - 1;
      while (i >= 1)
      {
        if (param - Spt(i).Parameter() <= Tol)
        {
          if (Or != TopAbs_EXTERNAL && Or != Spt(i).Orientation())
          {
            Or = TopAbs_EXTERNAL;
          }
          i--;
        }
        else
        {
          break;
        }
      }
      if (Or == TopAbs_EXTERNAL)
      {
        found = (i < 1);
        IndTo = i;
      }
      else
      { // on a une intersection franche
        IndFrom = i + 1;
        found   = true;
        RetVal  = true;
      }
    }
  }

  return RetVal;
}

//=================================================================================================

static bool LocAfter(const NCollection_Sequence<LocOpe_PntFace>& Spt,
                     const int                                   FromInd,
                     const double                                Tol,
                     TopAbs_Orientation&                         Or,
                     int&                                        IndFrom,
                     int&                                        IndTo)
{
  int nbpoints = Spt.Length();
  if (FromInd >= nbpoints)
  {
    return false;
  }

  double param, FMEPS;
  int    i, ifirst;
  if (FromInd >= 1)
  {
    FMEPS = Spt(FromInd).Parameter() - Tol;
    for (ifirst = FromInd + 1; ifirst <= nbpoints; ifirst++)
    {
      if (Spt(ifirst).Parameter() >= FMEPS)
      {
        break;
      }
    }
  }
  else
  {
    ifirst = 1;
  }

  bool RetVal = false;
  if (ifirst <= nbpoints)
  {
    i          = ifirst;
    IndFrom    = ifirst;
    bool found = false;
    while (!found)
    {
      Or    = Spt(i).Orientation();
      param = Spt(i).Parameter();
      i     = i + 1;
      while (i <= nbpoints)
      {
        if (Spt(i).Parameter() - param <= Tol)
        {
          if (Or != TopAbs_EXTERNAL && Or != Spt(i).Orientation())
          {
            Or = TopAbs_EXTERNAL;
          }
          i++;
        }
        else
        {
          break;
        }
      }
      if (Or == TopAbs_EXTERNAL)
      {
        found   = (i > nbpoints);
        IndFrom = i;
      }
      else
      { // on a une intersection franche
        IndTo  = i - 1;
        found  = true;
        RetVal = true;
      }
    }
  }
  return RetVal;
}

//=================================================================================================

static void AddPoints(IntCurvesFace_Intersector&            theInt,
                      NCollection_Sequence<LocOpe_PntFace>& theSeq,
                      const TopoDS_Face&                    theface)
{
  int    nbpoints = theSeq.Length();
  int    newpnt   = theInt.NbPnt();
  double param, paramu, paramv;
  for (int j = 1; j <= newpnt; j++)
  {
    const gp_Pnt& thept = theInt.Pnt(j);
    param               = theInt.WParameter(j);
    paramu              = theInt.UParameter(j);
    paramv              = theInt.VParameter(j);

    TopAbs_Orientation theor = TopAbs_FORWARD;

    switch (theInt.Transition(j))
    {
      case IntCurveSurface_In:
        /* JAG 13.09.96
              if ( orface == TopAbs_FORWARD) {
            theor = TopAbs_FORWARD;
              }
              else if (orface == TopAbs_REVERSED) {
            theor = TopAbs_REVERSED;
              }
              else {
            theor = TopAbs_EXTERNAL;
              }
        */
        theor = TopAbs_FORWARD;

        break;
      case IntCurveSurface_Out:
        /* JAG 13.09.96
              if ( orface == TopAbs_FORWARD) {
            theor = TopAbs_REVERSED;
              }
              else if (orface == TopAbs_REVERSED) {
            theor = TopAbs_FORWARD;
              }
              else {
            theor = TopAbs_EXTERNAL;
              }
        */
        theor = TopAbs_REVERSED;
        break;
      case IntCurveSurface_Tangent:
        theor = TopAbs_EXTERNAL;
        break;
    }
    LocOpe_PntFace newpt(thept, theface, theor, param, paramu, paramv);
    //    for (int k=1; k <= nbpoints; k++) {
    int k;
    for (k = 1; k <= nbpoints; k++)
    {
      if (theSeq(k).Parameter() - param > 0.)
      {
        break;
      }
    }
    if (k <= nbpoints)
    {
      theSeq.InsertBefore(k, newpt);
    }
    else
    {
      theSeq.Append(newpt);
    }
    nbpoints++;
  }
}
