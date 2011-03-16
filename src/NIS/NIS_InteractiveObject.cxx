// File:      NIS_InteractiveObject.cxx
// Created:   07.07.07 11:24
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2007


#include <NIS_InteractiveObject.hxx>
#include <NIS_InteractiveContext.hxx>
#include <Standard_NullValue.hxx>

IMPLEMENT_STANDARD_HANDLE  (NIS_InteractiveObject, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT (NIS_InteractiveObject, Standard_Transient)

//=======================================================================
//function : ~NIS_InteractiveObject()
//purpose  : Destructor
//=======================================================================

NIS_InteractiveObject::~NIS_InteractiveObject (  )
{
  if (myID != 0 && myDrawer.IsNull() == Standard_False)
    myDrawer->removeObject( this, Standard_False);
}

//=======================================================================
//function : SetDrawer
//purpose  : 
//=======================================================================

const Handle_NIS_Drawer& NIS_InteractiveObject::SetDrawer
                                        (const Handle(NIS_Drawer)& theDrawer)
{
  NIS_InteractiveContext * aCtx = theDrawer->GetContext();
  if (myDrawer.IsNull() == Standard_False)
    if (aCtx == 0L)
    {
      aCtx = myDrawer->GetContext();
      theDrawer->myCtx = aCtx;
    }
    else
    {
      Standard_NullValue_Raise_if
        (myDrawer->GetContext() != 0L && aCtx != myDrawer->GetContext(),
         "NIS_InteractiveObject::SetDrawer: different drawer context");
    }

  Standard_NullValue_Raise_if
    (aCtx == 0L, "NIS_InteractiveObject::SetDrawer: NULL drawer context");
  // Add (if necessary) the new drawer to the Context
  const Handle(NIS_Drawer)& aDrawer = aCtx->myDrawers.Added (theDrawer);
  if (myDrawer != aDrawer)
  {
    // Move the Object from the old drawer to the new one.
    if (myDrawer.IsNull() == Standard_False)
      myDrawer->removeObject(this, Standard_True);
    myDrawer = aDrawer;

    myDrawer->addObject(this, Standard_True);    
  }
  return aDrawer;
}

//=======================================================================
//function : SetTransparency
//purpose  : 
//=======================================================================

void NIS_InteractiveObject::SetTransparency (const Standard_Real theValue)
{
  if (fabs(theValue - myTransparency) > 0.001) {
    if (theValue > 0.001)
      myTransparency = static_cast<const Standard_ShortReal> (theValue);
    else
      myTransparency = 0.f;

    if (myDrawer.IsNull() == Standard_False && myID != 0) {
      NIS_InteractiveContext * aCtx = myDrawer->GetContext();
      Standard_NullValue_Raise_if
        (aCtx == 0L, "NIS_InteractiveObject::SetTransparency: "
                     "NULL drawer context");
      if (IsTransparent()) {
        if (myDrawType == NIS_Drawer::Draw_Normal) {
          aCtx->myMapObjects[NIS_Drawer::Draw_Transparent].Add(myID);
          aCtx->myMapObjects[NIS_Drawer::Draw_Normal].Remove(myID);
          myDrawType = NIS_Drawer::Draw_Transparent;
        }
        myDrawer->SetUpdated (NIS_Drawer::Draw_Normal);
      } else {
        if (myDrawType == NIS_Drawer::Draw_Transparent) {
          aCtx->myMapObjects[NIS_Drawer::Draw_Normal].Add(myID);
          aCtx->myMapObjects[NIS_Drawer::Draw_Transparent].Remove(myID);
          myDrawType = NIS_Drawer::Draw_Normal;
        }
        myDrawer->SetUpdated (NIS_Drawer::Draw_Transparent);
      }
      setDrawerUpdate();
    }
  }
}

//=======================================================================
//function : GetBox
//purpose  : 
//=======================================================================

const Bnd_B3f& NIS_InteractiveObject::GetBox ()
{
  if (myIsUpdateBox)
  {
    myIsUpdateBox = Standard_False;
    computeBox();
  }
  return myBox;
}

//=======================================================================
//function : Intersect
//purpose  : 
//=======================================================================

Standard_Boolean NIS_InteractiveObject::Intersect (const Bnd_B3f&,
                                                   const gp_Trsf&,
                                                   const Standard_Boolean) const
{
  return Standard_True;
}

//=======================================================================
//function : IsSelectable
//purpose  : Query if the Object is selectable.
//=======================================================================

Standard_Boolean NIS_InteractiveObject::IsSelectable () const
{
  Standard_Boolean aResult(Standard_False);
  if (myDrawer.IsNull() == Standard_False)
    aResult = myDrawer->GetContext()->IsSelectable (myID);
  return aResult;
}

//=======================================================================
//function : SetSelectable
//purpose  : Set or change the selectable state of the Object.
//=======================================================================

void NIS_InteractiveObject::SetSelectable (const Standard_Boolean isSel) const
{
  if (myDrawer.IsNull() == Standard_False) {
    NIS_InteractiveContext * aCtx = myDrawer->GetContext();
    if (isSel)
      aCtx->myMapNonSelectableObjects.Remove (myID);
    else {
      aCtx->myMapNonSelectableObjects.Add (myID);
      if (aCtx->myMapObjects[NIS_Drawer::Draw_Hilighted].Remove(myID))
      {
        if (IsTransparent()) {
          aCtx->myMapObjects[NIS_Drawer::Draw_Transparent].Add(myID);
          myDrawer->SetUpdated(NIS_Drawer::Draw_Transparent);
        } else {
          aCtx->myMapObjects[NIS_Drawer::Draw_Normal].Add(myID);
          myDrawer->SetUpdated(NIS_Drawer::Draw_Normal);
        }
        myDrawer->SetUpdated(NIS_Drawer::Draw_Hilighted);
      }
    }
  }
}
