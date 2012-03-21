// Created by: DAUTRY Philippe
// Copyright (c) 1997-1999 Matra Datavision
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

//      	--------------------------

// Version:	0.0
//Version	Date		Purpose
//		0.0	Feb  7 1997	Creation



#include <TDF_AttributeIterator.hxx>

//=======================================================================
//function : TDF_AttributeIterator
//purpose  : 
//=======================================================================

TDF_AttributeIterator::TDF_AttributeIterator()
  : myValue             (0L),
    myWithoutForgotten  (Standard_True)
{}


//=======================================================================
//function : TDF_AttributeIterator
//purpose  : 
//=======================================================================

TDF_AttributeIterator::TDF_AttributeIterator
                (const TDF_Label& aLabel,
                 const Standard_Boolean withoutForgotten)
 : myValue            (0L),
   myWithoutForgotten (withoutForgotten)
{
  const Handle(TDF_Attribute)& aFirstAttribute =
    aLabel.myLabelNode->FirstAttribute();
  if (!aFirstAttribute.IsNull())
    goToNext (aFirstAttribute);
}


//=======================================================================
//function : TDF_AttributeIterator
//purpose  : 
//=======================================================================

TDF_AttributeIterator::TDF_AttributeIterator
                (const TDF_LabelNodePtr aLabelNode,
                 const Standard_Boolean withoutForgotten)
 : myValue              (0L),
   myWithoutForgotten   (withoutForgotten)
{
  const Handle(TDF_Attribute)& aFirstAttribute = aLabelNode->FirstAttribute();
  if (!aFirstAttribute.IsNull())
    goToNext (aFirstAttribute);
}


//=======================================================================
//function : TDF_AttributeIterator
//purpose  : 
//=======================================================================

void TDF_AttributeIterator::Initialize
                (const TDF_Label& aLabel,
                 const Standard_Boolean withoutForgotten)
{
  myWithoutForgotten = withoutForgotten;
  const Handle(TDF_Attribute)& aFirstAttribute =
    aLabel.myLabelNode->FirstAttribute();
  if (aFirstAttribute.IsNull())
    myValue = 0L;
  else
    goToNext (aFirstAttribute);
}


//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void TDF_AttributeIterator::Next() 
{
  // A little bit complicated...
  // but necessary if we want to find sometimes the Forgotten attributes.
  if (myValue) {
    const Handle(TDF_Attribute)& anAttribute = myValue->myNext;
    if (anAttribute.IsNull())
      myValue = 0L;
    else
      goToNext (anAttribute);
  }
}

//=======================================================================
//function : goToNext
//purpose  : private method, used by the above
//=======================================================================

void TDF_AttributeIterator::goToNext (const Handle(TDF_Attribute)& anAttr)
{
  myValue = anAttr.operator->();
  if (myWithoutForgotten) {
    while (myValue->IsForgotten()) {
      const Handle(TDF_Attribute)& anAttribute = myValue->myNext;
      if (anAttribute.IsNull()) {
        myValue = 0L;
        break;
      }
      myValue = anAttribute.operator->();
    }
  }
}

