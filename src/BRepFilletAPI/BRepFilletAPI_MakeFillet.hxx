// Created on: 1994-06-17
// Created by: Modeling
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

#ifndef _BRepFilletAPI_MakeFillet_HeaderFile
#define _BRepFilletAPI_MakeFillet_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <ChFi3d_FilBuilder.hxx>
#include <TopTools_MapOfShape.hxx>
#include <BRepFilletAPI_LocalOperation.hxx>
#include <ChFi3d_FilletShape.hxx>
#include <Standard_Real.hxx>
#include <GeomAbs_Shape.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <Standard_Integer.hxx>
#include <TopTools_ListOfShape.hxx>
#include <ChFiDS_SecHArray1.hxx>
#include <ChFiDS_ErrorStatus.hxx>
class TopoDS_Shape;
class TopoDS_Edge;
class Law_Function;
class TopoDS_Vertex;
class TopOpeBRepBuild_HBuilder;
class Geom_Surface;


//! Describes functions to build fillets on the broken edges of a shell or solid.
//! A MakeFillet object provides a framework for:
//! -   initializing the construction algorithm with a given shape,
//! -   acquiring the data characterizing the fillets,
//! -   building the fillets and constructing the resulting shape, and
//! -   consulting the result.
class BRepFilletAPI_MakeFillet  : public BRepFilletAPI_LocalOperation
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Initializes   the computation    of   the  fillets.
  //! <FShape> sets   the type   of fillet  surface. The
  //! default value is ChFi3d_Rational (classical  nurbs
  //! representation of  circles).   ChFi3d_QuasiAngular
  //! corresponds to  a  nurbs representation of circles
  //! which   parameterisation matches  the  circle one.
  //! ChFi3d_Polynomial  corresponds to  a    polynomial
  //! representation of circles.
  Standard_EXPORT BRepFilletAPI_MakeFillet(const TopoDS_Shape& S, const ChFi3d_FilletShape FShape = ChFi3d_Rational);
  
  Standard_EXPORT void SetParams (const Standard_Real Tang, const Standard_Real Tesp, const Standard_Real T2d, const Standard_Real TApp3d, const Standard_Real TolApp2d, const Standard_Real Fleche);
  
  //! Changes     the      parameters     of  continiuity
  //! InternalContinuity to produce fillet'surfaces with
  //! an continuity   Ci (i=0,1 or    2).
  //! By defaultInternalContinuity = GeomAbs_C1.
  //! AngularTolerance  is the G1 tolerance between fillet
  //! and support'faces.
  Standard_EXPORT void SetContinuity (const GeomAbs_Shape InternalContinuity, const Standard_Real AngularTolerance);
  
  //! Adds a  fillet contour in  the  builder  (builds a
  //! contour  of tangent edges).
  //! The Radius must be set after.
  Standard_EXPORT void Add (const TopoDS_Edge& E) Standard_OVERRIDE;
  
  //! Adds a  fillet description in  the  builder
  //! - builds a contour  of tangent edges,
  //! - sets the radius.
  Standard_EXPORT void Add (const Standard_Real Radius, const TopoDS_Edge& E);
  
  //! Adds a  fillet description in  the  builder
  //! - builds a contour  of tangent edges,
  //! - sets a linear radius evolution law between
  //! the first and last vertex of the spine.
  Standard_EXPORT void Add (const Standard_Real R1, const Standard_Real R2, const TopoDS_Edge& E);
  
  //! Adds a  fillet description in  the  builder
  //! - builds a contour  of tangent edges,
  //! - sest the radius evolution law.
  Standard_EXPORT void Add (const Handle(Law_Function)& L, const TopoDS_Edge& E);
  
  //! Adds a  fillet description in  the  builder
  //! - builds a contour  of tangent edges,
  //! - sets the radius evolution law interpolating the values
  //! given in the array UandR :
  //!
  //! p2d.X() = relative parameter on the spine [0,1]
  //! p2d.Y() = value of the radius.
  Standard_EXPORT void Add (const TColgp_Array1OfPnt2d& UandR, const TopoDS_Edge& E);
  
  //! Sets the parameters of the fillet
  //! along the contour of index IC generated using the Add function
  //! in the internal data structure of
  //! this algorithm, where Radius is the radius of the fillet.
  Standard_EXPORT void SetRadius (const Standard_Real Radius, const Standard_Integer IC, const Standard_Integer IinC);
  
  //! Sets the parameters of the fillet
  //! along the contour of index IC generated using the Add function
  //! in the internal data structure of this algorithm, where the radius of the
  //! fillet evolves according to a linear evolution law defined
  //! from R1 to R2, between the first and last vertices of the contour of index IC.
  Standard_EXPORT void SetRadius (const Standard_Real R1, const Standard_Real R2, const Standard_Integer IC, const Standard_Integer IinC);
  
  //! Sets the parameters of the fillet
  //! along the contour of index IC generated using the Add function
  //! in the internal data structure of this algorithm, where the radius of the
  //! fillet evolves according to the evolution law L, between the
  //! first and last vertices of the contour of index IC.
  Standard_EXPORT void SetRadius (const Handle(Law_Function)& L, const Standard_Integer IC, const Standard_Integer IinC);
  
  //! Sets the parameters of the fillet
  //! along the contour of index IC generated using the Add function
  //! in the internal data structure of this algorithm,
  //! where the radius of the fillet evolves according to the evolution law
  //! which interpolates the set of parameter and radius pairs given
  //! in the array UandR as follows:
  //! -   the X coordinate of a point in UandR defines a
  //! relative parameter on the contour (i.e. a parameter between 0 and 1),
  //! -          the Y coordinate of a point in UandR gives the
  //! corresponding value of the radius, and the radius evolves
  //! between the first and last vertices of the contour of index IC.
  Standard_EXPORT void SetRadius (const TColgp_Array1OfPnt2d& UandR, const Standard_Integer IC, const Standard_Integer IinC);
  
  //! Erases the radius information on the contour of index
  //! IC in the internal data structure of this algorithm.
  //! Use the SetRadius function to reset this data.
  //! Warning
  //! Nothing is done if IC is outside the bounds of the table of contours.
  Standard_EXPORT void ResetContour (const Standard_Integer IC) Standard_OVERRIDE;
  
  //! Returns true if the radius of the fillet along the contour of index IC
  //! in the internal data structure of this algorithm is constant,
  //! Warning
  //! False is returned if IC is outside the bounds of the table
  //! of contours or if E does not belong to the contour of index IC.
  Standard_EXPORT Standard_Boolean IsConstant (const Standard_Integer IC);
  
  //! Returns the radius of the fillet along the contour of index IC in the
  //! internal data structure of this algorithm
  //! Warning
  //! -   Use this function only if the radius is constant.
  //! -   -1. is returned if IC is outside the bounds of the
  //! table of contours or if E does not belong to the contour of index IC.
  Standard_EXPORT Standard_Real Radius (const Standard_Integer IC);
  
  //! Returns true if the radius of the fillet along the edge E of the
  //! contour of index IC in the internal data structure of
  //! this algorithm is constant.
  //! Warning
  //! False is returned if IC is outside the bounds of the table
  //! of contours or if E does not belong to the contour of index IC.
  Standard_EXPORT Standard_Boolean IsConstant (const Standard_Integer IC, const TopoDS_Edge& E);
  
  //! Returns the radius of the fillet along the edge E of the contour of index
  //! IC in the internal data structure of this algorithm.
  //! Warning
  //! -   Use this function only if the radius is constant.
  //! -   -1 is returned if IC is outside the bounds of the
  //! table of contours or if E does not belong to the contour of index IC.
  Standard_EXPORT Standard_Real Radius (const Standard_Integer IC, const TopoDS_Edge& E);
  
  //! Assigns Radius as the radius of the fillet on the edge E
  Standard_EXPORT void SetRadius (const Standard_Real Radius, const Standard_Integer IC, const TopoDS_Edge& E);
  
  Standard_EXPORT void SetRadius (const Standard_Real Radius, const Standard_Integer IC, const TopoDS_Vertex& V);
  
  Standard_EXPORT Standard_Boolean GetBounds (const Standard_Integer IC, const TopoDS_Edge& E, Standard_Real& F, Standard_Real& L);
  
  Standard_EXPORT Handle(Law_Function) GetLaw (const Standard_Integer IC, const TopoDS_Edge& E);
  
  Standard_EXPORT void SetLaw (const Standard_Integer IC, const TopoDS_Edge& E, const Handle(Law_Function)& L);
  
  //! Assigns FShape as the type of fillet shape built by this algorithm.
  Standard_EXPORT void SetFilletShape (const ChFi3d_FilletShape FShape);
  
  //! Returns the type of fillet shape built by this algorithm.
  Standard_EXPORT ChFi3d_FilletShape GetFilletShape() const;
  
  //! Returns the number of contours generated using the
  //! Add function in the internal data structure of this algorithm.
  Standard_EXPORT Standard_Integer NbContours() const Standard_OVERRIDE;
  
  //! Returns the index of the contour in the internal data
  //! structure of this algorithm which contains the edge E of the shape.
  //! This function returns 0 if the edge E does not belong to any contour.
  //! Warning
  //! This index can change if a contour is removed from the
  //! internal data structure of this algorithm using the function Remove.
  Standard_EXPORT Standard_Integer Contour (const TopoDS_Edge& E) const Standard_OVERRIDE;
  
  //! Returns the number of edges in the contour of index I in
  //! the internal data structure of this algorithm.
  //! Warning
  //! Returns 0 if I is outside the bounds of the table of contours.
  Standard_EXPORT Standard_Integer NbEdges (const Standard_Integer I) const Standard_OVERRIDE;
  
  //! Returns the edge of index J in the contour of index I in
  //! the internal data structure of this algorithm.
  //! Warning
  //! Returns a null shape if:
  //! -   I is outside the bounds of the table of contours, or
  //! -   J is outside the bounds of the table of edges of the index I contour.
  Standard_EXPORT const TopoDS_Edge& Edge (const Standard_Integer I, const Standard_Integer J) const Standard_OVERRIDE;
  
  //! Removes the contour in the internal data structure of
  //! this algorithm which contains the edge E of the shape.
  //! Warning
  //! Nothing is done if the edge E does not belong to the
  //! contour in the internal data structure of this algorithm.
  Standard_EXPORT void Remove (const TopoDS_Edge& E) Standard_OVERRIDE;
  
  //! Returns the length of the contour of index IC in the
  //! internal data structure of this algorithm.
  //! Warning
  //! Returns -1. if IC is outside the bounds of the table of contours.
  Standard_EXPORT Standard_Real Length (const Standard_Integer IC) const Standard_OVERRIDE;
  
  //! Returns the first vertex of the contour of index IC
  //! in the internal data structure of this algorithm.
  //! Warning
  //! Returns a null shape if IC is outside the bounds of the table of contours.
  Standard_EXPORT TopoDS_Vertex FirstVertex (const Standard_Integer IC) const Standard_OVERRIDE;
  
  //! Returns the  last vertex of the contour of index IC
  //! in the internal data structure of this algorithm.
  //! Warning
  //! Returns a null shape if IC is outside the bounds of the table of contours.
  Standard_EXPORT TopoDS_Vertex LastVertex (const Standard_Integer IC) const Standard_OVERRIDE;
  
  //! Returns the curvilinear abscissa of the vertex V on the
  //! contour of index IC in the internal data structure of this algorithm.
  //! Warning
  //! Returns -1. if:
  //! -   IC is outside the bounds of the table of contours, or
  //! -   V is not on the contour of index IC.
  Standard_EXPORT Standard_Real Abscissa (const Standard_Integer IC, const TopoDS_Vertex& V) const Standard_OVERRIDE;
  
  //! Returns the relative curvilinear abscissa (i.e. between 0
  //! and 1) of the vertex V on the contour of index IC in the
  //! internal data structure of this algorithm.
  //! Warning
  //! Returns -1. if:
  //! -   IC is outside the bounds of the table of contours, or
  //! -   V is not on the contour of index IC.
  Standard_EXPORT Standard_Real RelativeAbscissa (const Standard_Integer IC, const TopoDS_Vertex& V) const Standard_OVERRIDE;
  
  //! Returns true if the contour of index IC in the internal
  //! data structure of this algorithm is closed and tangential
  //! at the point of closure.
  //! Warning
  //! Returns false if IC is outside the bounds of the table of contours.
  Standard_EXPORT Standard_Boolean ClosedAndTangent (const Standard_Integer IC) const Standard_OVERRIDE;
  
  //! Returns true if the contour of index IC in the internal
  //! data structure of this algorithm is closed.
  //! Warning
  //! Returns false if IC is outside the bounds of the table of contours.
  Standard_EXPORT Standard_Boolean Closed (const Standard_Integer IC) const Standard_OVERRIDE;
  
  //! Builds the fillets on all the contours in the internal data
  //! structure of this algorithm and constructs the resulting shape.
  //! Use the function IsDone to verify that the filleted shape
  //! is built. Use the function Shape to retrieve the filleted shape.
  //! Warning
  //! The construction of fillets implements highly complex
  //! construction algorithms. Consequently, there may be
  //! instances where the algorithm fails, for example if the
  //! data defining the radius of the fillet is not compatible
  //! with the geometry of the initial shape. There is no initial
  //! analysis of errors and they only become evident at the
  //! construction stage.
  //! Additionally, in the current software release, the
  //! following cases are not handled:
  //! -   the end point of the contour is the point of
  //! intersection of 4 or more edges of the shape, or
  //! -   the intersection of the fillet with a face which limits
  //! the contour is not fully contained in this face.
  Standard_EXPORT virtual void Build(const Message_ProgressRange& theRange = Message_ProgressRange()) Standard_OVERRIDE;
  
  //! Reinitializes this algorithm, thus canceling the effects of the Build function.
  //! This function allows modifications to be made to the
  //! contours and fillet parameters in order to rebuild the shape.
  Standard_EXPORT void Reset() Standard_OVERRIDE;
  
  //! Returns the internal topology building algorithm.
  Standard_EXPORT Handle(TopOpeBRepBuild_HBuilder) Builder() const;
  
  //! Returns the  list   of shapes generated   from the
  //! shape <EorV>.
  Standard_EXPORT virtual const TopTools_ListOfShape& Generated (const TopoDS_Shape& EorV) Standard_OVERRIDE;
  
  //! Returns the list  of shapes modified from the shape
  //! <F>.
  Standard_EXPORT virtual const TopTools_ListOfShape& Modified (const TopoDS_Shape& F) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Boolean IsDeleted (const TopoDS_Shape& F) Standard_OVERRIDE;
  
  //! returns the number of surfaces
  //! after the shape creation.
  Standard_EXPORT Standard_Integer NbSurfaces() const;
  
  //! Return the faces created for surface <I>.
  Standard_EXPORT const TopTools_ListOfShape& NewFaces (const Standard_Integer I);
  
  Standard_EXPORT void Simulate (const Standard_Integer IC) Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Integer NbSurf (const Standard_Integer IC) const Standard_OVERRIDE;
  
  Standard_EXPORT Handle(ChFiDS_SecHArray1) Sect (const Standard_Integer IC, const Standard_Integer IS) const Standard_OVERRIDE;
  
  //! Returns the number of contours where the computation
  //! of the fillet failed
  Standard_EXPORT Standard_Integer NbFaultyContours() const;
  
  //! for each I in [1.. NbFaultyContours] returns the index IC of
  //! the contour where the computation of the fillet failed.
  //! the method NbEdges(IC) gives the number of edges in the contour IC
  //! the method Edge(IC,ie) gives the edge number ie of the contour IC
  Standard_EXPORT Standard_Integer FaultyContour (const Standard_Integer I) const;
  
  //! returns the number of surfaces which have been
  //! computed on the contour IC
  Standard_EXPORT Standard_Integer NbComputedSurfaces (const Standard_Integer IC) const;
  
  //! returns the surface number IS concerning the contour IC
  Standard_EXPORT Handle(Geom_Surface) ComputedSurface (const Standard_Integer IC, const Standard_Integer IS) const;
  
  //! returns the number of vertices where the computation failed
  Standard_EXPORT Standard_Integer NbFaultyVertices() const;
  
  //! returns the vertex where the computation failed
  Standard_EXPORT TopoDS_Vertex FaultyVertex (const Standard_Integer IV) const;
  
  //! returns true if a part of the result has been computed
  //! if the filling in a corner failed a shape with a hole is returned
  Standard_EXPORT Standard_Boolean HasResult() const;
  
  //! if (HasResult()) returns the partial result
  Standard_EXPORT TopoDS_Shape BadShape() const;
  
  //! returns the status concerning the contour IC in case of error
  //! ChFiDS_Ok : the computation is Ok
  //! ChFiDS_StartsolFailure : the computation can't start, perhaps the
  //! the radius is too big
  //! ChFiDS_TwistedSurface : the computation failed because of a twisted
  //! surface
  //! ChFiDS_WalkingFailure : there is a problem in the walking
  //! ChFiDS_Error:  other error different from above
  Standard_EXPORT ChFiDS_ErrorStatus StripeStatus (const Standard_Integer IC) const;




protected:





private:



  ChFi3d_FilBuilder myBuilder;
  TopTools_MapOfShape myMap;


};







#endif // _BRepFilletAPI_MakeFillet_HeaderFile
