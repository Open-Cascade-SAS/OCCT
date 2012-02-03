// File:      OpenGl_Context.hxx
// Created:   26 January 2012
// Author:    Kirill GAVRILOV
// Copyright: OPEN CASCADE 2012

#ifndef _OpenGl_Context_H__
#define _OpenGl_Context_H__

#include <OpenGl_ArbVBO.hxx>
#include <OpenGl_ExtFBO.hxx>
#include <Standard_Transient.hxx>
#include <Handle_OpenGl_Context.hxx>

#if (!defined(_WIN32) && !defined(__WIN32__))
  extern "C" {
    extern void (*glXGetProcAddress (const GLubyte* theProcName))();
  }
#endif

//! This class provide access to the GL context and available extensions.
class OpenGl_Context : public Standard_Transient
{
public:

  OpenGl_Context();
  virtual ~OpenGl_Context();

  //! Initialize available extensions.
  //! GL context should be active!
  void Init();

  //! Parse theExtString string for presence of theExtName extension.
  static Standard_Boolean CheckExtension (const char* theExtName,
                                          const char* theExtString);

  //! Auxiliary template to retrieve GL function pointer.
  //! The context should be bound before call.
  template <typename FuncType_t>
  bool FindProc (const char* theFuncName,
                 FuncType_t& theFuncPtr)
  {
  #if (defined(_WIN32) || defined(__WIN32__))
    theFuncPtr = (FuncType_t )wglGetProcAddress (theFuncName);
  #else
    theFuncPtr = (FuncType_t )glXGetProcAddress ((const GLubyte* )theFuncName);
  #endif
    return (theFuncPtr != NULL);
  }

public:

  OpenGl_ArbVBO* arbVBO;
  OpenGl_ExtFBO* extFBO;

public:

  DEFINE_STANDARD_RTTI(OpenGl_Window) // Type definition

};

#endif // _OpenGl_Context_H__
