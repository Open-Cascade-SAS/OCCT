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

#ifndef _HLRBRep_ThePolygonToolOfInterCSurf_HeaderFile
#define _HLRBRep_ThePolygonToolOfInterCSurf_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
class Standard_OutOfRange;
class gp_Pnt;
class HLRBRep_ThePolygonOfInterCSurf;
class Bnd_Box;



class HLRBRep_ThePolygonToolOfInterCSurf 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Give the bounding box of the polygon.
    static const Bnd_Box& Bounding (const HLRBRep_ThePolygonOfInterCSurf& thePolygon);
  
    static Standard_Real DeflectionOverEstimation (const HLRBRep_ThePolygonOfInterCSurf& thePolygon);
  
    static Standard_Boolean Closed (const HLRBRep_ThePolygonOfInterCSurf& thePolygon);
  
    static Standard_Integer NbSegments (const HLRBRep_ThePolygonOfInterCSurf& thePolygon);
  
  //! Give the point of range Index in the Polygon.
    static const gp_Pnt& BeginOfSeg (const HLRBRep_ThePolygonOfInterCSurf& thePolygon, const Standard_Integer Index);
  
  //! Give the point of range Index in the Polygon.
    static const gp_Pnt& EndOfSeg (const HLRBRep_ThePolygonOfInterCSurf& thePolygon, const Standard_Integer Index);
  
  Standard_EXPORT static void Dump (const HLRBRep_ThePolygonOfInterCSurf& thePolygon);




protected:





private:





};

#define ThePoint gp_Pnt
#define ThePoint_hxx <gp_Pnt.hxx>
#define ThePolygon HLRBRep_ThePolygonOfInterCSurf
#define ThePolygon_hxx <HLRBRep_ThePolygonOfInterCSurf.hxx>
#define TheBoundingBox Bnd_Box
#define TheBoundingBox_hxx <Bnd_Box.hxx>
#define IntCurveSurface_PolygonTool HLRBRep_ThePolygonToolOfInterCSurf
#define IntCurveSurface_PolygonTool_hxx <HLRBRep_ThePolygonToolOfInterCSurf.hxx>

#include <IntCurveSurface_PolygonTool.lxx>

#undef ThePoint
#undef ThePoint_hxx
#undef ThePolygon
#undef ThePolygon_hxx
#undef TheBoundingBox
#undef TheBoundingBox_hxx
#undef IntCurveSurface_PolygonTool
#undef IntCurveSurface_PolygonTool_hxx




#endif // _HLRBRep_ThePolygonToolOfInterCSurf_HeaderFile
