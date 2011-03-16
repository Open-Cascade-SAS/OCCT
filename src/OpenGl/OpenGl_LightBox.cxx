/***********************************************************************

FONCTION :
----------
Gestion des light sous OpenGL


REMARQUES:
---------- 

- We can't take in account GL_QUADRATIC_ATTENUATION.    

- Position des lumieres. Il faut faire le glLight(GL_POSITION) apres la 
gestion des matrices de transformation (GL_MODELVIEW).

- Se mefier de GL_POSITION pour les lumieres directionelle. GL_POSITION
indique bien la position ou se trouve la lumiere. Comme on a une direction
comme valeur il faut l'inverser. 


FONCTIONNEMENT:
---------------
- La variable lightOn permet d'optimiser le changement d'etat des lumieres.
Ceci permet d'eviter d'appeler glIsEnable(GL_LIGHTING). Il faut bien sur
que l'on utilise les 2 points d'entree LightOn() et LightOff().
L'init de la variable est faite par call_func_redraw_all_structs (OpenGl_funcs.c)


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
20-06-97 : PCT ; creation
30-06-96 : FMN ; Integration
18-07-96 : FMN, PCT ; Correction indice,  ajout IsLightOn()
25-07-97 : CAL ; Portage NT (include OpenGl_tgl_all.h)
07-10-97 : FMN ; Simplification WNT

************************************************************************/

/*----------------------------------------------------------------------*/
/*
* Includes
*/

#include <OpenGl_tgl_all.hxx>

#include <stdio.h>
#include <stdlib.h>

#include <math.h>
#include <memory.h>

#include <OpenGl_LightBox.hxx>
#include <OpenGl_Memory.hxx>

/*----------------------------------------------------------------------*/
/*
* Constantes
*/
#ifndef DEBUG
#define DEBUG 0
#endif

#define NO_PRINT_DEBUG

#define GROW_SIZE_WKS 10
#define GROW_SIZE_LIGHT 8

#ifndef M_PI
# define M_PI          3.14159265358979323846
#endif

/*----------------------------------------------------------------------*/
/*
* Types definis
*/
struct TEL_LIGHT_DATA
{
  Tint LightID;
  TEL_LIGHT light;
  IMPLEMENT_MEMORY_OPERATORS
};

struct TEL_LIGHT_WKS
{
  Tint wks;
  int lights_count;
  int lights_size;
  TEL_LIGHT_DATA *lights;
  IMPLEMENT_MEMORY_OPERATORS
};


/*----------------------------------------------------------------------*/
/*
* Variables statiques
*/

static TEL_LIGHT_WKS *wks = NULL;
static int wks_count = 0;
static int wks_size = 0;

static GLfloat default_amb[4]    = { 0.0, 0.0, 0.0, 1.0 };
static GLfloat default_sptdir[3] = { 0.0, 0.0, -1.0 };
static GLfloat default_sptexpo   = 0.0;
static GLfloat default_sptcutoff = 180.0;

static GLboolean lightOn = GL_FALSE;

/*----------------------------------------------------------------------*/
/*
* Fonctions privees
*/

/*-----------------------------------------------------------------*/
/*
*  Set des lumieres
*/
static void bind_light(TEL_LIGHT *lptr, int *gl_lid)
{
  GLfloat data_amb[4];
  GLfloat data_diffu[4];
  GLfloat data_pos[4];
  GLfloat data_sptdir[3];
  GLfloat data_sptexpo;
  GLfloat data_sptcutoff;
  GLfloat data_constantattenuation;
  GLfloat data_linearattenuation;
  GLint cur_matrix;


  /* on n'a droit qu'a 8 lights avec OpenGL... */
  if (*gl_lid > GL_LIGHT7) return;


  /* la light est une headlight ? */
  if (lptr->HeadLight)
  {
    glGetIntegerv(GL_MATRIX_MODE, &cur_matrix);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
  }


  /* set la light en fonction de son type */
  switch (lptr->type)
  {
  case TLightAmbient:
    data_amb[0] = lptr->col.rgb[0];
    data_amb[1] = lptr->col.rgb[1];
    data_amb[2] = lptr->col.rgb[2];
    data_amb[3] = 1.0;

    /*------------------------- Ambient ---------------------------*/
    /*
    * The GL_AMBIENT parameter refers to RGBA intensity of the ambient
    * light.
    */
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, data_amb); 

#if DEBUG
    printf("OpenGL_LightBox::bind_light:LightAmbient \n");
    printf("\tGL_LIGHT_MODEL_AMBIENT %f %f %f \n", data_amb[0], data_amb[1], data_amb[2]);  
#endif
    break;


  case TLightDirectional:
    data_diffu[0] = lptr->col.rgb[0];
    data_diffu[1] = lptr->col.rgb[1];
    data_diffu[2] = lptr->col.rgb[2];
    data_diffu[3] = 1.0;

    /*------------------------- Direction ---------------------------*/
    /* From Open GL Programming Rev 1 Guide Chapt 6 :
    Lighting The Mathematics of Lighting ( p 168 )

    Directional Light Source ( Infinite ) :
    if the last parameter of GL_POSITION , w , is zero, the
    corresponding light source is a Directional one.

    GL_SPOT_CUTOFF a 180 signifie que ce n'est pas un spot.
    To create a realistic effect,  set the GL_SPECULAR parameter 
    to the same value as the GL_DIFFUSE.
    */

    data_pos[0] = -lptr->dir[0];
    data_pos[1] = -lptr->dir[1];
    data_pos[2] = -lptr->dir[2];
    data_pos[3] = 0.0;

    glLightfv(*gl_lid, GL_AMBIENT, default_amb);
    glLightfv(*gl_lid, GL_DIFFUSE, data_diffu);
    glLightfv(*gl_lid, GL_SPECULAR, data_diffu);

    glLightfv(*gl_lid, GL_POSITION, data_pos);
    glLightfv(*gl_lid, GL_SPOT_DIRECTION, default_sptdir);
    glLightf(*gl_lid, GL_SPOT_EXPONENT, default_sptexpo);
    glLightf(*gl_lid, GL_SPOT_CUTOFF, default_sptcutoff);

#if DEBUG
    printf("OpenGL_LightBox::bind_light:LightDirection \n");
    printf("\tGL_AMBIENT %f %f %f \n", default_amb[0], default_amb[1], default_amb[2]); 
    printf("\tGL_DIFFUSE %f %f %f \n", data_diffu[0], data_diffu[1], data_diffu[2]);  
    printf("\tGL_SPECULAR %f %f %f \n", data_diffu[0], data_diffu[1], data_diffu[2]); 
    printf("\tGL_POSITION %f %f %f \n", data_pos[0], data_pos[1], data_pos[2]); 
#endif
    break;


  case TLightPositional:
    data_diffu[0] = lptr->col.rgb[0];
    data_diffu[1] = lptr->col.rgb[1];
    data_diffu[2] = lptr->col.rgb[2];
    data_diffu[3] = 1.0;

    /*------------------------- Position -----------------------------*/
    /* From Open GL Programming Rev 1 Guide Chapt 6 :
    Lighting The Mathematics of Lighting ( p 168 )
    Positional Light Source :
    if the last parameter of GL_POSITION , w , is nonzero,
    the corresponding light source is a Positional one.

    GL_SPOT_CUTOFF a 180 signifie que ce n'est pas un spot.

    To create a realistic effect,  set the GL_SPECULAR parameter 
    to the same value as the GL_DIFFUSE.
    */

    data_pos[0] = lptr->pos[0];
    data_pos[1] = lptr->pos[1];
    data_pos[2] = lptr->pos[2];
    data_pos[3] = 1.0;

    data_constantattenuation = lptr->atten[0];
    data_linearattenuation = lptr->atten[1];

    glLightfv(*gl_lid, GL_AMBIENT, default_amb);
    glLightfv(*gl_lid, GL_DIFFUSE, data_diffu);
    glLightfv(*gl_lid, GL_SPECULAR, data_diffu);

    glLightfv(*gl_lid, GL_POSITION, data_pos);
    glLightfv(*gl_lid, GL_SPOT_DIRECTION, default_sptdir);
    glLightf(*gl_lid, GL_SPOT_EXPONENT, default_sptexpo);
    glLightf(*gl_lid, GL_SPOT_CUTOFF, default_sptcutoff);
    glLightf(*gl_lid, GL_CONSTANT_ATTENUATION, data_constantattenuation);
    glLightf(*gl_lid, GL_LINEAR_ATTENUATION, data_linearattenuation);
    glLightf(*gl_lid, GL_QUADRATIC_ATTENUATION, 0.0); 

#if DEBUG
    printf("OpenGL_LightBox::bind_light:LightPosition \n");
    printf("\tGL_AMBIENT %f %f %f \n", default_amb[0], default_amb[1], default_amb[2]); 
    printf("\tGL_DIFFUSE %f %f %f \n", data_diffu[0], data_diffu[1], data_diffu[2]);  
    printf("\tGL_SPECULAR %f %f %f \n", data_diffu[0], data_diffu[1], data_diffu[2]); 
    printf("\tGL_POSITION %f %f %f \n", data_pos[0], data_pos[1], data_pos[2]); 
#endif
    break;


  case TLightSpot:
    data_diffu[0] = lptr->col.rgb[0];
    data_diffu[1] = lptr->col.rgb[1];
    data_diffu[2] = lptr->col.rgb[2];
    data_diffu[3] = 1.0;

    data_pos[0] = lptr->pos[0];
    data_pos[1] = lptr->pos[1];
    data_pos[2] = lptr->pos[2];
    data_pos[3] = 1.0;

    data_sptdir[0] = lptr->dir[0];
    data_sptdir[1] = lptr->dir[1];
    data_sptdir[2] = lptr->dir[2];

    data_sptexpo = ( float )lptr->shine * 128.0F;
    data_sptcutoff = ( float )(lptr->angle * 180.0F)/( float )M_PI;

    data_constantattenuation = lptr->atten[0];
    data_linearattenuation = lptr->atten[1];

    glLightfv(*gl_lid, GL_AMBIENT, default_amb);
    glLightfv(*gl_lid, GL_DIFFUSE, data_diffu);
    glLightfv(*gl_lid, GL_SPECULAR, data_diffu);   

    glLightfv(*gl_lid, GL_POSITION, data_pos);      
    glLightfv(*gl_lid, GL_SPOT_DIRECTION, data_sptdir);
    glLightf(*gl_lid, GL_SPOT_EXPONENT, data_sptexpo);
    glLightf(*gl_lid, GL_SPOT_CUTOFF, data_sptcutoff);
    glLightf(*gl_lid, GL_CONSTANT_ATTENUATION, data_constantattenuation);
    glLightf(*gl_lid, GL_LINEAR_ATTENUATION, data_linearattenuation);
    glLightf(*gl_lid, GL_QUADRATIC_ATTENUATION, 0.0); 

#if DEBUG
    printf("OpenGL_LightBox::bind_light:LightSpot \n");
    printf("\tGL_AMBIENT %f %f %f \n", default_amb[0], default_amb[1], default_amb[2]); 
    printf("\tGL_DIFFUSE %f %f %f \n", data_diffu[0], data_diffu[1], data_diffu[2]);  
    printf("\tGL_SPECULAR %f %f %f \n", data_diffu[0], data_diffu[1], data_diffu[2]); 
    printf("\tGL_POSITION %f %f %f \n", data_pos[0], data_pos[1], data_pos[2]); 
    printf("\tGL_SPOT_DIRECTION %f %f %f \n", data_sptdir[0], data_sptdir[1], data_sptdir[2]);  
    printf("\tGL_SPOT_EXPONENT %f \n", data_sptexpo); 
    printf("\tGL_SPOT_CUTOFF   %f \n", data_sptcutoff); 
    printf("\tGL_CONSTANT_ATTENUATION %f \n", data_constantattenuation);  
    printf("\tGL_LINEAR_ATTENUATION   %f \n", data_linearattenuation);  
#endif
    break;
  }


  if (lptr->type != TLightAmbient) 
  {  
#if DEBUG
    printf("OpenGL_LightBox::bind_light:glEnable %d \n", *gl_lid);
#endif
    glEnable(*gl_lid);
    (*gl_lid)++;
  }


  /* si la light etait une headlight alors restaure la matrice precedente */
  if (lptr->HeadLight)
  {
    glPopMatrix();
    glMatrixMode(cur_matrix);
  }
}


/*-----------------------------------------------------------------*/
/*
* recherche de la liste de lampe d'une wks, creation d'une liste si non existante
*/
static int find_wks(Tint WksID, int alloc)
{
  int i;

  /* recherche la wks dans la liste si elle existe */
  for (i=0; i<wks_count; i++)
    if (wks[i].wks == WksID)
      return i;

  if (!alloc) return -1;

  /* la wks n'existe pas => on fait de la place si yen a plus */
  if (wks_count == wks_size )
  {
    wks_size += GROW_SIZE_WKS;
    if (!wks)
      wks = new TEL_LIGHT_WKS[wks_size];
    else
#if defined(__SUNPRO_CC) && (__SUNPRO_CC <= 0x530)
      wks = (TEL_LIGHT_WKS*)realloc(wks, wks_size*sizeof(TEL_LIGHT_WKS));
#else
      wks = cmn_resizemem<TEL_LIGHT_WKS>(wks, wks_size);
#endif

    if (!wks)
      return -1;
  }

  wks[wks_count].wks = WksID;
  wks[wks_count].lights = NULL;
  wks[wks_count].lights_size = 0;
  wks[wks_count].lights_count = 0;

  return wks_count++;
}

/*-----------------------------------------------------------------*/
/*
* recherche une lampe d'une wks, creation d'une lampe si elle n'existe pas
*/
static int find_light(int WksIdx, Tint LightID, int alloc)
{
  int i;
  TEL_LIGHT_DATA *lights;


  /* recherche la light dans la liste de la wks */
  lights = wks[WksIdx].lights;
  for (i=0; i<wks[WksIdx].lights_count; i++)
    if (lights[i].LightID == LightID)
      return i;

  if (!alloc) return -1;

  /* la lampe n'existe pas => on cree une lampe */
  if (wks[WksIdx].lights_count == wks[WksIdx].lights_size)
  {
    wks[WksIdx].lights_size += GROW_SIZE_LIGHT;
    if (!wks[WksIdx].lights)
      wks[WksIdx].lights = new TEL_LIGHT_DATA[wks[WksIdx].lights_size];
    else
      wks[WksIdx].lights =
#if defined(__SUNPRO_CC) && (__SUNPRO_CC <= 0x530)
      (TEL_LIGHT_DATA*)realloc( wks[WksIdx].lights,
      wks[WksIdx].lights_size*sizeof(TEL_LIGHT_DATA) );
#else
      cmn_resizemem<TEL_LIGHT_DATA>( wks[WksIdx].lights,
      wks[WksIdx].lights_size );
#endif
    if (!wks[WksIdx].lights)
      return -1;
  }

  return wks[WksIdx].lights_count++;
}


/*----------------------------------------------------------------------*/
/*
* Fonctions publiques 
*/


/*-----------------------------------------------------------------*/
/*
* Ajout d'une lumiere dans la Wks
*/
TStatus AddLight(Tint WksID, Tint LightID, tel_light light)
{
  int wks_entry;
  int light_entry;

#if DEBUG
  printf("AddLight %d dans wks %d  [wds_count=%d]\n", LightID, WksID, wks_count);
#endif

  /* obtient le numero de la liste de lights de la wks */
  wks_entry = find_wks(WksID, 1);
  if (wks_entry == -1) return TFailure;

  /* recherche le numero de la lampe si elle existe */
  light_entry = find_light(wks_entry, LightID, 1);
  if (light_entry == -1) return TFailure;

  /* met a jour la light */
  wks[wks_entry].lights[light_entry].LightID = LightID;
  wks[wks_entry].lights[light_entry].light = *light;

#if DEBUG
  printf("ajout ok\n");
#endif
  return TSuccess;
}


/*-----------------------------------------------------------------*/
/*
* Maj des lumieres de la Wks
*/
TStatus UpdateLight(Tint WksID)
{
  int wks_entry;
  int i;
  int gl_lid;


#if DEBUG
  printf("UpdateLight %d\n", WksID);
#endif

  /* vire toutes les lights des le depart avant une re-init complete */
  for (i=GL_LIGHT0; i<=GL_LIGHT7; i++)
    glDisable(i);
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, default_amb); 

  /* recherche la liste de light de la wks */
  wks_entry = find_wks(WksID, 0);
  if (wks_entry == -1) return TFailure;

#if DEBUG
  printf("*** Update:  nb = %d\n", wks[wks_entry].lights_count);
#endif

  /* set les lights */
  gl_lid = GL_LIGHT0;
  for (i=0; i<wks[wks_entry].lights_count; i++)
  {
#if DEBUG
    printf("binding light %d\n", i);
#endif
    bind_light(&wks[wks_entry].lights[i].light, &gl_lid);
  }

  if (wks[wks_entry].lights_count > 0) LightOn(); 

#if DEBUG
  printf("update ok\n");
#endif

  return TSuccess;
}


/*-----------------------------------------------------------------*/
/*
* Remove une lumiere de la Wks
*/
TStatus RemoveLight(Tint WksID, Tint LightID)
{
  int wks_entry;
  int light_entry;


  /* recherche de la wks */
  wks_entry = find_wks(WksID, 0);
  if (wks_entry == -1) return TFailure;

  /* recherche de la light */
  light_entry = find_light(wks_entry, LightID, 0);
  if (light_entry == -1) return TFailure;

  /* retire la light */
  memcpy(&wks[wks_entry].lights[light_entry],
    &wks[wks_entry].lights[light_entry+1],
    (wks[wks_entry].lights_count - light_entry - 1)*sizeof(TEL_LIGHT_DATA));  
  wks[wks_entry].lights_count--;

#if DEBUG
  printf("RemoveLight %d dans wks %d  [wds_count=%d]\n", LightID, WksID, wks_count);
#endif

  return TSuccess;
}


/*-----------------------------------------------------------------*/
/*
* Remove des lumieres de la Wks
*/
TStatus RemoveWksLight(Tint WksID)
{
  int wks_entry;

  /* recherche de la wks */
  wks_entry = find_wks(WksID, 0);
  if (wks_entry == -1) return TFailure;

  /* destruction de toute la wks */
  free(wks[wks_entry].lights);
  memcpy(&wks[wks_entry],
    &wks[wks_entry+1],
    (wks_count - wks_entry - 1)*sizeof(TEL_LIGHT_WKS));
  wks_count--;

  return TSuccess;
}


/*-----------------------------------------------------------------*/
/*
* Reset de toutes les lights d'une Wks
*/
TStatus ResetWksLight(Tint WksID)
{
  int wks_entry;

  /* recherche de la wks */
  wks_entry = find_wks(WksID, 0);
  if (wks_entry == -1) return TFailure;

  /* destruction de toutes les lights */  
  wks[wks_entry].lights_count = 0;

  return TSuccess;
}


/*-----------------------------------------------------------------*/
/*
* Enable des lights
*/
void LightOn(void)
{
#ifdef PRINT_DEBUG
  if(IsLightOn())
    printf("LightOn(): lighting already enabled!");
  else
    printf("LightOn() succeeded");
#endif
  glEnable(GL_LIGHTING);
}


/*-----------------------------------------------------------------*/
/*
* Disable des lights
*/
void LightOff(void)
{
#ifdef PRINT_DEBUG
  if(!IsLightOn())
    printf("LightOff(): lighting already disabled!");
  else
    printf("LightOff() succeeded");
#endif
  glDisable(GL_LIGHTING);
}
/*-----------------------------------------------------------------*/

/*
* IsEnable des lights
*/

GLboolean IsLightOn(void)
{
  return glIsEnabled(GL_LIGHTING);
}
/*-----------------------------------------------------------------*/
