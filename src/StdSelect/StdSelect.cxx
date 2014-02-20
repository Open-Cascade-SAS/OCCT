// Created on: 1995-03-27
// Created by: Robert COUBLANC
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

#include <StdSelect.ixx>
#include <SelectMgr_Selection.hxx>
#include <Graphic3d_StructureManager.hxx>
#include <Graphic3d_Group.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <Select3D_SensitiveCurve.hxx>
#include <Select3D_SensitiveFace.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_Structure.hxx>
#include <Quantity_NameOfColor.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax3.hxx>
#include <V3d_PerspectiveView.hxx>
#include <StdSelect_BRepOwner.hxx>

Handle(Select3D_Projector) StdSelect::GetProjector(const Handle(V3d_View)& aViou)
{
  Standard_Real Focale=0.,Xat,Yat,Zat,XUp,YUp,ZUp,DX,DY,DZ;
  Standard_Boolean Pers=Standard_False;
  // NKV - 31/07/07 - Fix for perspective view
  if ( aViou->Type() == V3d_PERSPECTIVE ) {
    Pers = Standard_True;
    Focale = aViou->Focale();} // must be replaced by the method Focale 
  
  aViou->At(Xat,Yat,Zat);
  aViou->Up(XUp,YUp,ZUp);
  aViou->Proj(DX,DY,DZ);
  gp_Pnt At (Xat,Yat,Zat); 
  gp_Dir Zpers (DX,DY,DZ);
  gp_Dir Ypers (XUp,YUp,ZUp);
  gp_Dir Xpers = Ypers.Crossed(Zpers);
  gp_Ax3 Axe (At, Zpers, Xpers);
  gp_Trsf T;
  T.SetTransformation(Axe);
  return new Select3D_Projector(T,Pers,Focale);
  
}

//=======================================================================
//function : SetDrawerForBRepOwner
//purpose  : 
//=======================================================================

void StdSelect::SetDrawerForBRepOwner(const Handle(SelectMgr_Selection)& /*Sel*/,
				     const Handle(Prs3d_Drawer)& /*Drwr*/)
{
//  Handle(StdSelect_BRepOwner) BROWN;

//   for(Sel->Init();Sel->More();Sel->Next()){
//     BROWN = Handle(StdSelect_BRepOwner)::DownCast(Sel->Sensitive()->OwnerId());
//     if(!BROWN.IsNull())
//       BROWN->SetDrawer(Drwr);
//   }
}
