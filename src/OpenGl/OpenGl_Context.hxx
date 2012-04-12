// Created on: 2012-01-26
// Created by: Kirill GAVRILOV
// Copyright (c) 2012-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#ifndef _OpenGl_Context_H__
#define _OpenGl_Context_H__

#include <Aspect_Handle.hxx>
#include <Aspect_Drawable.hxx>
#include <Aspect_Display.hxx>
#include <Aspect_RenderingContext.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_AsciiString.hxx>
#include <Handle_OpenGl_Context.hxx>

//! Forward declarations
struct OpenGl_GlCore12;
struct OpenGl_GlCore13;
struct OpenGl_GlCore14;
struct OpenGl_GlCore15;
struct OpenGl_GlCore20;
struct OpenGl_ArbVBO;
struct OpenGl_ExtFBO;

//! This class generalize access to the GL context and available extensions.
//!
//! Functions are grouped into structures and accessed as fields.
//! You should check the group for NULL before usage (if group is not NULL
//! then all functions are available):
//! @code
//!   if (myContext->core20 != NULL)
//!   {
//!     myGlProgram = myContext->core20->glCreateProgram();
//!     .. do more stuff ..
//!   }
//!   else
//!   {
//!     .. compatibility with outdated configurations ..
//!   }
//! @endcode
//!
//! Current implementation provide access to OpenGL core functionality up to 2.0 version
//! (core12, core13, core14, core15, fields core20).
//! within several extensions (arbVBO, extFBO, etc.).
//!
//! Simplified extensions classification:
//!  - prefixed with NV, AMD, ATI are vendor-specific (however may be provided by other vendors in some cases);
//!  - prefixed with EXT are accepted by 2+ vendors;
//!  - prefixed with ARB are accepted by Architecture Review Board and are candidates
//!    for inclusion into GL core functionality.
//! Some functionality can be represented in several extensions simultaneously.
//! In this case developer should be careful because different specification may differ
//! in aspects (like enumeration values and error-handling).
//!
//! Notice that some systems provide mechanisms to simultaneously incorporate with GL contexts
//! with different capabilities. Thats why OpenGl_Context should be initialized and used
//! for each GL context individually.
class OpenGl_Context : public Standard_Transient
{
public:

  //! Empty constructor. You should call Init() to perform initialization with bound GL context.
  Standard_EXPORT OpenGl_Context();

  //! Destructor.
  Standard_EXPORT virtual ~OpenGl_Context();

  //! Initialize available extensions.
  //! GL context should be active!
  Standard_EXPORT Standard_Boolean Init();

#if (defined(_WIN32) || defined(__WIN32__))
  Standard_EXPORT Standard_Boolean Init (const Aspect_Handle           theWindow,
                                         const Aspect_Handle           theWindowDC,
                                         const Aspect_RenderingContext theGContext);
#else
  Standard_EXPORT Standard_Boolean Init (const Aspect_Drawable         theWindow,
                                         const Aspect_Display          theDisplay,
                                         const Aspect_RenderingContext theGContext);
#endif

  //! Check if theExtName extension is supported by active GL context.
  Standard_EXPORT Standard_Boolean CheckExtension (const char* theExtName) const;

  //! Auxiliary template to retrieve GL function pointer.
  //! Pointer to function retrieved from library is statically casted
  //! to requested type - there no way to check real signature of exported function.
  //! The context should be bound before call.
  template <typename FuncType_t>
  Standard_Boolean FindProc (const char* theFuncName,
                             FuncType_t& theFuncPtr)
  {
    theFuncPtr = (FuncType_t )findProc (theFuncName);
    return (theFuncPtr != NULL);
  }

  //! @return true if detected GL version is greater or equal to requested one.
  inline Standard_Boolean IsGlGreaterEqual (const Standard_Integer theVerMajor,
                                            const Standard_Integer theVerMinor)
  {
    return (myGlVerMajor >  theVerMajor)
        || (myGlVerMajor == theVerMajor && myGlVerMinor >= theVerMinor);
  }

  //! Clean up errors stack for this GL context (glGetError() in loop).
  Standard_EXPORT void ResetErrors();

  //! Activates current context.
  //! Class should be initialized with appropriate info.
  Standard_EXPORT Standard_Boolean MakeCurrent();

  //! Return true if active mode is GL_FEEDBACK (cached state)
  Standard_EXPORT Standard_Boolean IsFeedback() const;

  //! Setup feedback mode cached state
  Standard_EXPORT void SetFeedback (const Standard_Boolean theFeedbackOn);

  //! This function retrieves information from GL about free GPU memory that is:
  //!  - OS-dependent. On some OS it is per-process and on others - for entire system.
  //!  - Vendor-dependent. Currently available only on NVIDIA and AMD/ATi drivers only.
  //!  - Numbers meaning may vary.
  //! You should use this info only for diagnostics purposes.
  //! @return free GPU dedicated memory in bytes.
  Standard_EXPORT Standard_Size AvailableMemory() const;

  //! This function retrieves information from GL about GPU memory
  //! and contains more vendor-specific values than AvailableMemory().
  Standard_EXPORT TCollection_AsciiString MemoryInfo() const;

private:

  //! Wrapper to system function to retrieve GL function pointer by name.
  Standard_EXPORT void* findProc (const char* theFuncName);

  //! Read OpenGL version information from active context.
  Standard_EXPORT void readGlVersion();

  //! Private initialization function that should be called only once.
  Standard_EXPORT void init();

public: // core profiles

  OpenGl_GlCore12* core12;
  OpenGl_GlCore13* core13;
  OpenGl_GlCore14* core14;
  OpenGl_GlCore15* core15;
  OpenGl_GlCore20* core20;

public: // extensions

  OpenGl_ArbVBO*   arbVBO; //!< GL_ARB_vertex_buffer_object
  OpenGl_ExtFBO*   extFBO; //!< GL_EXT_framebuffer_object
  Standard_Boolean atiMem; //!< GL_ATI_meminfo
  Standard_Boolean nvxMem; //!< GL_NVX_gpu_memory_info

private:

#if (defined(_WIN32) || defined(__WIN32__))
  Aspect_Handle           myWindow;   //!< window handle (owner of GL context) : HWND
  Aspect_Handle           myWindowDC; //!< Device Descriptor handle : HDC
  Aspect_RenderingContext myGContext; //!< Rendering Context handle : HGLRC
#else
  Aspect_Drawable         myWindow;   //!< window handle (owner of GL context) : GLXDrawable
  Aspect_Display          myDisplay;  //!< connection to the X-server : Display*
  Aspect_RenderingContext myGContext; //!< X-GLX rendering context : GLXContext
#endif

  void*            myGlLibHandle;   //!< optional handle to GL library
  OpenGl_GlCore20* myGlCore20;      //!< common structure for GL core functions upto 2.0
  Standard_Integer myGlVerMajor;    //!< cached GL version major number
  Standard_Integer myGlVerMinor;    //!< cached GL version minor number
  Standard_Boolean myIsFeedback;    //!< flag indicates GL_FEEDBACK mode
  Standard_Boolean myIsInitialized; //!< flag indicates initialization state

public:

  DEFINE_STANDARD_RTTI(OpenGl_Context) // Type definition

};

#endif // _OpenGl_Context_H__
