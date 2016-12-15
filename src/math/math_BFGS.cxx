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

//#ifndef OCCT_DEBUG
#define No_Standard_RangeError
#define No_Standard_OutOfRange
#define No_Standard_DimensionError

//#endif

#include <math_BFGS.hxx>
#include <math_BracketMinimum.hxx>
#include <math_BrentMinimum.hxx>
#include <math_FunctionWithDerivative.hxx>
#include <math_Matrix.hxx>
#include <math_MultipleVarFunctionWithGradient.hxx>
#include <Standard_DimensionError.hxx>
#include <StdFail_NotDone.hxx>
#include <Precision.hxx>

#define R 0.61803399
#define C (1.0-R)
#define SHFT(a,b,c,d) (a)=(b);(b)=(c);(c)=(d);
#define SIGN(a, b) ((b) > 0.0 ? fabs(a) : -fabs(a))
#define MOV3(a,b,c, d, e, f) (a)=(d); (b)= (e); (c)=(f);


// l'utilisation de math_BrentMinumim pur trouver un minimum dans une direction
// donnee n'est pas du tout optimale. voir peut etre interpolation cubique
// classique et aussi essayer "recherche unidimensionnelle economique"
// PROGRAMMATION MATHEMATIQUE (theorie et algorithmes) tome1 page 82.

class DirFunction : public math_FunctionWithDerivative {

     math_Vector *P0;
     math_Vector *Dir;
     math_Vector *P;
     math_Vector *G;
     math_MultipleVarFunctionWithGradient *F;

public :

     DirFunction(math_Vector& V1, 
                 math_Vector& V2,
                 math_Vector& V3,
		 math_Vector& V4,
                 math_MultipleVarFunctionWithGradient& f) ;

     void Initialize(const math_Vector& p0, const math_Vector& dir) const;
     void TheGradient(math_Vector& Grad);
     virtual Standard_Boolean Value(const Standard_Real x, Standard_Real& fval) ;
     virtual Standard_Boolean Values(const Standard_Real x, Standard_Real& fval, Standard_Real& D) ;
     virtual Standard_Boolean Derivative(const Standard_Real x, Standard_Real& D) ;

     
};

     DirFunction::DirFunction(math_Vector& V1, 
                              math_Vector& V2,
                              math_Vector& V3,
			      math_Vector& V4,
                              math_MultipleVarFunctionWithGradient& f) {
        
        P0  = &V1;
        Dir = &V2;
        P   = &V3;
        F   = &f;
	G =   &V4;
     }

     void DirFunction::Initialize(const math_Vector& p0, 
                                  const math_Vector& dir)  const{

        *P0 = p0;
        *Dir = dir;
     }

     void DirFunction::TheGradient(math_Vector& Grad) {
       Grad = *G;
     }


     Standard_Boolean DirFunction::Value(const Standard_Real x, 
					 Standard_Real& fval) 
     {
        *P = *Dir;
        P->Multiply(x);
        P->Add(*P0); 
        fval = 0.;
        return F->Value(*P, fval);
     }

     Standard_Boolean DirFunction::Values(const Standard_Real x, 
					  Standard_Real& fval, 
					  Standard_Real& D) 
     {
        *P = *Dir;
        P->Multiply(x);
        P->Add(*P0);
        fval = D = 0.;
        if (F->Values(*P, fval, *G))
        {
          D = (*G).Multiplied(*Dir);
          return Standard_True;
        }
        return Standard_False;
     }
     Standard_Boolean DirFunction::Derivative(const Standard_Real x, 
					      Standard_Real& D) 
     {
        *P = *Dir;
        P->Multiply(x);
        P->Add(*P0);        
	Standard_Real fval;
        D = 0.;
        if (F->Values(*P, fval, *G))
        {
          D = (*G).Multiplied(*Dir);
          return Standard_True;
        }
        return Standard_False;
     }

//=======================================================================
//function : ComputeInitScale
//purpose  : Compute the appropriate initial value of scale factor to apply
//           to the direction to approach to the minimum of the function
//=======================================================================
static Standard_Boolean ComputeInitScale(const Standard_Real theF0,
                                         const math_Vector& theDir,
                                         const math_Vector& theGr,
                                         Standard_Real& theScale)
{
  Standard_Real dy1 = theGr * theDir;
  if (Abs(dy1) < RealSmall())
    return Standard_False;
  Standard_Real aHnr1 = theDir.Norm2();
  Standard_Real alfa = 0.7*(-theF0) / dy1;
  theScale = 0.015 / Sqrt(aHnr1);
  if (theScale > alfa)
    theScale = alfa;
  return Standard_True;
}

//=======================================================================
//function : ComputeMinMaxScale
//purpose  : For a given point and direction, and bounding box,
//           find min and max scale factors with which the point reaches borders
//           if we apply translation Point+Dir*Scale.
//return   : True if found, False if point is out of bounds.
//=======================================================================
static Standard_Boolean ComputeMinMaxScale(const math_Vector& thePoint,
                                           const math_Vector& theDir,
                                           const math_Vector& theLeft,
                                           const math_Vector& theRight,
                                           Standard_Real& theMinScale,
                                           Standard_Real& theMaxScale)
{
  Standard_Integer anIdx;
  for (anIdx = 1; anIdx <= theLeft.Upper(); anIdx++)
  {
    Standard_Real aLeft = theLeft(anIdx) - thePoint(anIdx);
    Standard_Real aRight = theRight(anIdx) - thePoint(anIdx);
    if (Abs(theDir(anIdx)) > RealSmall())
    {
      // use PConfusion to get off a little from the bounds to prevent
      // possible refuse in Value function.
      Standard_Real aLScale = (aLeft + Precision::PConfusion()) / theDir(anIdx);
      Standard_Real aRScale = (aRight - Precision::PConfusion()) / theDir(anIdx);
      if (Abs(aLeft) < Precision::PConfusion())
      {
        // point is on the left border
        theMaxScale = Min(theMaxScale, Max(0., aRScale));
        theMinScale = Max(theMinScale, Min(0., aRScale));
      }
      else if (Abs(aRight) < Precision::PConfusion())
      {
        // point is on the right border
        theMaxScale = Min(theMaxScale, Max(0., aLScale));
        theMinScale = Max(theMinScale, Min(0., aLScale));
      }
      else if (aLeft * aRight < 0)
      {
        // point is inside allowed range
        theMaxScale = Min(theMaxScale, Max(aLScale, aRScale));
        theMinScale = Max(theMinScale, Min(aLScale, aRScale));
      }
      else
        // point is out of bounds
        return Standard_False;
    }
    else
    {
      // Direction is parallel to the border.
      // Check that the point is not out of bounds
      if (aLeft > Precision::PConfusion() ||
        aRight < -Precision::PConfusion())
      {
        return Standard_False;
      }
    }
  }
  return Standard_True;
}

static Standard_Boolean MinimizeDirection(math_Vector&   P,
                                          Standard_Real  F0,
                                          math_Vector&   Gr,
                                          math_Vector&   Dir,
                                          Standard_Real& Result,
                                          DirFunction&   F,
                                          Standard_Boolean isBounds,
                                          const math_Vector& theLeft,
                                          const math_Vector& theRight)
{
  Standard_Real lambda;
  if (!ComputeInitScale(F0, Dir, Gr, lambda))
    return Standard_False;

  // by default the scaling range is unlimited
  Standard_Real aMinLambda = -Precision::Infinite();
  Standard_Real aMaxLambda = Precision::Infinite();
  if (isBounds)
  {
    // limit the scaling range taking into account the bounds
    if (!ComputeMinMaxScale(P, Dir, theLeft, theRight, aMinLambda, aMaxLambda))
      return Standard_False;

    if (aMinLambda > -Precision::PConfusion() && aMaxLambda < Precision::PConfusion())
    {
      // Point is on the border and the direction shows outside.
      // Make direction to go along the border
      for (Standard_Integer anIdx = 1; anIdx <= theLeft.Upper(); anIdx++)
      {
        if (Abs(P(anIdx) - theRight(anIdx)) < Precision::PConfusion() ||
          Abs(P(anIdx) - theLeft(anIdx)) < Precision::PConfusion())
        {
          Dir(anIdx) = 0.0;
        }
      }

      // re-compute scale values with new direction
      if (!ComputeInitScale(F0, Dir, Gr, lambda))
        return Standard_False;
      if (!ComputeMinMaxScale(P, Dir, theLeft, theRight, aMinLambda, aMaxLambda))
        return Standard_False;
    }
    lambda = Min(lambda, aMaxLambda);
    lambda = Max(lambda, aMinLambda);
  }

  F.Initialize(P, Dir);
  Standard_Real F1;
  if (!F.Value(lambda, F1))
    return Standard_False;
  math_BracketMinimum Bracket(0.0, lambda);
  if (isBounds)
    Bracket.SetLimits(aMinLambda, aMaxLambda);
  Bracket.SetFA(F0);
  Bracket.SetFB(F1);
  Bracket.Perform(F);
  if (Bracket.IsDone()) {
    // find minimum inside the bracket
    Standard_Real ax, xx, bx, Fax, Fxx, Fbx;
    Bracket.Values(ax, xx, bx);
    Bracket.FunctionValues(Fax, Fxx, Fbx);

    Standard_Integer niter = 100;
    Standard_Real tol = 1.e-03;
    math_BrentMinimum Sol(tol, Fxx, niter, 1.e-08);
    Sol.Perform(F, ax, xx, bx);
    if (Sol.IsDone()) {
      Standard_Real Scale = Sol.Location();
      Result = Sol.Minimum();
      Dir.Multiply(Scale);
      P.Add(Dir);
      return Standard_True;
    }
  }
  else if (isBounds)
  {
    // Bracket definition is failure. If the bounds are defined then
    // set current point to intersection with bounds
    Standard_Real aFMin, aFMax;
    if (!F.Value(aMinLambda, aFMin))
      return Standard_False;
    if (!F.Value(aMaxLambda, aFMax))
      return Standard_False;
    Standard_Real aBestLambda;
    if (aFMin < aFMax)
    {
      aBestLambda = aMinLambda;
      Result = aFMin;
    }
    else
    {
      aBestLambda = aMaxLambda;
      Result = aFMax;
    }
    Dir.Multiply(aBestLambda);
    P.Add(Dir);
    return Standard_True;
  }
  return Standard_False;
}


void  math_BFGS::Perform(math_MultipleVarFunctionWithGradient& F,
                         const math_Vector& StartingPoint) {
  
       Standard_Boolean Good;
       Standard_Integer n = TheLocation.Length();
       Standard_Integer j, i;
       Standard_Real fae, fad, fac;

       math_Vector xi(1, n), dg(1, n), hdg(1, n);
       math_Matrix hessin(1, n, 1, n);
       hessin.Init(0.0);

       math_Vector Temp1(1, n);
       math_Vector Temp2(1, n);
       math_Vector Temp3(1, n);
       math_Vector Temp4(1, n);
       DirFunction F_Dir(Temp1, Temp2, Temp3, Temp4, F);

       TheLocation = StartingPoint;
       Good = F.Values(TheLocation, PreviousMinimum, TheGradient);
       if(!Good) { 
         Done = Standard_False;
         TheStatus = math_FunctionError;
         return;
       }
       for(i = 1; i <= n; i++) {
         hessin(i, i) = 1.0;
	 xi(i) = -TheGradient(i);
       }


       for(nbiter = 1; nbiter <= Itermax; nbiter++) {
	 TheMinimum = PreviousMinimum;
         Standard_Boolean IsGood = MinimizeDirection(TheLocation, TheMinimum,
                                                     TheGradient,
                                                     xi, TheMinimum, F_Dir,
                                                     myIsBoundsDefined, myLeft, myRight);
         if(!IsGood) {
           Done = Standard_False;
           TheStatus = math_DirectionSearchError;
           return;
         }
         if(IsSolutionReached(F)) {
           Done = Standard_True;
           TheStatus = math_OK;
           return;
         }
	 if (nbiter == Itermax) {
	   Done = Standard_False;
	   TheStatus = math_TooManyIterations;
	   return;
	 }
         PreviousMinimum = TheMinimum;

	 dg = TheGradient;

         Good = F.Values(TheLocation, TheMinimum, TheGradient);
         if(!Good) { 
           Done = Standard_False;
           TheStatus = math_FunctionError;
           return;
         }

         for(i = 1; i <= n; i++) {
	   dg(i) = TheGradient(i) - dg(i);
	 }
	 for(i = 1; i <= n; i++) {
	   hdg(i) = 0.0;
	   for (j = 1; j <= n; j++) 
	     hdg(i) += hessin(i, j) * dg(j);
	 }
	 fac = fae = 0.0;
	 for(i = 1; i <= n; i++) {
	   fac += dg(i) * xi(i);
	   fae += dg(i) * hdg(i);
	 }
         fac = 1.0 / fac;
         fad = 1.0 / fae;

	 for(i = 1; i <= n; i++) 
	   dg(i) = fac * xi(i) - fad * hdg(i);
	 
         for(i = 1; i <= n; i++) 
           for(j = 1; j <= n; j++)
             hessin(i, j) += fac * xi(i) * xi(j)
               - fad * hdg(i) * hdg(j) + fae * dg(i) * dg(j);

	 for(i = 1; i <= n; i++) {
	   xi(i) = 0.0;
	   for (j = 1; j <= n; j++) xi(i) -= hessin(i, j) * TheGradient(j);
	 }  
       }
       Done = Standard_False;
       TheStatus = math_TooManyIterations;
       return;
   }

    Standard_Boolean math_BFGS::IsSolutionReached(
                           math_MultipleVarFunctionWithGradient&) const {

       return 2.0 * fabs(TheMinimum - PreviousMinimum) <= 
              XTol * (fabs(TheMinimum) + fabs(PreviousMinimum) + EPSZ);
    }
                             
    math_BFGS::math_BFGS(const Standard_Integer     NbVariables,
                         const Standard_Real        Tolerance,
                         const Standard_Integer     NbIterations,
                         const Standard_Real        ZEPS) 
    : TheStatus(math_OK),
      TheLocation(1, NbVariables),
      TheGradient(1, NbVariables),
      PreviousMinimum(0.),
      TheMinimum(0.),
      XTol(Tolerance),
      EPSZ(ZEPS),
      nbiter(0),
      myIsBoundsDefined(Standard_False),
      myLeft(1, NbVariables, 0.0),
      myRight(1, NbVariables, 0.0),
      Done(Standard_False),
      Itermax(NbIterations)
    {
    }


    math_BFGS::~math_BFGS()
    {
    }

    void math_BFGS::Dump(Standard_OStream& o) const {

       o<< "math_BFGS resolution: ";
       if(Done) {
         o << " Status = Done \n";
	 o <<" Location Vector = " << Location() << "\n";
	 o <<" Minimum value = "<< Minimum()<<"\n";
	 o <<" Number of iterations = "<<NbIterations() <<"\n";;
       }
       else {
         o<< " Status = not Done because " << (Standard_Integer)TheStatus << "\n";
       }
    }

//=======================================================================
//function : SetBoundary
//purpose  : Set boundaries for conditional optimization
//=======================================================================
void math_BFGS::SetBoundary(const math_Vector& theLeftBorder,
                            const math_Vector& theRightBorder)
{
  myLeft = theLeftBorder;
  myRight = theRightBorder;
  myIsBoundsDefined = Standard_True;
}
