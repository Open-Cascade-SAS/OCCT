// Created on: 1996-12-16
// Created by: Bruno DUMORTIER
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _BiTgte_Blend_HeaderFile
#define _BiTgte_Blend_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <BRepFill_DataMapOfShapeDataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <BRepOffset_Analyse.hxx>
#include <BRepOffset_DataMapOfShapeOffset.hxx>
#include <BRepAlgo_Image.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TopTools_ListOfShape.hxx>
#include <BiTgte_ContactType.hxx>
#include <TopTools_DataMapOfShapeBox.hxx>
class BRepAlgo_AsDes;
class StdFail_NotDone;
class Standard_OutOfRange;
class TopoDS_Shape;
class TopoDS_Face;
class TopoDS_Edge;
class Geom_Surface;
class Geom_Curve;
class Geom2d_Curve;
class BRepOffset_Offset;
class BRepOffset_Inter3d;


//! Root class
class BiTgte_Blend 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BiTgte_Blend();
  
  //! <S>: Shape to be rounded
  //! <Radius>: radius of the fillet
  //! <Tol>: Tol3d used in approximations
  //! <NUBS>: if true,  generate only NUBS surfaces,
  //! if false, generate analytical surfaces if possible
  Standard_EXPORT BiTgte_Blend(const TopoDS_Shape& S, const Standard_Real Radius, const Standard_Real Tol, const Standard_Boolean NUBS);
  
  Standard_EXPORT void Init (const TopoDS_Shape& S, const Standard_Real Radius, const Standard_Real Tol, const Standard_Boolean NUBS);
  
  //! Clear all the Fields.
  Standard_EXPORT void Clear();
  
  //! Set two faces   of <myShape> on which the  Sphere
  //! must roll.
  Standard_EXPORT void SetFaces (const TopoDS_Face& F1, const TopoDS_Face& F2);
  
  //! Set an edge of <myShape> to be rounded.
  Standard_EXPORT void SetEdge (const TopoDS_Edge& Edge);
  
  //! Set a face on which the fillet must stop.
  Standard_EXPORT void SetStoppingFace (const TopoDS_Face& Face);
  
  //! Compute the generated surfaces.
  //! If <BuildShape> is true, compute the resulting Shape.
  //! If false, only the blending surfaces are computed.
  Standard_EXPORT void Perform (const Standard_Boolean BuildShape = Standard_True);
  
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! returns the result
  Standard_EXPORT const TopoDS_Shape& Shape() const;
  
  //! returns the Number of generated surfaces.
  Standard_EXPORT Standard_Integer NbSurfaces() const;
  
  //! returns the surface of range Index
  Standard_EXPORT Handle(Geom_Surface) Surface (const Standard_Integer Index) const;
  
  //! returns the surface of range Index
  Standard_EXPORT const TopoDS_Face& Face (const Standard_Integer Index) const;
  
  //! set in <LC> all the center lines
  Standard_EXPORT void CenterLines (TopTools_ListOfShape& LC) const;
  
  //! returns  the surface generated  by the centerline.
  //! <CenterLine> may be
  //! - an edge  : generate a pipe.
  //! - a vertex : generate a sphere.
  //! Warning: returns a Null Handle if <CenterLine> generates
  //! no surface.
  Standard_EXPORT Handle(Geom_Surface) Surface (const TopoDS_Shape& CenterLine) const;
  
  //! returns  the face generated  by the centerline.
  //! <CenterLine> may be
  //! - an edge  : generate a pipe.
  //! - a vertex : generate a sphere.
  //! Warning: returns a Null Shape if <CenterLine> generates
  //! no surface.
  Standard_EXPORT const TopoDS_Face& Face (const TopoDS_Shape& CenterLine) const;
  
  //! returns the type of contact
  Standard_EXPORT BiTgte_ContactType ContactType (const Standard_Integer Index) const;
  
  //! gives the first support shape relative to
  //! SurfaceFillet(Index);
  Standard_EXPORT const TopoDS_Shape& SupportShape1 (const Standard_Integer Index) const;
  
  //! gives the second support shape relative to
  //! SurfaceFillet(Index);
  Standard_EXPORT const TopoDS_Shape& SupportShape2 (const Standard_Integer Index) const;
  
  //! gives the 3d curve of SurfaceFillet(Index)
  //! on SupportShape1(Index)
  Standard_EXPORT Handle(Geom_Curve) CurveOnShape1 (const Standard_Integer Index) const;
  
  //! gives the 3d curve of SurfaceFillet(Index)
  //! on SupportShape2(Index)
  Standard_EXPORT Handle(Geom_Curve) CurveOnShape2 (const Standard_Integer Index) const;
  
  //! gives the PCurve associated to CurvOnShape1(Index)
  //! on the support face
  //! Warning: returns a Null Handle if SupportShape1 is not a Face
  Standard_EXPORT Handle(Geom2d_Curve) PCurveOnFace1 (const Standard_Integer Index) const;
  
  //! gives the PCurve associated to CurveOnShape1(Index)
  //! on the Fillet
  Standard_EXPORT Handle(Geom2d_Curve) PCurve1OnFillet (const Standard_Integer Index) const;
  
  //! gives the PCurve  associated to CurveOnShape2(Index)
  //! on the  support face
  //! Warning: returns a Null Handle if SupportShape2 is not a Face
  Standard_EXPORT Handle(Geom2d_Curve) PCurveOnFace2 (const Standard_Integer Index) const;
  
  //! gives the PCurve associated to CurveOnShape2(Index)
  //! on the fillet
  Standard_EXPORT Handle(Geom2d_Curve) PCurve2OnFillet (const Standard_Integer Index) const;
  
  Standard_EXPORT Standard_Integer NbBranches();
  
  //! Set in <From>,<To>   the indices of the faces  of
  //! the branche <Index>.
  //!
  //! i.e: Branche<Index> = Face(From) + Face(From+1) + ..+ Face(To)
  Standard_EXPORT void IndicesOfBranche (const Standard_Integer Index, Standard_Integer& From, Standard_Integer& To) const;
  
  //! Computes the center lines
  Standard_EXPORT void ComputeCenters();




protected:





private:

  
  //! Perform the generated surfaces.
  Standard_EXPORT void ComputeSurfaces();
  
  //! Build the resulting shape
  //! All the faces must be computed
  Standard_EXPORT void ComputeShape();
  
  //! Computes the intersections with <Face> and all the
  //! OffsetFaces stored  in <myMapSF>.  Returns <True>
  //! if an intersections ends on a boundary of a Face.
  Standard_EXPORT Standard_Boolean Intersect (const TopoDS_Shape& Init, const TopoDS_Face& Face, const TopTools_DataMapOfShapeBox& MapSBox, const BRepOffset_Offset& OF1, BRepOffset_Inter3d& Inter);


  Standard_Real myRadius;
  Standard_Real myTol;
  Standard_Boolean myNubs;
  TopoDS_Shape myShape;
  TopoDS_Shape myResult;
  Standard_Boolean myBuildShape;
  TopTools_IndexedDataMapOfShapeListOfShape myAncestors;
  BRepFill_DataMapOfShapeDataMapOfShapeListOfShape myCreated;
  TopTools_DataMapOfShapeListOfShape myCutEdges;
  TopTools_IndexedMapOfShape myFaces;
  TopTools_IndexedMapOfShape myEdges;
  TopTools_MapOfShape myStopFaces;
  BRepOffset_Analyse myAnalyse;
  TopTools_IndexedMapOfShape myCenters;
  BRepOffset_DataMapOfShapeOffset myMapSF;
  BRepAlgo_Image myInitOffsetFace;
  BRepAlgo_Image myImage;
  BRepAlgo_Image myImageOffset;
  Handle(BRepAlgo_AsDes) myAsDes;
  Standard_Integer myNbBranches;
  Handle(TColStd_HArray1OfInteger) myIndices;
  Standard_Boolean myDone;


};







#endif // _BiTgte_Blend_HeaderFile
