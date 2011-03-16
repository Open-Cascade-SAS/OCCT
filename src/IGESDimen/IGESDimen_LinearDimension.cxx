//--------------------------------------------------------------------
//
//  File Name : IGESDimen_LinearDimension.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESDimen_LinearDimension.ixx>
#include <Standard_OutOfRange.hxx>


    IGESDimen_LinearDimension::IGESDimen_LinearDimension ()    {  }


    void  IGESDimen_LinearDimension::Init
  (const Handle(IGESDimen_GeneralNote)& aNote,
   const Handle(IGESDimen_LeaderArrow)& aLeader,
   const Handle(IGESDimen_LeaderArrow)& anotherLeader,
   const Handle(IGESDimen_WitnessLine)& aWitness,
   const Handle(IGESDimen_WitnessLine)& anotherWitness)
{
  theNote          = aNote;
  theFirstLeader   = aLeader;
  theSecondLeader  = anotherLeader;
  theFirstWitness  = aWitness;        
  theSecondWitness = anotherWitness;        
  InitTypeAndForm(216,FormNumber());
//  FormNumber : Nature of the Dimension 0-1-2
}


    void  IGESDimen_LinearDimension::SetFormNumber (const Standard_Integer fm)
{
  if (fm < 0 || fm > 2) Standard_OutOfRange::Raise
    ("IGESDimen_LinearDimension : SetFormNumber");
  InitTypeAndForm(216,fm);
}


    Handle(IGESDimen_GeneralNote)  IGESDimen_LinearDimension::Note () const 
{
  return theNote;
}

    Handle(IGESDimen_LeaderArrow)  IGESDimen_LinearDimension::FirstLeader () const 
{
  return theFirstLeader;
}

    Handle(IGESDimen_LeaderArrow)  IGESDimen_LinearDimension::SecondLeader () const 
{
  return theSecondLeader;
}

    Standard_Boolean  IGESDimen_LinearDimension::HasFirstWitness () const 
{
  return (! theFirstWitness.IsNull());
}

    Handle(IGESDimen_WitnessLine)  IGESDimen_LinearDimension::FirstWitness () const 
{
  return theFirstWitness;
}

    Standard_Boolean  IGESDimen_LinearDimension::HasSecondWitness () const 
{
  return (! theSecondWitness.IsNull());
}

    Handle(IGESDimen_WitnessLine)  IGESDimen_LinearDimension::SecondWitness () const 
{
  return theSecondWitness;
}
