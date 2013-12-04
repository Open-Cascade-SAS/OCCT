// Created on: 1997-04-22
// Created by: Guest Design
// Copyright (c) 1997-1999 Matra Datavision
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

#include <AIS_MultipleConnectedShape.ixx>



#include <AIS_InteractiveContext.hxx>
#include <AIS_Drawer.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <StdPrs_WFDeflectionShape.hxx>
#include <StdPrs_HLRPolyShape.hxx>
#include <Prs3d_Drawer.hxx>
#include <Aspect_TypeOfDeflection.hxx>
#include <Bnd_Box.hxx>
#include <BRepTools.hxx>
#include <BRepBndLib.hxx>
#include <OSD_Timer.hxx>
#include <StdSelect_BRepSelectionTool.hxx>
#include <StdSelect.hxx>
#include <Precision.hxx>

//=======================================================================
//function : AIS_ConnectedShape
//purpose  : 
//=======================================================================

AIS_MultipleConnectedShape::AIS_MultipleConnectedShape (const TopoDS_Shape& aShape):
AIS_MultipleConnectedInteractive(PrsMgr_TOP_ProjectorDependant),
myShape(aShape)
{
}

//=======================================================================
//function : Type
//purpose  : 
//=======================================================================
AIS_KindOfInteractive AIS_MultipleConnectedShape::Type() const
{return AIS_KOI_Shape;}


//=======================================================================
//function : Signature
//purpose  : 
//=======================================================================
Standard_Integer AIS_MultipleConnectedShape::Signature() const
{return 2;}


//=======================================================================
//function : AcceptShapeDecomposition
//purpose  : 
//=======================================================================

Standard_Boolean AIS_MultipleConnectedShape::AcceptShapeDecomposition() const 
{return Standard_True;}




//=======================================================================
//function : Compute Hidden Lines
//purpose  : 
//=======================================================================

void AIS_MultipleConnectedShape::Compute(const Handle(Prs3d_Projector)& aProjector, 
					 const Handle(Prs3d_Presentation)& aPresentation)
{
  Compute(aProjector,aPresentation,myShape);
  
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void AIS_MultipleConnectedShape::Compute(const Handle(Prs3d_Projector)& aProjector, 
					 const Handle(Geom_Transformation)& aTrsf,
					 const Handle(Prs3d_Presentation)& aPresentation)
{
  aPresentation->Clear();

  const TopLoc_Location& loc = myShape.Location();
  TopoDS_Shape shbis = myShape.Located(TopLoc_Location(aTrsf->Trsf())*loc);
  Compute(aProjector,aPresentation,shbis);
}



  
void AIS_MultipleConnectedShape::Compute(const Handle(Prs3d_Projector)& aProjector, 
					 const Handle(Prs3d_Presentation)& aPresentation,
					 const TopoDS_Shape& SH)
{
  //Standard_Boolean recompute = Standard_False;
  //Standard_Boolean myFirstCompute = Standard_True;
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

void AIS_MultipleConnectedShape::Compute(const Handle_PrsMgr_PresentationManager3d& aPresentationManager3d,
                                         const Handle_Prs3d_Presentation& aPresentation,
                                         const int anint)
{
  AIS_MultipleConnectedInteractive::Compute( aPresentationManager3d , aPresentation , anint ) ;
}

//=======================================================================
//function : ComputeSelection 
//purpose  : 
//=======================================================================

void AIS_MultipleConnectedShape::ComputeSelection (const Handle(SelectMgr_Selection)& aSelection,
						   const Standard_Integer             aMode)
{
  //cout<<"AIS_MultipleConnectedShape::ComputeSelection"<<endl;

  Standard_Real aDeviationAngle = myDrawer->DeviationAngle();
  Standard_Real aDeflection = myDrawer->MaximalChordialDeviation();
  if (myDrawer->TypeOfDeflection() == Aspect_TOD_RELATIVE)
  {
    // On calcule la fleche en fonction des min max globaux de la piece:
    Bnd_Box aBndBox; //= BoundingBox(); ?
    BRepBndLib::Add (myShape, aBndBox);
    if (!aBndBox.IsVoid())
    {
      Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
      aBndBox.Get (aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
      aDeflection = Max (aXmax - aXmin, Max (aYmax - aYmin, aZmax - aZmin)) * myDrawer->DeviationCoefficient();
    }
  }

  switch(aMode){
  case 1:
    StdSelect_BRepSelectionTool::Load(aSelection,this,myShape,TopAbs_VERTEX, aDeflection, aDeviationAngle);
    break;
  case 2:
    StdSelect_BRepSelectionTool::Load(aSelection,this,myShape,TopAbs_EDGE, aDeflection, aDeviationAngle);
      break;
  case 3:
    StdSelect_BRepSelectionTool::Load(aSelection,this,myShape,TopAbs_WIRE, aDeflection, aDeviationAngle);
    break;
  case 4:
    StdSelect_BRepSelectionTool::Load(aSelection,this,myShape,TopAbs_FACE, aDeflection, aDeviationAngle);
    break;
  case 5:
    StdSelect_BRepSelectionTool::Load(aSelection,this,myShape,TopAbs_SHELL, aDeflection, aDeviationAngle);
    break;
  case 6:
    StdSelect_BRepSelectionTool::Load(aSelection,this,myShape,TopAbs_SOLID, aDeflection, aDeviationAngle);
    break;
  case 7:
    StdSelect_BRepSelectionTool::Load(aSelection,this,myShape,TopAbs_COMPOUND, aDeflection, aDeviationAngle);
    break;
  case 8:
    StdSelect_BRepSelectionTool::Load(aSelection,this,myShape,TopAbs_COMPSOLID, aDeflection, aDeviationAngle);
    break;
  default:
    StdSelect_BRepSelectionTool::Load(aSelection,this,myShape,TopAbs_SHAPE, aDeflection, aDeviationAngle);
    break;
  }
  // insert the drawer in the BrepOwners for hilight...
  StdSelect::SetDrawerForBRepOwner(aSelection,myDrawer);
  
}

  
  
  
