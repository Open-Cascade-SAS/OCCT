// Created on: 1992-08-24
// Created by: Michel CHAUVAT
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

#ifndef _GProp_GProps_HeaderFile
#define _GProp_GProps_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Pnt.hxx>
#include <Standard_Real.hxx>
#include <gp_Mat.hxx>
class Standard_DomainError;
class gp_Pnt;
class gp_Mat;
class gp_Ax1;
class GProp_PrincipalProps;



//! Implements a general mechanism to compute the global properties of
//! a "compound geometric system" in 3d space    by composition of the
//! global properties of "elementary geometric entities"       such as
//! (curve, surface, solid, set of points).  It is possible to compose
//! the properties of several "compound geometric systems" too.
//!
//! To computes the global properties of a compound geometric
//! system you should :
//! . declare the GProps using a constructor which initializes the
//! GProps and defines the location point used to compute the inertia
//! . compose the global properties of your geometric components with
//! the properties of your system using the method Add.
//!
//! To compute the global properties of the geometric components of
//! the system you should  use the services of the following classes :
//! - class PGProps for a set of points,
//! - class CGProps for a curve,
//! - class SGProps for a surface,
//! - class VGProps for a "solid".
//! The classes CGProps, SGProps, VGProps are generic classes and
//! must be instantiated for your application.
//!
//! The global properties computed are :
//! - the dimension (length, area or volume)
//! - the mass,
//! - the centre of mass,
//! - the moments of inertia (static moments and quadratic moments),
//! - the moment about an axis,
//! - the radius of gyration about an axis,
//! - the principal properties of inertia  :
//! (sea also class PrincipalProps)
//! . the principal moments,
//! . the principal axis of inertia,
//! . the principal radius of gyration,
//!
//! Example of utilisation in a simplified C++ implementation :
//!
//! //declares the GProps, the point (0.0, 0.0, 0.0) of the
//! //absolute cartesian coordinate system is used as
//! //default reference point to compute the centre of mass
//! GProp_GProps System ();
//!
//! //computes the inertia of a 3d curve
//! Your_CGProps Component1 (curve, ....);
//!
//! //computes the inertia of surfaces
//! Your_SGprops Component2 (surface1, ....);
//! Your_SGprops Component3 (surface2,....);
//!
//! //composes the global properties of components 1, 2, 3
//! //a density can be associated with the components, the
//! //density can be defaulted to 1.
//! Real Density1 = 2.0;
//! Real Density2 = 3.0;
//! System.Add (Component1, Density1);
//! System.Add (Component2, Density2);
//! System.Add (Component3);
//!
//! //returns the centre of mass of the system in the
//! //absolute cartesian coordinate system
//! gp_Pnt G = System.CentreOfMass ();
//!
//! //computes the principales inertia of the system
//! GProp_PrincipalProps Pp  = System.PrincipalProperties();
//!
//! //returns the principal moments and radius of gyration
//! Real Ixx, Iyy, Izz, Rxx, Ryy, Rzz;
//! Pp.Moments (Ixx, Iyy, Izz);
//! Pp.RadiusOfGyration (Ixx, Iyy, Izz);
class GProp_GProps 
{
public:

  DEFINE_STANDARD_ALLOC

  

  //! The origin (0, 0, 0) of the absolute cartesian coordinate system
  //! is used to compute the global properties.
  Standard_EXPORT GProp_GProps();
  

  //! The point SystemLocation is used to compute the gobal properties
  //! of the system. For more accuracy it is better to define this
  //! point closed to the location of the system. For example it could
  //! be a point around the centre of mass of the system.
  //! This point is referred to as the reference point for
  //! this framework. For greater accuracy it is better for
  //! the reference point to be close to the location of the
  //! system. It can, for example, be a point near the
  //! center of mass of the system.
  //! At initialization, the framework is empty; i.e. it
  //! retains no dimensional information such as mass, or
  //! inertia. However, it is now able to bring together
  //! global properties of various other systems, whose
  //! global properties have already been computed
  //! using another framework. To do this, use the
  //! function Add to define the components of the
  //! system. Use it once per component of the system,
  //! and then use the interrogation functions available to
  //! access the computed values.
  Standard_EXPORT GProp_GProps(const gp_Pnt& SystemLocation);
  
  //! Either
  //! - initializes the global properties retained by this
  //! framework from those retained by the framework Item, or
  //! - brings together the global properties still retained by
  //! this framework with those retained by the framework Item.
  //! The value Density, which is 1.0 by default, is used as
  //! the density of the system analysed by Item.
  //! Sometimes the density will have already been given at
  //! the time of construction of the framework Item. This
  //! may be the case for example, if Item is a
  //! GProp_PGProps framework built to compute the
  //! global properties of a set of points ; or another
  //! GProp_GProps object which already retains
  //! composite global properties. In these cases the real
  //! density was perhaps already taken into account at the
  //! time of construction of Item. Note that this is not
  //! checked: if the density of parts of the system is taken
  //! into account two or more times, results of the
  //! computation will be false.
  //! Notes :
  //! - The point relative to which the inertia of Item is
  //! computed (i.e. the reference point of Item) may be
  //! different from the reference point in this
  //! framework. Huygens' theorem is applied
  //! automatically to transfer inertia values to the
  //! reference point in this framework.
  //! - The function Add is used once per component of
  //! the system. After that, you use the interrogation
  //! functions available to access values computed for the system.
  //! - The system whose global properties are already
  //! brought together by this framework is referred to
  //! as the current system. However, the current system
  //! is not retained by this framework, which maintains
  //! only its global properties.
  //! Exceptions
  //! Standard_DomainError if Density is less than or
  //! equal to gp::Resolution().
  Standard_EXPORT void Add (const GProp_GProps& Item, const Standard_Real Density = 1.0);
  
  //! Returns the mass of the current system.
  //! If no density is attached to the components of the
  //! current system the returned value corresponds to :
  //! - the total length of the edges of the current
  //! system if this framework retains only linear
  //! properties, as is the case for example, when
  //! using only the LinearProperties function to
  //! combine properties of lines from shapes, or
  //! - the total area of the faces of the current system if
  //! this framework retains only surface properties,
  //! as is the case for example, when using only the
  //! SurfaceProperties function to combine
  //! properties of surfaces from shapes, or
  //! - the total volume of the solids of the current
  //! system if this framework retains only volume
  //! properties, as is the case for example, when
  //! using only the VolumeProperties function to
  //! combine properties of volumes from solids.
  //! Warning
  //! A length, an area, or a volume is computed in the
  //! current data unit system. The mass of a single
  //! object is obtained by multiplying its length, its area
  //! or its volume by the given density. You must be
  //! consistent with respect to the units used.
  Standard_EXPORT Standard_Real Mass() const;
  

  //! Returns the center of mass of the current system. If
  //! the gravitational field is uniform, it is the center of gravity.
  //! The coordinates returned for the center of mass are
  //! expressed in the absolute Cartesian coordinate system.
  Standard_EXPORT gp_Pnt CentreOfMass() const;
  

  //! returns the matrix of inertia. It is a symmetrical matrix.
  //! The coefficients of the matrix are the quadratic moments of
  //! inertia.
  //!
  //! | Ixx  Ixy  Ixz |
  //! matrix =    | Ixy  Iyy  Iyz |
  //! | Ixz  Iyz  Izz |
  //!
  //! The moments of inertia are denoted by Ixx, Iyy, Izz.
  //! The products of inertia are denoted by Ixy, Ixz, Iyz.
  //! The matrix of inertia is returned in the central coordinate
  //! system (G, Gx, Gy, Gz) where G is the centre of mass of the
  //! system and Gx, Gy, Gz the directions parallel to the X(1,0,0)
  //! Y(0,1,0) Z(0,0,1) directions of the absolute cartesian
  //! coordinate system. It is possible to compute the matrix of
  //! inertia at another location point using the Huyghens theorem
  //! (you can use the method of package GProp : HOperator).
  Standard_EXPORT gp_Mat MatrixOfInertia() const;
  
  //! Returns Ix, Iy, Iz, the static moments of inertia of the
  //! current system; i.e. the moments of inertia about the
  //! three axes of the Cartesian coordinate system.
  Standard_EXPORT void StaticMoments (Standard_Real& Ix, Standard_Real& Iy, Standard_Real& Iz) const;
  

  //! computes the moment of inertia of the material system about the
  //! axis A.
  Standard_EXPORT Standard_Real MomentOfInertia (const gp_Ax1& A) const;
  
  //! Computes the principal properties of inertia of the current system.
  //! There is always a set of axes for which the products
  //! of inertia of a geometric system are equal to 0; i.e. the
  //! matrix of inertia of the system is diagonal. These axes
  //! are the principal axes of inertia. Their origin is
  //! coincident with the center of mass of the system. The
  //! associated moments are called the principal moments of inertia.
  //! This function computes the eigen values and the
  //! eigen vectors of the matrix of inertia of the system.
  //! Results are stored by using a presentation framework
  //! of principal properties of inertia
  //! (GProp_PrincipalProps object) which may be
  //! queried to access the value sought.
  Standard_EXPORT GProp_PrincipalProps PrincipalProperties() const;
  
  //! Returns the radius of gyration of the current system about the axis A.
  Standard_EXPORT Standard_Real RadiusOfGyration (const gp_Ax1& A) const;




protected:



  gp_Pnt g;
  gp_Pnt loc;
  Standard_Real dim;
  gp_Mat inertia;


private:





};







#endif // _GProp_GProps_HeaderFile
