// Created on: 1993-02-05
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



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
