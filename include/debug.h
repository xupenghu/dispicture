#ifndef __DEBUG__H
#define __DEBUG__H


//#define DEBUG

#ifdef DEBUG
#define debug(argc,argv...) ({printf("[%s:%s:%05d]	",__FILE__,__func__,__LINE__);printf(argc,##argv); })  
#else
#define debug() 
#endif








#endif













