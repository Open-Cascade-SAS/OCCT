// Created on: 1991-09-09
// Created by: Michel Chauvat
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

#ifndef _CSLib_HeaderFile
#define _CSLib_HeaderFile

#include <CSLib_DerivativeStatus.hxx>
#include <CSLib_NormalStatus.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <TColgp_Array2OfVec.hxx>

class gp_Dir;
class gp_Vec;

//! Provides functions for basic geometric computation on curves and surfaces.
//!
//! This package implements functions for computing surface normals
//! and their derivatives at parametric points. The tolerance criteria
//! used are Resolution from gp and RealEpsilon from Standard_Real.
//!
//! Key functionality:
//! - Normal computation from surface first derivatives (D1U, D1V)
//! - Approximate normal in singular cases using second derivatives
//! - Derivatives of the non-normalized and normalized normal vectors
class CSLib
{
public:
  DEFINE_STANDARD_ALLOC

  //! Computes the normal direction of a surface as the cross product D1U ^ D1V.
  //!
  //! The normal is undefined if:
  //! - D1U has null length, or
  //! - D1V has null length, or
  //! - D1U and D1V are parallel.
  //!
  //! To check parallelism, the sine of the angle between D1U and D1V
  //! is computed and compared with theSinTol.
  //!
  //! @param[in]  theD1U    First derivative in U direction
  //! @param[in]  theD1V    First derivative in V direction
  //! @param[in]  theSinTol Sine tolerance for parallelism check
  //! @param[out] theStatus Result status indicating success or failure reason
  //! @param[out] theNormal Computed normal direction (valid only if theStatus == CSLib_Done)
  Standard_EXPORT static void Normal(const gp_Vec&           theD1U,
                                     const gp_Vec&           theD1V,
                                     double                  theSinTol,
                                     CSLib_DerivativeStatus& theStatus,
                                     gp_Dir&                 theNormal);

  //! Computes an approximate normal direction at a singular point using second derivatives.
  //!
  //! When the standard method cannot compute the normal (D1U ^ D1V is null or too small),
  //! this method uses a limited Taylor expansion:
  //!   N(u0+du, v0+dv) = N0 + dN/du * du + dN/dv * dv + O(du^2, dv^2)
  //!
  //! The normal is approximated from dN/du and dN/dv where N = D1U ^ D1V.
  //!
  //! @param[in]  theD1U    First derivative in U direction
  //! @param[in]  theD1V    First derivative in V direction
  //! @param[in]  theD2U    Second derivative in U direction (d^2S/du^2)
  //! @param[in]  theD2V    Second derivative in V direction (d^2S/dv^2)
  //! @param[in]  theD2UV   Mixed second derivative (d^2S/dudv)
  //! @param[in]  theSinTol Sine tolerance for parallelism check
  //! @param[out] theDone   True if normal was successfully computed
  //! @param[out] theStatus Result status with detailed information
  //! @param[out] theNormal Computed normal direction (valid only if theDone is true)
  Standard_EXPORT static void Normal(const gp_Vec&       theD1U,
                                     const gp_Vec&       theD1V,
                                     const gp_Vec&       theD2U,
                                     const gp_Vec&       theD2V,
                                     const gp_Vec&       theD2UV,
                                     double              theSinTol,
                                     bool&               theDone,
                                     CSLib_NormalStatus& theStatus,
                                     gp_Dir&             theNormal);

  //! Computes the normal direction using magnitude tolerance.
  //!
  //! A simpler version that checks if the cross product magnitude
  //! and derivative magnitudes exceed the given tolerance.
  //!
  //! @param[in]  theD1U    First derivative in U direction
  //! @param[in]  theD1V    First derivative in V direction
  //! @param[in]  theMagTol Magnitude tolerance for singularity detection
  //! @param[out] theStatus Result status (CSLib_Defined or CSLib_Singular)
  //! @param[out] theNormal Computed normal direction (valid only if theStatus == CSLib_Defined)
  Standard_EXPORT static void Normal(const gp_Vec&       theD1U,
                                     const gp_Vec&       theD1V,
                                     double              theMagTol,
                                     CSLib_NormalStatus& theStatus,
                                     gp_Dir&             theNormal);

  //! Computes the normal at a singular point using higher-order derivatives.
  //!
  //! Finds the first order k0 where the derivatives of N = D1U ^ D1V become non-null
  //! and collinear, ensuring a unique normal direction.
  //!
  //! @param[in]  theMaxOrder Maximum derivative order to examine
  //! @param[in]  theDerNUV   Array of derivatives of N (indices correspond to derivative orders)
  //! @param[in]  theMagTol   Magnitude tolerance
  //! @param[in]  theU, theV  Current parameter values
  //! @param[in]  theUmin, theUmax, theVmin, theVmax  Parameter bounds
  //! @param[out] theStatus   Result status
  //! @param[out] theNormal   Computed normal direction
  //! @param[out] theOrderU, theOrderV  Orders of the first non-null derivative used
  Standard_EXPORT static void Normal(int                       theMaxOrder,
                                     const TColgp_Array2OfVec& theDerNUV,
                                     double                    theMagTol,
                                     double                    theU,
                                     double                    theV,
                                     double                    theUmin,
                                     double                    theUmax,
                                     double                    theVmin,
                                     double                    theVmax,
                                     CSLib_NormalStatus&       theStatus,
                                     gp_Dir&                   theNormal,
                                     int&                      theOrderU,
                                     int&                      theOrderV);

  //! Computes the derivative of order (theNu, theNv) of the non-normalized normal vector.
  //!
  //! The non-normalized normal is N = dS/du ^ dS/dv.
  //! This function computes d^(Nu+Nv)N / (du^Nu * dv^Nv).
  //!
  //! @param[in] theNu      Derivative order in U direction
  //! @param[in] theNv      Derivative order in V direction
  //! @param[in] theDerSurf Surface derivatives array where theDerSurf(i,j) = d^(i+j)S/(du^i * dv^j)
  //!                       for i = 0..theNu+1, j = 0..theNv+1
  //! @return The derivative vector d^(Nu+Nv)N / (du^Nu * dv^Nv)
  Standard_EXPORT static gp_Vec DNNUV(int theNu, int theNv, const TColgp_Array2OfVec& theDerSurf);

  //! Computes the derivative of the non-normalized vector N = dS1/du ^ dS2/dv.
  //!
  //! This variant is used for osculating surfaces where the normal
  //! is computed from derivatives of two different surfaces.
  //!
  //! @param[in] theNu       Derivative order in U direction
  //! @param[in] theNv       Derivative order in V direction
  //! @param[in] theDerSurf1 Derivatives of the first surface S1
  //! @param[in] theDerSurf2 Derivatives of the second surface S2
  //! @return The derivative vector
  Standard_EXPORT static gp_Vec DNNUV(int                       theNu,
                                      int                       theNv,
                                      const TColgp_Array2OfVec& theDerSurf1,
                                      const TColgp_Array2OfVec& theDerSurf2);

  //! Computes the derivative of order (theNu, theNv) of the normalized normal vector.
  //!
  //! @param[in] theNu     Derivative order in U direction
  //! @param[in] theNv     Derivative order in V direction
  //! @param[in] theDerNUV Array of derivatives of the non-normalized normal.
  //!                      Contains derivatives d^(i+j)(D1U^D1V)/(du^i * dv^j)
  //!                      for i = theIduref..theNu+theIduref, j = theIdvref..theNv+theIdvref
  //! @param[in] theIduref Reference index offset in U (default 0 for regular cases)
  //! @param[in] theIdvref Reference index offset in V (default 0 for regular cases)
  //! @return The derivative of the normalized normal vector
  Standard_EXPORT static gp_Vec DNNormal(int                       theNu,
                                         int                       theNv,
                                         const TColgp_Array2OfVec& theDerNUV,
                                         int                       theIduref = 0,
                                         int                       theIdvref = 0);
};

#endif // _CSLib_HeaderFile
