// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _WIN32


#include <OSD_Directory.hxx>
#include <OSD_File.hxx>
#include <OSD_FileNode.hxx>
#include <OSD_Protection.hxx>
#include <OSD_WhoAmI.hxx>

#include <sys/stat.h>
// Ci-joint le tableau de gestion des protection (Ajout et Retrait). Les 
// tableaux sont des tableaux a deux dimensions, indices par l'enumeration
// OSD_SingleProtection. Il y a en tout 16 possibilites dans l enumeration.
// Voir JPT pour tous renseignements....
static OSD_SingleProtection TabProtAdd [16][16] =
{
{OSD_None,OSD_R,OSD_W,OSD_RW,OSD_X,OSD_RX,OSD_WX,OSD_RWX,OSD_D,OSD_RD,OSD_WD,OSD_RWD,OSD_XD,OSD_RXD,OSD_WXD,OSD_RWXD},

{OSD_R,OSD_R,OSD_RW,OSD_RW,OSD_RX,OSD_RX,OSD_RWX,OSD_RWX,OSD_RD,OSD_RD,OSD_RWD,OSD_RWD,OSD_RXD,OSD_RXD,OSD_RWXD,OSD_RWXD},

{OSD_W,OSD_RW,OSD_W,OSD_RW,OSD_WX,OSD_RWX,OSD_WX,OSD_RWX,OSD_WD,OSD_RWD,OSD_WD,OSD_RWD,OSD_WXD,OSD_RWXD,OSD_WXD,OSD_RWXD},

{OSD_RW,OSD_RW,OSD_RW,OSD_RW,OSD_RWX,OSD_RWX,OSD_RWX,OSD_RWX,OSD_RWD,OSD_RWD,OSD_RWD,OSD_RWD,OSD_RWXD,OSD_RWXD,OSD_RWXD,OSD_RWXD},

{OSD_X,OSD_RX,OSD_WX,OSD_RWX,OSD_X,OSD_RX,OSD_WX,OSD_RWX,OSD_XD,OSD_RXD,OSD_WXD,OSD_RWXD,OSD_XD,OSD_RXD,OSD_WXD,OSD_RWXD},

{OSD_RX,OSD_RX,OSD_RWX,OSD_RWX,OSD_RX,OSD_RX,OSD_RWX,OSD_RWX,OSD_RXD,OSD_RXD,OSD_RWXD,OSD_RWXD,OSD_RXD,OSD_RXD,OSD_RWXD,OSD_RWXD},

{OSD_WX,OSD_RWX,OSD_RWX,OSD_RWX,OSD_WX,OSD_RWX,OSD_WX,OSD_RWX,OSD_WXD,OSD_RWXD,OSD_WXD,OSD_RWXD,OSD_WXD,OSD_RWXD,OSD_WXD,OSD_RWXD},

{OSD_RWX,OSD_RWX,OSD_RWX,OSD_RWX,OSD_RWX,OSD_RWX,OSD_RWX,OSD_RWX,OSD_RWX,OSD_RWXD,OSD_RWXD,OSD_RWXD,OSD_RWXD,OSD_RWXD,OSD_RWXD,OSD_RWXD},

{OSD_D,OSD_RD,OSD_WD,OSD_RWD,OSD_XD,OSD_RXD,OSD_WXD,OSD_RWXD,OSD_D,OSD_RD,OSD_WD,OSD_RWD,OSD_XD,OSD_RXD,OSD_WXD,OSD_RWXD},

{OSD_RD,OSD_RD,OSD_RWD,OSD_RWD,OSD_RXD,OSD_RXD,OSD_RWXD,OSD_RWXD,OSD_RD,OSD_RD,OSD_RWD,OSD_RWD,OSD_RXD,OSD_RXD,OSD_RWXD,OSD_RWXD},

{OSD_WD,OSD_RWD,OSD_WD,OSD_RWD,OSD_WXD,OSD_RWXD,OSD_WXD,OSD_RWXD,OSD_WD,OSD_RWD,OSD_WD,OSD_RWD,OSD_WXD,OSD_RWXD,OSD_WXD,OSD_RWXD},

{OSD_RWD,OSD_RWD,OSD_RWD,OSD_RWD,OSD_RWXD,OSD_RWXD,OSD_RWXD,OSD_RWXD,OSD_RWD,OSD_RWD,OSD_RWD,OSD_RWD,OSD_RWXD,OSD_RWXD,OSD_RWXD,OSD_RWXD},

{OSD_XD,OSD_RXD,OSD_WXD,OSD_RWXD,OSD_XD,OSD_RXD,OSD_WXD,OSD_RWXD,OSD_XD,OSD_RXD,OSD_WXD,OSD_RWXD,OSD_XD,OSD_RXD,OSD_WXD,OSD_RWXD},

{OSD_RXD,OSD_RXD,OSD_RWXD,OSD_RWXD,OSD_RXD,OSD_RXD,OSD_RWXD,OSD_RWXD,OSD_RXD,OSD_RXD,OSD_RWXD,OSD_RWXD,OSD_RXD,OSD_RXD,OSD_RWXD,OSD_RWXD},

{OSD_WXD,OSD_RWXD,OSD_WXD,OSD_RWXD,OSD_WXD,OSD_RWXD,OSD_WXD,OSD_RWXD,OSD_WXD,OSD_RWXD,OSD_WXD,OSD_RWXD,OSD_WXD,OSD_RWXD,OSD_WXD,OSD_RWXD},

{OSD_RWXD,OSD_RWXD,OSD_RWXD,OSD_RWXD,OSD_RWXD,OSD_RWXD,OSD_RWXD,OSD_RWXD,OSD_RWXD,OSD_RWXD,OSD_RWXD,OSD_RWXD,OSD_RWXD,OSD_RWXD,OSD_RWXD,OSD_RWXD}

};

// ----------------------- 

static OSD_SingleProtection TabProtSub [16][16] =
{
{OSD_None,OSD_None,OSD_None,OSD_None,OSD_None,OSD_None,OSD_None,OSD_None,OSD_None,OSD_None,OSD_None,OSD_None,OSD_None,OSD_None,OSD_None,OSD_None},

{OSD_R,OSD_None,OSD_R,OSD_None,OSD_R,OSD_None,OSD_R,OSD_None,OSD_R,OSD_None,OSD_R,OSD_None,OSD_R,OSD_None,OSD_R,OSD_None},

{OSD_W,OSD_W,OSD_None,OSD_None,OSD_W,OSD_W,OSD_None,OSD_None,OSD_W,OSD_W,OSD_None,OSD_None,OSD_W,OSD_W,OSD_None,OSD_None},

{OSD_RW,OSD_W,OSD_R,OSD_None,OSD_RW,OSD_W,OSD_R,OSD_None,OSD_RW,OSD_W,OSD_R,OSD_None,OSD_RW,OSD_W,OSD_R,OSD_None},

{OSD_X,OSD_X,OSD_X,OSD_None,OSD_None,OSD_None,OSD_None,OSD_X,OSD_X,OSD_X,OSD_X,OSD_None,OSD_None,OSD_None,OSD_None,OSD_None},

{OSD_RX,OSD_X,OSD_RX,OSD_X,OSD_R,OSD_None,OSD_R,OSD_None,OSD_RX,OSD_X,OSD_RX,OSD_X,OSD_R,OSD_None,OSD_R,OSD_None},

{OSD_WX,OSD_WX,OSD_X,OSD_X,OSD_W,OSD_W,OSD_None,OSD_None,OSD_WX,OSD_WX,OSD_X,OSD_X,OSD_W,OSD_W,OSD_None,OSD_None},

{OSD_RWX,OSD_WX,OSD_RX,OSD_X,OSD_RW,OSD_W,OSD_R,OSD_None,OSD_RWX,OSD_WX,OSD_RX,OSD_X,OSD_RW,OSD_W,OSD_R,OSD_None},

{OSD_D,OSD_D,OSD_D,OSD_D,OSD_D,OSD_D,OSD_D,OSD_D,OSD_None,OSD_None,OSD_None,OSD_None,OSD_None,OSD_None,OSD_None,OSD_None},

{OSD_RD,OSD_D,OSD_RD,OSD_D,OSD_RD,OSD_D,OSD_RD,OSD_D,OSD_R,OSD_None,OSD_R,OSD_None,OSD_R,OSD_None,OSD_R,OSD_None},

{OSD_WD,OSD_WD,OSD_D,OSD_D,OSD_WD,OSD_WD,OSD_D,OSD_D,OSD_W,OSD_W,OSD_None,OSD_None,OSD_W,OSD_W,OSD_None,OSD_None},

{OSD_RWD,OSD_WD,OSD_RD,OSD_D,OSD_RWD,OSD_WD,OSD_RD,OSD_D,OSD_RW,OSD_W,OSD_R,OSD_None,OSD_RW,OSD_W,OSD_R,OSD_None},

{OSD_XD,OSD_XD,OSD_XD,OSD_XD,OSD_D,OSD_D,OSD_D,OSD_D,OSD_X,OSD_X,OSD_X,OSD_X,OSD_None,OSD_None,OSD_None,OSD_None},

{OSD_RXD,OSD_XD,OSD_RXD,OSD_XD,OSD_RD,OSD_D,OSD_RD,OSD_D,OSD_RX,OSD_X,OSD_RX,OSD_X,OSD_R,OSD_None,OSD_R,OSD_None},

{OSD_WXD,OSD_WXD,OSD_XD,OSD_XD,OSD_WD,OSD_WD,OSD_D,OSD_D,OSD_WX,OSD_WX,OSD_X,OSD_X,OSD_W,OSD_W,OSD_None,OSD_None},

{OSD_RWXD,OSD_WXD,OSD_RXD,OSD_XD,OSD_RWD,OSD_WD,OSD_RD,OSD_D,OSD_RWX,OSD_WX,OSD_RX,OSD_X,OSD_RW,OSD_W,OSD_R,OSD_None}

};
//const OSD_WhoAmI Iam = OSD_WProtection;


// Initialize System, Group, World for read only and User for read & write

OSD_Protection::OSD_Protection(){
 s = OSD_R;
 u = OSD_RWD;
 g = OSD_R;
 w = OSD_R;
}

OSD_Protection::OSD_Protection(const OSD_SingleProtection System, 
                                     const OSD_SingleProtection User,
                                     const OSD_SingleProtection Group,
                                     const OSD_SingleProtection World){

 s = System;
 u = User;
 g = Group;
 w = World;
}

void  OSD_Protection::Values(OSD_SingleProtection& System, 
                             OSD_SingleProtection& User,
                             OSD_SingleProtection& Group,
                             OSD_SingleProtection& World){
 System = s;
 User = u;
 Group = g;
 World = w;
}


void  OSD_Protection::SetValues(const OSD_SingleProtection System, 
                                const OSD_SingleProtection User,
                                const OSD_SingleProtection Group,
                                const OSD_SingleProtection World){
 
 s = System;
 u = User;
 g = Group;
 w = World;
}


void OSD_Protection::SetSystem (const OSD_SingleProtection priv){
 s = priv;
}

void OSD_Protection::SetUser (const OSD_SingleProtection priv){
 u = priv;
}

void OSD_Protection::SetGroup (const OSD_SingleProtection priv){
 g = priv;
}

void OSD_Protection::SetWorld (const OSD_SingleProtection priv){
 w = priv;
}


OSD_SingleProtection OSD_Protection::System()const{
 return(s);
}

OSD_SingleProtection OSD_Protection::User()const{
 return(u);
}

OSD_SingleProtection OSD_Protection::Group()const{
 return(g);
}

OSD_SingleProtection OSD_Protection::World()const{
 return(w);
}


void OSD_Protection::Add(OSD_SingleProtection& aProtection,
                         const OSD_SingleProtection aRight){
 aProtection = TabProtAdd[aProtection][aRight];
}


void OSD_Protection::Sub(OSD_SingleProtection& aProtection,
                         const OSD_SingleProtection aRight){
 aProtection = TabProtSub[aProtection][aRight];
}


/* Get internal UNIX's access rights for user, group and other */

Standard_Integer  OSD_Protection::Internal()const{

Standard_Integer internal_prot = 0;

 if (u & OSD_R) internal_prot |= S_IRUSR;
 if (u & OSD_W) internal_prot |= S_IWUSR;
 if (u & OSD_D) internal_prot |= S_IWUSR;
 if (u & OSD_X) internal_prot |= S_IXUSR;

 if (g & OSD_R) internal_prot |= S_IRGRP;
 if (g & OSD_W) internal_prot |= S_IWGRP;
 if (g & OSD_D) internal_prot |= S_IWGRP;
 if (g & OSD_X) internal_prot |= S_IXGRP;

 if (w & OSD_R) internal_prot |= S_IROTH;
 if (w & OSD_W) internal_prot |= S_IWOTH;
 if (w & OSD_D) internal_prot |= S_IWOTH;
 if (w & OSD_X) internal_prot |= S_IXOTH;

 if (s & OSD_R) internal_prot |= S_IROTH;
 if (s & OSD_W) internal_prot |= S_IWOTH;
 if (s & OSD_D) internal_prot |= S_IWOTH;
 if (s & OSD_X) internal_prot |= S_IXOTH;                          

 return ( internal_prot );
}


#else

//------------------------------------------------------------------------
//-------------------  WNT Sources of OSD_Protection ---------------------
//------------------------------------------------------------------------

#include <OSD_Protection.hxx>

#define FLAG_READ    0x00000001
#define FLAG_WRITE   0x00000002
#define FLAG_EXECUTE 0x00000004
#define FLAG_DELETE  0x00000008

static Standard_Integer     __fastcall _get_mask ( OSD_SingleProtection );
static OSD_SingleProtection __fastcall _get_prot ( Standard_Integer     );

OSD_Protection :: OSD_Protection () {

 s = OSD_RWXD;
 u = OSD_RWXD;
 g = OSD_RX;
 w = OSD_RX;

}  // end constructor ( 1 )

OSD_Protection :: OSD_Protection (
                   const OSD_SingleProtection System,
                   const OSD_SingleProtection User,
                   const OSD_SingleProtection Group,
                   const OSD_SingleProtection World
                  ) {

 SetValues ( System, User, Group, World );

}  // end constructor ( 2 )

void OSD_Protection :: Values (
                        OSD_SingleProtection& System,
                        OSD_SingleProtection& User,
                        OSD_SingleProtection& Group,
                        OSD_SingleProtection& World
                       ) {
 System = s;
 User   = u;
 Group  = g;
 World  = w;

}  // end OSD_Protection :: Values

void OSD_Protection :: SetValues (
                        const OSD_SingleProtection System,
                        const OSD_SingleProtection User,
                        const OSD_SingleProtection Group,
                        const OSD_SingleProtection World
                       ) {

 s = System;
 u = User;
 g = Group;
 w = World;

}  // end OSD_Protection :: SetValues

void OSD_Protection :: SetSystem ( const OSD_SingleProtection priv ) {

 s = priv;

}  // end OSD_Protection :: SetSystem

void OSD_Protection :: SetUser ( const OSD_SingleProtection priv ) {

 u = priv;

}  // end OSD_Protection :: SetUser

void OSD_Protection :: SetGroup ( const OSD_SingleProtection priv ) {

 g = priv;

}  // end OSD_Protection :: SetGroup

void OSD_Protection :: SetWorld ( const OSD_SingleProtection priv ) {

 w = priv;

}  // end OSD_Protection :: SetWorld

OSD_SingleProtection OSD_Protection :: System () const {

 return s;

}  // end OSD_Protection :: System

OSD_SingleProtection OSD_Protection :: User () const {

 return u;

}  // end OSD_Protection :: User

OSD_SingleProtection OSD_Protection :: Group () const {

 return g;

}  // end OSD_Protection :: Group

OSD_SingleProtection OSD_Protection :: World () const {

 return w;

}  // end OSD_Protection :: World

void OSD_Protection :: Add (
                        OSD_SingleProtection& aProt,
                        const OSD_SingleProtection aRight
                       ) {

 Standard_Integer pMask = 0;
 Standard_Integer rMask = 0;
 Standard_Integer sMask = 0;

 pMask = _get_mask ( aProt  );
 rMask = _get_mask ( aRight );

 if (   (  rMask & FLAG_READ && !( pMask & FLAG_READ )  ) || pMask & FLAG_READ   )

  sMask |= FLAG_READ;

 if (   (  rMask & FLAG_WRITE && !( pMask & FLAG_WRITE )  ) || pMask & FLAG_WRITE   )

  sMask |= FLAG_WRITE;

 if (   (  rMask & FLAG_EXECUTE && !( pMask & FLAG_EXECUTE )  ) || pMask & FLAG_EXECUTE   )

  sMask |= FLAG_EXECUTE;

 if (   (  rMask & FLAG_DELETE && !( pMask & FLAG_DELETE )  ) || pMask & FLAG_DELETE   )

  sMask |= FLAG_DELETE;

 aProt = _get_prot ( sMask );

}  // end OSD_Protection :: Add

void OSD_Protection :: Sub (
                        OSD_SingleProtection& aProt,
                        const OSD_SingleProtection aRight
                       ) {

 Standard_Integer pMask = 0;
 Standard_Integer rMask = 0;

 pMask = _get_mask ( aProt  );
 rMask = _get_mask ( aRight );

 if ( rMask & FLAG_READ )

  pMask &= ~FLAG_READ;

 if ( rMask & FLAG_WRITE )

  pMask &= ~FLAG_WRITE;

 if ( rMask & FLAG_EXECUTE )

  pMask &= ~FLAG_EXECUTE;

 if ( rMask & FLAG_DELETE )

  pMask &= ~FLAG_DELETE;

 aProt = _get_prot ( pMask );

}  // end OSD_Protection :: Sub

Standard_Integer OSD_Protection :: Internal () const {

 return 0;

}  // end OSD_Protection :: Internal

static Standard_Integer __fastcall _get_mask ( OSD_SingleProtection p ) {

 Standard_Integer retVal = 0;

 if ( p == OSD_R   || p == OSD_RW   || p == OSD_RX || p == OSD_RWX ||
      p == OSD_RXD || p == OSD_RWXD || p == OSD_RD || p == OSD_RWD
 ) retVal |= FLAG_READ;

 if ( p == OSD_W   || p == OSD_RW   || p == OSD_WX || p == OSD_RWX ||
      p == OSD_WXD || p == OSD_RWXD || p == OSD_WD || p == OSD_RWD
 ) retVal |= FLAG_WRITE;

 if ( p == OSD_X  || p == OSD_RX  || p == OSD_WX  || p == OSD_RWX  ||
      p == OSD_XD || p == OSD_RXD || p == OSD_WXD || p == OSD_RWXD
 ) retVal |= FLAG_EXECUTE;

 if ( p == OSD_D  || p == OSD_RD  || p == OSD_WD  || p == OSD_RWD  ||
      p == OSD_XD || p == OSD_RXD || p == OSD_WXD || p == OSD_RWXD
 ) retVal |= FLAG_DELETE;

 return retVal;

}  // end _get_mask

static OSD_SingleProtection __fastcall _get_prot ( Standard_Integer m ) {

 OSD_SingleProtection retVal;

 switch ( m ) {
 
  case FLAG_READ:

   retVal = OSD_R;

  break;

  case FLAG_WRITE:

   retVal = OSD_W;

  break;

  case FLAG_READ | FLAG_WRITE:

   retVal = OSD_RW;

  break;

  case FLAG_EXECUTE:

   retVal = OSD_X;

  break;

  case FLAG_READ | FLAG_EXECUTE:

   retVal = OSD_RX;

  break;

  case FLAG_WRITE | FLAG_EXECUTE:

   retVal = OSD_WX;

  break;

  case FLAG_READ | FLAG_WRITE | FLAG_EXECUTE:

   retVal = OSD_RWX;

  break;

  case FLAG_DELETE:

   retVal = OSD_D;

  break;

  case FLAG_READ | FLAG_DELETE:

   retVal = OSD_RD;

  break;

  case FLAG_WRITE | FLAG_DELETE:

   retVal = OSD_WD;

  break;

  case FLAG_READ | FLAG_WRITE | FLAG_DELETE:

   retVal = OSD_RWD;

  break;

  case FLAG_EXECUTE | FLAG_DELETE:

   retVal = OSD_XD;

  break;

  case FLAG_READ | FLAG_EXECUTE | FLAG_DELETE:

   retVal = OSD_RXD;

  break;

  case FLAG_WRITE | FLAG_EXECUTE | FLAG_DELETE:

   retVal = OSD_WXD;

  break;

  case FLAG_READ | FLAG_WRITE | FLAG_EXECUTE | FLAG_DELETE:

   retVal = OSD_RWXD;

  break;

  default:

   retVal = OSD_None;
 
 }  // end switch

 return retVal;

}  // end _get_prot

#endif
