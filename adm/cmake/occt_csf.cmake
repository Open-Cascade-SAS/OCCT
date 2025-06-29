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
   # Will be called later
  if (EMSCRIPTEN)
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
      set (CSF_fontconfig "fontconfig expat")
    endif()
  endif()
endif()

# Updates APPLE variables
function(occt_set_apple_csf_vars)
  set (CSF_objc        "objc" PARENT_SCOPE)

  # frameworks
  if (IOS)
    find_library (Appkit_LIB NAMES UIKit)
    if (Appkit_LIB)
      set (CSF_Appkit ${Appkit_LIB} PARENT_SCOPE)
    else()
      set (CSF_Appkit "UIKit" PARENT_SCOPE)
    endif()
  else()
    find_library (Appkit_LIB NAMES AppKit)
    if (Appkit_LIB)
      set (CSF_Appkit ${Appkit_LIB} PARENT_SCOPE)
    else()
      set (CSF_Appkit "AppKit" PARENT_SCOPE)
    endif()
  endif()
  OCCT_CHECK_AND_UNSET (Appkit_LIB)

  find_library (IOKit_LIB NAMES IOKit)
  if (IOKit_LIB)
    set (CSF_IOKit ${IOKit_LIB} PARENT_SCOPE)
  else()
    set (CSF_IOKit "IOKit" PARENT_SCOPE)
  endif()
  OCCT_CHECK_AND_UNSET (IOKit_LIB)

  if (IOS)
    find_library (OpenGlesLibs_LIB NAMES OpenGLES)
    if (OpenGlesLibs_LIB)
      set (CSF_OpenGlesLibs ${OpenGlesLibs_LIB} PARENT_SCOPE)
    else()
      set (CSF_OpenGlesLibs "OpenGLES" PARENT_SCOPE)
    endif()
    OCCT_CHECK_AND_UNSET (OpenGlesLibs_LIB)
  elseif (USE_XLIB)
    set (CSF_OpenGlLibs "GL" PARENT_SCOPE)
    set (CSF_XwLibs     "X11" PARENT_SCOPE)
  else()
    find_library (OpenGlLibs_LIB NAMES OpenGL)
    if (OpenGlLibs_LIB)
      set (CSF_OpenGlLibs ${OpenGlLibs_LIB} PARENT_SCOPE)
    else()
      set (CSF_OpenGlLibs "OpenGL" PARENT_SCOPE)
    endif()
    OCCT_CHECK_AND_UNSET (OpenGlLibs_LIB)
  endif()
endfunction()
