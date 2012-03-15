// File:      OpenGl_Context.hxx
// Created:   26 January 2012
// Author:    Kirill GAVRILOV
// Copyright: OPEN CASCADE 2012

#ifndef _OpenGl_Context_H__
#define _OpenGl_Context_H__

#include <Standard_Transient.hxx>
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
  Standard_EXPORT void Init();

  //! Check if theExtName extension is supported by active GL context.
  Standard_EXPORT static Standard_Boolean CheckExtension (const char* theExtName);

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

  //! @return true if detected GL version is higher or equal to requested one.
  inline Standard_Boolean IsGlUpperEqual (const Standard_Integer theVerMajor,
                                          const Standard_Integer theVerMinor)
  {
    return (myGlVerMajor >  theVerMajor)
        || (myGlVerMajor == theVerMajor && myGlVerMinor >= theVerMinor);
  }

  //! Clean up errors stack for this GL context (glGetError() in loop).
  Standard_EXPORT void ResetErrors();

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

  OpenGl_ArbVBO*   arbVBO;
  OpenGl_ExtFBO*   extFBO;

private:

  void*            myGlLibHandle;   //!< optional handle to GL library
  OpenGl_GlCore20* myGlCore20;      //!< common structure for GL core functions upto 2.0
  Standard_Integer myGlVerMajor;    //!< cached GL version major number
  Standard_Integer myGlVerMinor;    //!< cached GL version minor number
  Standard_Boolean myIsInitialized; //!< flag to indicate initialization state

public:

  DEFINE_STANDARD_RTTI(OpenGl_Context) // Type definition

};

#endif // _OpenGl_Context_H__
