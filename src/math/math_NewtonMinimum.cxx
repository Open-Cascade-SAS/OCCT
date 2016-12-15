// Created on: 1996-05-03
// Created by: Philippe MANGIN
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

//#ifndef OCCT_DEBUG
#define No_Standard_RangeError
#define No_Standard_OutOfRange
#define No_Standard_DimensionError

//#endif

#include <math_Gauss.hxx>
#include <math_Jacobi.hxx>
#include <math_MultipleVarFunctionWithHessian.hxx>
#include <math_NewtonMinimum.hxx>
#include <Precision.hxx>
#include <Standard_DimensionError.hxx>
#include <StdFail_NotDone.hxx>

//=======================================================================
//function : math_NewtonMinimum
//purpose  : Constructor
//=======================================================================
math_NewtonMinimum::math_NewtonMinimum(
  const math_MultipleVarFunctionWithHessian& theFunction,
  const Standard_Real                        theTolerance,
  const Standard_Integer                     theNbIterations,
  const Standard_Real                        theConvexity,
  const Standard_Boolean                     theWithSingularity
  )
: TheStatus  (math_NotBracketed),
  TheLocation(1, theFunction.NbVariables()),
  TheGradient(1, theFunction.NbVariables()),
  TheStep    (1, theFunction.NbVariables(), 10.0 * theTolerance),
  TheHessian (1, theFunction.NbVariables(), 1, theFunction.NbVariables()),
  PreviousMinimum   (0.0),
  TheMinimum        (0.0),
  MinEigenValue     (0.0),
  XTol              (theTolerance),
  CTol              (theConvexity),
  nbiter            (0),
  NoConvexTreatement(theWithSingularity),
  Convex            (Standard_True),
  myIsBoundsDefined(Standard_False),
  myLeft(1, theFunction.NbVariables(), 0.0),
  myRight(1, theFunction.NbVariables(), 0.0),
  Done              (Standard_False),
  Itermax           (theNbIterations)
{
}

//=======================================================================
//function : ~math_NewtonMinimum
//purpose  : Destructor
//=======================================================================
math_NewtonMinimum::~math_NewtonMinimum()
{
}

//=======================================================================
//function : SetBoundary
//purpose  : Set boundaries for conditional optimization
//=======================================================================
void math_NewtonMinimum::SetBoundary(const math_Vector& theLeftBorder,
                                     const math_Vector& theRightBorder)
{
  myLeft = theLeftBorder;
  myRight = theRightBorder;
  myIsBoundsDefined = Standard_True;
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void math_NewtonMinimum::Perform(math_MultipleVarFunctionWithHessian& F,
                                 const math_Vector& StartingPoint)
{
  math_Vector Point1 (1, F.NbVariables());
  Point1 =  StartingPoint;
  math_Vector Point2(1, F.NbVariables());
  math_Vector* precedent = &Point1;
  math_Vector* suivant = &Point2;
  math_Vector* auxiliaire = precedent;

  Standard_Boolean Ok = Standard_True;
  Standard_Integer NbConv = 0, ii, Nreduction;
  Standard_Real    VPrecedent, VItere; 

  Done = Standard_True;
  TheStatus = math_OK;
  nbiter = 0;

  while ( Ok && (NbConv < 2) ) {
    nbiter++;

    // Positionnement

    Ok = F.Values(*precedent, VPrecedent, TheGradient, TheHessian);
    if (!Ok) {
       Done = Standard_False;
       TheStatus = math_FunctionError;
       return;
    }
    if (nbiter==1) {
      PreviousMinimum =  VPrecedent;
      TheMinimum =  VPrecedent;
    }
    
    // Traitement de la non convexite

    math_Jacobi CalculVP(TheHessian);
    if ( !CalculVP.IsDone() ) {
       Done = Standard_False;
       TheStatus = math_FunctionError;
       return;
    }

    MinEigenValue = CalculVP.Values() ( CalculVP.Values().Min());
    if ( MinEigenValue < CTol) 
    {
      Convex = Standard_False;
      if (NoConvexTreatement && // Treatment is allowed.
          Abs (MinEigenValue) > CTol) // Treatment will have effect.
      {
        Standard_Real Delta = CTol + 0.1 * Abs(MinEigenValue) - MinEigenValue;
        for (ii=1; ii<=TheGradient.Length(); ii++)
          TheHessian(ii, ii) += Delta;
      }
      else
      {
        Done = Standard_False;
        TheStatus = math_FunctionError;
        return;
      }
    }

    // Schemas de Newton

    math_Gauss LU(TheHessian, CTol/100);
    if ( !LU.IsDone()) {
      Done = Standard_False;
      TheStatus = math_DirectionSearchError;
      return;
    }
    LU.Solve(TheGradient, TheStep);

    if (myIsBoundsDefined)
    {
      // Project point on bounds or nullify TheStep coords if point lies on boudary.

      *suivant = *precedent - TheStep;
      Standard_Real aMult = RealLast();
      for(Standard_Integer anIdx = 1; anIdx <= myLeft.Upper(); anIdx++)
      {
        if (suivant->Value(anIdx) < myLeft(anIdx))
        {
          Standard_Real aValue = Abs(precedent->Value(anIdx) - myLeft(anIdx)) / Abs(TheStep(anIdx));
          aMult = Min (aValue, aMult);
        }

        if (suivant->Value(anIdx) > myRight(anIdx))
        {
          Standard_Real aValue = Abs(precedent->Value(anIdx) - myRight(anIdx)) / Abs(TheStep(anIdx));
          aMult = Min (aValue, aMult);
        }
      }

      if (aMult != RealLast())
      {
        if (aMult > Precision::PConfusion())
        {
          // Project point into param space.
          TheStep *= aMult;
        }
        else
        {
          // Old point on border and new point out of border:
          // Nullify corresponding TheStep indexes.
          for(Standard_Integer anIdx = 1; anIdx <= myLeft.Upper(); anIdx++)
          {
            if (Abs(precedent->Value(anIdx) - myRight(anIdx)) < Precision::PConfusion() ||
                Abs(precedent->Value(anIdx) - myLeft(anIdx) ) < Precision::PConfusion())
            {
              TheStep(anIdx) = 0.0;
            }
          }
        }
      }
    }

    Standard_Boolean hasProblem = Standard_False;
    do
    {
      *suivant = *precedent - TheStep;

      //  Gestion de la convergence
      hasProblem = !(F.Value(*suivant, TheMinimum));

      if (hasProblem)
      {
        TheStep /= 2.0;
      }
    } while (hasProblem);

    if (IsConverged()) { NbConv++; }
    else               { NbConv=0; }

    //  Controle et corrections.

    VItere = TheMinimum;
    TheMinimum = PreviousMinimum;
    Nreduction =0;
    while (VItere > VPrecedent && Nreduction < 10) {
        TheStep *= 0.4;   
	*suivant = *precedent - TheStep;
	F.Value(*suivant, VItere);
	Nreduction++;
    }

    if (VItere <= VPrecedent) {
       auxiliaire =  precedent;
       precedent = suivant;
       suivant = auxiliaire;
       PreviousMinimum = VPrecedent;
       TheMinimum = VItere;
       Ok = (nbiter < Itermax);
       if (!Ok && NbConv < 2) TheStatus = math_TooManyIterations;
    } 
    else {       
       Ok = Standard_False;
       TheStatus = math_DirectionSearchError;
    }
  }
 TheLocation = *precedent;    
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================
void math_NewtonMinimum::Dump(Standard_OStream& o) const 
{
  o<< "math_Newton Optimisation: ";
  o << " Done   ="  << Done << endl; 
  o << " Status = " << (Standard_Integer)TheStatus << endl;
  o << " Location Vector = " << Location() << endl;
  o << " Minimum value = "<< Minimum()<< endl;
  o << " Previous value = "<< PreviousMinimum << endl;
  o << " Number of iterations = " <<NbIterations() << endl;
  o << " Convexity = " << Convex << endl;
  o << " Eigen Value = " << MinEigenValue << endl;
}

