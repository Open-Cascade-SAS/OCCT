// Created on: 2011-08-01
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#include <OpenGl_Group.hxx>

#include <OpenGl_Flipper.hxx>
#include <OpenGl_PrimitiveArray.hxx>
#include <OpenGl_SceneGeometry.hxx>
#include <OpenGl_StencilTest.hxx>
#include <OpenGl_Structure.hxx>
#include <OpenGl_Text.hxx>
#include <OpenGl_Workspace.hxx>

#include <Graphic3d_ArrayOfPrimitives.hxx>
#include <Graphic3d_GroupDefinitionError.hxx>

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_Group, Graphic3d_Group)

//=================================================================================================

OpenGl_Group::OpenGl_Group(const occ::handle<Graphic3d_Structure>& theStruct)
    : Graphic3d_Group(theStruct),
      myAspects(nullptr),
      myFirst(nullptr),
      myLast(nullptr),
      myIsRaytracable(false)
{
  occ::handle<OpenGl_Structure> aStruct =
    occ::down_cast<OpenGl_Structure>(myStructure->CStructure());
  if (aStruct.IsNull())
  {
    throw Graphic3d_GroupDefinitionError("OpenGl_Group should be created by OpenGl_Structure!");
  }
}

//=================================================================================================

OpenGl_Group::~OpenGl_Group()
{
  Release(occ::handle<OpenGl_Context>());
}

//=================================================================================================

void OpenGl_Group::SetGroupPrimitivesAspect(const occ::handle<Graphic3d_Aspects>& theAspect)
{
  if (IsDeleted())
  {
    return;
  }

  if (myAspects == nullptr)
  {
    myAspects = new OpenGl_Aspects(theAspect);
  }
  else
  {
    myAspects->SetAspect(theAspect);
  }

  if (OpenGl_Structure* aStruct = myIsRaytracable ? GlStruct() : nullptr)
  {
    aStruct->UpdateStateIfRaytracable(false);
  }

  Update();
}

//=================================================================================================

void OpenGl_Group::SetPrimitivesAspect(const occ::handle<Graphic3d_Aspects>& theAspect)
{
  if (myAspects == nullptr)
  {
    SetGroupPrimitivesAspect(theAspect);
    return;
  }
  else if (IsDeleted())
  {
    return;
  }

  OpenGl_Aspects* anAspects = new OpenGl_Aspects(theAspect);
  AddElement(anAspects);
  Update();
}

//=================================================================================================

void OpenGl_Group::SynchronizeAspects()
{
  if (myAspects != nullptr)
  {
    myAspects->SynchronizeAspects();
    if (OpenGl_Structure* aStruct = myIsRaytracable ? GlStruct() : nullptr)
    {
      aStruct->UpdateStateIfRaytracable(false);
    }
  }
  for (OpenGl_ElementNode* aNode = myFirst; aNode != nullptr; aNode = aNode->next)
  {
    aNode->elem->SynchronizeAspects();
  }
}

//=================================================================================================

void OpenGl_Group::ReplaceAspects(
  const NCollection_DataMap<occ::handle<Graphic3d_Aspects>, occ::handle<Graphic3d_Aspects>>& theMap)
{
  if (theMap.IsEmpty())
  {
    return;
  }

  occ::handle<Graphic3d_Aspects> anAspect;
  if (myAspects != nullptr && theMap.Find(myAspects->Aspect(), anAspect))
  {
    myAspects->SetAspect(anAspect);
    if (OpenGl_Structure* aStruct = myIsRaytracable ? GlStruct() : nullptr)
    {
      aStruct->UpdateStateIfRaytracable(false);
    }
  }
  for (OpenGl_ElementNode* aNode = myFirst; aNode != nullptr; aNode = aNode->next)
  {
    OpenGl_Aspects* aGlAspect = dynamic_cast<OpenGl_Aspects*>(aNode->elem);
    if (aGlAspect != nullptr && theMap.Find(aGlAspect->Aspect(), anAspect))
    {
      aGlAspect->SetAspect(anAspect);
    }
  }
}

//=================================================================================================

void OpenGl_Group::AddPrimitiveArray(const Graphic3d_TypeOfPrimitiveArray      theType,
                                     const occ::handle<Graphic3d_IndexBuffer>& theIndices,
                                     const occ::handle<Graphic3d_Buffer>&      theAttribs,
                                     const occ::handle<Graphic3d_BoundBuffer>& theBounds,
                                     const bool                                theToEvalMinMax)
{
  if (IsDeleted() || theAttribs.IsNull())
  {
    return;
  }

  OpenGl_Structure*           aStruct = GlStruct();
  const OpenGl_GraphicDriver* aDriver = aStruct->GlDriver();

  OpenGl_PrimitiveArray* anArray =
    new OpenGl_PrimitiveArray(aDriver, theType, theIndices, theAttribs, theBounds);
  AddElement(anArray);

  Graphic3d_Group::AddPrimitiveArray(theType, theIndices, theAttribs, theBounds, theToEvalMinMax);
}

//=================================================================================================

void OpenGl_Group::AddText(const occ::handle<Graphic3d_Text>& theTextParams,
                           const bool                         theToEvalMinMax)
{
  if (IsDeleted())
  {
    return;
  }

  if (theTextParams->Height() < 2.0)
  {
    // TODO - this should be handled in different way (throw exception / take default text height
    // without modifying Graphic3d_Text / log warning, etc.)
    OpenGl_Structure* aStruct = GlStruct();
    theTextParams->SetHeight(aStruct->GlDriver()->DefaultTextHeight());
  }
  OpenGl_Text* aText = new OpenGl_Text(theTextParams);

  AddElement(aText);
  Graphic3d_Group::AddText(theTextParams, theToEvalMinMax);
}

//=================================================================================================

void OpenGl_Group::SetFlippingOptions(const bool theIsEnabled, const gp_Ax2& theRefPlane)
{
  OpenGl_Flipper* aFlipper = new OpenGl_Flipper(theRefPlane);
  aFlipper->SetOptions(theIsEnabled);
  AddElement(aFlipper);
}

//=================================================================================================

void OpenGl_Group::SetStencilTestOptions(const bool theIsEnabled)
{
  OpenGl_StencilTest* aStencilTest = new OpenGl_StencilTest();
  aStencilTest->SetOptions(theIsEnabled);
  AddElement(aStencilTest);
}

//=================================================================================================

void OpenGl_Group::AddElement(OpenGl_Element* theElem)
{
  OpenGl_ElementNode* aNode = new OpenGl_ElementNode();

  aNode->elem                       = theElem;
  aNode->next                       = nullptr;
  (myLast ? myLast->next : myFirst) = aNode;
  myLast                            = aNode;

  if (OpenGl_Raytrace::IsRaytracedElement(aNode) && !HasPersistence())
  {
    myIsRaytracable = true;

    OpenGl_Structure* aStruct = GlStruct();
    if (aStruct != nullptr)
    {
      aStruct->UpdateStateIfRaytracable(false);
    }
  }
}

//=================================================================================================

bool OpenGl_Group::renderFiltered(const occ::handle<OpenGl_Workspace>& theWorkspace,
                                  OpenGl_Element*                      theElement) const
{
  if (!theWorkspace->ShouldRender(theElement, this))
  {
    return false;
  }

  theElement->Render(theWorkspace);
  return true;
}

//=================================================================================================

void OpenGl_Group::Render(const occ::handle<OpenGl_Workspace>& theWorkspace) const
{
  // Setup aspects
  theWorkspace->SetAllowFaceCulling(
    myIsClosed && !theWorkspace->GetGlContext()->Clipping().IsClippingOrCappingOn());
  const OpenGl_Aspects* aBackAspects = theWorkspace->Aspects();
  const bool            isAspectSet  = myAspects != nullptr && renderFiltered(theWorkspace, myAspects);

  // Render group elements
  for (OpenGl_ElementNode* aNodeIter = myFirst; aNodeIter != nullptr; aNodeIter = aNodeIter->next)
  {
    renderFiltered(theWorkspace, aNodeIter->elem);
  }

  // Restore aspects
  if (isAspectSet)
    theWorkspace->SetAspects(aBackAspects);
}

//=================================================================================================

void OpenGl_Group::Clear(const bool theToUpdateStructureMgr)
{
  if (IsDeleted())
  {
    return;
  }

  OpenGl_Structure*                  aStruct = GlStruct();
  const occ::handle<OpenGl_Context>& aCtx    = aStruct->GlDriver()->GetSharedContext();

  Release(aCtx);
  Graphic3d_Group::Clear(theToUpdateStructureMgr);

  myIsRaytracable = false;
}

//=================================================================================================

void OpenGl_Group::Release(const occ::handle<OpenGl_Context>& theGlCtx)
{
  // Delete elements
  while (myFirst != nullptr)
  {
    OpenGl_ElementNode* aNext = myFirst->next;
    OpenGl_Element::Destroy(theGlCtx.get(), myFirst->elem);
    delete myFirst;
    myFirst = aNext;
  }
  myLast = nullptr;

  OpenGl_Element::Destroy(theGlCtx.get(), myAspects);
}

//=================================================================================================

void OpenGl_Group::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Graphic3d_Group)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myAspects)
  for (OpenGl_ElementNode* aNode = myFirst; aNode != nullptr; aNode = aNode->next)
  {
    OpenGl_Element* anElement = aNode->elem;
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, anElement)
  }
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIsRaytracable)
}
