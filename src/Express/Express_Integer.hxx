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

#ifndef _Express_Integer_HeaderFile
#define _Express_Integer_HeaderFile

#include <Express_PredefinedType.hxx>
#include <Standard_Type.hxx>

class TCollection_AsciiString;

//! Implements EXPRESS type 'INTEGER'
class Express_Integer : public Express_PredefinedType
{

public:

  //! Empty constructor
  Standard_EXPORT Express_Integer();

  //! Returns "Standard_Integer"
  Standard_EXPORT virtual const TCollection_AsciiString CPPName() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(Express_Integer, Express_PredefinedType)

protected:

private:

};

#endif // _Express_Integer_HeaderFile
