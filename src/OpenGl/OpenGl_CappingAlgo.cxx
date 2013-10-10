// Created on: 2013-09-05
// Created by: Anton POLETAEV
// Copyright (c) 2013 OPEN CASCADE SAS
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

#include <OpenGl_CappingAlgo.hxx>
#include <OpenGl_Workspace.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_PrimitiveArray.hxx>
#include <OpenGl_CappingPlaneResource.hxx>
#include <OpenGl_Vec.hxx>

IMPLEMENT_STANDARD_HANDLE(OpenGl_CappingAlgoFilter, OpenGl_RenderFilter)
IMPLEMENT_STANDARD_RTTIEXT(OpenGl_CappingAlgoFilter, OpenGl_RenderFilter)

Handle(OpenGl_RenderFilter) OpenGl_CappingAlgo::myRenderFilter;
OpenGl_AspectFace OpenGl_CappingAlgo::myFrontCulling;
OpenGl_AspectFace OpenGl_CappingAlgo::myNoneCulling;
Standard_Boolean OpenGl_CappingAlgo::myIsInit = Standard_False;

namespace
{
  static const OpenGl_Vec4 THE_CAPPING_PLN_VERTS[12] =
    { OpenGl_Vec4 ( 0.0f, 0.0f, 0.0f, 1.0f),
      OpenGl_Vec4 ( 1.0f, 0.0f, 0.0f, 0.0f),
      OpenGl_Vec4 ( 0.0f, 0.0f, 1.0f, 0.0f),
      OpenGl_Vec4 ( 0.0f, 0.0f, 0.0f, 1.0f),
      OpenGl_Vec4 ( 0.0f, 0.0f, 1.0f, 0.0f),
      OpenGl_Vec4 (-1.0f, 0.0f, 0.0f, 0.0f),
      OpenGl_Vec4 ( 0.0f, 0.0f, 0.0f, 1.0f),
      OpenGl_Vec4 (-1.0f, 0.0f, 0.0f, 0.0f),
      OpenGl_Vec4 ( 0.0f, 0.0f,-1.0f, 0.0f),
      OpenGl_Vec4 ( 0.0f, 0.0f, 0.0f, 1.0f),
      OpenGl_Vec4 ( 0.0f, 0.0f,-1.0f, 0.0f),
      OpenGl_Vec4 ( 1.0f, 0.0f, 0.0f, 0.0f) };

  static const OpenGl_Vec4 THE_CAPPING_PLN_TCOORD[12] = 
    { OpenGl_Vec4 ( 0.0f, 0.0f, 0.0f, 1.0f),
      OpenGl_Vec4 ( 1.0f, 0.0f, 0.0f, 0.0f),
      OpenGl_Vec4 ( 0.0f, 1.0f, 0.0f, 0.0f),
      OpenGl_Vec4 ( 0.0f, 0.0f, 0.0f, 1.0f),
      OpenGl_Vec4 ( 0.0f, 1.0f, 0.0f, 0.0f),
      OpenGl_Vec4 (-1.0f, 0.0f, 0.0f, 0.0f),
      OpenGl_Vec4 ( 0.0f, 0.0f, 0.0f, 1.0f),
      OpenGl_Vec4 (-1.0f, 0.0f, 0.0f, 0.0f),
      OpenGl_Vec4 ( 0.0f,-1.0f, 0.0f, 0.0f),
      OpenGl_Vec4 ( 0.0f, 0.0f, 0.0f, 1.0f),
      OpenGl_Vec4 ( 0.0f,-1.0f, 0.0f, 0.0f),
      OpenGl_Vec4 ( 1.0f, 0.0f, 0.0f, 0.0f) };
};

// =======================================================================
// function : RenderCapping
// purpose  :
// =======================================================================
void OpenGl_CappingAlgo::RenderCapping (const Handle(OpenGl_Workspace)& theWorkspace,
                                        const OpenGl_ListOfGroup& theGroups)
{
  const Handle(OpenGl_Context)& aContext = theWorkspace->GetGlContext();

  // check whether algorithm need to be performed
  Standard_Boolean isCapping = Standard_False;
  const Graphic3d_SetOfHClipPlane& aContextPlanes = aContext->Clipping().Planes();
  Graphic3d_SetOfHClipPlane::Iterator aCappingIt (aContextPlanes);
  for (; aCappingIt.More(); aCappingIt.Next())
  {
    const Handle(Graphic3d_ClipPlane)& aPlane = aCappingIt.Value();
    if (aPlane->IsCapping())
    {
      isCapping = Standard_True;
      break;
    }
  }

  // do not perform algorithm is there is nothing to render
  if (!isCapping)
  {
    return;
  }

  // init internal data
  Init();

  // remember current aspect face defined in workspace
  const OpenGl_AspectFace* aFaceAsp = theWorkspace->AspectFace (Standard_False);

  // replace primitive groups rendering filter
  static Handle(OpenGl_CappingAlgoFilter) aCappingFilter = new OpenGl_CappingAlgoFilter();
  Handle(OpenGl_RenderFilter) aRenderFilter = theWorkspace->GetRenderFilter();
  theWorkspace->SetRenderFilter (aCappingFilter);

  // prepare for rendering the clip planes
  glEnable (GL_STENCIL_TEST);

  // generate capping for every clip plane
  for (aCappingIt.Init (aContextPlanes); aCappingIt.More(); aCappingIt.Next())
  {
    // get plane being rendered
    const Handle(Graphic3d_ClipPlane)& aRenderPlane = aCappingIt.Value();
    if (!aRenderPlane->IsCapping())
    {
      continue;
    }

    // enable only the rendering plane to generate stencil mask
    Graphic3d_SetOfHClipPlane::Iterator aPlaneIt (aContextPlanes);
    for (; aPlaneIt.More(); aPlaneIt.Next())
    {
      const Handle(Graphic3d_ClipPlane)& aPlane = aPlaneIt.Value();
      const Standard_Boolean isOn = (aPlane == aRenderPlane);
      aContext->ChangeClipping().SetEnabled (aPlane, isOn);
    }

    glClear (GL_STENCIL_BUFFER_BIT);
    glColorMask (GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    // override aspects, disable culling
    theWorkspace->SetAspectFace (NoneCulling());
    theWorkspace->AspectFace (Standard_True);

    // evaluate number of pair faces
    glDisable (GL_DEPTH_TEST);
    glDepthMask (GL_FALSE);
    glStencilFunc (GL_ALWAYS, 1, 0x01);
    glStencilOp (GL_KEEP, GL_INVERT, GL_INVERT);

    OpenGl_ListOfGroup::Iterator aGroupIt (theGroups);
    for (; aGroupIt.More(); aGroupIt.Next())
    {
      aGroupIt.Value()->Render (theWorkspace);
    }

    // override material, cull back faces
    theWorkspace->SetAspectFace (FrontCulling());
    theWorkspace->AspectFace (Standard_True);

    // enable all clip plane except the rendered one
    for (aPlaneIt.Init (aContextPlanes); aPlaneIt.More(); aPlaneIt.Next())
    {
      const Handle(Graphic3d_ClipPlane)& aPlane = aPlaneIt.Value();
      const Standard_Boolean isOn = (aPlane != aRenderPlane);
      aContext->ChangeClipping().SetEnabled (aPlane, isOn);
    }

    // render capping plane using the generated stencil mask
    glColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask (GL_TRUE);
    glDepthFunc (GL_LESS);
    glStencilFunc (GL_EQUAL, 1, 0x01);
    glStencilOp (GL_KEEP, GL_KEEP, GL_KEEP);
    glEnable (GL_DEPTH_TEST);

    RenderPlane (theWorkspace, aRenderPlane);
  }

  // restore previous application state
  glClear (GL_STENCIL_BUFFER_BIT);
  glStencilFunc (GL_ALWAYS, 0, 0xFF);
  glDisable (GL_STENCIL_TEST);

  // enable clipping
  for (aCappingIt.Init (aContextPlanes); aCappingIt.More(); aCappingIt.Next())
  {
    aContext->ChangeClipping().SetEnabled (aCappingIt.Value(), Standard_True);
  }

  // restore rendering aspects
  theWorkspace->SetAspectFace (aFaceAsp);
  theWorkspace->SetRenderFilter (aRenderFilter);
}

// =======================================================================
// function : RenderPlane
// purpose  :
// =======================================================================
void OpenGl_CappingAlgo::RenderPlane (const Handle(OpenGl_Workspace)& theWorkspace,
                                      const Handle(Graphic3d_ClipPlane)& thePlane)
{
  const Handle(OpenGl_Context)& aContext = theWorkspace->GetGlContext();

  // get resource for the plane
  TCollection_AsciiString aResId = thePlane->GetId();

  Handle(OpenGl_CappingPlaneResource) aPlaneRes;
  if (!aContext->GetResource (aResId, aPlaneRes))
  {
    // share and register for release once the resource is no longer used
    aPlaneRes = new OpenGl_CappingPlaneResource (thePlane);
    aContext->ShareResource (aResId, aPlaneRes);
  }

  aPlaneRes->Update (aContext);

  const OpenGl_AspectFace* aFaceAspect = theWorkspace->AspectFace (Standard_False);
  const OpenGl_AspectFace* aPlaneAspect = aPlaneRes->AspectFace();
  if (aPlaneAspect != NULL)
  {
    theWorkspace->SetAspectFace (aPlaneAspect);
  }

  // apply aspect for rendering
  theWorkspace->AspectFace (Standard_True);

  // set identity model matrix
  const OpenGl_Matrix* aModelMatrix = theWorkspace->SetStructureMatrix (&OpenGl_IdentityMatrix);

  glMultMatrixf ((const GLfloat*)aPlaneRes->Orientation());
  glNormal3f (0.0f, 1.0f, 0.0f);
  glEnableClientState (GL_VERTEX_ARRAY);
  glVertexPointer (4, GL_FLOAT, 0, (GLfloat* )&THE_CAPPING_PLN_VERTS);
  glEnableClientState (GL_TEXTURE_COORD_ARRAY);
  glTexCoordPointer (4, GL_FLOAT, 0, (GLfloat*)&THE_CAPPING_PLN_TCOORD);
  glDrawArrays (GL_TRIANGLES, 0, 12);
  glDisableClientState (GL_VERTEX_ARRAY);
  glDisableClientState (GL_TEXTURE_COORD_ARRAY);

  theWorkspace->SetStructureMatrix (aModelMatrix);
  theWorkspace->SetAspectFace (aFaceAspect);

  // set delayed resource release
  aPlaneRes.Nullify();
  aContext->ReleaseResource (aResId, Standard_True);
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
void OpenGl_CappingAlgo::Init()
{
  if (myIsInit)
    return;

  myRenderFilter = new OpenGl_CappingAlgoFilter();
  myNoneCulling.CullingMode = TelCullNone;
  myNoneCulling.Edge = 0;

  myFrontCulling.CullingMode = TelCullBack;
  myFrontCulling.Edge = 0;

  myIsInit = Standard_True;
}

// =======================================================================
// function : CanRender
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_CappingAlgoFilter::CanRender (const OpenGl_Element* theElement)
{
  const OpenGl_PrimitiveArray* aPArray =
    dynamic_cast<const OpenGl_PrimitiveArray*> (theElement);
  if (!aPArray)
    return Standard_False;

  switch (aPArray->PArray()->type)
  {
    case TelPolygonsArrayType :
    case TelTrianglesArrayType :
    case TelQuadranglesArrayType :
    case TelTriangleStripsArrayType :
    case TelQuadrangleStripsArrayType :
    case TelTriangleFansArrayType :
      return Standard_True;

    default:
      return Standard_False;
  }
}
