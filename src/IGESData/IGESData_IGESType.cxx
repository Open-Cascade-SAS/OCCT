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

#include <IGESData_IGESType.ixx>



IGESData_IGESType::IGESData_IGESType ()  {  thetype = 0; theform = 0;  }

   IGESData_IGESType::IGESData_IGESType
      (const Standard_Integer atype, const Standard_Integer aform)
      {  thetype = atype; theform = aform;  }

    Standard_Integer IGESData_IGESType::Type () const    {  return thetype;  }

    Standard_Integer IGESData_IGESType::Form () const    {  return theform;  }

    Standard_Boolean IGESData_IGESType::IsEqual (const IGESData_IGESType& other) const
      {  return (thetype == other.Type() && theform == other.Form());  }

    void IGESData_IGESType::Nullify ()      {  thetype = 0; theform = 0;  }
