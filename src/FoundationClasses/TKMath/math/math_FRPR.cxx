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

// #ifndef OCCT_DEBUG
#define No_Standard_RangeError
#define No_Standard_OutOfRange
#define No_Standard_DimensionError

// #endif

#include <math_BracketMinimum.hxx>
#include <math_BrentMinimum.hxx>
#include <math_FRPR.hxx>
#include <math_Function.hxx>
#include <math_MultipleVarFunctionWithGradient.hxx>

// l'utilisation de math_BrentMinumim pur trouver un minimum dans une direction
// donnee n'est pas du tout optimale. voir peut etre interpolation cubique
// classique et aussi essayer "recherche unidimensionnelle economique"
// PROGRAMMATION MATHEMATIQUE (theorie et algorithmes) tome1 page 82.
class DirFunctionTer : public math_Function
{

  math_Vector*              P0;
  math_Vector*              Dir;
  math_Vector*              P;
  math_MultipleVarFunction* F;

public:
  DirFunctionTer(math_Vector& V1, math_Vector& V2, math_Vector& V3, math_MultipleVarFunction& f);

  void Initialize(const math_Vector& p0, const math_Vector& dir);

  bool Value(const double x, double& fval) override;
};

DirFunctionTer::DirFunctionTer(math_Vector&              V1,
                               math_Vector&              V2,
                               math_Vector&              V3,
                               math_MultipleVarFunction& f)
{

  P0  = &V1;
  Dir = &V2;
  P   = &V3;
  F   = &f;
}

void DirFunctionTer::Initialize(const math_Vector& p0, const math_Vector& dir)
{

  *P0  = p0;
  *Dir = dir;
}

bool DirFunctionTer::Value(const double x, double& fval)
{

  *P = *Dir;
  P->Multiply(x);
  P->Add(*P0);
  fval = 0.;
  return F->Value(*P, fval);
}

static bool MinimizeDirection(math_Vector& P, math_Vector& Dir, double& Result, DirFunctionTer& F)
{

  double ax, xx, bx;

  F.Initialize(P, Dir);
  math_BracketMinimum Bracket(F, 0.0, 1.0);
  if (Bracket.IsDone())
  {
    Bracket.Values(ax, xx, bx);
    math_BrentMinimum Sol(1.e-10);
    Sol.Perform(F, ax, xx, bx);
    if (Sol.IsDone())
    {
      double Scale = Sol.Location();
      Result       = Sol.Minimum();
      Dir.Multiply(Scale);
      P.Add(Dir);
      return true;
    }
  }
  return false;
}

//=================================================================================================

math_FRPR::math_FRPR(const math_MultipleVarFunctionWithGradient& theFunction,
                     const double                                theTolerance,
                     const int                                   theNbIterations,
                     const double                                theZEPS)

    : TheLocation(1, theFunction.NbVariables()),
      TheGradient(1, theFunction.NbVariables()),
      TheMinimum(0.0),
      PreviousMinimum(0.0),
      XTol(theTolerance),
      EPSZ(theZEPS),
      Done(false),
      Iter(0),
      State(0),
      TheStatus(math_NotBracketed),
      Itermax(theNbIterations)
{
}

//=================================================================================================

math_FRPR::~math_FRPR() = default;

//=================================================================================================

void math_FRPR::Perform(math_MultipleVarFunctionWithGradient& F, const math_Vector& StartingPoint)
{
  bool   Good;
  int    n = TheLocation.Length();
  int    j, its;
  double gg, gam, dgg;

  math_Vector g(1, n), h(1, n);

  math_Vector    Temp1(1, n);
  math_Vector    Temp2(1, n);
  math_Vector    Temp3(1, n);
  DirFunctionTer F_Dir(Temp1, Temp2, Temp3, F);

  TheLocation = StartingPoint;
  Good        = F.Values(TheLocation, PreviousMinimum, TheGradient);
  if (!Good)
  {
    Done      = false;
    TheStatus = math_FunctionError;
    return;
  }

  g           = -TheGradient;
  h           = g;
  TheGradient = g;

  for (its = 1; its <= Itermax; its++)
  {
    Iter = its;

    bool IsGood = MinimizeDirection(TheLocation, TheGradient, TheMinimum, F_Dir);
    if (IsSolutionReached(F))
    {
      Done      = true;
      State     = F.GetStateNumber();
      TheStatus = math_OK;
      return;
    }
    if (!IsGood)
    {
      Done      = false;
      TheStatus = math_DirectionSearchError;
      return;
    }
    Good = F.Values(TheLocation, PreviousMinimum, TheGradient);
    if (!Good)
    {
      Done      = false;
      TheStatus = math_FunctionError;
      return;
    }

    dgg = 0.0;
    gg  = 0.0;

    for (j = 1; j <= n; j++)
    {
      gg += g(j) * g(j);
      //	   dgg += TheGradient(j)*TheGradient(j);  //for Fletcher-Reeves
      dgg += (TheGradient(j) + g(j)) * TheGradient(j); // for Polak-Ribiere
    }

    if (gg == 0.0)
    {
      // Unlikely. If gradient is exactly 0 then we are already done.
      Done      = false;
      TheStatus = math_FunctionError;
      return;
    }

    gam         = dgg / gg;
    g           = -TheGradient;
    TheGradient = g + gam * h;
    h           = TheGradient;
  }
  Done      = false;
  TheStatus = math_TooManyIterations;
  return;
}

//=================================================================================================

void math_FRPR::Dump(Standard_OStream& o) const
{
  o << "math_FRPR ";
  if (Done)
  {
    o << " Status = Done \n";
    o << " Location Vector = " << TheLocation << "\n";
    o << " Minimum value = " << TheMinimum << "\n";
    o << " Number of iterations = " << Iter << "\n";
  }
  else
  {
    o << " Status = not Done because " << (int)TheStatus << "\n";
  }
}
