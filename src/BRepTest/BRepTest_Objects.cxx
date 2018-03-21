// Created on: 2018/03/21
// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2018 OPEN CASCADE SAS
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

#include <BRepTest_Objects.hxx>

//=======================================================================
//function : BRepTest_Session
//purpose  : Class for the objects in the session
//=======================================================================
class BRepTest_Session
{
public:

  //! Empty constructor
  BRepTest_Session() {}

  //! Sets the History in the session
  void SetHistory(const Handle(BRepTools_History)& theHistory)
  {
    myHistory = theHistory;
  }

  //! Add the History to the history in the session
  void AddHistory(const Handle(BRepTools_History)& theHistory)
  {
    if (myHistory.IsNull())
      myHistory = new BRepTools_History;
    myHistory->Merge(theHistory);
  }

  //! Returns the history from the session
  const Handle(BRepTools_History)& History() const
  {
    return myHistory;
  }

private:

  Handle(BRepTools_History) myHistory;
};

//=======================================================================
//function : GetSession
//purpose  : 
//=======================================================================
static BRepTest_Session& GetSession()
{
  static BRepTest_Session* pSession = new BRepTest_Session();
  return *pSession;
}

//=======================================================================
//function : SetHistory
//purpose  : 
//=======================================================================
void BRepTest_Objects::SetHistory(const Handle(BRepTools_History)& theHistory)
{
  GetSession().SetHistory(theHistory);
}

//=======================================================================
//function : AddHistory
//purpose  : 
//=======================================================================
void BRepTest_Objects::AddHistory(const Handle(BRepTools_History)& theHistory)
{
  GetSession().AddHistory(theHistory);
}

//=======================================================================
//function : History
//purpose  : 
//=======================================================================
Handle(BRepTools_History) BRepTest_Objects::History()
{
  return GetSession().History();
}
