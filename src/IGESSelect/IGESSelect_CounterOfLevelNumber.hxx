// Created on: 1994-05-31
// Created by: Modelistation
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

#ifndef _IGESSelect_CounterOfLevelNumber_HeaderFile
#define _IGESSelect_CounterOfLevelNumber_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <IFSelect_SignCounter.hxx>
#include <Standard_Boolean.hxx>
#include <TColStd_HSequenceOfInteger.hxx>
class Standard_Transient;
class Interface_InterfaceModel;
class TCollection_HAsciiString;
class Message_Messenger;


class IGESSelect_CounterOfLevelNumber;
DEFINE_STANDARD_HANDLE(IGESSelect_CounterOfLevelNumber, IFSelect_SignCounter)

//! This class gives information about Level Number. It counts
//! entities according level number, considering also the
//! multiple level (see the class LevelList) for which an entity
//! is attached to each of the listed levels.
//!
//! Data are available, as level number, or as their alphanumeric
//! counterparts ("LEVEL nnnnnnn", " NO LEVEL", " LEVEL LIST")
class IGESSelect_CounterOfLevelNumber : public IFSelect_SignCounter
{

public:

  
  //! Creates a CounterOfLevelNumber, clear, ready to work
  //! <withmap> and <withlist> are transmitted to SignCounter
  Standard_EXPORT IGESSelect_CounterOfLevelNumber(const Standard_Boolean withmap = Standard_True, const Standard_Boolean withlist = Standard_False);
  
  //! Resets already memorized informations : also numeric data
  Standard_EXPORT virtual void Clear() Standard_OVERRIDE;
  
  //! Adds an entity by considering its lrvrl number(s)
  //! A level is added both in numeric and alphanumeric form,
  //! i.e. LevelList gives "LEVEL LIST", others (no level or
  //! positive level) displays level number on 7 digits (C : %7d)
  //! Remark : an entity attached to a Level List is added for
  //! " LEVEL LIST", and for each of its constituant levels
  Standard_EXPORT virtual void AddSign (const Handle(Standard_Transient)& ent, const Handle(Interface_InterfaceModel)& model) Standard_OVERRIDE;
  
  //! The internal action to record a new level number, positive,
  //! null (no level) or negative (level list)
  Standard_EXPORT void AddLevel (const Handle(Standard_Transient)& ent, const Standard_Integer level);
  
  //! Returns the highest value found for a level number
  Standard_EXPORT Standard_Integer HighestLevel() const;
  
  //! Returns the number of times a level is used,
  //! 0 if it has not been recorded at all
  //! <level> = 0 counts entities attached to no level
  //! <level> < 0 counts entities attached to a LevelList
  Standard_EXPORT Standard_Integer NbTimesLevel (const Standard_Integer level) const;
  
  //! Returns the ordered list of used positive Level numbers
  Standard_EXPORT Handle(TColStd_HSequenceOfInteger) Levels() const;
  
  //! Determines and returns the value of the signature for an
  //! entity as an HAsciiString. Redefined, gives the same result
  //! as AddSign, see this method ("LEVEL LIST" or "nnnnnnn")
  Standard_EXPORT virtual Handle(TCollection_HAsciiString) Sign (const Handle(Standard_Transient)& ent, const Handle(Interface_InterfaceModel)& model) const Standard_OVERRIDE;
  
  //! Prints the counts of items (not the list) then the Highest
  //! Level Number recorded
  Standard_EXPORT virtual void PrintCount (const Handle(Message_Messenger)& S) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(IGESSelect_CounterOfLevelNumber,IFSelect_SignCounter)

protected:




private:


  Standard_Integer thehigh;
  Standard_Integer thenblists;
  Handle(TColStd_HArray1OfInteger) thelevels;


};







#endif // _IGESSelect_CounterOfLevelNumber_HeaderFile
