// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen (Anand NATRAJAN)
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

#ifndef _IGESDimen_GeneralSymbol_HeaderFile
#define _IGESDimen_GeneralSymbol_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESDimen_HArray1OfLeaderArrow.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class IGESDimen_GeneralNote;
class Standard_OutOfRange;
class IGESData_IGESEntity;
class IGESDimen_LeaderArrow;


class IGESDimen_GeneralSymbol;
DEFINE_STANDARD_HANDLE(IGESDimen_GeneralSymbol, IGESData_IGESEntity)

//! defines General Symbol, Type <228>, Form <0-3,5001-9999>
//! in package IGESDimen
//! Consists of zero or one (Form 0) or one (all other
//! forms), one or more geometry entities which define
//! a symbol, and zero, one or more associated leaders.
class IGESDimen_GeneralSymbol : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESDimen_GeneralSymbol();
  
  //! This method is used to set the fields of the class
  //! GeneralSymbol
  //! - aNote      : General Note, null for form 0
  //! - allGeoms   : Geometric Entities
  //! - allLeaders : Leader Arrows
  Standard_EXPORT void Init (const Handle(IGESDimen_GeneralNote)& aNote, const Handle(IGESData_HArray1OfIGESEntity)& allGeoms, const Handle(IGESDimen_HArray1OfLeaderArrow)& allLeaders);
  
  //! Changes FormNumber (indicates the Nature of the Symbole)
  //! Error if not in ranges [0-3] or [> 5000]
  Standard_EXPORT void SetFormNumber (const Standard_Integer form);
  
  //! returns True if there is associated General Note Entity
  Standard_EXPORT Standard_Boolean HasNote() const;
  
  //! returns Null handle for form 0 only
  Standard_EXPORT Handle(IGESDimen_GeneralNote) Note() const;
  
  //! returns number of Geometry Entities
  Standard_EXPORT Standard_Integer NbGeomEntities() const;
  
  //! returns the Index'th Geometry Entity
  //! raises exception if Index <= 0 or Index > NbGeomEntities()
  Standard_EXPORT Handle(IGESData_IGESEntity) GeomEntity (const Standard_Integer Index) const;
  
  //! returns number of Leaders or zero if not specified
  Standard_EXPORT Standard_Integer NbLeaders() const;
  
  //! returns the Index'th Leader Arrow
  //! raises exception if Index <= 0 or Index > NbLeaders()
  Standard_EXPORT Handle(IGESDimen_LeaderArrow) LeaderArrow (const Standard_Integer Index) const;




  DEFINE_STANDARD_RTTIEXT(IGESDimen_GeneralSymbol,IGESData_IGESEntity)

protected:




private:


  Handle(IGESDimen_GeneralNote) theNote;
  Handle(IGESData_HArray1OfIGESEntity) theGeoms;
  Handle(IGESDimen_HArray1OfLeaderArrow) theLeaders;


};







#endif // _IGESDimen_GeneralSymbol_HeaderFile
