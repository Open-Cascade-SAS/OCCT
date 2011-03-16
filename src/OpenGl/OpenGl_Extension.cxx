/*
* Fonction
* ~~~~~~~~
*   Gestion des extensions sous OpenGL
*
*
* Attention:
* ~~~~~~~~~~~
*  Ce package a ete teste sur SGI, OSF, SUN,  HP et WNT.
*
* Remarques:
* ~~~~~~~~~~~
*  Le InitExtensionGLX permet d'initialiser le display. Ceci est necessaire
*  pour travailler sur les extensions de GLX. On ne peut appeler QueryExtensionGLX
*  si on n'a pas fait cette manip.
*  Par contre QueryExtension gere les extensions a GL,  on n'a pas besoin du
*  Display.
*
*  Pour l'instant on ne gere pas les extensions a GLU et a WGL.
*
* Historique des modifications
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*   14-10-97: FMN ; creation
*   23-10-97: FMN ; ajout gestion glx
*   04-11-97: FMN ; Gestion des differentes versions GLX
*   19-11-97: FMN ; Ajout GetCurrentDisplay
*   04-12-97: FMN ; Ajout supportsOneDotOne
*   22-07-98: FMN ; InitExtensionGLX n'est execute qu'une fois
*   28-07-98: FMN ; Renomme theDisplay en mytheDisplay
*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/*
* Includes
*/

#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

#include <OpenGl_Extension.hxx>

#define XDEBUG 

/*----------------------------------------------------------------------*/
/*
* Types definis
*/


/*----------------------------------------------------------------------*/
/*
* Variables statiques
*/

static Display *mytheDisplay = NULL;
static int  screen_num;
static int  GLXmajor, GLXminor;
static int  GLmajor, GLminor;

static GLboolean flag_InitExtensionGLX = GL_FALSE;

static GLboolean OneDotOne = GL_FALSE;
static GLboolean initOneDotOne = GL_FALSE;

/*----------------------------------------------------------------------*/
/*
* Fonctions publiques 
*/

/*----------------------------------------------------------------------*/
extern GLboolean CheckExtension(char *extName, const char *extString)
{
  /*
  ** Search for extName in the extensions string.  Use of strstr()
  ** is not sufficient because extension names can be prefixes of
  ** other extension names.  Could use strtok() but the constant
  ** string returned by glGetString can be in read-only memory.
  */
  char *p = (char *)extString;
  char *end;
  int extNameLen;

  extNameLen = strlen(extName);
  end = p + strlen(p);

  while (p < end) {
    int n = strcspn(p, " ");
    if ((extNameLen == n) && (strncmp(extName, p, n) == 0)) {
      return GL_TRUE;
    }
    p += (n + 1);
  }
  return GL_FALSE;
}

extern GLboolean InitExtensionGLX(Display *display)
{
#ifndef WNT 

  int dontcare; /* for returned arguments we don't care about */

  if (display == NULL) return GL_FALSE;

  if (!flag_InitExtensionGLX)
  {       
    /* does the server know about OpenGL & GLX? */
    if(!XQueryExtension(display, "GLX", &dontcare, &dontcare, &dontcare)) {
#ifdef DEBUG
      fprintf(stderr,"This system doesn't appear to support OpenGL\n");
#endif /* DEBUG */
      return GL_FALSE;
    }  

    /* find the glx version */
    if(glXQueryVersion(display, &GLXmajor, &GLXminor)) {
#ifdef DEBUG
      printf("GLX Version: %d.%d\n", GLXmajor, GLXminor);
#endif /* DEBUG */
    } else {
#ifdef DEBUG
      fprintf(stderr, "Error: glXQueryVersion() failed.\n");
#endif /* DEBUG */
      return GL_FALSE;
    }

    /* get screen number */
    screen_num = DefaultScreen(display);

    flag_InitExtensionGLX = GL_TRUE;
    mytheDisplay = display;

  } /* (!flag_InitExtensionGLX) */

  return GL_TRUE;

#else

  return GL_FALSE;

#endif /* WNT */
}

/*----------------------------------------------------------------------*/
extern GLboolean QueryExtensionGLX(char *extName)
{
  GLboolean result = GL_FALSE;

#ifdef GLX_VERSION_1_1
  if (flag_InitExtensionGLX)
  {
    if ( GLXminor > 1 || GLXmajor > 1 ) /* GLX_VERSION_1_2 */
    {
      /* Certaines extensions sont par defaut dans la version 1.2 */
      if (strcmp(extName,"GLX_EXT_import_context")) return GL_TRUE;
      result = CheckExtension(extName, glXQueryExtensionsString(mytheDisplay, screen_num));
    }
    else if( GLXminor > 0 || GLXmajor > 1 ) /* GLX_VERSION_1_1 */
    {
      result = CheckExtension(extName, glXQueryExtensionsString(mytheDisplay, screen_num));
    }
  }
#endif    
#ifdef DEBUG
  printf("QueryExtensionGLX: %s %d", extName, result);
#endif 
  return result;
}

/*----------------------------------------------------------------------*/
extern Display *GetCurrentDisplay(void)
{
#ifdef DEBUG
  printf("GetCurrentDisplay %x \n", mytheDisplay);
#endif 
  return mytheDisplay;  
}

/*----------------------------------------------------------------------*/
extern GLboolean QueryExtension(char *extName)
{
  GLboolean result = GL_FALSE;

  if (supportsOneDotOne()) /* GL_VERSION_1_1 ou more */
  {
    /* Certaines extensions sont par defaut dans la version 1.1 */
    /* Certain extensions are the defaut in version 1.1 */
    if ((strcmp(extName,"GL_EXT_vertex_array")) ||  
      (strcmp(extName,"GL_EXT_polygon_offset")) ||  
      (strcmp(extName,"GL_EXT_blend_logic_op")) ||  
      (strcmp(extName,"GL_EXT_texture"))    ||
      (strcmp(extName,"GL_EXT_copy_texture")) ||
      (strcmp(extName,"GL_EXT_subtexture")) ||  
      (strcmp(extName,"GL_EXT_texture_object"))) 
      result =  GL_TRUE;  
    else  
      result = CheckExtension(extName, (char *)glGetString(GL_EXTENSIONS)); 
  }
  else  /* GL_VERSION_1_0 */
  {
    result = CheckExtension(extName, (char *)glGetString(GL_EXTENSIONS)); 
  }

#ifdef DEBUG
  printf("QueryExtension: %s %d \n", extName, result);
#endif 
  return result;
}
/*----------------------------------------------------------------------*/

extern GLboolean supportsOneDotOne(void)
{
  const GLubyte *version;

  if (!initOneDotOne)
  {
    version = glGetString(GL_VERSION);
    if(sscanf((const char *)version, "%d.%d", &GLmajor, &GLminor) == 2)
    {
      OneDotOne = (GLmajor>=1 && GLminor >=1);
    }
    initOneDotOne = GL_TRUE;
  }
#ifdef DEBUG
  printf("GL Version: %d.%d\n", GLmajor, GLminor);
#endif /* DEBUG */
  return OneDotOne;
}

/*----------------------------------------------------------------------*/
