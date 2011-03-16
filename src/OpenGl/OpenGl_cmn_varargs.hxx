#ifndef  OPENGL_CMN_VARARGS_H
#define  OPENGL_CMN_VARARGS_H

#if !defined(__STDC__)
# define __STDC__ 1
#endif

#include <stdarg.h>

#if !defined(WNT)
# include <X11/Xlib.h>
# include <GL/glx.h>
#endif

typedef  union  CMN_KEY_DATA_UNION
{
  Tint    ldata;
#if !defined(WNT)
  Pixmap  pixmap;
  GLXPixmap
    glxpixmap;
#endif
  Tfloat  fdata;
  void   *pdata;
} CMN_KEY_DATA, *cmn_key_data;

typedef struct
{
  int           id;
  CMN_KEY_DATA  data;
} CMN_KEY, *cmn_key;

#if defined (SOLARIS) || defined (IRIXO32)

#define CMN_GET_STACK(n,k)  { \
  va_list p; \
  va_start (p, n); \
  if (n>=0) { \
  k = (cmn_key *)p; \
  } \
  else { \
  n = -n; \
  k = *(cmn_key **)p; \
  } \
}

#else /* DECOSF1 || HPUX || IRIX avec n32 */

#define TMaxArgs 128

#define CMN_GET_STACK(n,k)  { \
  int i; \
  va_list ap; \
  cmn_key * args; \
  va_start (ap,n); \
  if (n>=0) { \
  for (i=0; i<n && i<TMaxArgs; i++) \
  k[i] = va_arg (ap, cmn_key); \
  } \
  else { \
  n = -n; \
  args = va_arg (ap, cmn_key *); \
  for (i=0; i<n && i<TMaxArgs; i++) \
  k[i] = args[i]; \
  } \
  va_end (ap); \
}
#endif /* DECOSF1 || HPUX || IRIX avec n32 */

#endif
