// Created on: 1994-01-10
// Created by: Yves FRICAUD
// Copyright (c) 1994-1999 Matra Datavision
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

#include <Bisector_PointOnBis.hxx>
#include <gp_Pnt2d.hxx>

//=================================================================================================

Bisector_PointOnBis::Bisector_PointOnBis()
    : param1(0.0),
      param2(0.0),
      paramBis(0.0),
      distance(0.0),
      infinite(false)
{
}

//=================================================================================================

Bisector_PointOnBis::Bisector_PointOnBis(const double Param1,
                                         const double Param2,
                                         const double ParamBis,
                                         const double Distance,
                                         const gp_Pnt2d&     P)
    : param1(Param1),
      param2(Param2),
      paramBis(ParamBis),
      distance(Distance),
      point(P)
{
  infinite = false;
}

//=================================================================================================

void Bisector_PointOnBis::ParamOnC1(const double Param)
{
  param1 = Param;
}

//=================================================================================================

void Bisector_PointOnBis::ParamOnC2(const double Param)
{
  param2 = Param;
}

//=================================================================================================

void Bisector_PointOnBis::ParamOnBis(const double Param)
{
  paramBis = Param;
}

//=================================================================================================

void Bisector_PointOnBis::Distance(const double Distance)
{
  distance = Distance;
}

//=================================================================================================

void Bisector_PointOnBis::Point(const gp_Pnt2d& P)
{
  point = P;
}

//=================================================================================================

void Bisector_PointOnBis::IsInfinite(const bool Infinite)
{
  infinite = Infinite;
}

//=================================================================================================

double Bisector_PointOnBis::ParamOnC1() const
{
  return param1;
}

//=================================================================================================

double Bisector_PointOnBis::ParamOnC2() const
{
  return param2;
}

//=================================================================================================

double Bisector_PointOnBis::ParamOnBis() const
{
  return paramBis;
}

//=================================================================================================

double Bisector_PointOnBis::Distance() const
{
  return distance;
}

//=================================================================================================

gp_Pnt2d Bisector_PointOnBis::Point() const
{
  return point;
}

//=================================================================================================

bool Bisector_PointOnBis::IsInfinite() const
{
  return infinite;
}

//=================================================================================================

void Bisector_PointOnBis::Dump() const
{
  std::cout << "Param1    :" << param1 << std::endl;
  std::cout << "Param2    :" << param2 << std::endl;
  std::cout << "Param Bis :" << paramBis << std::endl;
  std::cout << "Distance  :" << distance << std::endl;
}
