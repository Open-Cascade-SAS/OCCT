//--------------------------------------------------------------------
//
//  File Name : IGESAppli_NodalResults.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESAppli_NodalResults.ixx>
#include <Standard_OutOfRange.hxx>
#include <TColStd_HArray1OfReal.hxx>


    IGESAppli_NodalResults::IGESAppli_NodalResults ()    {  }


// Data : Col -> // Nodes.  Row : Data per Node

    void  IGESAppli_NodalResults::Init
  (const Handle(IGESDimen_GeneralNote)&    aNote,
   const Standard_Integer aNumber, const Standard_Real aTime,
   const Handle(TColStd_HArray1OfInteger)& allNodeIdentifiers,
   const Handle(IGESAppli_HArray1OfNode)&  allNodes,
   const Handle(TColStd_HArray2OfReal)&    allData)
{
  if (allNodes->Lower()   != 1 || allNodeIdentifiers->Lower() != 1 ||
      allNodes->Length()  != allNodeIdentifiers->Length() ||
      allData->LowerCol() != 1 || allData->LowerRow() != 1 ||
      allNodes->Length()  != allData->UpperRow() )
    Standard_DimensionMismatch::Raise("IGESAppli_NodalResults : Init");
  theNote            = aNote;
  theSubCaseNum      = aNumber;
  theTime            = aTime;
  theNodeIdentifiers = allNodeIdentifiers;
  theNodes           = allNodes;
  theData            = allData;
  InitTypeAndForm(146,FormNumber());
// FormNumber -> Type of the Results
}

    void  IGESAppli_NodalResults::SetFormNumber (const Standard_Integer form)
{
  if (form < 0 || form > 34) Standard_OutOfRange::Raise
    ("IGESAppli_NodalResults : SetFormNumber");
  InitTypeAndForm(146,form);
}


    Handle(IGESDimen_GeneralNote)  IGESAppli_NodalResults::Note () const
{
  return theNote;
}

    Handle(IGESAppli_Node)  IGESAppli_NodalResults::Node
  (const Standard_Integer Index) const
{
  return theNodes->Value(Index);
}

    Standard_Integer  IGESAppli_NodalResults::NbNodes () const
{
  return theNodes->Length();
}

    Standard_Integer  IGESAppli_NodalResults::SubCaseNumber () const
{
  return theSubCaseNum;
}

    Standard_Real  IGESAppli_NodalResults::Time () const
{
  return theTime;
}

    Standard_Integer  IGESAppli_NodalResults::NbData () const
{
  return theData->RowLength();
}

    Standard_Integer  IGESAppli_NodalResults::NodeIdentifier
  (const Standard_Integer Index) const
{
  return theNodeIdentifiers->Value(Index);
}

    Standard_Real  IGESAppli_NodalResults::Data
  (const Standard_Integer NodeNum, const Standard_Integer DataNum) const
{
  return theData->Value(NodeNum,DataNum);
}
