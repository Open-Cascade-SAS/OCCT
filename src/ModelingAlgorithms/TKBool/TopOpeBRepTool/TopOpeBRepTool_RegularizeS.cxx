// Created on: 1999-01-06
// Created by: Xuan PHAM PHU
// Copyright (c) 1999-1999 Matra Datavision
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

#include <TopOpeBRepTool_REGUS.hxx>
#include <TopOpeBRepTool.hxx>
#include <TopoDS_Solid.hxx>
#include <TopExp_Explorer.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopAbs_State.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_List.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_DataMap.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TCollection_AsciiString.hxx>

//=================================================================================================

bool TopOpeBRepTool::RegularizeShells(
  const TopoDS_Solid& theSolid,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    OldSheNewShe,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    FSplits)
{
  OldSheNewShe.Clear();
  FSplits.Clear();
  TopOpeBRepTool_REGUS REGUS;
  REGUS.SetOshNsh(OldSheNewShe);
  REGUS.SetFsplits(FSplits);

  //  bool hastoregu = false;
  TopExp_Explorer exsh(theSolid, TopAbs_SHELL);
  for (; exsh.More(); exsh.Next())
  {

    const TopoDS_Shape& sh = exsh.Current();
    REGUS.Init(sh);
    bool ok = REGUS.MapS();
    if (!ok)
      return false;
    ok = REGUS.SplitFaces();
    if (!ok)
      return false;
    REGUS.REGU();

  } // exsh(theSolid)

  REGUS.GetOshNsh(OldSheNewShe); //??????????????????????????????
  REGUS.GetFsplits(FSplits);     //??????????????????????????????
  return true;
}
