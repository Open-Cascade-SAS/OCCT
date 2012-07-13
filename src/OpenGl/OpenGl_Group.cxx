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

#include <OpenGl_TextureBox.hxx>
#include <OpenGl_PrimitiveArray.hxx>

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

void OpenGl_Group::SetAspectFace (const CALL_DEF_CONTEXTFILLAREA& theContext,
                                  const Standard_Boolean theIsGlobal)
{
  if (theIsGlobal || myFirst == NULL)
  {
    if (myAspectFace == NULL)
      myAspectFace = new OpenGl_AspectFace();
    myAspectFace->SetContext (theContext);
  }
  else
  {
    OpenGl_AspectFace* anAspectFace = new OpenGl_AspectFace();
    anAspectFace->SetContext (theContext);
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

void OpenGl_Group::RemovePrimitiveArray (const Handle(OpenGl_Context)& theGlCtx,
                                         CALL_DEF_PARRAY*              thePArray)
{
  OpenGl_ElementNode *prevnode = NULL, *node = myFirst;
  while (node != NULL)
  {
    if (node->type == TelParray)
    {
      CALL_DEF_PARRAY* aCurPArray = ((const OpenGl_PrimitiveArray* )node->elem)->PArray();

      // validate for correct pointer
      if (aCurPArray->num_bounds  == thePArray->num_bounds  && 
          aCurPArray->num_edges   == thePArray->num_edges   &&
          aCurPArray->num_vertexs == thePArray->num_vertexs &&
          aCurPArray->type        == thePArray->type)
      {
        (prevnode ? prevnode->next : myFirst) = node->next;
        if (!myFirst) myLast = NULL;
        OpenGl_Element::Destroy (theGlCtx, node->elem);
        delete node;
        break;
      }
    }
    prevnode = node;
    node = node->next;
  }
}

/*----------------------------------------------------------------------*/

void OpenGl_Group::Render (const Handle(OpenGl_Workspace) &AWorkspace) const
{
  // Is rendering in ADD or IMMEDIATE mode?
  const Standard_Boolean isImmediate = (AWorkspace->NamedStatus & (OPENGL_NS_ADD | OPENGL_NS_IMMEDIATE)) != 0;

  // Setup aspects
  const OpenGl_AspectLine *aspect_line = AWorkspace->AspectLine(Standard_False);
  const OpenGl_AspectFace *aspect_face = AWorkspace->AspectFace(Standard_False);
  const OpenGl_AspectMarker *aspect_marker = AWorkspace->AspectMarker(Standard_False);
  const OpenGl_AspectText *aspect_text = AWorkspace->AspectText(Standard_False);
  if (myAspectLine)
    AWorkspace->SetAspectLine(myAspectLine);
  if (myAspectFace)
    AWorkspace->SetAspectFace(myAspectFace);
  if (myAspectMarker)
    AWorkspace->SetAspectMarker(myAspectMarker);
  if (myAspectText)
    AWorkspace->SetAspectText(myAspectText);

  // Render group elements
  OpenGl_ElementNode *node = myFirst;
  while (node)
  {
    switch (node->type)
    {
      case TelPolyline:
      case TelMarker:
      case TelMarkerSet:
      case TelText:
      {
        glDisable(GL_LIGHTING);

        if (isImmediate)
        {
          glDepthMask(GL_FALSE);
        }
        else if ( (AWorkspace->NamedStatus & OPENGL_NS_ANIMATION) != 0 &&
                  (AWorkspace->NamedStatus & OPENGL_NS_WIREFRAME) == 0 &&
                   AWorkspace->DegenerateModel != 0 )
        {
          glDisable( GL_DEPTH_TEST );
          if ( AWorkspace->NamedStatus & OPENGL_NS_TEXTURE ) DisableTexture();
          AWorkspace->NamedStatus |= OPENGL_NS_WIREFRAME;
        }

        node->elem->Render( AWorkspace );

        if ( !isImmediate && (AWorkspace->NamedStatus & OPENGL_NS_TEXTURE) != 0 ) EnableTexture();
        break;
      }

      case TelPolygon:
      case TelPolygonIndices:
      case TelQuadrangle:
      case TelTriangleMesh:
      {
        if (isImmediate)
        {
          glDepthMask(GL_FALSE);
        }
        else if ( (AWorkspace->NamedStatus & OPENGL_NS_ANIMATION) != 0 &&
                  (AWorkspace->NamedStatus & OPENGL_NS_WIREFRAME) != 0 &&
                  AWorkspace->DegenerateModel < 2 )
        {
          if ( AWorkspace->NamedStatus & OPENGL_NS_TEXTURE ) EnableTexture ();

          glEnable( GL_DEPTH_TEST );
          AWorkspace->NamedStatus &= ~OPENGL_NS_WIREFRAME;
        }

        if ( AWorkspace->NamedStatus & OPENGL_NS_HIGHLIGHT )
          AWorkspace->DisablePolygonOffset();

        node->elem->Render( AWorkspace );

        if ( AWorkspace->NamedStatus & OPENGL_NS_HIGHLIGHT )
          AWorkspace->EnablePolygonOffset();
        break;
      }

      default:
      {
        node->elem->Render( AWorkspace );
        break;
      }
    }
    node = node->next;
  }

  // Restore aspects
  AWorkspace->SetAspectLine(aspect_line);
  AWorkspace->SetAspectFace(aspect_face);
  AWorkspace->SetAspectMarker(aspect_marker);
  AWorkspace->SetAspectText(aspect_text);
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
