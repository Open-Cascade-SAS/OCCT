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

#include <math_BFGS.hxx>
#include <math_BracketMinimum.hxx>
#include <math_BrentMinimum.hxx>
#include <math_FunctionWithDerivative.hxx>
#include <math_Matrix.hxx>
#include <math_MultipleVarFunctionWithGradient.hxx>
#include <Precision.hxx>

// l'utilisation de math_BrentMinumim pur trouver un minimum dans une direction
// donnee n'est pas du tout optimale. voir peut etre interpolation cubique
// classique et aussi essayer "recherche unidimensionnelle economique"
// PROGRAMMATION MATHEMATIQUE (theorie et algorithmes) tome1 page 82.

// Target function for 1D problem, point and direction are known.
class DirFunction : public math_FunctionWithDerivative
{

  math_Vector*                          P0;
  math_Vector*                          Dir;
  math_Vector*                          P;
  math_Vector*                          G;
  math_MultipleVarFunctionWithGradient* F;

public:
  //! Ctor.
  DirFunction(math_Vector&                          V1,
              math_Vector&                          V2,
              math_Vector&                          V3,
              math_Vector&                          V4,
              math_MultipleVarFunctionWithGradient& f)
      : P0(&V1),
        Dir(&V2),
        P(&V3),
        G(&V4),
        F(&f)
  {
  }

  //! Sets point and direction.
  void Initialize(const math_Vector& p0, const math_Vector& dir) const
  {
    *P0  = p0;
    *Dir = dir;
  }

  void TheGradient(math_Vector& Grad) { Grad = *G; }

  virtual bool Value(const double x, double& fval)
  {
    *P = *Dir;
    P->Multiply(x);
    P->Add(*P0);
    fval = 0.;
    return F->Value(*P, fval);
  }

  virtual bool Values(const double x, double& fval, double& D)
  {
    *P = *Dir;
    P->Multiply(x);
    P->Add(*P0);
    fval = D = 0.;
    if (F->Values(*P, fval, *G))
    {
      D = (*G).Multiplied(*Dir);
      return true;
    }

    return false;
  }

  virtual bool Derivative(const double x, double& D)
  {
    *P = *Dir;
    P->Multiply(x);
    P->Add(*P0);
    double fval;
    D = 0.;
    if (F->Values(*P, fval, *G))
    {
      D = (*G).Multiplied(*Dir);
      return true;
    }

    return false;
  }
};

//=============================================================================
// function : ComputeInitScale
// purpose  : Compute the appropriate initial value of scale factor to apply
//           to the direction to approach to the minimum of the function
//=============================================================================
static bool ComputeInitScale(const double       theF0,
                             const math_Vector& theDir,
                             const math_Vector& theGr,
                             double&            theScale)
{
  const double dy1 = theGr * theDir;
  if (std::abs(dy1) < RealSmall())
    return false;

  const double aHnr1 = theDir.Norm2();
  const double alfa  = 0.7 * (-theF0) / dy1;
  theScale           = 0.015 / std::sqrt(aHnr1);
  if (theScale > alfa)
    theScale = alfa;

  return true;
}

//=============================================================================
// function : ComputeMinMaxScale
// purpose  : For a given point and direction, and bounding box,
//           find min and max scale factors with which the point reaches borders
//           if we apply translation Point+Dir*Scale.
// return   : True if found, False if point is out of bounds.
//=============================================================================
static bool ComputeMinMaxScale(const math_Vector& thePoint,
                               const math_Vector& theDir,
                               const math_Vector& theLeft,
                               const math_Vector& theRight,
                               double&            theMinScale,
                               double&            theMaxScale)
{
  for (int anIdx = 1; anIdx <= theLeft.Upper(); anIdx++)
  {
    const double aLeft  = theLeft(anIdx) - thePoint(anIdx);
    const double aRight = theRight(anIdx) - thePoint(anIdx);
    if (std::abs(theDir(anIdx)) > RealSmall())
    {
      // Use PConfusion to get off a little from the bounds to prevent
      // possible refuse in Value function.
      const double aLScale = (aLeft + Precision::PConfusion()) / theDir(anIdx);
      const double aRScale = (aRight - Precision::PConfusion()) / theDir(anIdx);
      if (std::abs(aLeft) < Precision::PConfusion())
      {
        // Point is on the left border.
        theMaxScale = std::min(theMaxScale, std::max(0., aRScale));
        theMinScale = std::max(theMinScale, std::min(0., aRScale));
      }
      else if (std::abs(aRight) < Precision::PConfusion())
      {
        // Point is on the right border.
        theMaxScale = std::min(theMaxScale, std::max(0., aLScale));
        theMinScale = std::max(theMinScale, std::min(0., aLScale));
      }
      else if (aLeft * aRight < 0)
      {
        // Point is inside allowed range.
        theMaxScale = std::min(theMaxScale, std::max(aLScale, aRScale));
        theMinScale = std::max(theMinScale, std::min(aLScale, aRScale));
      }
      else
        // point is out of bounds
        return false;
    }
    else
    {
      // Direction is parallel to the border.
      // Check that the point is not out of bounds
      if (aLeft > Precision::PConfusion() || aRight < -Precision::PConfusion())
      {
        return false;
      }
    }
  }
  return true;
}

//=============================================================================
// function : MinimizeDirection
// purpose  : Solves 1D minimization problem when point and directions
//           are known.
//=============================================================================
static bool MinimizeDirection(math_Vector&       P,
                              double             F0,
                              math_Vector&       Gr,
                              math_Vector&       Dir,
                              double&            Result,
                              DirFunction&       F,
                              bool               isBounds,
                              const math_Vector& theLeft,
                              const math_Vector& theRight)
{
  double lambda;
  if (!ComputeInitScale(F0, Dir, Gr, lambda))
    return false;

  // by default the scaling range is unlimited
  double aMinLambda = -Precision::Infinite();
  double aMaxLambda = Precision::Infinite();
  if (isBounds)
  {
    // limit the scaling range taking into account the bounds
    if (!ComputeMinMaxScale(P, Dir, theLeft, theRight, aMinLambda, aMaxLambda))
      return false;

    if (aMinLambda > -Precision::PConfusion() && aMaxLambda < Precision::PConfusion())
    {
      // Point is on the border and the direction shows outside.
      // Make direction to go along the border
      for (int anIdx = 1; anIdx <= theLeft.Upper(); anIdx++)
      {
        if ((std::abs(P(anIdx) - theRight(anIdx)) < Precision::PConfusion() && Dir(anIdx) > 0.0)
            || (std::abs(P(anIdx) - theLeft(anIdx)) < Precision::PConfusion() && Dir(anIdx) < 0.0))
        {
          Dir(anIdx) = 0.0;
        }
      }

      // re-compute scale values with new direction
      if (!ComputeInitScale(F0, Dir, Gr, lambda))
        return false;
      if (!ComputeMinMaxScale(P, Dir, theLeft, theRight, aMinLambda, aMaxLambda))
        return false;
    }
    lambda = std::min(lambda, aMaxLambda);
    lambda = std::max(lambda, aMinLambda);
  }

  F.Initialize(P, Dir);
  double F1;
  if (!F.Value(lambda, F1))
    return false;

  math_BracketMinimum Bracket(0.0, lambda);
  if (isBounds)
    Bracket.SetLimits(aMinLambda, aMaxLambda);
  Bracket.SetFA(F0);
  Bracket.SetFB(F1);
  Bracket.Perform(F);
  if (Bracket.IsDone())
  {
    // find minimum inside the bracket
    double ax, xx, bx, Fax, Fxx, Fbx;
    Bracket.Values(ax, xx, bx);
    Bracket.FunctionValues(Fax, Fxx, Fbx);

    int               niter = 100;
    double            tol   = 1.e-03;
    math_BrentMinimum Sol(tol, Fxx, niter, 1.e-08);
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
  else if (isBounds)
  {
    // Bracket definition is failure. If the bounds are defined then
    // set current point to intersection with bounds
    double aFMin, aFMax;
    if (!F.Value(aMinLambda, aFMin))
      return false;
    if (!F.Value(aMaxLambda, aFMax))
      return false;
    double aBestLambda;
    if (aFMin < aFMax)
    {
      aBestLambda = aMinLambda;
      Result      = aFMin;
    }
    else
    {
      aBestLambda = aMaxLambda;
      Result      = aFMax;
    }
    Dir.Multiply(aBestLambda);
    P.Add(Dir);
    return true;
  }
  return false;
}

//=============================================================================
// function : Perform
// purpose  : Performs minimization problem using BFGS method.
//=============================================================================
void math_BFGS::Perform(math_MultipleVarFunctionWithGradient& F, const math_Vector& StartingPoint)
{
  const int n    = TheLocation.Length();
  bool      Good = true;
  int       j, i;
  double    fae, fad, fac;

  math_Vector xi(1, n), dg(1, n), hdg(1, n);
  math_Matrix hessin(1, n, 1, n);
  hessin.Init(0.0);

  math_Vector Temp1(1, n);
  math_Vector Temp2(1, n);
  math_Vector Temp3(1, n);
  math_Vector Temp4(1, n);
  DirFunction F_Dir(Temp1, Temp2, Temp3, Temp4, F);

  TheLocation = StartingPoint;
  Good        = F.Values(TheLocation, PreviousMinimum, TheGradient);
  if (!Good)
  {
    Done      = false;
    TheStatus = math_FunctionError;
    return;
  }
  for (i = 1; i <= n; i++)
  {
    hessin(i, i) = 1.0;
    xi(i)        = -TheGradient(i);
  }

  for (nbiter = 1; nbiter <= Itermax; nbiter++)
  {
    TheMinimum        = PreviousMinimum;
    const bool IsGood = MinimizeDirection(TheLocation,
                                          TheMinimum,
                                          TheGradient,
                                          xi,
                                          TheMinimum,
                                          F_Dir,
                                          myIsBoundsDefined,
                                          myLeft,
                                          myRight);

    if (IsSolutionReached(F))
    {
      Done      = true;
      TheStatus = math_OK;
      return;
    }

    if (!IsGood)
    {
      Done      = false;
      TheStatus = math_DirectionSearchError;
      return;
    }
    PreviousMinimum = TheMinimum;

    dg = TheGradient;

    Good = F.Values(TheLocation, TheMinimum, TheGradient);
    if (!Good)
    {
      Done      = false;
      TheStatus = math_FunctionError;
      return;
    }

    for (i = 1; i <= n; i++)
      dg(i) = TheGradient(i) - dg(i);

    for (i = 1; i <= n; i++)
    {
      hdg(i) = 0.0;
      for (j = 1; j <= n; j++)
        hdg(i) += hessin(i, j) * dg(j);
    }

    fac = fae = 0.0;
    for (i = 1; i <= n; i++)
    {
      fac += dg(i) * xi(i);
      fae += dg(i) * hdg(i);
    }
    fac = 1.0 / fac;
    fad = 1.0 / fae;

    for (i = 1; i <= n; i++)
      dg(i) = fac * xi(i) - fad * hdg(i);

    for (i = 1; i <= n; i++)
    {
      for (j = 1; j <= n; j++)
        hessin(i, j) += fac * xi(i) * xi(j) - fad * hdg(i) * hdg(j) + fae * dg(i) * dg(j);
    }

    for (i = 1; i <= n; i++)
    {
      xi(i) = 0.0;
      for (j = 1; j <= n; j++)
        xi(i) -= hessin(i, j) * TheGradient(j);
    }
  }
  Done      = false;
  TheStatus = math_TooManyIterations;
  return;
}

//=============================================================================
// function : IsSolutionReached
// purpose  : Checks whether solution reached or not.
//=============================================================================
bool math_BFGS::IsSolutionReached(math_MultipleVarFunctionWithGradient&) const
{

  return 2.0 * fabs(TheMinimum - PreviousMinimum)
         <= XTol * (fabs(TheMinimum) + fabs(PreviousMinimum) + EPSZ);
}

//=================================================================================================

math_BFGS::math_BFGS(const int    NbVariables,
                     const double Tolerance,
                     const int    NbIterations,
                     const double ZEPS)
    : TheStatus(math_OK),
      TheLocation(1, NbVariables),
      TheGradient(1, NbVariables),
      PreviousMinimum(0.),
      TheMinimum(0.),
      XTol(Tolerance),
      EPSZ(ZEPS),
      nbiter(0),
      myIsBoundsDefined(false),
      myLeft(1, NbVariables, 0.0),
      myRight(1, NbVariables, 0.0),
      Done(false),
      Itermax(NbIterations)
{
}

//=================================================================================================

math_BFGS::~math_BFGS() {}

//=================================================================================================

void math_BFGS::Dump(Standard_OStream& o) const
{

  o << "math_BFGS resolution: ";
  if (Done)
  {
    o << " Status = Done \n";
    o << " Location Vector = " << Location() << "\n";
    o << " Minimum value = " << Minimum() << "\n";
    o << " Number of iterations = " << NbIterations() << "\n";
  }
  else
    o << " Status = not Done because " << (int)TheStatus << "\n";
}

//=============================================================================
// function : SetBoundary
// purpose  : Set boundaries for conditional optimization
//=============================================================================
void math_BFGS::SetBoundary(const math_Vector& theLeftBorder, const math_Vector& theRightBorder)
{
  myLeft            = theLeftBorder;
  myRight           = theRightBorder;
  myIsBoundsDefined = true;
}
