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


#include <IGESData_DefSwitch.hxx>

//  DefSwitch : represente une definition, soit vide (-> valeur = 0),
//  soit comme rang dans une table (-> valeur > 0 ce rang),
//  soit comme reference (-> valeur < 0), la reference elle-meme est ailleurs
//=======================================================================
//function : IGESData_DefSwitch
//purpose  : Default constructor.
//=======================================================================
IGESData_DefSwitch::IGESData_DefSwitch()
: theval(0)
{
}

//=======================================================================
//function : SetVoid
//purpose  : 
//=======================================================================
void IGESData_DefSwitch::SetVoid()
{
  theval = 0;
}

//=======================================================================
//function : SetReference
//purpose  : 
//=======================================================================
void IGESData_DefSwitch::SetReference()
{
  theval = -1;
}

//=======================================================================
//function : SetRank
//purpose  : 
//=======================================================================
void IGESData_DefSwitch::SetRank(const Standard_Integer theRank)
{
  theval = theRank;
}

//=======================================================================
//function : DefType
//purpose  : 
//=======================================================================
IGESData_DefType IGESData_DefSwitch::DefType() const
{
  if (theval <  0)
    return IGESData_DefReference;

  if (theval >  0)
    return IGESData_DefValue;

  return IGESData_DefVoid;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================
Standard_Integer IGESData_DefSwitch::Value() const
{
  return theval;
}
