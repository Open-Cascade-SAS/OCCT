#include <StepRepr_ItemDefinedTransformation.ixx>

StepRepr_ItemDefinedTransformation::StepRepr_ItemDefinedTransformation ()    {  }

void  StepRepr_ItemDefinedTransformation::Init
(const Handle(TCollection_HAsciiString)& aName,
 const Handle(TCollection_HAsciiString)& aDescription,
 const Handle(StepRepr_RepresentationItem)& aTransformItem1,
 const Handle(StepRepr_RepresentationItem)& aTransformItem2)
{  theName = aName;  theDescription = aDescription;
  theTransformItem1 = aTransformItem1;   theTransformItem2 = aTransformItem2; }

void  StepRepr_ItemDefinedTransformation::SetName (const Handle(TCollection_HAsciiString)& aName)
{  theName = aName;  }

Handle(TCollection_HAsciiString)  StepRepr_ItemDefinedTransformation::Name () const
{  return theName;  }

void  StepRepr_ItemDefinedTransformation::SetDescription (const Handle(TCollection_HAsciiString)& aDescription)
{  theDescription = aDescription;  }

Handle(TCollection_HAsciiString)  StepRepr_ItemDefinedTransformation::Description () const
{  return theDescription;  }

void  StepRepr_ItemDefinedTransformation::SetTransformItem1 (const Handle(StepRepr_RepresentationItem)& aTransformItem1)
{  theTransformItem1 = aTransformItem1;  }

Handle(StepRepr_RepresentationItem)  StepRepr_ItemDefinedTransformation::TransformItem1 () const
{  return theTransformItem1;  }

void  StepRepr_ItemDefinedTransformation::SetTransformItem2 (const Handle(StepRepr_RepresentationItem)& aTransformItem2)
{  theTransformItem2 = aTransformItem2;  }

Handle(StepRepr_RepresentationItem)  StepRepr_ItemDefinedTransformation::TransformItem2 () const
{  return theTransformItem2;  }
