// Created on: 2011-08-01
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2012 OPEN CASCADE SAS
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


#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include <OpenGl_Group.hxx>
#include <OpenGl_PrimitiveArray.hxx>
#include <OpenGl_Structure.hxx>
#include <OpenGl_Workspace.hxx>

// =======================================================================
// function : OpenGl_Group
// purpose  :
// =======================================================================
#ifndef HAVE_OPENCL
OpenGl_Group::OpenGl_Group()
#else
OpenGl_Group::OpenGl_Group (const OpenGl_Structure* theAncestorStructure)
#endif
: myAspectLine(NULL),
  myAspectFace(NULL),
  myAspectMarker(NULL),
  myAspectText(NULL),
  myFirst(NULL),
  myLast(NULL)
{
#ifdef HAVE_OPENCL
  myAncestorStructure = theAncestorStructure;
  myIsRaytracable = Standard_False;
  myModificationState = 0; // initial state
#endif
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
// function : SetAspectLine
// purpose  :
// =======================================================================
void OpenGl_Group::SetAspectLine (const CALL_DEF_CONTEXTLINE& theAspect,
                                  const Standard_Boolean theIsGlobal)
{
  if (theIsGlobal || myFirst == NULL)
  {
    if (myAspectLine == NULL)
    {
      myAspectLine = new OpenGl_AspectLine();
    }
    myAspectLine->SetAspect (theAspect);
  }
  else
  {
    OpenGl_AspectLine* anAspectLine = new OpenGl_AspectLine();
    anAspectLine->SetAspect (theAspect);
    AddElement (TelNil/*TelAspectLine*/, anAspectLine);
  }
}

// =======================================================================
// function : SetAspectFace
// purpose  :
// =======================================================================
void OpenGl_Group::SetAspectFace (const CALL_DEF_CONTEXTFILLAREA& theAspect,
                                  const Standard_Boolean          theIsGlobal)
{
  if (theIsGlobal || myFirst == NULL)
  {
    if (myAspectFace == NULL)
    {
      myAspectFace = new OpenGl_AspectFace();
    }
    myAspectFace->SetAspect (theAspect);
  }
  else
  {
    OpenGl_AspectFace* anAspectFace = new OpenGl_AspectFace();
    anAspectFace->SetAspect (theAspect);
    AddElement (TelNil/*TelAspectFace*/, anAspectFace);
  }

#ifdef HAVE_OPENCL
  if (myIsRaytracable)
  {
    myModificationState++;

    if (myAncestorStructure != NULL)
    {
      myAncestorStructure->UpdateStateWithAncestorStructures();
    }
  }
#endif
}

// =======================================================================
// function : SetAspectMarker
// purpose  :
// =======================================================================
void OpenGl_Group::SetAspectMarker (const CALL_DEF_CONTEXTMARKER& theAspect,
                                    const Standard_Boolean theIsGlobal)
{
  if (theIsGlobal || myFirst == NULL)
  {
    if (myAspectMarker == NULL)
    {
      myAspectMarker = new OpenGl_AspectMarker();
    }
    myAspectMarker->SetAspect (theAspect);
  }
  else
  {
    OpenGl_AspectMarker* anAspectMarker = new OpenGl_AspectMarker();
    anAspectMarker->SetAspect (theAspect);
    AddElement (TelNil/*TelAspectMarker*/, anAspectMarker);
  }
}

// =======================================================================
// function : SetAspectText
// purpose  :
// =======================================================================
void OpenGl_Group::SetAspectText (const CALL_DEF_CONTEXTTEXT& theAspect,
                                  const Standard_Boolean theIsGlobal)
{
  if (theIsGlobal || myFirst == NULL)
  {
    if (myAspectText == NULL)
    {
      myAspectText = new OpenGl_AspectText();
    }
    myAspectText->SetAspect (theAspect);
  }
  else
  {
    OpenGl_AspectText* anAspectText = new OpenGl_AspectText();
    anAspectText->SetAspect (theAspect);
    AddElement ( TelNil/*TelAspectText*/, anAspectText);
  }
}

// =======================================================================
// function : AddElement
// purpose  :
// =======================================================================
void OpenGl_Group::AddElement (const TelType theType, OpenGl_Element *theElem)
{
  OpenGl_ElementNode *aNode = new OpenGl_ElementNode();

  aNode->type = theType;
  aNode->elem = theElem;
  aNode->next = NULL;
  (myLast? myLast->next : myFirst) = aNode;
  myLast = aNode;

#ifdef HAVE_OPENCL
  if (OpenGl_Raytrace::IsRaytracedElement (aNode))
  {
    myModificationState++;
    myIsRaytracable = Standard_True;

    if (myAncestorStructure != NULL)
    {
      myAncestorStructure->UpdateStateWithAncestorStructures();
      myAncestorStructure->SetRaytracableWithAncestorStructures();
    }
  }
#endif
}

// =======================================================================
// function : Render
// purpose  :
// =======================================================================
void OpenGl_Group::Render (const Handle(OpenGl_Workspace)& theWorkspace) const
{
  // Is rendering in ADD or IMMEDIATE mode?
  const Standard_Boolean isImmediate = (theWorkspace->NamedStatus & (OPENGL_NS_ADD | OPENGL_NS_IMMEDIATE)) != 0;
  const Handle(OpenGl_RenderFilter)& aFilter = theWorkspace->GetRenderFilter();

  // Setup aspects
  const OpenGl_AspectLine*   aBackAspectLine   = theWorkspace->AspectLine   (Standard_False);
  const OpenGl_AspectFace*   aBackAspectFace   = theWorkspace->AspectFace   (Standard_False);
  const OpenGl_AspectMarker* aBackAspectMarker = theWorkspace->AspectMarker (Standard_False);
  const OpenGl_AspectText*   aBackAspectText   = theWorkspace->AspectText   (Standard_False);
  Standard_Boolean isLineSet   = myAspectLine   && myAspectLine->RenderFiltered (theWorkspace, aFilter);
  Standard_Boolean isFaceSet   = myAspectFace   && myAspectFace->RenderFiltered (theWorkspace, aFilter);
  Standard_Boolean isMarkerSet = myAspectMarker && myAspectMarker->RenderFiltered (theWorkspace, aFilter);
  Standard_Boolean isTextSet   = myAspectText   && myAspectText->RenderFiltered (theWorkspace, aFilter);

  // Render group elements
  for (OpenGl_ElementNode* aNodeIter = myFirst; aNodeIter != NULL; aNodeIter = aNodeIter->next)
  {
    switch (aNodeIter->type)
    {
      case TelMarker:
      case TelMarkerSet:
      case TelText:
      {
        glDisable (GL_LIGHTING);
        if (isImmediate)
        {
          glDepthMask (GL_FALSE);
        }

        aNodeIter->elem->RenderFiltered (theWorkspace, aFilter);
        break;
      }
      default:
      {
        aNodeIter->elem->RenderFiltered (theWorkspace, aFilter);
        break;
      }
    }
  }

  // Restore aspects
  if (isLineSet)
    theWorkspace->SetAspectLine (aBackAspectLine);
  if (isFaceSet)
    theWorkspace->SetAspectFace (aBackAspectFace);
  if (isMarkerSet)
    theWorkspace->SetAspectMarker (aBackAspectMarker);
  if (isTextSet)
    theWorkspace->SetAspectText (aBackAspectText);
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
    OpenGl_Element::Destroy (theGlCtx, myFirst->elem);
    delete myFirst;
    myFirst = aNext;
  }
  myLast = NULL;

  OpenGl_Element::Destroy (theGlCtx, myAspectLine);
  OpenGl_Element::Destroy (theGlCtx, myAspectFace);
  OpenGl_Element::Destroy (theGlCtx, myAspectMarker);
  OpenGl_Element::Destroy (theGlCtx, myAspectText);
}
