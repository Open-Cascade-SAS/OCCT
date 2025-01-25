// Created on: 1991-05-14
// Created by: Laurent PAINNOT
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _math_FunctionSetRoot_HeaderFile
#define _math_FunctionSetRoot_HeaderFile

#include <math_IntegerVector.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>
#include <Standard_OStream.hxx>
#include <Standard_DimensionError.hxx>
#include <StdFail_NotDone.hxx>

class math_FunctionSetWithDerivatives;

//! The math_FunctionSetRoot class calculates the root
//! of a set of N functions of M variables (N<M, N=M or N>M). Knowing
//! an initial guess of the solution and using a minimization algorithm, a search
//! is made in the Newton direction and then in the Gradient direction if there
//! is no success in the Newton direction. This algorithm can also be
//! used for functions minimization. Knowledge of all the partial
//! derivatives (the Jacobian) is required.
class math_FunctionSetRoot
{
public:
  DEFINE_STANDARD_ALLOC

  //! is used in a sub-class to initialize correctly all the fields
  //! of this class.
  //! The range (1, F.NbVariables()) must be especially
  //! respected for all vectors and matrix declarations.
  Standard_EXPORT math_FunctionSetRoot(math_FunctionSetWithDerivatives& F,
                                       const math_Vector&               Tolerance,
                                       const Standard_Integer           NbIterations = 100);

  //! is used in a sub-class to initialize correctly all the fields
  //! of this class.
  //! The range (1, F.NbVariables()) must be especially
  //! respected for all vectors and matrix declarations.
  //! The method SetTolerance must be called after this
  //! constructor.
  Standard_EXPORT math_FunctionSetRoot(math_FunctionSetWithDerivatives& F,
                                       const Standard_Integer           NbIterations = 100);

  //! Destructor
  Standard_EXPORT virtual ~math_FunctionSetRoot();

  //! Initializes the tolerance values.
  Standard_EXPORT void SetTolerance(const math_Vector& Tolerance);

  //! This routine is called at the end of each iteration
  //! to check if the solution was found. It can be redefined
  //! in a sub-class to implement a specific test to stop the iterations.
  //! In this case, the solution is found when: abs(Xi - Xi-1) <= Tolerance
  //! for all unknowns.
  virtual Standard_Boolean IsSolutionReached(math_FunctionSetWithDerivatives&)
  {
    for (Standard_Integer i = 1; i <= Sol.Length(); ++i)
    {
      if (Abs(Delta(i)) > Tol(i))
      {
        return Standard_False;
      }
    }
    return Standard_True;
  }

  //! Improves the root of function from the initial guess point.
  //! The infinum and supremum may be given to constrain the solution.
  //! In this case, the solution is found when: abs(Xi - Xi-1)(j) <= Tolerance(j)
  //! for all unknowns.
  Standard_EXPORT void Perform(math_FunctionSetWithDerivatives& theFunction,
                               const math_Vector&               theStartingPoint,
                               const Standard_Boolean theStopOnDivergent = Standard_False);

  //! Improves the root of function from the initial guess point.
  //! The infinum and supremum may be given to constrain the solution.
  //! In this case, the solution is found when: abs(Xi - Xi-1) <= Tolerance
  //! for all unknowns.
  Standard_EXPORT void Perform(math_FunctionSetWithDerivatives& theFunction,
                               const math_Vector&               theStartingPoint,
                               const math_Vector&               theInfBound,
                               const math_Vector&               theSupBound,
                               const Standard_Boolean theStopOnDivergent = Standard_False);

  //! Returns true if the computations are successful, otherwise returns false.
  Standard_Boolean IsDone() const { return Done; }

  //! Returns the number of iterations really done
  //! during the computation of the root.
  //! Exception NotDone is raised if the root was not found.
  Standard_Integer NbIterations() const
  {
    StdFail_NotDone_Raise_if(!Done, " ");
    return Kount;
  }

  //! returns the stateNumber (as returned by
  //! F.GetStateNumber()) associated to the root found.
  Standard_Integer StateNumber() const
  {
    StdFail_NotDone_Raise_if(!Done, " ");
    return State;
  }

  //! Returns the value of the root of function F.
  //! Exception NotDone is raised if the root was not found.
  const math_Vector& Root() const
  {
    StdFail_NotDone_Raise_if(!Done, " ");
    return Sol;
  }

  //! Outputs the root vector in Root.
  //! Exception NotDone is raised if the root was not found.
  //! Exception DimensionError is raised if the range of Root
  //! is not equal to the range of the StartingPoint.
  Standard_EXPORT void Root(math_Vector& Root) const;

  //! Returns the matrix value of the derivative at the root.
  //! Exception NotDone is raised if the root was not found.
  const math_Matrix& Derivative() const
  {
    StdFail_NotDone_Raise_if(!Done, " ");
    return DF;
  }

  //! outputs the matrix value of the derivative
  //! at the root in Der.
  //! Exception NotDone is raised if the root was not found.
  //! Exception DimensionError is raised if the column range
  //! of <Der> is not equal to the range of the startingPoint.
  void Derivative(math_Matrix& Der) const
  {
    StdFail_NotDone_Raise_if(!Done, " ");
    Standard_DimensionError_Raise_if(Der.ColNumber() != Sol.Length(), " ");
    Der = DF;
  }

  //! returns the vector value of the error done
  //! on the functions at the root.
  //! Exception NotDone is raised if the root was not found.
  const math_Vector& FunctionSetErrors() const
  {
    StdFail_NotDone_Raise_if(!Done, " ");
    return Delta;
  }

  //! outputs the vector value of the error done
  //! on the functions at the root in Err.
  //! Exception NotDone is raised if the root was not found.
  //! Exception DimensionError is raised if the range of Err
  //! is not equal to the range of the StartingPoint.
  Standard_EXPORT void FunctionSetErrors(math_Vector& Err) const;

  //! Prints on the stream o information on the current state
  //! of the object.
  //! Is used to redefine the operator <<.
  Standard_EXPORT void Dump(Standard_OStream& o) const;

  Standard_Boolean IsDivergent() const { return myIsDivergent; }

protected:
  math_Vector Delta;
  math_Vector Sol;
  math_Matrix DF;
  math_Vector Tol;

private:
  Standard_Boolean   Done;
  Standard_Integer   Kount;
  Standard_Integer   State;
  Standard_Integer   Itermax;
  math_Vector        InfBound;
  math_Vector        SupBound;
  math_Vector        SolSave;
  math_Vector        GH;
  math_Vector        DH;
  math_Vector        DHSave;
  math_Vector        FF;
  math_Vector        PreviousSolution;
  math_Vector        Save;
  math_IntegerVector Constraints;
  math_Vector        Temp1;
  math_Vector        Temp2;
  math_Vector        Temp3;
  math_Vector        Temp4;
  Standard_Boolean   myIsDivergent;
};

inline Standard_OStream& operator<<(Standard_OStream& theStream, const math_FunctionSetRoot& theF)
{
  theF.Dump(theStream);
  return theStream;
}

#endif // _math_FunctionSetRoot_HeaderFile
