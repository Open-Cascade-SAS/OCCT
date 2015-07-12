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


#include <IGESData_IGESType.hxx>

IGESData_IGESType::IGESData_IGESType ()  {  thetype = 0; theform = 0;  }

   IGESData_IGESType::IGESData_IGESType
      (const Standard_Integer atype, const Standard_Integer aform)
      {  thetype = atype; theform = aform;  }

    Standard_Integer IGESData_IGESType::Type () const    {  return thetype;  }

    Standard_Integer IGESData_IGESType::Form () const    {  return theform;  }

    Standard_Boolean IGESData_IGESType::IsEqual (const IGESData_IGESType& other) const
      {  return (thetype == other.Type() && theform == other.Form());  }

    void IGESData_IGESType::Nullify ()      {  thetype = 0; theform = 0;  }
