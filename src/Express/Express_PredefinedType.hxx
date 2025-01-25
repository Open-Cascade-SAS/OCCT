// Copyright (c) 1999-2020 OPEN CASCADE SAS
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

#ifndef _Express_PredefinedType_HeaderFile
#define _Express_PredefinedType_HeaderFile

#include <Standard_Type.hxx>
#include <Express_Type.hxx>

//! Base class for predefined types (like NUMBER or STRING)
//! in EXPRESS schema
class Express_PredefinedType : public Express_Type
{

public:
  //! Returns True
  Standard_EXPORT virtual Standard_Boolean IsStandard() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(Express_PredefinedType, Express_Type)

protected:
  //! Empty constructor
  Standard_EXPORT Express_PredefinedType();

private:
};

#endif // _Express_PredefinedType_HeaderFile
