// Copyright (c) 2025 OPEN CASCADE SAS
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

#ifndef _ExtremaCC_DistanceFunction_HeaderFile
#define _ExtremaCC_DistanceFunction_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <MathSys_Newton2D.hxx>
#include <Standard_DefineAlloc.hxx>

//! @brief 2D distance function for curve-curve extrema.
//!
//! Computes the gradient of squared distance between two curves:
//!   D(u1, u2) = ||C1(u1) - C2(u2)||^2
//!
//! The gradient is:
//!   F1(u1, u2) = (C1(u1) - C2(u2)) . C1'(u1) = 0
//!   F2(u1, u2) = (C2(u2) - C1(u1)) . C2'(u2) = 0
//!
//! Extrema occur where F1 = F2 = 0.
//!
//! The Jacobian is symmetric (since F1 and F2 are gradients of the same scalar D):
//!   J11 = dF1/du1 = C1'.C1' + (C1-C2).C1''
//!   J12 = dF1/du2 = dF2/du1 = -C1'.C2'
//!   J22 = dF2/du2 = C2'.C2' + (C2-C1).C2''
//!
//! This class provides evaluation of F and its Jacobian for 2D Newton iteration.
class ExtremaCC_DistanceFunction
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor.
  //! @param theCurve1 first curve adaptor
  //! @param theCurve2 second curve adaptor
  ExtremaCC_DistanceFunction(const Adaptor3d_Curve& theCurve1,
                             const Adaptor3d_Curve& theCurve2)
      : myCurve1(&theCurve1),
        myCurve2(&theCurve2)
  {
  }

  //! Evaluate function values F1 and F2 at (u1, u2).
  //! @param theU1 parameter on first curve
  //! @param theU2 parameter on second curve
  //! @param theF1 output: gradient component in u1 direction
  //! @param theF2 output: gradient component in u2 direction
  //! @return true if evaluation succeeded
  bool Value(double theU1, double theU2, double& theF1, double& theF2) const
  {
    gp_Pnt aPt1;
    gp_Vec aD1_1;
    myCurve1->D1(theU1, aPt1, aD1_1);

    gp_Pnt aPt2;
    gp_Vec aD1_2;
    myCurve2->D1(theU2, aPt2, aD1_2);

    // Vector from C2 to C1
    gp_Vec aVec(aPt2, aPt1);

    // F1 = (C1 - C2) . C1'
    theF1 = aVec.Dot(aD1_1);

    // F2 = (C2 - C1) . C2' = -aVec . C2'
    theF2 = -aVec.Dot(aD1_2);

    return true;
  }

  //! Evaluate function and symmetric Jacobian at (u1, u2).
  //!
  //! The Jacobian is symmetric: J = [[J11, J12], [J12, J22]].
  //!
  //! @param theU1 parameter on first curve
  //! @param theU2 parameter on second curve
  //! @param theF1 output: gradient component in u1
  //! @param theF2 output: gradient component in u2
  //! @param theJ11 output: d(F1)/du1
  //! @param theJ12 output: d(F1)/du2 = d(F2)/du1
  //! @param theJ22 output: d(F2)/du2
  //! @return true if evaluation succeeded
  bool ValueAndJacobian(double  theU1,
                        double  theU2,
                        double& theF1,
                        double& theF2,
                        double& theJ11,
                        double& theJ12,
                        double& theJ22) const
  {
    gp_Pnt aPt1;
    gp_Vec aD1_1, aD2_1;
    myCurve1->D2(theU1, aPt1, aD1_1, aD2_1);

    gp_Pnt aPt2;
    gp_Vec aD1_2, aD2_2;
    myCurve2->D2(theU2, aPt2, aD1_2, aD2_2);

    // Vector from C2 to C1
    gp_Vec aVec12(aPt2, aPt1);
    // Vector from C1 to C2
    gp_Vec aVec21(aPt1, aPt2);

    // Function values
    // F1 = (C1 - C2) . C1'
    theF1 = aVec12.Dot(aD1_1);

    // F2 = (C2 - C1) . C2'
    theF2 = aVec21.Dot(aD1_2);

    // Jacobian components (symmetric)
    // J11 = dF1/du1 = C1'.C1' + (C1-C2).C1''
    theJ11 = aD1_1.Dot(aD1_1) + aVec12.Dot(aD2_1);

    // J12 = dF1/du2 = -C2'.C1' (symmetric with dF2/du1)
    theJ12 = -aD1_2.Dot(aD1_1);

    // J22 = dF2/du2 = C2'.C2' + (C2-C1).C2''
    theJ22 = aD1_2.Dot(aD1_2) + aVec21.Dot(aD2_2);

    return true;
  }

  //! Compute squared distance at (u1, u2).
  //! @param theU1 parameter on first curve
  //! @param theU2 parameter on second curve
  //! @return squared distance between C1(u1) and C2(u2)
  double SquareDistance(double theU1, double theU2) const
  {
    gp_Pnt aPt1 = myCurve1->Value(theU1);
    gp_Pnt aPt2 = myCurve2->Value(theU2);
    return aPt1.SquareDistance(aPt2);
  }

  //! Returns the first curve.
  const Adaptor3d_Curve& Curve1() const { return *myCurve1; }

  //! Returns the second curve.
  const Adaptor3d_Curve& Curve2() const { return *myCurve2; }

private:
  const Adaptor3d_Curve* myCurve1; //!< First curve adaptor (not owned)
  const Adaptor3d_Curve* myCurve2; //!< Second curve adaptor (not owned)
};

//! @brief 2D Newton solver for curve-curve extrema.
//!
//! Solves the system [F1, F2] = [0, 0] using Newton-Raphson iteration.
//! Uses MathSys::Newton2DSymmetric for the core algorithm (Jacobian is symmetric).
namespace ExtremaCC_Newton
{

//! Result of Newton iteration.
struct Result
{
  bool   IsDone = false; //!< True if converged
  double U1     = 0.0;   //!< Solution parameter on first curve
  double U2     = 0.0;   //!< Solution parameter on second curve
  int    NbIter = 0;     //!< Number of iterations performed
  double FNorm  = 0.0;   //!< Final |F| norm
};

//! Perform 2D Newton iteration for curve-curve extrema.
//! @param theFunc distance function
//! @param theU1_0 initial guess on first curve
//! @param theU2_0 initial guess on second curve
//! @param theU1Min lower bound for first curve
//! @param theU1Max upper bound for first curve
//! @param theU2Min lower bound for second curve
//! @param theU2Max upper bound for second curve
//! @param theTol tolerance for convergence
//! @param theMaxIter maximum iterations
//! @return Newton result
inline Result Solve(const ExtremaCC_DistanceFunction& theFunc,
                    double                            theU1_0,
                    double                            theU2_0,
                    double                            theU1Min,
                    double                            theU1Max,
                    double                            theU2Min,
                    double                            theU2Max,
                    double                            theTol,
                    int                               theMaxIter = 20)
{
  // Use MathSys::Newton2DSymmetric for the core algorithm
  auto aMathResult = MathSys::Newton2DSymmetric(theFunc,
                                                theU1_0,
                                                theU2_0,
                                                theU1Min,
                                                theU1Max,
                                                theU2Min,
                                                theU2Max,
                                                theTol,
                                                static_cast<size_t>(theMaxIter));

  // Convert to local Result type
  Result aRes;
  aRes.IsDone = aMathResult.IsDone();
  aRes.U1     = aMathResult.U;
  aRes.U2     = aMathResult.V;
  aRes.NbIter = static_cast<int>(aMathResult.NbIter);
  aRes.FNorm  = aMathResult.FNorm;

  return aRes;
}

} // namespace ExtremaCC_Newton

#endif // _ExtremaCC_DistanceFunction_HeaderFile
