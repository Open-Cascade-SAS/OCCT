// Created on: 2013-09-26
// Created by: Dmitry BOBYLEV
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

#ifndef OpenGl_StencilTest_HeaderFile
#define OpenGl_StencilTest_HeaderFile

#include <OpenGl_Element.hxx>

class OpenGl_StencilTest : public OpenGl_Element
{
public:
  //! Default constructor
  Standard_EXPORT OpenGl_StencilTest();

  //! Render primitives to the window
  Standard_EXPORT virtual void Render(const Handle(OpenGl_Workspace)& theWorkspace) const
    Standard_OVERRIDE;

  Standard_EXPORT virtual void Release(OpenGl_Context* theContext) Standard_OVERRIDE;

  Standard_EXPORT void SetOptions(const Standard_Boolean theIsEnabled);

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

protected:
  //! Destructor
  Standard_EXPORT virtual ~OpenGl_StencilTest();

private:
  Standard_Boolean myIsEnabled;

public:
  DEFINE_STANDARD_ALLOC
};

#endif // OpenGl_StencilOptions_Header
