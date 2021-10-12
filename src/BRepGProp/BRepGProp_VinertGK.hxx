// Created on: 2005-12-21
// Created by: Sergey KHROMOV
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#ifndef _BRepGProp_VinertGK_HeaderFile
#define _BRepGProp_VinertGK_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <GProp_GProps.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Address.hxx>
class BRepGProp_Face;
class BRepGProp_Domain;
class gp_Pln;


//! Computes the global properties of a geometric solid
//! (3D closed region of space) delimited with :
//! -  a point and a surface
//! -  a plane and a surface
//!
//! The surface can be :
//! -  a surface limited with its parametric values U-V,
//! (naturally restricted)
//! -  a surface limited in U-V space with its boundary
//! curves.
//!
//! The surface's requirements to evaluate the global
//! properties are defined in the template FaceTool class from
//! the package GProp.
//!
//! The adaptive 2D algorithm of Gauss-Kronrod integration of
//! double integral is used.
//!
//! The inner integral is computed along U parameter of
//! surface. The integrand function is encapsulated in the
//! support class UFunction that is defined below.
//!
//! The outer integral is computed along T parameter of a
//! bounding curve. The integrand function is encapsulated in
//! the support class TFunction that is defined below.
class BRepGProp_VinertGK  : public GProp_GProps
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor.
  BRepGProp_VinertGK()
    : myErrorReached (0.), myAbsolutError (0.)
  {
  }
  
  //! Constructor. Computes the global properties of a region of
  //! 3D space delimited with the naturally restricted surface
  //! and the point VLocation.
  Standard_EXPORT BRepGProp_VinertGK(BRepGProp_Face& theSurface, const gp_Pnt& theLocation, const Standard_Real theTolerance = 0.001, const Standard_Boolean theCGFlag = Standard_False, const Standard_Boolean theIFlag = Standard_False);
  
  //! Constructor. Computes the global properties of a region of
  //! 3D space delimited with the naturally restricted surface
  //! and the point VLocation. The inertia is computed with
  //! respect to thePoint.
  Standard_EXPORT BRepGProp_VinertGK(BRepGProp_Face& theSurface, const gp_Pnt& thePoint, const gp_Pnt& theLocation, const Standard_Real theTolerance = 0.001, const Standard_Boolean theCGFlag = Standard_False, const Standard_Boolean theIFlag = Standard_False);
  
  //! Constructor. Computes the global properties of a region of
  //! 3D space delimited with the surface bounded by the domain
  //! and the point VLocation.
  Standard_EXPORT BRepGProp_VinertGK(BRepGProp_Face& theSurface, BRepGProp_Domain& theDomain, const gp_Pnt& theLocation, const Standard_Real theTolerance = 0.001, const Standard_Boolean theCGFlag = Standard_False, const Standard_Boolean theIFlag = Standard_False);
  
  //! Constructor. Computes the global properties of a region of
  //! 3D space delimited with the surface bounded by the domain
  //! and the point VLocation. The inertia is computed with
  //! respect to thePoint.
  Standard_EXPORT BRepGProp_VinertGK(BRepGProp_Face& theSurface, BRepGProp_Domain& theDomain, const gp_Pnt& thePoint, const gp_Pnt& theLocation, const Standard_Real theTolerance = 0.001, const Standard_Boolean theCGFlag = Standard_False, const Standard_Boolean theIFlag = Standard_False);
  
  //! Constructor. Computes the global properties of a region of
  //! 3D space delimited with the naturally restricted surface
  //! and the plane.
  Standard_EXPORT BRepGProp_VinertGK(BRepGProp_Face& theSurface, const gp_Pln& thePlane, const gp_Pnt& theLocation, const Standard_Real theTolerance = 0.001, const Standard_Boolean theCGFlag = Standard_False, const Standard_Boolean theIFlag = Standard_False);
  
  //! Constructor. Computes the global properties of a region of
  //! 3D space delimited with the surface bounded by the domain
  //! and the plane.
  Standard_EXPORT BRepGProp_VinertGK(BRepGProp_Face& theSurface, BRepGProp_Domain& theDomain, const gp_Pln& thePlane, const gp_Pnt& theLocation, const Standard_Real theTolerance = 0.001, const Standard_Boolean theCGFlag = Standard_False, const Standard_Boolean theIFlag = Standard_False);
  
  //! Sets the vertex that delimit 3D closed region of space.
  void SetLocation (const gp_Pnt& theLocation)
  {
    loc = theLocation;
  }
  
  //! Computes the global properties of a region of 3D space
  //! delimited with the naturally restricted surface and the
  //! point VLocation.
  Standard_EXPORT Standard_Real Perform (BRepGProp_Face& theSurface, const Standard_Real theTolerance = 0.001, const Standard_Boolean theCGFlag = Standard_False, const Standard_Boolean theIFlag = Standard_False);
  
  //! Computes the global properties of a region of 3D space
  //! delimited with the naturally restricted surface and the
  //! point VLocation. The inertia is computed with respect to
  //! thePoint.
  Standard_EXPORT Standard_Real Perform (BRepGProp_Face& theSurface, const gp_Pnt& thePoint, const Standard_Real theTolerance = 0.001, const Standard_Boolean theCGFlag = Standard_False, const Standard_Boolean theIFlag = Standard_False);
  
  //! Computes the global properties of a region of 3D space
  //! delimited with the surface bounded by the domain and the
  //! point VLocation.
  Standard_EXPORT Standard_Real Perform (BRepGProp_Face& theSurface, BRepGProp_Domain& theDomain, const Standard_Real theTolerance = 0.001, const Standard_Boolean theCGFlag = Standard_False, const Standard_Boolean theIFlag = Standard_False);
  
  //! Computes the global properties of a region of 3D space
  //! delimited with the surface bounded by the domain and the
  //! point VLocation. The inertia is computed with respect to
  //! thePoint.
  Standard_EXPORT Standard_Real Perform (BRepGProp_Face& theSurface, BRepGProp_Domain& theDomain, const gp_Pnt& thePoint, const Standard_Real theTolerance = 0.001, const Standard_Boolean theCGFlag = Standard_False, const Standard_Boolean theIFlag = Standard_False);
  
  //! Computes the global properties of a region of 3D space
  //! delimited with the naturally restricted surface and the
  //! plane.
  Standard_EXPORT Standard_Real Perform (BRepGProp_Face& theSurface, const gp_Pln& thePlane, const Standard_Real theTolerance = 0.001, const Standard_Boolean theCGFlag = Standard_False, const Standard_Boolean theIFlag = Standard_False);
  
  //! Computes the global properties of a region of 3D space
  //! delimited with the surface bounded by the domain and the
  //! plane.
  Standard_EXPORT Standard_Real Perform (BRepGProp_Face& theSurface, BRepGProp_Domain& theDomain, const gp_Pln& thePlane, const Standard_Real theTolerance = 0.001, const Standard_Boolean theCGFlag = Standard_False, const Standard_Boolean theIFlag = Standard_False);
  
  //! Returns the relative reached computation error.
  Standard_Real GetErrorReached () const
  {
    return myErrorReached;
  }
  
  //! Returns the absolut reached computation error.
    Standard_Real GetAbsolutError() const;

private:

  //! Main method for computation of the global properties that
  //! is invoked by each Perform method.
  Standard_EXPORT Standard_Real PrivatePerform (BRepGProp_Face& theSurface,
                                                const Standard_Address thePtrDomain,
                                                const Standard_Boolean IsByPoint,
                                                const Standard_Real* theCoeffs,
                                                const Standard_Real theTolerance,
                                                const Standard_Boolean theCGFlag,
                                                const Standard_Boolean theIFlag);


  Standard_Real myErrorReached;
  Standard_Real myAbsolutError;
};


#endif // _BRepGProp_VinertGK_HeaderFile
