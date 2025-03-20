// Created on: 1993-05-10
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

#ifndef _IGESData_GeneralModule_HeaderFile
#define _IGESData_GeneralModule_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Interface_GeneralModule.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class Interface_EntityIterator;
class IGESData_IGESEntity;
class Interface_ShareTool;
class Interface_Check;
class IGESData_DirChecker;
class Interface_CopyTool;
class TCollection_HAsciiString;

class IGESData_GeneralModule;
DEFINE_STANDARD_HANDLE(IGESData_GeneralModule, Interface_GeneralModule)

//! Definition of General Services adapted to IGES.
//! This Services comprise : Shared & Implied Lists, Copy, Check
//! They are adapted according to the organisation of IGES
//! Entities : Directory Part, Lists of Associativities and
//! Properties are specifically processed
class IGESData_GeneralModule : public Interface_GeneralModule
{

public:
  //! Fills the list of Entities shared by an IGESEntity <ent>,
  //! according a Case Number <CN> (formerly computed by CaseNum).
  //! Considers Properties and Directory Part, and calls
  //! OwnSharedCase (which is adapted to each Type of Entity)
  Standard_EXPORT void FillSharedCase(const Standard_Integer            CN,
                                      const Handle(Standard_Transient)& ent,
                                      Interface_EntityIterator& iter) const Standard_OVERRIDE;

  //! Lists the Entities shared by a given IGESEntity <ent>, from
  //! its specific parameters : specific for each type
  Standard_EXPORT virtual void OwnSharedCase(const Standard_Integer             CN,
                                             const Handle(IGESData_IGESEntity)& ent,
                                             Interface_EntityIterator&          iter) const = 0;

  //! Lists the Implied References of <ent>. Here, these are the
  //! Associativities, plus the Entities defined by OwnSharedCase
  Standard_EXPORT virtual void ListImpliedCase(const Standard_Integer            CN,
                                               const Handle(Standard_Transient)& ent,
                                               Interface_EntityIterator&         iter) const
    Standard_OVERRIDE;

  //! Specific list of Entities implied by a given IGESEntity <ent>
  //! (in addition to Associativities). By default, there are none,
  //! but this method can be redefined as required
  Standard_EXPORT virtual void OwnImpliedCase(const Standard_Integer             CN,
                                              const Handle(IGESData_IGESEntity)& ent,
                                              Interface_EntityIterator&          iter) const;

  //! Semantic Checking of an IGESEntity. Performs general Checks,
  //! which use DirChecker, then call OwnCheck which does a check
  //! specific for each type of Entity
  Standard_EXPORT void CheckCase(const Standard_Integer            CN,
                                 const Handle(Standard_Transient)& ent,
                                 const Interface_ShareTool&        shares,
                                 Handle(Interface_Check)&          ach) const Standard_OVERRIDE;

  //! Returns a DirChecker, specific for each type of Entity
  //! (identified by its Case Number) : this DirChecker defines
  //! constraints which must be respected by the DirectoryPart
  Standard_EXPORT virtual IGESData_DirChecker DirChecker(
    const Standard_Integer             CN,
    const Handle(IGESData_IGESEntity)& ent) const = 0;

  //! Performs Specific Semantic Check for each type of Entity
  Standard_EXPORT virtual void OwnCheckCase(const Standard_Integer             CN,
                                            const Handle(IGESData_IGESEntity)& ent,
                                            const Interface_ShareTool&         shares,
                                            Handle(Interface_Check)&           ach) const = 0;

  //! Specific answer to the question "is Copy properly implemented"
  //! For IGES, answer is always True
  Standard_EXPORT virtual Standard_Boolean CanCopy(const Standard_Integer            CN,
                                                   const Handle(Standard_Transient)& ent) const
    Standard_OVERRIDE;

  //! Specific creation of a new void entity
  Standard_EXPORT virtual Standard_Boolean NewVoid(const Standard_Integer      CN,
                                                   Handle(Standard_Transient)& entto) const
    Standard_OVERRIDE = 0;

  //! Copy ("Deep") from <entfrom> to <entto> (same type)
  //! by using a CopyTool which provides its working Map.
  //! For IGESEntities, Copies general data (Directory Part, List of
  //! Properties) and call OwnCopyCase
  Standard_EXPORT void CopyCase(const Standard_Integer            CN,
                                const Handle(Standard_Transient)& entfrom,
                                const Handle(Standard_Transient)& entto,
                                Interface_CopyTool&               TC) const Standard_OVERRIDE;

  //! Copies parameters which are specific of each Type of Entity
  Standard_EXPORT virtual void OwnCopyCase(const Standard_Integer             CN,
                                           const Handle(IGESData_IGESEntity)& entfrom,
                                           const Handle(IGESData_IGESEntity)& entto,
                                           Interface_CopyTool&                TC) const = 0;

  //! Renewing of Implied References.
  //! For IGESEntities, Copies general data(List of Associativities)
  //! and calls OwnRenewCase
  Standard_EXPORT virtual void RenewImpliedCase(const Standard_Integer            CN,
                                                const Handle(Standard_Transient)& entfrom,
                                                const Handle(Standard_Transient)& entto,
                                                const Interface_CopyTool&         TC) const
    Standard_OVERRIDE;

  //! Renews parameters which are specific of each Type of Entity :
  //! the provided default does nothing, but this method may be
  //! redefined as required
  Standard_EXPORT virtual void OwnRenewCase(const Standard_Integer             CN,
                                            const Handle(IGESData_IGESEntity)& entfrom,
                                            const Handle(IGESData_IGESEntity)& entto,
                                            const Interface_CopyTool&          TC) const;

  //! Prepares an IGES Entity for delete : works on directory part
  //! then calls OwnDeleteCase
  //! While dispatch requires to copy the entities, <dispatched> is
  //! ignored, entities are cleared in any case
  Standard_EXPORT virtual void WhenDeleteCase(const Standard_Integer            CN,
                                              const Handle(Standard_Transient)& ent,
                                              const Standard_Boolean            dispatched) const
    Standard_OVERRIDE;

  //! Specific preparation for delete, acts on own parameters
  //! Default does nothing, to be redefined as required
  Standard_EXPORT virtual void OwnDeleteCase(const Standard_Integer             CN,
                                             const Handle(IGESData_IGESEntity)& ent) const;

  //! Returns the name of an IGES Entity (its NameValue)
  //! Can be redefined for an even more specific case ...
  Standard_EXPORT virtual Handle(TCollection_HAsciiString) Name(
    const Standard_Integer            CN,
    const Handle(Standard_Transient)& ent,
    const Interface_ShareTool&        shares) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESData_GeneralModule, Interface_GeneralModule)

protected:
private:
};

#endif // _IGESData_GeneralModule_HeaderFile
