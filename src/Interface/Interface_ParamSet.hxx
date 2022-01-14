// Created on: 1992-10-29
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

#ifndef _Interface_ParamSet_HeaderFile
#define _Interface_ParamSet_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_PCharacter.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>
#include <Interface_ParamType.hxx>
class Interface_ParamList;
class Interface_FileParameter;


class Interface_ParamSet;
DEFINE_STANDARD_HANDLE(Interface_ParamSet, Standard_Transient)

//! Defines an ordered set of FileParameters, in a way to be
//! efficient as in memory requirement or in speed
class Interface_ParamSet : public Standard_Transient
{

public:

  
  //! Creates an empty ParamSet, beginning at number "nst" and of
  //! initial reservation "nres" : the "nres" first parameters
  //! which follow "ndeb" (included) will be put in an Array
  //! (a ParamList). The remainders are set in Next(s) ParamSet(s)
  Standard_EXPORT Interface_ParamSet(const Standard_Integer nres, const Standard_Integer nst = 1);
  
  //! Adds a parameter defined as its Value (CString and length) and
  //! Type. Optional EntityNumber (for FileReaderData) can be given
  //! Allows a better memory management than Appending a
  //! complete FileParameter
  //! If <lnval> < 0, <val> is assumed to be managed elsewhere : its
  //! address is stored as such. Else, <val> is copied in a locally
  //! (quickly) managed Page of Characters
  //! Returns new count of recorded Parameters
  Standard_EXPORT Standard_Integer Append (const Standard_CString val, const Standard_Integer lnval, const Interface_ParamType typ, const Standard_Integer nument);
  
  //! Adds a parameter at the end of the ParamSet (transparent
  //! about reservation and "Next")
  //! Returns new count of recorded Parameters
  Standard_EXPORT Standard_Integer Append (const Interface_FileParameter& FP);
  
  //! Returns the total count of parameters (including nexts)
  Standard_EXPORT Standard_Integer NbParams() const;
  
  //! Returns a parameter identified by its number
  Standard_EXPORT const Interface_FileParameter& Param (const Standard_Integer num) const;
  
  //! Same as above, but in order to be modified on place
  Standard_EXPORT Interface_FileParameter& ChangeParam (const Standard_Integer num);
  
  //! Changes a parameter identified by its number
  Standard_EXPORT void SetParam (const Standard_Integer num, const Interface_FileParameter& FP);
  
  //! Builds and returns the sub-list corresponding to parameters,
  //! from "num" included, with count "nb"
  //! If <num> and <nb> are zero, returns the whole list
  Standard_EXPORT Handle(Interface_ParamList) Params (const Standard_Integer num, const Standard_Integer nb) const;
  
  //! Destructor (waiting for transparent memory management)
  Standard_EXPORT void Destroy();
~Interface_ParamSet()
{
  Destroy();
}



  DEFINE_STANDARD_RTTIEXT(Interface_ParamSet,Standard_Transient)

protected:




private:


  Standard_PCharacter theval;
  Standard_Integer thelnval;
  Standard_Integer thelnres;
  Standard_Integer thenbpar;
  Standard_Integer themxpar;
  Handle(Interface_ParamList) thelist;
  Handle(Interface_ParamSet) thenext;


};







#endif // _Interface_ParamSet_HeaderFile
