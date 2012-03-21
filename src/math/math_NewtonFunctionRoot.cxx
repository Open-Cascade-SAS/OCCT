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

#include <math_NewtonFunctionRoot.ixx>
#include <math_FunctionWithDerivative.hxx>


math_NewtonFunctionRoot::math_NewtonFunctionRoot (math_FunctionWithDerivative& F, 
						  const Standard_Real Guess, 
						  const Standard_Real EpsX , 
						  const Standard_Real EpsF , 
						  const Standard_Real A,
						  const Standard_Real B,
						  const Standard_Integer NbIterations ){
  EpsilonX = EpsX;
  EpsilonF = EpsF;
  Binf = A;
  Bsup = B;
  Itermax = NbIterations;
  Done = Standard_False;
  X = RealLast();
  DFx = 0;
  Fx = RealLast();
  It = 0;
  Perform(F, Guess);
}


math_NewtonFunctionRoot::math_NewtonFunctionRoot (const Standard_Real A , 
						  const Standard_Real B, 
						  const Standard_Real EpsX , 
						  const Standard_Real EpsF , 
						  const Standard_Integer NbIterations ){
  
  Binf = A;
  Bsup = B;
  EpsilonX = EpsX;
  EpsilonF = EpsF;
  Itermax = NbIterations;
  Done = Standard_False;
  X = RealLast();
  DFx = 0;
  Fx = RealLast();
  It = 0;
}


math_NewtonFunctionRoot::math_NewtonFunctionRoot (math_FunctionWithDerivative& F, 
						  const Standard_Real Guess, 
						  const Standard_Real EpsX , 
						  const Standard_Real EpsF , 
						  const Standard_Integer NbIterations ){
  EpsilonX = EpsX;
  EpsilonF = EpsF;
  Itermax = NbIterations;
  Binf = RealFirst();
  Bsup = RealLast();
  Done = Standard_False;
  X = RealLast();
  DFx = 0;
  Fx = RealLast();
  It = 0;
  Perform(F, Guess);
}


void   math_NewtonFunctionRoot::Perform(math_FunctionWithDerivative& F,
					const Standard_Real Guess) {
  
  Standard_Real Dx;
  Standard_Boolean Ok;
  Standard_Real AA, BB;
  
  //--------------------------------------------------
  //-- lbr le 12 Nov 97
  //-- la meilleure estimation n est pas sauvee et on 
  //-- renvoie une solution plus fausse que Guess
  Standard_Real BestX=X,BestFx=RealLast();  
  //-- 
  
  if ( Binf < Bsup) {
    AA = Binf;
    BB = Bsup;
  }
  else {
    AA = Bsup;
    BB = Binf;
  }
  
  Dx = RealLast();
  Fx = RealLast(); 
  X = Guess;
  It = 1;
  while ( (It <= Itermax) && ( (Abs(Dx) > EpsilonX) || 
			      (Abs(Fx) > EpsilonF) ) ) {
    Ok = F.Values(X,Fx,DFx);
    
    Standard_Real AbsFx = Fx; if(AbsFx<0) AbsFx=-AbsFx;
    if(AbsFx<BestFx) {
      BestFx=AbsFx; 
      BestX =X;
    }
    
    if (Ok) {
      if (DFx == 0.) { 
	Done = Standard_False;
	It = Itermax + 1;                     
      }
      else {
	Dx = Fx/DFx;
	X -= Dx;                       
	// Limitation des variations de X:
	if (X <= AA) X = AA;
	if (X >= BB) X = BB;
	It++;
      }
    }
    else { 
      Done = Standard_False;
      It = Itermax + 1;                     
    }
  }
  X = BestX;
  
  if (It <= Itermax) { 
    Done = Standard_True;
  }
  else 
  {
    Done = Standard_False;
  }
}  


void math_NewtonFunctionRoot::Dump(Standard_OStream& o) const {
  
  o <<"math_NewtonFunctionRoot ";
  if (Done) {
    o << " Status = Done \n";
    o << " Location found = " << X <<"\n";
    o << " function value at this minimum = " << Fx <<"\n";
    o << " Number of iterations = " << It <<"\n";
  }
  else {
    o << "Status = not Done \n";
  }
}



