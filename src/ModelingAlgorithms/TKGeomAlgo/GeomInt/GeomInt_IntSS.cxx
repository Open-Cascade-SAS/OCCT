// Created on: 1995-01-27
// Created by: Jacques GOUSSARD
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

#include <GeomInt_IntSS.hxx>

#include <Adaptor3d_TopolTool.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Extrema_ExtPS.hxx>

//=================================================================================================

void GeomInt_IntSS::Perform(const occ::handle<Geom_Surface>& S1,
                            const occ::handle<Geom_Surface>& S2,
                            const double         Tol,
                            const bool      Approx,
                            const bool      ApproxS1,
                            const bool      ApproxS2)
{
  myHS1 = new GeomAdaptor_Surface(S1);
  if (S1 == S2)
    myHS2 = myHS1;
  else
    myHS2 = new GeomAdaptor_Surface(S2);
  InternalPerform(Tol, Approx, ApproxS1, ApproxS2, false, 0., 0., 0., 0.);
}

//=======================================================================
// function : Perform
// purpose  : General intersection with a Starting Point
//=======================================================================
void GeomInt_IntSS::Perform(const occ::handle<Geom_Surface>& S1,
                            const occ::handle<Geom_Surface>& S2,
                            const double         Tol,
                            const double         U1,
                            const double         V1,
                            const double         U2,
                            const double         V2,
                            const bool      Approx,
                            const bool      ApproxS1,
                            const bool      ApproxS2)
{
  myHS1 = new GeomAdaptor_Surface(S1);
  if (S1 == S2)
    myHS2 = myHS1;
  else
    myHS2 = new GeomAdaptor_Surface(S2);
  InternalPerform(Tol, Approx, ApproxS1, ApproxS2, true, U1, V1, U2, V2);
}

//=================================================================================================

void GeomInt_IntSS::InternalPerform(const double    Tol,
                                    const bool Approx,
                                    const bool ApproxS1,
                                    const bool ApproxS2,
                                    const bool useStart,
                                    const double    U1,
                                    const double    V1,
                                    const double    U2,
                                    const double    V2)
{
  myTolReached2d = myTolReached3d = 0.0;
  myNbrestr                       = 0;
  sline.Clear();

  double TolArc     = Tol;
  double TolTang    = Tol;
  double Deflection = 0.1;
  if (myHS1->GetType() == GeomAbs_BSplineSurface && myHS2->GetType() == GeomAbs_BSplineSurface)
  {
    Deflection /= 10.;
  }

  occ::handle<Adaptor3d_TopolTool> dom1 = new Adaptor3d_TopolTool(myHS1);
  occ::handle<Adaptor3d_TopolTool> dom2 = new Adaptor3d_TopolTool(myHS2);

  NCollection_Vector<occ::handle<Adaptor3d_Surface>> aVecHS1;
  NCollection_Vector<occ::handle<Adaptor3d_Surface>> aVecHS2;

  if (myHS1 == myHS2)
  {
    aVecHS1.Append(myHS1);
    aVecHS2.Append(myHS2);
  }
  else
  {
    myIntersector.PrepareSurfaces(myHS1, dom1, myHS2, dom2, Tol, aVecHS1, aVecHS2);
  }

  for (int aNumOfHS1 = 0; aNumOfHS1 < aVecHS1.Length(); aNumOfHS1++)
  {
    const occ::handle<Adaptor3d_Surface>& aHS1 = aVecHS1.Value(aNumOfHS1);

    for (int aNumOfHS2 = 0; aNumOfHS2 < aVecHS2.Length(); aNumOfHS2++)
    {
      const occ::handle<Adaptor3d_Surface>& aHS2 = aVecHS2.Value(aNumOfHS2);

      occ::handle<Adaptor3d_TopolTool> aDom1 = new Adaptor3d_TopolTool(aHS1);
      occ::handle<Adaptor3d_TopolTool> aDom2 = new Adaptor3d_TopolTool(aHS2);

      myLConstruct.Load(aDom1,
                        aDom2,
                        occ::down_cast<GeomAdaptor_Surface>(aHS1),
                        occ::down_cast<GeomAdaptor_Surface>(aHS2));

      double UVMaxStep = IntPatch_Intersection::DefineUVMaxStep(aHS1, aDom1, aHS2, aDom2);

      myIntersector.SetTolerances(TolArc, TolTang, UVMaxStep, Deflection);

      if (aHS1 == aHS2)
      {
        myIntersector.Perform(aHS1, aDom1, TolArc, TolTang);
      }
      else if (!useStart)
      {
        myIntersector.Perform(aHS1, aDom1, aHS2, aDom2, TolArc, TolTang);
      }
      else
      {
        TopAbs_State aState1 = aDom1->Classify(gp_Pnt2d(U1, V1), Tol);
        TopAbs_State aState2 = aDom2->Classify(gp_Pnt2d(U2, V2), Tol);

        if ((aState1 == TopAbs_IN || aState1 == TopAbs_ON)
            && (aState2 == TopAbs_IN || aState2 == TopAbs_ON))
        {
          myIntersector.Perform(aHS1, aDom1, aHS2, aDom2, U1, V1, U2, V2, TolArc, TolTang);
        }
      }

      // ============================================================
      if (myIntersector.IsDone())
      {
        const int nblin = myIntersector.NbLines();
        for (int i = 1; i <= nblin; i++)
        {
          MakeCurve(i, aDom1, aDom2, Tol, Approx, ApproxS1, ApproxS2);
        }
      }
    }
  }
}

//=================================================================================================

const occ::handle<Geom_Curve>& GeomInt_IntSS::Line(const int Index) const
{
  StdFail_NotDone_Raise_if(!myIntersector.IsDone(), "GeomInt_IntSS::Line");
  return sline(Index + myNbrestr);
}

//=================================================================================================

const occ::handle<Geom_Curve>& GeomInt_IntSS::Boundary(const int Index) const
{
  StdFail_NotDone_Raise_if(!myIntersector.IsDone(), "GeomInt_IntSS::Line");
  Standard_OutOfRange_Raise_if(Index <= 0 || Index > myNbrestr, "GeomInt_IntSS::Boundary");
  return sline(Index);
}

//=================================================================================================

gp_Pnt2d GeomInt_IntSS::Pnt2d(const int Index, const bool OnFirst) const
{
  const IntPatch_Point& thept = myIntersector.Point(Index);
  double         U, V;
  if (OnFirst)
    thept.ParametersOnS1(U, V);
  else
    thept.ParametersOnS2(U, V);
  return gp_Pnt2d(U, V);
}

//=================================================================================================

bool GeomInt_IntSS::HasLineOnS1(const int index) const
{
  StdFail_NotDone_Raise_if(!myIntersector.IsDone(), "GeomInt_IntSS::HasLineOnS1");
  return (!slineS1(index).IsNull());
}

//=================================================================================================

bool GeomInt_IntSS::HasLineOnS2(const int index) const
{
  StdFail_NotDone_Raise_if(!myIntersector.IsDone(), "GeomInt_IntSS::HasLineOnS2");
  return (!slineS2(index).IsNull());
}

//=================================================================================================

const occ::handle<Geom2d_Curve>& GeomInt_IntSS::LineOnS1(const int Index) const
{
  StdFail_NotDone_Raise_if(!myIntersector.IsDone(), "GeomInt_IntSS::LineOnS1");
  return slineS1(Index);
}

//=================================================================================================

const occ::handle<Geom2d_Curve>& GeomInt_IntSS::LineOnS2(const int Index) const
{
  StdFail_NotDone_Raise_if(!myIntersector.IsDone(), "GeomInt_IntSS::LineOnS2");
  return slineS2(Index);
}
