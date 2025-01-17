/* *************************************************************
MITELMNTCOHORT44D1.HPP - Container class to hold land cover 
                          cohort characteristics for TEM in the
                          MIT IGSM

20060129 - Created by DWK by modifying telmntcohort437.hpp
20060129 - DWK changed include from temconsts43.hpp to
           nemconsts.hpp
20060129 - DWK changed struct ElmntCohort43 to MITElmntCohort43
20060129 - DWK added double NEMtoppor, double NEMtopdens,
           double NEMtopksat, double NEMnsolc, double NEMrcvlin,
           double NEMrclin, double NEMrcrin, double NEMocin,
           double NEMano3in, double NEManh4in, 
           double NEMdphumin, double NEMpctclay and double NEMpH
20060218 - DWK added int srcCohort
20060422 - DWK added double MDMnpp
20070129 - DWK changed struct MITElmntCohort43 to 
           struct MITElmntCohort44
20080130 - DWK deleted double prevdst10
20080130 - DWK added int standage
20110705 - DWK added long prevchrtarea
20110707 - DWK changed include from nemconsts44a.hpp to
           nemconsts44c.hpp
20140519 - DWK added "yr" variables
20150429 - DWK changed include nemconsts44c.hpp to
           temigsmconstants.hpp
                      
************************************************************* */

#ifndef MITELMNTCOHORT44D_H
#define MITELMNTCOHORT44D_H

#include "temigsmconstants.hpp"
#include "bioms423.hpp"

struct MITElmntCohort44
{      
  // Index for agricultural crop type used for
  //   parameterization
  int agcmnt;

  // Number of growing degree days in croplands
  double aggrowdd;

  // Current value of decomposition parameter in croplands 
  double agkd;

  // Index to indicate whether cohort was in
  //   agriculture during the previous year
//  int agprevstate;

  int agprvstate;

  // Index to indicate whether cohort is in
  //   agriculture during the current year
  int agstate;

  double c2n;

  // Area covered by cohort (square kilometers)
  //   during current year 
  long chrtarea;

  // Index for vegetation community type used for
  //   parameterization
  int cmnt;

  double cneven;

  Biomass convrtflx;

  double cropprveetmx;

  double cropprvleafmx;

  double cropprvpetmx;

  Biomass cropResidue;

  double croptopt;

  // Index for current vegetation type
  int currentveg;

  // Counter of months since disturbance
  int distmnthcnt;

  // Flag to indicate that a disturbance has occurred
  //   in a particular year.  Different values represent
  //   different types of disturbances:
  //   0 = no disturbance
  //   1 = sustained disturbed state
  //         (e.g., conversion to agriculture or urban state)
  //   2 = timber harvest
  //   3 = fire
  int disturbflag;
     
  // Month in which disturbance occurred
  // (e.g. 1 = January, ...., 12 = December)
  int disturbmonth;

  // Maximum monthly estimated evapotranspiration
  //   during the current year
  double eetmx;

  // Index to indicate if crops are fertilized (= 1)
  //   or not (= 0)
  int fertflag;

  // Counter of months since fire disturbance
  int firemnthcnt;

  // Monthly reintroduction of N volatilized in fires
  //   back to the soil (as N deposition)
  double firendep;

  Biomass formPROD10;

  Biomass formPROD100;

  double fprevozone;

  // Fire return interval
  int FRI;

  Biomass initPROD1[CYCLE];
  Biomass initPROD10[10];
  Biomass initPROD100[100];

 // Index to indicate if crops are irrigated (= 1)
  //   or not (= 0)
  int irrgflag;

  // Current value of decomposition parameter in 
  //   natural ecosystems
  double kd;

  // Monthly NPP value from last month of previous year  
  //   (used when TEM/MDM/NEM is coupled to IGSM)
  double MDMnpp;

  double natprveetmx;
  double natprvleafmx;
  double natprvpetmx;
  double natseedC;
  double natseedSTRN;
  double natseedSTON;
  double natsoil;
  double nattopt;
  double natyreet;
  double natyrpet;

  // NEM variables
  
  double NEManh4in[NLVL]; 
  double NEMano3in[NLVL];

  double NEMdphumin[NLVL]; 

  double NEMocin[NLVL];

  double NEMrclin[NLVL]; 
  double NEMrcrin[NLVL];
  double NEMrcvlin[NLVL];

  double NEMnsolc;

//  double NEMtopdens;
//  double NEMtopksat;
//  double NEMtoppor;
  
  double newleafmx;

  double newtopt;

  double nretent;
  double nsretent;
  double nvretent;

  // Maximum monthly potential evapotranspiration 
  //   during current year
  double petmx;

  int potveg;

  // Air temperature that occurred 2 months ago
  double prev2tair;
  
  // Area covered by cohort (square kilometers)
  //   during previous year 
  long prevchrtarea;
  
  // Atmospheric CO2 concentration during the previous month
  double prevco2;

  Biomass prevCropResidue;

  Biomass prevPROD1;
  Biomass prevPROD10;
  Biomass prevPROD100;

  // Snowpack during the previous month
//  double prevspack;

  // Air temperature of the previous month
  double prevtair;

  // Unnormalized relative leaf area of the previous month
  double prevunrmleaf;

  // Value of y[] during previous time step
  double prevy[MAXSTATE];

//  double prod10par;

//  double prod100par;

  int productYear;

  // Crop net primary production during the previous month
  double prvcropnpp;

  // Maximum monthly estimated evapotranspiration
  //   during the previous year
  double prveetmx;

  // Maximum relative leaf area of cohort
  //   during the previous year 
  double prvleafmx;

  // Maximum monthly potential evapotranspiration
  //   during the previous year
  double prvpetmx;


  int qc;


//  double sconvert;

  Biomass sconvrtflx;

  Biomass slash;

//  double slashpar;

  // Source cohort for current cohort
  int srcCohort;

  // Age of cohort
  int standage;
    
  int subtype;

  int tillflag;

  double topt;

  int tqc;

//  double vconvert;

  Biomass vconvrtflx;

//  double vrespar;

  double y[MAXSTATE];
 
  double yragstubC;
  
  double yragstubN;

  double yrcflux;
  
  double yrCH4csmp;
  
  double yrCH4ems;
  
  double yrCH4flx;
  
  double yrCO2dnflx;
  
  double yrCO2nflx;
  
  double yrconvrtC;
  
  double yrconvrtN;
  
  double yrdecayPROD1C;
  
  double yrdecayPROD10C;
  
  double yrdecayPROD100C;

  double yrdecayPROD1N;
  
  double yrdecayPROD10N;
  
  double yrdecayPROD100N;
  
  double yrdecayTOTPRODC;

  double yrdecayTOTPRODN;

  double yreet;
  
  double yrfertn;
  
  double yrfluxResidueC;
  
  double yrfluxResidueN;

  double yrformPROD1C;
  
  double yrformPROD10C;
  
  double yrformPROD100C;

  double yrformPROD1N;
  
  double yrformPROD10N;
  
  double yrformPROD100N;
  
  double yrformResidueC;
  
  double yrformResidueN;

  double yrformTOTPRODC;

  double yrformTOTPRODN;

  double yrfpc;

  double yrgpp;
  
  double yrgpr;
  
//  double yrH2Oyld;  determined by CLM

  double yrimmob;
  
//  double yrineet;
  
  double yringpp;
  
  double yrinnpp;
    
//  double yrirrig; determined by CLM
    
  double yrlai;

  double yrleaf;

  double yrltrfalC;

  double yrltrfalN;
  
  double yrN2flx;
  
  double yrN2Odnflx;
  
  double yrN2Oflx;
  
  double yrN2Onflx;
  
  double yrnce;
  
  double yrnep;
  
  double yrninput;
  
  double yrnlost;
  
  double yrnmin;
   
  double yrnpp;
  
  double yrnrent;
    
  double yrnsrent;

  double yrnvrent;
  
  double yrpet;
  
//  double yrrgrndH2O; determined by CLM 
  
  double yrrh;
  
//  double yrrain; determined by CLM
  
//  double yrrperc; determined by CLM
  
//  double yrrrun; determined by CLM
  
  double yrsconvrtC;

  double yrsconvrtN;
  
//  double yrsgrndH2O; determined by CLM

  double yrslashC;

  double yrslashN;

//  double yrsnowfall; determined by CLM

//  double yrsnowinf; determined by CLM

//  double yrsnowpack; determined by CLM
  
//  double yrsoilavlH2O; determined by CLM

  double yrsoilavlN;

//  double yrsoilC2N;
    
//  double yrsoilmoist; determined by CLM

  double yrsoilorgC;
  
  double yrsoilorgN;

//  double yrsoilpctp; determined by CLM

//  double yrsoilvsm; determined by CLM

//  double yrsperc; determined by CLM
  
//  double yrsrun; determined by CLM
    
  double yrtotalC;
  
  double yrunleaf;

  double yrvconvrtC;
  
  double yrvconvrtN;

  double yrvegC;
    
//  double yrvegC2N;

  double yrveginnup;

  double yrveglup;
  
  double yrvegN;
  
  double yrvegnmobil;

  double yrvegnrsorb;

  double yrvegnup;

  double yrvegrgrowth;

  double yrvegrmaint;

  double yrvegSTON;
  
  double yrvegSTRN;

  double yrvegsup;
  
};

#endif
