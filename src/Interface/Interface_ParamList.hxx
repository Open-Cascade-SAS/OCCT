// Created on: 2008-01-21
// Created by: Galina KULIKOVA
// Copyright (c) 2008-2014 OPEN CASCADE SAS
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

#ifndef _Interface_ParamList_HeaderFile
#define _Interface_ParamList_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Interface_VectorOfFileParameter.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
class Interface_FileParameter;


class Interface_ParamList;
DEFINE_STANDARD_HANDLE(Interface_ParamList, Standard_Transient)


class Interface_ParamList : public Standard_Transient
{

public:

  
  //! Creates an vector with size of memory block equal to theIncrement
  Standard_EXPORT Interface_ParamList(const Standard_Integer theIncrement = 256);
  
  //! Returns the number of elements of <me>.
    Standard_Integer Length() const;
  
  //! Returns the lower bound.
  //! Warning
    Standard_Integer Lower() const;
  
  //! Returns the upper bound.
  //! Warning
    Standard_Integer Upper() const;
  
  //! Assigns the value <Value> to the <Index>-th item of this array.
  Standard_EXPORT void SetValue (const Standard_Integer Index, const Interface_FileParameter& Value);
  
  //! Return the value of  the  <Index>th element of the
  //! array.
  Standard_EXPORT const Interface_FileParameter& Value (const Standard_Integer Index) const;
const Interface_FileParameter& operator () (const Standard_Integer Index) const
{
  return Value(Index);
}
  
  //! return the value  of the <Index>th element  of the
  //! array.
  Standard_EXPORT Interface_FileParameter& ChangeValue (const Standard_Integer Index);
Interface_FileParameter& operator () (const Standard_Integer Index)
{
  return ChangeValue(Index);
}
  
  Standard_EXPORT void Clear();




  DEFINE_STANDARD_RTTIEXT(Interface_ParamList,Standard_Transient)

protected:




private:


  Interface_VectorOfFileParameter myVector;


};


#include <Interface_ParamList.lxx>





#endif // _Interface_ParamList_HeaderFile
