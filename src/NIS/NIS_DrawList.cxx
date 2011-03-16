// File:      NIS_DrawList.cxx
// Created:   09.07.07 21:46
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2007

#include <NIS_Drawer.hxx>

#ifdef WNT
#include <windows.h>
#endif
#include <GL/gl.h>

//=======================================================================
//function : NIS_DrawList()
//purpose  : Empty Constructor
//=======================================================================

NIS_DrawList::NIS_DrawList ()
  : myListID    (0)
{
  myIsUpdated[0] = Standard_True;
  myIsUpdated[1] = Standard_True;
  myIsUpdated[2] = Standard_True;
  myIsUpdated[3] = Standard_True;
}

//=======================================================================
//function : NIS_DrawList()
//purpose  : Constructor
//=======================================================================

NIS_DrawList::NIS_DrawList (const Handle_NIS_View& theView)
  : myView      (theView),
    myListID    (0)
{
  myIsUpdated[0] = Standard_True;
  myIsUpdated[1] = Standard_True;
  myIsUpdated[2] = Standard_True;
  myIsUpdated[3] = Standard_True;
}

//=======================================================================
//function : ~NIS_DrawList
//purpose  : 
//=======================================================================

NIS_DrawList::~NIS_DrawList ()
{
  if (myListID != 0)
    glDeleteLists (myListID, 4);
}

//=======================================================================
//function : BeginPrepare
//purpose  : 
//=======================================================================

void NIS_DrawList::BeginPrepare (const Standard_Integer theType)
{
  if (myListID == 0)
    myListID = glGenLists(4);
  glNewList (GetListID (theType), GL_COMPILE);
}

//=======================================================================
//function : EndPrepare
//purpose  : 
//=======================================================================

void NIS_DrawList::EndPrepare (const Standard_Integer theType)
{
  glEndList ();
  myIsUpdated[theType&0x3] = Standard_False;
}

//=======================================================================
//function : Call
//purpose  : 
//=======================================================================

void NIS_DrawList::Call (const Standard_Integer theType)
{
  glCallList (GetListID (theType));
}

//=======================================================================
//function : SetDynHilighted
//purpose  : 
//=======================================================================

Standard_Boolean NIS_DrawList::SetDynHilighted
                                (const Standard_Boolean              isHili,
                                 const Handle_NIS_InteractiveObject& theObj)
{
  Standard_Boolean aResult (Standard_False);
  NCollection_List<Handle_NIS_InteractiveObject>::Iterator anIt(myDynHilighted);
  for (; anIt.More(); anIt.Next())
    if (anIt.Value() == theObj)
      break;
  if (anIt.More()) {
    if (isHili == Standard_False) {
      myIsUpdated[NIS_Drawer::Draw_DynHilighted] = Standard_True;
      myDynHilighted.Remove (anIt);
      aResult = Standard_True;
    }
  } else
    if (isHili == Standard_True) {
      myIsUpdated[NIS_Drawer::Draw_DynHilighted] = Standard_True;
      myDynHilighted.Append (theObj);
      aResult = Standard_True;
    }
  return aResult;
}

//=======================================================================
//function : SetUpdated
//purpose  : 
//=======================================================================

void NIS_DrawList::SetUpdated (const Standard_Integer theType,
                               const Standard_Boolean theFlag)
{ 
  if ( theFlag )
    SetUpdated( theType );
  else
    myIsUpdated [theType&0x3] = Standard_False;
}

