//--------------------------------------------------------------------
//
//  File Name : IGESDimen_OrdinateDimension.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESDimen_OrdinateDimension.ixx>


    IGESDimen_OrdinateDimension::IGESDimen_OrdinateDimension ()    {  }


    void IGESDimen_OrdinateDimension::Init
  (const Handle(IGESDimen_GeneralNote)& aNote,
   const Standard_Boolean isLine,
   const Handle(IGESDimen_WitnessLine)& aLine,
   const Handle(IGESDimen_LeaderArrow)& anArrow)
{
  theNote        = aNote;
  isItLine       = isLine;
  theWitnessLine = aLine;
  theLeader      = anArrow;
  if ( (aLine.IsNull()) || (anArrow.IsNull()) ) InitTypeAndForm(218,0);
  else                                          InitTypeAndForm(218,1);
}


    Handle(IGESDimen_GeneralNote) IGESDimen_OrdinateDimension::Note () const
{
  return theNote;
}

    Handle(IGESDimen_WitnessLine) IGESDimen_OrdinateDimension::WitnessLine () const
{
  return theWitnessLine;
}

    Handle(IGESDimen_LeaderArrow) IGESDimen_OrdinateDimension::Leader () const
{
  return theLeader;
}

    Standard_Boolean IGESDimen_OrdinateDimension::IsLine () const
{
  return isItLine;
}

    Standard_Boolean IGESDimen_OrdinateDimension::IsLeader () const
{
  return !isItLine;
}
