#pragma once
#include <list>

#ifdef _MDPP_LIB_EXPORT_
#define _MDPP_LIB_API_   __declspec( dllexport ) 
#else
#define _MDPP_LIB_API_   
#endif