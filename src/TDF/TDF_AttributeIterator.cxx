// File:	TDF_AttributeIterator.cxx
//      	--------------------------
// Author:	DAUTRY Philippe
//		<fid@fox.paris1.matra-dtv.fr>
// Copyright:	Matra Datavision 1997

// Version:	0.0
// History:	Version	Date		Purpose
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

