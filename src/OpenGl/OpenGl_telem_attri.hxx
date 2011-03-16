#ifndef  OPENGL_TSM_ATTRI_H
#define  OPENGL_TSM_ATTRI_H

extern  TStatus TsmPushAttri( void );
extern  TStatus TsmPopAttri( void );
extern  TStatus TsmSetAttri( Tint, ... );
extern  TStatus TsmGetAttri( Tint, ... );
extern  TStatus TsmPushAttriLight( void );
extern  TStatus TsmPopAttriLight( void );

#endif
