//static const char* sccsid = "@(#)math_NewtonFunctionRoot.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.
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



