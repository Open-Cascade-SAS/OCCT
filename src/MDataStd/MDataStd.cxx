// File:	MDataStd.cxx
//      	------------
// Author:	DAUTRY Philippe
// modified     Sergey Zaritchny
// Copyright:	Matra Datavision 1997

// Version:	0.0
// History:	Version	Date		Purpose
//		0.0	Aug  4 1997	Creation



#include <MDataStd.ixx>
#include <MDF_ASDriverTable.hxx>
#include <MDF_ARDriverTable.hxx>
#include <CDM_MessageDriver.hxx>
//
// Storage
//
#include <MDataStd_DirectoryStorageDriver.hxx>
#include <MDataStd_UAttributeStorageDriver.hxx>
#include <MDataStd_IntegerArrayStorageDriver.hxx>
#include <MDataStd_RealArrayStorageDriver.hxx>
#include <MDataStd_ExtStringArrayStorageDriver.hxx>
#include <MDataStd_NameStorageDriver.hxx>
#include <MDataStd_CommentStorageDriver.hxx>
#include <MDataStd_IntegerStorageDriver.hxx>
#include <MDataStd_RealStorageDriver.hxx>
#include <MDataStd_VariableStorageDriver.hxx>
#include <MDataStd_ExpressionStorageDriver.hxx>
#include <MDataStd_RelationStorageDriver.hxx>
#include <MDataStd_NoteBookStorageDriver.hxx>
#include <MDataStd_TreeNodeStorageDriver.hxx>

#include <MDataStd_TickStorageDriver.hxx>
#include <MDataStd_IntegerListStorageDriver.hxx>
#include <MDataStd_RealListStorageDriver.hxx>
#include <MDataStd_ExtStringListStorageDriver.hxx>
#include <MDataStd_BooleanListStorageDriver.hxx>
#include <MDataStd_ReferenceListStorageDriver.hxx>
#include <MDataStd_BooleanArrayStorageDriver.hxx>
#include <MDataStd_ReferenceArrayStorageDriver.hxx>
#include <MDataStd_ByteArrayStorageDriver.hxx>
#include <MDataStd_NamedDataStorageDriver.hxx>
#include <MDataStd_AsciiStringStorageDriver.hxx>
#include <MDataStd_IntPackedMapStorageDriver.hxx>
//
// Retrieval
//
#include <MDataStd_DirectoryRetrievalDriver.hxx>
#include <MDataStd_RealArrayRetrievalDriver.hxx>
#include <MDataStd_ExtStringArrayRetrievalDriver.hxx>
#include <MDataStd_UAttributeRetrievalDriver.hxx>
#include <MDataStd_IntegerArrayRetrievalDriver.hxx>
#include <MDataStd_NameRetrievalDriver.hxx>
#include <MDataStd_CommentRetrievalDriver.hxx>
#include <MDataStd_IntegerRetrievalDriver.hxx>
#include <MDataStd_RealRetrievalDriver.hxx>
#include <MDataStd_VariableRetrievalDriver.hxx>
#include <MDataStd_ExpressionRetrievalDriver.hxx>
#include <MDataStd_RelationRetrievalDriver.hxx>
#include <MDataStd_NoteBookRetrievalDriver.hxx>
#include <MDataStd_TreeNodeRetrievalDriver.hxx>

#include <MDataStd_TickRetrievalDriver.hxx>
#include <MDataStd_IntegerListRetrievalDriver.hxx>
#include <MDataStd_RealListRetrievalDriver.hxx>
#include <MDataStd_ExtStringListRetrievalDriver.hxx>
#include <MDataStd_BooleanListRetrievalDriver.hxx>
#include <MDataStd_ReferenceListRetrievalDriver.hxx>
#include <MDataStd_BooleanArrayRetrievalDriver.hxx>
#include <MDataStd_ReferenceArrayRetrievalDriver.hxx>
#include <MDataStd_ByteArrayRetrievalDriver.hxx>
#include <MDataStd_NamedDataRetrievalDriver.hxx>
#include <MDataStd_AsciiStringRetrievalDriver.hxx>
#include <MDataStd_IntPackedMapRetrievalDriver.hxx>
#include <MDataStd_ByteArrayRetrievalDriver_1.hxx>
#include <MDataStd_IntegerArrayRetrievalDriver_1.hxx>
#include <MDataStd_RealArrayRetrievalDriver_1.hxx>
#include <MDataStd_ExtStringArrayRetrievalDriver_1.hxx>
#include <MDataStd_IntPackedMapRetrievalDriver_1.hxx>

//
// enums
//
#include <TDataStd_RealEnum.hxx>
#include <Standard_DomainError.hxx>

//=======================================================================
//function : AddStorageDriver
//purpose  : 
//=======================================================================

void MDataStd::AddStorageDrivers
(const Handle(MDF_ASDriverHSequence)& aDriverSeq, const Handle(CDM_MessageDriver)& theMsgDriver)
{
  aDriverSeq->Append(new MDataStd_DirectoryStorageDriver(theMsgDriver));
  aDriverSeq->Append(new MDataStd_IntegerArrayStorageDriver(theMsgDriver));
  aDriverSeq->Append(new MDataStd_RealArrayStorageDriver(theMsgDriver));
  aDriverSeq->Append(new MDataStd_UAttributeStorageDriver(theMsgDriver)); 
  aDriverSeq->Append(new MDataStd_NameStorageDriver(theMsgDriver)); 
  aDriverSeq->Append(new MDataStd_CommentStorageDriver(theMsgDriver));
  aDriverSeq->Append(new MDataStd_IntegerStorageDriver(theMsgDriver));
  aDriverSeq->Append(new MDataStd_RealStorageDriver(theMsgDriver));

  aDriverSeq->Append(new MDataStd_VariableStorageDriver(theMsgDriver)); 
  aDriverSeq->Append(new MDataStd_ExpressionStorageDriver(theMsgDriver)); 
  aDriverSeq->Append(new MDataStd_RelationStorageDriver(theMsgDriver));  
  aDriverSeq->Append(new MDataStd_NoteBookStorageDriver(theMsgDriver));  
  aDriverSeq->Append(new MDataStd_TreeNodeStorageDriver(theMsgDriver));
  aDriverSeq->Append(new MDataStd_ExtStringArrayStorageDriver(theMsgDriver));

  aDriverSeq->Append(new MDataStd_TickStorageDriver(theMsgDriver));
  aDriverSeq->Append(new MDataStd_IntegerListStorageDriver(theMsgDriver));
  aDriverSeq->Append(new MDataStd_RealListStorageDriver(theMsgDriver));
  aDriverSeq->Append(new MDataStd_ExtStringListStorageDriver(theMsgDriver));
  aDriverSeq->Append(new MDataStd_BooleanListStorageDriver(theMsgDriver));
  aDriverSeq->Append(new MDataStd_ReferenceListStorageDriver(theMsgDriver));
  aDriverSeq->Append(new MDataStd_BooleanArrayStorageDriver(theMsgDriver));
  aDriverSeq->Append(new MDataStd_ReferenceArrayStorageDriver(theMsgDriver));
  aDriverSeq->Append(new MDataStd_ByteArrayStorageDriver(theMsgDriver));
  aDriverSeq->Append(new MDataStd_NamedDataStorageDriver(theMsgDriver));
  aDriverSeq->Append(new MDataStd_AsciiStringStorageDriver(theMsgDriver));
  aDriverSeq->Append(new MDataStd_IntPackedMapStorageDriver(theMsgDriver));
}


//=======================================================================
//function : AddRetrievalDriver
//purpose  : 
//=======================================================================

void MDataStd::AddRetrievalDrivers
(const Handle(MDF_ARDriverHSequence)& aDriverSeq,const Handle(CDM_MessageDriver)& theMsgDriver)
{
  aDriverSeq->Append(new MDataStd_DirectoryRetrievalDriver(theMsgDriver)); 
  aDriverSeq->Append(new MDataStd_IntegerArrayRetrievalDriver(theMsgDriver));
  aDriverSeq->Append(new MDataStd_RealArrayRetrievalDriver(theMsgDriver));
  aDriverSeq->Append(new MDataStd_UAttributeRetrievalDriver(theMsgDriver)); 
  aDriverSeq->Append(new MDataStd_NameRetrievalDriver(theMsgDriver)); 
  aDriverSeq->Append(new MDataStd_CommentRetrievalDriver(theMsgDriver));
  aDriverSeq->Append(new MDataStd_IntegerRetrievalDriver(theMsgDriver));
  aDriverSeq->Append(new MDataStd_RealRetrievalDriver(theMsgDriver));

  aDriverSeq->Append(new MDataStd_VariableRetrievalDriver(theMsgDriver));  
  aDriverSeq->Append(new MDataStd_ExpressionRetrievalDriver(theMsgDriver));  
  aDriverSeq->Append(new MDataStd_RelationRetrievalDriver(theMsgDriver)); 
  aDriverSeq->Append(new MDataStd_NoteBookRetrievalDriver(theMsgDriver));  
  aDriverSeq->Append(new MDataStd_TreeNodeRetrievalDriver(theMsgDriver));
  aDriverSeq->Append(new MDataStd_ExtStringArrayRetrievalDriver(theMsgDriver));

  aDriverSeq->Append(new MDataStd_TickRetrievalDriver(theMsgDriver));
  aDriverSeq->Append(new MDataStd_IntegerListRetrievalDriver(theMsgDriver));
  aDriverSeq->Append(new MDataStd_RealListRetrievalDriver(theMsgDriver));
  aDriverSeq->Append(new MDataStd_ExtStringListRetrievalDriver(theMsgDriver));
  aDriverSeq->Append(new MDataStd_BooleanListRetrievalDriver(theMsgDriver));
  aDriverSeq->Append(new MDataStd_ReferenceListRetrievalDriver(theMsgDriver));
  aDriverSeq->Append(new MDataStd_BooleanArrayRetrievalDriver(theMsgDriver));
  aDriverSeq->Append(new MDataStd_ReferenceArrayRetrievalDriver(theMsgDriver));
  aDriverSeq->Append(new MDataStd_ByteArrayRetrievalDriver(theMsgDriver));
  aDriverSeq->Append(new MDataStd_NamedDataRetrievalDriver(theMsgDriver));
  aDriverSeq->Append(new MDataStd_AsciiStringRetrievalDriver(theMsgDriver));
  aDriverSeq->Append(new MDataStd_IntPackedMapRetrievalDriver(theMsgDriver));
  aDriverSeq->Append(new MDataStd_ByteArrayRetrievalDriver_1(theMsgDriver));
  aDriverSeq->Append(new MDataStd_IntegerArrayRetrievalDriver_1(theMsgDriver));
  aDriverSeq->Append(new MDataStd_RealArrayRetrievalDriver_1(theMsgDriver));
  aDriverSeq->Append(new MDataStd_ExtStringArrayRetrievalDriver_1(theMsgDriver));
  aDriverSeq->Append(new MDataStd_IntPackedMapRetrievalDriver_1(theMsgDriver));
}


//=======================================================================
//function : RealDimensionToInteger
//purpose  : 
//=======================================================================

Standard_Integer MDataStd::RealDimensionToInteger(const TDataStd_RealEnum e) 
{
  switch (e) {
  case TDataStd_SCALAR  : return  0;
  case TDataStd_LENGTH  : return  1;
  case TDataStd_ANGULAR : return  2;
    
  default:
    Standard_DomainError::Raise("TDataStd_RealEnum; enum term unknown");
  }
  return 0;
}

//=======================================================================
//function : IntegerToRealDimension
//purpose  : 
//=======================================================================

TDataStd_RealEnum MDataStd::IntegerToRealDimension(const Standard_Integer i) 
{
  switch(i)
    {
      // planar constraints
    case  0 : return TDataStd_SCALAR;
    case  1 : return TDataStd_LENGTH;
    case  2 : return TDataStd_ANGULAR;
      default :
	Standard_DomainError::Raise("TDataStd_RealEnum; enum term unknown ");
    }
  return TDataStd_SCALAR;
}
