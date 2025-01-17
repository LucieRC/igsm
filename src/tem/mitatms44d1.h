/* **************************************************************
*****************************************************************
MITATMS44C.H - object describes physical characteristics of the
	       atmosphere

Modifications:

20060127 - Created by DWK
20080130 - DWK changed include from mittemconsts43.hpp to
           mittemconsts44a.hpp
20080130 - DWK changed include from atms437.h to tigsmatms44a.h
20080130 - DWK changed class MITatms43 to class MITatms44
20080130 - DWK changed Atmosphere43 to Atmosphere44
20110707 - DWK changed include from mittemconsts44a.hpp to 
           mittemconsts44c.hpp
20110707 - DWK changed include from tigsmatms44a.h to 
           tigsmatms44c.h
20150428 - DWK changed include from mittemconsts44c.hpp to
           temigsmconstants.hpp
20150428 - DWK changed include from tigsmatms44c.h to 
           tigsmatms44d1.h                                                                                                                                                       
*****************************************************************
************************************************************** */

#ifndef MITATMS44D_H
#define MITATMS44D_H

// Global constants

#include "temigsmconstants.hpp"

// MITatms44 inherits Atmosphere44

#include "tigsmatms44d1.h"

class MITatms44 : public Atmosphere44
{

  public:

     MITatms44();
  
/* *************************************************************
		 Public Variables
************************************************************* */

     // Daily mean air temperature (degrees C)
//     double dayTair[CYCLE][MAXMDAYS];
     double dayTair[MAXMDAYS];

     double nox;

     // Duration of current rain event (hrs)
//     double rainDuration[CYCLE][MAXMDAYS];
     double rainDuration[MAXMDAYS];

     // Intensity of rain during current event (mm/hr)
//     double rainIntensity[CYCLE][MAXMDAYS];
     double rainIntensity[MAXMDAYS];

};

#endif
