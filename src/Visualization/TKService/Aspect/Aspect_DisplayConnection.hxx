// Copyright (c) 2013-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef Aspect_DisplayConnection_HeaderFile
#define Aspect_DisplayConnection_HeaderFile

#include <Standard_Transient.hxx>
#include <Aspect_XAtom.hxx>
#include <Aspect_FBConfig.hxx>

#include <TCollection_AsciiString.hxx>
#include <NCollection_DataMap.hxx>

struct Aspect_XDisplay;
struct Aspect_XVisualInfo;

//! This interface defines connection to platform-specific display.
class Aspect_DisplayConnection : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Aspect_DisplayConnection, Standard_Transient)
public:

  //! Empty constructor.
  Standard_EXPORT Aspect_DisplayConnection();

  //! Destructor.
  Standard_EXPORT virtual ~Aspect_DisplayConnection();

  //! @return pointer to Display structure that serves as the connection to the X server.
  virtual Aspect_XDisplay* GetDisplayAspect() { return nullptr; }

  //! @return identifier(atom) for custom named property associated with windows that use current connection to display.
  virtual uint64_t GetAtom (const Aspect_XAtom theAtom) const
  {
    (void )theAtom;
    return 0;
  }

  //! Set default window visual.
  virtual void SetDefaultVisualInfo (Aspect_XVisualInfo* theVisual,
                                     Aspect_FBConfig theFBConfig)
  {
    (void )theVisual;
    (void )theFBConfig;
  }

private:

  //! To protect the connection from closing copying allowed only through the handles.
  Aspect_DisplayConnection            (const Aspect_DisplayConnection& ) Standard_DELETE;
  Aspect_DisplayConnection& operator= (const Aspect_DisplayConnection& ) Standard_DELETE;

};

DEFINE_STANDARD_HANDLE(Aspect_DisplayConnection, Standard_Transient)

#endif // _Aspect_DisplayConnection_H__
