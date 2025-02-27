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

# OpenVR
if (USE_OPENVR)
  set (CSF_OpenVR "openvr_api")
else()
  set (CSF_OpenVR)
endif()

# TCL
if (USE_TCL)
  if (WIN32)
    set (CSF_TclLibs     "tcl86")
  else()
    if(APPLE)
      set (CSF_TclLibs   Tcl)
    elseif(UNIX)
      set (CSF_TclLibs   "tcl8.6")
    endif()
  endif()
endif()

# TK
if (USE_TK)
  if (WIN32)
    set (CSF_TclTkLibs   "tk86")
  else()
    if(APPLE)
      set (CSF_TclTkLibs Tk)
    elseif(UNIX)
      set (CSF_TclTkLibs "tk8.6")
    endif()
  endif()
endif()

# Draco
if (USE_DRACO)
  set (CSF_Draco "draco")
else()
  set (CSF_Draco)
endif()

# VTK
if (USE_VTK)
  # the variable must to be empty, but keep there the list of libs
  # that is used in the VTK component.
  set (CSF_VTK
  #   vtkCommonCore
  #   vtkRenderingCore
  #   vtkRenderingFreeType
  #   vtkFiltersGeneral
  #   vtkIOImage
  #   vtkImagingCore
  #   vtkInteractionStyle
  #   vtkRenderingOpenGL
  #   vtkRenderingFreeTypeOpenGL
  )
else()
  set (CSF_VTK)
endif()

if (WIN32)
  set (CSF_advapi32      "advapi32.lib")
  set (CSF_gdi32         "gdi32.lib")
  set (CSF_user32        "user32.lib")
  set (CSF_shell32       "shell32.lib")
  set (CSF_wsock32       "wsock32.lib")
  set (CSF_psapi         "psapi.lib")
  set (CSF_winmm         "winmm.lib")
  set (CSF_d3d9          "D3D9.lib")
  set (CSF_OpenGlLibs    "opengl32.lib")
  set (CSF_OpenGlesLibs  "libEGL libGLESv2")
else()

  if (APPLE)
    set (CSF_objc        "objc")

    # frameworks
    if (IOS)
      find_library (Appkit_LIB NAMES UIKit)
      set (CSF_Appkit ${Appkit_LIB})
    else()
      find_library (Appkit_LIB NAMES AppKit)
      set (CSF_Appkit ${Appkit_LIB})
    endif()
    OCCT_CHECK_AND_UNSET (Appkit_LIB)

    find_library (IOKit_LIB NAMES IOKit)
    set (CSF_IOKit ${IOKit_LIB})
    OCCT_CHECK_AND_UNSET (IOKit_LIB)

    if (IOS)
      find_library (OpenGlesLibs_LIB NAMES OpenGLES)
      set (CSF_OpenGlesLibs ${OpenGlesLibs_LIB})
      OCCT_CHECK_AND_UNSET (OpenGlesLibs_LIB)
    elseif (USE_XLIB)
      set (CSF_OpenGlLibs "GL")
      set (CSF_XwLibs     "X11")
    else()
      find_library (OpenGlLibs_LIB NAMES OpenGL)
      set (CSF_OpenGlLibs ${OpenGlLibs_LIB})
      OCCT_CHECK_AND_UNSET (OpenGlLibs_LIB)
    endif()

  elseif (EMSCRIPTEN)
    set (CSF_ThreadLibs   "pthread rt stdc++")
    set (CSF_OpenGlesLibs "EGL GLESv2")
    set (CSF_dl           "dl")
  elseif (ANDROID)
    set (CSF_ThreadLibs  "c")
    set (CSF_OpenGlesLibs "EGL GLESv2")
    set (CSF_androidlog  "log")
  elseif (UNIX)
    set (CSF_ThreadLibs  "pthread rt stdc++")
    if (USE_XLIB)
      set (CSF_OpenGlLibs "GL")
      set (CSF_XwLibs     "X11")
    else()
      set (CSF_OpenGlLibs "GL EGL")
    endif()
    set (CSF_OpenGlesLibs "EGL GLESv2")
    set (CSF_dl          "dl")
    if (USE_FREETYPE)
      set (CSF_fontconfig "fontconfig")
    endif()
  endif()
endif()
