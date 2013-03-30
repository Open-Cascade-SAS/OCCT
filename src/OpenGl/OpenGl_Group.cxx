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

#include <OpenGl_Group.hxx>

#include <OpenGl_PrimitiveArray.hxx>
#include <OpenGl_Workspace.hxx>

/*----------------------------------------------------------------------*/

OpenGl_Group::OpenGl_Group ()
: myAspectLine(NULL),
  myAspectFace(NULL),
  myAspectMarker(NULL),
  myAspectText(NULL),
  myFirst(NULL), myLast(NULL)
{
}

OpenGl_Group::~OpenGl_Group()
{
  Release (Handle(OpenGl_Context)());
}

/*----------------------------------------------------------------------*/

void OpenGl_Group::SetAspectLine (const CALL_DEF_CONTEXTLINE& theContext,
                                  const Standard_Boolean theIsGlobal)
{
  if (theIsGlobal || myFirst == NULL)
  {
    if (myAspectLine == NULL)
      myAspectLine = new OpenGl_AspectLine();
    myAspectLine->SetContext (theContext);
  }
  else
  {
    OpenGl_AspectLine* anAspectLine = new OpenGl_AspectLine();
    anAspectLine->SetContext (theContext);
    AddElement (TelNil/*TelAspectLine*/, anAspectLine);
  }
}

/*----------------------------------------------------------------------*/

void OpenGl_Group::SetAspectFace (const Handle(OpenGl_Context)&   theCtx,
                                  const CALL_DEF_CONTEXTFILLAREA& theAspect,
                                  const Standard_Boolean          theIsGlobal)
{
  if (theIsGlobal || myFirst == NULL)
  {
    if (myAspectFace == NULL)
    {
      myAspectFace = new OpenGl_AspectFace();
    }
    myAspectFace->Init (theCtx, theAspect);
  }
  else
  {
    OpenGl_AspectFace* anAspectFace = new OpenGl_AspectFace();
    anAspectFace->Init (theCtx, theAspect);
    AddElement (TelNil/*TelAspectFace*/, anAspectFace);
  }
}

/*----------------------------------------------------------------------*/

void OpenGl_Group::SetAspectMarker (const CALL_DEF_CONTEXTMARKER& theContext,
                                    const Standard_Boolean theIsGlobal)
{
  if (theIsGlobal || myFirst == NULL)
  {
    if (myAspectMarker == NULL)
      myAspectMarker = new OpenGl_AspectMarker();
    myAspectMarker->SetContext (theContext);
  }
  else
  {
    OpenGl_AspectMarker* anAspectMarker = new OpenGl_AspectMarker();
    anAspectMarker->SetContext (theContext);
    AddElement (TelNil/*TelAspectMarker*/, anAspectMarker);
  }
}

/*----------------------------------------------------------------------*/

void OpenGl_Group::SetAspectText (const CALL_DEF_CONTEXTTEXT& theContext,
                                  const Standard_Boolean theIsGlobal)
{
  if (theIsGlobal || myFirst == NULL)
  {
    if (myAspectText == NULL)
      myAspectText = new OpenGl_AspectText();
    myAspectText->SetContext (theContext);
  }
  else
  {
    OpenGl_AspectText* anAspectText = new OpenGl_AspectText();
    anAspectText->SetContext (theContext);
    AddElement ( TelNil/*TelAspectText*/, anAspectText);
  }
}

/*----------------------------------------------------------------------*/

void OpenGl_Group::AddElement (const TelType AType, OpenGl_Element *AElem )
{
  OpenGl_ElementNode *node = new OpenGl_ElementNode();

  node->type = AType;
  node->elem = AElem;
  node->next = NULL;
  (myLast? myLast->next : myFirst) = node;
  myLast = node;
}

/*----------------------------------------------------------------------*/

void OpenGl_Group::Render (const Handle(OpenGl_Workspace)& theWorkspace) const
{
  // Is rendering in ADD or IMMEDIATE mode?
  const Standard_Boolean isImmediate = (theWorkspace->NamedStatus & (OPENGL_NS_ADD | OPENGL_NS_IMMEDIATE)) != 0;

  // Setup aspects
  const OpenGl_AspectLine*   aBackAspectLine   = theWorkspace->AspectLine   (Standard_False);
  const OpenGl_AspectFace*   aBackAspectFace   = theWorkspace->AspectFace   (Standard_False);
  const OpenGl_AspectMarker* aBackAspectMarker = theWorkspace->AspectMarker (Standard_False);
  const OpenGl_AspectText*   aBackAspectText   = theWorkspace->AspectText   (Standard_False);
  if (myAspectLine)
  {
    theWorkspace->SetAspectLine (myAspectLine);
  }
  if (myAspectFace)
  {
    theWorkspace->SetAspectFace (myAspectFace);
  }
  if (myAspectMarker)
  {
    theWorkspace->SetAspectMarker (myAspectMarker);
  }
  if (myAspectText)
  {
    theWorkspace->SetAspectText (myAspectText);
  }

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

        aNodeIter->elem->Render (theWorkspace);
        break;
      }
      default:
      {
        aNodeIter->elem->Render (theWorkspace);
        break;
      }
    }
  }

  // Restore aspects
  theWorkspace->SetAspectLine   (aBackAspectLine);
  theWorkspace->SetAspectFace   (aBackAspectFace);
  theWorkspace->SetAspectMarker (aBackAspectMarker);
  theWorkspace->SetAspectText   (aBackAspectText);
}

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
