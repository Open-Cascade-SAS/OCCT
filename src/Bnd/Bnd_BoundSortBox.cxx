// Created on: 1992-11-24
// Created by: Didier PIFFAULT
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <Bnd_Array1OfBox.hxx>
#include <Bnd_BoundSortBox.hxx>
#include <Bnd_Box.hxx>
#include <gp_Pln.hxx>
#include <Standard_MultiplyDefined.hxx>
#include <Standard_NullValue.hxx>
#include <TColStd_DataMapIteratorOfDataMapOfIntegerInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>

#include <stdio.h>
//-- ================================================================================
//--  lbr le 27 fev 97
//--  
//-- 
//--      Initialisation:  Bounding box BE   
//--                       List of boxes Bi (Bi in BE)
//-- 
//--      Compare(b) returns the list of Boxes Bi touched by b
//-- 
//--      
//--      General principle: 
//-- 
//--       1) Discretize box BE into N*N*N voxels
//--          Each box Bi touches a certain number of voxels
//--          Bi touches { Vijk with i0<=i<=i1 ...  k0<=k<=k1 } 
//--       2) Project on each axis X,Y,Z boxes Bi
//--          to get the following structures : 
//-- 
//--          Example :  
//--
//--             box i1 touches voxels { Vijk  with 1<=i<=N-1  ... }
//--             box i2 touches voxels { Vijk  with 2<=i<=3  ... }
//--             box i3 touches voxels { Vijk  with 1<=i<=2  ... }
//--
//-- 
//--         X[.] |  1    2    3    4   ....   N-1   N     
//--         -----|-------------------------------------
//--              |  i3   i1   i1   i1          i1
//--              |       i2   i2
//--              |       i3 
//-- 
//-- 
//--         Produce the same thing for axes Y and Z 
//--         
//--         Obtain 3 tables (X,Y,Z) of lists of integers (indexes of boxes)
//-- 
//--       3) To find boxes contacting with box bt
//--             a) Find voxels touched by bt -> i0bt,i1bt ... k0bt,k1bt
//--             b) Find in list Z the boxes present in cases Z[k0bt ... k1bt]
//--             c) Find among these boxes the ones present in cases Y[j0bt ... j1bt]
//--             d) and the result is the intersection of the previous result with X[i0bt ... i1bt]
//-- 
//--
//--  Rejection of a higher level. 
//-- 
//--       *) Preserve a table representation of bit of voxels of space BE 
//--          that contains at least one box Bi.
//--       *) While box bt is texted, it is checked if this box includes in 
//--          the table of bit at least one occupied voxel. 
//--          If the occupied voxel is touched : no rejection
//--          Otherwise return
//--
//--      **) Another rejection was adopted. It consists in trying to locate in 
//--          the above structures (tables X,Y,Z and  table of Bits) a box Bi which is greater than the 
//--          bounding box BE. 
//--
//--          The indices of these boxes are located in table ToTest, and these 
//            boxes are compared systematically with bt. 
//--         
//--   Note : tables C replace here HArray1OfListOfInteger and other      
//--          structures that are sufficient for data adding but slow for reading.
//--          
//--          Here the data is added at start (Initialize and SortBoxes) and later,
//--          it takes much time to parse the tables. The slowly written, byut fastly read structures 
//--          are thus better.
//--         
//=======================================================================
#define VERIFICATION 0
#define DEBUG 0
#define DIMAXIS 20

#if DEBUG
static long unsigned   APPELREJECTION=0L;
static long unsigned   REJECTNIV0=0L;
static long unsigned   REJECTNIV1=0L;
static long unsigned   NBCOMPARE=0L;
static long unsigned   NBBOITES=0L;
static long unsigned   NBBOITESATESTER=0L;
#endif
//=======================================================================
static Standard_Integer  ComputeSize(const Standard_Integer n) { 
  if(n>40000) return(128);
  if(n>10000)  return(64);
  if(n>1000)   return(32);
  if(n>100)    return(16);
  return(8);
}
//=======================================================================
static long unsigned _P2[32] = { 1,2,4,8,  16,32,64,128,  256,512,1024,2048,
				 4096,8192,16384,32768,
				 65536,131072,262144,524288,
				 1048576,2097152,4194304,8388608,
				 16777216,33554432,67108864,134217728,
				 268435456,536870912,1073741824,2147483648U};

//-- size is power of 2 > 4
class BSB_T3Bits
{
public:

  Standard_Integer _DECAL;
  Standard_Integer _DECAL2;
  Standard_Integer _BASE;
  Standard_Integer _BASEM1;
  
  long unsigned ind;
  long unsigned Isize;
  Standard_Integer ssize;
  Standard_Real Xmin,Xmax,Ymin,Ymax,Zmin,Zmax;
  
  long unsigned *p;
  Standard_Integer **axisX;
  Standard_Integer **axisY;
  Standard_Integer **axisZ;
  
  Standard_Integer *ToTest;

public:
  BSB_T3Bits(int size);
  ~BSB_T3Bits();

  //-- Part HArray1OfListOfInteger

  void AppendAxisX(const Standard_Integer i,const Standard_Integer v);
  void AppendAxisY(const Standard_Integer i,const Standard_Integer v);
  void AppendAxisZ(const Standard_Integer i,const Standard_Integer v);

  void Add(long unsigned t) { int o=t&31;    int k=t>>5;    p[k]|=_P2[o];          }
  int Val(long unsigned t)  { int o=t&31;    int k=t>>5;    return(p[k]&_P2[o]);   }
  void Raz(long unsigned t) { int o=t&31;    int k=t>>5;    p[k]&= ~(_P2[o]);      }
  
  Standard_Integer NbAxisX(const Standard_Integer i) {   return(axisX[0][i]);   }
  Standard_Integer NbAxisY(const Standard_Integer i) {   return(axisY[0][i]);   }
  Standard_Integer NbAxisZ(const Standard_Integer i) {   return(axisZ[0][i]);   }
  
  inline Standard_Integer GrilleInteger(Standard_Integer ix,
                                        Standard_Integer iy,
                                        Standard_Integer iz)
  {
    Standard_Integer tz = iz<<_DECAL2;
    Standard_Integer ty = iy<<_DECAL;
    Standard_Integer t  = ix;
    t|=ty;
    t|=tz;
    return(t);
  }

  inline void IntegerGrille(Standard_Integer t,
                            Standard_Integer &ix,
                            Standard_Integer &iy,
                            Standard_Integer &iz)
  {
    ix = t & _BASEM1; t>>=_DECAL;
    iy = t & _BASEM1; t>>=_DECAL;
    iz = t;
  }

private:

  BSB_T3Bits (const BSB_T3Bits&);
  BSB_T3Bits& operator= (const BSB_T3Bits&);
};

//=======================================================================
BSB_T3Bits::~BSB_T3Bits()
{
  if(p) { delete [] p; p=0; } 
#if DEBUG
  printf("\n BASE:%d\n",_BASE);
#endif
  for(Standard_Integer i=0; i<=ssize; i++) { 
    if(axisX[i]) { delete [] axisX[i]; axisX[i]=0; } 
    if(axisY[i]) { delete [] axisY[i]; axisY[i]=0; }
    if(axisZ[i]) { delete [] axisZ[i]; axisZ[i]=0; }
  }
  free(axisX); axisX=0;
  free(axisY); axisY=0;
  free(axisZ); axisZ=0;
  if(ToTest)  { delete [] ToTest; ToTest=0; } 
}
//=======================================================================
BSB_T3Bits::BSB_T3Bits(int size)
	: ind(0),
    Xmin(0),Xmax(0),
	Ymin(0),Ymax(0),
	Zmin(0),Zmax(0)
{
  switch (size) { 
  case 128: {  _DECAL=7;   _DECAL2=14;   _BASE=128;  _BASEM1=127;  break; } 
  case  64: {  _DECAL=6;   _DECAL2=12;   _BASE= 64;  _BASEM1= 63;  break; } 
  case  32: {  _DECAL=5;   _DECAL2=10;   _BASE= 32;  _BASEM1= 31;  break; } 
  case  16: {  _DECAL=4;   _DECAL2= 8;   _BASE= 16;  _BASEM1= 15;  break; } 
  default : {  _DECAL=3;   _DECAL2= 6;   _BASE=  8;  _BASEM1=  7;  break; } 
  }
  Standard_Integer i ;    
  long unsigned nb = (size*size*size)>>5;
  Isize = nb;
  ssize = size;
  p = new long unsigned [nb];
  do { p[--nb]=0; } while(nb);

  axisX = (Standard_Integer **) malloc((size+1)*sizeof(Standard_Integer *));
  axisY = (Standard_Integer **) malloc((size+1)*sizeof(Standard_Integer *));
  axisZ = (Standard_Integer **) malloc((size+1)*sizeof(Standard_Integer *));

  axisX[0]=new Standard_Integer [_BASE+1];
  axisY[0]=new Standard_Integer [_BASE+1];
  axisZ[0]=new Standard_Integer [_BASE+1];

  for( i=0; i<(_BASE+1); i++) {
    axisX[0][i]=0;
    axisY[0][i]=0;
    axisZ[0][i]=0;
  }

  for(i=1; i<=size; i++) {  
    axisX[i] =  new Standard_Integer[DIMAXIS];
    axisY[i] =  new Standard_Integer[DIMAXIS];
    axisZ[i] =  new Standard_Integer[DIMAXIS];
    axisX[i][0]=DIMAXIS;
    axisY[i][0]=DIMAXIS;
    axisZ[i][0]=DIMAXIS;
    axisX[i][1]=axisY[i][1]=axisZ[i][1]=-1;
  }
  ToTest=0;
}
//=======================================================================
void BSB_T3Bits::AppendAxisZ(const Standard_Integer i,
			     const Standard_Integer v) { 
  Standard_Integer n = axisZ[0][i];
  n++;
  if(n<axisZ[i][0]) { axisZ[i][n]=v; }
  else { 
    Standard_Integer s=axisZ[i][0];
    Standard_Integer *nt = new Standard_Integer [s+s];
    nt[0]=s+s;
    for(Standard_Integer j=1;j<s;j++) { 
      nt[j]=axisZ[i][j];
    }
    nt[n]=v;
    delete [] axisZ[i];
    axisZ[i]=nt;
  }
  axisZ[0][i]=n;
}
//=======================================================================
void BSB_T3Bits::AppendAxisY(const Standard_Integer i,
			     const Standard_Integer v) { 
  Standard_Integer n = axisY[0][i];
  n++;
  if(n<axisY[i][0]) { axisY[i][n]=v;   }
  else { 
    Standard_Integer s=axisY[i][0];
    Standard_Integer *nt = new Standard_Integer [s+s];
    nt[0]=s+s;
    for(Standard_Integer j=1;j<s;j++) { 
      nt[j]=axisY[i][j];
    }
    nt[n]=v;
    delete [] axisY[i];
    axisY[i]=nt;
  }
  axisY[0][i]=n;
}
//=======================================================================
void BSB_T3Bits::AppendAxisX(const Standard_Integer i,
			     const Standard_Integer v) { 
  Standard_Integer n = axisX[0][i];

  n++;
  if(n<axisX[i][0]) {   axisX[i][n]=v;   }
  else { 
    //-- it is required to extend
    Standard_Integer s=axisX[i][0];
    Standard_Integer *nt = new Standard_Integer [s+s];
    nt[0]=s+s;
    for(Standard_Integer j=1;j<s;j++) { 
      nt[j]=axisX[i][j];
    }
    nt[n]=v;
    delete [] axisX[i];
    axisX[i]=nt;
  }
  axisX[0][i]=n;
}
//=======================================================================
//=======================================================================
//function : Bnd_BoundSortBox
//purpose  : 
//=======================================================================
Bnd_BoundSortBox::Bnd_BoundSortBox()
     : discrX(0), discrY(0), discrZ(0)
{
  TabBits=0;
#if DEBUG
  NBCOMPARE=0L;   NBBOITES=0L;   NBBOITESATESTER=0L;
  APPELREJECTION=0L;  REJECTNIV0=0L;  REJECTNIV1=0L;
#endif
}
//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void Bnd_BoundSortBox::Initialize(const Bnd_Box& CompleteBox,
				  const Handle(Bnd_HArray1OfBox)& SetOfBox)
{
  myBox=CompleteBox;
  myBndComponents=SetOfBox;
  const Bnd_Array1OfBox & taBox=myBndComponents->Array1();
  discrX=discrY=discrZ=ComputeSize(taBox.Upper()-taBox.Lower());
  Standard_Real Xmax, Ymax, Zmax;
  if(CompleteBox.IsVoid()) 
    return;
  CompleteBox.Get(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);
  deltaX = (Xmax-Xmin == 0. ? 0. : discrX/(Xmax-Xmin));
  deltaY = (Ymax-Ymin == 0. ? 0. : discrY/(Ymax-Ymin));
  deltaZ = (Zmax-Zmin == 0. ? 0. : discrZ/(Zmax-Zmin));
  SortBoxes();
}
//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================
void Bnd_BoundSortBox::Initialize(const Handle(Bnd_HArray1OfBox)& SetOfBox)
{
  myBndComponents=SetOfBox;
  const Bnd_Array1OfBox & taBox=myBndComponents->Array1();
  Standard_Integer i0,i1;
  i0=taBox.Lower();
  i1=taBox.Upper();
  discrX=discrY=discrZ=ComputeSize(i1-i0);
  Standard_Integer labox;
  for (labox=i0; labox<=i1; labox++) {
    if (!taBox(labox).IsVoid()) {
      myBox.Add(taBox(labox));
    }
  }
  Standard_Real Xmax, Ymax, Zmax;
  if(myBox.IsVoid()) 
    return;
  myBox.Get(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);
  deltaX = (Xmax-Xmin == 0. ? 0. : discrX/(Xmax-Xmin));
  deltaY = (Ymax-Ymin == 0. ? 0. : discrY/(Ymax-Ymin));
  deltaZ = (Zmax-Zmin == 0. ? 0. : discrZ/(Zmax-Zmin));
  SortBoxes();
}
//=======================================================================
//function : SortBoxes
//purpose  : 
//=======================================================================
void Bnd_BoundSortBox::SortBoxes() 
{
  Standard_Integer labox;
  Standard_Integer lacaseX, firstcaseX, lastcaseX;
  Standard_Integer lacaseY, firstcaseY, lastcaseY;
  Standard_Integer lacaseZ, firstcaseZ, lastcaseZ;
  Standard_Real xmin, ymin, zmin, xmax, ymax, zmax;
  const Bnd_Array1OfBox & taBox=myBndComponents->Array1();
  Standard_Integer i0=taBox.Lower();
  Standard_Integer i1=taBox.Upper();
  BSB_T3Bits* Map=0;
  if(TabBits) { 
    BSB_T3Bits* _Map = (BSB_T3Bits *)TabBits;
    delete _Map;
  }
  Map = new BSB_T3Bits(discrX);
  TabBits = (void *)Map;
  if(Map->ToTest==0) { 
    Standard_Integer s=i1-i0;
    if(s<2) s=2;
    Map->ToTest = new Standard_Integer [s];
    for(Standard_Integer i=0; i<s; i++) { 
      Map->ToTest[i]=i0-1;
    }
  }
  Standard_Real _Xmax,_Xmin,_Ymax,_Ymin,_Zmin,_Zmax;
  myBox.Get(_Xmin,_Ymin,_Zmin,_Xmax,_Ymax,_Zmax);
  Map->Xmax=_Xmax; Map->Ymax=_Ymax; Map->Zmax=_Zmax;
  Map->Xmin=_Xmin; Map->Ymin=_Ymin; Map->Zmin=_Zmin;
  for (labox=i0; labox<=i1; labox++) {
    if (!taBox(labox).IsVoid()) {
      taBox(labox).Get(xmin, ymin, zmin, xmax, ymax, zmax);
      if(xmin>Xmin) firstcaseX=(Standard_Integer )((xmin-Xmin)*deltaX)-1; else  firstcaseX=1;
      if(ymin>Ymin) firstcaseY=(Standard_Integer )((ymin-Ymin)*deltaY)-1; else  firstcaseY=1;
      if(zmin>Zmin) firstcaseZ=(Standard_Integer )((zmin-Zmin)*deltaZ)-1; else  firstcaseZ=1;
      if(xmax<_Xmax) lastcaseX=(Standard_Integer )((xmax-Xmin)*deltaX)+1; else  lastcaseX=discrX;
      if(ymax<_Ymax) lastcaseY=(Standard_Integer )((ymax-Ymin)*deltaY)+1; else  lastcaseY=discrY;
      if(zmax<_Zmax) lastcaseZ=(Standard_Integer )((zmax-Zmin)*deltaZ)+1; else  lastcaseZ=discrZ;
      if(firstcaseX<1) firstcaseX=1; else if(firstcaseX>discrX) firstcaseX=discrX;
      if(firstcaseY<1) firstcaseY=1; else if(firstcaseY>discrY) firstcaseY=discrY;
      if(firstcaseZ<1) firstcaseZ=1; else if(firstcaseZ>discrZ) firstcaseZ=discrZ;

      if(lastcaseX<1) lastcaseX=1; else if(lastcaseX>discrX) lastcaseX=discrX;
      if(lastcaseY<1) lastcaseY=1; else if(lastcaseY>discrY) lastcaseY=discrY;
      if(lastcaseZ<1) lastcaseZ=1; else if(lastcaseZ>discrZ) lastcaseZ=discrZ;

      Standard_Integer n = (lastcaseX-firstcaseX);
      if(n>(lastcaseY-firstcaseY)) n=lastcaseY-firstcaseY;
      if(n>(lastcaseZ-firstcaseZ)) n=lastcaseZ-firstcaseZ;
#if DEBUG
      NBBOITES++;
#endif
      n<<=2;
      if(n>discrX) { 
#if DEBUG
      NBBOITESATESTER++;
#endif
	for(Standard_Integer i=0; i<(i1-i0); i++) { 
	  if(Map->ToTest[i]<i0) { 
	    Map->ToTest[i]=labox;
	    break;
	  }
	}
      }
      else { 
	for (lacaseX=firstcaseX; lacaseX<=lastcaseX; lacaseX++) {
	  Map->AppendAxisX(lacaseX,labox);
	}
	for (lacaseY=firstcaseY; lacaseY<=lastcaseY; lacaseY++) {
	  Map->AppendAxisY(lacaseY,labox);
	}
	for (lacaseZ=firstcaseZ; lacaseZ<=lastcaseZ; lacaseZ++) {
	  Map->AppendAxisZ(lacaseZ,labox);
	}
	//------------------------------------------------------------
	//-- fill table with bits
	//--
	if(Map) { 
	  for (lacaseX=firstcaseX; lacaseX<=lastcaseX; lacaseX++) {
	    for (lacaseY=firstcaseY; lacaseY<=lastcaseY; lacaseY++) {
	      for (lacaseZ=firstcaseZ; lacaseZ<=lastcaseZ; lacaseZ++) {
		long unsigned t=Map->GrilleInteger(lacaseX-1,lacaseY-1,lacaseZ-1);
		Map->Add(t);
	      }
	    }
	  }
	}
      }     
    }
  }
}
//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================
void Bnd_BoundSortBox::Initialize(const Bnd_Box& CompleteBox,
				  const Standard_Integer nbComponents)
{
  Standard_NullValue_Raise_if (nbComponents <=0, "BoundSortBox nul!");
  myBox=CompleteBox;
  myBndComponents=new Bnd_HArray1OfBox(1,nbComponents);

  //***>>> JCD - 04.08.2000 - Array initialization is missing... 
  Bnd_Box emptyBox; 
  myBndComponents->Init( emptyBox ); 
  //***<<< JCD - End  

  discrX=discrY=discrZ=ComputeSize(nbComponents);
  Standard_Real Xmax, Ymax, Zmax;

  if(CompleteBox.IsVoid())
    return;
  CompleteBox.Get(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);
  myBox.Get(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);
  deltaX = (Xmax-Xmin == 0. ? 0. : discrX/(Xmax-Xmin));
  deltaY = (Ymax-Ymin == 0. ? 0. : discrY/(Ymax-Ymin));
  deltaZ = (Zmax-Zmin == 0. ? 0. : discrZ/(Zmax-Zmin));
  if(TabBits) { 
    BSB_T3Bits* _Map = (BSB_T3Bits *)TabBits;
    delete _Map;
    TabBits=0;
  }
  BSB_T3Bits* Map=0;
  Map = new BSB_T3Bits(discrX);
  TabBits = (void *)Map;
}
//=======================================================================
//function : Add
//purpose  : 
//=======================================================================
void Bnd_BoundSortBox::Add(const Bnd_Box& theBox, 
			   const Standard_Integer boxIndex)
{
  Standard_MultiplyDefined_Raise_if (!(myBndComponents->Value(boxIndex).IsVoid()), " This box is already defined !");
  if (!theBox.IsVoid()) {
    Standard_Integer i0=myBndComponents->Lower();
    Standard_Integer i1=myBndComponents->Upper();
    Standard_Integer theGapX, firstGapX , lastGapX;
    Standard_Integer theGapY, firstGapY , lastGapY;
    Standard_Integer theGapZ, firstGapZ , lastGapZ;
    Standard_Real xmin, ymin, zmin, xmax, ymax, zmax;
    myBndComponents->SetValue(boxIndex, theBox);
    theBox.Get(xmin, ymin, zmin, xmax, ymax, zmax);
    BSB_T3Bits* Map = (BSB_T3Bits *)TabBits;
    if(Map->ToTest==0) { 
      Standard_Integer s=i1-i0;
      if(s<2) s=2;
      Map->ToTest = new Standard_Integer [s];
      for(Standard_Integer i=0; i<s; i++) { 
	Map->ToTest[i]=i0-1;
      }
    }
    Standard_Real _Xmax,_Ymax,_Zmax;
    _Xmax=Map->Xmax; _Ymax=Map->Ymax; _Zmax=Map->Zmax;
    if(xmin>Xmin) firstGapX=(Standard_Integer )((xmin-Xmin)*deltaX)-1; else  firstGapX=1;
    if(ymin>Ymin) firstGapY=(Standard_Integer )((ymin-Ymin)*deltaY)-1; else  firstGapY=1;
    if(zmin>Zmin) firstGapZ=(Standard_Integer ) ((zmin-Zmin)*deltaZ)-1; else  firstGapZ=1;
    if(xmax<_Xmax) lastGapX=(Standard_Integer )((xmax-Xmin)*deltaX)+1; else  lastGapX=discrX;
    if(ymax<_Ymax) lastGapY=(Standard_Integer )((ymax-Ymin)*deltaY)+1; else  lastGapY=discrY;
    if(zmax<_Zmax) lastGapZ=(Standard_Integer )((zmax-Zmin)*deltaZ)+1; else  lastGapZ=discrZ;
    if(firstGapX<1) firstGapX=1; else if(firstGapX>discrX) firstGapX=discrX;
    if(firstGapY<1) firstGapY=1; else if(firstGapY>discrY) firstGapY=discrY;
    if(firstGapZ<1) firstGapZ=1; else if(firstGapZ>discrZ) firstGapZ=discrZ;

    if(lastGapX<1) lastGapX=1; else if(lastGapX>discrX) lastGapX=discrX;
    if(lastGapY<1) lastGapY=1; else if(lastGapY>discrY) lastGapY=discrY;
    if(lastGapZ<1) lastGapZ=1; else if(lastGapZ>discrZ) lastGapZ=discrZ;
    Standard_Integer n = (lastGapX-firstGapX);
    if(n>(lastGapY-firstGapY)) n=lastGapY-firstGapY;
    if(n>(lastGapZ-firstGapZ)) n=lastGapZ-firstGapZ;
    n<<=2;
#if DEBUG
      NBBOITES++;
#endif
    if(n>discrX) { 
#if DEBUG
      NBBOITESATESTER++;
#endif
      for(Standard_Integer i=0; i<(i1-i0); i++) { 
	if(Map->ToTest[i]<i0) { 
	  Map->ToTest[i]=boxIndex;
	  break;
	}
      }
    }
    for (theGapY=firstGapY; theGapY<=lastGapY; theGapY++) {
      Map->AppendAxisY(theGapY,boxIndex);
    }
    for (theGapX=firstGapX; theGapX<=lastGapX; theGapX++) {
      Map->AppendAxisX(theGapX,boxIndex);
    }
    for (theGapZ=firstGapZ; theGapZ<=lastGapZ; theGapZ++) {
      Map->AppendAxisZ(theGapZ,boxIndex);
    }      
    //------------------------------------------------------------
    //-- fill table with bits
    //--
    if(TabBits) { 
      Map=(BSB_T3Bits *)TabBits; 
      for (theGapX=firstGapX; theGapX<=lastGapX; theGapX++) {
	for (theGapY=firstGapY; theGapY<=lastGapY; theGapY++) {
	  for (theGapZ=firstGapZ; theGapZ<=lastGapZ; theGapZ++) {
	    long unsigned t=Map->GrilleInteger(theGapX-1,theGapY-1,theGapZ-1);
	    Map->Add(t);
	  }
	}
      }
    }
  }
}
//=======================================================================
#if VERIFICATION
static void VerifCompare(const TColStd_ListOfInteger& lastResult,
			 const Bnd_Box& theBox,
			 const Bnd_Array1OfBox&  taBox) { 
  static int Verif = 1;
  Standard_Integer i ;

  if(Verif) { 
    Standard_Integer i0,i1;
    i0=taBox.Lower();
    i1=taBox.Upper();
    char * qwe=new char [i1+1];  //-- $$$$$$$ ATTENTION IF I0 < 0
    for( i=i0; i<=i1; i++) qwe[i]='\0';
    TColStd_ListIteratorOfListOfInteger  theList(lastResult);
    for (; theList.More(); theList.Next()) {
      qwe[theList.Value()]=(char)1;
    }
    Standard_Integer labox;
    for (labox=i0; labox<=i1; labox++) {
      if (!taBox(labox).IsOut(theBox)) {
	qwe[labox]+=2;
      }
    }
    for(i=i0;i<=i1;i++) { 
      if(qwe[i]==2) { 
	printf("\nPb with box: %d ",i);
      }
      else if(qwe[i]==1) { 
	printf("\n false rejection by %d \n",i);
      }
    }
    delete [] qwe;
  }
}
#endif
//=======================================================================
//function : Compare
//purpose  : 
//=======================================================================
const TColStd_ListOfInteger& Bnd_BoundSortBox::Compare (const Bnd_Box& theBox)

{
 Standard_Integer lacase ;
#if DEBUG
  NBCOMPARE++;
#endif
  lastResult.Clear();
  if (theBox.IsVoid()) return lastResult;
  if (theBox.IsOut(myBox)) { 
#if DEBUG
    REJECTNIV0++;
#endif
    return lastResult;
  }
  const Bnd_Array1OfBox& taBox=myBndComponents->Array1();
  //-- Rejection with the table of bits
  Standard_Boolean touch = Standard_True;
  touch = Standard_False;
  Standard_Real _Xmax,_Ymax,_Zmax;
  BSB_T3Bits* Map = (BSB_T3Bits *)TabBits;
  Standard_Real xmin, ymin, zmin, xmax, ymax, zmax;
  _Xmax=Map->Xmax; _Ymax=Map->Ymax; _Zmax=Map->Zmax;
  theBox.Get(xmin, ymin, zmin, xmax, ymax, zmax);
  Standard_Integer i0,i1,j0,j1,k0,k1;
  if(xmin>Xmin) i0=(Standard_Integer )((xmin-Xmin)*deltaX)-1; else  i0=1;
  if(ymin>Ymin) j0=(Standard_Integer )((ymin-Ymin)*deltaY)-1; else  j0=1;
  if(zmin>Zmin) k0=(Standard_Integer )((zmin-Zmin)*deltaZ)-1; else  k0=1;
  if(xmax<_Xmax) i1=(Standard_Integer )((xmax-Xmin)*deltaX)+1; else  i1=discrX;
  if(ymax<_Ymax) j1=(Standard_Integer )((ymax-Ymin)*deltaY)+1; else  j1=discrY;
  if(zmax<_Zmax) k1=(Standard_Integer )((zmax-Zmin)*deltaZ)+1; else  k1=discrZ;
  if(i0<1) i0=1; else if(i0>discrX) i0=discrX;
  if(j0<1) j0=1; else if(j0>discrY) j0=discrY;
  if(k0<1) k0=1; else if(k0>discrZ) k0=discrZ;

  if(i1<1) i1=1; else if(i1>discrX) i1=discrX;
  if(j1<1) j1=1; else if(j1>discrY) j1=discrY;
  if(k1<1) k1=1; else if(k1>discrZ) k1=discrZ;
  i0--; j0--; k0--; i1--; j1--; k1--;
  for(Standard_Integer i=i0; touch==Standard_False && i<=i1;i++) { 
    for(Standard_Integer j=j0; touch==Standard_False && j<=j1;j++) { 
      for(Standard_Integer k=k0;  touch==Standard_False && k<=k1;k++) {
	long unsigned t=Map->GrilleInteger(i,j,k);
	if(Map->Val(t)) { 
	  touch = Standard_True;
	}
      }
    }
  }
  //-- processing of systematically tested boxes
  if(Map->ToTest) { 
    Standard_Integer l0 = taBox.Lower();
    Standard_Integer l1 = taBox.Upper();
    l1-=l0;
    for(Standard_Integer l=0; Map->ToTest[l]>=l0 && l<(l1-l0); l++) { 
      if(Map->ToTest[l]>=l0) { 
	if (!taBox(Map->ToTest[l]).IsOut(theBox)){
	  lastResult.Append(Map->ToTest[l]);
	}
      }
    }
  }
  if(touch == Standard_False) { 
#if DEBUG
    REJECTNIV1++;
#endif
#if VERIFICATION
    VerifCompare(lastResult,theBox,taBox);
#endif
    return(lastResult);
  }
  //------------------------
  //-- classic processing -- 
  //------------------------
  i0++; i1++; j0++; j1++; k0++; k1++;
  Crible.Clear();
  theFound=6;
  Standard_Integer cardY=0;
  for (lacase=j0; lacase<=j1; lacase++) {
    Standard_Integer nby=Map->NbAxisY(lacase);
    while(nby>0) {
      cardY++;
      Crible.Bind(Map->axisY[lacase][nby], 4);
      nby--;
    }
  }
  if (cardY==0) {
#if VERIFICATION
    VerifCompare(lastResult,theBox,taBox);
#endif
    return lastResult;
  }
  Standard_Integer cardZ=0;  
  for (lacase=k0; lacase<=k1; lacase++) {
    Standard_Integer nbz=Map->NbAxisZ(lacase);  
    while(nbz>0) { 
      cardZ++;
      if (Crible.IsBound(Map->axisZ[lacase][nbz])) {
	Crible.Bind(Map->axisZ[lacase][nbz], 6);
      }
      nbz--;
    }
  }
  if (cardZ==0) {
#if VERIFICATION 
    VerifCompare(lastResult,theBox,taBox);
#endif
    return lastResult;
  }  
  for (lacase=i0; lacase<=i1; lacase++) {
    Standard_Integer nbx = Map->NbAxisX(lacase); 
    while(nbx>0) { 
      Standard_Integer x=Map->axisX[lacase][nbx];
      if (Crible.IsBound(x)) {
	if (Crible(x)==theFound) {
	  Crible.UnBind(x);
	  if (!taBox(x).IsOut(theBox)){
	    lastResult.Append(x);
	  }
	}
      }
      nbx--;
    }
  }
#if VERIFICATION
  VerifCompare(lastResult,theBox,taBox);
#endif
  return lastResult; 
}
//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void Bnd_BoundSortBox::Dump() const
{}
//=======================================================================
//function : Compare
//purpose  : 
//=======================================================================
const TColStd_ListOfInteger& Bnd_BoundSortBox::Compare(const gp_Pln& thePlane)

{
  lastResult.Clear();
  Standard_Integer i;
  const Bnd_Array1OfBox& boxes = myBndComponents->Array1();
  for (i = boxes.Lower(); i <= boxes.Upper(); i++) {
    if (!boxes(i).IsOut(thePlane))
      lastResult.Append(i);
  }
  return lastResult;
}
//=======================================================================
void Bnd_BoundSortBox::Destroy() { 
#if DEBUG
  printf("\nDESTROY NBCOMPARE:%lu  REJECTNIV0:%lu  REJECTIONSOK=%lu  NBBOITES:%lu NBBOITESATESTER:%lu\n",
	 NBCOMPARE,REJECTNIV0,REJECTNIV1,NBBOITES,NBBOITESATESTER);
#endif
  BSB_T3Bits* Map = (BSB_T3Bits *)TabBits;
  if(Map) { 
    delete Map;
    Map=0;
  }
}
//=======================================================================    

