#include <User_Cylinder.ixx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_Drawer.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <TopExp_Explorer.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Plane.hxx>
#include <StdPrs_WFDeflectionShape.hxx>
#include <StdPrs_ShadedShape.hxx>
#include <StdPrs_HLRPolyShape.hxx>
#include <StdSelect_BRepSelectionTool.hxx>



User_Cylinder::User_Cylinder(const Standard_Real R,const Standard_Real H)
{
  BRepPrimAPI_MakeCylinder S(R,H);
  myShape = S.Shape();
  SetHilightMode(0);
  SetSelectionMode(0);
  myDrawer->SetShadingAspect(new Prs3d_ShadingAspect());
  myPlanarFaceColor = Quantity_NOC_FIREBRICK3;
  myCylindricalFaceColor = Quantity_NOC_AZURE;
}

 Standard_Integer User_Cylinder::NbPossibleSelection() 
{
  return 2;
}

 void User_Cylinder::Compute(const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,
			     const Handle(Prs3d_Presentation)& aPresentation,
			     const Standard_Integer aMode) 
{
  switch (aMode) 
    {
    case 0:
      StdPrs_WFDeflectionShape::Add(aPresentation,myShape, myDrawer);
      break;
    case 1:
      myDrawer->ShadingAspect()->SetMaterial(Graphic3d_NOM_PLASTIC);
      myDrawer->SetShadingAspectGlobal(Standard_False);
      TopExp_Explorer Ex;
      Handle(Geom_Surface) Surface;
      for (Ex.Init(myShape,TopAbs_FACE); Ex.More(); Ex.Next())
	{
	  Surface = BRep_Tool::Surface(TopoDS::Face(Ex.Current()));
	  if (Surface->IsKind(STANDARD_TYPE(Geom_Plane)))
	    myDrawer->ShadingAspect()->SetColor(myPlanarFaceColor);
	  else
	    myDrawer->ShadingAspect()->SetColor(myCylindricalFaceColor);
	  
	  StdPrs_ShadedShape::Add(aPresentation,Ex.Current(), myDrawer);
	}
      break;
    }
}

 void User_Cylinder::Compute(const Handle(Prs3d_Projector)& aProjector,
			     const Handle(Prs3d_Presentation)& aPresentation) 
{
  myDrawer->EnableDrawHiddenLine();
  StdPrs_HLRPolyShape::Add(aPresentation,myShape, myDrawer, aProjector);
}

 void User_Cylinder::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
				      const Standard_Integer aMode) 
{
  switch(aMode)
    {
    case 0:
      StdSelect_BRepSelectionTool::Load(aSelection,this,myShape,TopAbs_SHAPE,0,0);
      break;
    case 4:
      StdSelect_BRepSelectionTool::Load(aSelection,this,myShape,TopAbs_FACE,0,0);
      break;
    }
}

 void User_Cylinder::SetCylindricalFaceColor(const Quantity_NameOfColor aColor) 
{
  myCylindricalFaceColor = aColor;
}

 void User_Cylinder::SetPlanarFaceColor(const Quantity_NameOfColor aColor) 
{
  myPlanarFaceColor = aColor;
}

 Standard_Boolean User_Cylinder::AcceptShapeDecomposition() const
{
  return Standard_True;
}

