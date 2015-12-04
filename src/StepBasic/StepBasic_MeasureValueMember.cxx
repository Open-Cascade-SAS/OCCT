// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <Standard_Type.hxx>
#include <StepBasic_MeasureValueMember.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_MeasureValueMember,StepData_SelectReal)

//=======================================================================
//function : StepBasic_MeasureValueMember
//purpose  : 
//=======================================================================
StepBasic_MeasureValueMember::StepBasic_MeasureValueMember()
{
  thecase = 0;
}


//=======================================================================
//function : HasName
//purpose  : 
//=======================================================================

Standard_Boolean StepBasic_MeasureValueMember::HasName () const
{
  return (thecase > 0);
}


//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

Standard_CString StepBasic_MeasureValueMember::Name () const
{
  switch (thecase) {
    case  0 : return "";
    case  1 : return "LENGTH_MEASURE";
    case  2 : return "TIME_MEASURE";
    case  3 : return "PLANE_ANGLE_MEASURE";
    case  4 : return "SOLID_ANGLE_MEASURE";
    case  5 : return "RATIO_MEASURE";
    case  6 : return "PARAMETER_VALUE";
    case  7 : return "CONTEXT_DEPENDANT_MEASURE";
    case  8 : return "POSITIVE_LENGTH_MEASURE";
    case  9 : return "POSITIVE_PLANE_ANGLE_MEASURE";
    case 10 : return "POSITIVE_RATIO_MEASURE";
    case 11 : return "AREA_MEASURE"; 
    case 12 : return "VOLUME_MEASURE"; 
    case 13 : return "MASS_MEASURE"; 
    case 14 : return "THERMODYNAMIC_TEMPERATURE_MEASURE"; 
    case 15 : return "COUNT_MEASURE";
    default : break;
  }
  return "";
}


//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

Standard_Boolean  StepBasic_MeasureValueMember::SetName (const Standard_CString name)
{
  if (!name || name[0] == '\0') thecase = 0;
//  prefiltrage par une lettre caracteristique (ne pas se tromper)
  else if (name[0] == 'L' && !strcmp (name,"LENGTH_MEASURE"))      thecase = 1;
  else if (name[1] == 'I' && !strcmp (name,"TIME_MEASURE"))        thecase = 2;
  else if (name[1] == 'L' && !strcmp (name,"PLANE_ANGLE_MEASURE")) thecase = 3;
  else if (name[0] == 'S' && !strcmp (name,"SOLID_ANGLE_MEASURE")) thecase = 4;
  else if (name[2] == 'T' && !strcmp (name,"RATIO_MEASURE"))       thecase = 5;
  else if (name[2] == 'R' && !strcmp (name,"PARAMETER_VALUE"))     thecase = 6;
  else if (name[3] == 'T' && !strcmp (name,"CONTEXT_DEPENDANT_MEASURE"))    thecase = 7;
  else if (name[9] == 'L' && !strcmp (name,"POSITIVE_LENGTH_MEASURE"))      thecase = 8;
  else if (name[9] == 'P' && !strcmp (name,"POSITIVE_PLANE_ANGLE_MEASURE")) thecase = 9;
  else if (name[9] == 'R' && !strcmp (name,"POSITIVE_RATIO_MEASURE"))       thecase = 10;
  else if (name[0] == 'A' && !strcmp (name,"AREA_MEASURE"))        thecase = 11;
  else if (name[0] == 'V' && !strcmp (name,"VOLUME_MEASURE"))      thecase = 12;
  else if (name[0] == 'M' && !strcmp (name,"MASS_MEASURE"))        thecase = 13;
  else if (name[1] == 'H' && !strcmp (name,"THERMODYNAMIC_TEMPERATURE_MEASURE")) thecase = 14;
  else if (name[2] == 'U' && !strcmp (name,"COUNT_MEASURE"))       thecase = 15;
  else return Standard_False;

  return Standard_True;
}
