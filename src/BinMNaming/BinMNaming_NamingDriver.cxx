// Created on: 2004-05-13
// Created by: Sergey ZARITCHNY
// Copyright (c) 2004-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <BinMDF_ADriver.hxx>
#include <BinMNaming.hxx>
#include <BinMNaming_NamingDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <BinObjMgt_RRelocationTable.hxx>
#include <BinObjMgt_SRelocationTable.hxx>
#include <CDM_MessageDriver.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Tool.hxx>
#include <TNaming_Iterator.hxx>
#include <TNaming_ListIteratorOfListOfNamedShape.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_NameType.hxx>
#include <TNaming_Naming.hxx>
#include <TopAbs_ShapeEnum.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMNaming_NamingDriver,BinMDF_ADriver)

#define  NULL_ENTRY "0:0"
#define  OBSOLETE_NUM (int)sizeof(Standard_Integer)

//=======================================================================
// 'Z' - is reserved for: forfidden to use
//=======================================================================
static Standard_Character NameTypeToChar(const TNaming_NameType theNameType)
{
  switch(theNameType) {
    case TNaming_UNKNOWN      : return 'N';
    case TNaming_IDENTITY     : return 'I';
    case TNaming_MODIFUNTIL   : return 'M';
    case TNaming_GENERATION   : return 'G';
    case TNaming_INTERSECTION : return 'S';
    case TNaming_UNION        : return 'U';
    case TNaming_SUBSTRACTION : return 'B';
    case TNaming_CONSTSHAPE   : return 'C';
    case TNaming_FILTERBYNEIGHBOURGS : return 'F';
    case TNaming_ORIENTATION  : return 'O'; 
    case TNaming_WIREIN       : return 'W';
    case TNaming_SHELLIN      : return 'H';
  default:
    throw Standard_DomainError("TNaming_NameType:: Name Type Unknown");
  }
}

//=======================================================================
static TNaming_NameType CharTypeToName(const Standard_Character theCharType)
{
  switch(theCharType) {
    case 'N'  : return TNaming_UNKNOWN;
    case 'I'  : return TNaming_IDENTITY;
    case 'M'  : return TNaming_MODIFUNTIL;
    case 'G'  : return TNaming_GENERATION;
    case 'S'  : return TNaming_INTERSECTION;
    case 'U'  : return TNaming_UNION;
    case 'B'  : return TNaming_SUBSTRACTION;
    case 'C'  : return TNaming_CONSTSHAPE;
    case 'F'  : return TNaming_FILTERBYNEIGHBOURGS;
    case 'O'  : return TNaming_ORIENTATION;
    case 'W'  : return TNaming_WIREIN;
    case 'H'  : return TNaming_SHELLIN;
  default:
    throw Standard_DomainError("TNaming_NameType:: Name Type Unknown");
  }
}

//=======================================================================
static Standard_Character ShapeTypeToChar(const TopAbs_ShapeEnum theShapeType)
{
  switch (theShapeType)
    {
    case TopAbs_COMPOUND  : return 'C';
    case TopAbs_COMPSOLID : return 'O';
    case TopAbs_SOLID     : return 'S';
    case TopAbs_SHELL     : return 'H';
    case TopAbs_FACE      : return 'F';
    case TopAbs_WIRE      : return 'W';
    case TopAbs_EDGE      : return 'E';
    case TopAbs_VERTEX    : return 'V';
    case TopAbs_SHAPE     : return 'A';
    }
  return 'A'; // To avoid compilation error message.
}
//=======================================================================
static TopAbs_ShapeEnum CharToShapeType(const Standard_Character theCharType)
{
  switch (theCharType)
    {
    case 'C' : return TopAbs_COMPOUND;
    case 'O' : return TopAbs_COMPSOLID;
    case 'S' : return TopAbs_SOLID;
    case 'H' : return TopAbs_SHELL;
    case 'F' : return TopAbs_FACE;
    case 'W' : return TopAbs_WIRE;
    case 'E' : return TopAbs_EDGE;
    case 'V' : return TopAbs_VERTEX;
    case 'A' : return TopAbs_SHAPE;
    }
  return TopAbs_SHAPE; // To avoid compilation error message.
}
//=======================================================================
//function : BinMNaming_NamingDriver
//purpose  : Constructor
//=======================================================================

BinMNaming_NamingDriver::BinMNaming_NamingDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
     : BinMDF_ADriver (theMsgDriver, STANDARD_TYPE(TNaming_Naming)->Name())
{
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) BinMNaming_NamingDriver::NewEmpty() const
{
  return new TNaming_Naming();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================

Standard_Boolean BinMNaming_NamingDriver::Paste
                                (const BinObjMgt_Persistent&  theSource,
                                 const Handle(TDF_Attribute)& theTarget,
                                 BinObjMgt_RRelocationTable&  theRelocTable) const
{
  Handle(TNaming_Naming) anAtt = Handle(TNaming_Naming)::DownCast(theTarget);
  if(anAtt.IsNull()) return Standard_False;

  TNaming_Name& aName = anAtt->ChangeName();
  TCollection_ExtendedString aMsg;
//1. NameType
  Standard_Character aValue;
  Standard_Boolean ok = theSource >> aValue;
  Standard_Boolean aNewF = Standard_False;
  if (ok) {
    if(aValue == 'Z') {      // new format
      aNewF = Standard_True;
      ok = theSource >> aValue; //skip the sign & get NameType
      if(!ok) return ok;
    }

    aName.Type(CharTypeToName(aValue));

//2. ShapeType    
    ok = theSource >> aValue;
    if (ok) {
      aName.ShapeType(CharToShapeType(aValue));
      
//3. Args
      Standard_Integer aNbArgs=0;
      Standard_Integer anIndx;
      Handle(TNaming_NamedShape) aNS;
      ok = theSource >> aNbArgs;
      if (ok) {
        if(aNbArgs > 0) {
          Standard_Integer i;
          // read array
          for(i=1; i<=aNbArgs;i++) {
            if(!aNewF && i > OBSOLETE_NUM) break;//interrupt reading as old format can have only 4 items
            ok = theSource >> anIndx;
            if (!ok)
              break;
            else {
              if (theRelocTable.IsBound(anIndx))
                aNS = Handle(TNaming_NamedShape)::DownCast(theRelocTable.Find(anIndx));
              else {
                aNS = new TNaming_NamedShape;
                theRelocTable.Bind(anIndx, aNS);
              }
              aName.Append(aNS);
            }
          }
          //patch to release the rest of items	
          if(!aNewF && aNbArgs < OBSOLETE_NUM) {    
            for(i = aNbArgs+1;i <= OBSOLETE_NUM;i++)
              theSource >> anIndx;
          }
        }
//4. StopNS
        ok = theSource >> anIndx;
        if(ok) {
          if(anIndx > 0) {
            if (theRelocTable.IsBound(anIndx))
              aNS = Handle(TNaming_NamedShape)::DownCast(theRelocTable.Find(anIndx));
            else
            {
              aNS = new TNaming_NamedShape;
              theRelocTable.Bind(anIndx, aNS);
            }
            aName.StopNamedShape(aNS);  
          }

//5. Index
          ok = theSource >> anIndx;
          if(ok) 
            aName.Index(anIndx);
          else {
            aMsg = TCollection_ExtendedString("BinMNaming_NamingDriver: "
                                              "Cannot retrieve Index of Name");
            WriteMessage (aMsg); 
          }
        } else {
          aMsg = TCollection_ExtendedString("BinMNaming_NamingDriver: "
                                            "Cannot retrieve reference on "
                                            "StopNamedShape");
          WriteMessage (aMsg); 
        }
      } else {
        aMsg = TCollection_ExtendedString("BinMNaming_NamingDriver: "
                                          "Cannot retrieve reference on "
                                          "Arguments of Name");
	WriteMessage (aMsg);
	  }

    if(BinMNaming::DocumentVersion() > 3) {
	TCollection_AsciiString entry;
	ok = theSource >> entry;
	if(ok) {
#ifdef OCCT_DEBUG
	  cout << "NamingDriver:: Retrieved Context Label = " << entry << " Ok = " << theSource.IsOK()  <<endl;
#endif
	 
//6. context label
	  if(!entry.IsEmpty() && !entry.IsEqual(TCollection_AsciiString(NULL_ENTRY))) 
	    {
	      TDF_Label tLab; // Null label.
	      TDF_Tool::Label(anAtt->Label().Data(),entry, tLab, Standard_True);
	      if (!tLab.IsNull()) 
		aName.ContextLabel(tLab);
	    }
	}
    if(BinMNaming::DocumentVersion() > 4 && BinMNaming::DocumentVersion() < 7) {
          // Orientation processing - converting from old format
      Handle(TNaming_NamedShape) aNShape;
      if(anAtt->Label().FindAttribute(TNaming_NamedShape::GetID(), aNShape)) {
            //const TDF_Label& aLab = aNS->Label();
        TNaming_Iterator itL (aNShape);
        for (; itL.More(); itL.Next()) {
          const TopoDS_Shape& S = itL.NewShape();
          if (S.IsNull()) continue;
          if(aNShape->Evolution() == TNaming_SELECTED) {
            if (itL.More() && itL.NewShape().ShapeType() != TopAbs_VERTEX &&
                  !itL.OldShape().IsNull() && itL.OldShape().ShapeType() == TopAbs_VERTEX ) {//OR-N
              TopAbs_Orientation OrientationToApply = itL.OldShape().Orientation();
              aName.Orientation(OrientationToApply);
			}
		  }
		}
	  }
	}
    if(BinMNaming::DocumentVersion() > 6) {
      ok = theSource >> anIndx;
      TopAbs_Orientation OrientationToApply(TopAbs_FORWARD);
      if(ok) {
        OrientationToApply = (TopAbs_Orientation)anIndx;
		aName.Orientation(OrientationToApply);
#ifdef OCCT_DEBUG
	    cout << "NamingDriver:: Retrieved Orientation = " << OrientationToApply << " Ok = " << theSource.IsOK()  <<endl;
#endif
	  } else {
          aMsg = TCollection_ExtendedString("BinMNaming_NamingDriver: "
                                            "Cannot retrieve Name Orientation ");
	  WriteMessage (aMsg);
	  }
	}
	}
#ifdef OCCT_DEBUG
      else if(BinMNaming::DocumentVersion() == -1)
	cout << "Current DocVersion field is not initialized. "  <<endl;
      else 
	cout << "Current DocVersion = " << BinMNaming::DocumentVersion() <<endl;
#endif
	}
  }
  return ok;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================

void BinMNaming_NamingDriver::Paste (const Handle(TDF_Attribute)&  theSource,
                                       BinObjMgt_Persistent&       theTarget,
                                       BinObjMgt_SRelocationTable& theRelocTable) const
{
  Handle(TNaming_Naming) anAtt = Handle(TNaming_Naming)::DownCast(theSource);
  const TNaming_Name& aName = anAtt->GetName();

//0. add the sign of new format (to fix misprint with Array size)
  theTarget.PutCharacter('Z');

//1. << NameType to Char
  theTarget << NameTypeToChar(aName.Type());

//2. << ShapeType to Char
  theTarget << ShapeTypeToChar(aName.ShapeType());

//3. Keep Args
  Standard_Integer anIndx;
  Standard_Integer aNbArgs = aName.Arguments().Extent();
  theTarget << aNbArgs; // keep Number
  if (aNbArgs > 0) {
    Standard_Integer i=0;
    TColStd_Array1OfInteger anArray(1, aNbArgs);
    //fill array
    for (TNaming_ListIteratorOfListOfNamedShape it (aName.Arguments()); it.More(); it.Next()) {
      Handle(TNaming_NamedShape) anArg = it.Value();
      anIndx = 0; i++;
      if (!anArg.IsNull()) {
        anIndx = theRelocTable.FindIndex(anArg);
        if (anIndx == 0)
          anIndx = theRelocTable.Add(anArg);
      }
      anArray.SetValue(i, anIndx);
    }
    
    theTarget.PutIntArray ((BinObjMgt_PInteger) &anArray.Value(1), aNbArgs); // keep Array
  }

//4. keep StopNS
  Handle(TNaming_NamedShape) aStopNS = aName.StopNamedShape();
  if (!aStopNS.IsNull()) {
    anIndx = theRelocTable.FindIndex(aStopNS);
    if (anIndx == 0)
      anIndx = theRelocTable.Add(aStopNS);
  } else 
    anIndx = 0;
  theTarget << anIndx;

//5. keep Index
  theTarget << aName.Index();

//6. keep context label
  TCollection_AsciiString entry(NULL_ENTRY);
  if(!aName.ContextLabel().IsNull())
    TDF_Tool::Entry(aName.ContextLabel(), entry);
  theTarget << entry;

//7. keep Orientation
  theTarget << (Standard_Integer)aName.Orientation();

}
