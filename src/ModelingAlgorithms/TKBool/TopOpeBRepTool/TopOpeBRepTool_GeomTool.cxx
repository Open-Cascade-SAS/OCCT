// Created on: 1993-06-24
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
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

#include <Precision.hxx>
#include <TopOpeBRepTool_GeomTool.hxx>

//=================================================================================================

TopOpeBRepTool_GeomTool::TopOpeBRepTool_GeomTool(const TopOpeBRepTool_OutCurveType TypeC3D,
                                                 const Standard_Boolean            CompC3D,
                                                 const Standard_Boolean            CompPC1,
                                                 const Standard_Boolean            CompPC2)
    : myTypeC3D(TypeC3D),
      myCompC3D(CompC3D),
      myCompPC1(CompPC1),
      myCompPC2(CompPC2),
      myTol3d(Precision::Approximation()),
      myTol2d(Precision::PApproximation()),
      myNbPntMax(30)
{
}

//=================================================================================================

void TopOpeBRepTool_GeomTool::Define(const TopOpeBRepTool_OutCurveType TypeC3D,
                                     const Standard_Boolean            CompC3D,
                                     const Standard_Boolean            CompPC1,
                                     const Standard_Boolean            CompPC2)
{
  myTypeC3D = TypeC3D;
  myCompC3D = CompC3D;
  myCompPC1 = CompPC1;
  myCompPC2 = CompPC2;
}

//=================================================================================================

void TopOpeBRepTool_GeomTool::Define(const TopOpeBRepTool_OutCurveType TypeC3D)
{
  myTypeC3D = TypeC3D;
}

//=================================================================================================

void TopOpeBRepTool_GeomTool::DefineCurves(const Standard_Boolean CompC3D)
{
  myCompC3D = CompC3D;
}

//=================================================================================================

void TopOpeBRepTool_GeomTool::DefinePCurves1(const Standard_Boolean CompPC1)
{
  myCompPC1 = CompPC1;
}

//=================================================================================================

void TopOpeBRepTool_GeomTool::DefinePCurves2(const Standard_Boolean CompPC2)
{
  myCompPC2 = CompPC2;
}

//=================================================================================================

void TopOpeBRepTool_GeomTool::Define(const TopOpeBRepTool_GeomTool& GT)
{
  *this = GT;
}

//=================================================================================================

void TopOpeBRepTool_GeomTool::GetTolerances(Standard_Real& tol3d, Standard_Real& tol2d) const
{
  tol3d = myTol3d;
  tol2d = myTol2d;
}

//=================================================================================================

void TopOpeBRepTool_GeomTool::SetTolerances(const Standard_Real tol3d, const Standard_Real tol2d)
{
  myTol3d = tol3d;
  myTol2d = tol2d;
}

//=================================================================================================

Standard_Integer TopOpeBRepTool_GeomTool::NbPntMax() const
{
  return myNbPntMax;
}

//=================================================================================================

void TopOpeBRepTool_GeomTool::SetNbPntMax(const Standard_Integer NbPntMax)
{
  myNbPntMax = NbPntMax;
}

//=================================================================================================

Standard_Boolean TopOpeBRepTool_GeomTool::CompC3D() const
{
  return myCompC3D;
}

//=================================================================================================

TopOpeBRepTool_OutCurveType TopOpeBRepTool_GeomTool::TypeC3D() const
{
  return myTypeC3D;
}

//=================================================================================================

Standard_Boolean TopOpeBRepTool_GeomTool::CompPC1() const
{
  return myCompPC1;
}

//=================================================================================================

Standard_Boolean TopOpeBRepTool_GeomTool::CompPC2() const
{
  return myCompPC2;
}
