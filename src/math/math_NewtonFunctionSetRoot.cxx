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

#include <math_FunctionSetWithDerivatives.hxx>
#include <math_Matrix.hxx>
#include <math_NewtonFunctionSetRoot.hxx>
#include <math_Recipes.hxx>
#include <Standard_DimensionError.hxx>
#include <StdFail_NotDone.hxx>

//=======================================================================
//function : math_NewtonFunctionSetRoot
//purpose  : Constructor
//=======================================================================
math_NewtonFunctionSetRoot::math_NewtonFunctionSetRoot(
  math_FunctionSetWithDerivatives& theFunction,
  const math_Vector&               theXTolerance,
  const Standard_Real              theFTolerance,
  const Standard_Integer           theNbIterations)

: TolX    (1, theFunction.NbVariables()),
  TolF    (theFTolerance),
  Indx    (1, theFunction.NbVariables()),
  Scratch (1, theFunction.NbVariables()),
  Sol     (1, theFunction.NbVariables()),
  DeltaX  (1, theFunction.NbVariables()),
  FValues (1, theFunction.NbVariables()),
  Jacobian(1, theFunction.NbVariables(), 1, theFunction.NbVariables()),
  Done    (Standard_False),
  State   (0),
  Iter    (0),
  Itermax (theNbIterations)
{
  SetTolerance(theXTolerance);
}

//=======================================================================
//function : math_NewtonFunctionSetRoot
//purpose  : Constructor
//=======================================================================
math_NewtonFunctionSetRoot::math_NewtonFunctionSetRoot(
  math_FunctionSetWithDerivatives& theFunction,
  const Standard_Real              theFTolerance,
  const Standard_Integer           theNbIterations)

: TolX    (1, theFunction.NbVariables()),
  TolF    (theFTolerance),
  Indx    (1, theFunction.NbVariables()),
  Scratch (1, theFunction.NbVariables()),
  Sol     (1, theFunction.NbVariables()),
  DeltaX  (1, theFunction.NbVariables()),
  FValues (1, theFunction.NbVariables()),
  Jacobian(1, theFunction.NbVariables(), 1, theFunction.NbVariables()),
  Done    (Standard_False),
  State   (0),
  Iter    (0),
  Itermax (theNbIterations)
{
}

//=======================================================================
//function : ~math_NewtonFunctionSetRoot
//purpose  : Destructor
//=======================================================================
math_NewtonFunctionSetRoot::~math_NewtonFunctionSetRoot()
{
}

//=======================================================================
//function : SetTolerance
//purpose  : 
//=======================================================================
void math_NewtonFunctionSetRoot::SetTolerance(const math_Vector& theXTolerance)
{
  for (Standard_Integer i = 1; i <= TolX.Length(); ++i)
    TolX(i) = theXTolerance(i);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void math_NewtonFunctionSetRoot::Perform(
  math_FunctionSetWithDerivatives& theFunction,
  const math_Vector&               theStartingPoint)
{
  const math_Vector anInf(1, theFunction.NbVariables(), RealFirst());
  const math_Vector aSup (1, theFunction.NbVariables(), RealLast ());

  Perform(theFunction, theStartingPoint, anInf, aSup);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void math_NewtonFunctionSetRoot::Perform(
                           math_FunctionSetWithDerivatives& F,
                           const math_Vector& StartingPoint,
                           const math_Vector& InfBound,
                           const math_Vector& SupBound) 
{

  Standard_Real d;
  Standard_Boolean OK;
  Standard_Integer Error;
  
  Done = Standard_False;
  Sol = StartingPoint;
  OK = F.Values(Sol, FValues, Jacobian);
  if(!OK) return;
  for(Iter = 1; Iter <= Itermax; Iter++) {
    for(Standard_Integer k = 1; k <= DeltaX.Length(); k++) {
      DeltaX(k) = -FValues(k);
    }
    Error = LU_Decompose(Jacobian, Indx, d, Scratch, 1.0e-30);
    if(Error) return; 
    LU_Solve(Jacobian, Indx, DeltaX);
    for(Standard_Integer i = 1; i <= Sol.Length(); i++) { 
      Sol(i) += DeltaX(i);
      
      // Limitation de Sol dans les bornes [InfBound, SupBound] :
      if (Sol(i) <= InfBound(i)) Sol(i) = InfBound(i);
      if (Sol(i) >= SupBound(i)) Sol(i) = SupBound(i);
      
    }
    OK = F.Values(Sol, FValues, Jacobian);
    if(!OK) return;
    if(IsSolutionReached(F)) { 
      State = F.GetStateNumber();
      Done = Standard_True; 
      return;
    }
  }               
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================
void math_NewtonFunctionSetRoot::Dump(Standard_OStream& o) const 
{
  o <<"math_NewtonFunctionSetRoot ";
  if (Done) {
    o << " Status = Done \n";
    o << " Vector solution = " << Sol <<"\n";
    o << " Value of the function at this solution = \n";
    o << FValues <<"\n";
    o << " Number of iterations = " << Iter <<"\n";
  }
  else {
    o << "Status = not Done \n";
  }
}
