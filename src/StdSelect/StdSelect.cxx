// Created on: 1995-03-27
// Created by: Robert COUBLANC
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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

void StdSelect::SetDrawerForBRepOwner(const Handle(SelectMgr_Selection)& Sel,
				     const Handle(Prs3d_Drawer)& Drwr)
{
//  Handle(StdSelect_BRepOwner) BROWN;

//   for(Sel->Init();Sel->More();Sel->Next()){
//     BROWN = Handle(StdSelect_BRepOwner)::DownCast(Sel->Sensitive()->OwnerId());
//     if(!BROWN.IsNull())
//       BROWN->SetDrawer(Drwr);
//   }
}
