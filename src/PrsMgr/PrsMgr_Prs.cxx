#include <PrsMgr_Prs.ixx>
#include <PrsMgr_Presentation3d.hxx>
#include <Precision.hxx>
#include <gp_Trsf.hxx>
#include <Geom_Transformation.hxx>

PrsMgr_Prs::PrsMgr_Prs (const Handle(Graphic3d_StructureManager)& aStructureManager,
			const PrsMgr_Presentation3dPointer& aPresentation3d, 
			const PrsMgr_TypeOfPresentation3d aTypeOfPresentation)
:Prs3d_Presentation(aStructureManager),myPresentation3d(aPresentation3d) 
{
  
  if (aTypeOfPresentation == PrsMgr_TOP_ProjectorDependant)
    SetVisual(Graphic3d_TOS_COMPUTED);
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

Handle(Graphic3d_Structure) PrsMgr_Prs::Compute(const Handle(Graphic3d_DataStructureManager)& aProjector) {
  return myPresentation3d->Compute(aProjector);
}
//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

Handle(Graphic3d_Structure) PrsMgr_Prs::Compute(const Handle(Graphic3d_DataStructureManager)& aProjector,
						const TColStd_Array2OfReal& AMatrix) 
{
  gp_Trsf TheTrsf;
  Standard_Integer LC(AMatrix.LowerCol()),LR(AMatrix.LowerRow());
  TheTrsf.SetValues(AMatrix(LR,LC),AMatrix(LR,LC+1),AMatrix(LR,LC+2),AMatrix(LR,LC+3),
		    AMatrix(LR+1,LC),AMatrix(LR+1,LC+1),AMatrix(LR+1,LC+2),AMatrix(LR+1,LC+3),
		    AMatrix(LR+2,LC),AMatrix(LR+2,LC+1),AMatrix(LR+2,LC+2),AMatrix(LR+2,LC+3),
		    Precision::Angular(),Precision::Confusion());
   Handle(Geom_Transformation) G = new Geom_Transformation(TheTrsf);
  
  return myPresentation3d->Compute(aProjector,G);
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void PrsMgr_Prs::Compute(const Handle(Graphic3d_DataStructureManager)& aProjector,
			 Handle(Graphic3d_Structure)& aGivenStruct) 
{
  myPresentation3d->Compute(aProjector,aGivenStruct);
}


//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void PrsMgr_Prs::Compute(const Handle(Graphic3d_DataStructureManager)& aProjector,
			 const TColStd_Array2OfReal& AMatrix,
			  Handle(Graphic3d_Structure)& aGivenStruct) 
{
  gp_Trsf TheTrsf;
  Standard_Integer LC(AMatrix.LowerCol()),LR(AMatrix.LowerRow());
  TheTrsf.SetValues(AMatrix(LR,LC),AMatrix(LR,LC+1),AMatrix(LR,LC+2),AMatrix(LR,LC+3),
		    AMatrix(LR+1,LC),AMatrix(LR+1,LC+1),AMatrix(LR+1,LC+2),AMatrix(LR+1,LC+3),
		    AMatrix(LR+2,LC),AMatrix(LR+2,LC+1),AMatrix(LR+2,LC+2),AMatrix(LR+2,LC+3),
		    Precision::Angular(),Precision::Confusion());
  Handle(Geom_Transformation) G = new Geom_Transformation(TheTrsf);


  myPresentation3d->Compute(aProjector,G,aGivenStruct);
}
