// Created on: 2014-08-13
// Created by: Oleg AGASHIN
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#ifndef _BRepMesh_EdgeParameterProvider_HeaderFile
#define _BRepMesh_EdgeParameterProvider_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Extrema_LocateExtPC.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <BRepAdaptor_Curve.hxx>

class gp_Pnt;
class TopoDS_Edge;
class TopoDS_Face;
#include <TColStd_HArray1OfReal.hxx>

//! Auxiliary class provides correct parameters 
//! on curve regarding SameParameter flag.
class BRepMesh_EdgeParameterProvider
{
public:

  DEFINE_STANDARD_ALLOC

  //! Constructor.
  //! @param theEdge edge which parameters should be processed.
  //! @param theFace face the parametric values are defined for.
  //! @param theParameters parameters corresponded to discretization points.
  BRepMesh_EdgeParameterProvider(
    const TopoDS_Edge&                   theEdge,
    const TopoDS_Face&                   theFace,
    const Handle(TColStd_HArray1OfReal)& theParameters);

  //! Returns parameter according to SameParameter flag of the edge.
  //! If SameParameter is TRUE returns value from parameters w/o changes,
  //! elsewhere scales initial parameter and tries to determine resulting
  //! value using projection of the corresponded 3D point on PCurve.
  Standard_Real Parameter(const Standard_Integer theIndex,
                          const gp_Pnt&          thePoint3d);

private:

  Handle(TColStd_HArray1OfReal) myParameters;

  Standard_Boolean              myIsSameParam;
  Standard_Real                 myFirstParam;

  Standard_Real                 myOldFirstParam;
  Standard_Real                 myScale;

  Standard_Real                 myCurParam;
  Standard_Real                 myFoundParam;

  BRepAdaptor_Curve             myCurveAdaptor;
  Extrema_LocateExtPC           myProjector;
};

#endif
