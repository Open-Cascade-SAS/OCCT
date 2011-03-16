#include <PDocStd_Document.ixx>



//=======================================================================
//function : PDocStd_Document
//purpose  : 
//=======================================================================

PDocStd_Document::PDocStd_Document () {}


//=======================================================================
//function : PDocStd_Document
//purpose  : 
//=======================================================================

PDocStd_Document::PDocStd_Document(const Handle(PDF_Data)& data)
:myData(data)
{
}


//=======================================================================
//function : SetData
//purpose  : 
//=======================================================================

void PDocStd_Document::SetData(const Handle(PDF_Data)& data)
{
  myData = data;
}



//=======================================================================
//function : GetData
//purpose  : 
//=======================================================================

Handle(PDF_Data) PDocStd_Document::GetData() const
{
 return myData;
}

