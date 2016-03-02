// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <StdPersistent.hxx>
#include <StdObjMgt_MapOfInstantiators.hxx>

#include <StdPersistent_TopLoc.hxx>
#include <StdPersistent_TopoDS.hxx>
#include <StdPersistent_Naming.hxx>
#include <StdPersistent_HArray1.hxx>
#include <StdLPersistent_HArray1.hxx>
#include <StdPersistent_DataXtd.hxx>
#include <StdPersistent_DataXtd_Constraint.hxx>
#include <StdPersistent_DataXtd_PatternStd.hxx>
#include <StdPersistent_PPrsStd.hxx>


//=======================================================================
//function : BindTypes
//purpose  : Register types
//=======================================================================
void StdPersistent::BindTypes (StdObjMgt_MapOfInstantiators& theMap)
{
  // Non-attribute data
  theMap.Bind <StdPersistent_TopLoc::Datum3D>      ("PTopLoc_Datum3D");
  theMap.Bind <StdPersistent_TopLoc::ItemLocation> ("PTopLoc_ItemLocation");
  theMap.Bind <StdPersistent_TopoDS::TShape>       ("PTopoDS_TShape1");
  theMap.Bind <StdPersistent_HArray1::Shape1>      ("PTopoDS_HArray1OfShape1");
  theMap.Bind <StdPersistent_Naming::Name>         ("PNaming_Name");
  theMap.Bind <StdPersistent_Naming::Name_1>       ("PNaming_Name_1");
  theMap.Bind <StdPersistent_Naming::Name_2>       ("PNaming_Name_2");

  theMap.Bind <StdLPersistent_HArray1::Persistent>
    ("PNaming_HArray1OfNamedShape");

  // Attributes
  theMap.Bind <StdPersistent_Naming::NamedShape>   ("PNaming_NamedShape");
  theMap.Bind <StdPersistent_Naming::Naming>       ("PNaming_Naming");
  theMap.Bind <StdPersistent_Naming::Naming_1>     ("PNaming_Naming_1");
  theMap.Bind <StdPersistent_Naming::Naming_2>     ("PNaming_Naming_2");

  theMap.Bind <StdPersistent_DataXtd::Shape>       ("PDataXtd_Shape");
  theMap.Bind <StdPersistent_DataXtd::Point>       ("PDataXtd_Point");
  theMap.Bind <StdPersistent_DataXtd::Axis>        ("PDataXtd_Axis");
  theMap.Bind <StdPersistent_DataXtd::Plane>       ("PDataXtd_Plane");
  theMap.Bind <StdPersistent_DataXtd::Placement>   ("PDataXtd_Placement");
  theMap.Bind <StdPersistent_DataXtd::Geometry>    ("PDataXtd_Geometry");
  theMap.Bind <StdPersistent_DataXtd::Position>    ("PDataXtd_Position");
  theMap.Bind <StdPersistent_DataXtd_Constraint>   ("PDataXtd_Constraint");
  theMap.Bind <StdPersistent_DataXtd_PatternStd>   ("PDataXtd_PatternStd");

  theMap.Bind <StdPersistent_PPrsStd::AISPresentation>
    ("PPrsStd_AISPresentation");

  theMap.Bind <StdPersistent_PPrsStd::AISPresentation_1>
    ("PPrsStd_AISPresentation_1");
}
