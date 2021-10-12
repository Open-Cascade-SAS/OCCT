// Created on: 1997-10-13
// Created by: Roman BORISOV
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _BRepOffsetAPI_NormalProjection_HeaderFile
#define _BRepOffsetAPI_NormalProjection_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepAlgo_NormalProjection.hxx>
#include <BRepBuilderAPI_MakeShape.hxx>
#include <Standard_Real.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <TopTools_ListOfShape.hxx>
class TopoDS_Shape;
class TopoDS_Edge;


//! A framework to define projection onto a shape
//! according to the normal from each point to be projected.
//! The target shape is a face, and the source shape is an edge or a wire.
class BRepOffsetAPI_NormalProjection  : public BRepBuilderAPI_MakeShape
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructs an empty framework to define projection on
  //! a shape according to the normal from each point to be
  //! projected to the shape.
  Standard_EXPORT BRepOffsetAPI_NormalProjection();
  
  //! Constructs a framework to define projection onto the
  //! basis shape S according to the normal from each point
  //! to be projected from the shape added to this framework by Add.
  //! Default parameters of the algorithm: Tol3D = 1.e-04, Tol2D =sqr(tol3d)
  //! , InternalContinuity = GeomAbs_C2, MaxDegree = 14, MaxSeg = 16.
  Standard_EXPORT BRepOffsetAPI_NormalProjection(const TopoDS_Shape& S);
  
  //! Initializes the empty constructor framework with the shape S.
  Standard_EXPORT void Init (const TopoDS_Shape& S);
  
  //! Adds the shape ToProj to the framework for calculation
  //! of the projection by Compute3d.
  //! ToProj is an edge or a wire and will be projected onto the basis shape.
  //! Exceptions
  //! Standard_ConstructionError if ToProj is not added.
  Standard_EXPORT void Add (const TopoDS_Shape& ToProj);
  
  //! Sets the parameters  used  for computation
  //! Tol3 is the required  tolerance between the  3d projected
  //! curve  and     its    2d    representation
  //! InternalContinuity  is the order of constraints
  //! used for  approximation
  //! MaxDeg and MaxSeg are the maximum degree and the maximum
  //! number of segment for BSpline resulting of an approximation.
  Standard_EXPORT void SetParams (const Standard_Real Tol3D, const Standard_Real Tol2D, const GeomAbs_Shape InternalContinuity, const Standard_Integer MaxDegree, const Standard_Integer MaxSeg);
  
  //! Sets the maximum distance between target shape and
  //! shape to project. If this condition is not satisfied then corresponding
  //! part of solution is discarded.
  //! if MaxDist < 0 then this method does not affect the algorithm
  Standard_EXPORT void SetMaxDistance (const Standard_Real MaxDist);
  
  //! Manage  limitation  of  projected  edges.
  Standard_EXPORT void SetLimit (const Standard_Boolean FaceBoundaries = Standard_True);
  
  //! Returns true if a 3D curve is computed. If not, false is
  //! returned and an initial 3D curve is kept to build the resulting edges.
  Standard_EXPORT void Compute3d (const Standard_Boolean With3d = Standard_True);
  
  //! Builds the result of the projection as a compound of
  //! wires. Tries to build oriented wires.
  Standard_EXPORT virtual void Build(const Message_ProgressRange& theRange = Message_ProgressRange()) Standard_OVERRIDE;
  
  //! Returns true if the object was correctly built by the shape
  //! construction algorithm.
  //! If at the construction time of the shape, the algorithm
  //! cannot be completed, or the original data is corrupted,
  //! IsDone returns false and therefore protects the use of
  //! functions to access the result of the construction
  //! (typically the Shape function).
  Standard_EXPORT Standard_Boolean IsDone() const Standard_OVERRIDE;
  
  //! Performs the projection.
  //! The construction of the result is performed by Build.
  //! Exceptions
  //! StdFail_NotDone if the projection was not performed.
  Standard_EXPORT const TopoDS_Shape& Projection() const;
  
  //! Returns the initial face corresponding to the projected edge E.
  //! Exceptions
  //! StdFail_NotDone if no face was found.
  //! Standard_NoSuchObject if a face corresponding to
  //! E has already been found.
  Standard_EXPORT const TopoDS_Shape& Couple (const TopoDS_Edge& E) const;
  
  //! Returns the  list   of shapes generated   from the
  //! shape <S>.
  Standard_EXPORT virtual const TopTools_ListOfShape& Generated (const TopoDS_Shape& S) Standard_OVERRIDE;
  
  //! Returns the initial edge corresponding to the edge E
  //! resulting from the computation of the projection.
  //! Exceptions
  //! StdFail_NotDone if no edge was found.
  //! Standard_NoSuchObject if an edge corresponding to
  //! E has already been found.
  Standard_EXPORT const TopoDS_Shape& Ancestor (const TopoDS_Edge& E) const;
  
  //! build the result as a list of wire if possible in --
  //! a first returns a wire only if there is only a wire.
  Standard_EXPORT Standard_Boolean BuildWire (TopTools_ListOfShape& Liste) const;




protected:





private:



  BRepAlgo_NormalProjection myNormalProjector;


};







#endif // _BRepOffsetAPI_NormalProjection_HeaderFile
