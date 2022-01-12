// Created on: 1994-10-03
// Created by: Bruno DUMORTIER
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _BRepFill_Evolved_HeaderFile
#define _BRepFill_Evolved_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Shape.hxx>
#include <GeomAbs_JoinType.hxx>
#include <BRepFill_DataMapOfShapeDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
class gp_Ax3;
class BRepMAT2d_BisectingLocus;
class BRepMAT2d_LinkTopoBilo;
class BRepTools_Quilt;
class TopLoc_Location;
class TopoDS_Edge;
class TopoDS_Vertex;


//! Constructs an evolved volume from a spine (wire or face)
//! and  a profile ( wire).
class BRepFill_Evolved 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BRepFill_Evolved();
  
  //! Creates an evolved shape by sweeping the <Profile>
  //! along the  <Spine>.  <AxeProf> is  used to set the
  //! position of <Profile> along  <Spine> as  follows:
  //! <AxeProf> slides on the profile with direction
  //! colinear to the normal to <Spine>, and its
  //! <XDirection> mixed with the tangent to <Spine>.
  Standard_EXPORT BRepFill_Evolved(const TopoDS_Wire& Spine, const TopoDS_Wire& Profile, const gp_Ax3& AxeProf, const GeomAbs_JoinType Join = GeomAbs_Arc, const Standard_Boolean Solid = Standard_False);
  
  //! Creates an  evolved shape  by sweeping the <Profile>
  //! along the <Spine>
  Standard_EXPORT BRepFill_Evolved(const TopoDS_Face& Spine, const TopoDS_Wire& Profile, const gp_Ax3& AxeProf, const GeomAbs_JoinType Join = GeomAbs_Arc, const Standard_Boolean Solid = Standard_False);
  
  //! Performs an  evolved shape  by sweeping the <Profile>
  //! along the <Spine>
  Standard_EXPORT void Perform (const TopoDS_Wire& Spine, const TopoDS_Wire& Profile, const gp_Ax3& AxeProf, const GeomAbs_JoinType Join = GeomAbs_Arc, const Standard_Boolean Solid = Standard_False);
  
  //! Performs an  evolved shape  by sweeping the <Profile>
  //! along the <Spine>
  Standard_EXPORT void Perform (const TopoDS_Face& Spine, const TopoDS_Wire& Profile, const gp_Ax3& AxeProf, const GeomAbs_JoinType Join = GeomAbs_Arc, const Standard_Boolean Solid = Standard_False);
  
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! returns the generated shape.
  Standard_EXPORT const TopoDS_Shape& Shape() const;
  
  //! Returns   the  shapes  created  from   a  subshape
  //! <SpineShape>  of     the  spine   and   a subshape
  //! <ProfShape> on the profile.
  Standard_EXPORT const TopTools_ListOfShape& GeneratedShapes (const TopoDS_Shape& SpineShape, const TopoDS_Shape& ProfShape) const;
  
  Standard_EXPORT GeomAbs_JoinType JoinType() const;
  
  //! Return the face Top if <Solid> is True in the constructor.
  Standard_EXPORT const TopoDS_Shape& Top() const;
  
  //! Return the face Bottom  if <Solid> is True in the constructor.
  Standard_EXPORT const TopoDS_Shape& Bottom() const;




protected:





private:

  
  Standard_EXPORT void PrivatePerform (const TopoDS_Face& Spine, const TopoDS_Wire& Profile, const gp_Ax3& AxeProf, const GeomAbs_JoinType Join = GeomAbs_Arc, const Standard_Boolean Solid = Standard_False);
  
  Standard_EXPORT void SetWork (const TopoDS_Face& Spine, const TopoDS_Wire& Profile);
  
  Standard_EXPORT void ElementaryPerform (const TopoDS_Face& Spine, const TopoDS_Wire& Profile, const BRepMAT2d_BisectingLocus& Locus, BRepMAT2d_LinkTopoBilo& Link, const GeomAbs_JoinType Join = GeomAbs_Arc);
  
  Standard_EXPORT void PlanarPerform (const TopoDS_Face& Spine, const TopoDS_Wire& Profile, const BRepMAT2d_BisectingLocus& Locus, BRepMAT2d_LinkTopoBilo& Link, const GeomAbs_JoinType Join = GeomAbs_Arc);
  
  Standard_EXPORT void VerticalPerform (const TopoDS_Face& Spine, const TopoDS_Wire& Profile, const BRepMAT2d_BisectingLocus& Locus, BRepMAT2d_LinkTopoBilo& Link, const GeomAbs_JoinType Join = GeomAbs_Arc);
  
  Standard_EXPORT BRepFill_DataMapOfShapeDataMapOfShapeListOfShape& Generated();
  
  Standard_EXPORT void Add (BRepFill_Evolved& Vevo, const TopoDS_Wire& Prof, BRepTools_Quilt& Glue);
  
  Standard_EXPORT TopoDS_Shape& ChangeShape();
  
  Standard_EXPORT void Transfert (BRepFill_Evolved& Vevo, const TopTools_DataMapOfShapeShape& MapProf, const TopTools_DataMapOfShapeShape& MapSpine, const TopLoc_Location& LS, const TopLoc_Location& InitLS, const TopLoc_Location& InitLP);
  
  //! Prepare the profil as follow
  //! - Project the profile in the yOz Plane
  //! - Cut the profile at the extrema of distance from the
  //! Profile to the Oz Axis.
  //! - building the new wires with the cutting edges.
  Standard_EXPORT void PrepareProfile (TopTools_ListOfShape& WorkProf, TopTools_DataMapOfShapeShape& MapProf) const;
  
  //! Prepare the spine as follow
  //! - Cut the spine-Edges at the extrema of curvature and
  //! at the inflexion points.
  Standard_EXPORT void PrepareSpine (TopoDS_Face& WorkSpine, TopTools_DataMapOfShapeShape& SpineProf) const;
  
  Standard_EXPORT void MakePipe (const TopoDS_Edge& SpineEdge, const gp_Ax3& ProfRef);
  
  Standard_EXPORT void MakeRevol (const TopoDS_Edge& SpineEdge, const TopoDS_Vertex& SpineVertex, const gp_Ax3& ProfRef);
  
  //! Find the location transforming the planar shape <Shape>
  //! in the plane xOy
  //! if the Shape is not planar.
  Standard_EXPORT TopLoc_Location FindLocation (const TopoDS_Face& Face) const;
  
  //! Apply the Location <LS> to <mySpine> and  <LP> to
  //! <myProfil>
  //! in order to set the Shapes in the work space.
  Standard_EXPORT void TransformInitWork (const TopLoc_Location& LS, const TopLoc_Location& LP);
  
  Standard_EXPORT void ContinuityOnOffsetEdge (const TopTools_ListOfShape& WorkProf);
  
  Standard_EXPORT void AddTopAndBottom (BRepTools_Quilt& Glue);
  
  Standard_EXPORT void MakeSolid();


  TopoDS_Face mySpine;
  TopoDS_Wire myProfile;
  TopoDS_Shape myShape;
  Standard_Boolean myIsDone;
  Standard_Boolean mySpineType;
  GeomAbs_JoinType myJoinType;
  BRepFill_DataMapOfShapeDataMapOfShapeListOfShape myMap;
  TopoDS_Shape myTop;
  TopoDS_Shape myBottom;


};







#endif // _BRepFill_Evolved_HeaderFile
