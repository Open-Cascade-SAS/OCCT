// File:	AIS_ConnectedInteractive.cxx
// Created:	Wed Jan  8 15:24:35 1997
// Author:	Robert COUBLANC
//		<rob@robox.paris1.matra-dtv.fr>


#include <Standard_NotImplemented.hxx>

#include <AIS_ConnectedInteractive.ixx>
#include <SelectMgr_EntityOwner.hxx>
#include <Select3D_SensitiveEntity.hxx>
#include <Geom_Transformation.hxx>

#include <PrsMgr_ModedPresentation.hxx>
#include <PrsMgr_Presentation3d.hxx>
#include <OSD_Timer.hxx>


//=======================================================================
//function : AIS_ConnectedInteractive
//purpose  : 
//=======================================================================
AIS_ConnectedInteractive::AIS_ConnectedInteractive(const PrsMgr_TypeOfPresentation3d aTypeOfPresentation3d):
AIS_InteractiveObject(aTypeOfPresentation3d)
{    
  SetHilightMode(0);
}

//=======================================================================
//function : Type
//purpose  : 
//=======================================================================
AIS_KindOfInteractive AIS_ConnectedInteractive::Type() const
{return AIS_KOI_Object;}

Standard_Integer AIS_ConnectedInteractive::Signature() const
{return 0; }




//=======================================================================
//function : Connect
//purpose  : 
//=======================================================================
void AIS_ConnectedInteractive::
Connect(const Handle(AIS_InteractiveObject)& anotherIObj)
{
  // Pour Avoir le temps de faire le Disconnect en dessous, 
  // on garde l'ancien un peu. SMO. 
  if(myReference==anotherIObj) return;
  myOldReference = myReference;
//Disconnect();
  myReference = anotherIObj ;
}

//=======================================================================
//function : Connect
//purpose  : 
//=======================================================================
void AIS_ConnectedInteractive::
Connect(const Handle(AIS_InteractiveObject)& anotherIobj, 
	const TopLoc_Location& aLocation)
{
  if(myLocation!=aLocation)
    myLocation = aLocation;
  if(myReference!=anotherIobj) {
    myOldReference = myReference; // necessaire pour pouvoir faire le disconnect en dessous..
//  Disconnect();
    myReference = anotherIobj;}
  
}


//=======================================================================
//function : Disconnect
//purpose  : 
//=======================================================================

void AIS_ConnectedInteractive::Disconnect()
{
  for(Standard_Integer i =1;i<=myPresentations.Length();i++)
    {
      Handle(PrsMgr_Presentation3d) P = Handle(PrsMgr_Presentation3d)::DownCast(myPresentations(i).Presentation());
      if(!P.IsNull()) {
	P->Presentation()->DisconnectAll(Graphic3d_TOC_DESCENDANT);
      }
    }
}
//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void AIS_ConnectedInteractive::
Compute(const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,
	const Handle(Prs3d_Presentation)& aPresentation, 
	const Standard_Integer aMode)
{
  static Handle(Geom_Transformation) myPrsTrans ;

  if(!(HasLocation() ||HasConnection())) return;
  
  if(HasConnection()){
    aPresentation->Clear(Standard_False);
    aPresentation->RemoveAll();
    aPresentationManager->Connect( this,myReference, aMode, aMode);
    if(aPresentationManager->Presentation(myReference,aMode)->MustBeUpdated())
      aPresentationManager->Update(myReference,aMode);
    
  }
  if(HasLocation()){
    myPrsTrans = new Geom_Transformation(myLocation.Transformation());
    aPresentationManager->Transform(this, myPrsTrans, aMode);  }
  aPresentation->ReCompute();
}

void AIS_ConnectedInteractive::Compute(const Handle_Prs3d_Projector& aProjector, const Handle_Geom_Transformation& aTransformation, const Handle_Prs3d_Presentation& aPresentation)
{
// Standard_NotImplemented::Raise("AIS_ConnectedInteractive::Compute(const Handle_Prs3d_Projector&, const Handle_Geom_Transformation&, const Handle_Prs3d_Presentation&)");
  PrsMgr_PresentableObject::Compute( aProjector , aTransformation , aPresentation ) ;
}

void AIS_ConnectedInteractive::Compute(const Handle_Prs3d_Projector& aProjector, const Handle_Prs3d_Presentation& aPresentation)
{
// Standard_NotImplemented::Raise("AIS_ConnectedInteractive::Compute(const Handle_Prs3d_Projector&, const Handle_Prs3d_Presentation&)");
 PrsMgr_PresentableObject::Compute( aProjector , aPresentation ) ;
}

void AIS_ConnectedInteractive::Compute(const Handle_PrsMgr_PresentationManager2d& aPresentationManager2d, const Handle_Graphic2d_GraphicObject& aGraphicObject, const int anInteger)
{
// Standard_NotImplemented::Raise("AIS_ConnectedInteractive::Compute(const Handle_PrsMgr_PresentationManager2d&, const Handle_Graphic2d_GraphicObject&, const int)");
 PrsMgr_PresentableObject::Compute( aPresentationManager2d ,aGraphicObject,anInteger) ;
}

//=======================================================================
//function : ComputeSelection
//purpose  : 
//=======================================================================
void AIS_ConnectedInteractive::ComputeSelection(const Handle(SelectMgr_Selection)& aSel,
						const Standard_Integer aMode)
{
  if(!(HasLocation() ||HasConnection())) return;
  
  aSel->Clear();
  if(!myReference->HasSelection(aMode))
    myReference->UpdateSelection(aMode);
  
  
//  static OSD_Timer KronSel;
//  cout<<"debut calcul connexion primitives pour le mode "<<aMode<<endl;
//  KronSel.Reset();
//  KronSel.Start();
  
  const Handle(SelectMgr_Selection)& TheRefSel = myReference->Selection(aMode);
  Handle(SelectMgr_EntityOwner) OWN = new SelectMgr_EntityOwner(this);
  Handle(Select3D_SensitiveEntity) SE3D,SNew;
  
  if(TheRefSel->IsEmpty())
    myReference->UpdateSelection(aMode);
  for(TheRefSel->Init();TheRefSel->More();TheRefSel->Next()){
    SE3D = *((Handle(Select3D_SensitiveEntity)*) &(TheRefSel->Sensitive()));
    if(!SE3D.IsNull()){
      SNew = SE3D->GetConnected(myLocation);
      if(aMode==0)
	SNew->Set(OWN);
      aSel->Add(SNew);
    }
  }
//  KronSel.Stop();
//  cout<<"fin calcul connexion primitives pour le mode "<<aMode<<endl;
//  KronSel.Show();
}

void AIS_ConnectedInteractive::UpdateLocation()
{
// Standard_NotImplemented::Raise("AIS_ConnectedInteractive::UpdateLocation()");
 SelectMgr_SelectableObject::UpdateLocation() ;
}
void AIS_ConnectedInteractive::UpdateLocation(const Handle(SelectMgr_Selection)& Sel)
{
// Standard_NotImplemented::Raise("AIS_ConnectedInteractive::UpdateLocation(const Handle(SelectMgr_Selection)& Sel)");
 SelectMgr_SelectableObject::UpdateLocation(Sel) ;
}
/*void AIS_ConnectedInteractive::UpdateLocation(const Handle_Prs3d_Presentation& aPresentation)
{
// Standard_NotImplemented::Raise("AIS_ConnectedInteractive::UpdateLocation(const Handle_Prs3d_Presentation&)");
 SelectMgr_SelectableObject::UpdateLocation(aPresentation) ;
}*/
