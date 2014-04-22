// Created on: 1995-05-17
// Created by: Mister rmi
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _V3d_ListOfTransient_HeaderFile
#define _V3d_ListOfTransient_HeaderFile

#include <TColStd_ListOfTransient.hxx>
#include <TColStd_ListIteratorOfListOfTransient.hxx>

//! List of transient objects with methods to check presence and remove elements
class V3d_ListOfTransient: public TColStd_ListOfTransient
{
public:

  //! Return true if theObject is stored in the list
  Standard_Boolean Contains (const Handle(Standard_Transient)& theObject) const
  {
    for (TColStd_ListIteratorOfListOfTransient it (*this); it.More(); it.Next())
    {
      if (it.Value() == theObject)
        return Standard_True;
    }
    return Standard_False;
  }

  //! Remove all elements equal to theObject from the list
  void Remove (const Handle(Standard_Transient)& theObject)
  {
    for (TColStd_ListIteratorOfListOfTransient it (*this); it.More();)
    {
      if (it.Value() == theObject)
	TColStd_ListOfTransient::Remove (it);
      else
        it.Next();
    }
  }
};

#endif
