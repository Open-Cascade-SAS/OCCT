// Created on: 1992-02-14
// Created by: Jean Claude VAUTHIER
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _GProp_PGProps_HeaderFile
#define _GProp_PGProps_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GProp_GProps.hxx>
#include <Standard_Real.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array2OfReal.hxx>
class Standard_DimensionError;
class Standard_DomainError;
class gp_Pnt;


//! A framework for computing the global properties of a
//! set of points.
//! A point mass is attached to each point. The global
//! mass of the system is the sum of each individual
//! mass. By default, the point mass is equal to 1 and the
//! mass of a system composed of N points is equal to N.
//! Warning
//! A framework of this sort provides functions to handle
//! sets of points easily. But, like any GProp_GProps
//! object, by using the Add function, it can theoretically
//! bring together the computed global properties and
//! those of a system more complex than a set of points .
//! The mass of each point and the density of each
//! component of the composed system must be
//! coherent. Note that this coherence cannot be checked.
//! Nonetheless, you are advised to restrict your use of a
//! GProp_PGProps object to a set of points and to
//! create a GProp_GProps object in order to bring
//! together global properties of different systems.
class GProp_PGProps  : public GProp_GProps
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Initializes a framework to compute global properties
  //! on a set of points.
  //! The point relative to which the inertia of the system is
  //! computed will be the origin (0, 0, 0) of the
  //! absolute Cartesian coordinate system.
  //! At initialization, the framework is empty, i.e. it retains
  //! no dimensional information such as mass and inertia.
  //! It is, however, now able to keep global properties of a
  //! set of points while new points are added using the
  //! AddPoint function.
  //! The set of points whose global properties are brought
  //! together by this framework will then be referred to as
  //! the current system. The current system is, however,
  //! not kept by this framework, which only keeps that
  //! system's global properties. Note that the current
  //! system may be more complex than a set of points.
  Standard_EXPORT GProp_PGProps();
  
  //! Brings together the global properties already
  //! retained by this framework with those induced by
  //! the point Pnt. Pnt may be the first point of the current system.
  //! A point mass is attached to the point Pnt, it is either
  //! equal to 1. or to Density.
  Standard_EXPORT void AddPoint (const gp_Pnt& P);
  

  //! Adds a new point P with its density in the system of points
  //! Exceptions
  //! Standard_DomainError if the mass value Density
  //! is less than gp::Resolution().
  Standard_EXPORT void AddPoint (const gp_Pnt& P, const Standard_Real Density);
  

  //! computes the global properties of the system of points Pnts.
  //! The density of the points are defaulted to all being 1
  Standard_EXPORT GProp_PGProps(const TColgp_Array1OfPnt& Pnts);
  

  //! computes the global properties of the system of points Pnts.
  //! The density of the points are defaulted to all being 1
  Standard_EXPORT GProp_PGProps(const TColgp_Array2OfPnt& Pnts);
  

  //! computes the global properties of the system of points Pnts.
  //! A density is associated with each point.
  //!
  //! raises if a density is lower or equal to Resolution from package
  //! gp.
  //!
  //! raises if the length of Pnts and the length of Density
  //! is not the same.
  Standard_EXPORT GProp_PGProps(const TColgp_Array1OfPnt& Pnts, const TColStd_Array1OfReal& Density);
  

  //! computes the global properties of the system of points Pnts.
  //! A density is associated with each point.
  //!
  //! Raised if a density is lower or equal to Resolution from package
  //! gp.
  //!
  //! Raised if the length of Pnts and the length of Density
  //! is not the same.
  Standard_EXPORT GProp_PGProps(const TColgp_Array2OfPnt& Pnts, const TColStd_Array2OfReal& Density);
  

  //! Computes the barycentre of a set of points. The density of the
  //! points is defaulted to 1.
  Standard_EXPORT static gp_Pnt Barycentre (const TColgp_Array1OfPnt& Pnts);
  

  //! Computes the barycentre of a set of points. The density of the
  //! points is defaulted to 1.
  Standard_EXPORT static gp_Pnt Barycentre (const TColgp_Array2OfPnt& Pnts);
  

  //! Computes the barycentre of a set of points. A density is associated
  //! with each point.
  //!
  //! raises if a density is lower or equal to Resolution from package
  //! gp.
  //!
  //! Raised if the length of Pnts and the length of Density
  //! is not the same.
  Standard_EXPORT static void Barycentre (const TColgp_Array1OfPnt& Pnts, const TColStd_Array1OfReal& Density, Standard_Real& Mass, gp_Pnt& G);
  

  //! Computes the barycentre of a set of points. A density is associated
  //! with each point.
  //!
  //! Raised if a density is lower or equal to Resolution from package
  //! gp.
  //!
  //! Raised if the length of Pnts and the length of Density
  //! is not the same.
  Standard_EXPORT static void Barycentre (const TColgp_Array2OfPnt& Pnts, const TColStd_Array2OfReal& Density, Standard_Real& Mass, gp_Pnt& G);




protected:





private:





};







#endif // _GProp_PGProps_HeaderFile
