// Created on: 1995-01-27
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

#ifndef _GeomInt_TheMultiLineToolOfWLApprox_HeaderFile
#define _GeomInt_TheMultiLineToolOfWLApprox_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <Standard_Boolean.hxx>
#include <TColgp_Array1OfVec.hxx>
#include <TColgp_Array1OfVec2d.hxx>
#include <GeomInt_TheMultiLineOfWLApprox.hxx>
#include <Approx_Status.hxx>
class GeomInt_TheMultiLineOfWLApprox;
class ApproxInt_SvSurfaces;



class GeomInt_TheMultiLineToolOfWLApprox 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns the number of multipoints of the TheMultiLine.
    static Standard_Integer FirstPoint (const GeomInt_TheMultiLineOfWLApprox& ML);
  
  //! Returns the number of multipoints of the TheMultiLine.
    static Standard_Integer LastPoint (const GeomInt_TheMultiLineOfWLApprox& ML);
  
  //! Returns the number of 2d points of a TheMultiLine.
    static Standard_Integer NbP2d (const GeomInt_TheMultiLineOfWLApprox& ML);
  
  //! Returns the number of 3d points of a TheMultiLine.
    static Standard_Integer NbP3d (const GeomInt_TheMultiLineOfWLApprox& ML);
  
  //! returns the 3d points of the multipoint <MPointIndex>
  //! when only 3d points exist.
    static void Value (const GeomInt_TheMultiLineOfWLApprox& ML, const Standard_Integer MPointIndex, TColgp_Array1OfPnt& tabPt);
  
  //! returns the 2d points of the multipoint <MPointIndex>
  //! when only 2d points exist.
    static void Value (const GeomInt_TheMultiLineOfWLApprox& ML, const Standard_Integer MPointIndex, TColgp_Array1OfPnt2d& tabPt2d);
  
  //! returns the 3d and 2d points of the multipoint
  //! <MPointIndex>.
    static void Value (const GeomInt_TheMultiLineOfWLApprox& ML, const Standard_Integer MPointIndex, TColgp_Array1OfPnt& tabPt, TColgp_Array1OfPnt2d& tabPt2d);
  
  //! returns the 3d points of the multipoint <MPointIndex>
  //! when only 3d points exist.
    static Standard_Boolean Tangency (const GeomInt_TheMultiLineOfWLApprox& ML, const Standard_Integer MPointIndex, TColgp_Array1OfVec& tabV);
  
  //! returns the 2d tangency points of the multipoint
  //! <MPointIndex> only when 2d points exist.
    static Standard_Boolean Tangency (const GeomInt_TheMultiLineOfWLApprox& ML, const Standard_Integer MPointIndex, TColgp_Array1OfVec2d& tabV2d);
  
  //! returns the 3d and 2d points of the multipoint
  //! <MPointIndex>.
    static Standard_Boolean Tangency (const GeomInt_TheMultiLineOfWLApprox& ML, const Standard_Integer MPointIndex, TColgp_Array1OfVec& tabV, TColgp_Array1OfVec2d& tabV2d);
  
  //! returns the 3d curvature of the multipoint <MPointIndex>
  //! when only 3d points exist.
    static Standard_Boolean Curvature (const GeomInt_TheMultiLineOfWLApprox& ML, const Standard_Integer MPointIndex, TColgp_Array1OfVec& tabV);
  
  //! returns the 2d curvature points of the multipoint
  //! <MPointIndex> only when 2d points exist.
    static Standard_Boolean Curvature (const GeomInt_TheMultiLineOfWLApprox& ML, const Standard_Integer MPointIndex, TColgp_Array1OfVec2d& tabV2d);
  
  //! returns the 3d and 2d curvature of the multipoint
  //! <MPointIndex>.
    static Standard_Boolean Curvature (const GeomInt_TheMultiLineOfWLApprox& ML, const Standard_Integer MPointIndex, TColgp_Array1OfVec& tabV, TColgp_Array1OfVec2d& tabV2d);
  
  //! Is called if WhatStatus returned "PointsAdded".
    static GeomInt_TheMultiLineOfWLApprox MakeMLBetween (const GeomInt_TheMultiLineOfWLApprox& ML, const Standard_Integer I1, const Standard_Integer I2, const Standard_Integer NbPMin);
  
  //! Is called when the Bezier curve contains a loop
    static Standard_Boolean MakeMLOneMorePoint (const GeomInt_TheMultiLineOfWLApprox& ML,
                                                const Standard_Integer I1,
                                                const Standard_Integer I2,
                                                const Standard_Integer indbad,
                                                GeomInt_TheMultiLineOfWLApprox& OtherLine);
  
    static Approx_Status WhatStatus (const GeomInt_TheMultiLineOfWLApprox& ML, const Standard_Integer I1, const Standard_Integer I2);
  
  //! Dump of the current multi-line.
  static void Dump (const GeomInt_TheMultiLineOfWLApprox& ML);




protected:





private:





};

#define TheMultiLine GeomInt_TheMultiLineOfWLApprox
#define TheMultiLine_hxx <GeomInt_TheMultiLineOfWLApprox.hxx>
#define TheMultiMPoint ApproxInt_SvSurfaces
#define TheMultiMPoint_hxx <ApproxInt_SvSurfaces.hxx>
#define ApproxInt_MultiLineTool GeomInt_TheMultiLineToolOfWLApprox
#define ApproxInt_MultiLineTool_hxx <GeomInt_TheMultiLineToolOfWLApprox.hxx>

#include <ApproxInt_MultiLineTool.lxx>

#undef TheMultiLine
#undef TheMultiLine_hxx
#undef TheMultiMPoint
#undef TheMultiMPoint_hxx
#undef ApproxInt_MultiLineTool
#undef ApproxInt_MultiLineTool_hxx




#endif // _GeomInt_TheMultiLineToolOfWLApprox_HeaderFile
