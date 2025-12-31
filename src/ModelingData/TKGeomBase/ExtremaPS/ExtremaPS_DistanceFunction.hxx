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

#ifndef _ExtremaPS_DistanceFunction_HeaderFile
#define _ExtremaPS_DistanceFunction_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <MathSys_Newton2D.hxx>
#include <Standard_DefineAlloc.hxx>

//! @brief 2D distance function for point-surface extrema.
//!
//! Computes the gradient of squared distance from a point to a surface:
//!   D(u,v) = ||S(u,v) - P||^2
//!
//! The gradient is:
//!   F_u(u,v) = 2 * (S(u,v) - P) . dS/du
//!   F_v(u,v) = 2 * (S(u,v) - P) . dS/dv
//!
//! Extrema occur where F_u = F_v = 0.
//!
//! This class provides evaluation of F and its Jacobian for 2D Newton iteration.
class ExtremaPS_DistanceFunction
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor.
  //! @param theSurface surface adaptor
  //! @param theP query point
  ExtremaPS_DistanceFunction(const Adaptor3d_Surface& theSurface, const gp_Pnt& theP)
      : mySurface(&theSurface),
        myP(theP)
  {
  }

  //! Evaluate function values F_u and F_v at (u, v).
  //! @param theU U parameter
  //! @param theV V parameter
  //! @param theFu output: gradient component in U direction
  //! @param theFv output: gradient component in V direction
  //! @return true if evaluation succeeded
  bool Value(double theU, double theV, double& theFu, double& theFv) const
  {
    gp_Pnt aPt;
    gp_Vec aDU, aDV;
    mySurface->D1(theU, theV, aPt, aDU, aDV);

    gp_Vec aVec(myP, aPt);
    theFu = aVec.Dot(aDU);
    theFv = aVec.Dot(aDV);

    return true;
  }

  //! Evaluate function and Jacobian at (u, v).
  //! @param theU U parameter
  //! @param theV V parameter
  //! @param theFu output: gradient component in U
  //! @param theFv output: gradient component in V
  //! @param theDFuu output: d(Fu)/dU
  //! @param theDFuv output: d(Fu)/dV = d(Fv)/dU
  //! @param theDFvv output: d(Fv)/dV
  //! @return true if evaluation succeeded
  bool ValueAndJacobian(double  theU,
                        double  theV,
                        double& theFu,
                        double& theFv,
                        double& theDFuu,
                        double& theDFuv,
                        double& theDFvv) const
  {
    gp_Pnt aPt;
    gp_Vec aDU, aDV, aD2U, aD2V, aD2UV;
    mySurface->D2(theU, theV, aPt, aDU, aDV, aD2U, aD2V, aD2UV);

    gp_Vec aVec(myP, aPt);

    // Function values
    theFu = aVec.Dot(aDU);
    theFv = aVec.Dot(aDV);

    // Jacobian components
    // d(Fu)/dU = (dS/dU . dS/dU) + (S - P) . d2S/dU2
    theDFuu = aDU.Dot(aDU) + aVec.Dot(aD2U);

    // d(Fu)/dV = d(Fv)/dU = (dS/dU . dS/dV) + (S - P) . d2S/dUdV
    theDFuv = aDU.Dot(aDV) + aVec.Dot(aD2UV);

    // d(Fv)/dV = (dS/dV . dS/dV) + (S - P) . d2S/dV2
    theDFvv = aDV.Dot(aDV) + aVec.Dot(aD2V);

    return true;
  }

  //! Compute squared distance at (u, v).
  double SquareDistance(double theU, double theV) const
  {
    gp_Pnt aPt = mySurface->Value(theU, theV);
    return myP.SquareDistance(aPt);
  }

  //! Returns the query point.
  const gp_Pnt& Point() const { return myP; }

  //! Sets the query point.
  void SetPoint(const gp_Pnt& theP) { myP = theP; }

private:
  const Adaptor3d_Surface* mySurface; //!< Surface adaptor
  gp_Pnt                   myP;       //!< Query point
};

//! @brief 2D Newton solver for point-surface extrema.
//!
//! Solves the system [Fu, Fv] = [0, 0] using Newton-Raphson iteration.
//! Uses MathSys::Newton2DSymmetric for the core algorithm.
namespace ExtremaPS_Newton
{

//! Result of Newton iteration (wrapper around MathSys::Newton2DResult).
struct Result
{
  bool   IsDone = false; //!< True if converged
  double U      = 0.0;   //!< Solution U parameter
  double V      = 0.0;   //!< Solution V parameter
  int    NbIter = 0;     //!< Number of iterations performed
  double FNorm  = 0.0;   //!< Final |F| norm
};

//! Perform 2D Newton iteration.
//! @param theFunc distance function
//! @param theU0 initial U guess
//! @param theV0 initial V guess
//! @param theUMin U lower bound
//! @param theUMax U upper bound
//! @param theVMin V lower bound
//! @param theVMax V upper bound
//! @param theTol tolerance for convergence
//! @param theMaxIter maximum iterations
//! @return Newton result
inline Result Solve(const ExtremaPS_DistanceFunction& theFunc,
                    double                            theU0,
                    double                            theV0,
                    double                            theUMin,
                    double                            theUMax,
                    double                            theVMin,
                    double                            theVMax,
                    double                            theTol,
                    int                               theMaxIter = 12)
{
  // Use MathSys::Newton2DSymmetric for the core algorithm
  auto aMathResult = MathSys::Newton2DSymmetric(theFunc,
                                                theU0,
                                                theV0,
                                                theUMin,
                                                theUMax,
                                                theVMin,
                                                theVMax,
                                                theTol,
                                                static_cast<size_t>(theMaxIter));

  // Convert to local Result type
  Result aRes;
  aRes.IsDone = aMathResult.IsDone();
  aRes.U      = aMathResult.U;
  aRes.V      = aMathResult.V;
  aRes.NbIter = static_cast<int>(aMathResult.NbIter);
  aRes.FNorm  = aMathResult.FNorm;

  return aRes;
}

} // namespace ExtremaPS_Newton

#endif // _ExtremaPS_DistanceFunction_HeaderFile
