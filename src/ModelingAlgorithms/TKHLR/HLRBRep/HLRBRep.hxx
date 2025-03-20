// Created on: 1992-10-14
// Created by: Christophe MARION
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _HLRBRep_HeaderFile
#define _HLRBRep_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
class TopoDS_Edge;
class HLRBRep_Curve;

//! Hidden Lines Removal
//! algorithms on the BRep DataStructure.
//!
//! The class PolyAlgo  is used to remove Hidden lines
//! on Shapes with Triangulations.
class HLRBRep
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT static TopoDS_Edge MakeEdge(const HLRBRep_Curve& ec,
                                              const Standard_Real  U1,
                                              const Standard_Real  U2);

  Standard_EXPORT static TopoDS_Edge MakeEdge3d(const HLRBRep_Curve& ec,
                                                const Standard_Real  U1,
                                                const Standard_Real  U2);

  Standard_EXPORT static void PolyHLRAngleAndDeflection(const Standard_Real InAngl,
                                                        Standard_Real&      OutAngl,
                                                        Standard_Real&      OutDefl);
};

#endif // _HLRBRep_HeaderFile
