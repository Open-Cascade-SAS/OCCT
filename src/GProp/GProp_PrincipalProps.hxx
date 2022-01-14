// Created on: 1992-02-17
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

#ifndef _GProp_PrincipalProps_HeaderFile
#define _GProp_PrincipalProps_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Vec.hxx>
#include <gp_Pnt.hxx>
#include <GProp_GProps.hxx>
#include <Standard_Boolean.hxx>



//! A framework to present the principal properties of
//! inertia of a system of which global properties are
//! computed by a GProp_GProps object.
//! There is always a set of axes for which the
//! products of inertia of a geometric system are equal
//! to 0; i.e. the matrix of inertia of the system is
//! diagonal. These axes are the principal axes of
//! inertia. Their origin is coincident with the center of
//! mass of the system. The associated moments are
//! called the principal moments of inertia.
//! This sort of presentation object is created, filled and
//! returned by the function PrincipalProperties for
//! any GProp_GProps object, and can be queried to access the result.
//! Note: The system whose principal properties of
//! inertia are returned by this framework is referred to
//! as the current system. The current system,
//! however, is retained neither by this presentation
//! framework nor by the GProp_GProps object which activates it.
class GProp_PrincipalProps 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! creates an undefined PrincipalProps.
  Standard_EXPORT GProp_PrincipalProps();
  

  //! returns true if the geometric system has an axis of symmetry.
  //! For  comparing  moments  relative  tolerance  1.e-10  is  used.
  //! Usually  it  is  enough  for  objects,  restricted  by  faces  with
  //! analitycal  geometry.
  Standard_EXPORT Standard_Boolean HasSymmetryAxis() const;
  

  //! returns true if the geometric system has an axis of symmetry.
  //! aTol  is  relative  tolerance for  checking  equality  of  moments
  //! If  aTol  ==  0,  relative  tolerance  is  ~  1.e-16  (Epsilon(I))
  Standard_EXPORT Standard_Boolean HasSymmetryAxis (const Standard_Real aTol) const;
  

  //! returns true if the geometric system has a point of symmetry.
  //! For  comparing  moments  relative  tolerance  1.e-10  is  used.
  //! Usually  it  is  enough  for  objects,  restricted  by  faces  with
  //! analitycal  geometry.
  Standard_EXPORT Standard_Boolean HasSymmetryPoint() const;
  

  //! returns true if the geometric system has a point of symmetry.
  //! aTol  is  relative  tolerance for  checking  equality  of  moments
  //! If  aTol  ==  0,  relative  tolerance  is  ~  1.e-16  (Epsilon(I))
  Standard_EXPORT Standard_Boolean HasSymmetryPoint (const Standard_Real aTol) const;
  
  //! Ixx, Iyy and Izz return the principal moments of inertia
  //! in the current system.
  //! Notes :
  //! - If the current system has an axis of symmetry, two
  //! of the three values Ixx, Iyy and Izz are equal. They
  //! indicate which eigen vectors define an infinity of
  //! axes of principal inertia.
  //! - If the current system has a center of symmetry, Ixx,
  //! Iyy and Izz are equal.
  Standard_EXPORT void Moments (Standard_Real& Ixx, Standard_Real& Iyy, Standard_Real& Izz) const;
  
  //! returns the first axis of inertia.
  //!
  //! if the system has a point of symmetry there is an infinity of
  //! solutions. It is not possible to defines the three axis of
  //! inertia.
  Standard_EXPORT const gp_Vec& FirstAxisOfInertia() const;
  
  //! returns the second axis of inertia.
  //!
  //! if the system has a point of symmetry or an axis of symmetry the
  //! second and the third axis of symmetry are undefined.
  Standard_EXPORT const gp_Vec& SecondAxisOfInertia() const;
  
  //! returns the third axis of inertia.
  //! This and the above functions return the first, second or third eigen vector of the
  //! matrix of inertia of the current system.
  //! The first, second and third principal axis of inertia
  //! pass through the center of mass of the current
  //! system. They are respectively parallel to these three eigen vectors.
  //! Note that:
  //! - If the current system has an axis of symmetry, any
  //! axis is an axis of principal inertia if it passes
  //! through the center of mass of the system, and runs
  //! parallel to a linear combination of the two eigen
  //! vectors of the matrix of inertia, corresponding to the
  //! two eigen values which are equal. If the current
  //! system has a center of symmetry, any axis passing
  //! through the center of mass of the system is an axis
  //! of principal inertia. Use the functions
  //! HasSymmetryAxis and HasSymmetryPoint to
  //! check these particular cases, where the returned
  //! eigen vectors define an infinity of principal axis of inertia.
  //! - The Moments function can be used to know which
  //! of the three eigen vectors corresponds to the two
  //! eigen values which are equal.
  //!
  //! if the system has a point of symmetry or an axis of symmetry the
  //! second and the third axis of symmetry are undefined.
  Standard_EXPORT const gp_Vec& ThirdAxisOfInertia() const;
  
  //! Returns the principal radii of gyration  Rxx, Ryy
  //! and Rzz are the radii of gyration of the current
  //! system about its three principal axes of inertia.
  //! Note that:
  //! - If the current system has an axis of symmetry,
  //! two of the three values Rxx, Ryy and Rzz are equal.
  //! - If the current system has a center of symmetry,
  //! Rxx, Ryy and Rzz are equal.
  Standard_EXPORT void RadiusOfGyration (Standard_Real& Rxx, Standard_Real& Ryy, Standard_Real& Rzz) const;


friend   
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
  Standard_EXPORT GProp_PrincipalProps GProp_GProps::PrincipalProperties() const;


protected:





private:

  
  Standard_EXPORT GProp_PrincipalProps(const Standard_Real Ixx, const Standard_Real Iyy, const Standard_Real Izz, const Standard_Real Rxx, const Standard_Real Ryy, const Standard_Real Rzz, const gp_Vec& Vxx, const gp_Vec& Vyy, const gp_Vec& Vzz, const gp_Pnt& G);


  Standard_Real i1;
  Standard_Real i2;
  Standard_Real i3;
  Standard_Real r1;
  Standard_Real r2;
  Standard_Real r3;
  gp_Vec v1;
  gp_Vec v2;
  gp_Vec v3;
  gp_Pnt g;


};







#endif // _GProp_PrincipalProps_HeaderFile
