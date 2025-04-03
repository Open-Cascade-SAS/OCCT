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

#ifndef _StepTidy_DuplicateCleaner_HeaderFile
#define _StepTidy_DuplicateCleaner_HeaderFile

#include <TColStd_MapOfTransient.hxx>

class XSControl_WorkSession;

//! A class to merge STEP entities.
//! This class is used to merge equal STEP entities in the work session and remove duplicates.
//! It uses the child classes of StepTidy_EntityReducer class to perform the merging.
//! The child classes of StepTidy_EntityReducer are specialized for different types of entities.
//! StepTidy_EntityReducer implements the basic logic for searching and merging entities
//! while child classes implement the logic for replacing specific type of entities in the specific
//! type of sharing entities.
//! Classes StepTidy_*Hasher are used to hash the entities and compare them. They define which
//! entities are considered equal to each other. The hashers are used in the StepTidy_EntityReducer
//! class to store the entities in a map. The map is used to find the duplicates and replace them.
//! From this perspective of this module, 'equal' or 'duplicate' entities are those that
//! has equal names and very close numerical values, like for example Cartesian points with
//! coordinates that are equal up to 1e-12 or Vectors with equal orientations and magnitudes
//! up to 1e-12.
//! After the merging this class calls its own method to remove the duplicates.
//! How to use:
//! 1. Create an instance of the class by providing a pointer to the work session where the
//!    entities to process are stored.
//! 2. Call Perform() method to perform the merging of entities. After this call all entities
//!    that are considered equal to each other will be merged, and duplicates will be removed.
class StepTidy_DuplicateCleaner
{
public:
  //! Constructor.
  //! @param theWS the work session to merge entities in.
  Standard_EXPORT StepTidy_DuplicateCleaner(Handle(XSControl_WorkSession) theWS);

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

#endif // _StepTidy_DuplicateCleaner_HeaderFile
