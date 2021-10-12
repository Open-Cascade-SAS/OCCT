// Created on: 1993-11-29
// Created by: Isabelle GRIGNON
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _ChFiDS_CommonPoint_HeaderFile
#define _ChFiDS_CommonPoint_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <TopAbs_Orientation.hxx>


//! point    start/end of  fillet common  to  2 adjacent  filets
//! and  to an edge on  one of 2 faces participating
//! in  the construction of  the  fillet
class ChFiDS_CommonPoint 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor.
  Standard_EXPORT ChFiDS_CommonPoint();
  
  //! default value for all fields
  Standard_EXPORT void Reset();
  
  //! Sets the values of a point which is a vertex on
  //! the initial facet of restriction of one
  //! of the surface.
  void SetVertex (const TopoDS_Vertex& theVertex)
  {
    isvtx = Standard_True;
    vtx = theVertex;
  }

  //! Sets the values of a point which is on the arc
  //! A, at parameter Param.
  Standard_EXPORT void SetArc (const Standard_Real Tol, const TopoDS_Edge& A, const Standard_Real Param, const TopAbs_Orientation TArc);
  
  //! Sets the value of the parameter on the spine
  Standard_EXPORT void SetParameter (const Standard_Real Param);
  
  //! Set the 3d point for a commonpoint that is not
  //! a vertex or on an arc.
  void SetPoint (const gp_Pnt& thePoint) { point = thePoint; }
  
  //! Set the output 3d  vector
  void SetVector (const gp_Vec& theVector)
  {
    hasvector = Standard_True;
    vector = theVector;
  }
  
  //! This method set the fuzziness on the point.
  void SetTolerance (const Standard_Real Tol)
  {
    if (Tol > tol)
    {
      tol = Tol;
    }
  }
  
  //! This method returns the fuzziness on the point.
  Standard_Real Tolerance() const { return tol; }
  
  //! Returns TRUE if the point is a vertex on the initial
  //! restriction facet of the surface.
  Standard_Boolean IsVertex() const { return isvtx; }
  
  //! Returns the information about the point when it is
  //! on the domain of the first patch, i-e when the function
  //! IsVertex returns True.
  //! Otherwise, an exception is raised.
  const TopoDS_Vertex& Vertex() const
  {
    if (!isvtx) { throw Standard_DomainError(); }
    return vtx;
  }

  //! Returns TRUE if the point is a on an edge of the initial
  //! restriction facet of the surface.
  Standard_Boolean IsOnArc() const { return isonarc; }
  
  //! Returns the arc of restriction containing the
  //! vertex.
  Standard_EXPORT const TopoDS_Edge& Arc() const;
  
  //! Returns the transition of the point on the arc
  //! returned by Arc().
  Standard_EXPORT TopAbs_Orientation TransitionOnArc() const;
  
  //! Returns the parameter of the point on the
  //! arc returned by the method Arc().
  Standard_EXPORT Standard_Real ParameterOnArc() const;
  
  //! Returns the parameter on the spine
  Standard_EXPORT Standard_Real Parameter() const;
  
  //! Returns the 3d point
  const gp_Pnt& Point() const { return point; }
  
  //! Returns TRUE if the output vector is  stored.
  Standard_Boolean HasVector() const { return hasvector; }
  
  //! Returns the output  3d vector
  const gp_Vec& Vector() const
  {
    if (!hasvector) { throw Standard_DomainError ("ChFiDS_CommonPoint::Vector"); }
    return vector;
  }

private:

  TopoDS_Edge arc;
  TopoDS_Vertex vtx;
  gp_Pnt point;
  gp_Vec vector;
  Standard_Real tol;
  Standard_Real prmarc;
  Standard_Real prmtg;
  TopAbs_Orientation traarc;
  Standard_Boolean isonarc;
  Standard_Boolean isvtx;
  Standard_Boolean hasvector;

};

#endif // _ChFiDS_CommonPoint_HeaderFile
