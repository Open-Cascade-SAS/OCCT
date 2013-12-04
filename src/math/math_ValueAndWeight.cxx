// Created on: 2005-12-20
// Created by: Julia GERASIMOVA
// Copyright (c) 2005-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <math_ValueAndWeight.ixx>

math_ValueAndWeight::math_ValueAndWeight()
{
}

math_ValueAndWeight::math_ValueAndWeight(const Standard_Real Value,
					 const Standard_Real Weight)
     : myValue(Value),
       myWeight(Weight)
{
}

Standard_Real math_ValueAndWeight::Value() const
{
  return myValue;
}

Standard_Real math_ValueAndWeight::Weight() const
{
  return myWeight;
}
