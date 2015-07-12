// Created on: 1991-04-12
// Created by: Michel CHAUVAT
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

#ifndef _BRepGProp_Vinert_HeaderFile
#define _BRepGProp_Vinert_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <GProp_GProps.hxx>
class BRepGProp_Face;
class gp_Pnt;
class gp_Pln;
class BRepGProp_Domain;



//! Computes the global properties of a geometric solid
//! (3D closed region of space) delimited with :
//! . a surface
//! . a point and a surface
//! . a plane and a surface
//!
//! The surface can be :
//! . a surface limited with its parametric values U-V,
//! . a surface limited in U-V space with its curves of restriction,
//!
//! The surface 's requirements to evaluate the global properties
//! are defined in the template SurfaceTool from package GProp.
class BRepGProp_Vinert  : public GProp_GProps
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BRepGProp_Vinert();
  

  //! Computes the global properties of a region of 3D space
  //! delimited with the surface <S> and the point VLocation. S can be closed
  //! The method is quick and its precision is enough for many cases of analytical
  //! surfaces.
  //! Non-adaptive 2D Gauss integration with predefined numbers of Gauss points
  //! is used. Numbers of points depend on types of surfaces and  curves.
  //! Errror of the computation is not calculated.
  Standard_EXPORT BRepGProp_Vinert(const BRepGProp_Face& S, const gp_Pnt& VLocation);
  

  //! Computes the global properties of a region of 3D space
  //! delimited with the surface <S> and the point VLocation. S can be closed
  //! Adaptive 2D Gauss integration is used.
  //! Parameter Eps sets maximal relative error of computed mass (volume) for face.
  //! Error is calculated as Abs((M(i+1)-M(i))/M(i+1)), M(i+1) and M(i) are values
  //! for two successive steps of adaptive integration.
  Standard_EXPORT BRepGProp_Vinert(BRepGProp_Face& S, const gp_Pnt& VLocation, const Standard_Real Eps);
  

  //! Computes the global properties of the region of 3D space
  //! delimited with the surface <S> and the point VLocation.
  //! The method is quick and its precision is enough for many cases of analytical
  //! surfaces.
  //! Non-adaptive 2D Gauss integration with predefined numbers of Gauss points
  //! is used. Numbers of points depend on types of surfaces and  curves.
  //! Error of the computation is not calculated.
  Standard_EXPORT BRepGProp_Vinert(const BRepGProp_Face& S, const gp_Pnt& O, const gp_Pnt& VLocation);
  

  //! Computes the global properties of the region of 3D space
  //! delimited with the surface <S> and the point VLocation.
  //! Adaptive 2D Gauss integration is used.
  //! Parameter Eps sets maximal relative error of computed mass (volume) for face.
  //! Error is calculated as Abs((M(i+1)-M(i))/M(i+1)), M(i+1) and M(i) are values
  //! for two successive steps of adaptive integration.
  //! WARNING: if Eps > 0.001 algorithm performs non-adaptive integration.
  Standard_EXPORT BRepGProp_Vinert(BRepGProp_Face& S, const gp_Pnt& O, const gp_Pnt& VLocation, const Standard_Real Eps);
  

  //! Computes the global properties of the region of 3D space
  //! delimited with the surface <S> and the plane Pln.
  //! The method is quick and its precision is enough for many cases of analytical
  //! surfaces.
  //! Non-adaptive 2D Gauss integration with predefined numbers of Gauss points
  //! is used. Numbers of points depend on types of surfaces and  curves.
  //! Error of the computation is not calculated.
  Standard_EXPORT BRepGProp_Vinert(const BRepGProp_Face& S, const gp_Pln& Pl, const gp_Pnt& VLocation);
  

  //! Computes the global properties of the region of 3D space
  //! delimited with the surface <S> and the plane Pln.
  //! Adaptive 2D Gauss integration is used.
  //! Parameter Eps sets maximal relative error of computed mass (volume) for face.
  //! Error is calculated as Abs((M(i+1)-M(i))/M(i+1)), M(i+1) and M(i) are values
  //! for two successive steps of adaptive integration.
  //! WARNING: if Eps > 0.001 algorithm performs non-adaptive integration.
  Standard_EXPORT BRepGProp_Vinert(BRepGProp_Face& S, const gp_Pln& Pl, const gp_Pnt& VLocation, const Standard_Real Eps);
  

  //! Computes the global properties of a region of 3D space
  //! delimited with the surface <S> and the point VLocation. S can be closed
  //! The method is quick and its precision is enough for many cases of analytical
  //! surfaces.
  //! Non-adaptive 2D Gauss integration with predefined numbers of Gauss points
  //! is used. Numbers of points depend on types of surfaces and  curves.
  //! Errror of the computation is not calculated.
  Standard_EXPORT BRepGProp_Vinert(BRepGProp_Face& S, BRepGProp_Domain& D, const gp_Pnt& VLocation);
  

  //! Computes the global properties of a region of 3D space
  //! delimited with the surface <S> and the point VLocation. S can be closed
  //! Adaptive 2D Gauss integration is used.
  //! Parameter Eps sets maximal relative error of computed mass (volume) for face.
  //! Error is calculated as Abs((M(i+1)-M(i))/M(i+1)), M(i+1) and M(i) are values
  //! for two successive steps of adaptive integration.
  Standard_EXPORT BRepGProp_Vinert(BRepGProp_Face& S, BRepGProp_Domain& D, const gp_Pnt& VLocation, const Standard_Real Eps);
  

  //! Computes the global properties of the region of 3D space
  //! delimited with the surface <S> and the point VLocation.
  //! The method is quick and its precision is enough for many cases of analytical
  //! surfaces.
  //! Non-adaptive 2D Gauss integration with predefined numbers of Gauss points
  //! is used. Numbers of points depend on types of surfaces and  curves.
  //! Error of the computation is not calculated.
  Standard_EXPORT BRepGProp_Vinert(BRepGProp_Face& S, BRepGProp_Domain& D, const gp_Pnt& O, const gp_Pnt& VLocation);
  

  //! Computes the global properties of the region of 3D space
  //! delimited with the surface <S> and the point VLocation.
  //! Adaptive 2D Gauss integration is used.
  //! Parameter Eps sets maximal relative error of computed mass (volume) for face.
  //! Error is calculated as Abs((M(i+1)-M(i))/M(i+1)), M(i+1) and M(i) are values
  //! for two successive steps of adaptive integration.
  //! WARNING: if Eps > 0.001 algorithm performs non-adaptive integration.
  Standard_EXPORT BRepGProp_Vinert(BRepGProp_Face& S, BRepGProp_Domain& D, const gp_Pnt& O, const gp_Pnt& VLocation, const Standard_Real Eps);
  

  //! Computes the global properties of the region of 3D space
  //! delimited with the surface <S> and the plane Pln.
  //! The method is quick and its precision is enough for many cases of analytical
  //! surfaces.
  //! Non-adaptive 2D Gauss integration with predefined numbers of Gauss points
  //! is used. Numbers of points depend on types of surfaces and  curves.
  //! Error of the computation is not calculated.
  Standard_EXPORT BRepGProp_Vinert(BRepGProp_Face& S, BRepGProp_Domain& D, const gp_Pln& Pl, const gp_Pnt& VLocation);
  

  //! Computes the global properties of the region of 3D space
  //! delimited with the surface <S> and the plane Pln.
  //! Adaptive 2D Gauss integration is used.
  //! Parameter Eps sets maximal relative error of computed mass (volume) for face.
  //! Error is calculated as Abs((M(i+1)-M(i))/M(i+1)), M(i+1) and M(i) are values
  //! for two successive steps of adaptive integration.
  //! WARNING: if Eps > 0.001 algorithm performs non-adaptive integration.
  Standard_EXPORT BRepGProp_Vinert(BRepGProp_Face& S, BRepGProp_Domain& D, const gp_Pln& Pl, const gp_Pnt& VLocation, const Standard_Real Eps);
  
  Standard_EXPORT void SetLocation (const gp_Pnt& VLocation);
  
  Standard_EXPORT void Perform (const BRepGProp_Face& S);
  
  Standard_EXPORT Standard_Real Perform (BRepGProp_Face& S, const Standard_Real Eps);
  
  Standard_EXPORT void Perform (const BRepGProp_Face& S, const gp_Pnt& O);
  
  Standard_EXPORT Standard_Real Perform (BRepGProp_Face& S, const gp_Pnt& O, const Standard_Real Eps);
  
  Standard_EXPORT void Perform (const BRepGProp_Face& S, const gp_Pln& Pl);
  
  Standard_EXPORT Standard_Real Perform (BRepGProp_Face& S, const gp_Pln& Pl, const Standard_Real Eps);
  
  Standard_EXPORT void Perform (BRepGProp_Face& S, BRepGProp_Domain& D);
  
  Standard_EXPORT Standard_Real Perform (BRepGProp_Face& S, BRepGProp_Domain& D, const Standard_Real Eps);
  
  Standard_EXPORT void Perform (BRepGProp_Face& S, BRepGProp_Domain& D, const gp_Pnt& O);
  
  Standard_EXPORT Standard_Real Perform (BRepGProp_Face& S, BRepGProp_Domain& D, const gp_Pnt& O, const Standard_Real Eps);
  
  Standard_EXPORT void Perform (BRepGProp_Face& S, BRepGProp_Domain& D, const gp_Pln& Pl);
  
  Standard_EXPORT Standard_Real Perform (BRepGProp_Face& S, BRepGProp_Domain& D, const gp_Pln& Pl, const Standard_Real Eps);
  

  //! If previously used methods containe Eps parameter
  //! gets actual relative error of the computation, else returns  1.0.
  Standard_EXPORT Standard_Real GetEpsilon();




protected:





private:



  Standard_Real myEpsilon;


};







#endif // _BRepGProp_Vinert_HeaderFile
