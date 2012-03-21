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

#include <IFSelect_SelectType.ixx>


IFSelect_SelectType::IFSelect_SelectType ()
      {  thetype = STANDARD_TYPE(Standard_Transient);  }

    IFSelect_SelectType::IFSelect_SelectType (const Handle(Standard_Type)& atype)
      {  thetype = atype;  }

    void  IFSelect_SelectType::SetType (const Handle(Standard_Type)& atype)
      {  thetype = atype;  }

    Handle(Standard_Type)  IFSelect_SelectType::TypeForMatch () const
      {  return thetype;  }

    TCollection_AsciiString  IFSelect_SelectType::ExtractLabel () const 
      {  return TCollection_AsciiString(thetype->Name());  }
