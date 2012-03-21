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

#include <MoniTool_Element.ixx>


void  MoniTool_Element::SetHashCode (const Standard_Integer code)
      {  thecode = code;  }

    Standard_Integer  MoniTool_Element::GetHashCode () const
      {  return thecode;  }

    Handle(Standard_Type)  MoniTool_Element::ValueType () const
      {  return DynamicType();  }

    Standard_CString  MoniTool_Element::ValueTypeName () const
      {  return "(finder)";  }


//  ####    ATTRIBUTES    ####


    const MoniTool_AttrList&  MoniTool_Element::ListAttr () const
      {  return theattrib;  }

    MoniTool_AttrList&  MoniTool_Element::ChangeAttr ()
      {  return theattrib;  }
