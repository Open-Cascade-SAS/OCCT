//--------------------------------------------------------------------
//
//  File Name : IGESAppli_FiniteElement.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESAppli_FiniteElement.ixx>
#include <Standard_DimensionMismatch.hxx>
#include <IGESAppli.hxx>

    IGESAppli_FiniteElement::IGESAppli_FiniteElement ()    {  }


    void  IGESAppli_FiniteElement::Init
  (const Standard_Integer aType,
   const Handle(IGESAppli_HArray1OfNode)&  allNodes,
   const Handle(TCollection_HAsciiString)& aName)
{
  if (allNodes->Lower() != 1)
    Standard_DimensionMismatch::Raise("IGESAppli_FiniteElement : Init");
  theTopology = aType;
  theNodes    = allNodes;
  theName     = aName;
  InitTypeAndForm(136,0);
}

    Standard_Integer  IGESAppli_FiniteElement::Topology () const
{
  return theTopology;
}

    Handle(IGESAppli_Node)  IGESAppli_FiniteElement::Node
  (const Standard_Integer Index) const
{
  return theNodes->Value(Index);
}

    Handle(TCollection_HAsciiString)  IGESAppli_FiniteElement::Name () const
{
  return theName;
}

    Standard_Integer  IGESAppli_FiniteElement::NbNodes () const
{
  return theNodes->Length();
}
