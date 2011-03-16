// Copyright: 	Matra-Datavision 1994
// File:	Transfer_VoidBinder.cxx
// Created:	Mon Jun 27 16:16:47 1994
// Author:	Christian CAILLET
//		<design>

#include <Transfer_VoidBinder.ixx>

    Transfer_VoidBinder::Transfer_VoidBinder ()    {  }

//    Standard_Boolean  Transfer_VoidBinder::IsMultiple () const
//      { return Standard_False;  }


    Handle(Standard_Type)  Transfer_VoidBinder::ResultType () const
      {  return DynamicType();  }

    Standard_CString  Transfer_VoidBinder::ResultTypeName () const
      {  return "(void)";  }
