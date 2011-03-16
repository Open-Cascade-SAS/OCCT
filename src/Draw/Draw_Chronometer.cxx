// File:	Draw_Chronometer.cxx
// Created:	Mon Aug 16 12:18:58 1993
// Author:	Bruno DUMORTIER
//		<dub@phylox>



#include <Draw_Chronometer.ixx>

//=======================================================================
//function : Draw_Chronometer
//purpose  : 
//=======================================================================

Draw_Chronometer::Draw_Chronometer() 
{
}


//=======================================================================
//function : Timer
//purpose  : 
//=======================================================================

OSD_Timer&  Draw_Chronometer::Timer()
{
  return myTimer;
}



//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void  Draw_Chronometer::DrawOn(Draw_Display&)const 
{
}


//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Draw_Drawable3D)  Draw_Chronometer::Copy()const 
{
  Handle(Draw_Chronometer) C = new Draw_Chronometer();
  return C;
}


//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void  Draw_Chronometer::Dump(Standard_OStream& S)const 
{
  S << "Chronometer : ";
}


//=======================================================================
//function : Whatis
//purpose  : 
//=======================================================================

void  Draw_Chronometer::Whatis(Draw_Interpretor& i)const 
{
  i << "chronometer";
}


