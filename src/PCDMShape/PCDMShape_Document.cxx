// File:	PCDMShape_Document.cxx
// Created:	Thu Jan  8 15:59:23 1998
// Author:	Isabelle GRIGNON
//		<isg@bigbox.paris1.matra-dtv.fr>


#include <PCDMShape_Document.ixx>


//=======================================================================
//function : PCDMShape_Document
//purpose  : 
//=======================================================================

PCDMShape_Document::PCDMShape_Document()
{}


//=======================================================================
//function : PCDMShape_Document
//purpose  : 
//=======================================================================

PCDMShape_Document::PCDMShape_Document(const PTopoDS_Shape1& T)
: myShape(T)
{}


//=======================================================================
//function : TShape
//purpose  : 
//=======================================================================

const PTopoDS_Shape1 PCDMShape_Document::Shape()const 
{ return myShape; }


//=======================================================================
//function : TShape
//purpose  : 
//=======================================================================

void  PCDMShape_Document::Shape(const PTopoDS_Shape1& T)
{ myShape = T; }
