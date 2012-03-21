// Created on: 2007-07-07
// Created by: Alexander GRIGORIEV
// Copyright (c) 2007-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



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
                                        (const Handle(NIS_Drawer)& theDrawer,
                                         const Standard_Boolean    setUpdated)
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
  if (theDrawer->myIniId == 0)
    theDrawer->myIniId = myID;
  const Handle(NIS_Drawer)& aDrawer = aCtx->myDrawers.Added (theDrawer);
  if (myDrawer != aDrawer)
  {
    // Move the Object from the old drawer to the new one.
    if (myDrawer.IsNull() == Standard_False)
      myDrawer->removeObject(this, Standard_True);
    myDrawer = aDrawer;

    myDrawer->addObject(this, aCtx->myIsShareDrawList, Standard_True);
  }
  if (setUpdated)
    myDrawer->SetUpdated (NIS_Drawer::Draw_Normal,
                          NIS_Drawer::Draw_Top,
                          NIS_Drawer::Draw_Transparent,
                          NIS_Drawer::Draw_Hilighted);
  return aDrawer;
}

//=======================================================================
//function : SetTransparency
//purpose  : 
//=======================================================================

void NIS_InteractiveObject::SetTransparency (const Standard_Real theValue)
{
  Standard_Integer aValue =
    static_cast<Standard_Integer> (theValue * MaxTransparency);
  if (aValue != static_cast<Standard_Integer>(myTransparency))
  {
    if (aValue <= 0)
      myTransparency = 0;
    else if (aValue >= 1000)
      myTransparency = 1000u;
    else
      myTransparency = static_cast<unsigned int> (aValue);

    if (myDrawer.IsNull() == Standard_False && myID != 0) {
      const Handle(NIS_Drawer) aDrawer = DefaultDrawer(0L);
      aDrawer->Assign (GetDrawer());
      aDrawer->myTransparency = Transparency();
      SetDrawer (aDrawer, Standard_False);

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
//function : Clone
//purpose  : 
//=======================================================================

void NIS_InteractiveObject::Clone (const Handle_NCollection_BaseAllocator&,
                                   Handle_NIS_InteractiveObject& theDest) const
{
  if (theDest.IsNull() == Standard_False)
  {
    theDest->myID = 0;
    theDest->myDrawer = myDrawer;
    theDest->myDrawType = myDrawType;
    theDest->myBaseType = myBaseType;
    theDest->myIsHidden = myIsHidden;
    theDest->myIsDynHilighted = myIsDynHilighted;
    theDest->myIsUpdateBox = myIsUpdateBox;
    theDest->myTransparency = myTransparency;
    if (myIsUpdateBox == Standard_False)
      theDest->myBox = myBox;
    theDest->myAttributePtr = myAttributePtr;
  }
}

//=======================================================================
//function : CloneWithID
//purpose  : 
//=======================================================================

void NIS_InteractiveObject::CloneWithID
                        (const Handle_NCollection_BaseAllocator& theAlloc,
                         Handle_NIS_InteractiveObject&           theDest)
{
  Clone(theAlloc, theDest);
  theDest->myID = myID;
  myDrawer.Nullify();
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
//function : Intersect
//purpose  : 
//=======================================================================

Standard_Boolean NIS_InteractiveObject::Intersect
                     (const NCollection_List<gp_XY> &thePolygon,
                      const gp_Trsf                 &theTrf,
                      const Standard_Boolean         isFull) const
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
      if (myDrawType == NIS_Drawer::Draw_Hilighted)
      {
        aCtx->myMapObjects[NIS_Drawer::Draw_Hilighted].Remove(myID);
        aCtx->deselectObj (this, myID);
      }
    }
  }
}
