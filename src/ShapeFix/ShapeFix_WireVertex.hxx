// Created on: 1998-06-03
// Created by: data exchange team
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

#ifndef _ShapeFix_WireVertex_HeaderFile
#define _ShapeFix_WireVertex_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <ShapeAnalysis_WireVertex.hxx>
#include <Standard_Integer.hxx>
class TopoDS_Wire;
class ShapeExtend_WireData;


//! Fixing disconnected edges in the wire
//! Fixes vertices in the wire on the basis of pre-analysis
//! made by ShapeAnalysis_WireVertex (given as argument).
//! The Wire has formerly been loaded in a ShapeExtend_WireData.
class ShapeFix_WireVertex 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT ShapeFix_WireVertex();
  
  //! Loads the wire, ininializes internal analyzer
  //! (ShapeAnalysis_WireVertex) with the given precision,
  //! and performs analysis
  Standard_EXPORT void Init (const TopoDS_Wire& wire, const Standard_Real preci);
  
  //! Loads the wire, ininializes internal analyzer
  //! (ShapeAnalysis_WireVertex) with the given precision,
  //! and performs analysis
  Standard_EXPORT void Init (const Handle(ShapeExtend_WireData)& sbwd, const Standard_Real preci);
  
  //! Loads all the data on wire, already analysed by
  //! ShapeAnalysis_WireVertex
  Standard_EXPORT void Init (const ShapeAnalysis_WireVertex& sawv);
  
  //! returns internal analyzer
  Standard_EXPORT const ShapeAnalysis_WireVertex& Analyzer() const;
  
  //! returns data on wire (fixed)
  Standard_EXPORT const Handle(ShapeExtend_WireData)& WireData() const;
  
  //! returns resulting wire (fixed)
  Standard_EXPORT TopoDS_Wire Wire() const;
  
  //! Fixes "Same" or "Close" status (same vertex may be set,
  //! without changing parameters)
  //! Returns the count of fixed vertices, 0 if none
  Standard_EXPORT Standard_Integer FixSame();
  
  //! Fixes all statuses except "Disjoined", i.e. the cases in which a
  //! common value has been set, with or without changing parameters
  //! Returns the count of fixed vertices, 0 if none
  Standard_EXPORT Standard_Integer Fix();




protected:





private:



  ShapeAnalysis_WireVertex myAnalyzer;


};







#endif // _ShapeFix_WireVertex_HeaderFile
