// File:	AIS_ConnectedShape.cxx
// Created:	Wed Apr 10 16:40:43 1996
// Author:	Guest Design
//		<g_design>


#include <Standard_NotImplemented.hxx>

#include <AIS_ConnectedShape.ixx>

#include <AIS_InteractiveContext.hxx>
#include <AIS_Drawer.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <StdPrs_WFDeflectionShape.hxx>
#include <StdPrs_HLRPolyShape.hxx>
#include <Prs3d_Drawer.hxx>
#include <Aspect_TypeOfDeflection.hxx>
#include <BRepTools.hxx>
#include <OSD_Timer.hxx>
#include <StdSelect_BRepSelectionTool.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <StdSelect.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
#include <Select3D_SensitiveEntity.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectBasics_EntityOwner.hxx>
#include <AIS_MultipleConnectedShape.hxx>
#include <Precision.hxx>

//=======================================================================
//function : AIS_ConnectedShape
//purpose  : 
//=======================================================================

AIS_ConnectedShape::AIS_ConnectedShape (const Handle(AIS_Shape)& TheAISShape,
					const PrsMgr_TypeOfPresentation3d TheType): 
AIS_ConnectedInteractive(TheType)
{
  myReference = TheAISShape;
}

AIS_ConnectedShape::AIS_ConnectedShape (const Handle(AIS_ConnectedShape)& TheConnectedShape,
					const PrsMgr_TypeOfPresentation3d TheType): 
AIS_ConnectedInteractive(TheType)
{
  myReference = TheConnectedShape;
}


//=======================================================================
//function : Type
//purpose  : 
//=======================================================================

AIS_KindOfInteractive AIS_ConnectedShape::Type() const
{return AIS_KOI_Shape;}

//=======================================================================
//function : Signature
//purpose  : 
//=======================================================================
Standard_Integer AIS_ConnectedShape::Signature() const
{return 1;}

//=======================================================================
//function : AcceptShapeDecomposition
//purpose  : 
//=======================================================================
Standard_Boolean AIS_ConnectedShape::AcceptShapeDecomposition() const 
{return Standard_True;}

//=======================================================================
//function : Compute Hidden Lines
//purpose  : 
//=======================================================================

void AIS_ConnectedShape::Compute(const Handle(Prs3d_Projector)& aProjector, 
				 const Handle(Prs3d_Presentation)& aPresentation)
{
  UpdateShape(Standard_True);
  Compute(aProjector,aPresentation,myOwnSh);
}


//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================
void AIS_ConnectedShape::Compute(const Handle(Prs3d_Projector)& aProjector, 
				 const Handle(Geom_Transformation)& TheTrsf,
				 const Handle(Prs3d_Presentation)& aPresentation)
{
  UpdateShape(Standard_False);
  const TopLoc_Location& loc = myOwnSh.Location();
  TopoDS_Shape shbis = myOwnSh.Located(TopLoc_Location(TheTrsf->Trsf())*loc);
  Compute(aProjector,aPresentation,shbis);
}


//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void AIS_ConnectedShape::Compute(const Handle(Prs3d_Projector)& aProjector, 
				 const Handle(Prs3d_Presentation)& aPresentation,
				 const TopoDS_Shape& SH)
{
//  Standard_Boolean recompute = Standard_False;
//  Standard_Boolean myFirstCompute = Standard_True;
  switch (SH.ShapeType()){
  case TopAbs_VERTEX:
  case TopAbs_EDGE:
  case TopAbs_WIRE:
    {
      aPresentation->SetDisplayPriority(4);
      StdPrs_WFDeflectionShape::Add(aPresentation,SH,myDrawer);
      break;
    }
  default:
    {
      Handle (Prs3d_Drawer) defdrawer = GetContext()->DefaultDrawer();
      if (defdrawer->DrawHiddenLine()) 
	{myDrawer->EnableDrawHiddenLine();}
      else {myDrawer->DisableDrawHiddenLine();}
      
      Aspect_TypeOfDeflection prevdef = defdrawer->TypeOfDeflection();
      defdrawer->SetTypeOfDeflection(Aspect_TOD_RELATIVE);

      // traitement HLRAngle et HLRDeviationCoefficient()
      Standard_Real prevangl = myDrawer->HLRAngle();
      Standard_Real newangl = defdrawer->HLRAngle();
      if (Abs(newangl- prevangl) > Precision::Angular()) {
#ifdef DEB
	cout << "AIS_MultipleConnectedShape : compute"<<endl;
	cout << "newangl   : " << newangl << " # de " << "prevangl  : " << prevangl << endl;
#endif	
	BRepTools::Clean(SH);
      }
      myDrawer->SetHLRAngle(newangl);
      myDrawer->SetHLRDeviationCoefficient(defdrawer->HLRDeviationCoefficient());
      StdPrs_HLRPolyShape::Add(aPresentation,SH,myDrawer,aProjector);
      defdrawer->SetTypeOfDeflection (prevdef);
    }
  }
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void AIS_ConnectedShape::Compute(const Handle_PrsMgr_PresentationManager2d& aPresentationManager2d,
                                 const Handle_Graphic2d_GraphicObject& aGraphicObject,
                                 const int anInteger)
{
// Standard_NotImplemented::Raise("AIS_ConnectedShape::Compute(const Handle_PrsMgr_PresentationManager2d&, const Handle_Graphic2d_GraphicObject&, const int)");
 AIS_ConnectedInteractive::Compute( aPresentationManager2d ,aGraphicObject,anInteger) ; 
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void AIS_ConnectedShape::Compute(const Handle_PrsMgr_PresentationManager3d& /*aPresentationManager3d*/,
                                 const Handle_Prs3d_Presentation& /*aPresentation*/,
                                 const int /*anInteger*/)
{
 Standard_NotImplemented::Raise("AIS_ConnectedShape::Compute(const Handle_PrsMgr_PresentationManager3d&, const Handle_Prs3d_Presentation&, const int)");
// AIS_ConnectedInteractive::Compute( aPresentationManager3d ,aPresentation,anInteger) ;  not accessible
}

//=======================================================================
//function : ComputeSelection 
//purpose  : Attention fragile...
//=======================================================================

void AIS_ConnectedShape::ComputeSelection (const Handle(SelectMgr_Selection)& aSelection,
					   const Standard_Integer             aMode)
{
  UpdateShape();
  aSelection->Clear();
  // On regarde s'il n'y a pas des choses a faire sur la reference,
  // du type mise a jour...
  
  if(!myReference->HasSelection(aMode))
    myReference->UpdateSelection(aMode);
  const Handle(SelectMgr_Selection)& RefSel = myReference->Selection(aMode);
  if(RefSel->IsEmpty())
    myReference->UpdateSelection(aMode);
  if(RefSel->UpdateStatus()==SelectMgr_TOU_Full)
    myReference->UpdateSelection(aMode);

  //suivant le type de decomposition, on deduit les primitives connectees.
  // il faut suivre l'ordre de creation de StdSelect_BRepSelectionTool...
  
  TopAbs_ShapeEnum TheType = AIS_Shape::SelectionType(aMode);
  Handle(StdSelect_BRepOwner) OWNR;
  Handle(Select3D_SensitiveEntity) SE,NiouSE;
  TopLoc_Location BidLoc;

  switch(TheType){
  case TopAbs_VERTEX:
  case TopAbs_EDGE:
  case TopAbs_WIRE:
  case TopAbs_FACE:
  case TopAbs_SHELL:
    {
      TopTools_IndexedMapOfShape subshaps;
      TopExp::MapShapes(myOwnSh,TheType,subshaps);

      RefSel->Init();
      for(Standard_Integer I=1;
	  I<=subshaps.Extent()&& RefSel->More();
	  RefSel->Next(),I++){
	
	SE = *((Handle(Select3D_SensitiveEntity)*) &(RefSel->Sensitive()));
	if(!SE.IsNull()){
	  OWNR = new StdSelect_BRepOwner(subshaps(I),this,SE->OwnerId()->Priority());
	  
	  
	  if(myLocation.IsIdentity())
	    NiouSE = SE->GetConnected(BidLoc);
	  else
	    NiouSE = SE->GetConnected(myLocation);
	  NiouSE->Set(OWNR);
	  aSelection->Add(NiouSE);
	}
      }
     break;
    }
    
  case TopAbs_SHAPE:
  default:
    {
      // Dans ce cas on n'a qu'un seul proprietaire pour l'ensemble des
      // primitives sensibles...
      OWNR = new StdSelect_BRepOwner(myOwnSh,this);
      Standard_Boolean FirstIncr(Standard_True);
      for(RefSel->Init();RefSel->More();RefSel->Next()){
	SE = *((Handle(Select3D_SensitiveEntity)*) &(RefSel->Sensitive()));
	if(FirstIncr){
	  Standard_Integer Prior = SE->OwnerId()->Priority();
	  Handle(SelectBasics_EntityOwner)::DownCast(OWNR)->Set(Prior);
	  FirstIncr = Standard_False;}

	if(myLocation.IsIdentity())
	  NiouSE = SE->GetConnected(BidLoc);
	else
	  NiouSE = SE->GetConnected(myLocation);
	NiouSE->Set(OWNR);
	aSelection->Add(NiouSE);
      }
      break;
    }
  }
  StdSelect::SetDrawerForBRepOwner(aSelection,myDrawer);
  
}
 

//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

const TopoDS_Shape& AIS_ConnectedShape::Shape()
{ 
  UpdateShape(); 
  return myOwnSh;
}
AIS_ConnectedShape::AIS_ConnectedShape(const PrsMgr_TypeOfPresentation3d aTypeOfPresentation3d):
AIS_ConnectedInteractive(aTypeOfPresentation3d)
{    
  SetHilightMode(0);
}
  
//=======================================================================
//function : UpdateShape
//purpose  : if<WithLocation=False> computes only the deducted shape
//           from reference; does not put "myLocation"
//=======================================================================
void AIS_ConnectedShape::UpdateShape(const Standard_Boolean WithTheLocation)
{
  if(myReference.IsNull()) return;

  // attention grosse ligne...
  if(myReference->Type()!=AIS_KOI_Shape) return;

  Standard_Integer Sig = myReference->Signature();
  
  TopoDS_Shape S ;
  switch(Sig){
  case 0:
    S = (*((Handle(AIS_Shape)*) &myReference))->Shape();
    break;
  case 1:
    S = (*((Handle(AIS_ConnectedShape)*) &myReference))->Shape();
    break;
  case 2:
    S=(*((Handle(AIS_MultipleConnectedShape)*) &myReference))->Shape();
    break;
  default:
    S = myOwnSh;
  }
  if(S.IsNull()) return;
  if(myLocation.IsIdentity() || !WithTheLocation) 
    myOwnSh = S;
  else
    myOwnSh = S.Moved(myLocation);
}


//=======================================================================
//function : Connect
//purpose  : 
//=======================================================================
void AIS_ConnectedShape::
Connect(const Handle(AIS_InteractiveObject)& anotherIObj)
{
  if(anotherIObj->Type()== AIS_KOI_Shape){
    Standard_Integer Sig = anotherIObj->Signature();
    if(Sig <=2)
      AIS_ConnectedInteractive::Connect(anotherIObj);
  }
}

//=======================================================================
//function : Connect
//purpose  : 
//=======================================================================
void AIS_ConnectedShape::
Connect(const Handle(AIS_InteractiveObject)& anotherIObj, 
	const TopLoc_Location& aLocation)
{
  if(anotherIObj->Type()== AIS_KOI_Shape){
    Standard_Integer Sig = anotherIObj->Signature();
    if(Sig <=2)
      AIS_ConnectedInteractive::Connect(anotherIObj,aLocation);
  }
}

