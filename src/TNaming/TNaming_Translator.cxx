// File:	TNaming_Translator.cxx
// Created:	Wed Jun 30 16:37:58 1999
// Author:	Sergey ZARITCHNY
//		<szy@philipox.nnov.matra-dtv.fr>


#include <TNaming_Translator.ixx>

#include <TopoDS_Shape.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_IndexedDataMapOfTransientTransient.hxx>
#include <TopoDS_TShape.hxx>
#include <BRepTools.hxx>
#include <TopLoc_Datum3D.hxx>
#include <BRep_TVertex.hxx>
#include <BRep_TFace.hxx>
#include <BRep_TEdge.hxx>
#include <TopoDS_TWire.hxx>
#include <TopoDS_TShell.hxx>
#include <TopoDS_TSolid.hxx>
#include <TopoDS_TCompSolid.hxx>
#include <TopoDS_TCompound.hxx>
#include <TNaming_CopyShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeShape.hxx>

//=======================================================================
//function : TNaming_Translator
//purpose  : 
//=======================================================================

TNaming_Translator::TNaming_Translator () :myIsDone(Standard_False) 
{
  myDataMapOfResults.Clear();
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void TNaming_Translator::Add(const TopoDS_Shape& aShape) 
{
 TopoDS_Shape aResult;
 myDataMapOfResults.Bind(aShape, aResult);
}

//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean TNaming_Translator::IsDone() const
{
  return myIsDone;
}

//=======================================================================
//function : Copied
//purpose  : 
//=======================================================================

const TopTools_DataMapOfShapeShape& TNaming_Translator::Copied() const
{
  return myDataMapOfResults;
}

//=======================================================================
//function : Copied
//purpose  : find bind shape if it is in the Map
//=======================================================================

const TopoDS_Shape TNaming_Translator::Copied(const TopoDS_Shape& aShape) const
{
  TopoDS_Shape aResult;
  if(myDataMapOfResults.IsBound(aShape))
    aResult = myDataMapOfResults.Find(aShape);
  return aResult;
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void TNaming_Translator::Perform()
{
  TopoDS_Shape Result;
  TopTools_DataMapIteratorOfDataMapOfShapeShape itm(myDataMapOfResults);
  for(;itm.More();itm.Next()) {
    TNaming_CopyShape::CopyTool(itm.Key(), myMap, Result);
      if(!Result.IsNull())
	myDataMapOfResults(itm.Key()) = Result;
      Result.Nullify();
    }
  if(myDataMapOfResults.Extent()) myIsDone = Standard_True;
}

//=======================================================================
//function : DumpMap
//purpose  : 
//=======================================================================

void TNaming_Translator::DumpMap(const Standard_Boolean isWrite) const
{
  TCollection_AsciiString name("Map");
  TCollection_AsciiString keyname;
  TCollection_AsciiString itemname;
  keyname  = name.Cat("_Key");
  itemname = name.Cat("_Item");

  if (!myMap.Extent()) return;
  else 
    cout <<"TNaming_Translator:: IndexedDataMap Extent = "<< myMap.Extent() << endl;
  
  for (Standard_Integer i=1; i <= myMap.Extent(); i++)
    {
      cout <<"TNaming_Translator::DumpMap:  Index = "<< i << " Type = "<< (myMap.FindKey(i))->DynamicType() << endl;
      Handle(Standard_Type) T = (myMap.FindKey(i))->DynamicType();
      if((T == STANDARD_TYPE (BRep_TVertex)) ||(T == STANDARD_TYPE (BRep_TEdge)) ||
	 T == STANDARD_TYPE (BRep_TFace)|| T == STANDARD_TYPE (TopoDS_TWire)||
	 T == STANDARD_TYPE (TopoDS_TShell) || T == STANDARD_TYPE (TopoDS_TSolid) ||
	 T == STANDARD_TYPE (TopoDS_TCompSolid)|| T == STANDARD_TYPE (TopoDS_TCompound))
	{
	  if(isWrite) {
	    const Handle(TopoDS_TShape)& key = Handle(TopoDS_TShape)::DownCast(myMap.FindKey(i));
	    const Handle(TopoDS_TShape)& item = Handle(TopoDS_TShape)::DownCast(myMap.FindFromIndex(i));
	    TopoDS_Shape S1; S1.TShape(key);
	    TopoDS_Shape S2; S2.TShape(item);
	    BRepTools::Write(S1, keyname.Cat(i).ToCString());
	    BRepTools::Write(S2, itemname.Cat(i).ToCString());	  
	  }
	}
      else if((myMap.FindKey(i))->DynamicType() == STANDARD_TYPE (TopLoc_Datum3D))
	{
	  if(isWrite) {
	    const Handle(TopLoc_Datum3D) key = Handle(TopLoc_Datum3D)::DownCast(myMap.FindKey(i));
	    const Handle(TopLoc_Datum3D) Item = Handle(TopLoc_Datum3D)::DownCast(myMap.FindFromIndex(i));
	    cout << "TNaming_Translator::DumpMap: Location_Key_name  = "<< keyname.Cat(i).ToCString()<< endl;
	    key->ShallowDump(cout);
	    cout << "TNaming_Translator::DumpMap: Location_Item_name = "<< itemname.Cat(i).ToCString()<< endl;
	    Item->ShallowDump(cout);
	  }						       
	}
      else {
	cout <<"TNaming_Translator::DumpMap: Unexpected Type >> Idex = "<< i << " Type = "<<(myMap.FindKey(i))->DynamicType()<< endl;
	continue;
      }
    }
}
