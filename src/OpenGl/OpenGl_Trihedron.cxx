// Created on: 2011-09-20
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

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <OpenGl_TextureBox.hxx>

#include <InterfaceGraphic_Graphic3d.hxx>  /* pour CALL_DEF_STRUCTURE */
#include <InterfaceGraphic_Aspect.hxx>  /* pour CALL_DEF_VIEW  */
#include <InterfaceGraphic_Visual3d.hxx>

#include <OpenGl_transform_persistence.hxx>

#include <OpenGl_Workspace.hxx>
#include <OpenGl_View.hxx>
#include <OpenGl_Trihedron.hxx>

#include <GL/glu.h> // gluNewQuadric()

IMPLEMENT_STANDARD_HANDLE(OpenGl_Trihedron,MMgt_TShared)
IMPLEMENT_STANDARD_RTTIEXT(OpenGl_Trihedron,MMgt_TShared)

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
  OSD_FA_Regular //TextFontAspect
};

/*----------------------------------------------------------------------*/
/*
* Variables statiques
*/

/* Default parameters for ZBUFFER triheron */
static TEL_COLOUR theXColor = {{ 1.F, 0.F, 0.F, 0.6F }};
static TEL_COLOUR theYColor = {{ 0.F, 1.F, 0.F, 0.6F }};
static TEL_COLOUR theZColor = {{ 0.F, 0.F, 1.F, 0.6F }};
static float theRatio = 0.8f;
static float theDiameter = 0.05f;
static int   theNbFacettes = 12;

/*----------------------------------------------------------------------*/

/*
* affichage d'un triedre non zoomable a partir des index dans les tables
* des structures non zoomables.
*
* Triedre = Objet non Zoomable :
* on recalcule ses dimensions et son origine en fonction de la taille
* de la fenetre; on positionne selon le choix de l'init;
* et on inhibe seulement les translations.
*
*/

//call_triedron_redraw
void OpenGl_Trihedron::Redraw (const Handle(OpenGl_Workspace) &AWorkspace) const
{
  const Standard_Real U = AWorkspace->ActiveView()->Height();
  const Standard_Real V = AWorkspace->ActiveView()->Width();

  /* la taille des axes est 1 proportion (fixee a l'init du triedre) */
  /* de la dimension la plus petite de la window.                    */ 
  const GLdouble L = ( U < V ? U : V ) * myScale;

  /*
  * On inhibe les translations; on conserve les autres transformations.
  */

  /* on lit les matrices de transformation et de projection de la vue */
  /* pour annuler les translations (dernieres colonnes des matrices). */
  GLdouble modelMatrix[4][4];
  glGetDoublev( GL_MODELVIEW_MATRIX,  (GLdouble *) modelMatrix );
  GLdouble projMatrix[4][4];
  glGetDoublev( GL_PROJECTION_MATRIX, (GLdouble *) projMatrix );

  /* on annule la translation qui peut etre affectee a la vue */
  modelMatrix[3][0] = 0.;
  modelMatrix[3][1] = 0.;
  modelMatrix[3][2] = 0.;
  projMatrix[3][0] = 0.;
  projMatrix[3][1] = 0.;
  projMatrix[3][2] = 0.;

  /* sauvegarde du contexte des matrices avant chargement */
  glMatrixMode (GL_MODELVIEW);
  glPushMatrix ();
  glLoadMatrixd( (GLdouble *) modelMatrix );
  glMatrixMode ( GL_PROJECTION );
  glPushMatrix ();
  glLoadMatrixd( (GLdouble *) projMatrix );

  /*
  * Positionnement de l'origine du triedre selon le choix de l'init
  */

  /* on fait uniquement une translation de l'origine du Triedre */

  switch (myPos)
  {
    case Aspect_TOTP_LEFT_LOWER :
      glTranslated( -0.5*U + L , -0.5*V + L , 0. );
      break;

    case Aspect_TOTP_LEFT_UPPER :
      glTranslated( -0.5*U + L , +0.5*V - L -L/3., 0. );
      break;

    case Aspect_TOTP_RIGHT_LOWER :
      glTranslated( 0.5*U - L -L/3. , -0.5*V + L , 0. );
      break;

    case Aspect_TOTP_RIGHT_UPPER :
      glTranslated( 0.5*U - L -L/3. , +0.5*V - L -L/3. , 0. );
      break;

    //case Aspect_TOTP_CENTER :
    default :
      break;
  }

  /* 
  * Creation du triedre 
  */
  const OpenGl_AspectLine *AspectLine = AWorkspace->AspectLine( Standard_True );

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

  /* 
  * Noms des axes et de l'origine
  */
  const OpenGl_AspectText *AspectText = AWorkspace->AspectText( Standard_True );
  glColor3fv (AspectText->Color().rgb);

  AWorkspace->RenderText (L"X", 0, float(L + rayon),    0.0f,                   float(-rayon));
  AWorkspace->RenderText (L"Y", 0, float(rayon),        float(L + 3.0 * rayon), float(2.0 * rayon));
  AWorkspace->RenderText (L"Z", 0, float(-2.0 * rayon), float(0.5 * rayon),     float(L + 3.0 * rayon));

  /* 
  * restauration du contexte des matrices
  */
  glMatrixMode (GL_PROJECTION);
  glPopMatrix ();
  glMatrixMode (GL_MODELVIEW);
  glPopMatrix ();
}


/*******************************************************
*  Draws ZBUFFER trihedron mode
*******************************************************/
//call_zbuffer_triedron_redraw
void OpenGl_Trihedron::RedrawZBuffer (const Handle(OpenGl_Workspace) &AWorkspace) const
{
  const Standard_Real U = AWorkspace->ActiveView()->Height();
  const Standard_Real V = AWorkspace->ActiveView()->Width();

  GLdouble modelMatrix[4][4];
  glGetDoublev( GL_MODELVIEW_MATRIX,  (GLdouble *) modelMatrix );
  GLdouble projMatrix[4][4];
  glGetDoublev( GL_PROJECTION_MATRIX, (GLdouble *) projMatrix );

  /* Check position in the ViewPort */
  /* PCD 29/09/2008 */
  /* Simple code modification recommended by Fotis Sioutis and Peter Dolbey  */
  /* to remove the irritating default behaviour of triedrons using V3d_ZBUFFER   */
  /* which causes the glyph to jump around the screen when the origin moves offscreen. */
  GLboolean isWithinView = GL_FALSE;

  /* la taille des axes est 1 proportion (fixee a l'init du triedre) */
  /* de la dimension la plus petite de la window.                    */ 
  GLdouble L = ( U < V ? U : V ) * myScale;

  if (!isWithinView)
  {
    /* Annulate translation matrix */
    modelMatrix[3][0] = 0.;
    modelMatrix[3][1] = 0.;
    modelMatrix[3][2] = 0.;
    projMatrix[3][0] = 0.;
    projMatrix[3][1] = 0.;
    projMatrix[3][2] = 0.;

    /* save matrix */
    glMatrixMode (GL_MODELVIEW);
    glPushMatrix ();
    //glLoadIdentity ();
    glLoadMatrixd( (GLdouble *) modelMatrix);
    glMatrixMode ( GL_PROJECTION );
    glPushMatrix ();
    //glLoadIdentity();
    glLoadMatrixd( (GLdouble *) projMatrix);

    /*
    * Define position in the view
    */
    switch (myPos)
    {
      case Aspect_TOTP_LEFT_LOWER : 
        glTranslated( -0.5*U + L , -0.5*V + L , 0. );
        break;

      case Aspect_TOTP_LEFT_UPPER :
        glTranslated( -0.5*U + L , +0.5*V - L -L/3. , 0. );
        break;

      case Aspect_TOTP_RIGHT_LOWER :
        glTranslated( 0.5*U - L -L/3. , -0.5*V + L , 0. );
        break;

      case Aspect_TOTP_RIGHT_UPPER :
        glTranslated( 0.5*U - L -L/3. , +0.5*V - L -L/3. , 0. );
        break;

      //case Aspect_TOTP_CENTER :
      default :
        break;
    }
    L *= myRatio;
  }

  const OpenGl_AspectLine *AspectLine = AWorkspace->AspectLine( Standard_True );
  const TEL_COLOUR &aLineColor = AspectLine->Color();

  /* 
  * Creation the trihedron
  */
#define CYLINDER_LENGTH   0.75f

  const GLuint startList = glGenLists(4);
  GLUquadricObj* aQuadric = gluNewQuadric();

  const GLboolean aIsDepthEnabled = glIsEnabled(GL_DEPTH_TEST);

  GLboolean aIsDepthMaskEnabled;
  /*PCD 02/07/07   */
  /* GL_DEPTH_WRITEMASK is not a valid argument to glIsEnabled, the  */
  /* original code is shown to be broken when run under an OpenGL debugger  */
  /* like GLIntercept. This is the correct way to retrieve the mask value.  */
  glGetBooleanv(GL_DEPTH_WRITEMASK, &aIsDepthMaskEnabled); 

  const GLdouble aCylinderLength = L * CYLINDER_LENGTH;
  const GLdouble aCylinderDiametr = L * myDiameter;
  const GLdouble aConeDiametr = aCylinderDiametr * 2.;
  const GLdouble aConeLength = L * (1 - CYLINDER_LENGTH);
  /* Correct for owerlapping */
  /*    aCylinderLength += aConeLength - 1.2*aCylinderDiametr*aConeLength/aConeDiametr;*/

  /* Create cylinder for axis */
  gluQuadricDrawStyle(aQuadric, GLU_FILL); /* smooth shaded */
  gluQuadricNormals(aQuadric, GLU_FLAT);
  /* Axis */
  glNewList(startList, GL_COMPILE);
  gluCylinder(aQuadric, aCylinderDiametr, aCylinderDiametr, aCylinderLength, myNbFacettes, 1);
  glEndList();
  /* Cone */
  glNewList(startList + 1, GL_COMPILE);
  gluCylinder(aQuadric, aConeDiametr, 0., aConeLength, myNbFacettes, 1);
  glEndList();
  /* Central sphere */
  glNewList(startList + 2, GL_COMPILE);
  gluSphere(aQuadric, aCylinderDiametr * 2., myNbFacettes, myNbFacettes);
  glEndList();
  /* End disk */
  gluQuadricOrientation(aQuadric,GLU_INSIDE); /*szv*/
  glNewList(startList + 3, GL_COMPILE);
  gluDisk(aQuadric, aCylinderDiametr, aConeDiametr, myNbFacettes, 1/*szv:2*/);
  glEndList();

  /* Store previous attributes */
  glPushAttrib(GL_LIGHTING_BIT | GL_POLYGON_BIT);
  glEnable(GL_LIGHTING);

  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);

  /*Fotis Sioutis | 2008-01-21 10:55
  In the function call_zbuffer_triedron_redraw of TKOpengl, 
  the z buffered trihedron changes colors in case there 
  is an object in the scene that has an explicit material 
  attached to it.In the trihedron display loop, 
  GL_COLOR_MATERIAL is enabled, but only the GL_DIFFUSE 
  parameter is utilized in glColorMaterial(...).
  This causes the last ambient,specular and emission values 
  used, to stay at the stack and applied to the trihedron
  (which causes the color change).
  A fix is proposed , to change GL_DIFFUSE to 
  GL_AMBIENT_AND_DIFFUSE in glColorMaterial call in 
  line 946.The above of course will leave unchanged 
  the SPECULAR and EMISSION values.
  Another proposal which would fix 100% the problem 
  is to use glMaterial instead of glColor on the trihedron 
  drawing loop.               */
  const GLfloat aNULLColor[] = { 0.f, 0.f, 0.f, 0.f }; /* FS 21/01/08 */
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, aNULLColor);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, aNULLColor);
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, aNULLColor);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.f);

  glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);

  if (!aIsDepthEnabled)  {
    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);
  }

  if (!aIsDepthMaskEnabled)  {
    /* This is how the depthmask needs to be re-enabled...*/
    glDepthMask(GL_TRUE);
    /* ...and not this stuff below */
  }

  /* Position des Axes */
  GLdouble TriedronAxeX[3] = { 1.0, 0.0, 0.0 };
  GLdouble TriedronAxeY[3] = { 0.0, 1.0, 0.0 };
  GLdouble TriedronAxeZ[3] = { 0.0, 0.0, 1.0 };
  TriedronAxeX[0] = L;
  TriedronAxeY[1] = L;
  TriedronAxeZ[2] = L;

  glMatrixMode(GL_MODELVIEW);

  /* PCD 17/06/07  */
  GLint df;
  glGetIntegerv (GL_DEPTH_FUNC, &df); 

  int i;
  for (i = 0; i < 2; i++) /* PCD 11/02/08 Two pass method */
  {
    if (i == 0) /*  First pass  */
    {                          
      glDepthFunc(GL_ALWAYS); 
    }
    else
    {
      glDepthFunc(GL_LEQUAL); 
    }

    glPushMatrix();
    glPushMatrix();
    glPushMatrix();

    glColor3fv(aLineColor.rgb);
    glCallList(startList+2);

    // Z axis
    glColor4fv(myZColor.rgb);
    glCallList(startList);
    glTranslated(0, 0, L * CYLINDER_LENGTH);
    glCallList(startList + 3);
    glCallList(startList + 1);
    glPopMatrix();    

    // X axis
    glRotated(90.0, TriedronAxeY[0], TriedronAxeY[1], TriedronAxeY[2]);
    glColor4fv(myXColor.rgb);
    glCallList(startList);
    glTranslated(0, 0, L * CYLINDER_LENGTH);
    glCallList(startList + 3);
    glCallList(startList + 1);
    glPopMatrix();    

    // Y axis
    glRotated(-90.0, TriedronAxeX[0], TriedronAxeX[1], TriedronAxeX[2]);
    glColor4fv(myYColor.rgb);
    glCallList(startList);
    glTranslated(0, 0, L * CYLINDER_LENGTH);
    glCallList(startList + 3);
    glCallList(startList + 1);
    glPopMatrix();
  }

  if (!aIsDepthEnabled) 
    glDisable(GL_DEPTH_TEST);

  if (!aIsDepthMaskEnabled)
    glDepthMask(GL_FALSE);

  glDisable(GL_CULL_FACE);
  glDisable(GL_COLOR_MATERIAL);

  gluDeleteQuadric(aQuadric);
  glColor3fv (aLineColor.rgb);

  /* Always write the text */
  glDepthFunc(GL_ALWAYS); 

  glPopAttrib();

  /* fleches au bout des axes (= cones de la couleur demandee) */
  //const GLdouble l = 0.75*L; /* distance a l'origine */
  const GLdouble rayon = L/30. ; /* rayon de la base du cone */
  //const double Angle = 2. * M_PI/ myNbFacettes;

  glDeleteLists(startList, 4); 

  glDisable(GL_LIGHTING);

  /* 
  * origine names
  */
  const OpenGl_AspectText *AspectText = AWorkspace->AspectText( Standard_True );
  glColor3fv (AspectText->Color().rgb);

  AWorkspace->RenderText (L"X", 0, float(L + rayon),    0.0f,                   float(-rayon));
  AWorkspace->RenderText (L"Y", 0, float(rayon),        float(L + 3.0 * rayon), float(2.0 * rayon));
  AWorkspace->RenderText (L"Z", 0, float(-2.0 * rayon), float(0.5 * rayon),     float(L + 3.0 * rayon));

  /*PCD 17/06/07    */
  glDepthFunc(df);

  if (!isWithinView) { /* restore matrix */
    glMatrixMode (GL_PROJECTION);
    glPopMatrix ();
    glMatrixMode (GL_MODELVIEW);
    glPopMatrix ();
  }
}


/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/*
* Fonctions publiques 
*/


/*
* initialisation d'un triedre non zoomable dans une vue.
* ou modification des valeurs deja initialisees.
*/

//call_triedron_init
OpenGl_Trihedron::OpenGl_Trihedron (const Aspect_TypeOfTriedronPosition APosition, const Quantity_NameOfColor AColor,
                                  const Standard_Real AScale, const Standard_Boolean AsWireframe)
: myPos(APosition),
  myScale(AScale),
  myIsWireframe(AsWireframe)
{
  Standard_Real R,G,B;
  Quantity_Color Color(AColor);
  Color.Values(R,G,B,Quantity_TOC_RGB);

  CALL_DEF_CONTEXTLINE aContextLine = myDefaultContextLine;
  aContextLine.Color.r = (float)R;
  aContextLine.Color.g = (float)G;
  aContextLine.Color.b = (float)B;
  myAspectLine.SetContext(aContextLine);

  CALL_DEF_CONTEXTTEXT aContextText = myDefaultContextText;
  aContextText.Color.r = (float)R;
  aContextText.Color.g = (float)G;
  aContextText.Color.b = (float)B;
  myAspectText.SetContext(aContextText);

  myXColor = theXColor;
  myYColor = theYColor;
  myZColor = theZColor;

  myRatio = theRatio;
  myDiameter = theDiameter;
  myNbFacettes = theNbFacettes;
}

/*----------------------------------------------------------------------*/

/*
* destruction du triedre non zoomable d'une vue.
*/

//call_triedron_erase
OpenGl_Trihedron::~OpenGl_Trihedron ()
{
}

/*----------------------------------------------------------------------*/

/*
* affichage d'un triedre non zoomable dans la wks  awsid 
*
* Triedre = Objet non Zoomable;
* on cree cette fonction pour pouvoir travailler par les structures 
* utilisees par les fonctions Tsm* et TEL_VIEW_REP
*
*/

//call_triedron_redraw_from_wsid
void OpenGl_Trihedron::Render (const Handle(OpenGl_Workspace) &AWorkspace) const
{
  const OpenGl_AspectLine *oldAspectLine = AWorkspace->SetAspectLine(&myAspectLine);
  const OpenGl_AspectText *oldAspectText = AWorkspace->SetAspectText(&myAspectText);

  /* check if GL_LIGHTING should be disabled
  no enabling 'cause it will be done (if necessary: kinda Polygon types ) 
  during redrawing structures
  */
  if (!AWorkspace->UseGLLight())
    glDisable( GL_LIGHTING );

  /* sauvegarde du contexte (on reste dans le buffer courant) */
  const GLboolean save_texture_state = IsTextureEnabled();
  DisableTexture();

  /* affichage du Triedre Non Zoomable */
  AWorkspace->ActiveView()->EndTransformPersistence();

  if (myIsWireframe)
    Redraw (AWorkspace);
  else
    RedrawZBuffer (AWorkspace);

  /* restauration du contexte */
  if (save_texture_state) EnableTexture();

  AWorkspace->SetAspectText(oldAspectText);
  AWorkspace->SetAspectLine(oldAspectLine);
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
