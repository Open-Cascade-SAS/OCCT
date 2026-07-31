// Created on: 1992-04-07
// Created by: Christian CAILLET
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _IGESData_UndefinedEntity_HeaderFile
#define _IGESData_UndefinedEntity_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_DefType.hxx>
#include <IGESData_DefList.hxx>
class Interface_UndefinedContent;
class IGESData_IGESReaderData;
class IGESData_DirPart;
class Interface_Check;
class IGESData_ParamReader;
class IGESData_IGESWriter;

//! undefined (unknown or error) entity specific of IGES
//! DirPart can be correct or not : if it is not, a flag indicates
//! it, and each corrupted field has an associated error flag
class IGESData_UndefinedEntity : public IGESData_IGESEntity
{

public:
  //! creates an unknown entity
  Standard_EXPORT IGESData_UndefinedEntity();

  //! Returns own data as an UndefinedContent
  Standard_EXPORT occ::handle<Interface_UndefinedContent> UndefinedContent() const;

  //! Returns own data as an UndefinedContent, in order to touch it
  Standard_EXPORT occ::handle<Interface_UndefinedContent> ChangeableContent();

  //! Redefines a completely new UndefinedContent
  //! Used by a Copy which begins by ShallowCopy, for instance
  Standard_EXPORT void SetNewContent(const occ::handle<Interface_UndefinedContent>& cont);

  //! says if DirPart is OK or not (if not, it is erroneous)
  //! Note that if it is not, Def* methods can return Error status
  Standard_EXPORT bool IsOKDirPart() const;

  //! returns Directory Error Status (used for Copy)
  Standard_EXPORT int DirStatus() const;

  //! Erases the Directory Error Status
  //! Warning : Be sure that data are consistent to call this method ...
  Standard_EXPORT void SetOKDirPart();

  //! returns Error status if necessary, else calls original method
  Standard_EXPORT IGESData_DefType DefLineFont() const override;

  //! returns Error status if necessary, else calls original method
  Standard_EXPORT IGESData_DefList DefLevel() const override;

  //! returns Error status if necessary, else calls original method
  Standard_EXPORT IGESData_DefList DefView() const override;

  //! returns Error status if necessary, else calls original method
  Standard_EXPORT IGESData_DefType DefColor() const override;

  //! returns Error status if necessary, else calls original method
  //! (that is, if SubScript field is not blank or positive integer)
  Standard_EXPORT bool HasSubScriptNumber() const override;

  //! Computes the Directory Error Status, to be called before
  //! standard ReadDir from IGESReaderTool
  //! Returns True if OK (hence, Directory can be loaded),
  //! Else returns False and the DirPart <DP> is modified
  //! (hence, Directory Error Status is non null; and standard Read
  //! will work with an acceptable DirectoryPart)
  Standard_EXPORT virtual bool ReadDir(const occ::handle<IGESData_IGESReaderData>& IR,
                                       IGESData_DirPart&                           DP,
                                       occ::handle<Interface_Check>&               ach);

  //! reads own parameters from file; PR gives access to them, IR
  //! detains parameter types and values
  //! Here, reads all parameters, integers are considered as entity
  //! reference unless they cannot be; no list interpretation
  //! No property or associativity list is managed
  Standard_EXPORT virtual void ReadOwnParams(const occ::handle<IGESData_IGESReaderData>& IR,
                                             IGESData_ParamReader&                       PR);

  //! writes parameters to IGESWriter, taken from UndefinedContent
  Standard_EXPORT virtual void WriteOwnParams(IGESData_IGESWriter& IW) const;

  DEFINE_STANDARD_RTTIEXT(IGESData_UndefinedEntity, IGESData_IGESEntity)

private:
  int                                     thedstat;
  occ::handle<Interface_UndefinedContent> thecont;
};

#endif // _IGESData_UndefinedEntity_HeaderFile
