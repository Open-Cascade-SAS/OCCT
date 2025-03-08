// Created on: 2013-09-05
// Created by: Anton POLETAEV
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

#ifndef OpenGl_CappingAlgo_HeaderFile
#define OpenGl_CappingAlgo_HeaderFile

#include <OpenGl_Group.hxx>

// Forward declaration
class OpenGl_Structure;

//! Capping surface rendering algorithm.
class OpenGl_CappingAlgo
{
public:
  //! Draw capping surfaces by OpenGl for the clipping planes enabled in current context state.
  //! Depth buffer must be generated  for the passed groups.
  //! @param[in] theWorkspace  the GL workspace, context state
  //! @param[in] theStructure  the structure to be capped
  Standard_EXPORT static void RenderCapping(const Handle(OpenGl_Workspace)& theWorkspace,
                                            const OpenGl_Structure&         theStructure);
};

#endif
