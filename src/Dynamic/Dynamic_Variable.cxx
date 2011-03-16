// File:	Dynamic_Variable.cxx
// Created:	Wed Aug 24 10:16:48 1994
// Author:	Gilles DEBARBOUILLE
//		<gde@watson>


#include <Dynamic_Variable.ixx>

//=======================================================================
//function : Dynamic_Variable
//purpose  : 
//=======================================================================

Dynamic_Variable::Dynamic_Variable()
{
}

//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

void Dynamic_Variable::Parameter(const Handle(Dynamic_Parameter)& aparameter)
{
  theparameter = aparameter;
}

//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

Handle(Dynamic_Parameter) Dynamic_Variable::Parameter() const
{
  return theparameter;
}

//=======================================================================
//function : Mode
//purpose  : 
//=======================================================================

void Dynamic_Variable::Mode(const Dynamic_ModeEnum amode)
{
  themode = amode;
}

//=======================================================================
//function : Mode
//purpose  : 
//=======================================================================

Dynamic_ModeEnum Dynamic_Variable::Mode() const
{
  return themode;
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void Dynamic_Variable::Dump(Standard_OStream& astream) const
{
  theparameter->Dump(astream);
}
