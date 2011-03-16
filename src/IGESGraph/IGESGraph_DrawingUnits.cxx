//--------------------------------------------------------------------
//
//  File Name : IGESGraph_DrawingUnits.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESGraph_DrawingUnits.ixx>


    IGESGraph_DrawingUnits::IGESGraph_DrawingUnits ()    {  }


    void IGESGraph_DrawingUnits::Init
  (const Standard_Integer nbProps, const Standard_Integer aFlag,
   const Handle(TCollection_HAsciiString)& anUnit)
{
  theNbPropertyValues = nbProps;
  theFlag             = aFlag;
  theUnit             = anUnit;
  InitTypeAndForm(406,17);
}

    Standard_Integer IGESGraph_DrawingUnits::NbPropertyValues () const
{
  return theNbPropertyValues;
}

    Standard_Integer IGESGraph_DrawingUnits::Flag () const
{
  return theFlag;
}

    Handle(TCollection_HAsciiString) IGESGraph_DrawingUnits::Unit () const
{
  return theUnit;
}

    Standard_Real            IGESGraph_DrawingUnits::UnitValue () const
{
  switch (theFlag) {
    case  1 : return 0.0254;
    case  2 : return 0.001;
    case  3 : return 1.;
    case  4 : return 0.3048;
    case  5 : return 1609.27;
    case  6 : return 1.;
    case  7 : return 1000.;
    case  8 : return 0.0000254;
    case  9 : return 0.000001;
    case 10 : return 0.01;
    case 11 : return 0.0000000254;
    default : break;
  }
  return 1.;
}
