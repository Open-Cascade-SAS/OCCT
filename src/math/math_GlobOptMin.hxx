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

//! This class represents Evtushenko's algorithm of global optimization based on non-uniform mesh.
//! Article: Yu. Evtushenko. Numerical methods for finding global extreme (case of a non-uniform mesh).
//! U.S.S.R. Comput. Maths. Math. Phys., Vol. 11, N 6, pp. 38-54.
//!
//! This method performs search on non-uniform mesh. The search space is a box in R^n space.
//! The default behavior is to find all minimums in that box. Computation of maximums is not supported.
//!
//! The search box can be split into smaller boxes by discontinuity criteria.
//! This functionality is covered by SetGlobalParams and SetLocalParams API.
//!
//! It is possible to set continuity of the local boxes.
//! Such option can forcibly change local extrema search.
//! In other words if theFunc can be casted to the function with Hessian but, continuity is set to 1
//! Gradient based local optimization method will be used, not Hessian based method.
//! This functionality is covered by SetContinuity and GetContinuity API.
//!
//! It is possible to freeze Lipschitz const to avoid internal modifications on it.
//! This functionality is covered by SetLipConstState and GetLipConstState API.
//!
//! It is possible to perform single solution search.
//! This functionality is covered by first parameter in Perform method.
//!
//! It is possible to set / get minimal value of the functional.
//! It works well together with single solution search.
//! This functionality is covered by SetFunctionalMinimalValue and GetFunctionalMinimalValue API.
class math_GlobOptMin
{
public:

  //! Constructor. Perform method is not called from it.
  //! @param theFunc - objective functional.
  //! @param theLowerBorder - lower corner of the search box.
  //! @param theUpperBorder - upper corner of the search box.
  //! @param theC - Lipschitz constant.
  //! @param theDiscretizationTol - parameter space discretization tolerance.
  //! @param theSameTol - functional value space indifference tolerance.
  Standard_EXPORT math_GlobOptMin(math_MultipleVarFunction* theFunc,
                                 const math_Vector& theLowerBorder,
                                 const math_Vector& theUpperBorder,
                                 const Standard_Real theC = 9,
                                 const Standard_Real theDiscretizationTol = 1.0e-2,
                                 const Standard_Real theSameTol = 1.0e-7);

  //! @param theFunc - objective functional.
  //! @param theLowerBorder - lower corner of the search box.
  //! @param theUpperBorder - upper corner of the search box.
  //! @param theC - Lipschitz constant.
  //! @param theDiscretizationTol - parameter space discretization tolerance.
  //! @param theSameTol - functional value space indifference tolerance.
  Standard_EXPORT void SetGlobalParams(math_MultipleVarFunction* theFunc,
                                       const math_Vector& theLowerBorder,
                                       const math_Vector& theUpperBorder,
                                       const Standard_Real theC = 9,
                                       const Standard_Real theDiscretizationTol = 1.0e-2,
                                       const Standard_Real theSameTol = 1.0e-7);

  //! Method to reduce bounding box. Perform will use this box.
  //! @param theLocalA - lower corner of the local box.
  //! @param theLocalB - upper corner of the local box.
  Standard_EXPORT void SetLocalParams(const math_Vector& theLocalA,
                                      const math_Vector& theLocalB);

  //! Method to set tolerances.
  //! @param theDiscretizationTol - parameter space discretization tolerance.
  //! @param theSameTol - functional value space indifference tolerance.
  Standard_EXPORT void SetTol(const Standard_Real theDiscretizationTol,
                              const Standard_Real theSameTol);

  //! Method to get tolerances.
  //! @param theDiscretizationTol - parameter space discretization tolerance.
  //! @param theSameTol - functional value space indifference tolerance.
  Standard_EXPORT void GetTol(Standard_Real& theDiscretizationTol,
                              Standard_Real& theSameTol);

  //! @param isFindSingleSolution - defines whether to find single solution or all solutions.
  Standard_EXPORT void Perform(const Standard_Boolean isFindSingleSolution = Standard_False);

  //! Return solution theIndex, 1 <= theIndex <= NbExtrema.
  Standard_EXPORT void Points(const Standard_Integer theIndex, math_Vector& theSol);

  //! Set / Get continuity of local borders splits (0 ~ C0, 1 ~ C1, 2 ~ C2).
  inline void SetContinuity(const Standard_Integer theCont) { myCont = theCont; }
  inline Standard_Integer GetContinuity() const {return myCont; }

    //! Set / Get functional minimal value.
  inline void SetFunctionalMinimalValue(const Standard_Real theMinimalValue)
  { myFunctionalMinimalValue = theMinimalValue; }
  inline Standard_Real GetFunctionalMinimalValue() const {return myFunctionalMinimalValue;}

  //! Set / Get Lipchitz constant modification state. 
  //! True means that the constant is locked and unlocked otherwise.
  inline void SetLipConstState(const Standard_Boolean theFlag) {myIsConstLocked = theFlag; }
  inline Standard_Boolean GetLipConstState() const { return myIsConstLocked; }

  //! Return computation state of the algorithm.
  inline Standard_Boolean isDone() const {return myDone; }

  //! Get best functional value.
  inline Standard_Real GetF() const {return myF;}

  //! Return count of global extremas.
  inline Standard_Integer NbExtrema() const {return mySolCount;}

private:

  //! Class for duplicate fast search. For internal usage only.
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

    //! Access to coordinate.
    static Standard_Real Coord (int i, const Point &thePnt)
    {
      return thePnt(i + 1);
    }

    //! Auxiliary method to shift point by each coordinate on given value;
    //! useful for preparing a points range for Inspect with tolerance.
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


  // Compute cell size.
  void initCellSize();

  // Compute initial solution
  void ComputeInitSol();

  math_GlobOptMin & operator = (const math_GlobOptMin & theOther);

  Standard_Boolean computeLocalExtremum(const math_Vector& thePnt, Standard_Real& theVal, math_Vector& theOutPnt);

  void computeGlobalExtremum(Standard_Integer theIndex);

  //! Check possibility to stop computations.
  //! Find single solution + in neighborhood of best possible solution.
  Standard_Boolean CheckFunctionalStopCriteria();

  //! Computes starting value / approximation:
  //! myF - initial best value.
  //! myY - initial best point.
  //! myC - approximation of Lipschitz constant.
  //! to improve convergence speed.
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
  Standard_Real myC; //Lipchitz constant, default 9
  Standard_Real myInitC; // Lipchitz constant initial value.
  Standard_Boolean myIsFindSingleSolution; // Default value is false.
  Standard_Real myFunctionalMinimalValue; // Default value is -Precision::Infinite
  Standard_Boolean myIsConstLocked;  // Is constant locked for modifications.

  // Output.
  Standard_Boolean myDone;
  NCollection_Sequence<Standard_Real> myY;// Current solutions.
  Standard_Integer mySolCount; // Count of solutions.

  // Algorithm data.
  Standard_Real myZ;
  Standard_Real myE1; // Border coefficient.
  Standard_Real myE2; // Minimum step size.
  Standard_Real myE3; // Local extrema starting parameter.

  math_Vector myX; // Current modified solution.
  math_Vector myTmp; // Current modified solution.
  math_Vector myV; // Steps array.
  math_Vector myMaxV; // Max Steps array.
  Standard_Real myLastStep; // Last step.

  NCollection_Array1<Standard_Real> myCellSize;
  Standard_Integer myMinCellFilterSol;
  Standard_Boolean isFirstCellFilterInvoke;
  NCollection_CellFilter<NCollection_CellFilter_Inspector> myFilter;

  // Continuity of local borders.
  Standard_Integer myCont;

  Standard_Real myF; // Current value of Global optimum.
};

#endif
