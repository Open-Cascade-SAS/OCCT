// Created on: 1993-01-20
// Created by: Laurent PAINNOT
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

#ifndef _AppDef_MyLineTool_HeaderFile
#define _AppDef_MyLineTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <Standard_Boolean.hxx>
#include <TColgp_Array1OfVec.hxx>
#include <TColgp_Array1OfVec2d.hxx>
#include <Approx_Status.hxx>
class AppDef_MultiLine;


//! example of MultiLine tool corresponding to the tools of the packages
//! AppParCurves and Approx. For Approx, the tool will not addd points
//! if the algorithms want some.
class AppDef_MyLineTool 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns the first index of multipoints of the MultiLine.
  Standard_EXPORT static Standard_Integer FirstPoint (const AppDef_MultiLine& ML);
  
  //! Returns the last index of multipoints of the MultiLine.
  Standard_EXPORT static Standard_Integer LastPoint (const AppDef_MultiLine& ML);
  
  //! Returns the number of 2d points of a MultiLine.
  Standard_EXPORT static Standard_Integer NbP2d (const AppDef_MultiLine& ML);
  
  //! Returns the number of 3d points of a MultiLine.
  Standard_EXPORT static Standard_Integer NbP3d (const AppDef_MultiLine& ML);
  
  //! returns the 3d points of the multipoint <MPointIndex>
  //! when only 3d points exist.
  Standard_EXPORT static void Value (const AppDef_MultiLine& ML, const Standard_Integer MPointIndex, TColgp_Array1OfPnt& tabPt);
  
  //! returns the 2d points of the multipoint <MPointIndex>
  //! when only 2d points exist.
  Standard_EXPORT static void Value (const AppDef_MultiLine& ML, const Standard_Integer MPointIndex, TColgp_Array1OfPnt2d& tabPt2d);
  
  //! returns the 3d and 2d points of the multipoint
  //! <MPointIndex>.
  Standard_EXPORT static void Value (const AppDef_MultiLine& ML, const Standard_Integer MPointIndex, TColgp_Array1OfPnt& tabPt, TColgp_Array1OfPnt2d& tabPt2d);
  
  //! returns the 3d points of the multipoint <MPointIndex>
  //! when only 3d points exist.
  Standard_EXPORT static Standard_Boolean Tangency (const AppDef_MultiLine& ML, const Standard_Integer MPointIndex, TColgp_Array1OfVec& tabV);
  
  //! returns the 2d tangency points of the multipoint
  //! <MPointIndex> only when 2d points exist.
  Standard_EXPORT static Standard_Boolean Tangency (const AppDef_MultiLine& ML, const Standard_Integer MPointIndex, TColgp_Array1OfVec2d& tabV2d);
  
  //! returns the 3d and 2d points of the multipoint
  //! <MPointIndex>.
  Standard_EXPORT static Standard_Boolean Tangency (const AppDef_MultiLine& ML, const Standard_Integer MPointIndex, TColgp_Array1OfVec& tabV, TColgp_Array1OfVec2d& tabV2d);
  
  //! returns the 3d curvatures of the multipoint <MPointIndex>
  //! when only 3d points exist.
  Standard_EXPORT static Standard_Boolean Curvature (const AppDef_MultiLine& ML, const Standard_Integer MPointIndex, TColgp_Array1OfVec& tabV);
  
  //! returns the 2d curvatures of the multipoint
  //! <MPointIndex> only when 2d points exist.
  Standard_EXPORT static Standard_Boolean Curvature (const AppDef_MultiLine& ML, const Standard_Integer MPointIndex, TColgp_Array1OfVec2d& tabV2d);
  
  //! returns the 3d and 2d curvatures of the multipoint
  //! <MPointIndex>.
  Standard_EXPORT static Standard_Boolean Curvature (const AppDef_MultiLine& ML, const Standard_Integer MPointIndex, TColgp_Array1OfVec& tabV, TColgp_Array1OfVec2d& tabV2d);
  
  //! returns NoPointsAdded
  Standard_EXPORT static Approx_Status WhatStatus (const AppDef_MultiLine& ML, const Standard_Integer I1, const Standard_Integer I2);
  
  //! Is never called in the algorithms.
  //! Nothing is done.
  Standard_EXPORT static AppDef_MultiLine MakeMLBetween (const AppDef_MultiLine& ML,
                                                          const Standard_Integer I1,
                                                          const Standard_Integer I2,
                                                          const Standard_Integer NbPMin);

  //! Is never called in the algorithms.
  //! Nothing is done.
  Standard_EXPORT static  Standard_Boolean  MakeMLOneMorePoint (const AppDef_MultiLine& ML,
                                                                const Standard_Integer I1,
                                                                const Standard_Integer I2,
                                                                const Standard_Integer indbad,
                                                                AppDef_MultiLine& OtherLine);
};

#endif // _AppDef_MyLineTool_HeaderFile
