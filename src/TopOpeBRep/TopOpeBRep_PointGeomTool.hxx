// Created on: 1994-10-25
// Created by: Jean Yves LEBEY
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _TopOpeBRep_PointGeomTool_HeaderFile
#define _TopOpeBRep_PointGeomTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

class TopOpeBRepDS_Point;
class TopOpeBRep_VPointInter;
class TopOpeBRep_Point2d;
class TopOpeBRep_FaceEdgeIntersector;
class TopoDS_Shape;


//! Provide services needed by the Fillers
class TopOpeBRep_PointGeomTool 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static TopOpeBRepDS_Point MakePoint (const TopOpeBRep_VPointInter& IP);
  
  Standard_EXPORT static TopOpeBRepDS_Point MakePoint (const TopOpeBRep_Point2d& P2D);
  
  Standard_EXPORT static TopOpeBRepDS_Point MakePoint (const TopOpeBRep_FaceEdgeIntersector& FEI);
  
  Standard_EXPORT static TopOpeBRepDS_Point MakePoint (const TopoDS_Shape& S);
  
  Standard_EXPORT static Standard_Boolean IsEqual (const TopOpeBRepDS_Point& DSP1, const TopOpeBRepDS_Point& DSP2);




protected:





private:





};







#endif // _TopOpeBRep_PointGeomTool_HeaderFile
