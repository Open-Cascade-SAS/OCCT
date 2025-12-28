// Created on: 1994-09-05
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

#include <Geom2d_Curve.hxx>
#include <Geom2dLProp_Curve2dTool.hxx>
#include <Geom2dLProp_FuncCurExt.hxx>
#include <Geom2dLProp_FuncCurNul.hxx>
#include <Geom2dLProp_NumericCurInf2d.hxx>
#include <LProp_CurAndInf.hxx>
#include <math_BracketedRoot.hxx>
#include <math_FunctionRoots.hxx>
#include <Precision.hxx>

//=================================================================================================

Geom2dLProp_NumericCurInf2d::Geom2dLProp_NumericCurInf2d()
    : isDone(false)
{
}

//=================================================================================================

void Geom2dLProp_NumericCurInf2d::PerformCurExt(const occ::handle<Geom2d_Curve>& C,
                                                LProp_CurAndInf&                 Result)
{
  PerformCurExt(C,
                Geom2dLProp_Curve2dTool::FirstParameter(C),
                Geom2dLProp_Curve2dTool::LastParameter(C),
                Result);
}

//=================================================================================================

void Geom2dLProp_NumericCurInf2d::PerformCurExt(const occ::handle<Geom2d_Curve>& C,
                                                const double                     UMin,
                                                const double                     UMax,
                                                LProp_CurAndInf&                 Result)
{
  isDone = true;

  double           EpsH = 1.e-4 * (UMax - UMin);
  constexpr double Tol  = Precision::PConfusion();

  // la premiere recherce se fait avec une tolerance assez grande
  // car la derivee de la fonction est estimee assez grossierement.

  Geom2dLProp_FuncCurExt F(C, EpsH);
  int                    NbSamples = 100;
  bool                   SolType;

  math_FunctionRoots SolRoot(F, UMin, UMax, NbSamples, EpsH, EpsH, EpsH);

  if (SolRoot.IsDone())
  {
    for (int j = 1; j <= SolRoot.NbSolutions(); j++)
    {
      double Param = SolRoot.Value(j);
      // la solution est affinee.
      math_BracketedRoot BS(F, Param - EpsH, Param + EpsH, Tol);
      if (BS.IsDone())
      {
        Param = BS.Root();
      }
      SolType = F.IsMinKC(Param);
      Result.AddExtCur(Param, SolType);
    }
  }
  else
  {
    isDone = false;
  }
}

//=================================================================================================

void Geom2dLProp_NumericCurInf2d::PerformInf(const occ::handle<Geom2d_Curve>& C,
                                             LProp_CurAndInf&                 Result)
{
  PerformInf(C,
             Geom2dLProp_Curve2dTool::FirstParameter(C),
             Geom2dLProp_Curve2dTool::LastParameter(C),
             Result);
}

//=================================================================================================

void Geom2dLProp_NumericCurInf2d::PerformInf(const occ::handle<Geom2d_Curve>& C,
                                             const double                     UMin,
                                             const double                     UMax,
                                             LProp_CurAndInf&                 Result)
{
  isDone = true;
  Geom2dLProp_FuncCurNul F(C);
  double                 EpsX      = 1.e-6;
  double                 EpsF      = 1.e-6;
  int                    NbSamples = 30;

  math_FunctionRoots SolRoot(F, UMin, UMax, NbSamples, EpsX, EpsF, EpsX);

  if (SolRoot.IsDone())
  {
    for (int j = 1; j <= SolRoot.NbSolutions(); j++)
    {
      Result.AddInflection(SolRoot.Value(j));
    }
  }
  else
  {
    isDone = false;
  }
}

//=================================================================================================

bool Geom2dLProp_NumericCurInf2d::IsDone() const
{
  return isDone;
}
