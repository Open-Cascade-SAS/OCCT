# CSF variables definition

if (NOT DEFINED USE_TCL)
  OCCT_IS_PRODUCT_REQUIRED (CSF_TclLibs USE_TCL)
endif()

if (USE_TCL)
  if ("${3RDPARTY_TCL_LIBRARY_VERSION}" STREQUAL "")
    message (WARNING "TCL version has not been specified by CSF_TclLibs defining")
    message (WARNING "thus it will be used as 8.6")
    set (3RDPARTY_TCL_LIBRARY_VERSION "8.6")
  endif()

  if ("${3RDPARTY_TK_LIBRARY_VERSION}" STREQUAL "")
    message (WARNING "TK version has not been specified by CSF_TclTkLibs defining")
    message (WARNING "thus it will be used as 8.6")
    set (3RDPARTY_TK_LIBRARY_VERSION "8.6")
  endif()
endif()

if (USE_VTK AND NOT VTK_LIBRARY_NAMES)
  message (WARNING "CSF_VTK specification: VTK libraries are not defined")
endif()

if (USE_TBB)
  set (CSF_TBB "tbb tbbmalloc")
else()
  set (CSF_TBB)
endif()

if (WIN32)
  set (CSF_advapi32      "advapi32.lib")
  set (CSF_gdi32         "gdi32.lib")
  set (CSF_user32        "user32.lib")
  set (CSF_wsock32       "wsock32.lib")
  set (CSF_AviLibs       "ws2_32.lib vfw32.lib")
  set (CSF_OpenGlLibs    "opengl32.lib glu32.lib")

  if (USE_TCL)
    set (CSF_TclLibs     "tcl${3RDPARTY_TCL_LIBRARY_VERSION}.lib")
    set (CSF_TclTkLibs   "tk${3RDPARTY_TK_LIBRARY_VERSION}.lib")
  endif()

  if (USE_VTK)
    set (CSF_VTK "${VTK_LIBRARY_NAMES}")
  else()
    set (CSF_VTK)
  endif()

else()

  #-- Tcl/Tk configuration
  if (USE_TCL)
    set (CSF_TclLibs     "tcl${3RDPARTY_TCL_LIBRARY_VERSION}")
    set (CSF_TclTkLibs   "X11 tk${3RDPARTY_TK_LIBRARY_VERSION}")
  endif()

  if(APPLE)
    set (CSF_objc        "objc")

    # frameworks
    set (CSF_Appkit      "Appkit")
    set (CSF_IOKit       "IOKit")
    set (CSF_OpenGlLibs  "OpenGL")
    
    set (CSF_TclLibs     "Tcl")
    set (CSF_TclTkLibs   "Tk")
  elseif(ANDROID)
    set (CSF_ThreadLibs  "c")
    set (CSF_OpenGlLibs  "EGL GLESv2")
  elseif (UNIX)
    set (CSF_ThreadLibs  "pthread rt")
    set (CSF_OpenGlLibs  "GLU GL")
  endif()

  if (NOT DEFINED ANDROID)
    set (CSF_XwLibs      "X11 Xext Xmu Xi")
    set (CSF_MotifLibs   "X11")
  endif()

  if (USE_FREETYPE)
    set (CSF_FREETYPE "freetype")
  else()
    set (CSF_FREETYPE)
  endif()

  if (USE_FREEIMAGE)
    set (CSF_FreeImagePlus "freeimage")
  else()
    set (CSF_FreeImagePlus)
  endif()

  if (NOT DEFINED ANDROID)
    if (USE_GL2PS)
      set (CSF_GL2PS "gl2ps")
    else()
      set (CSF_GL2PS)
    endif()
  endif()
  
  if (USE_VTK)
    set (CSF_VTK "${VTK_LIBRARY_NAMES}")
  else()
    set (CSF_VTK)
  endif()
endif()