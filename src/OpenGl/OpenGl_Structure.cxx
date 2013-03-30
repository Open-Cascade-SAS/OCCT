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

#include <OpenGl_GlCore11.hxx>

#include <OpenGl_Structure.hxx>

#include <OpenGl_Workspace.hxx>
#include <OpenGl_Vec.hxx>
#include <OpenGl_View.hxx>

#include <OpenGl_telem_util.hxx>

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
    if ((theWorkspace->NamedStatus & (OPENGL_NS_ADD | OPENGL_NS_IMMEDIATE)) != 0)
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

static void call_util_transpose_mat (float tmat[16], float mat[4][4])
{
  int i, j;

  for (i=0; i<4; i++)
    for (j=0; j<4; j++)
      tmat[j*4+i] = mat[i][j];
}

/*----------------------------------------------------------------------*/

OpenGl_Structure::OpenGl_Structure ()
: myTransformation(NULL),
  myTransPers(NULL),
  myAspectLine(NULL),
  myAspectFace(NULL),
  myAspectMarker(NULL),
  myAspectText(NULL),
  myHighlightBox(NULL),
  myHighlightColor(NULL),
  myNamedStatus(0),
  myZLayer(0)
{
}

/*----------------------------------------------------------------------*/

OpenGl_Structure::~OpenGl_Structure()
{
  Release (Handle(OpenGl_Context)());
  delete myTransformation;  myTransformation  = NULL;
  delete myTransPers;       myTransPers       = NULL;
}

/*----------------------------------------------------------------------*/

void OpenGl_Structure::SetTransformation(const float *AMatrix)
{
  if (!myTransformation)
    myTransformation = new OpenGl_Matrix();

  matcpy( myTransformation->mat, AMatrix );
}

/*----------------------------------------------------------------------*/

void OpenGl_Structure::SetTransformPersistence(const CALL_DEF_TRANSFORM_PERSISTENCE &ATransPers)
{
  if (!myTransPers)
    myTransPers = new TEL_TRANSFORM_PERSISTENCE;

  myTransPers->mode = ATransPers.Flag;
  myTransPers->pointX = ATransPers.Point.x;
  myTransPers->pointY = ATransPers.Point.y;
  myTransPers->pointZ = ATransPers.Point.z;
}

/*----------------------------------------------------------------------*/

void OpenGl_Structure::SetAspectLine (const CALL_DEF_CONTEXTLINE &AContext)
{
  if (!myAspectLine)
    myAspectLine = new OpenGl_AspectLine();
  myAspectLine->SetContext( AContext );
}

/*----------------------------------------------------------------------*/

void OpenGl_Structure::SetAspectFace (const Handle(OpenGl_Context)&   theCtx,
                                      const CALL_DEF_CONTEXTFILLAREA& theAspect)
{
  if (!myAspectFace)
  {
    myAspectFace = new OpenGl_AspectFace();
  }
  myAspectFace->Init (theCtx, theAspect);
}

/*----------------------------------------------------------------------*/

void OpenGl_Structure::SetAspectMarker (const CALL_DEF_CONTEXTMARKER &AContext)
{
  if (!myAspectMarker)
    myAspectMarker = new OpenGl_AspectMarker();
  myAspectMarker->SetContext( AContext );
}

/*----------------------------------------------------------------------*/

void OpenGl_Structure::SetAspectText (const CALL_DEF_CONTEXTTEXT &AContext)
{
  if (!myAspectText)
    myAspectText = new OpenGl_AspectText();
  myAspectText->SetContext( AContext );
}

/*----------------------------------------------------------------------*/

void OpenGl_Structure::SetHighlightBox (const Handle(OpenGl_Context)& theGlCtx,
                                        const CALL_DEF_BOUNDBOX&      theBoundBox)
{
  if (myHighlightBox != NULL)
  {
    myHighlightBox->Release (theGlCtx);
  }
  else
  {
    myHighlightBox = new OpenGl_Group();
  }

  CALL_DEF_CONTEXTLINE aContextLine;
  aContextLine.Color    = theBoundBox.Color;
  aContextLine.LineType = Aspect_TOL_SOLID;
  aContextLine.Width    = 1.0f;
  myHighlightBox->SetAspectLine (aContextLine);

  OpenGl_BndBoxPrs* aBndBoxPrs = new OpenGl_BndBoxPrs (theBoundBox);
  myHighlightBox->AddElement (TelParray, aBndBoxPrs);
}

/*----------------------------------------------------------------------*/

void OpenGl_Structure::ClearHighlightBox (const Handle(OpenGl_Context)& theGlCtx)
{
  if (myHighlightBox != NULL)
  {
    OpenGl_Element::Destroy (theGlCtx, myHighlightBox);
  }
}

/*----------------------------------------------------------------------*/

void OpenGl_Structure::SetHighlightColor (const Handle(OpenGl_Context)& theGlCtx,
                                          const Standard_ShortReal R,
                                          const Standard_ShortReal G,
                                          const Standard_ShortReal B)
{
  ClearHighlightBox (theGlCtx);
  if (myHighlightColor == NULL)
  {
    myHighlightColor = new TEL_COLOUR();
  }

  myHighlightColor->rgb[0] = R;
  myHighlightColor->rgb[1] = G;
  myHighlightColor->rgb[2] = B;
  myHighlightColor->rgb[3] = 1.F;
}

/*----------------------------------------------------------------------*/

void OpenGl_Structure::ClearHighlightColor (const Handle(OpenGl_Context)& theGlCtx)
{
  ClearHighlightBox(theGlCtx);
  delete myHighlightColor;
  myHighlightColor = NULL;
}

/*----------------------------------------------------------------------*/

void OpenGl_Structure::Connect (const OpenGl_Structure *AStructure)
{
  Disconnect (AStructure);
  myConnected.Append(AStructure);
}

/*----------------------------------------------------------------------*/

void OpenGl_Structure::Disconnect (const OpenGl_Structure *AStructure)
{
  OpenGl_ListOfStructure::Iterator its(myConnected);
  while (its.More())
  {
    // Check for the given structure
    if (its.Value() == AStructure)
    {
      myConnected.Remove(its);
      return;
    }
    its.Next();
  }
}

/*----------------------------------------------------------------------*/

OpenGl_Group * OpenGl_Structure::AddGroup ()
{
  // Create new group
  OpenGl_Group *g = new OpenGl_Group;
  myGroups.Append(g);
  return g;
}

/*----------------------------------------------------------------------*/

void OpenGl_Structure::RemoveGroup (const Handle(OpenGl_Context)& theGlCtx,
                                    const OpenGl_Group*           theGroup)
{
  for (OpenGl_ListOfGroup::Iterator anIter (myGroups); anIter.More(); anIter.Next())
  {
    // Check for the given group
    if (anIter.Value() == theGroup)
    {
      // Delete object
      OpenGl_Element::Destroy (theGlCtx, const_cast<OpenGl_Group*& > (anIter.ChangeValue()));
      myGroups.Remove (anIter);
      return;
    }
  }
}

/*----------------------------------------------------------------------*/

void OpenGl_Structure::Clear (const Handle(OpenGl_Context)& theGlCtx)
{
  // Release groups
  for (OpenGl_ListOfGroup::Iterator anIter (myGroups); anIter.More(); anIter.Next())
  {
    // Delete objects
    OpenGl_Element::Destroy (theGlCtx, const_cast<OpenGl_Group*& > (anIter.ChangeValue()));
  }
  myGroups.Clear();
}

/*----------------------------------------------------------------------*/

void OpenGl_Structure::Render (const Handle(OpenGl_Workspace) &AWorkspace) const
{
  // Process the structure only if visible
  if ( myNamedStatus & OPENGL_NS_HIDE )
    return;

  // Render named status
  const Standard_Integer named_status = AWorkspace->NamedStatus;
  AWorkspace->NamedStatus |= myNamedStatus;

  // Is rendering in ADD or IMMEDIATE mode?
  const Standard_Boolean isImmediate = (AWorkspace->NamedStatus & (OPENGL_NS_ADD | OPENGL_NS_IMMEDIATE)) != 0;

  // Apply local transformation
  GLint matrix_mode = 0;
  const OpenGl_Matrix *local_trsf = NULL;
  if (myTransformation)
  {
    if (isImmediate)
    {
      float mat16[16];
      call_util_transpose_mat (mat16, myTransformation->mat);
      glGetIntegerv (GL_MATRIX_MODE, &matrix_mode);
      glMatrixMode (GL_MODELVIEW);
      glPushMatrix ();
      glScalef (1.F, 1.F, 1.F);
      glMultMatrixf (mat16);
    }
    else
    {
      glMatrixMode (GL_MODELVIEW);
      glPushMatrix();

      local_trsf = AWorkspace->SetStructureMatrix(myTransformation);
    }
  }

  // Apply transform persistence
  const TEL_TRANSFORM_PERSISTENCE *trans_pers = NULL;
  if ( myTransPers && myTransPers->mode != 0 )
  {
    trans_pers = AWorkspace->ActiveView()->BeginTransformPersistence( myTransPers );
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
  if (myHighlightBox)
    myHighlightBox->Render( AWorkspace );

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

  // Render groups
  OpenGl_ListOfGroup::Iterator itg(myGroups);
  while (itg.More())
  {
    itg.Value()->Render(AWorkspace);
    itg.Next();
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
    AWorkspace->ActiveView()->BeginTransformPersistence( trans_pers );
  }

  // Restore local transformation
  if (myTransformation)
  {
    if (isImmediate)
    {
      glPopMatrix ();
      glMatrixMode (matrix_mode);
    }
    else
    {
      AWorkspace->SetStructureMatrix(local_trsf);

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
  ClearHighlightColor (theGlCtx);
}

// =======================================================================
// function : ReleaseGlResources
// purpose  :
// =======================================================================
void OpenGl_Structure::ReleaseGlResources (const Handle(OpenGl_Context)& theGlCtx)
{
  for (OpenGl_ListOfGroup::Iterator anIter (myGroups); anIter.More(); anIter.Next())
  {
    OpenGl_Group* aGroup = const_cast<OpenGl_Group*& > (anIter.ChangeValue());
    if (aGroup != NULL)
    {
      aGroup->Release (theGlCtx);
    }
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
  if (myHighlightBox != NULL)
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
