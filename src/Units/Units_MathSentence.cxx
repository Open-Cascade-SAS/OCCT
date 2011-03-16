// File:	Units_MathSentence.cxx
// Created:	Wed Jun 24 12:48:45 1992
// Author:	Gilles DEBARBOUILLE
//		<gde@phobox>


#include <Units_MathSentence.ixx>
#include <Units.hxx>

//=======================================================================
//function : Units_MathSentence
//purpose  : 
//=======================================================================

Units_MathSentence::Units_MathSentence(const Standard_CString astring)
     : Units_Sentence(Units::LexiconFormula(), astring)
{
  SetConstants();
}
