// Created on: 1993-04-07
// Created by: Laurent BUCHARD
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

#ifndef _IntCurveSurface_ThePolygonToolOfHInter_HeaderFile
#define _IntCurveSurface_ThePolygonToolOfHInter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Standard_Integer.hxx>
class Standard_OutOfRange;
class gp_Pnt;
class IntCurveSurface_ThePolygonOfHInter;
class Bnd_Box;



class IntCurveSurface_ThePolygonToolOfHInter 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Give the bounding box of the polygon.
    static const Bnd_Box& Bounding (const IntCurveSurface_ThePolygonOfHInter& thePolygon);
  
    static Standard_Real DeflectionOverEstimation (const IntCurveSurface_ThePolygonOfHInter& thePolygon);
  
    static Standard_Boolean Closed (const IntCurveSurface_ThePolygonOfHInter& thePolygon);
  
    static Standard_Integer NbSegments (const IntCurveSurface_ThePolygonOfHInter& thePolygon);
  
  //! Give the point of range Index in the Polygon.
    static const gp_Pnt& BeginOfSeg (const IntCurveSurface_ThePolygonOfHInter& thePolygon, const Standard_Integer Index);
  
  //! Give the point of range Index in the Polygon.
    static const gp_Pnt& EndOfSeg (const IntCurveSurface_ThePolygonOfHInter& thePolygon, const Standard_Integer Index);
  
  Standard_EXPORT static void Dump (const IntCurveSurface_ThePolygonOfHInter& thePolygon);




protected:





private:





};

#define ThePoint gp_Pnt
#define ThePoint_hxx <gp_Pnt.hxx>
#define ThePolygon IntCurveSurface_ThePolygonOfHInter
#define ThePolygon_hxx <IntCurveSurface_ThePolygonOfHInter.hxx>
#define TheBoundingBox Bnd_Box
#define TheBoundingBox_hxx <Bnd_Box.hxx>
#define IntCurveSurface_PolygonTool IntCurveSurface_ThePolygonToolOfHInter
#define IntCurveSurface_PolygonTool_hxx <IntCurveSurface_ThePolygonToolOfHInter.hxx>

#include <IntCurveSurface_PolygonTool.lxx>

#undef ThePoint
#undef ThePoint_hxx
#undef ThePolygon
#undef ThePolygon_hxx
#undef TheBoundingBox
#undef TheBoundingBox_hxx
#undef IntCurveSurface_PolygonTool
#undef IntCurveSurface_PolygonTool_hxx




#endif // _IntCurveSurface_ThePolygonToolOfHInter_HeaderFile
