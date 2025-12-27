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
#include <NCollection_CellFilter.hxx>
#include <math_MultipleVarFunction.hxx>
#include <NCollection_Sequence.hxx>

//! This class represents Evtushenko's algorithm of global optimization based on non-uniform mesh.
//! Article: Yu. Evtushenko. Numerical methods for finding global extreme (case of a non-uniform
//! mesh). U.S.S.R. Comput. Maths. Math. Phys., Vol. 11, N 6, pp. 38-54.
//!
//! This method performs search on non-uniform mesh. The search space is a box in R^n space.
//! The default behavior is to find all minimums in that box. Computation of maximums is not
//! supported.
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
                                  const math_Vector&        theLowerBorder,
                                  const math_Vector&        theUpperBorder,
                                  const double       theC                 = 9,
                                  const double       theDiscretizationTol = 1.0e-2,
                                  const double       theSameTol           = 1.0e-7);

  //! @param theFunc - objective functional.
  //! @param theLowerBorder - lower corner of the search box.
  //! @param theUpperBorder - upper corner of the search box.
  //! @param theC - Lipschitz constant.
  //! @param theDiscretizationTol - parameter space discretization tolerance.
  //! @param theSameTol - functional value space indifference tolerance.
  Standard_EXPORT void SetGlobalParams(math_MultipleVarFunction* theFunc,
                                       const math_Vector&        theLowerBorder,
                                       const math_Vector&        theUpperBorder,
                                       const double       theC                 = 9,
                                       const double       theDiscretizationTol = 1.0e-2,
                                       const double       theSameTol           = 1.0e-7);

  //! Method to reduce bounding box. Perform will use this box.
  //! @param theLocalA - lower corner of the local box.
  //! @param theLocalB - upper corner of the local box.
  Standard_EXPORT void SetLocalParams(const math_Vector& theLocalA, const math_Vector& theLocalB);

  //! Method to set tolerances.
  //! @param theDiscretizationTol - parameter space discretization tolerance.
  //! @param theSameTol - functional value space indifference tolerance.
  Standard_EXPORT void SetTol(const double theDiscretizationTol,
                              const double theSameTol);

  //! Method to get tolerances.
  //! @param theDiscretizationTol - parameter space discretization tolerance.
  //! @param theSameTol - functional value space indifference tolerance.
  Standard_EXPORT void GetTol(double& theDiscretizationTol, double& theSameTol);

  //! @param isFindSingleSolution - defines whether to find single solution or all solutions.
  Standard_EXPORT void Perform(const bool isFindSingleSolution = false);

  //! Return solution theIndex, 1 <= theIndex <= NbExtrema.
  Standard_EXPORT void Points(const int theIndex, math_Vector& theSol);

  //! Set / Get continuity of local borders splits (0 ~ C0, 1 ~ C1, 2 ~ C2).
  inline void SetContinuity(const int theCont) { myCont = theCont; }

  inline int GetContinuity() const { return myCont; }

  //! Set / Get functional minimal value.
  inline void SetFunctionalMinimalValue(const double theMinimalValue)
  {
    myFunctionalMinimalValue = theMinimalValue;
  }

  inline double GetFunctionalMinimalValue() const { return myFunctionalMinimalValue; }

  //! Set / Get Lipchitz constant modification state.
  //! True means that the constant is locked and unlocked otherwise.
  inline void SetLipConstState(const bool theFlag) { myIsConstLocked = theFlag; }

  inline bool GetLipConstState() const { return myIsConstLocked; }

  //! Return computation state of the algorithm.
  inline bool isDone() const { return myDone; }

  //! Get best functional value.
  inline double GetF() const { return myF; }

  //! Return count of global extremas.
  inline int NbExtrema() const { return mySolCount; }

private:
  //! Class for duplicate fast search. For internal usage only.
  class NCollection_CellFilter_Inspector
  {
  public:
    //! Points and target type
    typedef math_Vector Point;
    typedef math_Vector Target;

    NCollection_CellFilter_Inspector(const int theDim, const double theTol)
        : myCurrent(1, theDim)
    {
      myTol     = theTol * theTol;
      myIsFind  = false;
      Dimension = theDim;
    }

    //! Access to coordinate.
    static double Coord(int i, const Point& thePnt) { return thePnt(i + 1); }

    //! Auxiliary method to shift point by each coordinate on given value;
    //! useful for preparing a points range for Inspect with tolerance.
    void Shift(const Point&                             thePnt,
               const NCollection_Array1<double>& theTol,
               Point&                                   theLowPnt,
               Point&                                   theUppPnt) const
    {
      for (int anIdx = 1; anIdx <= Dimension; anIdx++)
      {
        theLowPnt(anIdx) = thePnt(anIdx) - theTol(anIdx - 1);
        theUppPnt(anIdx) = thePnt(anIdx) + theTol(anIdx - 1);
      }
    }

    void ClearFind() { myIsFind = false; }

    bool isFind() { return myIsFind; }

    //! Set current point to search for coincidence
    void SetCurrent(const math_Vector& theCurPnt) { myCurrent = theCurPnt; }

    //! Implementation of inspection method
    NCollection_CellFilter_Action Inspect(const Target& theObject)
    {
      double aSqDist = (myCurrent - theObject).Norm2();

      if (aSqDist < myTol)
      {
        myIsFind = true;
      }

      return CellFilter_Keep;
    }

  private:
    double    myTol;
    math_Vector      myCurrent;
    bool myIsFind;
    int Dimension;
  };

  // Compute cell size.
  void initCellSize();

  // Compute initial solution
  void ComputeInitSol();

  math_GlobOptMin& operator=(const math_GlobOptMin& theOther);

  bool computeLocalExtremum(const math_Vector& thePnt,
                                        double&     theVal,
                                        math_Vector&       theOutPnt);

  void computeGlobalExtremum(int theIndex);

  //! Check possibility to stop computations.
  //! Find single solution + in neighborhood of best possible solution.
  bool CheckFunctionalStopCriteria();

  //! Computes starting value / approximation:
  //! myF - initial best value.
  //! myY - initial best point.
  //! myC - approximation of Lipschitz constant.
  //! to improve convergence speed.
  void computeInitialValues();

  //! Check that myA <= thePnt <= myB
  bool isInside(const math_Vector& thePnt);

  //! Check presence of thePnt in GlobOpt sequence.
  bool isStored(const math_Vector& thePnt);

  //! Check and add candidate point into solutions.
  //! @param thePnt   Candidate point.
  //! @param theValue Function value in the candidate point.
  void checkAddCandidate(const math_Vector& thePnt, const double theValue);

  // Input.
  math_MultipleVarFunction* myFunc;
  int          myN;
  math_Vector               myA;       // Left border on current C2 interval.
  math_Vector               myB;       // Right border on current C2 interval.
  math_Vector               myGlobA;   // Global left border.
  math_Vector               myGlobB;   // Global right border.
  double             myTol;     // Discretization tolerance, default 1.0e-2.
  double             mySameTol; // points with ||p1 - p2|| < mySameTol is equal,
                                       // function values |val1 - val2| * 0.01 < mySameTol is equal,
                                       // default value is 1.0e-7.
  double    myC;                // Lipchitz constant, default 9
  double    myInitC;            // Lipchitz constant initial value.
  bool myIsFindSingleSolution;   // Default value is false.
  double    myFunctionalMinimalValue; // Default value is -Precision::Infinite
  bool myIsConstLocked;          // Is constant locked for modifications.

  // Output.
  bool                    myDone;
  NCollection_Sequence<double> myY;        // Current solutions.
  int                    mySolCount; // Count of solutions.

  // Algorithm data.
  double myZ;
  double myE1{}; // Border coefficient.
  double myE2{}; // Minimum step size.
  double myE3{}; // Local extrema starting parameter.

  math_Vector   myX;          // Current modified solution.
  math_Vector   myTmp;        // Current modified solution.
  math_Vector   myV;          // Steps array.
  math_Vector   myMaxV;       // Max Steps array.
  double myLastStep{}; // Last step.

  NCollection_Array1<double>                        myCellSize;
  int                                         myMinCellFilterSol;
  bool                                         isFirstCellFilterInvoke{};
  NCollection_CellFilter<NCollection_CellFilter_Inspector> myFilter;

  // Continuity of local borders.
  int myCont;

  double myF; // Current value of Global optimum.
};

#endif
