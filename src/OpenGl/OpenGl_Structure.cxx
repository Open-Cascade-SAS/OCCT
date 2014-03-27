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

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include <OpenGl_CappingAlgo.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_GlCore11.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <OpenGl_ShaderManager.hxx>
#include <OpenGl_ShaderProgram.hxx>
#include <OpenGl_Structure.hxx>
#include <OpenGl_telem_util.hxx>
#include <OpenGl_Vec.hxx>
#include <OpenGl_View.hxx>
#include <OpenGl_Workspace.hxx>

#include <Graphic3d_SequenceOfHClipPlane_Handle.hxx>

IMPLEMENT_STANDARD_HANDLE (OpenGl_Structure, Graphic3d_CStructure)
IMPLEMENT_STANDARD_RTTIEXT(OpenGl_Structure, Graphic3d_CStructure)

//! Auxiliary class for bounding box presentation
class OpenGl_BndBoxPrs : public OpenGl_Element
{

public:

  //! Main constructor
  OpenGl_BndBoxPrs (const CALL_DEF_BOUNDBOX& theBndBox)
  {
    const float Xm = theBndBox.Pmin.x;
    const float Ym = theBndBox.Pmin.y;
    const float Zm = theBndBox.Pmin.z;
    const float XM = theBndBox.Pmax.x;
    const float YM = theBndBox.Pmax.y;
    const float ZM = theBndBox.Pmax.z;
    myVerts[0]  = OpenGl_Vec3 (Xm, Ym, Zm);
    myVerts[1]  = OpenGl_Vec3 (Xm, Ym, ZM);
    myVerts[2]  = OpenGl_Vec3 (Xm, YM, ZM);
    myVerts[3]  = OpenGl_Vec3 (Xm, YM, Zm);
    myVerts[4]  = OpenGl_Vec3 (Xm, Ym, Zm);
    myVerts[5]  = OpenGl_Vec3 (XM, Ym, Zm);
    myVerts[6]  = OpenGl_Vec3 (XM, Ym, ZM);
    myVerts[7]  = OpenGl_Vec3 (XM, YM, ZM);
    myVerts[8]  = OpenGl_Vec3 (XM, YM, Zm);
    myVerts[9]  = OpenGl_Vec3 (XM, Ym, Zm);
    myVerts[10] = OpenGl_Vec3 (XM, YM, Zm);
    myVerts[11] = OpenGl_Vec3 (Xm, YM, Zm);
    myVerts[12] = OpenGl_Vec3 (Xm, YM, ZM);
    myVerts[13] = OpenGl_Vec3 (XM, YM, ZM);
    myVerts[14] = OpenGl_Vec3 (XM, Ym, ZM);
    myVerts[15] = OpenGl_Vec3 (Xm, Ym, ZM);
  }

  //! Render presentation
  virtual void Render  (const Handle(OpenGl_Workspace)& theWorkspace) const
  {
    // Apply line aspect
    const OpenGl_AspectLine*     anAspectLine = theWorkspace->AspectLine (Standard_True);
    const Handle(OpenGl_Texture) aPrevTexture = theWorkspace->DisableTexture();

    glDisable (GL_LIGHTING);
    if ((theWorkspace->NamedStatus & OPENGL_NS_IMMEDIATE) != 0)
    {
      glDepthMask (GL_FALSE);
    }

    // Use highlight colors
    glColor3fv ((theWorkspace->NamedStatus & OPENGL_NS_HIGHLIGHT) ? theWorkspace->HighlightColor->rgb : anAspectLine->Color().rgb);

    glEnableClientState (GL_VERTEX_ARRAY);
    glVertexPointer (3, GL_FLOAT, 0, (GLfloat* )&myVerts);
    glDrawArrays (GL_LINE_STRIP, 0, 16);
    glDisableClientState (GL_VERTEX_ARRAY);

    // restore aspects
    if (!aPrevTexture.IsNull())
    {
      theWorkspace->EnableTexture (aPrevTexture);
    }
  }

  //! Release graphical resources
  virtual void Release (const Handle(OpenGl_Context)& )
  {
    //
  }

protected:

  //! Protected destructor
  virtual ~OpenGl_BndBoxPrs() {}

private:

  OpenGl_Vec3 myVerts[16]; //!< vertices array

public:

  DEFINE_STANDARD_ALLOC

};

/*----------------------------------------------------------------------*/

// =======================================================================
// function : call_util_transpose_mat
// purpose  :
// =======================================================================
static void call_util_transpose_mat (float tmat[16], float mat[4][4])
{
  int i, j;

  for (i=0; i<4; i++)
    for (j=0; j<4; j++)
      tmat[j*4+i] = mat[i][j];
}

// =======================================================================
// function : OpenGl_Structure
// purpose  :
// =======================================================================
OpenGl_Structure::OpenGl_Structure (const Handle(Graphic3d_StructureManager)& theManager)
: Graphic3d_CStructure (theManager),
  myTransformation(NULL),
  myTransPers(NULL),
  myAspectLine(NULL),
  myAspectFace(NULL),
  myAspectMarker(NULL),
  myAspectText(NULL),
  myHighlightColor(NULL),
  myNamedStatus(0),
  myZLayer(0)
{
  UpdateNamedStatus();
#if HAVE_OPENCL
  myIsRaytracable = Standard_False;
  myModificationState = 0;
#endif
}

// =======================================================================
// function : ~OpenGl_Structure
// purpose  :
// =======================================================================
OpenGl_Structure::~OpenGl_Structure()
{
  Release (Handle(OpenGl_Context)());
  delete myTransformation;  myTransformation  = NULL;
  delete myTransPers;       myTransPers       = NULL;
}

// =======================================================================
// function : UpdateAspects
// purpose  :
// =======================================================================
void OpenGl_Structure::UpdateAspects()
{
  SetTransformPersistence (TransformPersistence);

  if (ContextLine.IsDef)
    SetAspectLine (ContextLine);

  if (ContextFillArea.IsDef)
    SetAspectFace (ContextFillArea);

  if (ContextMarker.IsDef)
    SetAspectMarker (ContextMarker);

  if (ContextText.IsDef)
    SetAspectText (ContextText);
}

// =======================================================================
// function : UpdateTransformation
// purpose  :
// =======================================================================
void OpenGl_Structure::UpdateTransformation()
{
  if (myTransformation == NULL)
  {
    myTransformation = new OpenGl_Matrix();
  }

  matcpy (myTransformation->mat, &Graphic3d_CStructure::Transformation[0][0]);

#ifdef HAVE_OPENCL
  if (myIsRaytracable)
  {
    UpdateStateWithAncestorStructures();
  }
#endif
}

// =======================================================================
// function : SetTransformPersistence
// purpose  :
// =======================================================================
void OpenGl_Structure::SetTransformPersistence(const CALL_DEF_TRANSFORM_PERSISTENCE &ATransPers)
{
  if (!myTransPers)
    myTransPers = new TEL_TRANSFORM_PERSISTENCE;

  myTransPers->mode = ATransPers.Flag;
  myTransPers->pointX = ATransPers.Point.x;
  myTransPers->pointY = ATransPers.Point.y;
  myTransPers->pointZ = ATransPers.Point.z;
}

// =======================================================================
// function : SetAspectLine
// purpose  :
// =======================================================================
void OpenGl_Structure::SetAspectLine (const CALL_DEF_CONTEXTLINE &theAspect)
{
  if (!myAspectLine)
  {
    myAspectLine = new OpenGl_AspectLine();
  }
  myAspectLine->SetAspect (theAspect);
}

// =======================================================================
// function : SetAspectFace
// purpose  :
// =======================================================================
void OpenGl_Structure::SetAspectFace (const CALL_DEF_CONTEXTFILLAREA& theAspect)
{
  if (!myAspectFace)
  {
    myAspectFace = new OpenGl_AspectFace();
  }
  myAspectFace->SetAspect (theAspect);

#ifdef HAVE_OPENCL
  if (myIsRaytracable)
  {
    UpdateStateWithAncestorStructures();
  }
#endif
}

// =======================================================================
// function : SetAspectMarker
// purpose  :
// =======================================================================
void OpenGl_Structure::SetAspectMarker (const CALL_DEF_CONTEXTMARKER& theAspect)
{
  if (!myAspectMarker)
  {
    myAspectMarker = new OpenGl_AspectMarker();
  }
  myAspectMarker->SetAspect (theAspect);
}

// =======================================================================
// function : SetAspectText
// purpose  :
// =======================================================================
void OpenGl_Structure::SetAspectText (const CALL_DEF_CONTEXTTEXT &theAspect)
{
  if (!myAspectText)
  {
    myAspectText = new OpenGl_AspectText();
  }
  myAspectText->SetAspect (theAspect);
}

// =======================================================================
// function : clearHighlightBox
// purpose  :
// =======================================================================
void OpenGl_Structure::clearHighlightBox (const Handle(OpenGl_Context)& theGlCtx)
{
  if (!myHighlightBox.IsNull())
  {
    myHighlightBox->Release (theGlCtx);
    myHighlightBox.Nullify();
  }
}

// =======================================================================
// function : HighlightWithColor
// purpose  :
// =======================================================================
void OpenGl_Structure::HighlightWithColor (const Graphic3d_Vec3&  theColor,
                                           const Standard_Boolean theToCreate)
{
  const Handle(OpenGl_Context)& aCtx = GlDriver()->GetSharedContext();
  if (theToCreate)
    setHighlightColor   (aCtx, theColor);
  else
    clearHighlightColor (aCtx);
}

// =======================================================================
// function : HighlightWithBndBox
// purpose  :
// =======================================================================
void OpenGl_Structure::HighlightWithBndBox (const Handle(Graphic3d_Structure)& theStruct,
                                            const Standard_Boolean             theToCreate)
{
  const Handle(OpenGl_Context)& aCtx = GlDriver()->GetSharedContext();
  if (!theToCreate)
  {
    clearHighlightBox (aCtx);
    return;
  }

  if (!myHighlightBox.IsNull())
  {
    myHighlightBox->Release (aCtx);
  }
  else
  {
    myHighlightBox = new OpenGl_Group (theStruct);
  }

  CALL_DEF_CONTEXTLINE& aContextLine = myHighlightBox->ChangeContextLine();
  aContextLine.IsDef    = 1;
  aContextLine.Color    = BoundBox.Color;
  aContextLine.LineType = Aspect_TOL_SOLID;
  aContextLine.Width    = 1.0f;
  myHighlightBox->UpdateAspectLine (Standard_True);

  OpenGl_BndBoxPrs* aBndBoxPrs = new OpenGl_BndBoxPrs (BoundBox);
  myHighlightBox->AddElement (aBndBoxPrs);
}

// =======================================================================
// function : setHighlightColor
// purpose  :
// =======================================================================
void OpenGl_Structure::setHighlightColor (const Handle(OpenGl_Context)& theGlCtx,
                                          const Graphic3d_Vec3&         theColor)
{
  clearHighlightBox (theGlCtx);
  if (myHighlightColor == NULL)
  {
    myHighlightColor = new TEL_COLOUR();
  }

  myHighlightColor->rgb[0] = theColor.r();
  myHighlightColor->rgb[1] = theColor.g();
  myHighlightColor->rgb[2] = theColor.b();
  myHighlightColor->rgb[3] = 1.F;
}

// =======================================================================
// function : clearHighlightColor
// purpose  :
// =======================================================================
void OpenGl_Structure::clearHighlightColor (const Handle(OpenGl_Context)& theGlCtx)
{
  clearHighlightBox(theGlCtx);
  delete myHighlightColor;
  myHighlightColor = NULL;
}

// =======================================================================
// function : UpdateNamedStatus
// purpose  :
// =======================================================================
void OpenGl_Structure::UpdateNamedStatus()
{
  myNamedStatus = 0;
  if (highlight) myNamedStatus |= OPENGL_NS_HIGHLIGHT;
  if (!visible)  myNamedStatus |= OPENGL_NS_HIDE;

#ifdef HAVE_OPENCL
  if (myIsRaytracable)
  {
    UpdateStateWithAncestorStructures();
  }
#endif
}

#ifdef HAVE_OPENCL

// =======================================================================
// function : RegisterAncestorStructure
// purpose  :
// =======================================================================
void OpenGl_Structure::RegisterAncestorStructure (const OpenGl_Structure* theStructure) const
{
  for (OpenGl_ListOfStructure::Iterator anIt (myAncestorStructures); anIt.More(); anIt.Next())
  {
    if (anIt.Value() == theStructure)
    {
      return;
    }
  }

  myAncestorStructures.Append (theStructure);
}

// =======================================================================
// function : UnregisterAncestorStructure
// purpose  :
// =======================================================================
void OpenGl_Structure::UnregisterAncestorStructure (const OpenGl_Structure* theStructure) const
{
  for (OpenGl_ListOfStructure::Iterator anIt (myAncestorStructures); anIt.More(); anIt.Next())
  {
    if (anIt.Value() == theStructure)
    {
      myAncestorStructures.Remove (anIt);
      return;
    }
  }
}

// =======================================================================
// function : UnregisterFromAncestorStructure
// purpose  :
// =======================================================================
void OpenGl_Structure::UnregisterFromAncestorStructure() const
{
  for (OpenGl_ListOfStructure::Iterator anIta (myAncestorStructures); anIta.More(); anIta.Next())
  {
    OpenGl_Structure* anAncestor = const_cast<OpenGl_Structure*> (anIta.ChangeValue());

    for (OpenGl_ListOfStructure::Iterator anIts (anAncestor->myConnected); anIts.More(); anIts.Next())
    {
      if (anIts.Value() == this)
      {
        anAncestor->myConnected.Remove (anIts);
        return;
      }
    }
  }
}

// =======================================================================
// function : UpdateStateWithAncestorStructures
// purpose  :
// =======================================================================
void OpenGl_Structure::UpdateStateWithAncestorStructures() const
{
  myModificationState++;

  for (OpenGl_ListOfStructure::Iterator anIt (myAncestorStructures); anIt.More(); anIt.Next())
  {
    anIt.Value()->UpdateStateWithAncestorStructures();
  }
}

// =======================================================================
// function : UpdateRaytracableWithAncestorStructures
// purpose  :
// =======================================================================
void OpenGl_Structure::UpdateRaytracableWithAncestorStructures() const
{
  myIsRaytracable = OpenGl_Raytrace::IsRaytracedStructure (this);

  if (!myIsRaytracable)
  {
    for (OpenGl_ListOfStructure::Iterator anIt (myAncestorStructures); anIt.More(); anIt.Next())
    {
      anIt.Value()->UpdateRaytracableWithAncestorStructures();
    }
  }
}

// =======================================================================
// function : SetRaytracableWithAncestorStructures
// purpose  :
// =======================================================================
void OpenGl_Structure::SetRaytracableWithAncestorStructures() const
{
  myIsRaytracable = Standard_True;

  for (OpenGl_ListOfStructure::Iterator anIt (myAncestorStructures); anIt.More(); anIt.Next())
  {
    if (!anIt.Value()->IsRaytracable())
    {
      anIt.Value()->SetRaytracableWithAncestorStructures();
    }
  }
}

#endif

// =======================================================================
// function : Connect
// purpose  :
// =======================================================================
void OpenGl_Structure::Connect (Graphic3d_CStructure& theStructure)
{
  OpenGl_Structure* aStruct = (OpenGl_Structure* )&theStructure;
  Disconnect (theStructure);
  myConnected.Append (aStruct);

#ifdef HAVE_OPENCL
  if (aStruct->IsRaytracable())
  {
    UpdateStateWithAncestorStructures();
    SetRaytracableWithAncestorStructures();
  }

  aStruct->RegisterAncestorStructure (this);
#endif
}

// =======================================================================
// function : Disconnect
// purpose  :
// =======================================================================
void OpenGl_Structure::Disconnect (Graphic3d_CStructure& theStructure)
{
  OpenGl_Structure* aStruct = (OpenGl_Structure* )&theStructure;
  for (OpenGl_ListOfStructure::Iterator anIter (myConnected); anIter.More(); anIter.Next())
  {
    // Check for the given structure
    if (anIter.Value() == aStruct)
    {
      myConnected.Remove (anIter);

#ifdef HAVE_OPENCL
      if (aStruct->IsRaytracable())
      {
        UpdateStateWithAncestorStructures();
        UpdateRaytracableWithAncestorStructures();
      }

      aStruct->UnregisterAncestorStructure (this);
#endif
      return;
    }
  }
}

// =======================================================================
// function : NewGroup
// purpose  :
// =======================================================================
Handle(Graphic3d_Group) OpenGl_Structure::NewGroup (const Handle(Graphic3d_Structure)& theStruct)
{
  Handle(OpenGl_Group) aGroup = new OpenGl_Group (theStruct);
  myGroups.Append (aGroup);
  return aGroup;
}

// =======================================================================
// function : RemoveGroup
// purpose  :
// =======================================================================
void OpenGl_Structure::RemoveGroup (const Handle(Graphic3d_Group)& theGroup)
{
  if (theGroup.IsNull())
  {
    return;
  }

  for (Graphic3d_SequenceOfGroup::Iterator aGroupIter (myGroups); aGroupIter.More(); aGroupIter.Next())
  {
    // Check for the given group
    if (aGroupIter.Value() == theGroup)
    {
      theGroup->Clear (Standard_False);

    #ifdef HAVE_OPENCL
      if (((OpenGl_Group* )theGroup.operator->())->IsRaytracable())
      {
        UpdateStateWithAncestorStructures();
        UpdateRaytracableWithAncestorStructures();
      }
    #endif

      myGroups.Remove (aGroupIter);
      return;
    }
  }
}

// =======================================================================
// function : Clear
// purpose  :
// =======================================================================
void OpenGl_Structure::Clear()
{
  Clear (GlDriver()->GetSharedContext());
}

// =======================================================================
// function : Clear
// purpose  :
// =======================================================================
void OpenGl_Structure::Clear (const Handle(OpenGl_Context)& theGlCtx)
{
#ifdef HAVE_OPENCL
  Standard_Boolean aRaytracableGroupDeleted (Standard_False);
#endif

  // Release groups
  for (OpenGl_Structure::GroupIterator aGroupIter (myGroups); aGroupIter.More(); aGroupIter.Next())
  {
  #ifdef HAVE_OPENCL
    aRaytracableGroupDeleted |= aGroupIter.Value()->IsRaytracable();
  #endif

    // Delete objects
    aGroupIter.ChangeValue()->Release (theGlCtx);
  }
  myGroups.Clear();

#ifdef HAVE_OPENCL
  if (aRaytracableGroupDeleted)
  {
    UpdateStateWithAncestorStructures();
    UpdateRaytracableWithAncestorStructures();
  }
#endif
}

// =======================================================================
// function : Render
// purpose  :
// =======================================================================
void OpenGl_Structure::Render (const Handle(OpenGl_Workspace) &AWorkspace) const
{
  // Process the structure only if visible
  if ( myNamedStatus & OPENGL_NS_HIDE )
    return;

  // Render named status
  const Standard_Integer named_status = AWorkspace->NamedStatus;
  AWorkspace->NamedStatus |= myNamedStatus;

  // Is rendering in ADD or IMMEDIATE mode?
  const Standard_Boolean isImmediate = (AWorkspace->NamedStatus & OPENGL_NS_IMMEDIATE) != 0;

  const Handle(OpenGl_Context)& aCtx = AWorkspace->GetGlContext();

  // Apply local transformation
  GLint matrix_mode = 0;
  const OpenGl_Matrix *local_trsf = NULL;
  if (myTransformation)
  {
    if (isImmediate)
    {
      Tmatrix3 aModelWorld;
      call_util_transpose_mat (*aModelWorld, myTransformation->mat);
      glGetIntegerv (GL_MATRIX_MODE, &matrix_mode);

      if (!aCtx->ShaderManager()->IsEmpty())
      {
        Tmatrix3 aWorldView;
        glGetFloatv (GL_MODELVIEW_MATRIX, *aWorldView);

        Tmatrix3 aProjection;
        glGetFloatv (GL_PROJECTION_MATRIX, *aProjection);

        aCtx->ShaderManager()->UpdateModelWorldStateTo (&aModelWorld);
        aCtx->ShaderManager()->UpdateWorldViewStateTo (&aWorldView);
        aCtx->ShaderManager()->UpdateProjectionStateTo (&aProjection);
      }

      glMatrixMode (GL_MODELVIEW);
      glPushMatrix ();
      glScalef (1.F, 1.F, 1.F);
      glMultMatrixf (*aModelWorld);
    }
    else
    {
      glMatrixMode (GL_MODELVIEW);
      glPushMatrix();

      local_trsf = AWorkspace->SetStructureMatrix (myTransformation);
    }
  }

  // Apply transform persistence
  const TEL_TRANSFORM_PERSISTENCE *trans_pers = NULL;
  if ( myTransPers && myTransPers->mode != 0 )
  {
    trans_pers = AWorkspace->ActiveView()->BeginTransformPersistence (aCtx, myTransPers);
  }

  // Apply aspects
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

  // Apply highlight box
  if (!myHighlightBox.IsNull())
  {
    myHighlightBox->Render (AWorkspace);
  }

  // Apply highlight color
  const TEL_COLOUR *highlight_color = AWorkspace->HighlightColor;
  if (myHighlightColor)
    AWorkspace->HighlightColor = myHighlightColor;

  // Render connected structures
  OpenGl_ListOfStructure::Iterator its(myConnected);
  while (its.More())
  {
    its.Value()->Render(AWorkspace);
    its.Next();
  }

  // Set up plane equations for non-structure transformed global model-view matrix
  const Handle(OpenGl_Context)& aContext = AWorkspace->GetGlContext();

  // List of planes to be applied to context state
  Handle(Graphic3d_SequenceOfHClipPlane) aUserPlanes;

  // Collect clipping planes of structure scope
  if (!myClipPlanes.IsEmpty())
  {
    Graphic3d_SequenceOfHClipPlane::Iterator aClippingIt (myClipPlanes);
    for (; aClippingIt.More(); aClippingIt.Next())
    {
      const Handle(Graphic3d_ClipPlane)& aClipPlane = aClippingIt.Value();
      if (!aClipPlane->IsOn())
      {
        continue;
      }

      if (aUserPlanes.IsNull())
      {
        aUserPlanes = new Graphic3d_SequenceOfHClipPlane();
      }

      aUserPlanes->Append (aClipPlane);
    }
  }

  if (!aUserPlanes.IsNull() && !aUserPlanes->IsEmpty())
  {
    // add planes at loaded view matrix state
    aContext->ChangeClipping().AddWorld (*aUserPlanes, AWorkspace);

    // Set OCCT state uniform variables
    if (!aContext->ShaderManager()->IsEmpty())
    {
      aContext->ShaderManager()->UpdateClippingState();
    }
  }

  // Render groups
  const Graphic3d_SequenceOfGroup& aGroups = DrawGroups();
  for (OpenGl_Structure::GroupIterator aGroupIter (aGroups); aGroupIter.More(); aGroupIter.Next())
  {
    aGroupIter.Value()->Render (AWorkspace);
  }

  // Render capping for structure groups
  if (!aContext->Clipping().Planes().IsEmpty())
  {
    OpenGl_CappingAlgo::RenderCapping (AWorkspace, aGroups);
  }

  // Revert structure clippings
  if (!aUserPlanes.IsNull() && !aUserPlanes->IsEmpty())
  {
    aContext->ChangeClipping().Remove (*aUserPlanes);

    // Set OCCT state uniform variables
    if (!aContext->ShaderManager()->IsEmpty())
    {
      aContext->ShaderManager()->RevertClippingState();
    }
  }

  // Restore highlight color
  AWorkspace->HighlightColor = highlight_color;

  // Restore aspects
  AWorkspace->SetAspectLine(aspect_line);
  AWorkspace->SetAspectFace(aspect_face);
  AWorkspace->SetAspectMarker(aspect_marker);
  AWorkspace->SetAspectText(aspect_text);

  // Restore transform persistence
  if ( myTransPers && myTransPers->mode != 0 )
  {
    AWorkspace->ActiveView()->BeginTransformPersistence (aContext, trans_pers);
  }

  // Restore local transformation
  if (myTransformation)
  {
    if (isImmediate)
    {
      glPopMatrix ();
      glMatrixMode (matrix_mode);

      Tmatrix3 aModelWorldState = { { 1.f, 0.f, 0.f, 0.f },
                                    { 0.f, 1.f, 0.f, 0.f },
                                    { 0.f, 0.f, 1.f, 0.f },
                                    { 0.f, 0.f, 0.f, 1.f } };

      aContext->ShaderManager()->RevertModelWorldStateTo (&aModelWorldState);
    }
    else
    {
      AWorkspace->SetStructureMatrix (local_trsf, true);

      glMatrixMode (GL_MODELVIEW);
      glPopMatrix();
    }
  }

  // Restore named status
  AWorkspace->NamedStatus = named_status;
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_Structure::Release (const Handle(OpenGl_Context)& theGlCtx)
{
  // Release groups
  Clear (theGlCtx);
  OpenGl_Element::Destroy (theGlCtx, myAspectLine);
  OpenGl_Element::Destroy (theGlCtx, myAspectFace);
  OpenGl_Element::Destroy (theGlCtx, myAspectMarker);
  OpenGl_Element::Destroy (theGlCtx, myAspectText);
  clearHighlightColor (theGlCtx);

#ifdef HAVE_OPENCL
  // Remove from connected list of ancestor
  UnregisterFromAncestorStructure();
#endif
}

// =======================================================================
// function : ReleaseGlResources
// purpose  :
// =======================================================================
void OpenGl_Structure::ReleaseGlResources (const Handle(OpenGl_Context)& theGlCtx)
{
  for (OpenGl_Structure::GroupIterator aGroupIter (myGroups); aGroupIter.More(); aGroupIter.Next())
  {
    aGroupIter.ChangeValue()->Release (theGlCtx);
  }
  if (myAspectLine != NULL)
  {
    myAspectLine->Release (theGlCtx);
  }
  if (myAspectFace != NULL)
  {
    myAspectFace->Release (theGlCtx);
  }
  if (myAspectMarker != NULL)
  {
    myAspectMarker->Release (theGlCtx);
  }
  if (myAspectText != NULL)
  {
    myAspectText->Release (theGlCtx);
  }
  if (!myHighlightBox.IsNull())
  {
    myHighlightBox->Release (theGlCtx);
  }
}

//=======================================================================
//function : SetZLayer
//purpose  :
//=======================================================================
void OpenGl_Structure::SetZLayer (const Standard_Integer theLayerIndex)
{
  myZLayer = theLayerIndex;
}

//=======================================================================
//function : GetZLayer
//purpose  :
//=======================================================================
Standard_Integer OpenGl_Structure::GetZLayer () const
{
  return myZLayer;
}

class OpenGl_StructureShadow : public OpenGl_Structure
{

public:

  //! Create empty structure
  OpenGl_StructureShadow (const Handle(Graphic3d_StructureManager)& theManager,
                          const Handle(OpenGl_Structure)&           theStructure);

  virtual const Graphic3d_SequenceOfGroup& DrawGroups() const { return myParent->DrawGroups(); }

private:

  Handle(OpenGl_Structure) myParent;

public:

  DEFINE_STANDARD_RTTI(OpenGl_Structure) // Type definition

};

DEFINE_STANDARD_HANDLE(OpenGl_StructureShadow, OpenGl_Structure)

IMPLEMENT_STANDARD_HANDLE (OpenGl_StructureShadow, OpenGl_Structure)
IMPLEMENT_STANDARD_RTTIEXT(OpenGl_StructureShadow, OpenGl_Structure)

OpenGl_StructureShadow::OpenGl_StructureShadow (const Handle(Graphic3d_StructureManager)& theManager,
                                                const Handle(OpenGl_Structure)&           theStructure)
: OpenGl_Structure (theManager)
{
  Handle(OpenGl_StructureShadow) aShadow = Handle(OpenGl_StructureShadow)::DownCast (theStructure);
  myParent = aShadow.IsNull() ? theStructure : aShadow->myParent;


  Composition   = myParent->Composition;
  ContainsFacet = myParent->ContainsFacet;
  IsInfinite    = myParent->IsInfinite;
  for (Standard_Integer i = 0; i <= 3; ++i)
  {
    for (Standard_Integer j = 0; j <= 3; ++j)
    {
      Graphic3d_CStructure::Transformation[i][j] = myParent->Graphic3d_CStructure::Transformation[i][j];
    }
  }

  TransformPersistence.IsSet = myParent->TransformPersistence.IsSet;
  TransformPersistence.Flag  = myParent->TransformPersistence.Flag;
  TransformPersistence.Point = myParent->TransformPersistence.Point;
}

//=======================================================================
//function : ShadowLink
//purpose  :
//=======================================================================
Handle(Graphic3d_CStructure) OpenGl_Structure::ShadowLink (const Handle(Graphic3d_StructureManager)& theManager) const
{
  return new OpenGl_StructureShadow (theManager, this);
}
