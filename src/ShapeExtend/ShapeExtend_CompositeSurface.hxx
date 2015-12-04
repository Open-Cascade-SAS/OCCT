// Created on: 1999-04-27
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
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

#ifndef _ShapeExtend_CompositeSurface_HeaderFile
#define _ShapeExtend_CompositeSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColGeom_HArray2OfSurface.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <Standard_Boolean.hxx>
#include <Geom_Surface.hxx>
#include <ShapeExtend_Parametrisation.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <GeomAbs_Shape.hxx>
class Geom_Surface;
class gp_Pnt2d;
class gp_Trsf2d;
class gp_Trsf;
class Geom_Geometry;
class Geom_Curve;
class gp_Pnt;
class gp_Vec;


class ShapeExtend_CompositeSurface;
DEFINE_STANDARD_HANDLE(ShapeExtend_CompositeSurface, Geom_Surface)

//! Composite surface is represented by a grid of surfaces
//! (patches) connected geometrically. Patches may have different
//! parametrisation ranges, but they should be parametrised in
//! the same manner so that parameter of each patch (u,v) can be converted
//! to global parameter on the whole surface (U,V) with help of linear
//! transformation:
//!
//! for any i,j-th patch
//! U = Ui + ( u - uijmin ) * ( Ui+1 - Ui ) / ( uijmax - uijmin )
//! V = Vj + ( v - vijmin ) * ( Vj+1 - Vj ) / ( vijmax - vijmin )
//!
//! where
//!
//! [uijmin, uijmax] * [ vijmin, vijmax] - parametric range of i,j-th patch,
//!
//! Ui (i=1,..,Nu+1), Vi (j=1,..,Nv+1) - values defining global
//! parametrisation by U and V (correspond to points between patches and
//! bounds, (Ui,Uj) corresponds to (uijmin,vijmin) on i,j-th patch) and to
//! (u(i-1)(j-1)max,v(i-1)(j-1)max) on (i-1),(j-1)-th patch.
//!
//! Geometrical connectivity is expressed via global parameters:
//! S[i,j](Ui+1,V) = S[i+1,j](Ui+1,V) for any i, j, V
//! S[i,j](U,Vj+1) = S[i,j+1](U,Vj+1) for any i, j, U
//! It is checked with Precision::Confusion() by default.
//!
//! NOTE 1: This class is inherited from Geom_Surface in order to
//! make it more easy to store and deal with it. However, it should
//! not be passed to standard methods dealing with geometry since
//! this type is not known to them.
//! NOTE 2: Not all the inherited methods are implemented, and some are
//! implemented not in the full form.
class ShapeExtend_CompositeSurface : public Geom_Surface
{

public:

  
  //! Empty constructor.
  Standard_EXPORT ShapeExtend_CompositeSurface();
  
  //! Initializes by a grid of surfaces (calls Init()).
  Standard_EXPORT ShapeExtend_CompositeSurface(const Handle(TColGeom_HArray2OfSurface)& GridSurf, const ShapeExtend_Parametrisation param = ShapeExtend_Natural);
  
  //! Initializes by a grid of surfaces (calls Init()).
  Standard_EXPORT ShapeExtend_CompositeSurface(const Handle(TColGeom_HArray2OfSurface)& GridSurf, const TColStd_Array1OfReal& UJoints, const TColStd_Array1OfReal& VJoints);
  
  //! Initializes by a grid of surfaces.
  //! All the Surfaces of the grid must have geometrical
  //! connectivity as stated above.
  //! If geometrical connectivity is not satisfied, method
  //! returns False.
  //! However, class is initialized even in that case.
  //!
  //! Last parameter defines how global parametrisation
  //! (joint values) will be computed:
  //! ShapeExtend_Natural: U1 = u11min, Ui+1 = Ui + (ui1max-ui1min), etc.
  //! ShapeExtend_Uniform: Ui = i-1, Vj = j-1
  //! ShapeExtend_Unitary: Ui = (i-1)/Nu, Vi = (j-1)/Nv
  Standard_EXPORT Standard_Boolean Init (const Handle(TColGeom_HArray2OfSurface)& GridSurf, const ShapeExtend_Parametrisation param = ShapeExtend_Natural);
  
  //! Initializes by a grid of surfaces with given global
  //! parametrisation defined by UJoints and VJoints arrays,
  //! each having langth equal to number of patches in corresponding
  //! direction + 1. Global joint values should be sorted in
  //! increasing order.
  //! All the Surfaces of the grid must have geometrical
  //! connectivity as stated above.
  //! If geometrical connectivity is not satisfied, method
  //! returns False.
  //! However, class is initialized even in that case.
  Standard_EXPORT Standard_Boolean Init (const Handle(TColGeom_HArray2OfSurface)& GridSurf, const TColStd_Array1OfReal& UJoints, const TColStd_Array1OfReal& VJoints);
  
  //! Returns number of patches in U direction.
  Standard_EXPORT Standard_Integer NbUPatches() const;
  
  //! Returns number of patches in V direction.
  Standard_EXPORT Standard_Integer NbVPatches() const;
  
  //! Returns one surface patch
  Standard_EXPORT const Handle(Geom_Surface)& Patch (const Standard_Integer i, const Standard_Integer j) const;
  
  //! Returns grid of surfaces
  Standard_EXPORT const Handle(TColGeom_HArray2OfSurface)& Patches() const;
  
  //! Returns the array of U values corresponding to joint
  //! points between patches as well as to start and end points,
  //! which define global parametrisation of the surface
  Standard_EXPORT Handle(TColStd_HArray1OfReal) UJointValues() const;
  
  //! Returns the array of V values corresponding to joint
  //! points between patches as well as to start and end points,
  //! which define global parametrisation of the surface
  Standard_EXPORT Handle(TColStd_HArray1OfReal) VJointValues() const;
  
  //! Returns i-th joint value in U direction
  //! (1-st is global Umin, (NbUPatches()+1)-th is global Umax
  //! on the composite surface)
  Standard_EXPORT Standard_Real UJointValue (const Standard_Integer i) const;
  
  //! Returns j-th joint value in V direction
  //! (1-st is global Vmin, (NbVPatches()+1)-th is global Vmax
  //! on the composite surface)
  Standard_EXPORT Standard_Real VJointValue (const Standard_Integer j) const;
  
  //! Sets the array of U values corresponding to joint
  //! points, which define global parametrisation of the surface.
  //! Number of values in array should be equal to NbUPatches()+1.
  //! All the values should be sorted in increasing order.
  //! If this is not satisfied, does nothing and returns False.
  Standard_EXPORT Standard_Boolean SetUJointValues (const TColStd_Array1OfReal& UJoints);
  
  //! Sets the array of V values corresponding to joint
  //! points, which define global parametrisation of the surface
  //! Number of values in array should be equal to NbVPatches()+1.
  //! All the values should be sorted in increasing order.
  //! If this is not satisfied, does nothing and returns False.
  Standard_EXPORT Standard_Boolean SetVJointValues (const TColStd_Array1OfReal& VJoints);
  
  //! Changes starting value for global U parametrisation (all
  //! other joint values are shifted accordingly)
  Standard_EXPORT void SetUFirstValue (const Standard_Real UFirst);
  
  //! Changes starting value for global V parametrisation (all
  //! other joint values are shifted accordingly)
  Standard_EXPORT void SetVFirstValue (const Standard_Real VFirst);
  
  //! Returns number of col that contains given (global) parameter
  Standard_EXPORT Standard_Integer LocateUParameter (const Standard_Real U) const;
  
  //! Returns number of row that contains given (global) parameter
  Standard_EXPORT Standard_Integer LocateVParameter (const Standard_Real V) const;
  
  //! Returns number of row and col of surface that contains
  //! given point
  Standard_EXPORT void LocateUVPoint (const gp_Pnt2d& pnt, Standard_Integer& i, Standard_Integer& j) const;
  
  //! Returns one surface patch that contains given (global) parameters
  Standard_EXPORT const Handle(Geom_Surface)& Patch (const Standard_Real U, const Standard_Real V) const;
  
  //! Returns one surface patch that contains given point
  Standard_EXPORT const Handle(Geom_Surface)& Patch (const gp_Pnt2d& pnt) const;
  
  //! Converts local parameter u on patch i,j to global parameter U
  Standard_EXPORT Standard_Real ULocalToGlobal (const Standard_Integer i, const Standard_Integer j, const Standard_Real u) const;
  
  //! Converts local parameter v on patch i,j to global parameter V
  Standard_EXPORT Standard_Real VLocalToGlobal (const Standard_Integer i, const Standard_Integer j, const Standard_Real v) const;
  
  //! Converts local parameters uv on patch i,j to global parameters UV
  Standard_EXPORT gp_Pnt2d LocalToGlobal (const Standard_Integer i, const Standard_Integer j, const gp_Pnt2d& uv) const;
  
  //! Converts global parameter U to local parameter u on patch i,j
  Standard_EXPORT Standard_Real UGlobalToLocal (const Standard_Integer i, const Standard_Integer j, const Standard_Real U) const;
  
  //! Converts global parameter V to local parameter v on patch i,j
  Standard_EXPORT Standard_Real VGlobalToLocal (const Standard_Integer i, const Standard_Integer j, const Standard_Real V) const;
  
  //! Converts global parameters UV to local parameters uv on patch i,j
  Standard_EXPORT gp_Pnt2d GlobalToLocal (const Standard_Integer i, const Standard_Integer j, const gp_Pnt2d& UV) const;
  
  //! Computes transformation operator and uFactor descrinbing affine
  //! transformation required to convert global parameters on composite
  //! surface to local parameters on patch (i,j):
  //! uv = ( uFactor, 1. ) X Trsf * UV;
  //! NOTE: Thus Trsf contains shift and scale by V, scale by U is stored in uFact.
  //! Returns True if transformation is not an identity
  Standard_EXPORT Standard_Boolean GlobalToLocalTransformation (const Standard_Integer i, const Standard_Integer j, Standard_Real& uFact, gp_Trsf2d& Trsf) const;
  
  //! Applies transformation to all the patches
  Standard_EXPORT virtual void Transform (const gp_Trsf& T) Standard_OVERRIDE;
  
  //! Returns a copy of the surface
  Standard_EXPORT virtual Handle(Geom_Geometry) Copy() const Standard_OVERRIDE;
  
  //! NOT IMPLEMENTED (does nothing)
  Standard_EXPORT virtual void UReverse() Standard_OVERRIDE;
  
  //! Returns U
  Standard_EXPORT virtual Standard_Real UReversedParameter (const Standard_Real U) const Standard_OVERRIDE;
  
  //! NOT IMPLEMENTED (does nothing)
  Standard_EXPORT virtual void VReverse() Standard_OVERRIDE;
  
  //! Returns V
  Standard_EXPORT virtual Standard_Real VReversedParameter (const Standard_Real V) const Standard_OVERRIDE;
  
  //! Returns the parametric bounds of grid
  Standard_EXPORT virtual void Bounds (Standard_Real& U1, Standard_Real& U2, Standard_Real& V1, Standard_Real& V2) const Standard_OVERRIDE;
  
  //! Returns True if grid is closed in U direction
  //! (i.e. connected with Precision::Confusion)
  Standard_EXPORT virtual Standard_Boolean IsUClosed() const Standard_OVERRIDE;
  
  //! Returns True if grid is closed in V direction
  //! (i.e. connected with Precision::Confusion)
  Standard_EXPORT virtual Standard_Boolean IsVClosed() const Standard_OVERRIDE;
  
  //! Returns False
  Standard_EXPORT virtual Standard_Boolean IsUPeriodic() const Standard_OVERRIDE;
  
  //! Returns False
  Standard_EXPORT virtual Standard_Boolean IsVPeriodic() const Standard_OVERRIDE;
  
  //! NOT IMPLEMENTED (returns Null curve)
  Standard_EXPORT virtual Handle(Geom_Curve) UIso (const Standard_Real U) const Standard_OVERRIDE;
  
  //! NOT IMPLEMENTED (returns Null curve)
  Standard_EXPORT virtual Handle(Geom_Curve) VIso (const Standard_Real V) const Standard_OVERRIDE;
  
  //! returns C0
  Standard_EXPORT virtual GeomAbs_Shape Continuity() const Standard_OVERRIDE;
  
  //! returns True if N <=0
  Standard_EXPORT virtual Standard_Boolean IsCNu (const Standard_Integer N) const Standard_OVERRIDE;
  
  //! returns True if N <=0
  Standard_EXPORT virtual Standard_Boolean IsCNv (const Standard_Integer N) const Standard_OVERRIDE;
  
  //! Computes the point of parameter U,V on the grid.
  Standard_EXPORT virtual void D0 (const Standard_Real U, const Standard_Real V, gp_Pnt& P) const Standard_OVERRIDE;
  
  //! Computes the point P and the first derivatives in the
  //! directions U and V at this point.
  Standard_EXPORT virtual void D1 (const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V) const Standard_OVERRIDE;
  
  //! Computes the point P, the first and the second derivatives in
  //! the directions U and V at this point.
  Standard_EXPORT virtual void D2 (const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V, gp_Vec& D2U, gp_Vec& D2V, gp_Vec& D2UV) const Standard_OVERRIDE;
  
  //! Computes the point P, the first,the second and the third
  //! derivatives in the directions U and V at this point.
  Standard_EXPORT virtual void D3 (const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V, gp_Vec& D2U, gp_Vec& D2V, gp_Vec& D2UV, gp_Vec& D3U, gp_Vec& D3V, gp_Vec& D3UUV, gp_Vec& D3UVV) const Standard_OVERRIDE;
  
  //! Computes the derivative of order Nu in the direction U and Nv
  //! in the direction V at the point P(U, V).
  Standard_EXPORT virtual gp_Vec DN (const Standard_Real U, const Standard_Real V, const Standard_Integer Nu, const Standard_Integer Nv) const Standard_OVERRIDE;
  
  //! Computes the point of parameter pnt on the grid.
  Standard_EXPORT gp_Pnt Value (const gp_Pnt2d& pnt) const;
  
  //! Computes Joint values according to parameter
  Standard_EXPORT void ComputeJointValues (const ShapeExtend_Parametrisation param = ShapeExtend_Natural);
  
  //! Checks geometrical connectivity of the patches, including
  //! closedness (sets fields muUClosed and myVClosed)
  Standard_EXPORT Standard_Boolean CheckConnectivity (const Standard_Real prec);




  DEFINE_STANDARD_RTTIEXT(ShapeExtend_CompositeSurface,Geom_Surface)

protected:




private:


  Handle(TColGeom_HArray2OfSurface) myPatches;
  Handle(TColStd_HArray1OfReal) myUJointValues;
  Handle(TColStd_HArray1OfReal) myVJointValues;
  Standard_Boolean myUClosed;
  Standard_Boolean myVClosed;


};







#endif // _ShapeExtend_CompositeSurface_HeaderFile
