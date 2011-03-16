// File:	AIS_InteractiveObject.cxx
// Created:	Wed Dec 18 14:44:37 1996
// Author:	Robert COUBLANC
//		<rob@robox.paris1.matra-dtv.fr>

// Modified :   22/03/04 ; SAN : OCC4895 High-level interface for controlling polygon offsets 

#define BUC60577        //GG_101099     Enable to compute correctly
//                      transparency with more than one object in the view.

#define GER61351	//GG_171199     Enable to set an object RGB color
//			instead a restricted object NameOfColor. 
//			Add SetCurrentFacingModel() method
//			

#define G003		//EUG/GG 260100 DEgenerate mode support
//			Add SetDegenerateModel() methods

#define IMP020200	//GG Add SetTransformation() method

#define IMP140200	//GG Add HasPresentation() and Presentation() methods
//			     Add SetAspect() method.

#define BUC60632	//GG 15/03/00 Add protection on SetDisplayMode()
//			method, compute only authorized presentation.

#define IMP220501	//GG CADPAK_V2 Update selection properly

#define OCC708          //SAV unsetting transformation correctly

#include <AIS_InteractiveObject.ixx>

#include <Aspect_PolygonOffsetMode.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_TextAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Root.hxx>
#include <PrsMgr_ModedPresentation.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <AIS_GraphicTool.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_Structure.hxx>


//=======================================================================
//function : AIS_InteractiveObject
//purpose  : 
//=======================================================================

AIS_InteractiveObject::
AIS_InteractiveObject(const PrsMgr_TypeOfPresentation3d aTypeOfPresentation3d):
SelectMgr_SelectableObject(aTypeOfPresentation3d),
myDrawer(new AIS_Drawer()),
myTransparency(0.),
myOwnColor(Quantity_NOC_WHITE),
myHilightMode(-1),
myOwnWidth(0.0),
myInfiniteState(Standard_False),
hasOwnColor(Standard_False),
hasOwnMaterial(Standard_False),
myRecomputeEveryPrs(Standard_True),
mySelPriority(-1),
myDisplayMode (-1),
mySelectionMode(0),
mystate(0)
{
  Handle (AIS_InteractiveContext) Bid;
  myCTXPtr = Bid.operator->();
#ifdef GER61351
  SetCurrentFacingModel();
#endif
}

//=======================================================================
//function : Redisplay
//purpose  : 
//=======================================================================

void AIS_InteractiveObject::Redisplay(const Standard_Boolean AllModes)
{
  Update(AllModes);
  UpdateSelection();
}

//=======================================================================
//function : Type
//purpose  : 
//=======================================================================

AIS_KindOfInteractive AIS_InteractiveObject::Type() const 
{return AIS_KOI_None;}

//=======================================================================
//function : Signature
//purpose  : 
//=======================================================================

Standard_Integer AIS_InteractiveObject::Signature() const 
{return -1;}

//=======================================================================
//function : RecomputeEveryPrs
//purpose  : 
//=======================================================================

Standard_Boolean  AIS_InteractiveObject::RecomputeEveryPrs() const 
{return myRecomputeEveryPrs;}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
Standard_Boolean AIS_InteractiveObject::HasInteractiveContext() const 
{
  Handle (AIS_InteractiveContext) aNull;
  return  (myCTXPtr != aNull.operator->());
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
Handle(AIS_InteractiveContext) AIS_InteractiveObject::GetContext() const 
{
  return myCTXPtr;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
void AIS_InteractiveObject::SetContext(const Handle(AIS_InteractiveContext)& aCtx)
{
  myCTXPtr = aCtx.operator->();
  if (myDrawer.IsNull()) {
    myDrawer = new AIS_Drawer;
#ifdef DEB
    cout << "AIS_InteractiveObject::SetContext DRAWER NUL!" << endl;
#endif
  }
  myDrawer->Link(aCtx->DefaultDrawer());
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
Standard_Boolean AIS_InteractiveObject::HasOwner() const 
{
  return (!myOwner.IsNull());
}



//=======================================================================
//function : 
//purpose  : 
//=======================================================================
void AIS_InteractiveObject::ClearOwner()
{
  myOwner.Nullify();
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
Standard_Boolean AIS_InteractiveObject::HasUsers() const 
{
  return (!myUsers.IsEmpty());
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================
void AIS_InteractiveObject::AddUser(const Handle(Standard_Transient)& aUser)
{
  myUsers.Append(aUser);
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
void AIS_InteractiveObject::ClearUsers()
{
  myUsers.Clear();
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================
void AIS_InteractiveObject::SetDisplayMode(const Standard_Integer aMode)
{
#ifdef BUC60632
  if( AcceptDisplayMode(aMode) )
#endif
    myDisplayMode = aMode;
}
  

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
void AIS_InteractiveObject::SetSelectionMode(const Standard_Integer aMode)
{
  mySelectionMode = aMode;
}



//=======================================================================
//function : 
//purpose  : 
//=======================================================================
#ifdef GER61351
void AIS_InteractiveObject::SetCurrentFacingModel(const Aspect_TypeOfFacingModel aModel) {
  myCurrentFacingModel = aModel;
}

//=======================================================================
//function : CurrentFacingModel
//purpose  : 
//=======================================================================

Aspect_TypeOfFacingModel AIS_InteractiveObject::CurrentFacingModel() const {
  return myCurrentFacingModel;
}
#endif

//=======================================================================
//function : SetColor
//purpose  : 
//=======================================================================

void AIS_InteractiveObject::SetColor(const Quantity_NameOfColor aColor)
#ifdef GER61351
{
  SetColor(Quantity_Color(aColor));
}

//=======================================================================
//function : SetColor
//purpose  : 
//=======================================================================

void AIS_InteractiveObject::SetColor(const Quantity_Color &aColor)
#endif
{
  myOwnColor = aColor;
  hasOwnColor = Standard_True;
}

//=======================================================================
//function : UnsetColor
//purpose  : 
//=======================================================================
void AIS_InteractiveObject::UnsetColor()
{
  hasOwnColor = Standard_False;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
void AIS_InteractiveObject::SetWidth(const Standard_Real aValue)
{
  myOwnWidth = aValue;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
void AIS_InteractiveObject::UnsetWidth()
{
  myOwnWidth = 0.;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================
//POP pour K4L
void AIS_InteractiveObject::SetMaterial(const Graphic3d_NameOfMaterial aName)
//void AIS_InteractiveObject::SetMaterial(const Graphic3d_NameOfPhysicalMaterial aName)
{
  if( HasColor() || IsTransparent() || HasMaterial() )
    {
      myDrawer->ShadingAspect()->SetMaterial(aName);
    }
  else 
    {
      myDrawer->SetShadingAspect(new Prs3d_ShadingAspect());
      
      myDrawer->ShadingAspect()->SetMaterial(aName);
#ifndef BUC60577	//???
      myDrawer->ShadingAspect()->SetColor(AIS_GraphicTool::GetInteriorColor(myDrawer->Link()));
#endif
    }
  myOwnMaterial  = aName;
  hasOwnMaterial = Standard_True;
}
//=======================================================================
//function : SetMaterial
//purpose  : 
//=======================================================================

void AIS_InteractiveObject::SetMaterial(const Graphic3d_MaterialAspect& aMat)
{
#ifdef BUC60577
  if( HasColor() || IsTransparent() || HasMaterial() )
#else
  if(hasOwnColor ||(myTransparency==0.0) || hasOwnMaterial )
#endif
    {
      myDrawer->ShadingAspect()->SetMaterial(aMat);
    }
  else 
    {
      myDrawer->SetShadingAspect(new Prs3d_ShadingAspect());
      myDrawer->ShadingAspect()->SetMaterial(aMat);
    }
  hasOwnMaterial = Standard_True;
  
}
//=======================================================================
//function : 
//purpose  : 
//=======================================================================
void AIS_InteractiveObject::UnsetMaterial()
{
#ifdef BUC60577
  if( !HasMaterial() ) return;
  if( HasColor() || IsTransparent()) {
    myDrawer->ShadingAspect()->SetMaterial(
		AIS_GraphicTool::GetMaterial(myDrawer->Link()));
    if( HasColor() ) SetColor(myOwnColor);
    if( IsTransparent() ) SetTransparency(myTransparency);
  }
#else
  if(!hasOwnMaterial) return;
  if(hasOwnColor ||(myTransparency==0.0))
    {
      myDrawer->ShadingAspect()->SetMaterial(AIS_GraphicTool::GetMaterial(myDrawer->Link()));
    }
#endif
  else{
    Handle(Prs3d_ShadingAspect) SA;
    myDrawer->SetShadingAspect(SA);
  }
  hasOwnMaterial = Standard_False;
}

//=======================================================================
//function : SetTransparency
//purpose  : 
//=======================================================================
void AIS_InteractiveObject::SetTransparency(const Standard_Real aValue)
{

#ifdef BUC60577                     // Back & Front material can be different !

  if(!HasColor() && !IsTransparent() && !HasMaterial() ) {
        myDrawer->SetShadingAspect(new Prs3d_ShadingAspect());
      if(!myDrawer->Link().IsNull())
        myDrawer->ShadingAspect()->SetMaterial(AIS_GraphicTool::GetMaterial(myDrawer->Link()));
  }
  Graphic3d_MaterialAspect FMat = myDrawer->ShadingAspect()->Aspect()->FrontMaterial();
  Graphic3d_MaterialAspect BMat = myDrawer->ShadingAspect()->Aspect()->BackMaterial();
  FMat.SetTransparency(aValue); BMat.SetTransparency(aValue);
  myDrawer->ShadingAspect()->Aspect()->SetFrontMaterial(FMat);
  myDrawer->ShadingAspect()->Aspect()->SetBackMaterial(BMat);
#else
  if(aValue<0.0 || aValue>1.0) return;
  
  if(aValue<=0.05) 
    {
      UnsetTransparency();
      return;
    }
  

  if(hasOwnColor || hasOwnMaterial || myTransparency> 0.0)
    {
      Graphic3d_MaterialAspect Mat = myDrawer->ShadingAspect()->Aspect()->FrontMaterial();
      Mat.SetTransparency(aValue);
      myDrawer->ShadingAspect()->Aspect()->SetFrontMaterial(Mat);
      myDrawer->ShadingAspect()->Aspect()->SetBackMaterial(Mat);
    }
  else
    {
      myDrawer->SetShadingAspect(new Prs3d_ShadingAspect());
      if(!myDrawer->Link().IsNull())
	myDrawer->ShadingAspect()->SetMaterial(AIS_GraphicTool::GetMaterial(myDrawer->Link()));
      Graphic3d_MaterialAspect Mat = myDrawer->ShadingAspect()->Aspect()->FrontMaterial();
      Mat.SetTransparency(aValue);
      myDrawer->ShadingAspect()->Aspect()->SetFrontMaterial(Mat);
      myDrawer->ShadingAspect()->Aspect()->SetBackMaterial(Mat);
    }
#endif
  myTransparency = aValue;
}

//=======================================================================
//function : UnsetTransparency
//purpose  : 
//=======================================================================
void AIS_InteractiveObject::UnsetTransparency()
{
#ifdef BUC60577                     // Back & Front material can be different !
    if(HasColor() || HasMaterial() )
    {
      Graphic3d_MaterialAspect FMat = myDrawer->ShadingAspect()->Aspect()->FrontMaterial();
      Graphic3d_MaterialAspect BMat = myDrawer->ShadingAspect()->Aspect()->BackMaterial();
      FMat.SetTransparency(0.); BMat.SetTransparency(0.);
      myDrawer->ShadingAspect()->Aspect()->SetFrontMaterial(FMat);
      myDrawer->ShadingAspect()->Aspect()->SetBackMaterial(BMat);
    }
#else
    if(hasOwnColor || hasOwnMaterial )
    {
      Graphic3d_MaterialAspect Mat = myDrawer->ShadingAspect()->Aspect()->FrontMaterial();
      Mat.SetTransparency(0.0);
//      myDrawer->ShadingAspect()->Aspect()->SetFrontMaterial(Mat);
//      myDrawer->ShadingAspect()->Aspect()->SetBackMaterial(Mat);
       myDrawer->ShadingAspect()->SetMaterial(Mat);
    }
#endif
  else{
    Handle (Prs3d_ShadingAspect) SA;
    myDrawer->SetShadingAspect(SA);
  }
  myTransparency =0.0;
}
//=======================================================================
//function : Transparency
//purpose  : 
//=======================================================================
Standard_Real AIS_InteractiveObject::Transparency() const 
{
  return (myTransparency<=0.05 ? 0 : myTransparency);
// Graphic3d_MaterialAspect Mat = myDrawer->ShadingAspect()->Aspect()->FrontMaterial();
// return Mat.Transparency();
}

//=======================================================================
//function : SetAttributes
//purpose  : 
//=======================================================================

void AIS_InteractiveObject::SetAttributes(const Handle(AIS_Drawer)& aDrawer)
{myDrawer = aDrawer;}


//=======================================================================
//function : UnsetAttributes
//purpose  : 
//=======================================================================
void AIS_InteractiveObject::UnsetAttributes()
{
  Handle(AIS_Drawer) dr = new AIS_Drawer();
  if(myDrawer->HasLink())
    dr->Link(myDrawer->Link());
  myDrawer       = dr;
  hasOwnColor    = Standard_False;
  hasOwnMaterial = Standard_False;
  myOwnWidth     = 0.0;
  myTransparency = 0.0;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
void AIS_InteractiveObject::MustRecomputePrs(const Standard_Integer ) const 
{}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
const TColStd_ListOfInteger& AIS_InteractiveObject::ListOfRecomputeModes() const 
{return myToRecomputeModes;}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
void AIS_InteractiveObject::SetRecomputeOk()
{myToRecomputeModes.Clear();}


//=======================================================================
//function : AcceptDisplayMode
//purpose  : 
//=======================================================================

Standard_Boolean  AIS_InteractiveObject::AcceptDisplayMode(const Standard_Integer ) const
{return Standard_True;}

//=======================================================================
//function : DefaultDisplayMode
//purpose  : 
//=======================================================================

Standard_Integer AIS_InteractiveObject::DefaultDisplayMode() const
{return 0;}


//=======================================================================
//function : SetInfiniteState
//purpose  : 
//=======================================================================
void AIS_InteractiveObject::SetInfiniteState(const Standard_Boolean aFlag)
{
  myInfiniteState = aFlag;
  Handle(Prs3d_Presentation) P;

  for(Standard_Integer i =1; i<=myPresentations.Length();i++){
    P = Handle(Prs3d_Presentation)::DownCast(myPresentations(i).Presentation());
    if(!P.IsNull())
      P->SetInfiniteState(myInfiniteState);}
}

#ifdef G003
//=======================================================================
//function : SetDegenerateModel
//purpose  : 
//=======================================================================
void AIS_InteractiveObject::SetDegenerateModel(
		const Aspect_TypeOfDegenerateModel aModel,
		const Quantity_Ratio aRatio ) {
  if( !HasColor() && !IsTransparent() && !HasMaterial() ) {
    myDrawer->SetShadingAspect(new Prs3d_ShadingAspect());
  }

  myDrawer->ShadingAspect()->Aspect()->SetDegenerateModel(aModel,aRatio);

  if(!GetContext().IsNull()){
    if( GetContext()->MainPrsMgr()->HasPresentation(this,1)){
      Handle(Prs3d_Presentation) P = 
	GetContext()->MainPrsMgr()->CastPresentation(this,1)->Presentation();
      Handle(Graphic3d_AspectFillArea3d) a4bis = 
				myDrawer->ShadingAspect()->Aspect();
      P->SetPrimitivesAspect(a4bis);
    }
  }
}

//=======================================================================
//function : DegenerateModel
//purpose  : 
//=======================================================================

Aspect_TypeOfDegenerateModel AIS_InteractiveObject::DegenerateModel( 
			Quantity_Ratio& aRatio) const
{
  return myDrawer->ShadingAspect()->Aspect()->DegenerateModel(aRatio);
}
#endif

#ifdef IMP020200
//=======================================================================
//function : SetTransformation
//purpose  : 
//=======================================================================
void AIS_InteractiveObject::SetTransformation(const Handle(Geom_Transformation)& aTrsf, const Standard_Boolean postConcatenate, const Standard_Boolean updateSelection) {

  if(!GetContext().IsNull()){
    const PrsMgr_Presentations& prs = Presentations(); 
    Handle(Prs3d_Presentation) P;
    Standard_Integer mode;
    myHasTransformation = Standard_True;
    for( Standard_Integer i=1 ; i<=prs.Length() ; i++ ) {
      mode = prs(i).Mode();
      P = GetContext()->MainPrsMgr()->CastPresentation(this,mode)->Presentation();
      if( postConcatenate ) P->Multiply(aTrsf);
      else     	            P->Transform(aTrsf);
      if( updateSelection ) {
#ifdef IMP220501
        myCTXPtr->ClearSelected(Standard_True);
	myCTXPtr->RecomputeSelectionOnly(this);
#else
        if( HasSelection(mode) ) {
          UpdateSelection(mode);      
        }
#endif
      }
    }
  }
}

//=======================================================================
//function : SetTransformation
//purpose  : 
//=======================================================================
void AIS_InteractiveObject::UnsetTransformation() {
#ifdef OCC708
  static Handle(Geom_Transformation) trsf = new Geom_Transformation( gp_Trsf() );
#else
Handle(Geom_Transformation) trsf;
#endif

    SetTransformation(trsf);	// Set identity transformation
    myHasTransformation = Standard_False;
}

//=======================================================================
//function : Transformation
//purpose  : 
//=======================================================================
Handle(Geom_Transformation) AIS_InteractiveObject::Transformation() {
Handle(Geom_Transformation) trsf; 

  if(!GetContext().IsNull() ) {
    const PrsMgr_Presentations& prs = Presentations(); 
    if( prs.Length() > 0 ) {
      Handle(Prs3d_Presentation) P = 
	  GetContext()->MainPrsMgr()->CastPresentation(this,1)->Presentation();
      trsf = P->Transformation();
    }
  }

  return trsf;
}

//=======================================================================
//function : HasTransformation
//purpose  : 
//=======================================================================
Standard_Boolean AIS_InteractiveObject::HasTransformation() const {

  return myHasTransformation;
}
#endif

#ifdef IMP140200
//=======================================================================
//function : HasPresentation
//purpose  : 
//=======================================================================
Standard_Boolean AIS_InteractiveObject::HasPresentation() const {

  if( !GetContext().IsNull() && 
	GetContext()->MainPrsMgr()->HasPresentation(this,myDisplayMode) ) {
    return Standard_True;
  }

  return Standard_False;
}

//=======================================================================
//function : Presentation
//purpose  : 
//=======================================================================
Handle(Prs3d_Presentation) AIS_InteractiveObject::Presentation() const {
Handle(Prs3d_Presentation) prs;

  if( HasPresentation() ) {
    prs = GetContext()->MainPrsMgr()->
		CastPresentation(this,myDisplayMode)->Presentation();
  }

  return prs;
}

//=======================================================================
//function : SetAspect 
//purpose  : 
//=======================================================================
void AIS_InteractiveObject::SetAspect(const Handle(Prs3d_BasicAspect)& anAspect,
				      const Standard_Boolean globalChange) {

  if( HasPresentation() ) {
    Handle(Prs3d_Presentation) prs = Presentation();
    { Handle(Prs3d_ShadingAspect) aspect =
			Handle(Prs3d_ShadingAspect)::DownCast(anAspect);
      if( !aspect.IsNull() ) {
        if( globalChange ) prs->SetPrimitivesAspect(aspect->Aspect());
        Prs3d_Root::CurrentGroup(prs)->SetGroupPrimitivesAspect(aspect->Aspect());
        return;
      }
    }
    { Handle(Prs3d_LineAspect) aspect =
			Handle(Prs3d_LineAspect)::DownCast(anAspect);
      if( !aspect.IsNull() ) {
        if( globalChange ) prs->SetPrimitivesAspect(aspect->Aspect());
        Prs3d_Root::CurrentGroup(prs)->SetGroupPrimitivesAspect(aspect->Aspect());
        return;
      }
    }
    { Handle(Prs3d_PointAspect) aspect =
			Handle(Prs3d_PointAspect)::DownCast(anAspect);
      if( !aspect.IsNull() ) {
        if( globalChange ) prs->SetPrimitivesAspect(aspect->Aspect());
        Prs3d_Root::CurrentGroup(prs)->SetGroupPrimitivesAspect(aspect->Aspect());
        return;
      }
    }
    { Handle(Prs3d_TextAspect) aspect =
			Handle(Prs3d_TextAspect)::DownCast(anAspect);
      if( !aspect.IsNull() ) {
        if( globalChange ) prs->SetPrimitivesAspect(aspect->Aspect());
        Prs3d_Root::CurrentGroup(prs)->SetGroupPrimitivesAspect(aspect->Aspect());
        return;
      }
    }
  }
}
#endif

// OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets

//=======================================================================
//function : SetPolygonOffsets 
//purpose  : 
//======================================================================= 
void AIS_InteractiveObject::SetPolygonOffsets(const Standard_Integer aMode,
                                              const Standard_Real    aFactor,
                                              const Standard_Real    aUnits) 
{
  if ( !HasPolygonOffsets() )
    myDrawer->SetShadingAspect(new Prs3d_ShadingAspect());

  myDrawer->ShadingAspect()->Aspect()->SetPolygonOffsets( aMode, aFactor, aUnits );

  // Modify existing presentations 
  Handle(Graphic3d_Structure) aStruct;
  for( Standard_Integer i = 1, n = myPresentations.Length(); i <= n; i++ ) {
    Handle(PrsMgr_Presentation3d) aPrs3d =
      Handle(PrsMgr_Presentation3d)::DownCast( myPresentations(i).Presentation() );
    if ( !aPrs3d.IsNull() ) {
      aStruct = Handle(Graphic3d_Structure)::DownCast( aPrs3d->Presentation() );
      if( !aStruct.IsNull() )
        aStruct->SetPrimitivesAspect( myDrawer->ShadingAspect()->Aspect() );
    }
  }
}


//=======================================================================
//function : HasPolygonOffsets 
//purpose  : 
//=======================================================================
Standard_Boolean AIS_InteractiveObject::HasPolygonOffsets() const
{
  return !( myDrawer->ShadingAspect().IsNull() || 
          ( !myDrawer->Link().IsNull() && 
          myDrawer->ShadingAspect() == myDrawer->Link()->ShadingAspect() ) );
}

//=======================================================================
//function : PolygonOffsets 
//purpose  : 
//=======================================================================
void AIS_InteractiveObject::PolygonOffsets(Standard_Integer& aMode,
                                           Standard_Real&    aFactor,
                                           Standard_Real&    aUnits) const 
{
  if( HasPolygonOffsets() )
    myDrawer->ShadingAspect()->Aspect()->PolygonOffsets( aMode, aFactor, aUnits );
}

void AIS_InteractiveObject::Fill(const Handle(PrsMgr_PresentationManager)& aPresentationManager,
                                 const Handle(PrsMgr_Presentation)& aPresentation,
                                 const Standard_Integer aMode)
{
  PrsMgr_PresentableObject::Fill(aPresentationManager, aPresentation, aMode);

  // Update polygon offsets for <aPresentation> using <myDrawer> data
  if ( !myDrawer->ShadingAspect().IsNull() )
  {
    Standard_Integer aMode1 = Aspect_POM_Fill;
    Standard_Real aFactor = 1., aUnits = 0.;
    myDrawer->ShadingAspect()->Aspect()->PolygonOffsets( aMode1, aFactor, aUnits );

    // Here we force this object to have default polygon offsets , if they are not 
    // turned on for this object explicitly
    if ( ( aMode1 & Aspect_POM_None ) == Aspect_POM_None )
    {
      aMode1 = Aspect_POM_Fill;
      aFactor = 1.;
      aUnits = 0.;
      myDrawer->ShadingAspect()->Aspect()->SetPolygonOffsets( aMode1, aFactor, aUnits );
    }
  }

  Handle(PrsMgr_Presentation3d) aPrs3d =
    Handle(PrsMgr_Presentation3d)::DownCast( aPresentation );
  if ( !aPrs3d.IsNull() ) {
    Handle(Graphic3d_Structure) aStruct = 
      Handle(Graphic3d_Structure)::DownCast( aPrs3d->Presentation() );
    if( !aStruct.IsNull() )
      aStruct->SetPrimitivesAspect( myDrawer->ShadingAspect()->Aspect() );
  }
}
// OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets 
