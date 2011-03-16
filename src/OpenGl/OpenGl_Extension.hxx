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
 *   14-10-97: FMN ; Creation
 *   23-10-97: FMN ; Ajout gestion glx
 *   19-11-97: FMN ; Ajout GetCurrentDisplay
 *   04-12-97: FMN ; Ajout supportsOneDotOne
 *   19-06-98: FMN ; Portage Optimizer (C++)
 */
/*----------------------------------------------------------------------*/

#ifndef _OPENGL_EXTENSION_H__
#define _OPENGL_EXTENSION_H__

/*----------------------------------------------------------------------*/
/*
 * Includes
 */

#if defined(WNT) && !defined(HAVE_NO_DLL)
# ifdef __OpenGl_DLL
#  define EXPORT __declspec(dllexport)
# else
#  define EXPORT
# endif  /* DLL */
# ifdef STRICT
#  undef STRICT
# endif
# define STRICT
# include <windows.h>
#else
# define EXPORT
#endif  /* WNT */

#include <GL/gl.h>
#include <GL/glu.h>

#ifdef WNT
# include <windows.h>
# ifndef Display
# define Display char
# endif  /* Display */
#else
# include <GL/glx.h>
#endif /* WNT */


#ifdef GL_VERSION_1_1
#define GL_EXT_vertex_array                 1
#define GL_EXT_polygon_offset               1
#define GL_EXT_blend_logic_op               1
#define GL_EXT_texture                      1
#define GL_EXT_copy_texture                 1
#define GL_EXT_subtexture                   1
#define GL_EXT_texture_object               1
#endif /* GL_VERSION_1_1 */


#ifndef  GLU_VERSION_1_2
#define GLUtesselator    GLUtriangulatorObj
#define GLU_TESS_BEGIN   100100
#define GLU_TESS_VERTEX  100101
#define GLU_TESS_END     100102
#define GLU_TESS_ERROR   100103
#define GLU_TESS_COMBINE 100105
#endif

#define INVALID_EXT_FUNCTION_PTR 0xffffffff
/* 
 * Contournement temporaire glPolygoneOffsetEXT 
 * La syntaxe change entre OpenGL 1.0 et OpenGL 1.1
 */
 
#if defined (__sun) || defined (__osf__) || defined (__hp)
#define glPolygonOffsetEXT(a, b) glPolygonOffset(a, b)
#endif
#if defined (__sun) 
#define GL_POLYGON_OFFSET_EXT   GL_POLYGON_OFFSET_FILL            
#endif

#ifdef WNT
#define glPolygonOffsetEXT(a, b) glPolygonOffset(a, b)
#define GL_POLYGON_OFFSET_EXT   GL_POLYGON_OFFSET_FILL            
#endif /* WNT */

#if defined (__sun) || defined (__osf__) || defined (__hp) || defined (__sgi) 
#else 
typedef void (APIENTRY * PFNGLBLENDEQUATIONEXTPROC) (GLenum mode);
#endif

/*----------------------------------------------------------------------*/
/*
 * Prototypes
 */

/*
 * Points d'entree Public du module 
 */


extern GLboolean InitExtensionGLX(Display *display);
extern GLboolean QueryExtensionGLX(char *extName);

extern GLboolean QueryExtension(char *extName);

extern Display *GetCurrentDisplay(void);

extern GLboolean supportsOneDotOne(void);

extern GLboolean CheckExtension(char *extName, const char *extString);


/* Methods defined in OpenGl_GraphicDriver.cxx */

EXPORT GLboolean OpenGl_QueryExtensionGLX (const char *extName);

EXPORT GLboolean OpenGl_QueryExtension    (const char *extName);

/*----------------------------------------------------------------------*/

#endif /* _OPENGL_EXTENSION_H__ */
