// Created on: 2011-09-20
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

#include <OpenGl_Trihedron.hxx>

#include <OpenGl_GlCore11.hxx>

#include <OpenGl_View.hxx>
#include <OpenGl_Workspace.hxx>

static const OpenGl_TextParam THE_LABEL_PARAMS =
{
  16, Graphic3d_HTA_LEFT, Graphic3d_VTA_BOTTOM
};

static const CALL_DEF_CONTEXTLINE myDefaultContextLine =
{
  1, //IsDef
  1, //IsSet
  { 1.F, 1.F, 1.F }, //Color
  Aspect_TOL_SOLID, //LineType
  1.F //Width
};

static const CALL_DEF_CONTEXTTEXT myDefaultContextText =
{
  1, //IsDef
  1, //IsSet
  "Courier", //Font
  0.3F, //Space
  1.0F, //Expan
  { 1.F, 1.F, 1.F }, //Color
  Aspect_TOST_NORMAL, //Style
  Aspect_TODT_NORMAL, //DisplayType
  { 1.F, 1.F, 1.F }, //ColorSubTitle
  0, //TextZoomable
  0.F, //TextAngle
  Font_FA_Regular //TextFontAspect
};

static TEL_COLOUR theXColor = {{ 1.F, 0.F, 0.F, 0.6F }};
static TEL_COLOUR theYColor = {{ 0.F, 1.F, 0.F, 0.6F }};
static TEL_COLOUR theZColor = {{ 0.F, 0.F, 1.F, 0.6F }};
static float theRatio = 0.8f;
static float theDiameter = 0.05f;
static int   theNbFacettes = 12;

// =======================================================================
// function : redraw
// purpose  :
// =======================================================================
void OpenGl_Trihedron::redraw (const Handle(OpenGl_Workspace)& theWorkspace) const
{
#if !defined(GL_ES_VERSION_2_0)
  const Standard_Real U = theWorkspace->ActiveView()->Height();
  const Standard_Real V = theWorkspace->ActiveView()->Width();

  Handle(OpenGl_Context) aContext = theWorkspace->GetGlContext();

  /* la taille des axes est 1 proportion (fixee a l'init du triedre) */
  /* de la dimension la plus petite de la window.                    */
  const GLdouble L = ( U < V ? U : V ) * myScale;

  /*
  * On inhibe les translations; on conserve les autres transformations.
  */

  aContext->WorldViewState.Push();
  aContext->ProjectionState.Push();

  /* on lit les matrices de transformation et de projection de la vue */
  /* pour annuler les translations (dernieres colonnes des matrices). */
  OpenGl_Mat4d aModelMatrix;
  aModelMatrix.Convert (aContext->WorldViewState.Current());

  OpenGl_Mat4d aProjMatrix;

  /* on annule la translation qui peut etre affectee a la vue */
  aModelMatrix.ChangeValue (0, 3) = 0.0;
  aModelMatrix.ChangeValue (1, 3) = 0.0;
  aModelMatrix.ChangeValue (2, 3) = 0.0;

  aProjMatrix.ChangeValue (0, 0) = 2.0 / U;
  aProjMatrix.ChangeValue (1, 0) = 0.0;
  aProjMatrix.ChangeValue (2, 0) = 0.0;
  aProjMatrix.ChangeValue (3, 0) = 0.0;

  aProjMatrix.ChangeValue (0, 1) = 0.0;
  aProjMatrix.ChangeValue (1, 1) = 2.0 / V;
  aProjMatrix.ChangeValue (2, 1) = 0.0;
  aProjMatrix.ChangeValue (3, 1) = 0.0;

  aProjMatrix.ChangeValue (0, 2) = 0.0;
  aProjMatrix.ChangeValue (1, 2) = 0.0;
  aProjMatrix.ChangeValue (2, 2) = -2.0 * 1e-7;
  aProjMatrix.ChangeValue (3, 2) = 0.0; 
   
  aProjMatrix.ChangeValue (0, 3) = 0.0;
  aProjMatrix.ChangeValue (1, 3) = 0.0;
  aProjMatrix.ChangeValue (2, 3) = 0.0;
  aProjMatrix.ChangeValue (3, 3) = 1.0;

  /*
  * Positionnement de l'origine du triedre selon le choix de l'init
  */

  /* on fait uniquement une translation de l'origine du Triedre */

  switch (myPos)
  {
    case Aspect_TOTP_LEFT_LOWER :
    {
      OpenGl_Utils::Translate (aProjMatrix,
        -0.5 * U + L, -0.5 * V + L, 0.0);
    }
    break;

    case Aspect_TOTP_LEFT_UPPER :
    {
      OpenGl_Utils::Translate (aProjMatrix,
        -0.5 * U + L, 0.5 * V - L - L/3.0, 0.0);
    }
    break;

    case Aspect_TOTP_RIGHT_LOWER :
    {
      OpenGl_Utils::Translate (aProjMatrix,
        0.5 * U - L - L/3.0, -0.5 * V + L, 0.0);
    }
    break;

    case Aspect_TOTP_RIGHT_UPPER :
    {
      OpenGl_Utils::Translate (aProjMatrix,
        0.5 * U - L - L/3.0, 0.5 * V - L - L/3.0, 0.0);
    }
    break;

    //case Aspect_TOTP_CENTER :
    default :
      break;
  }

  aContext->ProjectionState.SetCurrent<Standard_Real> (aProjMatrix);
  aContext->WorldViewState.SetCurrent<Standard_Real> (aModelMatrix);
  aContext->ApplyProjectionMatrix();
  aContext->ApplyWorldViewMatrix();

  /*
  * Creation du triedre
  */

  /* Fotis Sioutis 2007-11-14 15:06
  I have also seen in previous posts that the view trihedron in V3d_WIREFRAME mode
  changes colors depending on the state of the view. This behaviour can be easily
  corrected by altering call_triedron_redraw function in OpenGl_triedron.c of TKOpengl.
  The only change needed is to erase glDisable(GL_LIGHTING) that is called before the
  Axis name drawing and move this function call just before the initial axis drawing.
  Below is the code portion with the modification.I don't know if this is considered to
  be a bug but anyway i believe it might help some of you out there.*/
  glDisable(GL_LIGHTING);

  /* Position de l'origine */
  const GLdouble TriedronOrigin[3] = { 0.0, 0.0, 0.0 };

  /* Position des Axes */
  GLdouble TriedronAxeX[3] = { 1.0, 0.0, 0.0 };
  GLdouble TriedronAxeY[3] = { 0.0, 1.0, 0.0 };
  GLdouble TriedronAxeZ[3] = { 0.0, 0.0, 1.0 };
  TriedronAxeX[0] = L ;
  TriedronAxeY[1] = L ;
  TriedronAxeZ[2] = L ;

  /* dessin des axes */
  glBegin(GL_LINES);
  glVertex3dv( TriedronOrigin );
  glVertex3dv( TriedronAxeX );

  glVertex3dv( TriedronOrigin );
  glVertex3dv( TriedronAxeY );

  glVertex3dv( TriedronOrigin );
  glVertex3dv( TriedronAxeZ );
  glEnd();

  /* fleches au bout des axes (= cones de la couleur demandee) */
  const GLdouble l = 0.75*L; /* distance a l'origine */
  const GLdouble rayon = L/30. ; /* rayon de la base du cone */
  const int NbFacettes = 12; /* le cone sera compose de 12 facettes triangulaires */
  const double Angle = 2. * M_PI/ NbFacettes;

  int      ii;
  GLdouble TriedronCoord[3] = { 1.0, 0.0, 0.0 };

  /* solution FILAIRE des cones au bout des axes : une seule ligne */
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  /* (la couleur est deja initialisee dans AspectLine) */
  /* FIN de la solution FILAIRE CHOISIE pour les cones des axes */

  /* fleche en X */
  glBegin(GL_TRIANGLE_FAN);
  glVertex3dv( TriedronAxeX );
  TriedronCoord[0] = l;
  ii = NbFacettes;
  while (ii >= 0 ) {
    TriedronCoord[1] = rayon * sin(ii * Angle);
    TriedronCoord[2] = rayon * cos(ii * Angle);
    glVertex3dv( TriedronCoord );
    ii--;
  }
  glEnd();

  /* fleche en Y */
  glBegin(GL_TRIANGLE_FAN);
  glVertex3dv( TriedronAxeY );
  TriedronCoord[1] = l;
  ii = NbFacettes;
  while (ii >= 0 ) {
    TriedronCoord[0] = rayon * cos(ii * Angle);
    TriedronCoord[2] = rayon * sin(ii * Angle);
    glVertex3dv( TriedronCoord );
    ii--;
  }
  glEnd();

  /* fleche en Z */
  glBegin(GL_TRIANGLE_FAN);
  glVertex3dv( TriedronAxeZ );
  TriedronCoord[2] = l;
  ii = NbFacettes;
  while (ii >= 0 ) {
    TriedronCoord[0] = rayon * sin(ii * Angle);
    TriedronCoord[1] = rayon * cos(ii * Angle);
    glVertex3dv( TriedronCoord );
    ii--;
  }
  glEnd();

  /* dessin de l'origine */
  TriedronCoord[2] = 0.0 ;
  ii = 24 ;
  const double Angle1 = 2. * M_PI/ ii;
  glBegin(GL_LINE_LOOP);
  while (ii >= 0 ) {
    TriedronCoord[0] = rayon * sin(ii * Angle1);
    TriedronCoord[1] = rayon * cos(ii * Angle1);
    glVertex3dv( TriedronCoord );
    ii--;
  }
  glEnd();

  // draw axes labels
  myLabelX.SetPosition (OpenGl_Vec3(float(L + rayon),    0.0f,                   float(-rayon)));
  myLabelY.SetPosition (OpenGl_Vec3(float(rayon),        float(L + 3.0 * rayon), float(2.0 * rayon)));
  myLabelZ.SetPosition (OpenGl_Vec3(float(-2.0 * rayon), float(0.5 * rayon),     float(L + 3.0 * rayon)));
  myLabelX.Render (theWorkspace);
  myLabelY.Render (theWorkspace);
  myLabelZ.Render (theWorkspace);

  /*
  * restauration du contexte des matrices
  */

  aContext->WorldViewState.Pop();
  aContext->ProjectionState.Pop();
  aContext->ApplyProjectionMatrix();
#endif
}

// =======================================================================
// function : redrawZBuffer
// purpose  :
// =======================================================================
void OpenGl_Trihedron::redrawZBuffer (const Handle(OpenGl_Workspace)& theWorkspace) const
{
  Handle(OpenGl_Context)     aContext = theWorkspace->GetGlContext();
  const Handle(OpenGl_View)& aView    = theWorkspace->ActiveView();

  OpenGl_Mat4d aModelMatrix, aProjMatrix;
  aContext->WorldViewState.Push();
  aContext->ProjectionState.Push();
  aModelMatrix.Convert (aContext->WorldViewState.Current());

  GLdouble U = 1.0;
  GLdouble V = 1.0;
  if (aView->Height() < aView->Width())
  {
    V = aView->Width() / aView->Height();
  }
  else
  {
    U = aView->Height() / aView->Width();
  }

  GLdouble aScale = myScale;

  // Annulate translation matrix
  aModelMatrix.ChangeValue (0, 3) = 0.0;
  aModelMatrix.ChangeValue (1, 3) = 0.0;
  aModelMatrix.ChangeValue (2, 3) = 0.0;

  aProjMatrix.ChangeValue (0, 0) = 2.0 / U;
  aProjMatrix.ChangeValue (1, 0) = 0.0;
  aProjMatrix.ChangeValue (2, 0) = 0.0;
  aProjMatrix.ChangeValue (3, 0) = 0.0;

  aProjMatrix.ChangeValue (0, 1) = 0.0;
  aProjMatrix.ChangeValue (1, 1) = 2.0 / V;
  aProjMatrix.ChangeValue (2, 1) = 0.0;
  aProjMatrix.ChangeValue (3, 1) = 0.0;

  aProjMatrix.ChangeValue (0, 2) = 0.0;
  aProjMatrix.ChangeValue (1, 2) = 0.0;
  aProjMatrix.ChangeValue (2, 2) = -2.0 * 1e-2;
  aProjMatrix.ChangeValue (3, 2) = 0.0;

  aProjMatrix.ChangeValue (0, 3) = 0.0;
  aProjMatrix.ChangeValue (1, 3) = 0.0;
  aProjMatrix.ChangeValue (2, 3) = 0.0;
  aProjMatrix.ChangeValue (3, 3) = 1.0;

  // Define position in the view
  switch (myPos)
  {
    case Aspect_TOTP_LEFT_LOWER:
    {
      OpenGl_Utils::Translate<Standard_Real> (aProjMatrix,
        -0.5 * U + aScale, -0.5 * V + aScale, 0.0);
      break;
    }
    case Aspect_TOTP_LEFT_UPPER:
    {
      OpenGl_Utils::Translate<Standard_Real> (aProjMatrix,
        -0.5 * U + aScale, 0.5 * V - aScale - aScale / 3.0, 0.0);
      break;
    }
    case Aspect_TOTP_RIGHT_LOWER:
    {
      OpenGl_Utils::Translate<Standard_Real> (aProjMatrix,
        0.5 * U - aScale - aScale / 3.0, -0.5 * V + aScale, 0.0);
      break;
    }
    case Aspect_TOTP_RIGHT_UPPER:
    {
      OpenGl_Utils::Translate<Standard_Real> (aProjMatrix,
        0.5 * U - aScale - aScale / 3.0, 0.5 * V - aScale - aScale / 3.0, 0.0);
      break;
    }
    //case Aspect_TOTP_CENTER:
    default:
      break;
  }
  aScale *= myRatio;

  aContext->ProjectionState.SetCurrent<Standard_Real> (aProjMatrix);
  aContext->ApplyProjectionMatrix();

  const OpenGl_AspectLine* anAspectLine = theWorkspace->AspectLine (Standard_True);
  const TEL_COLOUR&        aLineColor   = anAspectLine->Color();

  // Create the trihedron
  const Standard_Real THE_CYLINDER_LENGTH = 0.75;
  const GLdouble aCylinderLength  = aScale * THE_CYLINDER_LENGTH;
  const GLdouble aCylinderDiametr = aScale * myDiameter;
  const GLdouble aConeDiametr     = aCylinderDiametr * 2.0;
  const GLdouble aConeLength      = aScale * (1.0 - THE_CYLINDER_LENGTH);

  // Position des Axes
  GLdouble aTriedronAxeX[3] = { aScale, 0.0,    0.0 };
  GLdouble aTriedronAxeY[3] = { 0.0,    aScale, 0.0 };
  if (!myDisk.IsDefined())
  {
    myDisk.Init (static_cast<GLfloat> (aCylinderDiametr),
                 static_cast<GLfloat> (aConeDiametr),
                 myNbFacettes, 1);
  }

  if (!mySphere.IsDefined())
  {
    mySphere.Init (static_cast<GLfloat> (aCylinderDiametr * 2.0), myNbFacettes, myNbFacettes);
  }

  if (!myCone.IsDefined())
  {
    myCone.Init (static_cast<GLfloat> (aConeDiametr), 0.0f, static_cast<GLfloat> (aConeLength), myNbFacettes, 1);
  }

  if (!myCylinder.IsDefined())
  {
    myCylinder.Init (static_cast<GLfloat> (aCylinderDiametr),
                     static_cast<GLfloat> (aCylinderDiametr),
                     static_cast<GLfloat> (aCylinderLength),
                     myNbFacettes, 1);
  }

  GLboolean wasDepthMaskEnabled = GL_FALSE;
  GLint aDepthFuncBack = 0, aCullFaceModeBack = GL_BACK;
  const GLboolean wasDepthEnabled    = aContext->core11fwd->glIsEnabled (GL_DEPTH_TEST);
  const GLboolean wasCullFaceEnabled = aContext->core11fwd->glIsEnabled (GL_CULL_FACE);
  aContext->core11fwd->glGetIntegerv (GL_DEPTH_FUNC,      &aDepthFuncBack);
  aContext->core11fwd->glGetIntegerv (GL_CULL_FACE_MODE,  &aCullFaceModeBack);
  aContext->core11fwd->glGetBooleanv (GL_DEPTH_WRITEMASK, &wasDepthMaskEnabled);
  if (!wasDepthEnabled)
  {
    aContext->core11fwd->glEnable (GL_DEPTH_TEST);
    aContext->core11fwd->glClear (GL_DEPTH_BUFFER_BIT);
  }
  if (!wasDepthMaskEnabled)
  {
    aContext->core11fwd->glDepthMask (GL_TRUE);
  }
  aContext->core11fwd->glCullFace (GL_BACK);
  if (!wasCullFaceEnabled)
  {
    aContext->core11fwd->glEnable (GL_CULL_FACE);
  }

  OpenGl_AspectFace anAspectC;
  OpenGl_AspectFace anAspectX;
  OpenGl_AspectFace anAspectY;
  OpenGl_AspectFace anAspectZ;
  memcpy (anAspectX.ChangeIntFront().matcol.rgb,   myXColor.rgb, sizeof (TEL_COLOUR));
  memcpy (anAspectY.ChangeIntFront().matcol.rgb,   myYColor.rgb, sizeof (TEL_COLOUR));
  memcpy (anAspectZ.ChangeIntFront().matcol.rgb,   myZColor.rgb, sizeof (TEL_COLOUR));
  memcpy (anAspectC.ChangeIntFront().matcol.rgb, aLineColor.rgb, sizeof (TEL_COLOUR));
  for (Standard_Integer aPass = 0; aPass < 2; ++aPass)
  {
    OpenGl_Mat4d aModelViewX (aModelMatrix);
    OpenGl_Mat4d aModelViewY (aModelMatrix);
    OpenGl_Mat4d aModelViewZ (aModelMatrix);
    aContext->core11fwd->glDepthFunc (aPass == 0 ? GL_ALWAYS : GL_LEQUAL);

    const OpenGl_AspectFace* anOldAspect = theWorkspace->SetAspectFace (&anAspectC);

    // Origin
    aContext->WorldViewState.SetCurrent<Standard_Real> (aModelMatrix);
    aContext->ApplyWorldViewMatrix();
    mySphere.Render (theWorkspace);

    // Z axis
    theWorkspace->SetAspectFace (&anAspectZ);
    myCylinder.Render (theWorkspace);
    OpenGl_Utils::Translate (aModelViewZ, 0.0, 0.0, aScale * THE_CYLINDER_LENGTH);
    aContext->WorldViewState.SetCurrent<Standard_Real> (aModelViewZ);
    aContext->ApplyWorldViewMatrix();
    myDisk.Render (theWorkspace);
    myCone.Render (theWorkspace);

    // X axis
    theWorkspace->SetAspectFace (&anAspectX);
    OpenGl_Utils::Rotate (aModelViewX, 90.0, aTriedronAxeY[0], aTriedronAxeY[1], aTriedronAxeY[2]);
    aContext->WorldViewState.SetCurrent<Standard_Real> (aModelViewX);
    aContext->ApplyWorldViewMatrix();
    myCylinder.Render (theWorkspace);
    OpenGl_Utils::Translate (aModelViewX, 0.0, 0.0, aScale * THE_CYLINDER_LENGTH);
    aContext->WorldViewState.SetCurrent<Standard_Real> (aModelViewX);
    aContext->ApplyWorldViewMatrix();
    myDisk.Render (theWorkspace);
    myCone.Render (theWorkspace);

    // Y axis
    theWorkspace->SetAspectFace (&anAspectY);
    OpenGl_Utils::Rotate (aModelViewY, -90.0, aTriedronAxeX[0], aTriedronAxeX[1], aTriedronAxeX[2]);
    aContext->WorldViewState.SetCurrent<Standard_Real> (aModelViewY);
    aContext->ApplyWorldViewMatrix();
    myCylinder.Render (theWorkspace);
    OpenGl_Utils::Translate (aModelViewY, 0.0, 0.0, aScale * THE_CYLINDER_LENGTH);
    aContext->WorldViewState.SetCurrent<Standard_Real> (aModelViewY);
    aContext->ApplyWorldViewMatrix();
    myDisk.Render (theWorkspace);
    myCone.Render (theWorkspace);

    theWorkspace->SetAspectFace (anOldAspect);
  }

  if (!wasDepthEnabled)
  {
    aContext->core11fwd->glDisable (GL_DEPTH_TEST);
  }
  if (!wasDepthMaskEnabled)
  {
    aContext->core11fwd->glDepthMask (GL_FALSE);
  }
  if (!wasCullFaceEnabled)
  {
    aContext->core11fwd->glDisable (GL_CULL_FACE);
  }
  aContext->core11fwd->glCullFace (aCullFaceModeBack);

  // Always write the text
  aContext->core11fwd->glDepthFunc (GL_ALWAYS);

  // draw axes labels
  const GLdouble rayon = aScale / 30.0;
  myLabelX.SetPosition (OpenGl_Vec3(float(aScale + 2.0 * rayon), 0.0f,                        float(-rayon)));
  myLabelY.SetPosition (OpenGl_Vec3(float(rayon),                float(aScale + 3.0 * rayon), float(2.0 * rayon)));
  myLabelZ.SetPosition (OpenGl_Vec3(float(-2.0 * rayon),         float(0.5 * rayon),          float(aScale + 3.0 * rayon)));
  aContext->WorldViewState.SetCurrent<Standard_Real> (aModelMatrix);
  aContext->ApplyWorldViewMatrix();
  myLabelX.Render (theWorkspace);
  myLabelY.Render (theWorkspace);
  myLabelZ.Render (theWorkspace);

  aContext->core11fwd->glDepthFunc (aDepthFuncBack);

  aContext->WorldViewState.Pop();
  aContext->ProjectionState.Pop();
  aContext->ApplyProjectionMatrix();
}

// =======================================================================
// function : OpenGl_Trihedron
// purpose  :
// =======================================================================
OpenGl_Trihedron::OpenGl_Trihedron (const Aspect_TypeOfTriedronPosition thePosition,
                                    const Quantity_NameOfColor          theColor,
                                    const Standard_Real                 theScale,
                                    const Standard_Boolean              theAsWireframe)
: myPos (thePosition),
  myScale (theScale),
  myIsWireframe (theAsWireframe),
  myLabelX ("X", OpenGl_Vec3(1.0f, 0.0f, 0.0f), THE_LABEL_PARAMS),
  myLabelY ("Y", OpenGl_Vec3(0.0f, 1.0f, 0.0f), THE_LABEL_PARAMS),
  myLabelZ ("Z", OpenGl_Vec3(0.0f, 0.0f, 1.0f), THE_LABEL_PARAMS)
{
  Standard_Real R,G,B;
  Quantity_Color aColor (theColor);
  aColor.Values (R, G, B, Quantity_TOC_RGB);

  CALL_DEF_CONTEXTLINE aLineAspect = myDefaultContextLine;
  aLineAspect.Color.r = (float)R;
  aLineAspect.Color.g = (float)G;
  aLineAspect.Color.b = (float)B;
  myAspectLine.SetAspect (aLineAspect);

  CALL_DEF_CONTEXTTEXT aTextAspect = myDefaultContextText;
  aTextAspect.Color.r = (float)R;
  aTextAspect.Color.g = (float)G;
  aTextAspect.Color.b = (float)B;
  myAspectText.SetAspect (aTextAspect);

  myXColor = theXColor;
  myYColor = theYColor;
  myZColor = theZColor;

  myRatio = theRatio;
  myDiameter = theDiameter;
  myNbFacettes = theNbFacettes;
}

// =======================================================================
// function : ~OpenGl_Trihedron
// purpose  :
// =======================================================================
OpenGl_Trihedron::~OpenGl_Trihedron()
{
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_Trihedron::Release (OpenGl_Context* theCtx)
{
  myLabelX.Release (theCtx);
  myLabelY.Release (theCtx);
  myLabelZ.Release (theCtx);
  myAspectLine.Release (theCtx);
  myAspectText.Release (theCtx);
  myCone    .Release (theCtx);
  myDisk    .Release (theCtx);
  mySphere  .Release (theCtx);
  myCylinder.Release (theCtx);
}

// =======================================================================
// function : Render
// purpose  :
// =======================================================================
void OpenGl_Trihedron::Render (const Handle(OpenGl_Workspace)& theWorkspace) const
{
  const OpenGl_AspectLine* aPrevAspectLine = theWorkspace->SetAspectLine (&myAspectLine);
  const OpenGl_AspectText* aPrevAspectText = theWorkspace->SetAspectText (&myAspectText);

  /* check if GL_LIGHTING should be disabled
  no enabling 'cause it will be done (if necessary: kinda Polygon types )
  during redrawing structures
  */
  if (!theWorkspace->UseGLLight())
  {
  #if !defined(GL_ES_VERSION_2_0)
    glDisable (GL_LIGHTING);
  #endif
  }

  const Handle(OpenGl_Texture) aPrevTexture = theWorkspace->DisableTexture();

  /* affichage du Triedre Non Zoomable */
  theWorkspace->ActiveView()->EndTransformPersistence (theWorkspace->GetGlContext());

  if (myIsWireframe)
  {
    redraw (theWorkspace);
  }
  else
  {
    redrawZBuffer (theWorkspace);
  }

  // restore aspects
  if (!aPrevTexture.IsNull())
  {
    theWorkspace->EnableTexture (aPrevTexture);
  }

  theWorkspace->SetAspectText (aPrevAspectText);
  theWorkspace->SetAspectLine (aPrevAspectLine);
}

/*----------------------------------------------------------------------*/
//call_ztriedron_setup
void OpenGl_Trihedron::Setup (const Quantity_NameOfColor XColor, const Quantity_NameOfColor YColor, const Quantity_NameOfColor ZColor,
                             const Standard_Real SizeRatio, const Standard_Real AxisDiametr, const Standard_Integer NbFacettes)
{
  Standard_Real R,G,B;

  Quantity_Color(XColor).Values(R, G, B, Quantity_TOC_RGB);
  theXColor.rgb[0] = float (R);
  theXColor.rgb[1] = float (G);
  theXColor.rgb[2] = float (B);

  Quantity_Color(YColor).Values(R, G, B, Quantity_TOC_RGB);
  theYColor.rgb[0] = float (R);
  theYColor.rgb[1] = float (G);
  theYColor.rgb[2] = float (B);

  Quantity_Color(ZColor).Values(R, G, B, Quantity_TOC_RGB);
  theZColor.rgb[0] = float (R);
  theZColor.rgb[1] = float (G);
  theZColor.rgb[2] = float (B);

  theRatio = float (SizeRatio);
  theDiameter = float (AxisDiametr);
  theNbFacettes = NbFacettes;
}
