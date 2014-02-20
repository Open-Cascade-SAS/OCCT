// Created on: 1993-02-05
// Created by: Gilles DEBARBOUILLE
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

#include <Dynamic_DynamicDerivedClass.ixx>

//=======================================================================
//function : Dynamic_DynamicDerivedClass
//purpose  : 
//=======================================================================

Dynamic_DynamicDerivedClass::Dynamic_DynamicDerivedClass(const Standard_CString aname)
     : Dynamic_DynamicClass(aname)
{
  thesequenceofclasses = new Dynamic_SequenceOfClasses();
}

//=======================================================================
//function : AddClass
//purpose  : 
//=======================================================================

void Dynamic_DynamicDerivedClass::AddClass(const Handle(Dynamic_DynamicClass)& aclass)
{
  thesequenceofclasses->Append(aclass);
}  

//=======================================================================
//function : Method
//purpose  : 
//=======================================================================

Handle(Dynamic_Method) Dynamic_DynamicDerivedClass::Method
       (const Standard_CString amethod) const
{
  Standard_Integer index;
  Handle(Dynamic_Method) method = Dynamic_DynamicClass::Method(amethod);

  if(method.IsNull())
    {
      for(index=1;index<=thesequenceofclasses->Length();index++)
	{
	  method = (thesequenceofclasses->Value(index))->Method(amethod);
	  if(!method.IsNull()) break;
	}
    }
  return method;
}

//=======================================================================
//function : Instance
//purpose  : 
//=======================================================================

Handle(Dynamic_DynamicInstance) Dynamic_DynamicDerivedClass::Instance() const
{
  Standard_Integer index;
  Handle(Dynamic_DynamicInstance) instance = Dynamic_DynamicClass::Instance();

  for(index=1;index<=thesequenceofclasses->Length();index++)
    {
      ((thesequenceofclasses->Value(index))->Instance())->Parameter(instance);
    }
  return instance;
}
