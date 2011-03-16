// File:	TopoDSToStep_Root.cxx
// Created:	Mon Jul 26 17:31:52 1993
// Author:	Martine LANGLOIS
//		<mla@mastox>

#include <TopoDSToStep_Root.ixx>

TopoDSToStep_Root::TopoDSToStep_Root ()
{  done = Standard_False;  toler = 0.0001;  }

Standard_Real& TopoDSToStep_Root::Tolerance ()
{  return toler;  }

Standard_Boolean TopoDSToStep_Root::IsDone () const
{
  return done; 
}
