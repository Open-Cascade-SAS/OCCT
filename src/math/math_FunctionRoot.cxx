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

#include <math_FunctionRoot.hxx>
#include <math_FunctionSetRoot.hxx>
#include <math_FunctionSetWithDerivatives.hxx>
#include <math_FunctionWithDerivative.hxx>
#include <Standard_Failure.hxx>
#include <StdFail_NotDone.hxx>

class math_MyFunctionSetWithDerivatives : public math_FunctionSetWithDerivatives {

  private:
         math_FunctionWithDerivative *Ff;
  
  public :

    math_MyFunctionSetWithDerivatives (math_FunctionWithDerivative& F );
    
    Standard_Integer NbVariables () const;
    Standard_Integer NbEquations () const;
    Standard_Boolean Value (const math_Vector& X, math_Vector& F) ;
    Standard_Boolean Derivatives (const math_Vector& X, math_Matrix& D) ;    
    Standard_Boolean Values (const math_Vector& X, math_Vector& F, math_Matrix& D) ;    
};


    math_MyFunctionSetWithDerivatives::math_MyFunctionSetWithDerivatives
              (math_FunctionWithDerivative& F ) {
                  Ff = &F ;
     
	      }
    
    Standard_Integer math_MyFunctionSetWithDerivatives::NbVariables () const {
      return 1;
    }
    Standard_Integer math_MyFunctionSetWithDerivatives::NbEquations () const {
      return 1;
    }
    Standard_Boolean math_MyFunctionSetWithDerivatives::Value (const math_Vector& X, math_Vector& Fs)  {
      return Ff->Value(X(1),Fs(1));
    }
    Standard_Boolean math_MyFunctionSetWithDerivatives::Derivatives (const math_Vector& X, math_Matrix& D) {
      return Ff->Derivative(X(1),D(1,1));
    }    
    Standard_Boolean math_MyFunctionSetWithDerivatives::Values (const math_Vector& X, math_Vector& F, math_Matrix& D) {
      return Ff->Values(X(1),F(1),D(1,1));      
    }




   math_FunctionRoot::math_FunctionRoot(math_FunctionWithDerivative& F, 
                                        const Standard_Real Guess, 
                                        const Standard_Real Tolerance, 
                                        const Standard_Integer NbIterations ){
     math_Vector V(1,1), Tol(1,1);
     math_MyFunctionSetWithDerivatives Ff(F);
     V(1)=Guess;
     Tol(1) = Tolerance;
     math_FunctionSetRoot Sol(Ff, Tol, NbIterations);
     Sol.Perform(Ff, V);
     Done = Sol.IsDone(); 
     if (Done) {
       F.GetStateNumber();
       TheRoot = Sol.Root()(1);
       TheDerivative = Sol.Derivative()(1,1);
       F.Value(TheRoot,TheError);
       NbIter = Sol.NbIterations();
     }       
   }
   math_FunctionRoot::math_FunctionRoot(math_FunctionWithDerivative& F, 
                                        const Standard_Real Guess, 
                                        const Standard_Real Tolerance, 
                                        const Standard_Real A,
                                        const Standard_Real B,
                                        const Standard_Integer NbIterations ){
     math_Vector V(1,1),Aa(1,1),Bb(1,1), Tol(1,1);
     math_MyFunctionSetWithDerivatives Ff(F);
     V(1)=Guess;
     Tol(1) = Tolerance;
     Aa(1)=A;
     Bb(1)=B;
     math_FunctionSetRoot Sol(Ff, Tol, NbIterations);
     Sol.Perform(Ff, V, Aa, Bb);
     Done = Sol.IsDone();
     if (Done) {
       F.GetStateNumber();
       TheRoot = Sol.Root()(1);
       TheDerivative = Sol.Derivative()(1,1);
       F.Value(TheRoot,TheError);
       NbIter = Sol.NbIterations();
     }
   }

    void math_FunctionRoot::Dump(Standard_OStream& o) const {

       o<< "math_FunctionRoot ";
       if(Done) {
         o<< " Status = Done \n";
	 o << " Number of iterations = " << NbIter << endl;
	 o << " The Root is: " << TheRoot << endl;
	 o << "The value at the root is: " << TheError << endl;
       }
       else {
         o<< " Status = not Done \n";
       }
    }
