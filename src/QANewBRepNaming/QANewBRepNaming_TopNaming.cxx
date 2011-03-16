// File:	QANewBRepNaming_TopNaming.cxx
// Created:	Fri Sep 24 16:51:14 1999
// Author:	Sergey ZARITCHNY
//		<szy@shamox.nnov.matra-dtv.fr>


#include <QANewBRepNaming_TopNaming.ixx>

#include <TDF_Label.hxx>

#include <Standard_NullObject.hxx>

//=======================================================================
//function : QANewBRepNaming_TopNaming
//purpose  : 
//=======================================================================

QANewBRepNaming_TopNaming::QANewBRepNaming_TopNaming()
{}

//=======================================================================
//function : QANewBRepNaming_TopNaming
//purpose  : 
//=======================================================================

QANewBRepNaming_TopNaming::QANewBRepNaming_TopNaming(const TDF_Label& Label)
{
  if(Label.IsNull())
    Standard_NullObject::Raise("QANewBRepNaming_TopNaming:: The Result label is Null ..."); 
  myResultLabel = Label;
}

