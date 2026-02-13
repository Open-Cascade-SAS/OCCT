// Created on: 1998-05-12
// Created by: Philippe NOUAILLE
// Copyright (c) 1998-1999 Matra Datavision
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

#include <Blend_AppFunction.hxx>
#include <BRepBlend_AppFuncRoot.hxx>
#include <BRepBlend_Line.hxx>
#include <gp_Pnt.hxx>
#include <math_FunctionSetRoot.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepBlend_AppFuncRoot, Approx_SweepFunction)

BRepBlend_AppFuncRoot::BRepBlend_AppFuncRoot(occ::handle<BRepBlend_Line>& Line,
                                             Blend_AppFunction&           Func,
                                             const double                 Tol3d,
                                             const double                 Tol2d)
    : myLine(Line),
      myFunc(&Func),
      myTolerance(1, Func.NbVariables()),
      X1(1, Func.NbVariables()),
      X2(1, Func.NbVariables()),
      XInit(1, Func.NbVariables()),
      Sol(1, Func.NbVariables())
{
  int NbPoles, NbKnots, Degree, NbPoles2d;
  int ii;

  //  Tolerances
  Func.GetTolerance(myTolerance, Tol3d);
  int dim = Func.NbVariables();
  for (ii = 1; ii <= dim; ii++)
  {
    if (myTolerance(ii) > Tol2d)
    {
      myTolerance(ii) = Tol2d;
    }
  }

  //  Tables
  Func.GetShape(NbPoles, NbKnots, Degree, NbPoles2d);

  // Calculation of BaryCentre (rational case).
  if (Func.IsRational())
  {
    double Xmax = -1.e100, Xmin = 1.e100, Ymax = -1.e100, Ymin = 1.e100, Zmax = -1.e100,
           Zmin = 1.e100;
    Blend_Point P;
    for (ii = 1; ii <= myLine->NbPoints(); ii++)
    {
      P    = myLine->Point(ii);
      Xmax = std::max(std::max(P.PointOnS1().X(), P.PointOnS2().X()), Xmax);
      Xmin = std::min(std::min(P.PointOnS1().X(), P.PointOnS2().X()), Xmin);
      Ymax = std::max(std::max(P.PointOnS1().Y(), P.PointOnS2().Y()), Ymax);
      Ymin = std::min(std::min(P.PointOnS1().Y(), P.PointOnS2().Y()), Ymin);
      Zmax = std::max(std::max(P.PointOnS1().Z(), P.PointOnS2().Z()), Zmax);
      Zmin = std::min(std::min(P.PointOnS1().Z(), P.PointOnS2().Z()), Zmin);

      myBary.SetCoord((Xmax + Xmin) / 2, (Ymax + Ymin) / 2, (Zmax + Zmin) / 2);
    }
  }
  else
  {
    myBary.SetCoord(0, 0, 0);
  }

  mySolver = std::make_unique<math_FunctionSetRoot>(Func, myTolerance, 30);
}

//==================================================================================================

BRepBlend_AppFuncRoot::~BRepBlend_AppFuncRoot() = default;

//================================================================================
// Function: D0
// Purpose : Calculation of section for v = Param, if calculation fails
//           false is raised.
//================================================================================
bool BRepBlend_AppFuncRoot::D0(const double Param,
                               const double /*First*/,
                               const double /*Last*/,
                               NCollection_Array1<gp_Pnt>&   Poles,
                               NCollection_Array1<gp_Pnt2d>& Poles2d,
                               NCollection_Array1<double>&   Weigths)
{
  bool               Ok   = true;
  Blend_AppFunction* Func = (Blend_AppFunction*)myFunc;
  Ok                      = SearchPoint(*Func, Param, myPnt);

  if (Ok)
    (*Func).Section(myPnt, Poles, Poles2d, Weigths);
  return Ok;
}

//================================================================================
// Function: D1
// Purpose : Calculation of the partial derivative of the section corresponding to v
//           for v = Param, if the calculation fails false is raised.
//================================================================================
bool BRepBlend_AppFuncRoot::D1(const double Param,
                               const double /*First*/,
                               const double /*Last*/,
                               NCollection_Array1<gp_Pnt>&   Poles,
                               NCollection_Array1<gp_Vec>&   DPoles,
                               NCollection_Array1<gp_Pnt2d>& Poles2d,
                               NCollection_Array1<gp_Vec2d>& DPoles2d,
                               NCollection_Array1<double>&   Weigths,
                               NCollection_Array1<double>&   DWeigths)
{
  bool               Ok   = true;
  Blend_AppFunction* Func = (Blend_AppFunction*)myFunc;

  Ok = SearchPoint(*Func, Param, myPnt);

  if (Ok)
  {
    Ok = (*Func).Section(myPnt, Poles, DPoles, Poles2d, DPoles2d, Weigths, DWeigths);
  }

  return Ok;
}

//===========================================================================
// Function: D2
// Purpose : Calculation of the derivative and second partial of the
//           section corresponding to v.
//           For v = Param, if the calculation fails false is raised.
//===========================================================================
bool BRepBlend_AppFuncRoot::D2(const double Param,
                               const double /*First*/,
                               const double /*Last*/,
                               NCollection_Array1<gp_Pnt>&   Poles,
                               NCollection_Array1<gp_Vec>&   DPoles,
                               NCollection_Array1<gp_Vec>&   D2Poles,
                               NCollection_Array1<gp_Pnt2d>& Poles2d,
                               NCollection_Array1<gp_Vec2d>& DPoles2d,
                               NCollection_Array1<gp_Vec2d>& D2Poles2d,
                               NCollection_Array1<double>&   Weigths,
                               NCollection_Array1<double>&   DWeigths,
                               NCollection_Array1<double>&   D2Weigths)
{
  bool               Ok   = true;
  Blend_AppFunction* Func = (Blend_AppFunction*)myFunc;

  Ok = SearchPoint(*Func, Param, myPnt);
  if (Ok)
  {
    Ok = (*Func).Section(myPnt,
                         Poles,
                         DPoles,
                         D2Poles,
                         Poles2d,
                         DPoles2d,
                         D2Poles2d,
                         Weigths,
                         DWeigths,
                         D2Weigths);
  }
  return Ok;
}

int BRepBlend_AppFuncRoot::Nb2dCurves() const
{
  Blend_AppFunction* Func = (Blend_AppFunction*)myFunc;
  int                i, j, k, nbpol2d;
  (*Func).GetShape(i, j, k, nbpol2d);
  return nbpol2d;
}

void BRepBlend_AppFuncRoot::SectionShape(int& NbPoles, int& NbKnots, int& Degree) const
{
  Blend_AppFunction* Func = (Blend_AppFunction*)myFunc;
  int                ii;
  (*Func).GetShape(NbPoles, NbKnots, Degree, ii);
}

void BRepBlend_AppFuncRoot::Knots(NCollection_Array1<double>& TKnots) const
{
  Blend_AppFunction* Func = (Blend_AppFunction*)myFunc;
  Func->Knots(TKnots);
}

void BRepBlend_AppFuncRoot::Mults(NCollection_Array1<int>& TMults) const
{
  Blend_AppFunction* Func = (Blend_AppFunction*)myFunc;
  Func->Mults(TMults);
}

bool BRepBlend_AppFuncRoot::IsRational() const
{
  Blend_AppFunction* Func = (Blend_AppFunction*)myFunc;
  return (*Func).IsRational();
}

int BRepBlend_AppFuncRoot::NbIntervals(const GeomAbs_Shape S) const
{
  Blend_AppFunction* Func = (Blend_AppFunction*)myFunc;
  return Func->NbIntervals(S);
}

void BRepBlend_AppFuncRoot::Intervals(NCollection_Array1<double>& T, const GeomAbs_Shape S) const
{
  Blend_AppFunction* Func = (Blend_AppFunction*)myFunc;
  Func->Intervals(T, S);
}

void BRepBlend_AppFuncRoot::SetInterval(const double First, const double Last)
{
  Blend_AppFunction* Func = (Blend_AppFunction*)myFunc;
  Func->Set(First, Last);
}

void BRepBlend_AppFuncRoot::Resolution(const int    Index,
                                       const double Tol,
                                       double&      TolU,
                                       double&      TolV) const
{
  Blend_AppFunction* Func = (Blend_AppFunction*)myFunc;
  Func->Resolution(Index, Tol, TolU, TolV);
}

void BRepBlend_AppFuncRoot::GetTolerance(const double                BoundTol,
                                         const double                SurfTol,
                                         const double                AngleTol,
                                         NCollection_Array1<double>& Tol3d) const
{
  int                ii;
  math_Vector        V3d(1, Tol3d.Length()), V1d(1, Tol3d.Length());
  Blend_AppFunction* Func = (Blend_AppFunction*)myFunc;

  Func->GetTolerance(BoundTol, SurfTol, AngleTol, V3d, V1d);
  for (ii = 1; ii <= Tol3d.Length(); ii++)
    Tol3d(ii) = V3d(ii);
}

void BRepBlend_AppFuncRoot::SetTolerance(const double Tol3d, const double Tol2d)
{
  Blend_AppFunction* Func = (Blend_AppFunction*)myFunc;
  int                ii, dim = Func->NbVariables();
  Func->GetTolerance(myTolerance, Tol3d);
  for (ii = 1; ii <= dim; ii++)
  {
    if (myTolerance(ii) > Tol2d)
    {
      myTolerance(ii) = Tol2d;
    }
  }
}

gp_Pnt BRepBlend_AppFuncRoot::BarycentreOfSurf() const
{
  return myBary;
}

double BRepBlend_AppFuncRoot::MaximalSection() const
{
  Blend_AppFunction* Func = (Blend_AppFunction*)myFunc;
  return Func->GetSectionSize();
}

void BRepBlend_AppFuncRoot::GetMinimalWeight(NCollection_Array1<double>& Weigths) const
{
  Blend_AppFunction* Func = (Blend_AppFunction*)myFunc;
  Func->GetMinimalWeight(Weigths);
}

//================================================================================
//
// Function : SearchPoint
//
// Purpose : Find point solution with parameter Param (on 2 Surfaces)
//
// Algorithm :
//     1) Approximative solution is found from already calculated Points
//     2) Convergence is done by a method of type Newton
//
// Possible causes of fails :
//        - Singularity on surfaces.
//        - no information oin the "line" resulting from processing.
//
//================================================================================

bool BRepBlend_AppFuncRoot::SearchPoint(Blend_AppFunction& Func,
                                        const double       Param,
                                        Blend_Point&       Pnt)
{
  bool Trouve;
  int  dim = Func.NbVariables();
  // (1) Find a point of init
  int    I1 = 1, I2 = myLine->NbPoints(), Index;
  double t1, t2;

  //  (1.a) It is checked if it is inside
  if (Param < myLine->Point(I1).Parameter())
  {
    return false;
  }
  if (Param > myLine->Point(I2).Parameter())
  {
    return false;
  }

  //  (1.b) Find the interval
  Trouve = SearchLocation(Param, I1, I2, Index);

  //  (1.c) If the point is already calculated it is returned
  if (Trouve)
  {
    Pnt = myLine->Point(Index);
    Vec(XInit, Pnt);
  }
  else
  {
    //  (1.d) Initialisation by linear interpolation
    Pnt = myLine->Point(Index);
    Vec(X1, Pnt);
    t1 = Pnt.Parameter();

    Pnt = myLine->Point(Index + 1);
    Vec(X2, Pnt);
    t2 = Pnt.Parameter();

    double Parammt1 = (Param - t1) / (t2 - t1);
    double t2mParam = (t2 - Param) / (t2 - t1);
    for (int i = 1; i <= dim; i++)
    {
      XInit(i) = X2(i) * Parammt1 + X1(i) * t2mParam;
    }
  }

  // (2) Calculation of the solution ------------------------
  Func.Set(Param);
  Func.GetBounds(X1, X2);
  mySolver->Perform(Func, XInit, X1, X2);

  if (!mySolver->IsDone())
  {
#ifdef BREPBLEND_DEB
    std::cout << "AppFunc : RNLD Not done en t = " << Param << std::endl;
#endif
    return false;
  }
  mySolver->Root(Sol);

  // (3) Storage of the point
  Point(Func, Param, Sol, Pnt);

  // (4) Insertion of the point if the calculation seems long.
  if ((!Trouve) && (mySolver->NbIterations() > 3))
  {
#ifdef OCCT_DEBUG
    std::cout << "Evaluation in t = " << Param << "given" << std::endl;
    mySolver->Dump(std::cout);
#endif
    myLine->InsertBefore(Index + 1, Pnt);
  }
  return true;
}

//=============================================================================
//
// Function : SearchLocation
//
// Purpose : Binary search of the line of the parametric interval containing
//           Param in the list of calculated points (myline)
//           if the point of parameter Param is already stored in the list
//           True is raised and ParamIndex corresponds to line of Point.
//           Complexity of this algorithm is log(n)/log(2)
//================================================================================
bool BRepBlend_AppFuncRoot::SearchLocation(const double Param,
                                           const int    FirstIndex,
                                           const int    LastIndex,
                                           int&         ParamIndex) const
{
  int    Ideb = FirstIndex, Ifin = LastIndex, Idemi;
  double Valeur;

  Valeur = myLine->Point(Ideb).Parameter();
  if (Param == Valeur)
  {
    ParamIndex = Ideb;
    return true;
  }

  Valeur = myLine->Point(Ifin).Parameter();
  if (Param == Valeur)
  {
    ParamIndex = Ifin;
    return true;
  }

  while (Ideb + 1 != Ifin)
  {
    Idemi  = (Ideb + Ifin) / 2;
    Valeur = myLine->Point(Idemi).Parameter();
    if (Valeur < Param)
    {
      Ideb = Idemi;
    }
    else
    {
      if (Valeur > Param)
      {
        Ifin = Idemi;
      }
      else
      {
        ParamIndex = Idemi;
        return true;
      }
    }
  }

  ParamIndex = Ideb;
  return false;
}
