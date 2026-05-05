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

#include <gp_Pnt.hxx>
#include <gp_Mat.hxx>
class gp_Ax1;
class GProp_PrincipalProps;

//! Implements a general mechanism to compute the global properties of
//! a "compound geometric system" in 3D space by composition of the
//! global properties of elementary geometric entities such as a curve,
//! surface, solid, or set of points. It is also possible to compose
//! the properties of several "compound geometric systems".
//!
//! To compute the global properties of a compound geometric system:
//! - declare a GProp_GProps using a constructor which initializes the
//!   instance and defines the location point used to compute the
//!   inertia,
//! - compose the global properties of the geometric components into
//!   the system using the method Add().
//!
//! To compute the global properties of the geometric components of
//! the system, use the services of the following frameworks:
//! - GProp_PGProps for a set of points,
//! - CGProps       for a curve,
//! - SGProps       for a surface,
//! - VGProps       for a "solid".
//! The CGProps, SGProps and VGProps frameworks are generic and must
//! be instantiated for the application (see BRepGProp / GeomGProp).
//!
//! The global properties computed are:
//! - the dimension (length, area or volume),
//! - the mass,
//! - the centre of mass,
//! - the moments of inertia (static moments and quadratic moments),
//! - the moment about an axis,
//! - the radius of gyration about an axis,
//! - the principal properties of inertia
//!   (see GProp_PrincipalProps):
//!   - the principal moments,
//!   - the principal axes of inertia,
//!   - the principal radii of gyration.
//!
//! Example:
//! @code
//!   // Declares the GProps; the absolute origin (0, 0, 0) is used as the
//!   // default reference point to compute the centre of mass.
//!   GProp_GProps aSystem;
//!
//!   // Computes the inertia of a 3D curve.
//!   Your_CGProps aComponent1(theCurve, ...);
//!
//!   // Computes the inertia of two surfaces.
//!   Your_SGProps aComponent2(theSurface1, ...);
//!   Your_SGProps aComponent3(theSurface2, ...);
//!
//!   // Composes the global properties of components 1, 2, 3. A density
//!   // can be associated with the components; it defaults to 1.0.
//!   const double aDensity1 = 2.0;
//!   const double aDensity2 = 3.0;
//!   aSystem.Add(aComponent1, aDensity1);
//!   aSystem.Add(aComponent2, aDensity2);
//!   aSystem.Add(aComponent3);
//!
//!   // Returns the centre of mass of the system in the absolute
//!   // Cartesian coordinate system.
//!   const gp_Pnt aG = aSystem.CentreOfMass();
//!
//!   // Computes the principal properties of inertia of the system.
//!   const GProp_PrincipalProps aPp = aSystem.PrincipalProperties();
//!
//!   // Returns the principal moments and radii of gyration.
//!   double aIxx, aIyy, aIzz, aRxx, aRyy, aRzz;
//!   aPp.Moments(aIxx, aIyy, aIzz);
//!   aPp.RadiusOfGyration(aRxx, aRyy, aRzz);
//! @endcode
class GProp_GProps
{
public:
  DEFINE_STANDARD_ALLOC

  //! The origin (0, 0, 0) of the absolute Cartesian coordinate system
  //! is used to compute the global properties.
  Standard_EXPORT GProp_GProps();

  //! The point SystemLocation is used to compute the global properties
  //! of the system. For greater accuracy, define this point close to
  //! the location of the system; for example a point near the centre
  //! of mass of the system.
  //!
  //! At initialization the framework is empty: it retains no
  //! dimensional information such as mass or inertia. It is, however,
  //! ready to bring together global properties of various other
  //! systems whose global properties have already been computed using
  //! another framework. To do this, use Add() to define the components
  //! of the system, once per component, and then use the interrogation
  //! functions to access the computed values.
  //!
  //! @param[in] SystemLocation reference point of the system used for
  //!                           inertia accumulation
  Standard_EXPORT GProp_GProps(const gp_Pnt& SystemLocation);

  //! Either:
  //! - initializes the global properties retained by this framework
  //!   from those retained by the framework Item, or
  //! - brings together the global properties retained by this
  //!   framework with those retained by the framework Item.
  //!
  //! The value Density (1.0 by default) is used as the density of the
  //! system analysed by Item. Sometimes the density has already been
  //! accounted for at construction time of Item - for example when
  //! Item is a GProp_PGProps framework built to compute the global
  //! properties of a set of weighted points, or another GProp_GProps
  //! object that already retains composite global properties. In these
  //! cases the real density was already taken into account at
  //! construction of Item. Note that this is not checked: if the
  //! density of parts of the system is taken into account two or more
  //! times, the result of the computation will be wrong.
  //!
  //! Notes:
  //! - The reference point of Item may differ from the reference point
  //!   of this framework. Huygens' theorem is applied automatically to
  //!   transfer inertia values to the reference point of this
  //!   framework.
  //! - Add() is used once per component of the system. After all
  //!   components are composed, the interrogation functions return
  //!   values for the system as a whole.
  //! - The system whose global properties have been brought together
  //!   by this framework is referred to as the "current system". The
  //!   current system itself is not retained: only its global
  //!   properties are.
  //!
  //! @param[in] Item    framework holding the global properties of the
  //!                    component to compose
  //! @param[in] Density density of the component (default 1.0)
  //! @throws Standard_DomainError if Density is less than or equal to
  //!         gp::Resolution().
  Standard_EXPORT void Add(const GProp_GProps& Item, const double Density = 1.0);

  //! Returns the mass of the current system.
  //!
  //! If no density has been attached to the components of the current
  //! system, the returned value corresponds to:
  //! - the total length of the edges of the current system if this
  //!   framework retains only linear properties (for example, when
  //!   using only LinearProperties() to combine properties of lines
  //!   from shapes), or
  //! - the total area of the faces of the current system if this
  //!   framework retains only surface properties (for example, when
  //!   using only SurfaceProperties() to combine properties of
  //!   surfaces from shapes), or
  //! - the total volume of the solids of the current system if this
  //!   framework retains only volume properties (for example, when
  //!   using only VolumeProperties() to combine properties of volumes
  //!   from solids).
  //!
  //! @warning A length, an area or a volume is computed in the current
  //!          unit system. The mass of a single object is its length,
  //!          area or volume multiplied by its density. Be consistent
  //!          with respect to the units used.
  Standard_EXPORT double Mass() const;

  //! Returns the centre of mass of the current system. With a uniform
  //! gravitational field this is also the centre of gravity. The
  //! coordinates returned for the centre of mass are expressed in the
  //! absolute Cartesian coordinate system.
  Standard_EXPORT gp_Pnt CentreOfMass() const;

  //! Returns the matrix of inertia. It is a symmetric matrix whose
  //! coefficients are the quadratic moments of inertia:
  //! @verbatim
  //!              | Ixx  Ixy  Ixz |
  //!     matrix = | Ixy  Iyy  Iyz |
  //!              | Ixz  Iyz  Izz |
  //! @endverbatim
  //! Ixx, Iyy, Izz are the moments of inertia; Ixy, Ixz, Iyz are the
  //! products of inertia.
  //!
  //! The matrix of inertia is returned in the central coordinate
  //! system (G, Gx, Gy, Gz), where G is the centre of mass of the
  //! system and Gx, Gy, Gz are parallel to the X(1, 0, 0), Y(0, 1, 0)
  //! and Z(0, 0, 1) directions of the absolute Cartesian coordinate
  //! system. To compute the matrix of inertia at another location use
  //! GProp::HOperator() (Huygens' theorem).
  Standard_EXPORT gp_Mat MatrixOfInertia() const;

  //! Returns the static moments of inertia of the current system -
  //! i.e. the moments of inertia about the three axes of the absolute
  //! Cartesian coordinate system.
  //!
  //! @param[out] Ix static moment of inertia about X
  //! @param[out] Iy static moment of inertia about Y
  //! @param[out] Iz static moment of inertia about Z
  Standard_EXPORT void StaticMoments(double& Ix, double& Iy, double& Iz) const;

  //! Computes the moment of inertia of the system about the axis A.
  //! @param[in] A axis about which the moment of inertia is computed
  Standard_EXPORT double MomentOfInertia(const gp_Ax1& A) const;

  //! Computes the principal properties of inertia of the current
  //! system. There is always a set of axes for which the products of
  //! inertia of a geometric system are equal to 0 - i.e. the matrix of
  //! inertia of the system is diagonal. These axes are the principal
  //! axes of inertia; their origin coincides with the centre of mass
  //! of the system. The associated moments are called the principal
  //! moments of inertia.
  //!
  //! This function computes the eigen values and eigen vectors of the
  //! matrix of inertia of the system. Results are stored in a
  //! GProp_PrincipalProps framework which can be queried to access
  //! the value sought.
  Standard_EXPORT GProp_PrincipalProps PrincipalProperties() const;

  //! Returns the radius of gyration of the current system about the
  //! axis A.
  //! @param[in] A axis about which the radius of gyration is computed
  Standard_EXPORT double RadiusOfGyration(const gp_Ax1& A) const;

protected:
  gp_Pnt g;       //!< Centre of mass (absolute frame)
  gp_Pnt loc;     //!< Reference point used for inertia accumulation
  double dim;     //!< Total mass / length / area / volume
  gp_Mat inertia; //!< Quadratic moments of inertia matrix
};

#endif // _GProp_GProps_HeaderFile
