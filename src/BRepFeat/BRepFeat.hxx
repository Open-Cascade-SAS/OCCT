// Created on: 1995-06-13
// Created by: Jacques GOUSSARD
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _BRepFeat_HeaderFile
#define _BRepFeat_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColgp_SequenceOfPnt.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <TopAbs_Orientation.hxx>
#include <Standard_OStream.hxx>
#include <BRepFeat_StatusError.hxx>
class TopoDS_Shape;
class gp_Pnt;
class Geom_Curve;
class TopoDS_Face;
class BRepTopAdaptor_FClass2d;
class Geom2dAdaptor_Curve;
class TopoDS_Solid;


//! BRepFeat is necessary for the
//! creation and manipulation of both form and mechanical features in a
//! Boundary Representation framework. Form features can be depressions or
//! protrusions and include the following types:
//! -          Cylinder
//! -          Draft Prism
//! -          Prism
//! -          Revolved feature
//! -          Pipe
//! Depending on whether you wish to make a depression or a protrusion,
//! you can choose your operation type between the following:
//! - removing matter (a Boolean cut: Fuse setting 0)
//! - adding matter (Boolean fusion: Fuse setting 1)
//! The semantics of form feature creation is based on the
//! construction of shapes:
//! -          for a certain length in a certain direction
//! -          up to a limiting face
//! -          from a limiting face at a height
//! -          above and/or below a plane
//! The shape defining the construction of a feature can be either a
//! supporting edge or a concerned area of a face.
//! In case of supporting edge, this contour can be attached to a face
//! of the basis shape by binding. When the contour is bound to this face,
//! the information that the contour will slide on the face becomes
//! available to the relevant class methods. In case of the concerned
//! area of a face, you could, for example, cut it out and move it at
//! a different height, which will define the limiting face of a
//! protrusion or depression. Topological definition with local
//! operations of this sort makes calculations simpler and faster
//! than a global operation. The latter would entail a second phase of
//! removing unwanted matter to get the same result.
//! Mechanical features include ribs - protrusions - and grooves (or
//! slots) - depressions along planar (linear) surfaces or revolution surfaces.
//! The semantics of mechanical features is based on giving
//! thickness to a contour. This thickness can either be unilateral
//! - on one side of the contour - or bilateral - on both sides. As in
//! the semantics of form features, the thickness is defined by
//! construction of shapes in specific contexts.
//! However, in case of mechanical features, development contexts
//! differ. Here they include extrusion:
//! -          to a limiting face of the basis shape
//! -          to or from a limiting plane
//! -          to a height.
class BRepFeat 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static void SampleEdges (const TopoDS_Shape& S, TColgp_SequenceOfPnt& Pt);
  
  Standard_EXPORT static void Barycenter (const TopoDS_Shape& S, gp_Pnt& Pt);
  
  Standard_EXPORT static Standard_Real ParametricBarycenter (const TopoDS_Shape& S, const Handle(Geom_Curve)& C);
  
  //! Ori = True taking account the orientation
  Standard_EXPORT static void ParametricMinMax (const TopoDS_Shape& S, const Handle(Geom_Curve)& C, Standard_Real& prmin, Standard_Real& prmax, Standard_Real& prbmin, Standard_Real& prbmax, Standard_Boolean& flag, const Standard_Boolean Ori = Standard_False);
  
  Standard_EXPORT static Standard_Boolean IsInside (const TopoDS_Face& F1, const TopoDS_Face& F2);
  
  Standard_EXPORT static Standard_Boolean IsInOut (const BRepTopAdaptor_FClass2d& FC, const Geom2dAdaptor_Curve& AC);
  
  Standard_EXPORT static void FaceUntil (const TopoDS_Shape& S, TopoDS_Face& F);
  
  Standard_EXPORT static TopoDS_Solid Tool (const TopoDS_Shape& SRef, const TopoDS_Face& Fac, const TopAbs_Orientation Orf);
  
  //! Prints the Error description of the State <St> as a String on
  //! the Stream <S> and returns <S>.
  Standard_EXPORT static Standard_OStream& Print (const BRepFeat_StatusError SE, Standard_OStream& S);




protected:





private:




friend class BRepFeat_Builder;
friend class BRepFeat_MakeCylindricalHole;
friend class BRepFeat_SplitShape;
friend class BRepFeat_Form;
friend class BRepFeat_RibSlot;
friend class BRepFeat_MakePrism;
friend class BRepFeat_MakeRevol;
friend class BRepFeat_MakePipe;
friend class BRepFeat_Gluer;
friend class BRepFeat_MakeDPrism;
friend class BRepFeat_MakeLinearForm;
friend class BRepFeat_MakeRevolutionForm;

};







#endif // _BRepFeat_HeaderFile
