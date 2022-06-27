// Created on: 1996-02-13
// Created by: Jean Yves LEBEY
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _TopOpeBRepBuild_GTool_HeaderFile
#define _TopOpeBRepBuild_GTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopAbs_ShapeEnum.hxx>
#include <Standard_OStream.hxx>
class TopOpeBRepBuild_GTopo;



class TopOpeBRepBuild_GTool 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static TopOpeBRepBuild_GTopo GFusUnsh (const TopAbs_ShapeEnum s1, const TopAbs_ShapeEnum s2);
  
  Standard_EXPORT static TopOpeBRepBuild_GTopo GFusSame (const TopAbs_ShapeEnum s1, const TopAbs_ShapeEnum s2);
  
  Standard_EXPORT static TopOpeBRepBuild_GTopo GFusDiff (const TopAbs_ShapeEnum s1, const TopAbs_ShapeEnum s2);
  
  Standard_EXPORT static TopOpeBRepBuild_GTopo GCutUnsh (const TopAbs_ShapeEnum s1, const TopAbs_ShapeEnum s2);
  
  Standard_EXPORT static TopOpeBRepBuild_GTopo GCutSame (const TopAbs_ShapeEnum s1, const TopAbs_ShapeEnum s2);
  
  Standard_EXPORT static TopOpeBRepBuild_GTopo GCutDiff (const TopAbs_ShapeEnum s1, const TopAbs_ShapeEnum s2);
  
  Standard_EXPORT static TopOpeBRepBuild_GTopo GComUnsh (const TopAbs_ShapeEnum s1, const TopAbs_ShapeEnum s2);
  
  Standard_EXPORT static TopOpeBRepBuild_GTopo GComSame (const TopAbs_ShapeEnum s1, const TopAbs_ShapeEnum s2);
  
  Standard_EXPORT static TopOpeBRepBuild_GTopo GComDiff (const TopAbs_ShapeEnum s1, const TopAbs_ShapeEnum s2);
  
  Standard_EXPORT static void Dump (Standard_OStream& OS);




protected:





private:





};







#endif // _TopOpeBRepBuild_GTool_HeaderFile
