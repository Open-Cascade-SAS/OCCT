/***********************************************************************

FONCTION :
----------
File OpenGl_triedron : gestion du triedre non zoomable.

REMARQUES:
---------- 

Algorithme :
o dimensions et origine du triedre sont recalcules a chaque fois, en
fonction de la taille (u,v = largeur-hauteur) de la fenetre =>
il est inutile ici de jouer sur le facteur d'echelle.
o on n'inhibe que les translations, car on veut conserver les
transformations d'orientations.


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
08-12-98 : BGN ; Creation.


************************************************************************/

#define BUC60851  /* GG 15/03/01 Avoid to raise after the second creation
of the trihedron
*/

#define BUC61045        /* 25/10/01 SAV ; added functionality to control gl lighting 
from higher API */


#define QTOCC_PATCH   /* Active QtOPENCASCADE patches */

/*----------------------------------------------------------------------*/
/*
* Includes
*/ 

#include <OpenGl_tgl_all.hxx>
#include <OpenGl_tgl_tox.hxx>


#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h> /* pour M_PI */

#include <OpenGl_LightBox.hxx>
#include <OpenGl_TextureBox.hxx>
#include <OpenGl_tgl_tox.hxx>

#include <OpenGl_tsm_ws.hxx>  /* pour TsmGetWSAttri */
#include <OpenGl_telem_view.hxx>  /* pour tel_view_data */
#include <InterfaceGraphic_Graphic3d.hxx>  /* pour CALL_DEF_STRUCTURE */
#include <InterfaceGraphic_Aspect.hxx>  /* pour CALL_DEF_VIEW  */
#include <InterfaceGraphic_Visual3d.hxx>

#include <OpenGl_transform_persistence.hxx>

#ifdef HAVE_GL2PS
#include <gl2ps.h>
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <OpenGl_TextRender.hxx>
/*----------------------------------------------------------------------*/
/*
* Constantes
*/ 

#define NO_DEBUG
#define NO_PRINT
#define NO_PRINT_MATRIX

#ifndef M_PI
# define M_PI          3.14159265358979323846
#endif

/* on trouve que 10 vues sera suffisant */
#define GROW_SIZE_NZ_WKS   10

/* pour l'instant on ne travaille que sur le triedre */
/* on augmentera le define quand on developpera les struc. non zoomables*/
#define GROW_SIZE_NZ_STRUC  1 

/* Identificateur du Triedre Non Zommable dans la liste des structures */
# define TRIEDRON_ID -100

/*----------------------------------------------------------------------*/
/*
* Definition de types
*/ 
typedef struct
{
  float aXColor[3];
  float aYColor[3];
  float aZColor[3];
  float aRatio;
  float aDiametr;
  int   aNbFacettes;
} ZBUF_STRUCT;


/* donnees des structures non zoomables liees a une wks (=une vue) */
typedef struct
{
  int   NZStrucID; /* = TRIEDRON_ID ou astructure->Id de Graphic3d sinon.*/
  int   aPos;
  float aColor[3];
  float aScale;
  int   isWireframe;
  CALL_DEF_STRUCTURE * astructure;
  ZBUF_STRUCT* aZBufParam;
} NZ_STRUC_DATA;



/* liste des wks (=vues) concernees par les structures non zoomables */
typedef struct
{
  int   nz_wks; /* = id de la wks  */
  int   triedron_on; /* =0 si le triedre non zoomable n'est pas defini */
  int   nz_struc_count; /* nb de structures non zoomables de la vue */
  int   nz_struc_size;
  NZ_STRUC_DATA *nz_struc;
} NZ_WKS;


typedef  struct
{
  TEL_VIEW_REP    vrep;
} TEL_VIEW_DATA, *tel_view_data; /* Internal data stored for every view rep */

/*----------------------------------------------------------------------*/
/*
* Variables statiques
*/

static NZ_WKS *nz_wks = NULL;
static int     nz_wks_count = 0;
static int     nz_wks_size = 0;

/* Default parameters for ZBUFFER triheron */
static float theXColor[] = { 1.0, 0.0, 0.0 };
static float theYColor[] = { 0.0, 1.0, 0.0 };
static float theZColor[] = { 0.0, 0.0, 1.0 };
static float theRatio = 0.8f;
static float theDiametr = 0.05f;
static int   theNBFacettes = 12;

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/*
* Fonctions privees
*/



/*----------------------------------------------------------------------*/
/*
* recherche de la wks NZWksID concernee par les structures non zoomables;
* creation de l'element wks s'il n'existe pas.
*/
static int find_nz_wks(int NZWksID, int alloc)
{
  int i;

  /* recherche la wks dans la liste si elle existe */
  for (i=0; i<nz_wks_count; i++)
    if (nz_wks[i].nz_wks == NZWksID)
      return i;

  if (!alloc) return -1;

  /* la wks n'existe pas => on fait de la place si il n'y en a plus */
  if (nz_wks_count == nz_wks_size) {      
#ifdef BUC60851
    if( nz_wks_size > 0 ) {
      nz_wks_size += GROW_SIZE_NZ_WKS;
      nz_wks =  (NZ_WKS *) realloc(nz_wks, nz_wks_size * sizeof(NZ_WKS)); 
    } else {
      nz_wks_size = GROW_SIZE_NZ_WKS;
      nz_wks = (NZ_WKS *) malloc(nz_wks_size * sizeof(NZ_WKS));
    }
#else
    nz_wks_size += GROW_SIZE_NZ_WKS;
    nz_wks =  (NZ_WKS *) realloc(nz_wks, nz_wks_size * sizeof(NZ_WKS));
#endif
    if (nz_wks == NULL) return -1;
  }

  nz_wks[nz_wks_count].nz_wks = NZWksID;
  nz_wks[nz_wks_count].triedron_on = 0;
  nz_wks[nz_wks_count].nz_struc = NULL;
  nz_wks[nz_wks_count].nz_struc_size = 0;
  nz_wks[nz_wks_count].nz_struc_count = 0;

  return nz_wks_count++;
}

/*-----------------------------------------------------------------*/
/*
* recherche de la structure non zoomable NZStrucID dans la liste
* de la wks NZWksIdx ;
* creation de l'element structure non zoomable s'il n'existe pas.
*/
static int find_nz_struc(int NZWksIdx, int NZStrucID, int alloc)
{
  int i;
  NZ_STRUC_DATA *nz_struc;


  /* recherche la structure non zoomable dans la liste de la wks */
  nz_struc = nz_wks[NZWksIdx].nz_struc;
  for (i=0; i<nz_wks[NZWksIdx].nz_struc_count; i++)
    if (nz_struc[i].NZStrucID == NZStrucID)
      return i;

  if (!alloc) return -1;

  /* la structure non zoomable n'existe pas => on la cree */
  if (nz_wks[NZWksIdx].nz_struc_count == nz_wks[NZWksIdx].nz_struc_size) {
#ifdef BUC60851
    if( nz_wks[NZWksIdx].nz_struc_size > 0 ) {
      nz_wks[NZWksIdx].nz_struc_size += GROW_SIZE_NZ_STRUC;
      nz_wks[NZWksIdx].nz_struc = 
        (NZ_STRUC_DATA *)  realloc(nz_wks[NZWksIdx].nz_struc, 
        nz_wks[NZWksIdx].nz_struc_size * sizeof(NZ_STRUC_DATA));
    } else {
      nz_wks[NZWksIdx].nz_struc_size = GROW_SIZE_NZ_STRUC;
      nz_wks[NZWksIdx].nz_struc = 
        (NZ_STRUC_DATA *)  malloc( nz_wks[NZWksIdx].nz_struc_size * sizeof(NZ_STRUC_DATA));
    }
#else
    nz_wks[NZWksIdx].nz_struc_size += GROW_SIZE_NZ_STRUC;
    nz_wks[NZWksIdx].nz_struc = (NZ_STRUC_DATA *)  realloc(nz_wks[NZWksIdx].nz_struc, nz_wks[NZWksIdx].nz_struc_size * sizeof(NZ_STRUC_DATA));
#endif
    if (nz_wks[NZWksIdx].nz_struc == NULL) return -1;

    nz_wks[NZWksIdx].nz_struc[nz_wks[NZWksIdx].nz_struc_count].aZBufParam = NULL;
  }

  return nz_wks[NZWksIdx].nz_struc_count++;
}


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




TStatus call_triedron_redraw (
                              int      nz_wks_entry,
                              int      nz_struc_entry,
                              GLdouble U,
                              GLdouble V
                              )
{
  GLdouble modelMatrix[4][4];
  GLdouble projMatrix[4][4];

  GLdouble TriedronAxeX[3] = { 1.0, 0.0, 0.0 };
  GLdouble TriedronAxeY[3] = { 0.0, 1.0, 0.0 };
  GLdouble TriedronAxeZ[3] = { 0.0, 0.0, 1.0 };
  GLdouble TriedronOrigin[3] = { 0.0, 0.0, 0.0 };
  GLfloat TriedronColor[3] = { 1.0, 1.0, 1.0 }; /* def = blanc */

  GLfloat TriedronWidth = 1.0;
  GLfloat TriedronScale = (float)0.1 ;
  GLint   TriedronPosition = 0; /* def = Aspect_TOTP_CENTER */

  GLdouble L, l, rayon ;
  GLdouble minUV;
  int      ii;
#ifdef PRINT_MATRIX 
  int      jj;
#endif
  int      NbFacettes;
  double   Angle;
  GLdouble TriedronCoord[3] = { 1.0, 0.0, 0.0 };

  GLuint fontBase = 0;
  GLint mode;

#ifdef QTOCC_PATCH /* PCD 10/02/08 */
  /* Fix to problem with clipping planes chopping off pieces of the triedron */
  GLint max_plane=0;
  GLboolean* isPlaneActive;
  glGetIntegerv( GL_MAX_CLIP_PLANES, &max_plane);
  isPlaneActive= new GLboolean[max_plane];
  /* Backup the clip planes.         */
  for (ii = 0; ii < max_plane ; ii++) {
    isPlaneActive[ii] = glIsEnabled(GL_CLIP_PLANE0 + ii);
    glDisable(GL_CLIP_PLANE0 + ii);
  }
#endif

  /* 
  * Lecture des Init. du Triedre 
  */

  TriedronColor[0] = nz_wks[nz_wks_entry].nz_struc[nz_struc_entry].aColor[0];
  TriedronColor[1] = nz_wks[nz_wks_entry].nz_struc[nz_struc_entry].aColor[1];
  TriedronColor[2] = nz_wks[nz_wks_entry].nz_struc[nz_struc_entry].aColor[2];
  TriedronScale = nz_wks[nz_wks_entry].nz_struc[nz_struc_entry].aScale;
  TriedronPosition = nz_wks[nz_wks_entry].nz_struc[nz_struc_entry].aPos;


  /* 
  * Calcul des axes => on inhibe le zoom.
  */

  /* la taille des axes est 1 proportion (fixee a l'init du triedre) */
  /* de la dimension la plus petite de la window.                    */ 
  if ( U < V )  minUV = U;
  else minUV = V;
  L = minUV * (double) TriedronScale ;

  /* Position de l'origine */
  TriedronOrigin[0]= 0.0; 
  TriedronOrigin[1]= 0.0;
  TriedronOrigin[2]= 0.0; 

  /* Position des Axes */
  TriedronAxeX[0] = TriedronOrigin[0] + L ;
  TriedronAxeX[1] = TriedronOrigin[1] + 0.0;
  TriedronAxeX[2] = TriedronOrigin[2] + 0.0;

  TriedronAxeY[0] = TriedronOrigin[0] + 0.0;
  TriedronAxeY[1] = TriedronOrigin[1] + L ;
  TriedronAxeY[2] = TriedronOrigin[2] + 0.0;

  TriedronAxeZ[0] = TriedronOrigin[0] + 0.0;
  TriedronAxeZ[1] = TriedronOrigin[1] + 0.0;
  TriedronAxeZ[2] = TriedronOrigin[2] + L ;

  /*
  * On inhibe les translations; on conserve les autres transformations.
  */

  /* on lit les matrices de transformation et de projection de la vue */
  /* pour annuler les translations (dernieres colonnes des matrices). */
  glGetDoublev( GL_MODELVIEW_MATRIX,  (GLdouble *) modelMatrix );
  glGetDoublev( GL_PROJECTION_MATRIX, (GLdouble *) projMatrix );

#ifdef PRINT_MATRIX 
  printf ("\n--- call_triedron_redraw : avant transfo projMatrix= ");
  for (ii = 0; ii<4 ; ii++) {
    printf ("\n ");
    for (jj = 0; jj<4 ; jj++) {
      printf ("  %f  ", projMatrix[ii][jj] );
    }
  }
  printf ("\n--- call_triedron_redraw : avant transfo  modelMatrix= ");
  for (ii = 0; ii<4 ; ii++) {
    printf ("\n ");
    for (jj = 0; jj<4 ; jj++) {
      printf ("  %f  ", modelMatrix[ii][jj] );
    }
  }
#endif


  /* on annule la translation qui peut etre affectee a la vue */
  modelMatrix[3][0] = 0. ;
  modelMatrix[3][1] = 0. ;
  modelMatrix[3][2] = 0. ; 
  projMatrix[3][0] = 0. ;
  projMatrix[3][1] = 0. ;
  projMatrix[3][2] = 0. ; 


  /* sauvegarde du contexte des matrices avant chargement */
  glMatrixMode (GL_MODELVIEW);
  glPushMatrix ();
  glLoadIdentity ();
  glLoadMatrixd( (GLdouble *) modelMatrix);
  glMatrixMode ( GL_PROJECTION );
  glPushMatrix ();
  glLoadIdentity();
  glLoadMatrixd( (GLdouble *) projMatrix);


#ifdef PRINT_MATRIX 
  printf ("\n\n\n--- call_triedron_redraw :  APRES transfo projMatrix= ");
  for (ii = 0; ii<4 ; ii++) {
    printf ("\n ");
    for (jj = 0; jj<4 ; jj++) {
      printf ("  %f  ", projMatrix[ii][jj] );
    }
  }
  printf ("\n--- call_triedron_redraw : APRES transfo  modelMatrix= ");
  for (ii = 0; ii<4 ; ii++) {
    printf ("\n ");
    for (jj = 0; jj<4 ; jj++) {
      printf ("  %f  ", modelMatrix[ii][jj] );
    }
  }
#endif


  /*
  * Positionnement de l'origine du triedre selon le choix de l'init
  */

  /* on fait uniquement une translation de l'origine du Triedre */

  switch (TriedronPosition) {

       case 0 : /* Aspect_TOTP_CENTER */
         break;

       case 1 : /* Aspect_TOTP_LEFT_LOWER */
         glTranslated( -U/2. + L , -V/2. + L , 0. );
         break;

       case 2  : /*Aspect_TOTP_LEFT_UPPER */
         glTranslated( -U/2. + L , +V/2. - L -L/3. , 0. );
         break;

       case 3 : /* Aspect_TOTP_RIGHT_LOWER */
         glTranslated( U/2. - L -L/3. , -V/2. + L , 0. );
         break;

       case 4  : /* Aspect_TOTP_RIGHT_UPPER */
         glTranslated( U/2. - L -L/3. , +V/2. - L -L/3. , 0. );
         break;

       default :
         break;

  }

#ifdef PRINT 
  printf ("\n--- call_triedron_redraw :  TriedronOrigin= %f %f %f\n",
    TriedronOrigin[0], TriedronOrigin[1], TriedronOrigin[2]);
  printf ("\n---                      :  TriedronAxeX= %f %f %f \n",
    TriedronAxeX[0], TriedronAxeX[1], TriedronAxeX[2]);
  printf ("\n---                      :  TriedronAxeY= %f %f %f \n",
    TriedronAxeY[0], TriedronAxeY[1], TriedronAxeY[2] );
  printf ("\n---                      :  TriedronAxeZ= %f %f %f \n",
    TriedronAxeZ[0], TriedronAxeZ[1], TriedronAxeZ[2]);
  printf ("\n---                      : TriedronScale= %f \n",TriedronScale);
  printf ("\n---                      : 1/echelle = ( %f %f %f %f ) \n",
    modelMatrix[0][0],modelMatrix[1][1],modelMatrix[2][2],modelMatrix[3][3]);
#endif


  /* 
  * Creation du triedre 
  */
  glColor3fv (TriedronColor);

  glGetIntegerv( GL_RENDER_MODE, &mode );

#ifdef HAVE_GL2PS
  if( mode==GL_RENDER )
    glLineWidth( TriedronWidth );
  else if( mode==GL_FEEDBACK )
    gl2psLineWidth( TriedronWidth );
#else
	glLineWidth( TriedronWidth );
#endif 

#ifdef QTOCC_PATCH /* Fotis Sioutis 2007-11-14 15:06 
  I have also seen in previous posts that the view trihedron in V3d_WIREFRAME mode 
  changes colors depending on the state of the view. This behaviour can be easily 
  corrected by altering call_triedron_redraw function in OpenGl_triedron.c of TKOpengl.
  The only change needed is to erase the LightOff() function that is called before the 
  Axis name drawing and move this function call just before the initial axis drawing.
  Below is the code portion with the modification.I don't know if this is considered to 
  be a bug but anyway i believe it might help some of you out there.*/
  LightOff();
#endif

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
  l = L - L/4. ; /* distance a l'origine */
  rayon = L/30. ; /* rayon de la base du cone */
  NbFacettes = 12; /* le cone sera compose de 12 facettes triangulaires */
  Angle = 2. * M_PI/ NbFacettes;

  /* solution FILAIRE des cones au bout des axes : une seule ligne */
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  /* (la couleur est deja initialisee dans TriedronColor) */
  /* FIN de la solution FILAIRE CHOISIE pour les cones des axes */

  /* fleche en X */
  glBegin(GL_TRIANGLE_FAN);
  glVertex3dv( TriedronAxeX );
  TriedronCoord[0] = TriedronOrigin[0] + l ;
  ii = NbFacettes;
  while (ii >= 0 ) {
    TriedronCoord[1] = TriedronOrigin[1] + ( rayon * sin(ii * Angle) );
    TriedronCoord[2] = TriedronOrigin[2] + ( rayon * cos(ii * Angle) );
    glVertex3dv( TriedronCoord );
    ii--;
  }
  glEnd();

  /* fleche en Y */
  glBegin(GL_TRIANGLE_FAN);
  glVertex3dv( TriedronAxeY );
  TriedronCoord[1] = TriedronOrigin[1] + l ;
  ii = NbFacettes;
  while (ii >= 0 ) {
    TriedronCoord[0] = TriedronOrigin[0] + (rayon * cos(ii * Angle) );
    TriedronCoord[2] = TriedronOrigin[2] + (rayon * sin(ii * Angle) );
    glVertex3dv( TriedronCoord );
    ii--;
  }
  glEnd();

  /* fleche en Z */
  glBegin(GL_TRIANGLE_FAN);
  glVertex3dv( TriedronAxeZ );
  TriedronCoord[2] = TriedronOrigin[2] + l ;
  ii = NbFacettes;
  while (ii >= 0 ) {
    TriedronCoord[0] = TriedronOrigin[0] + ( rayon * sin(ii * Angle) );
    TriedronCoord[1] = TriedronOrigin[1] + ( rayon * cos(ii * Angle) );
    glVertex3dv( TriedronCoord );
    ii--;
  }
  glEnd();

  /* dessin de l'origine */
  TriedronCoord[0] = TriedronOrigin[0] + rayon ;
  TriedronCoord[1] = TriedronOrigin[1] + 0.0 ;
  TriedronCoord[2] = TriedronOrigin[2] + 0.0 ;
  ii = 24 ;
  Angle = 2. * M_PI/ii ;
  glBegin(GL_LINE_LOOP);
  while (ii >= 0 ) {
    TriedronCoord[0] = TriedronOrigin[0] + ( rayon * sin(ii * Angle) );
    TriedronCoord[1] = TriedronOrigin[1] + ( rayon * cos(ii * Angle) );
    glVertex3dv( TriedronCoord );
    ii--;
  }  
  glEnd();

  LightOff();

  /* 
  * Noms des axes et de l'origine
  */

  /* init de la fonte */

  OpenGl_TextRender* textRender=OpenGl_TextRender::instance();

  /* Axe X */
  TriedronCoord[0] = TriedronOrigin[0] + ( L + rayon ) ;
  TriedronCoord[1] = TriedronOrigin[1] + 0.0;
  TriedronCoord[2] = TriedronOrigin[2] - rayon ;
  textRender->RenderText(L"X", fontBase, 0, (float)TriedronCoord[0], (float)TriedronCoord[1], (float)TriedronCoord[2] );

  /* Axe Y */
  TriedronCoord[0] = TriedronOrigin[0] + rayon ;
  TriedronCoord[1] = TriedronOrigin[1] + ( L + 3.0 * rayon ) ;
  TriedronCoord[2] = TriedronOrigin[2] + ( 2.0 * rayon );
  textRender->RenderText(L"Y", fontBase, 0, (float)TriedronCoord[0], (float)TriedronCoord[1], (float)TriedronCoord[2]);

  /* Axe Z */
  TriedronCoord[0] = TriedronOrigin[0] + ( - 2.0 * rayon ) ;
  TriedronCoord[1] = TriedronOrigin[1] +  rayon/2. ;
  TriedronCoord[2] = TriedronOrigin[2] + ( L + 3.0 * rayon ) ;
  textRender->RenderText(L"Z", fontBase, 0, (float)TriedronCoord[0], (float)TriedronCoord[1], (float)TriedronCoord[2]);

#ifdef QTOCC_PATCH /* PCD 10/02/08 */
  /* Recover the clip planes */
  glGetIntegerv( GL_MAX_CLIP_PLANES, &max_plane);
  for (ii = 0; ii <max_plane ; ii++) {
    if (isPlaneActive[ii]) { 
      glEnable(GL_CLIP_PLANE0 + ii);
    }
  } 
  free(isPlaneActive);

#endif

  /* 
  * restauration du contexte des matrices
  */
  glMatrixMode (GL_PROJECTION);
  glPopMatrix ();
  glMatrixMode (GL_MODELVIEW);
  glPopMatrix ();


  return (TSuccess);

}




/*******************************************************
*  Draws ZBUFFER trihedron mode
*******************************************************/
TStatus call_zbuffer_triedron_redraw (
                                      int      nz_wks_entry,
                                      int      nz_struc_entry,
                                      GLdouble U,
                                      GLdouble V
                                      )
{
  GLdouble modelMatrix[4][4];
  GLdouble projMatrix[4][4];

  GLdouble TriedronAxeX[3] = { 1.0, 0.0, 0.0 };
  GLdouble TriedronAxeY[3] = { 0.0, 1.0, 0.0 };
  GLdouble TriedronAxeZ[3] = { 0.0, 0.0, 1.0 };
  GLdouble TriedronOrigin[3] = { 0.0, 0.0, 0.0 };
  GLfloat TriedronColor[3] = { 1.0, 1.0, 1.0 }; /* def = blanc */

  GLfloat TriedronScale = (float)0.1 ;
  GLint   TriedronPosition = 0; /* def = Aspect_TOTP_CENTER */

  GLdouble L, l, rayon ;
  GLdouble minUV;
  int      NbFacettes = 12;
  double   Angle;
  GLdouble TriedronCoord[3] = { 1.0, 0.0, 0.0 };

  GLuint fontBase = 0;

  GLuint startList;
  GLUquadricObj* aQuadric;
  GLfloat aXColor[] = { 1.0, 0.0, 0.0, 0.6f };
  GLfloat aYColor[] = { 0.0, 1.0, 0.0, 0.6f };
  GLfloat aZColor[] = { 0.0, 0.0, 1.0, 0.6f };
  GLdouble aConeDiametr;
  GLdouble aConeLength;
  GLdouble aCylinderDiametr;
  GLdouble aCylinderLength;
  GLboolean aIsDepthEnabled;
#ifndef BUG
  GLboolean aIsDepthMaskEnabled;
#endif
  GLint   aViewPort[4];  /* to store view port coordinates */
  GLdouble aWinCoord[3];
  GLboolean isWithinView;
  GLdouble aLengthReduce = 0.8;
  GLdouble aAxisDiametr = 0.05;
  ZBUF_STRUCT* aParam;

#ifdef QTOCC_PATCH 
  GLint df;                                       /* PCD 17/06/07 */      
  GLfloat aNULLColor[] = { 0.0, 0.0, 0.0, 0.0f }; /* FS 21/01/08 */
  /* Fix to problem with clipping planes chopping off pieces of the triedron   */
  int i;
  GLint max_plane=0;
  GLboolean* isPlaneActive;
  glGetIntegerv( GL_MAX_CLIP_PLANES, &max_plane);
  isPlaneActive = new GLboolean[max_plane];
  /* Backup the clip planes. */
  for (i = 0; i < max_plane ; i++) {
    isPlaneActive[i] = glIsEnabled(GL_CLIP_PLANE0 + i);
    glDisable(GL_CLIP_PLANE0 + i);
  }
#endif

  /* 
  * Lecture des Init. du Triedre 
  */

  TriedronColor[0] = nz_wks[nz_wks_entry].nz_struc[nz_struc_entry].aColor[0];
  TriedronColor[1] = nz_wks[nz_wks_entry].nz_struc[nz_struc_entry].aColor[1];
  TriedronColor[2] = nz_wks[nz_wks_entry].nz_struc[nz_struc_entry].aColor[2];
  TriedronScale = nz_wks[nz_wks_entry].nz_struc[nz_struc_entry].aScale;
  TriedronPosition = nz_wks[nz_wks_entry].nz_struc[nz_struc_entry].aPos;

  if (nz_wks[nz_wks_entry].nz_struc[nz_struc_entry].aZBufParam != NULL) {
    aParam = nz_wks[nz_wks_entry].nz_struc[nz_struc_entry].aZBufParam;
    aXColor[0] = aParam->aXColor[0];
    aXColor[1] = aParam->aXColor[1];
    aXColor[2] = aParam->aXColor[2];

    aYColor[0] = aParam->aYColor[0];
    aYColor[1] = aParam->aYColor[1];
    aYColor[2] = aParam->aYColor[2];

    aZColor[0] = aParam->aZColor[0];
    aZColor[1] = aParam->aZColor[1];
    aZColor[2] = aParam->aZColor[2];

    aLengthReduce = aParam->aRatio;
    aAxisDiametr = aParam->aDiametr;
    NbFacettes = aParam->aNbFacettes;
  }

  /* 
  * Calcul des axes => on inhibe le zoom.
  */

  /* la taille des axes est 1 proportion (fixee a l'init du triedre) */
  /* de la dimension la plus petite de la window.                    */ 
  if ( U < V )  minUV = U;
  else minUV = V;
  L = minUV * (double) TriedronScale ;

  /* Position de l'origine */
  TriedronOrigin[0]= 0.0; 
  TriedronOrigin[1]= 0.0;
  TriedronOrigin[2]= 0.0; 

  /* Position des Axes */
  TriedronAxeX[0] = TriedronOrigin[0] + L ;
  TriedronAxeX[1] = TriedronOrigin[1] + 0.0;
  TriedronAxeX[2] = TriedronOrigin[2] + 0.0;

  TriedronAxeY[0] = TriedronOrigin[0] + 0.0;
  TriedronAxeY[1] = TriedronOrigin[1] + L ;
  TriedronAxeY[2] = TriedronOrigin[2] + 0.0;

  TriedronAxeZ[0] = TriedronOrigin[0] + 0.0;
  TriedronAxeZ[1] = TriedronOrigin[1] + 0.0;
  TriedronAxeZ[2] = TriedronOrigin[2] + L ;

  /* Check position in the ViewPort */
  glGetDoublev( GL_MODELVIEW_MATRIX,  (GLdouble *) modelMatrix );
  glGetDoublev( GL_PROJECTION_MATRIX, (GLdouble *) projMatrix );

  glGetIntegerv(GL_VIEWPORT, aViewPort);
  gluProject(TriedronOrigin[0], TriedronOrigin[1], TriedronOrigin[2],
    (GLdouble *)modelMatrix, (GLdouble *)projMatrix, aViewPort,
    &aWinCoord[0], &aWinCoord[1], &aWinCoord[2]);

#ifdef QTOCC_PATCH /* PCD 29/09/2008 */
  /* Simple code modification recommended by Fotis Sioutis and Peter Dolbey  */
  /* to remove the irritating default behaviour of triedrons using V3d_ZBUFFER   */
  /* which causes the glyph to jump around the screen when the origin moves offscreen. */
  isWithinView = GL_FALSE;
#else
  /* Original code */
  isWithinView = !((aWinCoord[0]<aViewPort[0]) || (aWinCoord[0]>aViewPort[2]) ||
    (aWinCoord[1]<aViewPort[1]) || (aWinCoord[1]>aViewPort[3]));
#endif

  if (!isWithinView) {
    /* Annulate translation matrix */
    modelMatrix[3][0] = 0. ;
    modelMatrix[3][1] = 0. ;
    modelMatrix[3][2] = 0. ; 
    projMatrix[3][0] = 0. ;
    projMatrix[3][1] = 0. ;
    projMatrix[3][2] = 0. ; 

    /* save matrix */
    glMatrixMode (GL_MODELVIEW);
    glPushMatrix ();
    glLoadIdentity ();
    glLoadMatrixd( (GLdouble *) modelMatrix);
    glMatrixMode ( GL_PROJECTION );
    glPushMatrix ();
    glLoadIdentity();
    glLoadMatrixd( (GLdouble *) projMatrix);


    /*
    * Define position in the view
    */
    switch (TriedronPosition) {
      case 0 :  /* Aspect_TOTP_CENTER */
        break;

      case 1 :  /* Aspect_TOTP_LEFT_LOWER */
        glTranslated( -U/2. + L , -V/2. + L , 0. );
        break;

      case 2  : /* Aspect_TOTP_LEFT_UPPER */
        glTranslated( -U/2. + L , +V/2. - L -L/3. , 0. );
        break;

      case 3 :  /* Aspect_TOTP_RIGHT_LOWER */
        glTranslated( U/2. - L -L/3. , -V/2. + L , 0. );
        break;

      case 4  :  /* Aspect_TOTP_RIGHT_UPPER */
        glTranslated( U/2. - L -L/3. , +V/2. - L -L/3. , 0. );
        break;

      default :
        break;

    }
    L *= aLengthReduce;
  }


  /* 
  * Creation the trihedron
  */

#define COLOR_REDUCE      0.3f
#define CYLINDER_LENGTH   0.75f
#ifdef BUG
#define ALPHA_REDUCE      0.4f
#else
#define ALPHA_REDUCE      1.0f
#endif

  startList = glGenLists(4);
  aQuadric = gluNewQuadric();

  aCylinderLength = L * CYLINDER_LENGTH;
  aCylinderDiametr = L * aAxisDiametr;
  aConeDiametr = aCylinderDiametr * 2;
  aConeLength = L * (1 - CYLINDER_LENGTH);
  /* Correct for owerlapping */
  /*    aCylinderLength += aConeLength - 1.2*aCylinderDiametr*aConeLength/aConeDiametr;*/

  aIsDepthEnabled = glIsEnabled(GL_DEPTH_TEST);
#ifndef BUG

#ifdef QTOCC_PATCH  /*PCD 02/07/07   */
  /* GL_DEPTH_WRITEMASK is not a valid argument to glIsEnabled, the  */
  /* original code is shown to be broken when run under an OpenGL debugger  */
  /* like GLIntercept. This is the correct way to retrieve the mask value.  */
  glGetBooleanv(GL_DEPTH_WRITEMASK, &aIsDepthMaskEnabled); 
#else
  aIsDepthMaskEnabled = glIsEnabled(GL_DEPTH_WRITEMASK);
#endif

#endif 

  /* Create cylinder for axis */
  gluQuadricDrawStyle(aQuadric, GLU_FILL); /* smooth shaded */
  gluQuadricNormals(aQuadric, GLU_FLAT);
  /* Axis */
  glNewList(startList, GL_COMPILE);
  gluCylinder(aQuadric, aCylinderDiametr, aCylinderDiametr, aCylinderLength, NbFacettes, 1);
  glEndList();
  /* Cone */
  glNewList(startList + 1, GL_COMPILE);
  gluCylinder(aQuadric, aConeDiametr, 0, aConeLength, NbFacettes, 1);
  glEndList();
  /* Central sphere */
  glNewList(startList + 2, GL_COMPILE);
#ifdef QTOCC_PATCH
  gluSphere(aQuadric, aCylinderDiametr * 2, NbFacettes, NbFacettes);
#else
  gluSphere(aQuadric, aCylinderDiametr, NbFacettes, NbFacettes);
#endif
  glEndList();
  /* End disk */
  gluQuadricOrientation(aQuadric,GLU_INSIDE); /*szv*/
  glNewList(startList + 3, GL_COMPILE);
  gluDisk(aQuadric, aCylinderDiametr, aConeDiametr, NbFacettes, 1/*szv:2*/);
  glEndList();

#ifdef QTOCC_PATCH
  /* Store previous attributes */
  glPushAttrib(GL_LIGHTING_BIT | GL_POLYGON_BIT);
  LightOn();
#else
  LightOn();

  /* Store previous attributes */
  glPushAttrib(GL_LIGHTING_BIT | GL_POLYGON_BIT);
#endif

  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);

#ifdef QTOCC_PATCH /*Fotis Sioutis | 2008-01-21 10:55 
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
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, aNULLColor);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, aNULLColor);
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, aNULLColor);

  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.);
#endif

  glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);

  if (!aIsDepthEnabled)  {
    glEnable(GL_DEPTH_TEST);
#ifndef BUG
    glClear(GL_DEPTH_BUFFER_BIT);
#endif
  }
#ifdef BUG
  if (!(aIsDepthEnabled && isWithinView))
    glClear(GL_DEPTH_BUFFER_BIT);
#endif
#ifndef BUG
  if (!aIsDepthMaskEnabled)  {
    /* This is how the depthmask needs to be re-enabled...*/
    glDepthMask(GL_TRUE);
    /* ...and not this stuff below */
  }
#endif

  glMatrixMode(GL_MODELVIEW);
#ifdef QTOCC_PATCH /* PCD 17/06/07  */
  glGetIntegerv (GL_DEPTH_FUNC, &df); 
#else
  /*szv:if (isWithinView) {*/
  glDepthFunc(GL_GREATER);
  glPushMatrix();
  glPushMatrix();
  glPushMatrix();

  glColor4f(TriedronColor[0]*COLOR_REDUCE, 
    TriedronColor[1]*COLOR_REDUCE, 
    TriedronColor[2]*COLOR_REDUCE,
    ALPHA_REDUCE);
  glCallList(startList+2);

  /* Z axis */
  glColor4f(aZColor[0]*COLOR_REDUCE, 
    aZColor[1]*COLOR_REDUCE, 
    aZColor[2]*COLOR_REDUCE,
    ALPHA_REDUCE);
  glCallList(startList);
  glTranslated(0, 0, L * CYLINDER_LENGTH);
  glCallList(startList + 3);
  glCallList(startList + 1);
  glPopMatrix();    

  /* X axis */
  glRotated(90.0, TriedronAxeY[0], TriedronAxeY[1], TriedronAxeY[2]);
  glColor4f(aXColor[0]*COLOR_REDUCE, 
    aXColor[1]*COLOR_REDUCE, 
    aXColor[2]*COLOR_REDUCE,
    ALPHA_REDUCE);
  glCallList(startList);
  glTranslated(0, 0, L * CYLINDER_LENGTH);
  glCallList(startList + 3);
  glCallList(startList + 1);
  glPopMatrix();    

  /* Y axis */
  glRotated(-90.0, TriedronAxeX[0], TriedronAxeX[1], TriedronAxeX[2]);
  glColor4f(aYColor[0]*COLOR_REDUCE, 
    aYColor[1]*COLOR_REDUCE, 
    aYColor[2]*COLOR_REDUCE,
    ALPHA_REDUCE);
  glCallList(startList);
  glTranslated(0, 0, L * CYLINDER_LENGTH);
  glCallList(startList + 3);
  glCallList(startList + 1);
  glPopMatrix();

  glDepthFunc(GL_LESS);
  /*szv:}*/
#endif

#ifdef QTOCC_PATCH
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
#endif

    glPushMatrix();
    glPushMatrix();
    glPushMatrix();

    glColor3fv(TriedronColor);
    glCallList(startList+2);

    /* Z axis */
    glColor4fv(aZColor);
    glCallList(startList);
    glTranslated(0, 0, L * CYLINDER_LENGTH);
    glCallList(startList + 3);
    glCallList(startList + 1);
    glPopMatrix();    

    /* X axis */
    glRotated(90.0, TriedronAxeY[0], TriedronAxeY[1], TriedronAxeY[2]);
    glColor4fv(aXColor);
    glCallList(startList);
    glTranslated(0, 0, L * CYLINDER_LENGTH);
    glCallList(startList + 3);
    glCallList(startList + 1);
    glPopMatrix();    

    /* Y axis */
    glRotated(-90.0, TriedronAxeX[0], TriedronAxeX[1], TriedronAxeX[2]);
    glColor4fv(aYColor);
    glCallList(startList);
    glTranslated(0, 0, L * CYLINDER_LENGTH);
    glCallList(startList + 3);
    glCallList(startList + 1);
    glPopMatrix();

#ifdef QTOCC_PATCH
  }
#endif

  if (!aIsDepthEnabled) 
    glDisable(GL_DEPTH_TEST);
#ifndef BUG
  if (!aIsDepthMaskEnabled)

#ifdef QTOCC_PATCH /*PCD 02/07/07   */
    glDepthMask(GL_FALSE);
#else
    glDisable(GL_DEPTH_WRITEMASK);
#endif

#endif
  glDisable(GL_CULL_FACE);
  glDisable(GL_COLOR_MATERIAL);

  gluDeleteQuadric(aQuadric);
  glColor3fv (TriedronColor);

#ifdef QTOCC_PATCH /* PCD 11/02/08 */
  /* Always write the text */
  glDepthFunc(GL_ALWAYS); 
#endif

  glPopAttrib();

  /* fleches au bout des axes (= cones de la couleur demandee) */
  l = L - L/4. ; /* distance a l'origine */
  rayon = L/30. ; /* rayon de la base du cone */
  Angle = 2. * M_PI/ NbFacettes;

  glDeleteLists(startList, 4); 

  LightOff();

  /* 
  * origine names
  */

  /* init font */

  OpenGl_TextRender* textRender=OpenGl_TextRender::instance();

  /* Axe X */
  TriedronCoord[0] = TriedronOrigin[0] + ( L + rayon ) ;
  TriedronCoord[1] = TriedronOrigin[1] + 0.0;
  TriedronCoord[2] = TriedronOrigin[2] - rayon ;
  textRender->RenderText(L"X", fontBase, 0, (float)TriedronCoord[0], (float)TriedronCoord[1], (float)TriedronCoord[2]);

  /* Axe Y */
  TriedronCoord[0] = TriedronOrigin[0] + rayon ;
  TriedronCoord[1] = TriedronOrigin[1] + ( L + 3.0 * rayon ) ;
  TriedronCoord[2] = TriedronOrigin[2] + ( 2.0 * rayon );
  textRender->RenderText(L"Y", fontBase, 0, (float)TriedronCoord[0], (float)TriedronCoord[1], (float)TriedronCoord[2]);

  /* Axe Z */
  TriedronCoord[0] = TriedronOrigin[0] + ( - 2.0 * rayon ) ;
  TriedronCoord[1] = TriedronOrigin[1] +  rayon/2. ;
  TriedronCoord[2] = TriedronOrigin[2] + ( L + 3.0 * rayon ) ;
  textRender->RenderText(L"Z", fontBase, 0, (float)TriedronCoord[0], (float)TriedronCoord[1], (float)TriedronCoord[2]);

#ifdef QTOCC_PATCH 
  /*PCD 17/06/07    */
  glDepthFunc(df);

  /* PCD 10/02/08  */
  /* Recover the clip planes */ 
  glGetIntegerv( GL_MAX_CLIP_PLANES, &max_plane);
  for (i = 0; i < max_plane ; i++) {
    if (isPlaneActive[i]) { 
      glEnable(GL_CLIP_PLANE0 + i);
    }
  }
  free(isPlaneActive);
#endif

  if (!isWithinView) { /* restore matrix */
    glMatrixMode (GL_PROJECTION);
    glPopMatrix ();
    glMatrixMode (GL_MODELVIEW);
    glPopMatrix ();
  }

  return (TSuccess);

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

TStatus call_triedron_init (
                            CALL_DEF_VIEW * aview, 
                            int aPosition, 
                            float r,
                            float g,
                            float b, 
                            float aScale,
                            int asWireframe 
                            )

{

  int nz_wks_entry;
  int nz_struc_entry;
  ZBUF_STRUCT* aParam;


#ifdef PRINT
  printf("\n----------- call_triedron_init  r = %f, g = %f, b = %f",
    r, g, b);
  printf(", aScale = %f, aPosition = %d \n", aScale, aPosition );
#endif

  if (aview->WsId == -1) return(TFailure);
  if (aview->ViewId == -1) return(TFailure);
#ifdef PRINT
  printf(", aview->WsId=%d  aview->ViewId=%d \n",aview->WsId,aview->ViewId);
#endif



  /* recherche du num de la liste de structures non zoomables de la wks */
  /* creation sinon */
  nz_wks_entry = find_nz_wks(aview->WsId, 1);
  if (nz_wks_entry == -1) return TFailure;

  /* recherche du Triedre s'il existe; creation sinon */
  nz_struc_entry = find_nz_struc(nz_wks_entry, TRIEDRON_ID, 1);
  if (nz_struc_entry == -1) return TFailure;

  /* mise a jour du Triedre */
  nz_wks[nz_wks_entry].triedron_on = 1;
  nz_wks[nz_wks_entry].nz_struc[nz_struc_entry].NZStrucID = TRIEDRON_ID;
  nz_wks[nz_wks_entry].nz_struc[nz_struc_entry].astructure = NULL;
  nz_wks[nz_wks_entry].nz_struc[nz_struc_entry].aPos = aPosition;
  nz_wks[nz_wks_entry].nz_struc[nz_struc_entry].aColor[0] = r;
  nz_wks[nz_wks_entry].nz_struc[nz_struc_entry].aColor[1] = g;
  nz_wks[nz_wks_entry].nz_struc[nz_struc_entry].aColor[2] = b;
  nz_wks[nz_wks_entry].nz_struc[nz_struc_entry].aScale = aScale;
  nz_wks[nz_wks_entry].nz_struc[nz_struc_entry].isWireframe = asWireframe;

  nz_wks[nz_wks_entry].nz_struc[nz_struc_entry].aZBufParam = new ZBUF_STRUCT();
  if (nz_wks[nz_wks_entry].nz_struc[nz_struc_entry].aZBufParam == NULL) return TFailure;

  aParam = nz_wks[nz_wks_entry].nz_struc[nz_struc_entry].aZBufParam;
  aParam->aXColor[0] = theXColor[0];
  aParam->aXColor[1] = theXColor[1];
  aParam->aXColor[2] = theXColor[2];

  aParam->aYColor[0] = theYColor[0];
  aParam->aYColor[1] = theYColor[1];
  aParam->aYColor[2] = theYColor[2];

  aParam->aZColor[0] = theZColor[0];
  aParam->aZColor[1] = theZColor[1];
  aParam->aZColor[2] = theZColor[2];

  aParam->aRatio = theRatio;
  aParam->aDiametr = theDiametr;
  aParam->aNbFacettes = theNBFacettes;


#ifdef DEBUG
  printf("nz_wks_entry=%d nz_struc_entry=%d \n",nz_wks_entry,nz_struc_entry);
  printf("ajout ok\n");
#endif
  return (TSuccess);

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

TStatus call_triedron_redraw_from_wsid (
                                        Tint awsid
                                        )

{

  TStatus   status = TSuccess;
  int       nz_wks_entry;
  int       nz_struc_entry;
  int    save_texture_state;
  GLdouble  U, V ; /* largeur,hauteur de la fenetre */

  CMN_KEY_DATA    key;
  tel_view_data   vptr;

  if ( awsid == -1) return (TFailure );

#ifdef BUC61045
  /* check if GL_LIGHTING should be disabled
  no enabling 'cause it will be done (if necessary: kinda Polygon types ) 
  during redrawing structures 
  */
  TsmGetWSAttri (awsid, WSGLLight, &key );
  if ( key.ldata == TOff )
    glDisable( GL_LIGHTING );
#endif

  /* recherche du numero de la liste de structures non zoomables de la wks */
#ifdef PRINT
  printf("\n----------- call_triedron_redraw_from_WSID : appel find_nz_wks \n");
#endif
  nz_wks_entry = find_nz_wks(awsid, 0);
  /* si on ne l'a pas trouve, il n'y a rien a faire */
  if (nz_wks_entry == -1) return (TSuccess);

  /* recherche du Triedre */
#ifdef PRINT
  printf("\n----------- call_triedron_redraw_from_WSID : appel find_nz_struc \n");
#endif
  nz_struc_entry = find_nz_struc(nz_wks_entry, TRIEDRON_ID, 0);
  /* si on ne l'a pas trouve, il n'y a rien a faire */
  if (nz_struc_entry == -1) return (TSuccess);
  /* si il est "off" il n'y a rien a faire */
  if (nz_wks[nz_wks_entry].triedron_on == 0) return (TSuccess);


  /* recherche de la dimension de la fenetre                   */
  /* (car la taille des axes du treiedre en sera 1 proportion) */
  TsmGetWSAttri (awsid, WSViews, &key );
  vptr = (tel_view_data)key.pdata ; /* Obtain defined view data*/
  if ( !vptr ) return TFailure; /* no view defined yet */
  U = vptr->vrep.extra.map.window.xmax - vptr->vrep.extra.map.window.xmin;
  V = vptr->vrep.extra.map.window.ymax - vptr->vrep.extra.map.window.ymin;

  /* sauvegarde du contexte (on reste dans le buffer courant) */
  save_texture_state = IsTextureEnabled();
  DisableTexture();

  /* affichage du Triedre Non Zoomable */
  transform_persistence_end();
  if (nz_wks[nz_wks_entry].nz_struc[nz_struc_entry].isWireframe)
    status = call_triedron_redraw (nz_wks_entry, nz_struc_entry, U, V);
  else
    status = call_zbuffer_triedron_redraw (nz_wks_entry, nz_struc_entry, U, V);

  /* restauration du contexte */
  if (save_texture_state) EnableTexture();

  return status;

}

/*----------------------------------------------------------------------*/

/*
* affichage d'un triedre non zoomable dans la vue aview 
*
* Triedre = Objet non Zoomable;
* on cree cette fonction pour pouvoir utiliser CALL_DEF_VIEW
*
*/

TStatus call_triedron_redraw_from_view (
                                        CALL_DEF_VIEW * aview
                                        )
{

  TStatus   status = TSuccess;
  int       nz_wks_entry;
  int       nz_struc_entry;
  int    save_texture_state;
  GLdouble  U, V ; /* largeur,hauteur de la fenetre */



  if (aview->WsId == -1) return (TFailure );
  if (aview->ViewId == -1) return (TFailure);
#ifdef PRINT
  printf("\n call_triedron_redraw_from_VIEW aview->WsId=%d  aview->ViewId=%d \n",aview->WsId,aview->ViewId);
#endif


  /* recherche du numero de la liste de structures non zoomables de la wks */
#ifdef PRINT
  printf("\n----------- call_triedron_redraw_from_VIEW : appel find_nz_wks \n");
#endif
  nz_wks_entry = find_nz_wks(aview->WsId, 0);
  /* si on ne l'a pas trouve, il n'y a rien a faire */
  if (nz_wks_entry == -1) return (TSuccess);

  /* recherche du Triedre */
#ifdef PRINT
  printf("\n----------- call_triedron_redraw_from_VIEW : appel find_nz_struc \n");
#endif
  nz_struc_entry = find_nz_struc(nz_wks_entry, TRIEDRON_ID, 0);
  /* si on ne l'a pas trouve, il n'y a rien a faire */
  if (nz_struc_entry == -1) return (TSuccess);
  /* si il est "off" il n'y a rien a faire */
  if (nz_wks[nz_wks_entry].triedron_on == 0) return (TSuccess);


  /* recherche de la dimension de la fenetre                   */
  /* (car la taille des axes du treiedre en sera 1 proportion) */
  U = aview->Mapping.WindowLimit.uM - aview->Mapping.WindowLimit.um ;
  V = aview->Mapping.WindowLimit.vM - aview->Mapping.WindowLimit.vm ;

  /* sauvegarde du contexte et ecriture en front buffer */
  save_texture_state = IsTextureEnabled();
  DisableTexture();
  glDrawBuffer (GL_FRONT);

  /* affichage du Triedre Non Zoomable */
  transform_persistence_end();
  status = call_triedron_redraw (nz_wks_entry, nz_struc_entry, U, V);

  /* necessaire pour WNT */
  glFlush();

  /* restauration du contexte */
  if (save_texture_state) EnableTexture();
  glDrawBuffer (GL_BACK); 

  return status;

}

/*----------------------------------------------------------------------*/

/*
* destruction du triedre non zoomable d'une vue.
*/

TStatus call_triedron_erase (
                             CALL_DEF_VIEW * aview
                             )

{
  int   nz_wks_entry;
  int i;


#ifdef PRINT
  printf("\n----------- call_triedron_erase  " );
#endif

  if (aview->WsId == -1) return(TFailure);
  if (aview->ViewId == -1) return(TFailure);
#ifdef PRINT
  printf(", aview->WsId=%d  aview->ViewId=%d \n",aview->WsId,aview->ViewId);
#endif


  /* recherche du num de la liste de structures non zoomables de la wks */
  /* si on ne l'a pas trouve, il n'y a rien a faire                     */
  nz_wks_entry = find_nz_wks(aview->WsId, 0);
  if (nz_wks_entry == -1) return TSuccess;


  /* 
  * destruction du Triedre puis de toute la wks
  */
  /* (aujourd'hui il n'y a pas d'autre structure graphique dans la table */
  /* de la wks => on detruit tout ;                                      */
  /* si on ajoutait d'autres structures non zoomables, il faudrait       */
  /* selectionner et detruire uniquement l'element Triedre en cherchant  */
  /* s'il existe via une variable nz_struc_entry ).                      */
  for (i = 0; i < nz_wks[nz_wks_entry].nz_struc_count; i++) {
    if (nz_wks[nz_wks_entry].nz_struc[i].aZBufParam != NULL) 
      free(nz_wks[nz_wks_entry].nz_struc[i].aZBufParam);
  }

  if (nz_wks[nz_wks_entry].nz_struc != NULL) {
    free(nz_wks[nz_wks_entry].nz_struc);
  }

  if (nz_wks_count == 1) {
    free(nz_wks);
    nz_wks_count = 0;
    nz_wks_size = 0;
  }
  else { /* il y a au moins 2 wks definies */
    memcpy(&nz_wks[nz_wks_entry],
      &nz_wks[nz_wks_entry+1],
      (nz_wks_count - nz_wks_entry - 1)*sizeof(NZ_WKS));
    nz_wks_count--;
  }


#ifdef DEBUG
  printf("nz_wks_entry=%d   nz_wks_count=%d   nz_wks_size=%d\n",
    nz_wks_entry,  nz_wks_count, nz_wks_size );
  printf("erase ok\n");
#endif

  return (TSuccess);
}

/*----------------------------------------------------------------------*/


/*
* gestion d'un echo de designation du triedre non zoomable d' une vue.
*/
TStatus call_triedron_echo (
                            CALL_DEF_VIEW * aview,
                            int aType
                            )

{
  return (TSuccess);
}


/*----------------------------------------------------------------------*/

/*
* initialisation of zbuffer trihedron
*/
extern TStatus  call_ztriedron_setup (
                                      float* xcolor,
                                      float* ycolor,
                                      float* zcolor,
                                      float  sizeratio,
                                      float  axisdiameter,
                                      int    nbfacettes)
{
  theXColor[0] = xcolor[0];
  theXColor[1] = xcolor[1];
  theXColor[2] = xcolor[2];

  theYColor[0] = ycolor[0];
  theYColor[1] = ycolor[1];
  theYColor[2] = ycolor[2];

  theZColor[0] = zcolor[0];
  theZColor[1] = zcolor[1];
  theZColor[2] = zcolor[2];

  theRatio = sizeratio;
  theDiametr = axisdiameter;
  theNBFacettes = nbfacettes;

  return (TSuccess);
}

