// Created on: 1998-11-24
// Created by: Xuan PHAM PHU
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

#include <TopOpeBRepTool_2d.hxx>
#include <BRep_Tool.hxx>

#define M_FORWARD(sta)  (sta == TopAbs_FORWARD)
#define M_REVERSED(sta) (sta == TopAbs_REVERSED)
#define M_INTERNAL(sta) (sta == TopAbs_INTERNAL)
#define M_EXTERNAL(sta) (sta == TopAbs_EXTERNAL)

#ifdef DRAW
#include <TopOpeBRepTool_DRAW.hxx>
#endif

#ifdef OCCT_DEBUG
extern TopTools_IndexedMapOfShape STATIC_PURGE_mapv;
extern TopTools_IndexedMapOfOrientedShape STATIC_PURGE_mapeds;
extern Standard_Boolean TopOpeBRepTool_GettracePURGE();
void FUN_REINIT()
{
  STATIC_PURGE_mapv.Clear(); STATIC_PURGE_mapeds.Clear();
}

Standard_EXPORT void FUN_tool_tori(const TopAbs_Orientation Or)
{
  switch (Or) {
  case TopAbs_FORWARD:
    std::cout<<"FOR";break;
  case TopAbs_REVERSED:
    std::cout<<"REV";break;
  case TopAbs_INTERNAL:
    std::cout<<"INT";break;
  case TopAbs_EXTERNAL:
    std::cout<<"EXT";break;
  }    
}
#endif

Standard_EXPORT void FUN_tool_trace(const Standard_Integer Index)
{
  if (Index == 1) std::cout <<"FORWARD ";
  if (Index == 2) std::cout <<"REVERSED ";
}
Standard_EXPORT void FUN_tool_trace(const gp_Pnt2d p2d)
{
  std::cout<<" = ("<<p2d.X()<<" "<<p2d.Y()<<")"<<std::endl;
}
