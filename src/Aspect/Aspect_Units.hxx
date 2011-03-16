
#ifndef _Aspect_Units_HeaderFile
#define _Aspect_Units_HeaderFile

/*
       Since Cas.cade version 1.5 ,the default unit LENGTH is MILLIMETER.
#define METER *1.
#define CENTIMETER *0.01
#define TOCENTIMETER(v) (v)*100.
#define FROMCENTIMETER(v) (v)/100.
#define MILLIMETER *0.001
#define TOMILLIMETER(v) (v)*1000.
#define FROMMILLIMETER(v) (v)/1000.
*/

#define METER *1000.
#define CENTIMETER *10.
#define TOCENTIMETER(v) (v)/10.
#define FROMCENTIMETER(v) (v)*10.
#define MILLIMETER *1.
#define TOMILLIMETER(v) v
#define FROMMILLIMETER(v) v

#endif
