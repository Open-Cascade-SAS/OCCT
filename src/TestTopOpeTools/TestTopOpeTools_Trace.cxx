// File:	TestTopOpeTools_Trace.cxx
// Created:	Mon Aug  1 12:13:25 1994
// Author:	Jean Yves LEBEY
//		<jyl@meteox>

#include <TestTopOpeTools_Trace.hxx>

TestTopOpeTools_Trace::TestTopOpeTools_Trace
(const Standard_Integer nbmaxentry,
 const TCollection_AsciiString& genre) :
 mygenre(genre),
 myfirstentry(1),mynbmaxentry(nbmaxentry),mynbentries(0),
 myflag(myfirstentry,mynbmaxentry), 
 myfunc(myfirstentry,mynbmaxentry), 
 myftyp(myfirstentry,mynbmaxentry),
 myverbose(Standard_False),
 myfverbose(myfirstentry,mynbmaxentry)
{}

TestTopOpeTools_Trace::TestTopOpeTools_Trace
(const Standard_Integer nbmaxentry) :
mygenre("flag"),
myfirstentry(1),mynbmaxentry(nbmaxentry),mynbentries(0),
myflag(myfirstentry,mynbmaxentry), 
myfunc(myfirstentry,mynbmaxentry), 
myftyp(myfirstentry,mynbmaxentry),
myverbose(Standard_False),
myfverbose(myfirstentry,mynbmaxentry)
{}

Standard_Integer TestTopOpeTools_Trace::Add
(const t_flag flag,tf_value func)
{ return Add(flag,(tf_value)func,te_value); }

Standard_Integer TestTopOpeTools_Trace::SetVerbose
(const Standard_Boolean b)
{ myverbose = b; return 0;}

Standard_Integer TestTopOpeTools_Trace::SetVerbose
(const t_flag flag,const Standard_Boolean b)
{ 
  Standard_Integer index;
  if ( !Exist(flag,index) ) return 1;
  if (b) cout<<"set "<<mygenre<<" "<<flag<<" verbose"<<endl;
  else   cout<<"set "<<mygenre<<" "<<flag<<" not verbose"<<endl;
  myfverbose.SetValue(index,b);
  return 0;
}

Standard_Integer TestTopOpeTools_Trace::Set
(const Standard_Integer mute,const t_flag flag,const t_value value)
{ 
  Standard_Integer index;
  if ( !Exist(flag,index) ) return 1;
  if (mute==0) {
    if (value) cout<<"activation de "<<mygenre<<" "<<flag<<endl;
    else       cout<<"desactivation de "<<mygenre<<" "<<flag<<endl;
  }
  (*((tf_value)Getfunc(index)))(value);
  return 0;
}

Standard_Integer TestTopOpeTools_Trace::Add
(const t_flag flag,tf_intarg func)
{ return Add(flag,(tf_value)func,te_intarg); }

Standard_Integer TestTopOpeTools_Trace::Add
(const t_flag flag,tf_int_intarg func)
{ return Add(flag,(tf_value)func,te_int_intarg); }

Standard_Integer TestTopOpeTools_Trace::Set
(const Standard_Integer mute,const t_flag flag, const t_value value,
 Standard_Integer n, const char** a)
{ 
  Standard_Integer index;
  if ( !Exist(flag,index) ) return 1;
  if      ( Getftyp(index) == te_intarg ) {
    (*((tf_intarg)Getfunc(index)))(value,n,a);
    if (mute==0) {
      cout<<mygenre<<" "<<flag<<" active avec :";
      cout<<" "<<value;
      for(Standard_Integer i=0;i<n;i++) cout<<" "<<a[i];
      cout<<endl;
    }
  }
  else if ( Getftyp(index) == te_int_intarg ) {
    Standard_Integer err = (*((tf_int_intarg)Getfunc(index)))(value,n,a);
    if (err) {
      cout<<"ERREUR activation de "<<mygenre<<" "<<flag<<" "<<value;
      for(Standard_Integer i=0;i<n;i++) cout<<" "<<a[i];cout<<endl;
    }
    else {
      if (mute==0) {
	cout<<mygenre<<" "<<flag<<" active avec :";
	cout<<" "<<value;
	for(Standard_Integer i=0;i<n;i++) cout<<" "<<a[i];
	cout<<endl;
      }
    }
  }
  else if ( Getftyp(index) == te_value ) {
    return Set(mute,flag,value);
  }
  else {
    cout<<"TestTopOpeTools_Trace::Set : unknown function type"<<endl;
    return 1;
  }
  return 0;
}

void TestTopOpeTools_Trace::Dump()
{ 
  for (Standard_Integer i=myfirstentry; i<=mynbentries; i++) {
    if (i==myfirstentry) cout<<"Defined "<<mygenre<<"s :";
    cout<<" "<<Getflag(i);
    if (i==mynbentries) cout<<endl;
  }
}

void TestTopOpeTools_Trace::Reset(const t_value value)
{
  for (Standard_Integer i=myfirstentry;i<=mynbentries;i++)
    if      (Getftyp(i)==te_value)
      (*((tf_value)Getfunc(i)))(value);
    else if (Getftyp(i)==te_intarg)
      (*((tf_intarg)Getfunc(i)))(value,0,NULL);    
    else if (Getftyp(i)==te_int_intarg)
      (*((tf_int_intarg)Getfunc(i)))(value,0,NULL);    
}

// ==== Private

Standard_Boolean TestTopOpeTools_Trace::Exist
(const t_flag flag, Standard_Integer& index)
{ 
  for (Standard_Integer i=myfirstentry; i<=mynbentries; i++)
    if (myflag.Value(i).IsEqual(flag)) { index = i; return Standard_True; }
  return Standard_False;	 
}

Standard_Boolean TestTopOpeTools_Trace::Exist
(const tf_value func, Standard_Integer& index)
{ 
  for (Standard_Integer i=myfirstentry; i<=mynbentries; i++)
    if (myfunc.Value(i) == ((long)func)) { index = i; return Standard_True; }
  return Standard_False;
}

const t_flag TestTopOpeTools_Trace::Getflag(const Standard_Integer index)
{ return (t_flag)myflag.Value(index); }

const tf_value TestTopOpeTools_Trace::Getfunc(const Standard_Integer index)
{ return (tf_value)myfunc.Value(index); }

const te_ftyp TestTopOpeTools_Trace::Getftyp(const Standard_Integer index)
{
//JR/Hp :
  Standard_Integer intenum = myftyp.Value(index);
  return (te_ftyp) intenum ;
//  return (te_ftyp) myftyp.Value(index);
}

Standard_Integer TestTopOpeTools_Trace::Add
(const t_flag flag, const tf_value func, const te_ftyp ftyp)
{ 
  Standard_Integer index;
  if ( Exist(flag,index) || ( mynbentries >= mynbmaxentry) ) return 1;
  mynbentries++;
  myflag.SetValue(mynbentries,flag);
  myfunc.SetValue(mynbentries,(long)func);
  myftyp.SetValue(mynbentries,(Standard_Integer)ftyp);
  myfverbose.SetValue(mynbentries,Standard_False);
  return 0;
}
