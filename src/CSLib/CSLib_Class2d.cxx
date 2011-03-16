// File:	CSLib_Class2d.cxx
// Created:	Wed Mar  8 15:06:24 1995
// Author:	Laurent BUCHARD
//		<lbr@mastox>

//#define No_Standard_OutOfRange

#include <CSLib_Class2d.ixx>
#include <Standard_ConstructionError.hxx>

static inline 
  Standard_Real Transform2d(const Standard_Real u,
			    const Standard_Real umin,
			    const Standard_Real umaxmumin);

//=======================================================================
//function : CSLib_Class2d
//purpose  : 
//=======================================================================
CSLib_Class2d::CSLib_Class2d(const TColgp_Array1OfPnt2d& TP2d,
			     const Standard_Real aTolu,
			     const Standard_Real aTolv,
			     const Standard_Real umin,
			     const Standard_Real vmin,
			     const Standard_Real umax,
			     const Standard_Real vmax) 
{
  Umin=umin;
  Vmin=vmin;
  Umax=umax;
  Vmax=vmax;
  //
  if(umax<=umin || vmax<=vmin) { 
    MyPnts2dX=NULL;
    MyPnts2dY=NULL;
    N=0;
  }
  //
  else {
    Standard_Integer i, iLower;
    Standard_Real du,dv,*Pnts2dX,*Pnts2dY, aPrc;   
    //
    aPrc=1.e-10;
    N = TP2d.Length();
    Tolu = aTolu;
    Tolv = aTolv;
    MyPnts2dX = new Standard_Real [N+1];
    MyPnts2dY = new Standard_Real [N+1];
    du=umax-umin;
    dv=vmax-vmin;
    Pnts2dX = (Standard_Real *)MyPnts2dX;
    Pnts2dY = (Standard_Real *)MyPnts2dY;
    //
    iLower=TP2d.Lower();
    for(i = 0; i<N; ++i) { 
      const gp_Pnt2d& aP2D=TP2d(i+iLower);
      Pnts2dX[i] = Transform2d(aP2D.X(), umin, du);
      Pnts2dY[i] = Transform2d(aP2D.Y(), vmin, dv);
    }
    Pnts2dX[N]=Pnts2dX[0];
    Pnts2dY[N]=Pnts2dY[0];
    //
    if(du>aPrc) {
      Tolu/=du;
    }
    if(dv>aPrc) {
      Tolv/=dv;
    }
  }
}
//=======================================================================
//function : Destroy
//purpose  : 
//=======================================================================
void CSLib_Class2d::Destroy() { 
  if(MyPnts2dX) { 
    delete [] (Standard_Real *)MyPnts2dX;
    MyPnts2dX=NULL;
  }
  if(MyPnts2dY) { 
    delete [] (Standard_Real *)MyPnts2dY;
    MyPnts2dY=NULL;
  }
}

//-- Attention   Tableau de 0 ------> N + 1 
//--                        P1 ..... Pn P1
//--
//--     1  2  3
//--     4  0  5
//--     6  7  8
//-- 
//=======================================================================
//function : SiDans
//purpose  : 
//=======================================================================
Standard_Integer CSLib_Class2d::SiDans(const gp_Pnt2d& P) const
{ 
  if(!N) {
    return 0;
  }
  //
  Standard_Real x,y, aTolu, aTolv;
  //
  x = P.X(); y = P.Y();
  aTolu=Tolu*(Umax-Umin);
  aTolv=Tolv*(Vmax-Vmin);
  //
  if(Umin<Umax && Vmin<Vmax)    {
    if( ( x<(Umin-aTolu) ) || 
       ( x>(Umax+aTolu) ) || 
       ( y<(Vmin-aTolv) ) || 
       ( y>(Vmax+aTolv) ) ) {
      return -1;
    }
    x=Transform2d(x,Umin,Umax-Umin);
    y=Transform2d(y,Vmin,Vmax-Vmin);
  }


  Standard_Integer res = InternalSiDansOuOn(x,y);
  if(res==-1) {     //-- on est peut etre ON 
    return 0;
  }
  if(Tolu || Tolv) {
    if(res != InternalSiDans(x-Tolu,y-Tolv)) return 0;
    if(res != InternalSiDans(x+Tolu,y-Tolv)) return 0;
    if(res != InternalSiDans(x-Tolu,y+Tolv)) return 0;
    if(res != InternalSiDans(x+Tolu,y+Tolv)) return 0; 
  }
  //
  return((res)? 1: -1);
}
//=======================================================================
//function : SiDans_OnMode
//purpose  : 
//=======================================================================
Standard_Integer CSLib_Class2d::SiDans_OnMode(const gp_Pnt2d& P,
					      const Standard_Real Tol) const
{ 
  if(!N){
    return 0;
  }
  //
  Standard_Real x,y, aTolu, aTolv;
  //
  x = P.X(); y = P.Y();
  aTolu=Tol; 
  aTolv=Tol; 

  //-- ****** A FAIRE PLUS TARD, ESTIMER EN CHAQUE POINT la Tol2d en fct de la Tol3d *****
  if(Umin<Umax && Vmin<Vmax) { 
    if(x<(Umin-aTolu) || (x>Umax+aTolu) || 
       (y<Vmin-aTolv) || (y>Vmax+aTolv)) {
      return -1;
    }
    x=Transform2d(x,Umin,Umax-Umin);
    y=Transform2d(y,Vmin,Vmax-Vmin);
  }
  //
  Standard_Integer res = InternalSiDansOuOn(x,y);
  if(aTolu || aTolv) {
    if(res != InternalSiDans(x-aTolu,y-aTolv)) return 0;
    if(res != InternalSiDans(x+aTolu,y-aTolv)) return 0;
    if(res != InternalSiDans(x-aTolu,y+aTolv)) return 0;
    if(res != InternalSiDans(x+aTolu,y+aTolv)) return 0; 
  }
  return((res)? 1: -1);
}
//=======================================================================
//function : InternalSiDans
//purpose  : 
//=======================================================================
Standard_Integer CSLib_Class2d::InternalSiDans(const Standard_Real Px,
					       const Standard_Real Py) const
{ 
  Standard_Integer nbc, i, ip1, SH, NH;
  Standard_Real *Pnts2dX, *Pnts2dY;
  Standard_Real  x, y, nx, ny;
  //
  nbc = 0;
  i   = 0;
  ip1 = 1;
  Pnts2dX = (Standard_Real *)MyPnts2dX;
  Pnts2dY = (Standard_Real *)MyPnts2dY;
  x   = (Pnts2dX[i]-Px);
  y   = (Pnts2dY[i]-Py);
  SH  = (y<0.)? -1 : 1;
  //
  for(i=0; i<N ; i++,ip1++) { 
    nx = Pnts2dX[ip1] - Px;
    ny = Pnts2dY[ip1] - Py;
    
    NH = (ny<0.)? -1 : 1;
    if(NH!=SH) { 
      if(x>0. && nx>0.) {
	nbc++;
      }
      else { 
	if(x>0.0 || nx>0.) { 
	  if((x-y*(nx-x)/(ny-y))>0.) {
	    nbc++;
	  }
	}
      }
      SH = NH;
    }
    x = nx; y = ny;
  }
  return(nbc&1);
}
//modified by NIZNHY-PKV Fri Jan 15 09:03:48 2010f
//=======================================================================
//function : InternalSiDansOuOn
//purpose  :  meme code que ci-dessus + test sur ON (return(-1) dans ce cas
//=======================================================================
Standard_Integer CSLib_Class2d::InternalSiDansOuOn(const Standard_Real Px,
						   const Standard_Real Py) const 
{ 
  Standard_Integer nbc, i, ip1, SH, NH, iRet;
  Standard_Real *Pnts2dX, *Pnts2dY;
  Standard_Real x, y, nx, ny, aX;
  Standard_Real aYmin, aTol;
  //
  aTol=(Tolu>Tolv)? Tolu : Tolv;
  nbc = 0;
  i   = 0;
  ip1 = 1;
  Pnts2dX = (Standard_Real *)MyPnts2dX;
  Pnts2dY = (Standard_Real *)MyPnts2dY;
  x   = (Pnts2dX[i]-Px);
  y   = (Pnts2dY[i]-Py);
  aYmin=y;
  SH  = (y<0.)? -1 : 1;
  for(i=0; i<N ; i++, ip1++) { 
   
    nx = Pnts2dX[ip1] - Px;
    ny = Pnts2dY[ip1] - Py;
    //-- le 14 oct 97 
    if(nx<Tolu && nx>-Tolu && ny<Tolv && ny>-Tolv) { 
      iRet=-1;
      return iRet;
    }
    //find Y coordinate of polyline for current X gka
    //in order to detect possible status ON
    Standard_Real aDx = (Pnts2dX[ip1] - Pnts2dX[ip1-1]);
    if( (Pnts2dX[ip1-1] - Px) * nx < 0.)
    {
     
      Standard_Real aCurPY =  Pnts2dY[ip1] - (Pnts2dY[ip1] - Pnts2dY[ip1-1])/aDx *nx;
      Standard_Real aDeltaY = aCurPY - Py;
      if(aDeltaY >= -Tolv && aDeltaY <= Tolv)
      {
         iRet=-1;
         return iRet;
      }
    }
    //
      
    NH = (ny<0.)? -1 : 1;
    if(NH!=SH) { 
      if(x>0. && nx>0.) {
	nbc++;
      }
      else { 
	if(x>0. || nx>0.) { 
	  aX=x-y*(nx-x)/(ny-y);
	  if(aX>0.){
	    nbc++;
	  }
	}
      }
      SH = NH;
    }
    else {// y has same sign as ny  
      if (ny<aYmin) {
	aYmin=ny;
      }
    }
    x = nx; y = ny; 
  }// for(i=0; i<N ; i++,ip1++) { 
 
  iRet=nbc&1;
  return iRet;
}
//modified by NIZNHY-PKV Fri Jan 15 09:03:55 2010t
//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================
const CSLib_Class2d& CSLib_Class2d::Copy(const CSLib_Class2d& ) const 
{ 
  cerr<<"Copy not allowed in CSLib_Class2d"<<endl;
  Standard_ConstructionError::Raise();
  return *this;
}
//=======================================================================
//function : Transform2d
//purpose  : 
//=======================================================================
Standard_Real Transform2d(const Standard_Real u,
			  const Standard_Real umin,
			  const Standard_Real umaxmumin) 
{ 
  if(umaxmumin>1e-10) { 
    Standard_Real U = (u-umin)/umaxmumin;
    return U;
  }
  else { 
    return u;
  }
}
