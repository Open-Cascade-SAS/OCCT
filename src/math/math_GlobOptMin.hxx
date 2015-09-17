// Created on: 2014-01-20
// Created by: Alexaner Malyshev
// Copyright (c) 2014-2015 OPEN CASCADE SAS
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

#ifndef _math_GlobOptMin_HeaderFile
#define _math_GlobOptMin_HeaderFile

#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_CellFilter.hxx>
#include <math_MultipleVarFunction.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_Type.hxx>

class NCollection_CellFilter_Inspector
{
public:

  //! Points and target type
  typedef math_Vector Point;
  typedef math_Vector Target;

  NCollection_CellFilter_Inspector(const Standard_Integer theDim,
                                   const Standard_Real theTol)
  : myCurrent(1, theDim)
  {
    myTol = theTol * theTol;
    myIsFind = Standard_False;
    Dimension = theDim;
  }

  //! Access to co-ordinate
  static Standard_Real Coord (int i, const Point &thePnt)
  {
    return thePnt(i + 1);
  }

  //! Auxiliary method to shift point by each coordinate on given value;
  //! useful for preparing a points range for Inspect with tolerance
  void Shift (const Point& thePnt,
              const NCollection_Array1<Standard_Real> &theTol,
              Point& theLowPnt,
              Point& theUppPnt) const
  {
    for(Standard_Integer anIdx = 1; anIdx <= Dimension; anIdx++)
    {
      theLowPnt(anIdx) = thePnt(anIdx) - theTol(anIdx - 1);
      theUppPnt(anIdx) = thePnt(anIdx) + theTol(anIdx - 1);
    }
  }

  void ClearFind()
  {
    myIsFind = Standard_False;
  }

  Standard_Boolean isFind()
  {
    return myIsFind;
  }

  //! Set current point to search for coincidence
  void SetCurrent (const math_Vector& theCurPnt)
  { 
    myCurrent = theCurPnt;
  }

  //! Implementation of inspection method
  NCollection_CellFilter_Action Inspect (const Target& theObject)
  {
    Standard_Real aSqDist = (myCurrent - theObject).Norm2();

    if(aSqDist < myTol)
    {
      myIsFind = Standard_True;
    }

    return CellFilter_Keep;
  }

private:
  Standard_Real myTol;
  math_Vector myCurrent;
  Standard_Boolean myIsFind;
  Standard_Integer Dimension;
};

//! This class represents Evtushenko's algorithm of global optimization based on nonuniform mesh.<br>
//! Article: Yu. Evtushenko. Numerical methods for finding global extreme (case of a non-uniform mesh). <br>
//! U.S.S.R. Comput. Maths. Math. Phys., Vol. 11, N 6, pp. 38-54.

class math_GlobOptMin
{
public:

  Standard_EXPORT math_GlobOptMin(math_MultipleVarFunction* theFunc,
                                 const math_Vector& theLowerBorder,
                                 const math_Vector& theUpperBorder,
                                 const Standard_Real theC = 9,
                                 const Standard_Real theDiscretizationTol = 1.0e-2,
                                 const Standard_Real theSameTol = 1.0e-7);

  Standard_EXPORT void SetGlobalParams(math_MultipleVarFunction* theFunc,
                                       const math_Vector& theLowerBorder,
                                       const math_Vector& theUpperBorder,
                                       const Standard_Real theC = 9,
                                       const Standard_Real theDiscretizationTol = 1.0e-2,
                                       const Standard_Real theSameTol = 1.0e-7);

  Standard_EXPORT void SetLocalParams(const math_Vector& theLocalA,
                                      const math_Vector& theLocalB);

  Standard_EXPORT void SetTol(const Standard_Real theDiscretizationTol,
                              const Standard_Real theSameTol);

  Standard_EXPORT void GetTol(Standard_Real& theDiscretizationTol,
                              Standard_Real& theSameTol);

  Standard_EXPORT ~math_GlobOptMin();

  //! @param isFindSingleSolution - defines whether to find single solution or all solutions.
  Standard_EXPORT void Perform(const Standard_Boolean isFindSingleSolution = Standard_False);

  //! Get best functional value.
  Standard_EXPORT Standard_Real GetF();

  //! Return count of global extremas.
  Standard_EXPORT Standard_Integer NbExtrema();

  //! Return solution theIndex, 1 <= theIndex <= NbExtrema.
  Standard_EXPORT void Points(const Standard_Integer theIndex, math_Vector& theSol);

  Standard_EXPORT Standard_Boolean isDone();

  //! Set functional minimal value.
  Standard_EXPORT void SetFunctionalMinimalValue(const Standard_Real theMinimalValue);

  //! Get functional minimal value.
  Standard_EXPORT Standard_Real GetFunctionalMinimalValue();

private:

  // Compute cell size.
  void initCellSize();

  math_GlobOptMin & operator = (const math_GlobOptMin & theOther);

  Standard_Boolean computeLocalExtremum(const math_Vector& thePnt, Standard_Real& theVal, math_Vector& theOutPnt);

  void computeGlobalExtremum(Standard_Integer theIndex);

  //! Check possibility to stop computations.
  //! Find single solution + in neighbourhood of best possible solution.
  Standard_Boolean CheckFunctionalStopCriteria();

  //! Computes starting value / approximation:
  //! myF - initial best value.
  //! myY - initial best point.
  //! myC - approximation of Lipschitz constant.
  //! to imporve convergence speed.
  void computeInitialValues();

  //! Check that myA <= thePnt <= myB
  Standard_Boolean isInside(const math_Vector& thePnt);

  //! Check presence of thePnt in GlobOpt sequence.
  Standard_Boolean isStored(const math_Vector &thePnt);

  // Input.
  math_MultipleVarFunction* myFunc;
  Standard_Integer myN;
  math_Vector myA; // Left border on current C2 interval.
  math_Vector myB; // Right border on current C2 interval.
  math_Vector myGlobA; // Global left border.
  math_Vector myGlobB; // Global right border.
  Standard_Real myTol; // Discretization tolerance, default 1.0e-2.
  Standard_Real mySameTol; // points with ||p1 - p2|| < mySameTol is equal,
                           // function values |val1 - val2| * 0.01 < mySameTol is equal,
                           // default value is 1.0e-7.
  Standard_Real myC; //Lipschitz constant, default 9
  Standard_Boolean myIsFindSingleSolution; // Default value is false.
  Standard_Real myFunctionalMinimalValue; // Default value is -Precision::Infinite

  // Output.
  Standard_Boolean myDone;
  NCollection_Sequence<Standard_Real> myY;// Current solutions.
  Standard_Integer mySolCount; // Count of solutions.

  // Algorithm data.
  Standard_Real myZ;
  Standard_Real myE1; // Border coeff.
  Standard_Real myE2; // Minimum step size.
  Standard_Real myE3; // Local extrema starting parameter.

  math_Vector myX; // Current modified solution.
  math_Vector myTmp; // Current modified solution.
  math_Vector myV; // Steps array.
  math_Vector myMaxV; // Max Steps array.
  math_Vector myExpandCoeff; // Define expand coefficient between neighboring indiced dimensions.

  NCollection_Array1<Standard_Real> myCellSize;
  Standard_Integer myMinCellFilterSol;
  Standard_Boolean isFirstCellFilterInvoke;
  NCollection_CellFilter<NCollection_CellFilter_Inspector> myFilter;

  Standard_Real myF; // Current value of Global optimum.
};

#endif
