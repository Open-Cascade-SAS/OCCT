// Created on: 1998-04-08
// Created by: Philippe MANGIN
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _BRepOffsetAPI_MakePipeShell_HeaderFile
#define _BRepOffsetAPI_MakePipeShell_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepPrimAPI_MakeSweep.hxx>
#include <Standard_Boolean.hxx>
#include <BRepFill_TypeOfContact.hxx>
#include <BRepBuilderAPI_PipeError.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <BRepBuilderAPI_TransitionMode.hxx>
#include <TopTools_ListOfShape.hxx>
class BRepFill_PipeShell;
class Standard_DomainError;
class StdFail_NotDone;
class TopoDS_Wire;
class gp_Ax2;
class gp_Dir;
class TopoDS_Shape;
class TopoDS_Vertex;
class Law_Function;


//! This class provides for a framework to construct a shell
//! or a solid along a spine consisting in a wire.
//! To produce a solid, the initial wire must be closed.
//! Two approaches are used:
//! - definition by section
//! - by a section and a scaling law
//! - by addition of successive intermediary sections
//! - definition by sweep mode.
//! - pseudo-Frenet
//! - constant
//! - binormal constant
//! - normal defined by a surface support
//! - normal defined by a guiding contour.
//! The two global approaches can also be combined.
//! You can also close the surface later in order to form a solid.
//! Warning: some limitations exist
//! -- Mode with auxilary spine is incompatible with hometetic laws
//! -- Mode with auxilary spine and keep contact produce only CO surface.
class BRepOffsetAPI_MakePipeShell  : public BRepPrimAPI_MakeSweep
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructs the shell-generating framework defined by the wire Spine.
  //! Sets an sweep's mode
  //! If no mode are setted, the mode use in MakePipe is used
  Standard_EXPORT BRepOffsetAPI_MakePipeShell(const TopoDS_Wire& Spine);
  
  //! Sets a Frenet or a CorrectedFrenet trihedron
  //! to  perform  the  sweeping
  //! If IsFrenet is false, a corrected Frenet trihedron is used.
  Standard_EXPORT void SetMode (const Standard_Boolean IsFrenet = Standard_False);
  
  //! Sets a Discrete trihedron
  //! to  perform  the  sweeping
  Standard_EXPORT void SetDiscreteMode();
  
  //! Sets  a  fixed  trihedron  to  perform  the  sweeping
  //! all sections will be parallel.
  Standard_EXPORT void SetMode (const gp_Ax2& Axe);
  
  //! Sets a fixed BiNormal  direction to perform the --
  //! sweeping.   Angular   relations   beetween  the
  //! section(s) and <BiNormal> will be constant
  Standard_EXPORT void SetMode (const gp_Dir& BiNormal);
  
  //! Sets support to the spine to define the BiNormal of
  //! the trihedron, like the normal  to the surfaces.
  //! Warning:  To be effective, Each  edge of the <spine> must
  //! have an representaion on one face of<SpineSupport>
  Standard_EXPORT Standard_Boolean SetMode (const TopoDS_Shape& SpineSupport);
  
  //! Sets  an  auxiliary  spine  to  define  the Normal
  //! For  each  Point  of  the  Spine  P,  an  Point  Q  is  evalued
  //! on  <AuxiliarySpine>
  //! If <CurvilinearEquivalence>
  //! Q split <AuxiliarySpine> with  the  same  length ratio
  //! than P split  <Spline>.
  //! Else  the  plan  define  by  P  and  the  tangent  to  the  <Spine>
  //! intersect <AuxiliarySpine> in Q.
  //! If <KeepContact> equals BRepFill_NoContact: The Normal is defined
  //! by the vector PQ.
  //! If <KeepContact> equals BRepFill_Contact: The Normal is defined to
  //! achieve that the sweeped section is in contact to the
  //! auxiliarySpine. The width of section is constant all along the path.
  //! In other words, the auxiliary spine lies on the swept surface,
  //! but not necessarily is a boundary of this surface. However,
  //! the auxiliary spine has to be close enough to the main spine
  //! to provide intersection with any section all along the path.
  //! If <KeepContact> equals BRepFill_ContactOnBorder: The auxiliary spine
  //! becomes a boundary of the swept surface and the width of section varies
  //! along the path.
  //! Give section to sweep.
  //! Possibilities are :
  //! - Give one or sevral section
  //! - Give one profile and an homotetic law.
  //! - Automatic compute of correspondance beetween spine, and section
  //! on the sweeped shape
  //! - correspondance beetween spine, and section on the sweeped shape
  //! defined by a vertex of the spine
  Standard_EXPORT void SetMode (const TopoDS_Wire& AuxiliarySpine, const Standard_Boolean CurvilinearEquivalence, const BRepFill_TypeOfContact KeepContact = BRepFill_NoContact);
  
  //! Adds the section Profile to this framework. First and last
  //! sections may be punctual, so the shape Profile may be
  //! both wire and vertex. Correspondent point on spine is
  //! computed automatically.
  //! If WithContact is true, the section is translated to be in
  //! contact with the spine.
  //! If WithCorrection is true, the section is rotated to be
  //! orthogonal to the spine?s tangent in the correspondent
  //! point. This option has no sense if the section is punctual
  //! (Profile is of type TopoDS_Vertex).
  Standard_EXPORT void Add (const TopoDS_Shape& Profile, const Standard_Boolean WithContact = Standard_False, const Standard_Boolean WithCorrection = Standard_False);
  
  //! Adds the section Profile to this framework.
  //! Correspondent point on the spine is given by Location.
  //! Warning:
  //! To be effective, it is not recommended to combine methods Add and SetLaw.
  Standard_EXPORT void Add (const TopoDS_Shape& Profile, const TopoDS_Vertex& Location, const Standard_Boolean WithContact = Standard_False, const Standard_Boolean WithCorrection = Standard_False);
  
  //! Sets the evolution law defined by the wire Profile with
  //! its position (Location, WithContact, WithCorrection
  //! are the same options as in methods Add) and a
  //! homotetic law defined by the function L.
  //! Warning:
  //! To be effective, it is not recommended to combine methods Add and SetLaw.
  Standard_EXPORT void SetLaw (const TopoDS_Shape& Profile, const Handle(Law_Function)& L, const Standard_Boolean WithContact = Standard_False, const Standard_Boolean WithCorrection = Standard_False);
  
  //! Sets the evolution law defined by the wire Profile with
  //! its position (Location, WithContact, WithCorrection
  //! are the same options as in methods Add) and a
  //! homotetic law defined by the function L.
  //! Warning:
  //! To be effective, it is not recommended to combine methods Add and SetLaw.
  Standard_EXPORT void SetLaw (const TopoDS_Shape& Profile, const Handle(Law_Function)& L, const TopoDS_Vertex& Location, const Standard_Boolean WithContact = Standard_False, const Standard_Boolean WithCorrection = Standard_False);
  
  //! Removes the section Profile from this framework.
  Standard_EXPORT void Delete (const TopoDS_Shape& Profile);
  
  //! Returns true if this tool object is ready to build the
  //! shape, i.e. has a definition for the wire section Profile.
  Standard_EXPORT Standard_Boolean IsReady() const;
  
  //! Get a status, when Simulate or Build failed.       It can be
  //! BRepBuilderAPI_PipeDone,
  //! BRepBuilderAPI_PipeNotDone,
  //! BRepBuilderAPI_PlaneNotIntersectGuide,
  //! BRepBuilderAPI_ImpossibleContact.
  Standard_EXPORT BRepBuilderAPI_PipeError GetStatus() const;
  
  //! Sets the following tolerance values
  //! - 3D tolerance Tol3d
  //! - boundary tolerance BoundTol
  //! - angular tolerance TolAngular.
  Standard_EXPORT void SetTolerance (const Standard_Real Tol3d = 1.0e-4, const Standard_Real BoundTol = 1.0e-4, const Standard_Real TolAngular = 1.0e-2);
  
  //! Define the maximum V degree of resulting surface
  Standard_EXPORT void SetMaxDegree (const Standard_Integer NewMaxDegree);
  
  //! Define the maximum number of spans in V-direction
  //! on resulting surface
  Standard_EXPORT void SetMaxSegments (const Standard_Integer NewMaxSegments);
  
  //! Set the flag that indicates attempt to approximate
  //! a C1-continuous surface if a swept surface proved
  //! to be C0.
  Standard_EXPORT void SetForceApproxC1 (const Standard_Boolean ForceApproxC1);
  
  //! Sets the transition mode to manage discontinuities on
  //! the swept shape caused by fractures on the spine. The
  //! transition mode can be BRepBuilderAPI_Transformed
  //! (default value), BRepBuilderAPI_RightCorner,
  //! BRepBuilderAPI_RoundCorner:
  //! -              RepBuilderAPI_Transformed:
  //! discontinuities are treated by
  //! modification of the sweeping mode. The
  //! pipe is "transformed" at the fractures of
  //! the spine. This mode assumes building a
  //! self-intersected shell.
  //! -              BRepBuilderAPI_RightCorner:
  //! discontinuities are treated like right
  //! corner. Two pieces of the pipe
  //! corresponding to two adjacent
  //! segments of the spine are extended
  //! and intersected at a fracture of the spine.
  //! -              BRepBuilderAPI_RoundCorner:
  //! discontinuities are treated like round
  //! corner. The corner is treated as rotation
  //! of the profile around an axis which
  //! passes through the point of the spine?s
  //! fracture. This axis is based on cross
  //! product of directions tangent to the
  //! adjacent segments of the spine at their common point.
  //! Warnings
  //! The mode BRepBuilderAPI_RightCorner provides a
  //! valid result if intersection of two pieces of the pipe
  //! (corresponding to two adjacent segments of the spine)
  //! in the neighborhood of the spine?s fracture is
  //! connected and planar. This condition can be violated if
  //! the spine is non-linear in some neighborhood of the
  //! fracture or if the profile was set with a scaling law.
  //! The last mode, BRepBuilderAPI_RoundCorner, will
  //! assuredly provide a good result only if a profile was set
  //! with option WithCorrection = True, i.e. it is strictly
  //! orthogonal to the spine.
  Standard_EXPORT void SetTransitionMode (const BRepBuilderAPI_TransitionMode Mode = BRepBuilderAPI_Transformed);
  
  //! Simulates the resulting shape by calculating its
  //! cross-sections. The spine is devided by this
  //! cross-sections into (NumberOfSection - 1) equal
  //! parts, the number of cross-sections is
  //! NumberOfSection. The cross-sections are wires and
  //! they are returned in the list Result.
  //! This gives a rapid preview of the resulting shape,
  //! which will be obtained using the settings you have provided.
  //! Raises  NotDone if  <me> it is not Ready
  Standard_EXPORT void Simulate (const Standard_Integer NumberOfSection, TopTools_ListOfShape& Result);
  
  //! Builds the resulting shape (redefined from MakeShape).
  Standard_EXPORT virtual void Build() Standard_OVERRIDE;
  
  //! Transforms the sweeping Shell in Solid.
  //! If a propfile is not closed returns False
  Standard_EXPORT Standard_Boolean MakeSolid();
  
  //! Returns the  TopoDS  Shape of the bottom of the sweep.
  Standard_EXPORT virtual TopoDS_Shape FirstShape() Standard_OVERRIDE;
  
  //! Returns the TopoDS Shape of the top of the sweep.
  Standard_EXPORT virtual TopoDS_Shape LastShape() Standard_OVERRIDE;
  
  //! Returns a list of new shapes generated from the shape
  //! S by the shell-generating algorithm.
  //! This function is redefined from BRepOffsetAPI_MakeShape::Generated.
  //! S can be an edge or a vertex of a given Profile (see methods Add).
  Standard_EXPORT virtual const TopTools_ListOfShape& Generated (const TopoDS_Shape& S) Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Real ErrorOnSurface() const;




protected:





private:



  Handle(BRepFill_PipeShell) myPipe;


};







#endif // _BRepOffsetAPI_MakePipeShell_HeaderFile
