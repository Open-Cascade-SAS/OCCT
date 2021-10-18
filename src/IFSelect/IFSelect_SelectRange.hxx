// Created on: 1992-11-18
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

#ifndef _IFSelect_SelectRange_HeaderFile
#define _IFSelect_SelectRange_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_SelectExtract.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
class IFSelect_IntParam;
class Standard_Transient;
class Interface_InterfaceModel;
class TCollection_AsciiString;

class IFSelect_SelectRange;
DEFINE_STANDARD_HANDLE(IFSelect_SelectRange, IFSelect_SelectExtract)

//! A SelectRange keeps or rejects a sub-set of the input set,
//! that is the Entities of which rank in the iteration list
//! is in a given range (for instance form 2nd to 6th, etc...)
class IFSelect_SelectRange : public IFSelect_SelectExtract
{

public:

  
  //! Creates a SelectRange. Default is Take all the input list
  Standard_EXPORT IFSelect_SelectRange();
  
  //! Sets a Range for numbers, with a lower and a upper limits
  //! Error if rankto is lower then rankfrom
  Standard_EXPORT void SetRange (const Handle(IFSelect_IntParam)& rankfrom, const Handle(IFSelect_IntParam)& rankto);
  
  //! Sets a unique number (only one Entity will be sorted as True)
  Standard_EXPORT void SetOne (const Handle(IFSelect_IntParam)& rank);
  
  //! Sets a Lower limit but no upper limit
  Standard_EXPORT void SetFrom (const Handle(IFSelect_IntParam)& rankfrom);
  
  //! Sets an Upper limit but no lower limit (equivalent to lower 1)
  Standard_EXPORT void SetUntil (const Handle(IFSelect_IntParam)& rankto);
  
  //! Returns True if a Lower limit is defined
  Standard_EXPORT Standard_Boolean HasLower() const;
  
  //! Returns Lower limit (if there is; else, value is senseless)
  Standard_EXPORT Handle(IFSelect_IntParam) Lower() const;
  
  //! Returns Value of Lower Limit (0 if none is defined)
  Standard_EXPORT Standard_Integer LowerValue() const;
  
  //! Returns True if a Lower limit is defined
  Standard_EXPORT Standard_Boolean HasUpper() const;
  
  //! Returns Upper limit (if there is; else, value is senseless)
  Standard_EXPORT Handle(IFSelect_IntParam) Upper() const;
  
  //! Returns Value of Upper Limit (0 if none is defined)
  Standard_EXPORT Standard_Integer UpperValue() const;

  //! Returns True for an Entity of which occurrence number in the
  //! iteration is inside the selected Range (considers <rank>)
  Standard_EXPORT Standard_Boolean Sort (const Standard_Integer rank, const Handle(Standard_Transient)& ent, const Handle(Interface_InterfaceModel)& model) const Standard_OVERRIDE;

  //! Returns a text defining the criterium : following cases,
  //! " From .. Until .." or "From .." or "Until .." or "Rank no .."
  Standard_EXPORT TCollection_AsciiString ExtractLabel() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IFSelect_SelectRange,IFSelect_SelectExtract)

private:

  Handle(IFSelect_IntParam) thelower;
  Handle(IFSelect_IntParam) theupper;

};

#endif // _IFSelect_SelectRange_HeaderFile
