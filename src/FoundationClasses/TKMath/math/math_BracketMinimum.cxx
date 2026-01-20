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

#include <math_BracketMinimum.hxx>
#include <math_Function.hxx>
#include <StdFail_NotDone.hxx>

// waiting for NotDone Exception
#define GOLD 1.618034
#define CGOLD 0.3819660
#define GLIMIT 100.0
#define TINY 1.0e-20
#ifdef MAX
  #undef MAX
#endif
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define SIGN(a, b) ((b) > 0.0 ? fabs(a) : -fabs(a))
#define SHFT(a, b, c, d)                                                                           \
  (a) = (b);                                                                                       \
  (b) = (c);                                                                                       \
  (c) = (d)

bool math_BracketMinimum::LimitAndMayBeSwap(math_Function&      F,
                                                        const double theA,
                                                        double&      theB,
                                                        double&      theFB,
                                                        double&      theC,
                                                        double&      theFC) const
{
  theC = Limited(theC);
  if (std::abs(theB - theC) < Precision::PConfusion())
    return false;
  bool OK = F.Value(theC, theFC);
  if (!OK)
    return false;
  // check that B is between A and C
  if ((theA - theB) * (theB - theC) < 0)
  {
    // swap B and C
    double dum;
    SHFT(dum, theB, theC, dum);
    SHFT(dum, theFB, theFC, dum);
  }
  return true;
}

void math_BracketMinimum::Perform(math_Function& F)
{

  bool OK;
  double    ulim, u, r, q, fu, dum;

  Done                 = false;
  double Lambda = GOLD;
  if (!myFA)
  {
    OK = F.Value(Ax, FAx);
    if (!OK)
      return;
  }
  if (!myFB)
  {
    OK = F.Value(Bx, FBx);
    if (!OK)
      return;
  }
  if (FBx > FAx)
  {
    SHFT(dum, Ax, Bx, dum);
    SHFT(dum, FBx, FAx, dum);
  }

  // get next prob after (A, B)
  Cx = Bx + Lambda * (Bx - Ax);
  if (myIsLimited)
  {
    OK = LimitAndMayBeSwap(F, Ax, Bx, FBx, Cx, FCx);
    if (!OK)
      return;
  }
  else
  {
    OK = F.Value(Cx, FCx);
    if (!OK)
      return;
  }

  while (FBx > FCx)
  {
    r    = (Bx - Ax) * (FBx - FCx);
    q    = (Bx - Cx) * (FBx - FAx);
    u    = Bx - ((Bx - Cx) * q - (Bx - Ax) * r) / (2.0 * SIGN(MAX(fabs(q - r), TINY), q - r));
    ulim = Bx + GLIMIT * (Cx - Bx);
    if (myIsLimited)
      ulim = Limited(ulim);
    if ((Bx - u) * (u - Cx) > 0.0)
    {
      // u is between B and C
      OK = F.Value(u, fu);
      if (!OK)
        return;
      if (fu < FCx)
      {
        // solution is found (B, u, c)
        Ax   = Bx;
        Bx   = u;
        FAx  = FBx;
        FBx  = fu;
        Done = true;
        return;
      }
      else if (fu > FBx)
      {
        // solution is found (A, B, u)
        Cx   = u;
        FCx  = fu;
        Done = true;
        return;
      }
      // get next prob after (B, C)
      u = Cx + Lambda * (Cx - Bx);
      if (myIsLimited)
      {
        OK = LimitAndMayBeSwap(F, Bx, Cx, FCx, u, fu);
        if (!OK)
          return;
      }
      else
      {
        OK = F.Value(u, fu);
        if (!OK)
          return;
      }
    }
    else if ((Cx - u) * (u - ulim) > 0.0)
    {
      // u is beyond C but between C and limit
      OK = F.Value(u, fu);
      if (!OK)
        return;
    }
    else if ((u - ulim) * (ulim - Cx) >= 0.0)
    {
      // u is beyond limit
      u  = ulim;
      OK = F.Value(u, fu);
      if (!OK)
        return;
    }
    else
    {
      // u tends to approach to the side of A,
      // so reset it to the next prob after (B, C)
      u = Cx + GOLD * (Cx - Bx);
      if (myIsLimited)
      {
        OK = LimitAndMayBeSwap(F, Bx, Cx, FCx, u, fu);
        if (!OK)
          return;
      }
      else
      {
        OK = F.Value(u, fu);
        if (!OK)
          return;
      }
    }
    SHFT(Ax, Bx, Cx, u);
    SHFT(FAx, FBx, FCx, fu);
  }
  Done = true;
}

math_BracketMinimum::math_BracketMinimum(math_Function&      F,
                                         const double A,
                                         const double B)
    : Done(false),
      Ax(A),
      Bx(B),
      Cx(0.),
      FAx(0.),
      FBx(0.),
      FCx(0.),
      myLeft(-Precision::Infinite()),
      myRight(Precision::Infinite()),
      myIsLimited(false),
      myFA(false),
      myFB(false)
{
  Perform(F);
}

math_BracketMinimum::math_BracketMinimum(math_Function&      F,
                                         const double A,
                                         const double B,
                                         const double FA)
    : Done(false),
      Ax(A),
      Bx(B),
      Cx(0.),
      FAx(FA),
      FBx(0.),
      FCx(0.),
      myLeft(-Precision::Infinite()),
      myRight(Precision::Infinite()),
      myIsLimited(false),
      myFA(true),
      myFB(false)
{
  Perform(F);
}

math_BracketMinimum::math_BracketMinimum(math_Function&      F,
                                         const double A,
                                         const double B,
                                         const double FA,
                                         const double FB)
    : Done(false),
      Ax(A),
      Bx(B),
      Cx(0.),
      FAx(FA),
      FBx(FB),
      FCx(0.),
      myLeft(-Precision::Infinite()),
      myRight(Precision::Infinite()),
      myIsLimited(false),
      myFA(true),
      myFB(true)
{
  Perform(F);
}

void math_BracketMinimum::Values(double& A, double& B, double& C) const
{

  StdFail_NotDone_Raise_if(!Done, " ");
  A = Ax;
  B = Bx;
  C = Cx;
}

void math_BracketMinimum::FunctionValues(double& FA,
                                         double& FB,
                                         double& FC) const
{

  StdFail_NotDone_Raise_if(!Done, " ");
  FA = FAx;
  FB = FBx;
  FC = FCx;
}

void math_BracketMinimum::Dump(Standard_OStream& o) const
{

  o << "math_BracketMinimum ";
  if (Done)
  {
    o << " Status = Done \n";
    o << " The bracketed triplet is: " << std::endl;
    o << Ax << ", " << Bx << ", " << Cx << std::endl;
    o << " The corresponding function values are: " << std::endl;
    o << FAx << ", " << FBx << ", " << FCx << std::endl;
  }
  else
  {
    o << " Status = not Done \n";
  }
}
