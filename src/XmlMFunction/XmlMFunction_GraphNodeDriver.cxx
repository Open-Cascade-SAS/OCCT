// Created on: 2008-03-07
// Created by: Vlad ROMASHKO
// Copyright (c) 2008-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <XmlMFunction_GraphNodeDriver.ixx>
#include <XmlObjMgt.hxx>

#include <TFunction_GraphNode.hxx>
#include <TColStd_MapIteratorOfMapOfInteger.hxx>

IMPLEMENT_DOMSTRING (LastPreviousIndex, "lastprev")
IMPLEMENT_DOMSTRING (LastNextIndex,     "lastnext")
IMPLEMENT_DOMSTRING (ExecutionStatus,   "exec")

//=======================================================================
//function : XmlMFunction_GraphNodeDriver
//purpose  : Constructor
//=======================================================================
XmlMFunction_GraphNodeDriver::XmlMFunction_GraphNodeDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, NULL)
{

}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMFunction_GraphNodeDriver::NewEmpty() const
{
  return (new TFunction_GraphNode());
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMFunction_GraphNodeDriver::Paste(const XmlObjMgt_Persistent&  theSource,
						     const Handle(TDF_Attribute)& theTarget,
						     XmlObjMgt_RRelocationTable&  ) const
{
  Handle(TFunction_GraphNode) G = Handle(TFunction_GraphNode)::DownCast(theTarget);

  Standard_Integer aFirstIndPrev, aLastIndPrev, aFirstIndNext, aLastIndNext, aValue, ind;
  const XmlObjMgt_Element& anElement = theSource;

  // Previous
  // ========

  // Read the FirstIndex; if the attribute is absent initialize to 1
  aFirstIndPrev = 1; // It is absent :-) because I didn't wrote it on the stage of writing the file.

  // Read the LastIndex; the attribute should present
  if (!anElement.getAttribute(::LastPreviousIndex()).GetInteger(aLastIndPrev)) 
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the last index"
                                 " for previous functions of GraphNode attribute");
    WriteMessage (aMessageString);
    return Standard_False;
  }

  if (aFirstIndPrev == aLastIndPrev) 
  {
    Standard_Integer anInteger;
    if (!XmlObjMgt::GetStringValue(anElement).GetInteger(anInteger)) 
    {
      TCollection_ExtendedString aMessageString =
        TCollection_ExtendedString("Cannot retrieve integer member"
                                   " for previous functions of GraphNode attribute");
      WriteMessage (aMessageString);
      return Standard_False;
    }
    G->AddPrevious(anInteger);
  }
  else 
  {
    Standard_CString aValueStr =
      Standard_CString(XmlObjMgt::GetStringValue(anElement).GetString());
    
    for (ind = aFirstIndPrev; ind <= aLastIndPrev; ind++)
    {
      if (!XmlObjMgt::GetInteger(aValueStr, aValue)) 
      {
        TCollection_ExtendedString aMessageString =
          TCollection_ExtendedString("Cannot retrieve integer member"
                                     " for previous functions of GraphNode attribute as \"")
            + aValueStr + "\"";
        WriteMessage (aMessageString);
        return Standard_False;
      }
      G->AddPrevious(aValue);
    }
  }


  // Next
  // ====

  // Read the FirstIndex; if the attribute is absent initialize to 1
  aFirstIndNext = aLastIndPrev + 1; // It is absent :-) because I didn't wrote it on the stage of writing the file.

  // Read the LastIndex; the attribute should present
  if (!anElement.getAttribute(::LastNextIndex()).GetInteger(aLastIndNext))
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the last index"
                                 " for next functions of GraphNode attribute");
    WriteMessage (aMessageString);
    return Standard_False;
  }
  aLastIndNext += aLastIndPrev;

  Standard_CString aValueStr =
    Standard_CString(XmlObjMgt::GetStringValue(anElement).GetString());
    
  for (ind = 1; ind <= aLastIndNext; ind++)
  {
    if (!XmlObjMgt::GetInteger(aValueStr, aValue)) 
    {
      TCollection_ExtendedString aMessageString =
        TCollection_ExtendedString("Cannot retrieve integer member"
                                   " for next functions of GraphNode attribute as \"")
          + aValueStr + "\"";
      WriteMessage (aMessageString);
      return Standard_False;
    }
    if (ind < aFirstIndNext)
      continue;
    G->AddNext(aValue);
  }

  // Execution status
  Standard_Integer exec = 0;
  if (!anElement.getAttribute(::ExecutionStatus()).GetInteger(exec)) 
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the execution status"
                                 " for GraphNode attribute");
    WriteMessage (aMessageString);
    return Standard_False;
  }
  G->SetStatus((TFunction_ExecutionStatus) exec);

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void XmlMFunction_GraphNodeDriver::Paste (const Handle(TDF_Attribute)& theSource,
					  XmlObjMgt_Persistent&        theTarget,
					  XmlObjMgt_SRelocationTable&  ) const
{
  Handle(TFunction_GraphNode) G = Handle(TFunction_GraphNode)::DownCast(theSource);

  // Previous
  // ========

  theTarget.Element().setAttribute(::LastPreviousIndex(), G->GetPrevious().Extent());

  TCollection_AsciiString aValueStr;
  TColStd_MapIteratorOfMapOfInteger itrm(G->GetPrevious());
  for (; itrm.More(); itrm.Next())
  {
    const Standard_Integer ID = itrm.Key();
    aValueStr += TCollection_AsciiString(ID);
    aValueStr += ' ';
  }
  aValueStr += "\n";

  // Next
  // ====

  theTarget.Element().setAttribute(::LastNextIndex(), G->GetNext().Extent());

  itrm.Initialize(G->GetNext());
  for (; itrm.More(); itrm.Next())
  {
    const Standard_Integer ID = itrm.Key();
    aValueStr += TCollection_AsciiString(ID);
    aValueStr += ' ';
  }

  XmlObjMgt::SetStringValue (theTarget, aValueStr.ToCString(), Standard_True);

  // Execution status
  theTarget.Element().setAttribute(::ExecutionStatus(), (Standard_Integer) G->GetStatus());
}
