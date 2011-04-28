// Copyright: 	Matra-Datavision 1995
// File:	StdSelect.cxx
// Created:	Mon Mar 27 17:07:10 1995
// Author:	Robert COUBLANC
//		<rob>

#include <StdSelect.ixx>
#include <SelectMgr_Selection.hxx>
#include <Graphic3d_StructureManager.hxx>
#include <Graphic3d_Group.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <Select3D_SensitiveCurve.hxx>
#include <Select3D_SensitiveFace.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_Structure.hxx>
#include <Graphic3d_Array1OfVertex.hxx>
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
