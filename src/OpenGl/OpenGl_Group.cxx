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

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_Group,Graphic3d_Group)

// =======================================================================
// function : OpenGl_Group
// purpose  :
// =======================================================================
OpenGl_Group::OpenGl_Group (const Handle(Graphic3d_Structure)& theStruct)
: Graphic3d_Group (theStruct),
  myAspects(NULL),
  myFirst(NULL),
  myLast(NULL),
  myIsRaytracable (Standard_False)
{
  Handle(OpenGl_Structure) aStruct = Handle(OpenGl_Structure)::DownCast (myStructure->CStructure());
  if (aStruct.IsNull())
  {
    throw Graphic3d_GroupDefinitionError("OpenGl_Group should be created by OpenGl_Structure!");
  }
}

// =======================================================================
// function : ~OpenGl_Group
// purpose  :
// =======================================================================
OpenGl_Group::~OpenGl_Group()
{
  Release (Handle(OpenGl_Context)());
}

// =======================================================================
// function : SetGroupPrimitivesAspect
// purpose  :
// =======================================================================
void OpenGl_Group::SetGroupPrimitivesAspect (const Handle(Graphic3d_Aspects)& theAspect)
{
  if (IsDeleted())
  {
    return;
  }

  if (myAspects == NULL)
  {
    myAspects = new OpenGl_Aspects (theAspect);
  }
  else
  {
    myAspects->SetAspect (theAspect);
  }

  if (OpenGl_Structure* aStruct = myIsRaytracable ? GlStruct() : NULL)
  {
    aStruct->UpdateStateIfRaytracable (Standard_False);
  }

  Update();
}

// =======================================================================
// function : SetPrimitivesAspect
// purpose  :
// =======================================================================
void OpenGl_Group::SetPrimitivesAspect (const Handle(Graphic3d_Aspects)& theAspect)
{
  if (myAspects == NULL)
  {
    SetGroupPrimitivesAspect (theAspect);
    return;
  }
  else if (IsDeleted())
  {
    return;
  }

  OpenGl_Aspects* anAspects = new OpenGl_Aspects (theAspect);
  AddElement (anAspects);
  Update();
}

// =======================================================================
// function : SynchronizeAspects
// purpose  :
// =======================================================================
void OpenGl_Group::SynchronizeAspects()
{
  if (myAspects != NULL)
  {
    myAspects->SynchronizeAspects();
    if (OpenGl_Structure* aStruct = myIsRaytracable ? GlStruct() : NULL)
    {
      aStruct->UpdateStateIfRaytracable (Standard_False);
    }
  }
  for (OpenGl_ElementNode* aNode = myFirst; aNode != NULL; aNode = aNode->next)
  {
    aNode->elem->SynchronizeAspects();
  }
}

// =======================================================================
// function : ReplaceAspects
// purpose  :
// =======================================================================
void OpenGl_Group::ReplaceAspects (const Graphic3d_MapOfAspectsToAspects& theMap)
{
  if (theMap.IsEmpty())
  {
    return;
  }

  Handle(Graphic3d_Aspects) anAspect;
  if (myAspects != NULL
   && theMap.Find (myAspects->Aspect(), anAspect))
  {
    myAspects->SetAspect (anAspect);
    if (OpenGl_Structure* aStruct = myIsRaytracable ? GlStruct() : NULL)
    {
      aStruct->UpdateStateIfRaytracable (Standard_False);
    }
  }
  for (OpenGl_ElementNode* aNode = myFirst; aNode != NULL; aNode = aNode->next)
  {
    OpenGl_Aspects* aGlAspect = dynamic_cast<OpenGl_Aspects*> (aNode->elem);
    if (aGlAspect != NULL
     && theMap.Find (aGlAspect->Aspect(), anAspect))
    {
      aGlAspect->SetAspect (anAspect);
    }
  }
}

// =======================================================================
// function : AddPrimitiveArray
// purpose  :
// =======================================================================
void OpenGl_Group::AddPrimitiveArray (const Graphic3d_TypeOfPrimitiveArray theType,
                                      const Handle(Graphic3d_IndexBuffer)& theIndices,
                                      const Handle(Graphic3d_Buffer)&      theAttribs,
                                      const Handle(Graphic3d_BoundBuffer)& theBounds,
                                      const Standard_Boolean               theToEvalMinMax)
{
  if (IsDeleted()
   || theAttribs.IsNull())
  {
    return;
  }

  OpenGl_Structure* aStruct = GlStruct();
  const OpenGl_GraphicDriver* aDriver = aStruct->GlDriver();

  OpenGl_PrimitiveArray* anArray = new OpenGl_PrimitiveArray (aDriver, theType, theIndices, theAttribs, theBounds);
  AddElement (anArray);

  Graphic3d_Group::AddPrimitiveArray (theType, theIndices, theAttribs, theBounds, theToEvalMinMax);
}

// =======================================================================
// function : AddText
// purpose  :
// =======================================================================
void OpenGl_Group::AddText (const Handle(Graphic3d_Text)& theTextParams,
                            const Standard_Boolean theToEvalMinMax)
{
  if (IsDeleted())
  {
    return;
  }

  if (theTextParams->Height() < 2.0)
  {
    // TODO - this should be handled in different way (throw exception / take default text height without modifying Graphic3d_Text / log warning, etc.)
    OpenGl_Structure* aStruct = GlStruct();
    theTextParams->SetHeight (aStruct->GlDriver()->DefaultTextHeight());
  }
  OpenGl_Text* aText = new OpenGl_Text (theTextParams);

  AddElement (aText);
  Graphic3d_Group::AddText (theTextParams, theToEvalMinMax);
}

// =======================================================================
// function : SetFlippingOptions
// purpose  :
// =======================================================================
void OpenGl_Group::SetFlippingOptions (const Standard_Boolean theIsEnabled,
                                       const gp_Ax2&          theRefPlane)
{
  OpenGl_Flipper* aFlipper = new OpenGl_Flipper (theRefPlane);
  aFlipper->SetOptions (theIsEnabled);
  AddElement (aFlipper);
}

// =======================================================================
// function : SetStencilTestOptions
// purpose  :
// =======================================================================
void OpenGl_Group::SetStencilTestOptions (const Standard_Boolean theIsEnabled)
{
  OpenGl_StencilTest* aStencilTest = new OpenGl_StencilTest();
  aStencilTest->SetOptions (theIsEnabled);
  AddElement (aStencilTest);
}

// =======================================================================
// function : AddElement
// purpose  :
// =======================================================================
void OpenGl_Group::AddElement (OpenGl_Element* theElem)
{
  OpenGl_ElementNode *aNode = new OpenGl_ElementNode();

  aNode->elem = theElem;
  aNode->next = NULL;
  (myLast? myLast->next : myFirst) = aNode;
  myLast = aNode;

  if (OpenGl_Raytrace::IsRaytracedElement (aNode) && !HasPersistence())
  {
    myIsRaytracable = Standard_True;

    OpenGl_Structure* aStruct = GlStruct();
    if (aStruct != NULL)
    {
      aStruct->UpdateStateIfRaytracable (Standard_False);
    }
  }
}

// =======================================================================
// function : renderFiltered
// purpose  :
// =======================================================================
bool OpenGl_Group::renderFiltered (const Handle(OpenGl_Workspace)& theWorkspace,
                                   OpenGl_Element* theElement) const
{
  if (!theWorkspace->ShouldRender (theElement, this))
  {
    return false;
  }

  theElement->Render (theWorkspace);
  return true;
}

// =======================================================================
// function : Render
// purpose  :
// =======================================================================
void OpenGl_Group::Render (const Handle(OpenGl_Workspace)& theWorkspace) const
{
  // Setup aspects
  theWorkspace->SetAllowFaceCulling (myIsClosed
                                 && !theWorkspace->GetGlContext()->Clipping().IsClippingOrCappingOn());
  const OpenGl_Aspects* aBackAspects = theWorkspace->Aspects();
  const bool isAspectSet = myAspects != NULL && renderFiltered (theWorkspace, myAspects);

  // Render group elements
  for (OpenGl_ElementNode* aNodeIter = myFirst; aNodeIter != NULL; aNodeIter = aNodeIter->next)
  {
    renderFiltered (theWorkspace, aNodeIter->elem);
  }

  // Restore aspects
  if (isAspectSet)
    theWorkspace->SetAspects (aBackAspects);
}

// =======================================================================
// function : Clear
// purpose  :
// =======================================================================
void OpenGl_Group::Clear (const Standard_Boolean theToUpdateStructureMgr)
{
  if (IsDeleted())
  {
    return;
  }

  OpenGl_Structure* aStruct = GlStruct();
  const Handle(OpenGl_Context)& aCtx = aStruct->GlDriver()->GetSharedContext();

  Release (aCtx);
  Graphic3d_Group::Clear (theToUpdateStructureMgr);

  myIsRaytracable = Standard_False;
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_Group::Release (const Handle(OpenGl_Context)& theGlCtx)
{
  // Delete elements
  while (myFirst != NULL)
  {
    OpenGl_ElementNode* aNext = myFirst->next;
    OpenGl_Element::Destroy (theGlCtx.get(), myFirst->elem);
    delete myFirst;
    myFirst = aNext;
  }
  myLast = NULL;

  OpenGl_Element::Destroy (theGlCtx.get(), myAspects);
}

// =======================================================================
// function : DumpJson
// purpose  :
// =======================================================================
void OpenGl_Group::DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)

  OCCT_DUMP_BASE_CLASS (theOStream, theDepth, Graphic3d_Group)

  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myAspects)
  for (OpenGl_ElementNode* aNode = myFirst; aNode != NULL; aNode = aNode->next)
  {
    OpenGl_Element* anElement = aNode->elem;
    OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, anElement)
  }
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myIsRaytracable)
}
