// Created on: 1998-01-14
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

#ifndef _BRepFill_LocationLaw_HeaderFile
#define _BRepFill_LocationLaw_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Wire.hxx>
#include <GeomFill_HArray1OfLocationLaw.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TopTools_HArray1OfShape.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>
#include <GeomFill_PipeError.hxx>
#include <TColStd_Array1OfInteger.hxx>
class GeomFill_LocationLaw;
class TopoDS_Edge;
class TopoDS_Vertex;
class TopoDS_Shape;


class BRepFill_LocationLaw;
DEFINE_STANDARD_HANDLE(BRepFill_LocationLaw, Standard_Transient)

//! Location Law on a  Wire.
class BRepFill_LocationLaw : public Standard_Transient
{

public:

  
  //! Return a error status, if the  status is not PipeOk then
  //! it exist a parameter tlike the law is not valuable for t.
  Standard_EXPORT GeomFill_PipeError GetStatus() const;
  
  //! Apply a linear   transformation  on each law, to  have
  //! continuity of the global law between the edges.
  Standard_EXPORT virtual void TransformInG0Law();
  
  //! Apply a linear transformation on each law, to reduce
  //! the   dicontinuities  of law at one  rotation.
  Standard_EXPORT virtual void TransformInCompatibleLaw (const Standard_Real AngularTolerance);
  
  Standard_EXPORT void DeleteTransform();
  
  Standard_EXPORT Standard_Integer NbHoles (const Standard_Real Tol = 1.0e-7);
  
  Standard_EXPORT void Holes (TColStd_Array1OfInteger& Interval) const;
  
  //! Return the number of elementary Law
  Standard_EXPORT Standard_Integer NbLaw() const;
  
  //! Return the elementary Law of rank <Index>
  //! <Index> have to be in [1, NbLaw()]
  Standard_EXPORT const Handle(GeomFill_LocationLaw)& Law (const Standard_Integer Index) const;
  
  //! return the path
  Standard_EXPORT const TopoDS_Wire& Wire() const;
  
  //! Return the Edge of rank <Index> in the path
  //! <Index> have to be in [1, NbLaw()]
  Standard_EXPORT const TopoDS_Edge& Edge (const Standard_Integer Index) const;
  
  //! Return the vertex of rank <Index> in the path
  //! <Index> have to be in [0, NbLaw()]
  Standard_EXPORT TopoDS_Vertex Vertex (const Standard_Integer Index) const;
  
  //! Compute <OutputVertex> like a transformation of
  //! <InputVertex>  the  transformation   is given by
  //! evaluation of the location law   in the vertex of
  //! rank   <Index>.
  //! <Location> is used to manage discontinuities :
  //! - -1 : The law before the vertex is used.
  //! -  1 : The law after the vertex is used.
  //! -  0 : Average of the both laws is used.
  Standard_EXPORT void PerformVertex (const Standard_Integer Index, const TopoDS_Vertex& InputVertex, const Standard_Real TolMin, TopoDS_Vertex& OutputVertex, const Standard_Integer Location = 0) const;
  
  //! Return the Curvilinear Bounds of the <Index> Law
  Standard_EXPORT void CurvilinearBounds (const Standard_Integer Index, Standard_Real& First, Standard_Real& Last) const;
  
  Standard_EXPORT Standard_Boolean IsClosed() const;
  
  //! Compute the Law's continuity between 2 edges of the path
  //! The result can be :
  //! -1 : Case Not connex
  //! 0  : It is connex (G0)
  //! 1  : It is tangent (G1)
  Standard_EXPORT Standard_Integer IsG1 (const Standard_Integer Index, const Standard_Real SpatialTolerance = 1.0e-7, const Standard_Real AngularTolerance = 1.0e-4) const;

  //! Apply the Law to a shape, for a given Curvilinear abscissa
  Standard_EXPORT void D0 (const Standard_Real Abscissa, TopoDS_Shape& Section);

  //! Find the index Law and the parameter, for a given Curvilinear abscissa
  Standard_EXPORT void Parameter (const Standard_Real Abscissa, Standard_Integer& Index, Standard_Real& Param);

  //! Return the curvilinear abscissa  corresponding to a point
  //! of  the path, defined by  <Index>  of  Edge and a
  //! parameter on the edge.
  Standard_EXPORT Standard_Real Abscissa (const Standard_Integer Index, const Standard_Real Param);




  DEFINE_STANDARD_RTTIEXT(BRepFill_LocationLaw,Standard_Transient)

protected:

  
  //! Initialize all the fields, this methode have to
  //! be called by the constructors of Inherited class.
  Standard_EXPORT void Init (const TopoDS_Wire& Path);
  
  //! To preseve if possible  the Tangent in transformations
  //! It is the default mode.
  Standard_EXPORT void TangentIsMain();
  
  //! To preseve if possible the Normal in transformations
  Standard_EXPORT void NormalIsMain();
  
  //! To preseve if possible the BiNormal in transformations
  Standard_EXPORT void BiNormalIsMain();

  TopoDS_Wire myPath;
  Standard_Real myTol;
  Handle(GeomFill_HArray1OfLocationLaw) myLaws;
  Handle(TColStd_HArray1OfReal) myLength;
  Handle(TopTools_HArray1OfShape) myEdges;
  Handle(TColStd_HArray1OfInteger) myDisc;


private:


  Standard_Integer myType;


};







#endif // _BRepFill_LocationLaw_HeaderFile
