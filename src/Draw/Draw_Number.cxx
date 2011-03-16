// File:	Draw_Number.cxx
// Created:	Mon Aug 16 09:49:50 1993
// Author:	Bruno DUMORTIER
//		<dub@phylox>


#include <Draw_Number.ixx>

//=======================================================================
//function : Draw_Number
//purpose  : 
//=======================================================================

Draw_Number::Draw_Number(const Standard_Real V) :
       myValue(V)
{
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Real  Draw_Number::Value()const 
{
  return myValue;
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

void  Draw_Number::Value(const Standard_Real V)
{
  myValue = V;
}


//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void  Draw_Number::DrawOn(Draw_Display&)const 
{
}


//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Draw_Drawable3D)  Draw_Number::Copy()const 
{
  Handle(Draw_Number) D = new Draw_Number(myValue);
  return D;
}


//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void  Draw_Number::Dump(Standard_OStream& S)const 
{
  S << myValue;
}


//=======================================================================
//function : Whatis
//purpose  : 
//=======================================================================

void  Draw_Number::Whatis(Draw_Interpretor& S)const 
{
  S << "numeric";
}
