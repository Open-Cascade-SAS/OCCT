// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

//#ifndef DEB
#define No_Standard_RangeError
#define No_Standard_OutOfRange
#define No_Standard_DimensionError
//#endif

#include <math_NewtonFunctionSetRoot.ixx>
#include <math_Recipes.hxx>
#include <math_FunctionSetWithDerivatives.hxx>

Standard_Boolean math_NewtonFunctionSetRoot::IsSolutionReached
//                              (math_FunctionSetWithDerivatives& F) 
                              (math_FunctionSetWithDerivatives& ) 
{
  
  for(Standard_Integer i = DeltaX.Lower(); i <= DeltaX.Upper(); i++) {
    if(Abs(DeltaX(i)) > TolX(i) || Abs(FValues(i)) > TolF) return Standard_False;
  } 
  return Standard_True;
}


// Constructeurs d'initialisation des champs (pour utiliser Perform)

math_NewtonFunctionSetRoot::math_NewtonFunctionSetRoot(
			    math_FunctionSetWithDerivatives& F,
			    const math_Vector& XTol,
			    const Standard_Real FTol,
			    const Standard_Integer NbIterations):
                            TolX(1, F.NbVariables()),
                            TolF(FTol),
                            Indx(1, F.NbVariables()),
                            Scratch(1, F.NbVariables()),
                            Sol(1, F.NbVariables()),
                            DeltaX(1, F.NbVariables()),
                            FValues(1, F.NbVariables()),
                            Jacobian(1, F.NbVariables(),
				     1, F.NbVariables()),
                            Itermax(NbIterations)
{
  for (Standard_Integer i = 1; i <= TolX.Length(); i++) {
    TolX(i) = XTol(i);
  }
}


math_NewtonFunctionSetRoot::math_NewtonFunctionSetRoot(
			    math_FunctionSetWithDerivatives& F,
			    const Standard_Real FTol,
			    const Standard_Integer NbIterations):
                            TolX(1, F.NbVariables()),
                            TolF(FTol),
                            Indx(1, F.NbVariables()),
                            Scratch(1, F.NbVariables()),
                            Sol(1, F.NbVariables()),
                            DeltaX(1, F.NbVariables()),
                            FValues(1, F.NbVariables()),
                            Jacobian(1, F.NbVariables(),
				     1, F.NbVariables()),
                            Itermax(NbIterations)
{
}


math_NewtonFunctionSetRoot::math_NewtonFunctionSetRoot
                           (math_FunctionSetWithDerivatives& F,
			    const math_Vector& StartingPoint,
			    const math_Vector&    XTol,
			    const Standard_Real    FTol,
			    const Standard_Integer NbIterations) :
			    
			    TolX(1, F.NbVariables()),
			    TolF(FTol),
			    Indx    (1, F.NbVariables()),
			    Scratch (1, F.NbVariables()),
			    Sol     (1, F.NbVariables()),
			    DeltaX  (1, F.NbVariables()),
			    FValues (1, F.NbVariables()),
			    Jacobian(1, F.NbVariables(),
				     1, F.NbVariables()),
			    Itermax(NbIterations)
{
  for (Standard_Integer i = 1; i <= TolX.Length(); i++) {
    TolX(i) = XTol(i);
  }
  math_Vector UFirst(1, F.NbVariables()),
              ULast(1, F.NbVariables());
  UFirst.Init(RealFirst());
  ULast.Init(RealLast());
  Perform(F, StartingPoint, UFirst, ULast);
}

math_NewtonFunctionSetRoot::math_NewtonFunctionSetRoot
                              (math_FunctionSetWithDerivatives& F,
			       const math_Vector& StartingPoint,
                               const math_Vector&    InfBound,
                               const math_Vector&    SupBound,
			       const math_Vector&    XTol,
                               const Standard_Real    FTol,
			       const Standard_Integer NbIterations) :
                                                            
                               TolX(1, F.NbVariables()),
                               TolF(FTol),
                               Indx    (1, F.NbVariables()),
                               Scratch (1, F.NbVariables()),
                               Sol     (1, F.NbVariables()),
                               DeltaX  (1, F.NbVariables()),
                               FValues (1, F.NbVariables()),
                               Jacobian(1, F.NbVariables(),
                                        1, F.NbVariables()),
                               Itermax(NbIterations)
{
  for (Standard_Integer i = 1; i <= TolX.Length(); i++) {
    TolX(i) = XTol(i);
  }
  Perform(F, StartingPoint, InfBound, SupBound);
}

void math_NewtonFunctionSetRoot::Delete()
{}

void math_NewtonFunctionSetRoot::SetTolerance
                              (const math_Vector& XTol)
{
  for (Standard_Integer i = 1; i <= TolX.Length(); i++) {
    TolX(i) = XTol(i);
  }
}



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
