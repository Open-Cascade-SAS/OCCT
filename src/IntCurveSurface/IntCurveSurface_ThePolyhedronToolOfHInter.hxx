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

#ifndef _IntCurveSurface_ThePolyhedronToolOfHInter_HeaderFile
#define _IntCurveSurface_ThePolyhedronToolOfHInter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Bnd_HArray1OfBox.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class Standard_OutOfRange;
class IntCurveSurface_ThePolyhedronOfHInter;
class Bnd_Box;
class gp_Pnt;



class IntCurveSurface_ThePolyhedronToolOfHInter 
{
public:

  DEFINE_STANDARD_ALLOC

  
    static const Bnd_Box& Bounding (const IntCurveSurface_ThePolyhedronOfHInter& thePolyh);
  
    static const Handle(Bnd_HArray1OfBox)& ComponentsBounding (const IntCurveSurface_ThePolyhedronOfHInter& thePolyh);
  
    static Standard_Real DeflectionOverEstimation (const IntCurveSurface_ThePolyhedronOfHInter& thePolyh);
  
    static Standard_Integer NbTriangles (const IntCurveSurface_ThePolyhedronOfHInter& thePolyh);
  
    static void Triangle (const IntCurveSurface_ThePolyhedronOfHInter& thePolyh, const Standard_Integer Index, Standard_Integer& P1, Standard_Integer& P2, Standard_Integer& P3);
  
    static const gp_Pnt& Point (const IntCurveSurface_ThePolyhedronOfHInter& thePolyh, const Standard_Integer Index);
  
    static Standard_Integer TriConnex (const IntCurveSurface_ThePolyhedronOfHInter& thePolyh, const Standard_Integer Triang, const Standard_Integer Pivot, const Standard_Integer Pedge, Standard_Integer& TriCon, Standard_Integer& OtherP);
  
    static Standard_Boolean IsOnBound (const IntCurveSurface_ThePolyhedronOfHInter& thePolyh, const Standard_Integer Index1, const Standard_Integer Index2);
  
    static Standard_Real GetBorderDeflection (const IntCurveSurface_ThePolyhedronOfHInter& thePolyh);
  
  Standard_EXPORT static void Dump (const IntCurveSurface_ThePolyhedronOfHInter& thePolyh);




protected:





private:





};

#define ThePolyhedron IntCurveSurface_ThePolyhedronOfHInter
#define ThePolyhedron_hxx <IntCurveSurface_ThePolyhedronOfHInter.hxx>
#define IntCurveSurface_PolyhedronTool IntCurveSurface_ThePolyhedronToolOfHInter
#define IntCurveSurface_PolyhedronTool_hxx <IntCurveSurface_ThePolyhedronToolOfHInter.hxx>

#include <IntCurveSurface_PolyhedronTool.lxx>

#undef ThePolyhedron
#undef ThePolyhedron_hxx
#undef IntCurveSurface_PolyhedronTool
#undef IntCurveSurface_PolyhedronTool_hxx




#endif // _IntCurveSurface_ThePolyhedronToolOfHInter_HeaderFile
