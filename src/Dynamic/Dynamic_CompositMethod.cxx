// File:	Dynamic_CompositMethod.cxx
// Created:	Fri Aug 26 11:35:39 1994
// Author:	Gilles DEBARBOUILLE
//		<gde@watson>
// CRD : 15/04/97 : Passage WOK++ : Remplacement de TYPE par STANDARD_TYPE

#include <Dynamic_CompositMethod.ixx>


//=======================================================================
//function : Dynamic_CompositMethod
//purpose  : 
//=======================================================================

Dynamic_CompositMethod::Dynamic_CompositMethod(const Standard_CString aname)
: Dynamic_MethodDefinition(aname)
{
  thesequenceofmethods = new Dynamic_SequenceOfMethods();
}

//=======================================================================
//function : Method
//purpose  : 
//=======================================================================

void Dynamic_CompositMethod::Method(const Handle(Dynamic_Method)& amethod)
{
  if(amethod->IsKind(STANDARD_TYPE(Dynamic_MethodDefinition)))
    cout<<"bad argument type"<<endl;
  else
    thesequenceofmethods->Append(amethod);
}

//=======================================================================
//function : NumberOfMethods
//purpose  : 
//=======================================================================

Standard_Integer Dynamic_CompositMethod::NumberOfMethods() const
{
  return thesequenceofmethods->Length();
}

//=======================================================================
//function : Method
//purpose  : 
//=======================================================================

Handle(Dynamic_Method) Dynamic_CompositMethod::Method(const Standard_Integer anindex) const
{
  return thesequenceofmethods->Value(anindex);
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void Dynamic_CompositMethod::Dump(Standard_OStream& astream) const
{
  astream << "CompositMethod : " << endl;
  Dynamic_MethodDefinition::Dump(astream);
  astream << "Dump of Methods Instances : " << endl;
  for (Standard_Integer i=1; i<= thesequenceofmethods->Length(); i++) {
    astream << "Method No : " << i << endl;
    thesequenceofmethods->Value(i)->Dump(astream);
    astream << endl;
  }
}
