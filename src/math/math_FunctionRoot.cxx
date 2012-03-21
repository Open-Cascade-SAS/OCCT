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

#include <Standard_Failure.hxx>
#include <math_FunctionRoot.ixx>
#include <math_FunctionSetRoot.hxx>

#include <math_FunctionSetWithDerivatives.hxx>
#include <math_FunctionWithDerivative.hxx>


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
     Standard_Boolean Ok;
     math_Vector V(1,1), Tol(1,1);
     math_MyFunctionSetWithDerivatives Ff(F);
     V(1)=Guess;
     Tol(1) = Tolerance;
     math_FunctionSetRoot Sol(Ff,V,Tol,NbIterations);
     Done = Sol.IsDone(); 
     if (Done) {
       F.GetStateNumber();
       TheRoot = Sol.Root()(1);
       TheDerivative = Sol.Derivative()(1,1);
       Ok = F.Value(TheRoot,TheError);
       NbIter = Sol.NbIterations();
     }       
   }
   math_FunctionRoot::math_FunctionRoot(math_FunctionWithDerivative& F, 
                                        const Standard_Real Guess, 
                                        const Standard_Real Tolerance, 
                                        const Standard_Real A,
                                        const Standard_Real B,
                                        const Standard_Integer NbIterations ){
     Standard_Boolean Ok;
     math_Vector V(1,1),Aa(1,1),Bb(1,1), Tol(1,1);
     math_MyFunctionSetWithDerivatives Ff(F);
     V(1)=Guess;
     Tol(1) = Tolerance;
     Aa(1)=A;
     Bb(1)=B;
     math_FunctionSetRoot Sol(Ff,V,Tol,Aa,Bb,NbIterations);
     Done = Sol.IsDone();
     if (Done) {
       F.GetStateNumber();
       TheRoot = Sol.Root()(1);
       TheDerivative = Sol.Derivative()(1,1);
       Ok = F.Value(TheRoot,TheError);
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
