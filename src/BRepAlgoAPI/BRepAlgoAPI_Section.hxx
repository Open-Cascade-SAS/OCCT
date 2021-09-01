// Created on: 1994-02-18
// Created by: Remi LEQUETTE
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

#ifndef _BRepAlgoAPI_Section_HeaderFile
#define _BRepAlgoAPI_Section_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <BRepAlgoAPI_BooleanOperation.hxx>
class BOPAlgo_PaveFiller;
class TopoDS_Shape;
class gp_Pln;
class Geom_Surface;



//! The algorithm is to build a Section operation between arguments and tools.
//! The result of Section operation consists of vertices and edges.
//! The result of Section operation contains:
//! 1. new vertices that are subjects of V/V, E/E, E/F, F/F interferences
//! 2. vertices that are subjects of V/E, V/F interferences
//! 3. new edges that are subjects of F/F interferences
//! 4. edges that are Common Blocks
class BRepAlgoAPI_Section  : public BRepAlgoAPI_BooleanOperation
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor
  Standard_EXPORT BRepAlgoAPI_Section();
Standard_EXPORT virtual ~BRepAlgoAPI_Section();
  
  //! Empty constructor
  //! <PF> - PaveFiller object that is carried out
  Standard_EXPORT BRepAlgoAPI_Section(const BOPAlgo_PaveFiller& PF);
  
  //! Constructor with two shapes
  //! <S1>  -argument
  //! <S2>  -tool
  //! <PerformNow> - the flag:
  //! if <PerformNow>=True - the algorithm is performed immediately
  //! Obsolete
  Standard_EXPORT BRepAlgoAPI_Section(const TopoDS_Shape& S1, const TopoDS_Shape& S2, const Standard_Boolean PerformNow = Standard_True);
  
  //! Constructor with two shapes
  //! <S1>  -argument
  //! <S2>  -tool
  //! <PF> - PaveFiller object that is carried out
  //! <PerformNow> - the flag:
  //! if <PerformNow>=True - the algorithm is performed immediately
  //! Obsolete
  Standard_EXPORT BRepAlgoAPI_Section(const TopoDS_Shape& S1, const TopoDS_Shape& S2, const BOPAlgo_PaveFiller& aDSF, const Standard_Boolean PerformNow = Standard_True);
  
  //! Constructor with two shapes
  //! <S1>  - argument
  //! <Pl>  - tool
  //! <PerformNow> - the flag:
  //! if <PerformNow>=True - the algorithm is performed immediately
  //! Obsolete
  Standard_EXPORT BRepAlgoAPI_Section(const TopoDS_Shape& S1, const gp_Pln& Pl, const Standard_Boolean PerformNow = Standard_True);
  
  //! Constructor with two shapes
  //! <S1>  - argument
  //! <Sf>  - tool
  //! <PerformNow> - the flag:
  //! if <PerformNow>=True - the algorithm is performed immediately
  //! Obsolete
  Standard_EXPORT BRepAlgoAPI_Section(const TopoDS_Shape& S1, const Handle(Geom_Surface)& Sf, const Standard_Boolean PerformNow = Standard_True);
  
  //! Constructor with two shapes
  //! <Sf>  - argument
  //! <S2>  - tool
  //! <PerformNow> - the flag:
  //! if <PerformNow>=True - the algorithm is performed immediately
  //! Obsolete
  Standard_EXPORT BRepAlgoAPI_Section(const Handle(Geom_Surface)& Sf, const TopoDS_Shape& S2, const Standard_Boolean PerformNow = Standard_True);
  
  //! Constructor with two shapes
  //! <Sf1>  - argument
  //! <Sf2>  - tool
  //! <PerformNow> - the flag:
  //! if <PerformNow>=True - the algorithm is performed immediately
  //! Obsolete
  Standard_EXPORT BRepAlgoAPI_Section(const Handle(Geom_Surface)& Sf1, const Handle(Geom_Surface)& Sf2, const Standard_Boolean PerformNow = Standard_True);
  
  //! initialize the argument
  //! <S1>  - argument
  //! Obsolete
  Standard_EXPORT void Init1 (const TopoDS_Shape& S1);
  
  //! initialize the argument
  //! <Pl>  - argument
  //! Obsolete
  Standard_EXPORT void Init1 (const gp_Pln& Pl);
  
  //! initialize the argument
  //! <Sf>  - argument
  //! Obsolete
  Standard_EXPORT void Init1 (const Handle(Geom_Surface)& Sf);
  
  //! initialize the tool
  //! <S2>  - tool
  //! Obsolete
  Standard_EXPORT void Init2 (const TopoDS_Shape& S2);
  
  //! initialize the tool
  //! <Pl>  - tool
  //! Obsolete
  Standard_EXPORT void Init2 (const gp_Pln& Pl);
  
  //! initialize the tool
  //! <Sf>  - tool
  //! Obsolete
  Standard_EXPORT void Init2 (const Handle(Geom_Surface)& Sf);
  
  Standard_EXPORT void Approximation (const Standard_Boolean B);
  

  //! Indicates whether the P-Curve should be (or not)
  //! performed on the argument.
  //! By default, no parametric 2D curve (pcurve) is defined for the
  //! edges of the result.
  //! If ComputePCurve1 equals true, further computations performed
  //! to attach an P-Curve in the parametric space of the argument
  //! to the constructed edges.
  //! Obsolete
  Standard_EXPORT void ComputePCurveOn1 (const Standard_Boolean B);
  

  //! Indicates whether the P-Curve should be (or not)
  //! performed on the tool.
  //! By default, no parametric 2D curve (pcurve) is defined for the
  //! edges of the result.
  //! If ComputePCurve1 equals true, further computations performed
  //! to attach an P-Curve in the parametric space of the tool
  //! to the constructed edges.
  //! Obsolete
  Standard_EXPORT void ComputePCurveOn2 (const Standard_Boolean B);
  
  //! Performs the algorithm
  //! Filling interference Data Structure (if it is necessary)
  //! Building the result of the operation.
  Standard_EXPORT virtual void Build(const Message_ProgressRange& theRange = Message_ProgressRange()) Standard_OVERRIDE;
  

  //! get the face of the first part giving section edge <E>.
  //! Returns True on the 3 following conditions :
  //! 1/ <E> is an edge returned by the Shape() metwod.
  //! 2/ First part of section performed is a shape.
  //! 3/ <E> is built on a intersection curve (i.e <E>
  //! is not the result of common edges)
  //! When False, F remains untouched.
  //! Obsolete
  Standard_EXPORT Standard_Boolean HasAncestorFaceOn1 (const TopoDS_Shape& E, TopoDS_Shape& F) const;
  
  //! Identifies the ancestor faces of
  //! the intersection edge E resulting from the last
  //! computation performed in this framework, that is, the faces of
  //! the two original shapes on which the edge E lies:
  //! -      HasAncestorFaceOn1 gives the ancestor face in the first shape, and
  //! -      HasAncestorFaceOn2 gives the ancestor face in the second shape.
  //! These functions return true if an ancestor face F is found, or false if not.
  //! An ancestor face is identifiable for the edge E if the following
  //! conditions are satisfied:
  //! -  the first part on which this algorithm performed its
  //! last computation is a shape, that is, it was not given as
  //! a surface or a plane at the time of construction of this
  //! algorithm or at a later time by the Init1 function,
  //! - E is one of the elementary edges built by the
  //! last computation of this section algorithm.
  //! To use these functions properly, you have to test the returned
  //! Boolean value before using the ancestor face: F is significant
  //! only if the returned Boolean value equals true.
  //! Obsolete
  Standard_EXPORT Standard_Boolean HasAncestorFaceOn2 (const TopoDS_Shape& E, TopoDS_Shape& F) const;


protected:

  
  Standard_EXPORT void Init (const Standard_Boolean PerformNow);
  
  Standard_EXPORT virtual void SetAttributes() Standard_OVERRIDE;


private:

  Standard_Boolean myApprox;
  Standard_Boolean myComputePCurve1;
  Standard_Boolean myComputePCurve2;

};

#endif // _BRepAlgoAPI_Section_HeaderFile
