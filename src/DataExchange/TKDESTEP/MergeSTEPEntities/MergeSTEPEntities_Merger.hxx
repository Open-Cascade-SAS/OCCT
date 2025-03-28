// Copyright (c) 2025 OPEN CASCADE SAS
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

#ifndef _MergeSTEPEntities_Merger_HeaderFile
#define _MergeSTEPEntities_Merger_HeaderFile

#include <TColStd_MapOfTransient.hxx>

class XSControl_WorkSession;

//! A class to merge STEP entities.
//! This class is used to merge equal STEP entities in the work session and remove duplicates.
//! More detailed information about merging entities cam be found in
//! MergeSTEPEntities_EntityProcessor class and its descendants.
class MergeSTEPEntities_Merger
{
public:
  //! Constructor.
  //! @param theWS the work session to merge entities in.
  Standard_EXPORT MergeSTEPEntities_Merger(Handle(XSControl_WorkSession) theWS);

  //! Perform the merging of entities.
  //! All entities in a model stored in the provided work session that are considered equal to
  //! each other will be merged, and duplicates will be removed.
  Standard_EXPORT void Perform();

private:
  //! Remove entities from the work session.
  //! @param theToRemove the entities to remove.
  void removeEntities(const TColStd_MapOfTransient& theToRemove);

private:
  Handle(XSControl_WorkSession) myWS; //!< The work session containing the model with entities.
};

#endif // _MergeSTEPEntities_Merger_HeaderFile
