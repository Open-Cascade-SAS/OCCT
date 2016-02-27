# CSF variables definition

if(CSFS_ALREADY_INCLUDED)
  return()
endif()
set(CSFS_ALREADY_INCLUDED 1)


if (NOT DEFINED USE_TCL)
  OCCT_IS_PRODUCT_REQUIRED (CSF_TclLibs USE_TCL)
endif()

if (USE_TCL)
  if ("${3RDPARTY_TCL_LIBRARY_VERSION}" STREQUAL "")
    message (STATUS "Warning: TCL version has not been specified by CSF_TclLibs defining thus it will be used as 8.6")
    set (3RDPARTY_TCL_LIBRARY_VERSION "8.6")
  endif()

  if ("${3RDPARTY_TK_LIBRARY_VERSION}" STREQUAL "")
    message (STATUS "Warning: TK version has not been specified by CSF_TclTkLibs defining thus it will be used as 8.6")
    set (3RDPARTY_TK_LIBRARY_VERSION "8.6")
  endif()
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
  set (CSF_winspool      "Winspool.lib")
  set (CSF_psapi         "Psapi.lib")
  set (CSF_AviLibs       "ws2_32.lib vfw32.lib")
  set (CSF_OpenGlLibs    "opengl32.lib glu32.lib")

  if (USE_FREETYPE)
    set (CSF_FREETYPE "freetype.lib")
  else()
    set (CSF_FREETYPE)
  endif()

  if (USE_FREEIMAGE)
    set (CSF_FreeImagePlus "freeimage.lib freeimageplus.lib")
  else()
    set (CSF_FreeImagePlus)
  endif()

  if (USE_TCL)
    set (CSF_TclLibs     "tcl${3RDPARTY_TCL_LIBRARY_VERSION}.lib")
    set (CSF_TclTkLibs   "tk${3RDPARTY_TK_LIBRARY_VERSION}.lib")
  endif()

else()

  if (APPLE)
    set (CSF_objc        "objc")

    # frameworks
    find_library (Appkit_LIB NAMES Appkit)
    set (CSF_Appkit ${Appkit_LIB})

    find_library (IOKit_LIB NAMES IOKit)
    set (CSF_IOKit ${IOKit_LIB})

    OCCT_CHECK_AND_UNSET (Appkit_LIB)
    OCCT_CHECK_AND_UNSET (IOKit_LIB)

    if (USE_GLX)
      set (CSF_OpenGlLibs GL)
      set (CSF_XwLibs     "X11 Xext Xmu Xi")
    else()
      find_library (OpenGlLibs_LIB NAMES OpenGL)
      set (CSF_OpenGlLibs ${OpenGlLibs_LIB})

      OCCT_CHECK_AND_UNSET (OpenGlLibs_LIB)
    endif()
    
    if (USE_TCL)
      set (CSF_TclTkLibs Tk)
      set (CSF_TclLibs   Tcl)
    endif()
  elseif (ANDROID)
    set (CSF_ThreadLibs  "c")
    set (CSF_OpenGlLibs  "EGL GLESv2")
  elseif (UNIX)
    set (CSF_ThreadLibs  "pthread rt stdc++")
    set (CSF_OpenGlLibs  "GLU GL")
    set (CSF_XwLibs      "X11 Xext Xmu Xi")
    set (CSF_dl          "dl")

    if (USE_TCL)
      set (CSF_TclLibs     "tcl${3RDPARTY_TCL_LIBRARY_VERSION}")
      set (CSF_TclTkLibs   "tk${3RDPARTY_TK_LIBRARY_VERSION}")
    endif()
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
endif()