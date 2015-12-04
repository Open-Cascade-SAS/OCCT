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


#include <IFSelect_SelectControl.hxx>
#include <IFSelect_Selection.hxx>
#include <IFSelect_SelectionIterator.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IFSelect_SelectControl,IFSelect_Selection)

Handle(IFSelect_Selection)  IFSelect_SelectControl::MainInput () const 
      {  return themain;  }

    Handle(IFSelect_Selection)  IFSelect_SelectControl::SecondInput () const
      {  return thesecond;  }

    Standard_Boolean  IFSelect_SelectControl::HasSecondInput () const
      {  return (!thesecond.IsNull());  }

    void  IFSelect_SelectControl::SetMainInput
  (const Handle(IFSelect_Selection)& sel)
      {  themain = sel;  }

    void  IFSelect_SelectControl::SetSecondInput
  (const Handle(IFSelect_Selection)& sel)
      {  thesecond = sel;  }


    void  IFSelect_SelectControl::FillIterator
  (IFSelect_SelectionIterator& iter) const 
{
  iter.AddItem(themain);
  if (!thesecond.IsNull()) iter.AddItem(thesecond);
}
