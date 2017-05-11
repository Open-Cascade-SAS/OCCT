// Created on: 1996-12-18
// Created by: Robert COUBLANC
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <AIS_InteractiveObject.hxx>

#include <AIS_GraphicTool.hxx>
#include <AIS_InteractiveContext.hxx>
#include <Aspect_PolygonOffsetMode.hxx>
#include <Bnd_Box.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_AspectText3d.hxx>
#include <Graphic3d_BndBox4f.hxx>
#include <Graphic3d_CStructure.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_MaterialAspect.hxx>
#include <Graphic3d_Structure.hxx>
#include <Prs3d_BasicAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Root.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_TextAspect.hxx>
#include <PrsMgr_ModedPresentation.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_InteractiveObject,SelectMgr_SelectableObject)

//=======================================================================
//function : AIS_InteractiveObject
//purpose  : 
//=======================================================================
AIS_InteractiveObject::AIS_InteractiveObject (const PrsMgr_TypeOfPresentation3d aTypeOfPresentation3d)
: SelectMgr_SelectableObject (aTypeOfPresentation3d),
  myCTXPtr (NULL),
  myOwnWidth (0.0),
  myCurrentFacingModel (Aspect_TOFM_BOTH_SIDE),
  myInfiniteState (Standard_False),
  hasOwnColor (Standard_False),
  hasOwnMaterial (Standard_False),
  myRecomputeEveryPrs (Standard_True)
{
  SetCurrentFacingModel();
}

//=======================================================================
//function : Redisplay
//purpose  :
//=======================================================================
void AIS_InteractiveObject::Redisplay (const Standard_Boolean AllModes)
{
  if (myCTXPtr == NULL)
    return;

  myCTXPtr->Redisplay (this, Standard_False, AllModes);
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
Handle(AIS_InteractiveContext) AIS_InteractiveObject::GetContext() const 
{
  return myCTXPtr;
}

//=======================================================================
//function : SetContext
//purpose  :
//=======================================================================
void AIS_InteractiveObject::SetContext (const Handle(AIS_InteractiveContext)& theCtx)
{
  if (myCTXPtr == theCtx.get())
  {
    return;
  }

  myCTXPtr = theCtx.get();
  if (!theCtx.IsNull())
  {
    myDrawer->Link (theCtx->DefaultDrawer());
  }
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
//function : SetDisplayMode
//purpose  :
//=======================================================================
void AIS_InteractiveObject::SetDisplayMode (const Standard_Integer theMode)
{
  if (AcceptDisplayMode (theMode))
  {
    myDrawer->SetDisplayMode (theMode);
  }
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
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

//=======================================================================
//function : SetColor
//purpose  : 
//=======================================================================

void AIS_InteractiveObject::SetColor(const Quantity_Color& theColor)
{
  myDrawer->SetColor (theColor);
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
//function : Material
//purpose  :
//=======================================================================
Graphic3d_NameOfMaterial AIS_InteractiveObject::Material() const
{
  return myDrawer->ShadingAspect()->Material().Name();
}

//=======================================================================
//function : SetMaterial
//purpose  :
//=======================================================================
void AIS_InteractiveObject::SetMaterial (const Graphic3d_MaterialAspect& theMaterial)
{
  if (!myDrawer->HasOwnShadingAspect())
  {
    myDrawer->SetShadingAspect (new Prs3d_ShadingAspect());
    if (myDrawer->HasLink())
    {
      *myDrawer->ShadingAspect()->Aspect() = *myDrawer->Link()->ShadingAspect()->Aspect();
    }
  }

  myDrawer->ShadingAspect()->SetMaterial (theMaterial);
  hasOwnMaterial = Standard_True;
}

//=======================================================================
//function : UnsetMaterial
//purpose  :
//=======================================================================
void AIS_InteractiveObject::UnsetMaterial()
{
  if (!HasMaterial())
  {
    return;
  }

  if (HasColor() || IsTransparent())
  {
    if(myDrawer->HasLink())
    {
      myDrawer->ShadingAspect()->SetMaterial (AIS_GraphicTool::GetMaterial (myDrawer->Link()));
    }

    if (HasColor())
    {
      SetColor (myDrawer->Color());
    }

    if (IsTransparent())
    {
      SetTransparency (myDrawer->Transparency());
    }
  }
  else
  {
    Handle(Prs3d_ShadingAspect) anAspect;
    myDrawer->SetShadingAspect (anAspect);
  }

  hasOwnMaterial = Standard_False;
}

//=======================================================================
//function : SetTransparency
//purpose  :
//=======================================================================
void AIS_InteractiveObject::SetTransparency (const Standard_Real theValue)
{
  if (!myDrawer->HasOwnShadingAspect())
  {
    myDrawer->SetShadingAspect(new Prs3d_ShadingAspect());
    if(myDrawer->HasLink())
      myDrawer->ShadingAspect()->SetMaterial(AIS_GraphicTool::GetMaterial(myDrawer->Link()));
  }

  myDrawer->ShadingAspect()->Aspect()->ChangeFrontMaterial().SetTransparency (Standard_ShortReal(theValue));
  myDrawer->ShadingAspect()->Aspect()->ChangeBackMaterial() .SetTransparency (Standard_ShortReal(theValue));
  myDrawer->SetTransparency (Standard_ShortReal(theValue));
}

//=======================================================================
//function : UnsetTransparency
//purpose  : 
//=======================================================================
void AIS_InteractiveObject::UnsetTransparency()
{
  if(HasColor() || HasMaterial() )
  {
    myDrawer->ShadingAspect()->Aspect()->ChangeFrontMaterial().SetTransparency (0.0f);
    myDrawer->ShadingAspect()->Aspect()->ChangeBackMaterial() .SetTransparency (0.0f);
  }
  else{
    Handle (Prs3d_ShadingAspect) SA;
    myDrawer->SetShadingAspect(SA);
  }
  myDrawer->SetTransparency (0.0f);
}
//=======================================================================
//function : Transparency
//purpose  : 
//=======================================================================
Standard_Real AIS_InteractiveObject::Transparency() const 
{
  return (myDrawer->Transparency() <= 0.005f ? 0.0 : myDrawer->Transparency());
// Graphic3d_MaterialAspect Mat = myDrawer->ShadingAspect()->Aspect()->FrontMaterial();
// return Mat.Transparency();
}

//=======================================================================
//function : UnsetAttributes
//purpose  : 
//=======================================================================
void AIS_InteractiveObject::UnsetAttributes()
{
  SelectMgr_SelectableObject::UnsetAttributes();

  hasOwnColor    = Standard_False;
  hasOwnMaterial = Standard_False;
  myOwnWidth     = 0.0;
  myDrawer->SetTransparency (0.0f);
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

  for(Standard_Integer i =1; i<=myPresentations.Length();i++)
  {
    P = myPresentations(i).Presentation()->Presentation();
    if(!P.IsNull())
      P->SetInfiniteState(myInfiniteState);
  }
}

//=======================================================================
//function : HasPresentation
//purpose  :
//=======================================================================
Standard_Boolean AIS_InteractiveObject::HasPresentation() const
{
  return HasInteractiveContext()
      && myCTXPtr->MainPrsMgr()->HasPresentation (this, myDrawer->DisplayMode());
}

//=======================================================================
//function : Presentation
//purpose  :
//=======================================================================
Handle(Prs3d_Presentation) AIS_InteractiveObject::Presentation() const
{
  if (!HasInteractiveContext())
  {
    return Handle(Prs3d_Presentation)();
  }

  Handle(PrsMgr_Presentation) aPrs = myCTXPtr->MainPrsMgr()->Presentation (this, myDrawer->DisplayMode(), false);
  return !aPrs.IsNull()
       ? aPrs->Presentation()
       : Handle(Prs3d_Presentation)();
}

//=======================================================================
//function : SetAspect 
//purpose  : 
//=======================================================================
void AIS_InteractiveObject::SetAspect(const Handle(Prs3d_BasicAspect)& anAspect)
{

  if( HasPresentation() ) {
    Handle(Prs3d_Presentation) prs = Presentation();
    { Handle(Prs3d_ShadingAspect) aspect =
			Handle(Prs3d_ShadingAspect)::DownCast(anAspect);
      if( !aspect.IsNull() ) {
        Prs3d_Root::CurrentGroup(prs)->SetGroupPrimitivesAspect(aspect->Aspect());
        return;
      }
    }
    { Handle(Prs3d_LineAspect) aspect =
			Handle(Prs3d_LineAspect)::DownCast(anAspect);
      if( !aspect.IsNull() ) {
        Prs3d_Root::CurrentGroup(prs)->SetGroupPrimitivesAspect(aspect->Aspect());
        return;
      }
    }
    { Handle(Prs3d_PointAspect) aspect =
			Handle(Prs3d_PointAspect)::DownCast(anAspect);
      if( !aspect.IsNull() ) {
        Prs3d_Root::CurrentGroup(prs)->SetGroupPrimitivesAspect(aspect->Aspect());
        return;
      }
    }
    { Handle(Prs3d_TextAspect) aspect =
			Handle(Prs3d_TextAspect)::DownCast(anAspect);
      if( !aspect.IsNull() ) {
        Prs3d_Root::CurrentGroup(prs)->SetGroupPrimitivesAspect(aspect->Aspect());
        return;
      }
    }
  }
}

//=======================================================================
//function : SetPolygonOffsets 
//purpose  : 
//======================================================================= 
void AIS_InteractiveObject::SetPolygonOffsets(const Standard_Integer    aMode,
                                              const Standard_ShortReal  aFactor,
                                              const Standard_ShortReal  aUnits) 
{
  if ( !HasPolygonOffsets() )
    myDrawer->SetShadingAspect(new Prs3d_ShadingAspect());

  myDrawer->ShadingAspect()->Aspect()->SetPolygonOffsets( aMode, aFactor, aUnits );

  // Modify existing presentations 
  for (Standard_Integer aPrsIter = 1, n = myPresentations.Length(); aPrsIter <= n; ++aPrsIter)
  {
    const Handle(PrsMgr_Presentation)& aPrs3d = myPresentations (aPrsIter).Presentation();
    if ( !aPrs3d.IsNull() ) {
      const Handle(Graphic3d_Structure)& aStruct = aPrs3d->Presentation();
      if( !aStruct.IsNull() ) {
        // Workaround for issue 23115: Need to update also groups, because their
        // face aspect ALWAYS overrides the structure's.
        const Graphic3d_SequenceOfGroup& aGroups = aStruct->Groups();
        for (Graphic3d_SequenceOfGroup::Iterator aGroupIter (aGroups); aGroupIter.More(); aGroupIter.Next())
        {
          Handle(Graphic3d_Group)& aGrp = aGroupIter.ChangeValue();
          if (aGrp.IsNull()
          || !aGrp->IsGroupPrimitivesAspectSet (Graphic3d_ASPECT_FILL_AREA))
          {
            continue;
          }

          Handle(Graphic3d_AspectFillArea3d) aFaceAsp = aGrp->FillAreaAspect();
          aFaceAsp->SetPolygonOffsets(aMode, aFactor, aUnits);
          aGrp->SetGroupPrimitivesAspect(aFaceAsp);
        }
      }
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
          ( myDrawer->HasLink() &&
          myDrawer->ShadingAspect() == myDrawer->Link()->ShadingAspect() ) );
}

//=======================================================================
//function : PolygonOffsets 
//purpose  : 
//=======================================================================
void AIS_InteractiveObject::PolygonOffsets(Standard_Integer&    aMode,
                                           Standard_ShortReal&  aFactor,
                                           Standard_ShortReal&  aUnits) const 
{
  if( HasPolygonOffsets() )
    myDrawer->ShadingAspect()->Aspect()->PolygonOffsets( aMode, aFactor, aUnits );
}

//=======================================================================
//function : BoundingBox
//purpose  : Returns bounding box of object correspondingly to its
//           current display mode
//=======================================================================
void AIS_InteractiveObject::BoundingBox (Bnd_Box& theBndBox)
{
  if (myDrawer->DisplayMode() == -1)
  {
    if (!myPresentations.IsEmpty())
    {
      const Handle(PrsMgr_Presentation)& aPrs3d = myPresentations.First().Presentation();
      const Handle(Graphic3d_Structure)& aStruct = aPrs3d->Presentation();
      const Graphic3d_BndBox3d& aBndBox = aStruct->CStructure()->BoundingBox();

      if (!aBndBox.IsValid())
      {
        theBndBox.SetVoid();
        return;
      }

      theBndBox.Update (aBndBox.CornerMin().x(), aBndBox.CornerMin().y(), aBndBox.CornerMin().z(),
                        aBndBox.CornerMax().x(), aBndBox.CornerMax().y(), aBndBox.CornerMax().z());
      return;
    }
    else
    {
      for (PrsMgr_ListOfPresentableObjectsIter aPrsIter (Children()); aPrsIter.More(); aPrsIter.Next())
      {
        Handle(AIS_InteractiveObject) aChild (Handle(AIS_InteractiveObject)::DownCast (aPrsIter.Value()));
        if (aChild.IsNull())
        {
          continue;
        }
        Bnd_Box aBox;
        aChild->BoundingBox (aBox);
        theBndBox.Add (aBox);
      }
      return;
    }
  }
  else
  {
    for (Standard_Integer aPrsIter = 1; aPrsIter <= myPresentations.Length(); ++aPrsIter)
    {
      if (myPresentations (aPrsIter).Mode() == myDrawer->DisplayMode())
      {
        const Handle(PrsMgr_Presentation)& aPrs3d = myPresentations (aPrsIter).Presentation();
        const Handle(Graphic3d_Structure)& aStruct = aPrs3d->Presentation();
        const Graphic3d_BndBox3d& aBndBox = aStruct->CStructure()->BoundingBox();

        if (!aBndBox.IsValid())
        {
          theBndBox.SetVoid();
          return;
        }

        theBndBox.Update (aBndBox.CornerMin().x(), aBndBox.CornerMin().y(), aBndBox.CornerMin().z(),
                          aBndBox.CornerMax().x(), aBndBox.CornerMax().y(), aBndBox.CornerMax().z());
        return;
      }
    }
  }
}

//=======================================================================
//function : SetIsoOnTriangulation
//purpose  : Enables or disables isoline on triangulation building
//=======================================================================
void AIS_InteractiveObject::SetIsoOnTriangulation (const Standard_Boolean theIsEnabled)
{
  myDrawer->SetIsoOnTriangulation (theIsEnabled);
}

//=======================================================================
//function : SynchronizeAspects
//purpose  :
//=======================================================================
void AIS_InteractiveObject::SynchronizeAspects()
{
  for (PrsMgr_Presentations::Iterator aPrsIter (myPresentations); aPrsIter.More(); aPrsIter.Next())
  {
    const Handle(PrsMgr_Presentation)& aPrs3d = aPrsIter.ChangeValue().Presentation();
    if (aPrs3d.IsNull()
     || aPrs3d->Presentation().IsNull())
    {
      continue;
    }

    for (Graphic3d_SequenceOfGroup::Iterator aGroupIter (aPrs3d->Presentation()->Groups()); aGroupIter.More(); aGroupIter.Next())
    {
      Handle(Graphic3d_Group)& aGrp = aGroupIter.ChangeValue();
      if (aGrp.IsNull())
      {
        continue;
      }

      Handle(Graphic3d_AspectLine3d)     aLineAspect   = aGrp->LineAspect();
      Handle(Graphic3d_AspectFillArea3d) aFaceAspect   = aGrp->FillAreaAspect();
      Handle(Graphic3d_AspectMarker3d)   aMarkerAspect = aGrp->MarkerAspect();
      Handle(Graphic3d_AspectText3d)     aTextAspect   = aGrp->TextAspect();
      if (!aLineAspect.IsNull())
      {
        aGrp->SetGroupPrimitivesAspect (aLineAspect);
      }
      if (!aFaceAspect.IsNull())
      {
        aGrp->SetGroupPrimitivesAspect (aFaceAspect);
      }
      if (!aMarkerAspect.IsNull())
      {
        aGrp->SetGroupPrimitivesAspect (aMarkerAspect);
      }
      if (!aTextAspect.IsNull())
      {
        aGrp->SetGroupPrimitivesAspect (aTextAspect);
      }
    }
  }
}
