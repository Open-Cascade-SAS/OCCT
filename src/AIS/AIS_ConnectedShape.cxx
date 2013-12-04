// Created on: 1996-04-10
// Created by: Guest Design
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

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
#include <TopTools_OrientedShapeMapHasher.hxx>
#include <TopExp.hxx>
#include <Select3D_SensitiveEntity.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectBasics_EntityOwner.hxx>
#include <AIS_MultipleConnectedShape.hxx>
#include <Precision.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_List.hxx>

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

      // process HLRAngle and HLRDeviationCoefficient()
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
//function : ComputeSelection 
//purpose  : Attention fragile...
//=======================================================================

void AIS_ConnectedShape::ComputeSelection (const Handle(SelectMgr_Selection)& aSelection, 
                                           const Standard_Integer aMode)
{
  typedef NCollection_List<Handle(Select3D_SensitiveEntity)> SensitiveList;
  typedef NCollection_DataMap<TopoDS_Shape, SensitiveList, TopTools_OrientedShapeMapHasher>
    Shapes2EntitiesMap;

  UpdateShape();
  aSelection->Clear();
  // It is checked if there is nothing to do with the reference
  // of type update...
  
  if(!myReference->HasSelection(aMode))
    myReference->UpdateSelection(aMode);
  const Handle(SelectMgr_Selection)& aRefSel = myReference->Selection(aMode);
  if(aRefSel->IsEmpty())
    myReference->UpdateSelection(aMode);

  if(aRefSel->UpdateStatus()==SelectMgr_TOU_Full)
    myReference->UpdateSelection(aMode);
  
  Handle(StdSelect_BRepOwner) anOwner;
  TopLoc_Location aBidLoc;
  Handle(Select3D_SensitiveEntity) aSE, aNewSE;
  Shapes2EntitiesMap aShapes2EntitiesMap;
  SensitiveList aSEList;
  TopoDS_Shape aSubShape;

  // Fill in the map of subshapes and corresponding 
  // sensitive entities associated with aMode 
  for(aRefSel->Init(); aRefSel->More(); aRefSel->Next())
  {
    aSE = Handle(Select3D_SensitiveEntity)::DownCast(aRefSel->Sensitive()); 
    if(!aSE.IsNull())
    {
      anOwner = Handle(StdSelect_BRepOwner)::DownCast(aSE->OwnerId());
      if(!anOwner.IsNull())
      {
        aSubShape = anOwner->Shape(); 
        if(!aShapes2EntitiesMap.IsBound(aSubShape))
        {
          aShapes2EntitiesMap.Bind(aSubShape, aSEList);
        }
        aShapes2EntitiesMap(aSubShape).Append(aSE);
      }
    }
  }

  // Fill in selection from aShapes2EntitiesMap
  Shapes2EntitiesMap::Iterator aMapIt(aShapes2EntitiesMap);
  for(; aMapIt.More(); aMapIt.Next())
  {
    aSEList = aMapIt.Value();
    anOwner = new StdSelect_BRepOwner(aMapIt.Key(), 
                                      this, 
                                      aSEList.First()->OwnerId()->Priority(), 
                                      Standard_True);
    
    SensitiveList::Iterator aListIt(aSEList);
    for(; aListIt.More(); aListIt.Next())
    {
      aSE = aListIt.Value();
      if(myLocation.IsIdentity())
      {
        aNewSE = aSE->GetConnected(aBidLoc);
        aNewSE->Set(anOwner);
        // In case if aSE caches some location-dependent data 
        // that must be updated after setting anOwner
        aNewSE->SetLocation(aBidLoc);
      }
      else
      {
        aNewSE = aSE->GetConnected(myLocation); 
        aNewSE->Set(anOwner); 
        // In case if aSE caches some location-dependent data 
        // that must be updated after setting anOwner
        aNewSE->SetLocation(myLocation);
      }
      aSelection->Add(aNewSE);
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

  // attention great line...
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

