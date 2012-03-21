// Copyright (c) 1998-1999 Matra Datavision
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

#include <Standard_Transient.hxx>
#include <Standard_Persistent.hxx>
#include <Standard_Type.hxx>
#include <Standard_RangeError.hxx>

//============================================================================
Standard_EXPORT Standard_Integer HashCode(const Standard_Address me,const Standard_Integer Upper,const Handle(Standard_Type)& aType)
{
  char             *mecharPtr;
  Standard_Integer *meintPtr = (Standard_Integer *) me;
  Standard_Integer aHashCode, aRest, i, aSize = aType->Size();
  
  Standard_RangeError_Raise_if (Upper < 1,
      "Try to apply HashCode method with negative or null argument.");

  aRest = aSize % sizeof(Standard_Integer);

  aHashCode = 0;

  if (aSize == 0) 
    aHashCode = (Standard_Integer) ptrdiff_t(me);


  for (i = 0;  (unsigned int ) i < aSize / sizeof(Standard_Integer); i++)
  {
    aHashCode = aHashCode ^ *meintPtr;
    meintPtr++;
  }
  
  mecharPtr = (char *) meintPtr;

  for (i = 0; i < aRest; i++) 
  {
    aHashCode = aHashCode ^ *mecharPtr;
    mecharPtr++;
  }


  return HashCode( aHashCode , Upper) ;
}

//============================================================================
Standard_EXPORT Standard_Integer HashCode (const Handle(Standard_Transient)& me,
                                           const Standard_Integer Upper )
{
  Standard_RangeError_Raise_if (Upper < 1,
                                "Try to apply HashCode method "
                                "with negative or null argument.");

  const Standard_Integer I = (Standard_Integer) ptrdiff_t(me.operator->());
  return HashCode( I , Upper ) ;
}

//============================================================================
Standard_EXPORT Standard_Integer HashCode(const Handle(Standard_Persistent)& me,
                                          const Standard_Integer Upper )
{
  Standard_RangeError_Raise_if (Upper < 1,
                                "Try to apply HashCode method "
                                "with negative or null argument.");

  const Standard_Integer I = (Standard_Integer) ptrdiff_t(me.operator->());
  return HashCode( I , Upper) ;
}

