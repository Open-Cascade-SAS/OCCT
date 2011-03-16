// File:	TDocStd_XLinkIterator.cxx
//      	----------------------
// Author:	DAUTRY Philippe
//		<fid@fox.paris1.matra-dtv.fr>
// Copyright:	Matra Datavision 1997

// Version:	0.0
// History:	Version	Date		Purpose
//		0.0	Sep 15 1997	Creation



#include <TDocStd_XLinkIterator.ixx>
#include <TDF_Data.hxx>
#include <TDocStd_XLink.hxx>
#include <TDocStd_XLinkRoot.hxx>


//=======================================================================
//function : TDocStd_XLinkIterator
//purpose  : 
//=======================================================================

TDocStd_XLinkIterator::TDocStd_XLinkIterator()
: myValue(NULL)
{}


//=======================================================================
//function : TDocStd_XLinkIterator
//purpose  : 
//=======================================================================

TDocStd_XLinkIterator::TDocStd_XLinkIterator
(const Handle(TDocStd_Document)& DOC)
: myValue(NULL)
{ Init(DOC); }


//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void TDocStd_XLinkIterator::Initialize(const Handle(TDocStd_Document)& DOC) 
{ myValue = NULL; Init(DOC); }


//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void TDocStd_XLinkIterator::Next() 
{
  if (myValue == NULL) Standard_NoMoreObject::Raise("");
  else                 myValue = myValue->Next();
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void TDocStd_XLinkIterator::Init(const Handle(TDocStd_Document)& DOC) 
{
  Handle(TDocStd_XLinkRoot) xRefRoot;
  if (DOC->GetData()->Root().FindAttribute(TDocStd_XLinkRoot::GetID(),xRefRoot))
    myValue = xRefRoot->First();
}

