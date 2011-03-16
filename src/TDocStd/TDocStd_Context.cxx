// File:	TDocStd_Context.cxx
// Created:	Mon Jun  8 15:25:21 1998
// Author:	Isabelle GRIGNON
//		<isg@bigbox.paris1.matra-dtv.fr>


#include <TDocStd_Context.ixx>

//=======================================================================
//function : TDocStd_Context
//purpose  : 
//=======================================================================

TDocStd_Context::TDocStd_Context():modifiedRef(Standard_False){}

//=======================================================================
//function : SetModifiedReferences
//purpose  : 
//=======================================================================

void TDocStd_Context::SetModifiedReferences(const Standard_Boolean Mod) {
  modifiedRef = Mod;
}

Standard_Boolean  TDocStd_Context::ModifiedReferences() const {
  return modifiedRef;
}
