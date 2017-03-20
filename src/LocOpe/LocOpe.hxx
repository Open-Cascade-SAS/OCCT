// Created on: 1995-04-25
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

#ifndef _LocOpe_HeaderFile
#define _LocOpe_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <TColgp_SequenceOfPnt.hxx>
class TopoDS_Wire;
class TopoDS_Face;
class TopoDS_Edge;
class TopoDS_Shape;
class LocOpe_SplitShape;
class LocOpe_WiresOnShape;
class LocOpe_Spliter;
class LocOpe_Generator;
class LocOpe_GeneratedShape;
class LocOpe_GluedShape;
class LocOpe_Prism;
class LocOpe_Revol;
class LocOpe_Pipe;
class LocOpe_DPrism;
class LocOpe_LinearForm;
class LocOpe_RevolutionForm;
class LocOpe_Gluer;
class LocOpe_FindEdges;
class LocOpe_FindEdgesInFace;
class LocOpe_PntFace;
class LocOpe_CurveShapeIntersector;
class LocOpe_CSIntersector;
class LocOpe_BuildShape;
class LocOpe_SplitDrafts;
class LocOpe_BuildWires;


//! Provides  tools to implement local     topological
//! operations on a shape.
class LocOpe 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns Standard_True  when the wire <W> is closed
  //! on the face <OnF>.
  Standard_EXPORT static Standard_Boolean Closed (const TopoDS_Wire& W, const TopoDS_Face& OnF);
  
  //! Returns Standard_True  when the edge <E> is closed
  //! on the face <OnF>.
  Standard_EXPORT static Standard_Boolean Closed (const TopoDS_Edge& E, const TopoDS_Face& OnF);
  
  //! Returns Standard_True  when the faces are tangent
  Standard_EXPORT static Standard_Boolean TgtFaces (const TopoDS_Edge& E, const TopoDS_Face& F1, const TopoDS_Face& F2);
  
  Standard_EXPORT static void SampleEdges (const TopoDS_Shape& S, TColgp_SequenceOfPnt& Pt);




protected:





private:




friend class LocOpe_SplitShape;
friend class LocOpe_WiresOnShape;
friend class LocOpe_Spliter;
friend class LocOpe_Generator;
friend class LocOpe_GeneratedShape;
friend class LocOpe_GluedShape;
friend class LocOpe_Prism;
friend class LocOpe_Revol;
friend class LocOpe_Pipe;
friend class LocOpe_DPrism;
friend class LocOpe_LinearForm;
friend class LocOpe_RevolutionForm;
friend class LocOpe_Gluer;
friend class LocOpe_FindEdges;
friend class LocOpe_FindEdgesInFace;
friend class LocOpe_PntFace;
friend class LocOpe_CurveShapeIntersector;
friend class LocOpe_CSIntersector;
friend class LocOpe_BuildShape;
friend class LocOpe_SplitDrafts;
friend class LocOpe_BuildWires;

};







#endif // _LocOpe_HeaderFile
