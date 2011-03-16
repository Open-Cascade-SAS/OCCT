// File:	IFSelect_ActFunc.cxx
// Created:	Tue Feb 29 15:57:24 2000
// Author:	data exchange team
//		<det@kinox>


#include <IFSelect_ActFunc.hxx>

//=======================================================================
//function : IFSelect_ActFunc_Type_
//purpose  : 
//=======================================================================

Handle(Standard_Type)& STANDARD_TYPE(IFSelect_ActFunc)
{
  static Handle(Standard_Type) _aType = 
    new Standard_Type("IFSelect_ActFunc", sizeof(IFSelect_ActFunc), 0, NULL);

  return _aType;
}

