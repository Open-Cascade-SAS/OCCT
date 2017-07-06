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


#include <MoniTool_IntVal.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(MoniTool_IntVal,Standard_Transient)

MoniTool_IntVal::MoniTool_IntVal  (const Standard_Integer val)    {  theval = val;  }

Standard_Integer  MoniTool_IntVal::Value () const  {  return theval;  }

Standard_Integer&  MoniTool_IntVal::CValue ()  {  return theval;  }
