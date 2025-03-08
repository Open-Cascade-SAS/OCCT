// Created on: 1992-12-04
// Created by: Isabelle GRIGNON
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

#ifndef _BRepGProp_HeaderFile
#define _BRepGProp_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <TColgp_Array1OfXYZ.hxx>

class TopoDS_Shape;
class GProp_GProps;
class gp_Pln;

//! Provides global functions to compute a shape's global
//! properties for lines, surfaces or volumes, and bring
//! them together with the global properties already
//! computed for a geometric system.
//! The global properties computed for a system are :
//! - its mass,
//! - its center of mass,
//! - its matrix of inertia,
//! - its moment about an axis,
//! - its radius of gyration about an axis,
//! - and its principal properties of inertia such as
//! principal axis, principal moments, principal radius of gyration.
class BRepGProp
{
public:
  DEFINE_STANDARD_ALLOC

  //! Computes the linear global properties of the shape S,
  //! i.e. the global properties induced by each edge of the
  //! shape S, and brings them together with the global
  //! properties still retained by the framework LProps. If
  //! the current system of LProps was empty, its global
  //! properties become equal to the linear global
  //! properties of S.
  //! For this computation no linear density is attached to
  //! the edges. So, for example, the added mass
  //! corresponds to the sum of the lengths of the edges of
  //! S. The density of the composed systems, i.e. that of
  //! each component of the current system of LProps, and
  //! that of S which is considered to be equal to 1, must be coherent.
  //! Note that this coherence cannot be checked. You are
  //! advised to use a separate framework for each
  //! density, and then to bring these frameworks together
  //! into a global one.
  //! The point relative to which the inertia of the system is
  //! computed is the reference point of the framework LProps.
  //! Note: if your programming ensures that the framework
  //! LProps retains only linear global properties (brought
  //! together for example, by the function
  //! LinearProperties) for objects the density of which is
  //! equal to 1 (or is not defined), the function Mass will
  //! return the total length of edges of the system analysed by LProps.
  //! Warning
  //! No check is performed to verify that the shape S
  //! retains truly linear properties. If S is simply a vertex, it
  //! is not considered to present any additional global properties.
  //! SkipShared is a special flag, which allows taking in calculation
  //! shared topological entities or not.
  //! For ex., if SkipShared = True, edges, shared by two or more faces,
  //! are taken into calculation only once.
  //! If we have cube with sizes 1, 1, 1, its linear properties = 12
  //! for SkipEdges = true and 24 for SkipEdges = false.
  //! UseTriangulation is a special flag, which defines preferable
  //! source of geometry data. If UseTriangulation = Standard_False,
  //! exact geometry objects (curves) are used, otherwise polygons of
  //! triangulation are used first.
  Standard_EXPORT static void LinearProperties(
    const TopoDS_Shape&    S,
    GProp_GProps&          LProps,
    const Standard_Boolean SkipShared       = Standard_False,
    const Standard_Boolean UseTriangulation = Standard_False);

  //! Computes the surface global properties of the
  //! shape S, i.e. the global properties induced by each
  //! face of the shape S, and brings them together with
  //! the global properties still retained by the framework
  //! SProps. If the current system of SProps was empty,
  //! its global properties become equal to the surface
  //! global properties of S.
  //! For this computation, no surface density is attached
  //! to the faces. Consequently, the added mass
  //! corresponds to the sum of the areas of the faces of
  //! S. The density of the component systems, i.e. that
  //! of each component of the current system of
  //! SProps, and that of S which is considered to be
  //! equal to 1, must be coherent.
  //! Note that this coherence cannot be checked. You
  //! are advised to use a framework for each different
  //! value of density, and then to bring these
  //! frameworks together into a global one.
  //! The point relative to which the inertia of the system
  //! is computed is the reference point of the framework SProps.
  //! Note : if your programming ensures that the
  //! framework SProps retains only surface global
  //! properties, brought together, for example, by the
  //! function SurfaceProperties, for objects the density
  //! of which is equal to 1 (or is not defined), the
  //! function Mass will return the total area of faces of
  //! the system analysed by SProps.
  //! Warning
  //! No check is performed to verify that the shape S
  //! retains truly surface properties. If S is simply a
  //! vertex, an edge or a wire, it is not considered to
  //! present any additional global properties.
  //! SkipShared is a special flag, which allows taking in calculation
  //! shared topological entities or not.
  //! For ex., if SkipShared = True, faces, shared by two or more shells,
  //! are taken into calculation only once.
  //! UseTriangulation is a special flag, which defines preferable
  //! source of geometry data. If UseTriangulation = Standard_False,
  //! exact geometry objects (surfaces) are used,
  //! otherwise face triangulations are used first.
  Standard_EXPORT static void SurfaceProperties(
    const TopoDS_Shape&    S,
    GProp_GProps&          SProps,
    const Standard_Boolean SkipShared       = Standard_False,
    const Standard_Boolean UseTriangulation = Standard_False);

  //! Updates <SProps> with the shape <S>, that contains its principal properties.
  //! The surface properties of all the faces in <S> are computed.
  //! Adaptive 2D Gauss integration is used.
  //! Parameter Eps sets maximal relative error of computed mass (area) for each face.
  //! Error is calculated as Abs((M(i+1)-M(i))/M(i+1)), M(i+1) and M(i) are values
  //! for two successive steps of adaptive integration.
  //! Method returns estimation of relative error reached for whole shape.
  //! WARNING: if Eps > 0.001 algorithm performs non-adaptive integration.
  //! SkipShared is a special flag, which allows taking in calculation
  //! shared topological entities or not
  //! For ex., if SkipShared = True, faces, shared by two or more shells,
  //! are taken into calculation only once.
  Standard_EXPORT static Standard_Real SurfaceProperties(
    const TopoDS_Shape&    S,
    GProp_GProps&          SProps,
    const Standard_Real    Eps,
    const Standard_Boolean SkipShared = Standard_False);
  //!
  //! Computes the global volume properties of the solid
  //! S, and brings them together with the global
  //! properties still retained by the framework VProps. If
  //! the current system of VProps was empty, its global
  //! properties become equal to the global properties of S for volume.
  //! For this computation, no volume density is attached
  //! to the solid. Consequently, the added mass
  //! corresponds to the volume of S. The density of the
  //! component systems, i.e. that of each component of
  //! the current system of VProps, and that of S which
  //! is considered to be equal to 1, must be coherent to each other.
  //! Note that this coherence cannot be checked. You
  //! are advised to use a separate framework for each
  //! density, and then to bring these frameworks
  //! together into a global one.
  //! The point relative to which the inertia of the system
  //! is computed is the reference point of the framework VProps.
  //! Note: if your programming ensures that the
  //! framework VProps retains only global properties of
  //! volume (brought together for example, by the
  //! function VolumeProperties) for objects the density
  //! of which is equal to 1 (or is not defined), the
  //! function Mass will return the total volume of the
  //! solids of the system analysed by VProps.
  //! Warning
  //! The shape S must represent an object whose
  //! global volume properties can be computed. It may
  //! be a finite solid, or a series of finite solids all
  //! oriented in a coherent way. Nonetheless, S must be
  //! exempt of any free boundary. Note that these
  //! conditions of coherence are not checked by this
  //! algorithm, and results will be false if they are not respected.
  //! SkipShared a is special flag, which allows taking in calculation
  //! shared topological entities or not.
  //! For ex., if SkipShared = True, the volumes formed by the equal
  //! (the same TShape, location and orientation) faces are taken
  //! into calculation only once.
  //! UseTriangulation is a special flag, which defines preferable
  //! source of geometry data. If UseTriangulation = Standard_False,
  //! exact geometry objects (surfaces) are used,
  //! otherwise face triangulations are used first.
  Standard_EXPORT static void VolumeProperties(
    const TopoDS_Shape&    S,
    GProp_GProps&          VProps,
    const Standard_Boolean OnlyClosed       = Standard_False,
    const Standard_Boolean SkipShared       = Standard_False,
    const Standard_Boolean UseTriangulation = Standard_False);

  //! Updates <VProps> with the shape <S>, that contains its principal properties.
  //! The volume properties of all the FORWARD and REVERSED faces in <S> are computed.
  //! If OnlyClosed is True then computed faces must belong to closed Shells.
  //! Adaptive 2D Gauss integration is used.
  //! Parameter Eps sets maximal relative error of computed mass (volume) for each face.
  //! Error is calculated as Abs((M(i+1)-M(i))/M(i+1)), M(i+1) and M(i) are values
  //! for two successive steps of adaptive integration.
  //! Method returns estimation of relative error reached for whole shape.
  //! WARNING: if Eps > 0.001 algorithm performs non-adaptive integration.
  //! SkipShared is a special flag, which allows taking in calculation shared
  //! topological entities or not.
  //! For ex., if SkipShared = True, the volumes formed by the equal
  //! (the same TShape, location and orientation)
  //! faces are taken into calculation only once.
  Standard_EXPORT static Standard_Real VolumeProperties(
    const TopoDS_Shape&    S,
    GProp_GProps&          VProps,
    const Standard_Real    Eps,
    const Standard_Boolean OnlyClosed = Standard_False,
    const Standard_Boolean SkipShared = Standard_False);

  //! Updates <VProps> with the shape <S>, that contains its principal properties.
  //! The volume properties of all the FORWARD and REVERSED faces in <S> are computed.
  //! If OnlyClosed is True then computed faces must belong to closed Shells.
  //! Adaptive 2D Gauss integration is used.
  //! Parameter IsUseSpan says if it is necessary to define spans on a face.
  //! This option has an effect only for BSpline faces.
  //! Parameter Eps sets maximal relative error of computed property for each face.
  //! Error is delivered by the adaptive Gauss-Kronrod method of integral computation
  //! that is used for properties computation.
  //! Method returns estimation of relative error reached for whole shape.
  //! Returns negative value if the computation is failed.
  //! SkipShared is a special flag, which allows taking in calculation
  //! shared topological entities or not.
  //! For ex., if SkipShared = True, the volumes formed by the equal
  //! (the same TShape, location and orientation) faces are taken into calculation only once.
  Standard_EXPORT static Standard_Real VolumePropertiesGK(
    const TopoDS_Shape&    S,
    GProp_GProps&          VProps,
    const Standard_Real    Eps        = 0.001,
    const Standard_Boolean OnlyClosed = Standard_False,
    const Standard_Boolean IsUseSpan  = Standard_False,
    const Standard_Boolean CGFlag     = Standard_False,
    const Standard_Boolean IFlag      = Standard_False,
    const Standard_Boolean SkipShared = Standard_False);

  Standard_EXPORT static Standard_Real VolumePropertiesGK(
    const TopoDS_Shape&    S,
    GProp_GProps&          VProps,
    const gp_Pln&          thePln,
    const Standard_Real    Eps        = 0.001,
    const Standard_Boolean OnlyClosed = Standard_False,
    const Standard_Boolean IsUseSpan  = Standard_False,
    const Standard_Boolean CGFlag     = Standard_False,
    const Standard_Boolean IFlag      = Standard_False,
    const Standard_Boolean SkipShared = Standard_False);
};

#endif // _BRepGProp_HeaderFile
