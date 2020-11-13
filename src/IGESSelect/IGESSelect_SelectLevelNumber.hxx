// Created on: 1994-05-31
// Created by: Christian CAILLET
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _IGESSelect_SelectLevelNumber_HeaderFile
#define _IGESSelect_SelectLevelNumber_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_SelectExtract.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
class IFSelect_IntParam;
class Standard_Transient;
class Interface_InterfaceModel;
class TCollection_AsciiString;


class IGESSelect_SelectLevelNumber;
DEFINE_STANDARD_HANDLE(IGESSelect_SelectLevelNumber, IFSelect_SelectExtract)

//! This selection looks at Level Number of IGES Entities :
//! it considers items attached, either to a single level with a
//! given value, or to a level list which contains this value
//!
//! Level = 0  means entities not attached to any level
//!
//! Remark : the class CounterOfLevelNumber gives information
//! about present levels in a file.
class IGESSelect_SelectLevelNumber : public IFSelect_SelectExtract
{

public:

  
  //! Creates a SelectLevelNumber, with no Level criterium : see
  //! SetLevelNumber. Empty, this selection filters nothing.
  Standard_EXPORT IGESSelect_SelectLevelNumber();
  
  //! Sets a Parameter as Level criterium
  Standard_EXPORT void SetLevelNumber (const Handle(IFSelect_IntParam)& levnum);
  
  //! Returns the Level criterium. NullHandle if not yet set
  //! (interpreted as Level = 0 : no level number attached)
  Standard_EXPORT Handle(IFSelect_IntParam) LevelNumber() const;
  
  //! Returns True if <ent> is an IGES Entity with Level Number
  //! admits the criterium (= value if single level, or one of the
  //! attached level numbers = value if level list)
  Standard_EXPORT Standard_Boolean Sort (const Standard_Integer rank, const Handle(Standard_Transient)& ent, const Handle(Interface_InterfaceModel)& model) const Standard_OVERRIDE;
  
  //! Returns the Selection criterium :
  //! "IGES Entity, Level Number admits <nn>" (if nn > 0) or
  //! "IGES Entity attached to no Level" (if nn = 0)
  Standard_EXPORT TCollection_AsciiString ExtractLabel() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(IGESSelect_SelectLevelNumber,IFSelect_SelectExtract)

protected:




private:


  Handle(IFSelect_IntParam) thelevnum;


};







#endif // _IGESSelect_SelectLevelNumber_HeaderFile
