// Created on: 1993-09-07
// Created by: Christian CAILLET
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _IGESData_SpecificModule_HeaderFile
#define _IGESData_SpecificModule_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
class IGESData_IGESEntity;
class IGESData_IGESDumper;

class IGESData_SpecificModule;
DEFINE_STANDARD_HANDLE(IGESData_SpecificModule, Standard_Transient)

//! This class defines some Services which are specifically
//! attached to IGES Entities : Dump
class IGESData_SpecificModule : public Standard_Transient
{

public:
  //! Specific Dump for each type of IGES Entity : it concerns only
  //! own parameters, the general data (Directory Part, Lists) are
  //! taken into account by the IGESDumper
  //! See class IGESDumper for the rules to follow for <own> and
  //! <attached> level
  Standard_EXPORT virtual void OwnDump(const Standard_Integer             CN,
                                       const Handle(IGESData_IGESEntity)& ent,
                                       const IGESData_IGESDumper&         dumper,
                                       Standard_OStream&                  S,
                                       const Standard_Integer             own) const = 0;

  //! Specific Automatic Correction on own Parameters of an Entity.
  //! It works by setting in accordance redundant data, if there are
  //! when there is no ambiguity (else, it does nothing).
  //! Remark that classic Corrections on Directory Entry (to set
  //! void data) are taken into account alsewhere.
  //!
  //! For instance, many "Associativity Entities" have a Number of
  //! Properties which must have a fixed value.
  //! Or, a ConicalArc has its Form Number which records the kind of
  //! Conic, also determined from its coefficients
  //! But, a CircularArc of which Distances (Center-Start) and
  //! (Center-End) are not equal cannot be corrected ...
  //!
  //! Returns True if something has been corrected in <ent>
  //! By default, does nothing. If at least one of the Types
  //! processed by a sub-class of SpecificModule has a Correct
  //! procedure attached, this method can be redefined
  Standard_EXPORT virtual Standard_Boolean OwnCorrect(const Standard_Integer             CN,
                                                      const Handle(IGESData_IGESEntity)& ent) const;

  DEFINE_STANDARD_RTTIEXT(IGESData_SpecificModule, Standard_Transient)

protected:
private:
};

#endif // _IGESData_SpecificModule_HeaderFile
