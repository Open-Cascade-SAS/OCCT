// File:	PrsMgr_Presentation3d.cxx
// Created:	Thu Oct 21 13:11:19 1993
// Author:	Jean-Louis FRENKEL
// Modified by  rob 09-oct-96
//		<jlf@stylox>


#define IMP040200	//GG Recompute HLR after transformation
//			in all the case.

#include <PrsMgr_Presentation3d.ixx>
#include <PrsMgr_PresentationManager.hxx>
#include <PrsMgr_Prs.hxx>
#include <Visual3d_View.hxx>
#include <Visual3d_ViewOrientation.hxx>
#include <Graphic3d_Structure.hxx>
#include <Precision.hxx>

PrsMgr_Presentation3d::PrsMgr_Presentation3d (const Handle(PrsMgr_PresentationManager3d)& aPresentationManager, 
                                              const Handle(PrsMgr_PresentableObject)& aPresentableObject)
: PrsMgr_Presentation(aPresentationManager), 
  myDisplayReason(Standard_False),
  myPresentableObject (aPresentableObject.operator->())
{
  myStructure = new PrsMgr_Prs(aPresentationManager->StructureManager(),
                               this, aPresentableObject->TypeOfPresentation3d());
  myStructure->SetOwner(myPresentableObject);
}

PrsMgr_KindOfPrs PrsMgr_Presentation3d::KindOfPresentation() const
{return PrsMgr_KOP_3D;}


void PrsMgr_Presentation3d::Display () {
  myStructure->Display();
  myDisplayReason = Standard_False;
}

void PrsMgr_Presentation3d::Erase () const {
  myStructure->Erase();}

void PrsMgr_Presentation3d::Highlight () {
  if(!myStructure->IsDisplayed()) {
    myStructure->Display();
    myDisplayReason = Standard_True;
  }
  myStructure->Highlight();}

void PrsMgr_Presentation3d::Unhighlight () const {
  myStructure->UnHighlight();
  if(myDisplayReason) myStructure->Erase();
}

void PrsMgr_Presentation3d::Clear() {
  // This modification remove the contain of the structure:
  // Consequence: 
  //    1. The memory zone of the group is reused
  //    2. The speed for animation is constant
  //myPresentableObject = NULL;
  SetUpdateStatus(Standard_True);
  myStructure->Clear(Standard_True);
  //  myStructure->Clear(Standard_False);

  myStructure->RemoveAll();
}

void PrsMgr_Presentation3d::Color(const Quantity_NameOfColor aColor){
  Standard_Boolean ImmMode = myPresentationManager->IsImmediateModeOn();
  if(!ImmMode){
    if(!myStructure->IsDisplayed()) {
      myStructure->Display();
      myDisplayReason = Standard_True;
    }
  }
  myStructure->Color(aColor);
}

void PrsMgr_Presentation3d::BoundBox() const {
   myStructure->BoundBox();
}

Standard_Boolean PrsMgr_Presentation3d::IsDisplayed () const {
  return myStructure->IsDisplayed() && !myDisplayReason;
}        

Standard_Boolean PrsMgr_Presentation3d::IsHighlighted () const {
  return myStructure->IsHighlighted();
}


Standard_Integer PrsMgr_Presentation3d::DisplayPriority() const {
return myStructure->DisplayPriority();
}

void PrsMgr_Presentation3d::SetDisplayPriority(const Standard_Integer TheNewPrior)
{
  myStructure->SetDisplayPriority(TheNewPrior);
}

Handle(Prs3d_Presentation) PrsMgr_Presentation3d::Presentation() const {
  return myStructure;
}
void PrsMgr_Presentation3d::Connect(const Handle(PrsMgr_Presentation3d)& anOtherPresentation) const {
  myStructure->Connect(anOtherPresentation->Presentation());
}

void PrsMgr_Presentation3d::Transform (const Handle(Geom_Transformation)& aTransformation) const {
  myStructure->Transform(aTransformation);
}

void PrsMgr_Presentation3d::Place (const Quantity_Length X,
			      const Quantity_Length Y,
			      const Quantity_Length Z) const {
  myStructure->Place(X,Y,Z);
}

void PrsMgr_Presentation3d::Multiply (const Handle(Geom_Transformation)& aTransformation) const {
  myStructure->Multiply(aTransformation);
}

void PrsMgr_Presentation3d::Move (const Quantity_Length X,
			      const Quantity_Length Y,
			      const Quantity_Length Z) const {
  myStructure->Move(X,Y,Z);
}

void PrsMgr_Presentation3d::SetShadingAspect(const Handle(Prs3d_ShadingAspect)& aShadingAspect) const {
  myStructure->SetShadingAspect(aShadingAspect);
}



//=======================================================================
//function : Compute
//purpose  : Methods for hidden parts...
//=======================================================================

Handle(Graphic3d_Structure) PrsMgr_Presentation3d::
Compute(const Handle(Graphic3d_DataStructureManager)& aProjector)
{
#ifdef DEB
  cout<<"passage in g = Compute(P) "<<endl;
#endif
  Handle(Prs3d_Presentation) g = new Prs3d_Presentation(Handle(PrsMgr_PresentationManager3d)::DownCast(PresentationManager())->StructureManager());
  myPresentableObject->Compute(Projector(aProjector),g);
  return g;
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void PrsMgr_Presentation3d::Compute(const Handle(Graphic3d_DataStructureManager)& aProjector,
				    const Handle(Graphic3d_Structure)& TheStructToFill)
{
#ifdef DEB
  cout<<"passage in Compute(P,Str)"<<endl;
#endif
  TheStructToFill->Clear();
  const Handle(Prs3d_Presentation)& P = *((Handle(Prs3d_Presentation)*) &TheStructToFill);
  myPresentableObject->Compute(Projector(aProjector),P);
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================
Handle(Graphic3d_Structure) PrsMgr_Presentation3d::
Compute(const Handle(Graphic3d_DataStructureManager)& aProjector,
	const Handle(Geom_Transformation)& TheTrsf)
{
#ifdef DEB
  cout<<"passage in G =  Compute(P,Trsf)"<<endl;
#endif
  Handle(Prs3d_Presentation) g = new Prs3d_Presentation(Handle(PrsMgr_PresentationManager3d)::DownCast(PresentationManager())->StructureManager());


  if(TheTrsf->Form()== gp_Translation){
#ifdef DEB
    cout<<"\tla Trsf est une translation"<<endl;
#endif
   
    myPresentableObject->Compute(Projector(aProjector),g);
    g->Transform(TheTrsf);
  }
  else{
    // waiting that something is done in gp_Trsf...rob
    Standard_Boolean good (Standard_True);
    for (Standard_Integer i=1;i<=3 && good ;i++){
      for (Standard_Integer j=1;j<=3 && good ;j++){
	if(i!=j){
	  if(Abs(TheTrsf->Value(i,j)) > Precision::Confusion())
	    good = Standard_False;
	}
      }
    }
    
    if(good){
#ifdef DEB
      cout<<"\t it is checked if Trsf is a Translation"<<endl;
#endif
    myPresentableObject->Compute(Projector(aProjector),g);
    g->Transform(TheTrsf);
      
    }
    else{
#ifdef DEB
      cout<<"\t Trsf is not only translation..."<<endl;
#endif
      myPresentableObject->Compute(Projector(aProjector),TheTrsf,g);
    }
  }
  return g;
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================
void PrsMgr_Presentation3d::Compute(const Handle(Graphic3d_DataStructureManager)& aProjector,
				    const Handle(Geom_Transformation)& TheTrsf,
				    const Handle(Graphic3d_Structure)& TheStructToFill)
{
  
#ifdef DEB
  cout<<"passage in Compute(P,Trsf,Str)"<<endl;
#endif

  Handle(Prs3d_Presentation) P = *((Handle(Prs3d_Presentation)*)&TheStructToFill);
 
#ifdef IMP040200	
  TheStructToFill->Clear();
  myPresentableObject->Compute(Projector(aProjector),TheTrsf,P);
#else	//Does not work properly, HLR seems deactivated for view rotation 
  if(TheTrsf->Form()== gp_Translation){
#ifdef DEB
    cout<<"\t Trsf is a translation"<<endl;
#endif
//    myPresentableObject->Compute(Projector(aProjector),P);
    P->Transform(TheTrsf);
  }
  else{
    //  waiting that something is done in gp_Trsf...rob
    Standard_Boolean good (Standard_True);
    for (Standard_Integer i=1;i<=3 && good ;i++){
      for (Standard_Integer j=1;j<=3 && good ;j++){
	if(i!=j){
	  if(Abs(TheTrsf->Value(i,j)) > Precision::Confusion())
	    good = Standard_False;
	}
      }
    }
    if(good && !TheStructToFill->IsEmpty()){
#ifdef DEB
      cout<<"\t it is checked if Trsf is a Translation"<<endl;
#endif
      
      P->Transform(TheTrsf);
    }
    else{
      TheStructToFill->Clear();
      
#ifdef DEB
      cout<<"\t Trsf is not only translation..."<<endl;
#endif
      
      myPresentableObject->Compute(Projector(aProjector),TheTrsf,P);
    }
  }
#endif
}






//=======================================================================
//function : Projector
//purpose  : 
//=======================================================================

Handle(Prs3d_Projector) PrsMgr_Presentation3d::Projector(const Handle(Graphic3d_DataStructureManager)& aProjector) {
  Visual3d_ViewOrientation VO = Handle(Visual3d_View)::DownCast(aProjector)->ViewOrientation();
  Standard_Real DX, DY, DZ,XAt, YAt , ZAt,XUp, YUp, ZUp;
  VO.ViewReferencePlane().Coord(DX, DY, DZ);
  VO.ViewReferencePoint().Coord(XAt,YAt,ZAt);
  VO.ViewReferenceUp().Coord(XUp, YUp, ZUp);
  Visual3d_ViewMapping VM =  Handle(Visual3d_View)::DownCast(aProjector)->ViewMapping();
  Standard_Boolean pers = (VM.Projection() == Visual3d_TOP_PERSPECTIVE);
    Standard_Real focale = 0.0 ;
  if (pers) {
    Standard_Real Xrp,Yrp,Zrp,ViewPlane,FrontPlane ;
    Graphic3d_Vertex Prp = VM.ProjectionReferencePoint() ;
    Prp.Coord(Xrp,Yrp,Zrp);
    FrontPlane = VM.FrontPlaneDistance() ;
    ViewPlane = VM.ViewPlaneDistance() ;
    focale = FrontPlane + Zrp - ViewPlane ;
  }
  Handle(Prs3d_Projector) Proj = new Prs3d_Projector(pers,focale,DX, DY, DZ,XAt, YAt , ZAt,XUp, YUp, ZUp);
  return Proj;

}

void PrsMgr_Presentation3d::Destroy () {
  if (!myStructure.IsNull())
    myStructure->Clear();
  myStructure.Nullify();
}
