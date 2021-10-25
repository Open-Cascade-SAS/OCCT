// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( Anand NATRAJAN )
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

#ifndef _IGESBasic_ExternalRefFileIndex_HeaderFile
#define _IGESBasic_ExternalRefFileIndex_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Interface_HArray1OfHAsciiString.hxx>
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;


class IGESBasic_ExternalRefFileIndex;
DEFINE_STANDARD_HANDLE(IGESBasic_ExternalRefFileIndex, IGESData_IGESEntity)

//! defines ExternalRefFileIndex, Type <402> Form <12>
//! in package IGESBasic
//! Contains a list of the symbolic names used by the
//! referencing files and the DE pointers to the
//! corresponding definitions within the referenced file
class IGESBasic_ExternalRefFileIndex : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESBasic_ExternalRefFileIndex();
  
  //! This method is used to set the fields of the class
  //! ExternalRefFileIndex
  //! - aNameArray  : External Reference Entity symbolic names
  //! - allEntities : External Reference Entities
  //! raises exception if array lengths are not equal
  //! if size of aNameArray is not equal to size of allEntities
  Standard_EXPORT void Init (const Handle(Interface_HArray1OfHAsciiString)& aNameArray, const Handle(IGESData_HArray1OfIGESEntity)& allEntities);
  
  //! returns number of index entries
  Standard_EXPORT Standard_Integer NbEntries() const;
  
  //! returns the External Reference Entity symbolic name
  //! raises exception if Index <= 0 or Index > NbEntries()
  Standard_EXPORT Handle(TCollection_HAsciiString) Name (const Standard_Integer Index) const;
  
  //! returns the internal entity
  //! raises exception if Index <= 0 or Index > NbEntries()
  Standard_EXPORT Handle(IGESData_IGESEntity) Entity (const Standard_Integer Index) const;




  DEFINE_STANDARD_RTTIEXT(IGESBasic_ExternalRefFileIndex,IGESData_IGESEntity)

protected:




private:


  Handle(Interface_HArray1OfHAsciiString) theNames;
  Handle(IGESData_HArray1OfIGESEntity) theEntities;


};







#endif // _IGESBasic_ExternalRefFileIndex_HeaderFile
