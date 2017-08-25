# CSF variables definition

if(CSFS_ALREADY_INCLUDED)
  return()
endif()
set(CSFS_ALREADY_INCLUDED 1)


if (NOT DEFINED USE_TCL)
  OCCT_IS_PRODUCT_REQUIRED (CSF_TclLibs USE_TCL)
endif()

# TBB
if (USE_TBB)
  set (CSF_TBB "tbb tbbmalloc")
else()
  set (CSF_TBB)
endif()

# FREETYPE
if (USE_FREETYPE)
  set (CSF_FREETYPE "freetype")
else()
  set (CSF_FREETYPE)
endif()

# FFmpeg
if (USE_FFMPEG)
  set (CSF_FFmpeg "avcodec avformat swscale avutil")
else()
  set (CSF_FFmpeg)
endif()

# FREEIMAGE
if (USE_FREEIMAGE)
  set (CSF_FreeImagePlus "freeimage")
else()
  if (WIN32)
    set (CSF_FreeImagePlus "windowscodecs")
  else()
    set (CSF_FreeImagePlus)
  endif()
endif()

# TCL/TK
if (USE_TCL)
  if (WIN32)
    set (CSF_TclLibs     "tcl86")
    set (CSF_TclTkLibs   "tk86")
  else()
    if(APPLE)
      set (CSF_TclTkLibs Tk)
      set (CSF_TclLibs   Tcl)
    elseif(UNIX)
      set (CSF_TclLibs     "tcl8.6")
      set (CSF_TclTkLibs   "tk8.6")
    endif()
  endif()
endif()

# GL2PS
if (NOT DEFINED ANDROID)
  if (USE_GL2PS)
    set (CSF_GL2PS "gl2ps")
  else()
    set (CSF_GL2PS)
  endif()
endif()

if (WIN32)
  set (CSF_advapi32      "advapi32.lib")
  set (CSF_gdi32         "gdi32.lib")
  set (CSF_user32        "user32.lib")
  set (CSF_wsock32       "wsock32.lib")
  set (CSF_psapi         "Psapi.lib")
  if ("${CMAKE_SYSTEM_NAME}" STREQUAL "WindowsStore" OR USE_GLES2)
    set (CSF_OpenGlLibs  "libEGL libGLESv2")
  else()
    set (CSF_OpenGlLibs  "opengl32.lib")
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
    
  elseif (ANDROID)
    set (CSF_ThreadLibs  "c")
    set (CSF_OpenGlLibs  "EGL GLESv2")
  elseif (UNIX)
    set (CSF_ThreadLibs  "pthread rt stdc++")
    set (CSF_OpenGlLibs  "GL")
    set (CSF_XwLibs      "X11 Xext Xmu Xi")
    set (CSF_dl          "dl")
  endif()
endif()
