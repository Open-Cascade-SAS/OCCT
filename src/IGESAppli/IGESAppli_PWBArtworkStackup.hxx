// Created on: 1993-01-11
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

#ifndef _IGESAppli_PWBArtworkStackup_HeaderFile
#define _IGESAppli_PWBArtworkStackup_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <IGESData_IGESEntity.hxx>
class TCollection_HAsciiString;


class IGESAppli_PWBArtworkStackup;
DEFINE_STANDARD_HANDLE(IGESAppli_PWBArtworkStackup, IGESData_IGESEntity)

//! defines PWBArtworkStackup, Type <406> Form <25>
//! in package IGESAppli
//! Used to communicate which exchange file levels are to
//! be combined in order to create the artwork for a
//! printed wire board (PWB). This property should be
//! attached to the entity defining the printed wire
//! assembly (PWA) or if no such entity exists, then the
//! property should stand alone in the file.
class IGESAppli_PWBArtworkStackup : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESAppli_PWBArtworkStackup();
  
  //! This method is used to set the fields of the class
  //! PWBArtworkStackup
  //! - nbPropVal    : number of property values
  //! - anArtIdent   : Artwork Stackup Identification
  //! - allLevelNums : Level Numbers
  Standard_EXPORT void Init (const Standard_Integer nbPropVal, const Handle(TCollection_HAsciiString)& anArtIdent, const Handle(TColStd_HArray1OfInteger)& allLevelNums);
  
  //! returns number of property values
  Standard_EXPORT Standard_Integer NbPropertyValues() const;
  
  //! returns Artwork Stackup Identification
  Standard_EXPORT Handle(TCollection_HAsciiString) Identification() const;
  
  //! returns total number of Level Numbers
  Standard_EXPORT Standard_Integer NbLevelNumbers() const;
  
  //! returns Level Number
  //! raises exception if Index <= 0 or Index > NbLevelNumbers
  Standard_EXPORT Standard_Integer LevelNumber (const Standard_Integer Index) const;




  DEFINE_STANDARD_RTTIEXT(IGESAppli_PWBArtworkStackup,IGESData_IGESEntity)

protected:




private:


  Standard_Integer theNbPropertyValues;
  Handle(TCollection_HAsciiString) theArtworkStackupIdent;
  Handle(TColStd_HArray1OfInteger) theLevelNumbers;


};







#endif // _IGESAppli_PWBArtworkStackup_HeaderFile
