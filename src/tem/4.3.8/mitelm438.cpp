/* **************************************************************
*****************************************************************
MITELM438.CPP - Runs TEM for a single grid cell

Modifications:

20060129 - DWK created by modifying telm437.cpp
20060129 - DWK changed include from telm437.h to mitelm437a.h
20060129 - DWK changed TEMelmnt43:: to MITelmnt43::
20060202 - DWK added I_NIRR, I_PAR, I_TAIR, I_PREC, I_RAIN,   
           I_SNWFAL, I_CO2, I_AOT40 to outputTEMmonth() and 
           temwritepred()
20060202 - DWK deleted atmswritemiss() and atmswritepred()
20060422 - DWK added MDMnpp to initializeCohortTEMState(),
           readCohortState(), saveTEMCohortState(), 
           setCohortTEMState(), and writeCohortState()
                                                      
****************************************************************
************************************************************* */

#include<cstdio>

  using std::fscanf;
  using std::FILE;
  using std::printf;

#include<iostream>

  using std::cout;
  using std::cin;
  using std::ios;
  using std::cerr;
  using std::endl;

#include<fstream>

  using std::ifstream;
  using std::ofstream;

#include<iomanip>

  using std::setprecision;
  using std::setw;

#include<cstdlib>

  using std::exit;
  using std::atof;
  using std::atoi;

#include<cmath>

  using std::exp;
  using std::fabs;
  using std::pow;

#include<vector>

  using std::vector;
      
#include<string>
  
  using std::string;


#include "mitelm438.h"

/* *************************************************************
************************************************************* */

MITelmnt43::MITelmnt43()
{

  col = MISSING;
  row = MISSING;
  carea = -999;
  subarea = -999;
  fatalerr = 0;

};

/* *************************************************************
************************************************************* */


/* *************************************************************
************************************************************* */

int MITelmnt43::coregerr( ofstream& rflog1,
                          const string& varname1,
                          const float& col1,
                          const float& row1,
                          const string& varname2,
                          const float& col2,
                          const float& row2 )
{

  int fatalerr = 0;

  if( col1 != col2 || row1 != row2 )
  {
    fatalerr = 1;

    cout << "ERROR:  " << varname1 << " data and ";
    cout << varname2 << "data are not coregistered." << endl;
    cout << "COL = " << col1 << " and ROW = " << row1;
    cout << " in " << varname1 << " data" << endl;
    cout << "COL = " << col2 << " and ROW = " << row2;
    cout << " in " << varname2 << " data" << endl;

    rflog1 << "ERROR:  " << varname1 << " data and ";
    rflog1 << varname2 << "data are not coregistered." << endl;
    rflog1 << "COL = " << col1 << " and ROW = " << row1;
    rflog1 << " in " << varname1 << " data" << endl;
    rflog1 << "COL = " << col2 << " and ROW = " << row2;
    rflog1 << " in " << varname2 << " data" << endl;
  }

  return fatalerr;

};

/* *************************************************************
************************************************************** */


/* *************************************************************
************************************************************* */

int MITelmnt43::equilibrateTEM( const int& pchrt,
                                const double& ptol )
{
  // Run TEM until steady state conditions occur (equilibrium)

  int dyr = 0;
  int dm;


  // Initialize standing stocks of carbon and nitrogen from 
  //   calibration ("ECD") data and fluxes for integrator

  tem.ECDsetODEstate( tem.veg.cmnt, tem.soil.getPSIPLUSC() );

  
  // Set previous value of TEM ODE state variables to the 
  //   current values of TEM ODE state variables for initial
  //   conditions
 
  tem.setPrevState();


  // Initialize all agricultural and wood product pools to 
  //   zero

  tem.ag.resetPROD();
  
  tem.totyr = 0;
  tem.endeq = 0;
  tem.intflag = 0;
  tem.initFlag = 0;
  tem.microbe.nem.firstday = 0;

  // Initialize tem.atms.prevtair for water balance (WBM) and 
  //   soil thermal (STM) calculations
  
  tem.atms.setPREVTAIR( tem.atms.getTAIR() );


  // Initialize tem.atms.prev2tair for water balance  
  //   calculations (WBM)

  tem.atms.setPREV2TAIR( tem.atms.getTAIR() );

  
  // Initialize tem.veg.prvleafmx and tem.veg.prevunrmleaf
  //   for phenology calculations
  
  tem.veg.setPRVLEAFMX( tem.veg.getINITLEAFMX( tem.veg.cmnt ) );
  
  tem.veg.setPREVUNRMLEAF( tem.veg.getUNLEAF12( tem.veg.cmnt ) );


  while( (dyr < tem.runsize) && (tem.endeq < 2) )
  {
    for( dm = 0; dm < CYCLE; ++dm )
    {
      // Pass climate data for particular month to TEM

      tem.atms.setNIRR( climate[mitclm.I_NIRR][dm] );
      tem.atms.setPAR( climate[mitclm.I_PAR][dm] );
      tem.atms.setTAIR( climate[mitclm.I_TAIR][dm] );
      tem.soil.setINEET( initAET[pchrt][dm] );
      tem.soil.setMOIST( initSH2O[pchrt][dm] );
      tem.soil.setSNOWPACK( initSNOWPACK[pchrt][dm] );
      tem.soil.setSURFRUN( initSURFRUN[pchrt][dm] );
      tem.soil.setDRAINAGE( initDRAINAGE[pchrt][dm] );
      tem.atms.setCO2( climate[mitclm.I_CO2][dm] );
      tem.atms.setAOT40( climate[mitclm.I_AOT40][dm] );

      tem.endeq = tem.stepmonth( dyr, 
                                 dm, 
                                 tem.intflag, 
                                 ptol );


      // Save TEM output to telmnt[0].output
      
      outputTEMmonth( pchrt, dm );
    }

    ++dyr;
    ++tem.totyr;


// Check to see if steady state conditions have been reached.

    if( dyr >= tem.strteq && 0 == tem.endeq )
    {
      tem.endeq = tem.testEquilibrium();
    }
  }

  if( tem.totyr >= tem.runsize && tem.endeq < 2 ) 
  { 
    tem.nattempt += 1; 
    tem.initFlag = 0;
  }
  else 
  { 
    tem.initFlag = 1; 
  }

  return tem.nattempt;

};

/* *************************************************************
************************************************************* */


/* *************************************************************
************************************************************** */

void MITelmnt43::getTEMCohortState( const int& pichrt )
{
  int dlyr;
  int dm;
  int i;
  
  tem.veg.setPOTVEG( cohort[pichrt].potveg );

  tem.veg.setCURRENTVEG( cohort[pichrt].currentveg );

  tem.veg.setSUBTYPE( cohort[pichrt].subtype );

  tem.veg.cmnt = cohort[pichrt].cmnt;

  for( i = 0; i < MAXSTATE; ++i )
  {
    tem.setY( cohort[pichrt].y[i], i );
    tem.setPREVY( cohort[pichrt].prevy[i], i );
  }

  tem.ag.cmnt = cohort[pichrt].agcmnt;

  tem.ag.setGROWDD( cohort[pichrt].aggrowdd );

  tem.ag.setKD( cohort[pichrt].agkd );

  tem.ag.prvstate = cohort[pichrt].agprvstate;

  tem.ag.state = cohort[pichrt].agstate;

  tem.veg.setC2N( cohort[pichrt].c2n );
  
  tem.veg.setCNEVEN( cohort[pichrt].cneven );

  tem.ag.setCONVRTFLXC( cohort[pichrt].convrtflx.carbon );
  tem.ag.setCONVRTFLXN( cohort[pichrt].convrtflx.nitrogen );

  tem.ag.setCROPPRVEETMX( cohort[pichrt].cropprveetmx );

  tem.ag.setCROPPRVLEAFMX( cohort[pichrt].cropprvleafmx );

  tem.ag.setCROPPRVPETMX( cohort[pichrt].cropprvpetmx );

  tem.ag.setCROPRESIDUEC( cohort[pichrt].cropResidue.carbon );
  tem.ag.setCROPRESIDUEN( cohort[pichrt].cropResidue.nitrogen );

  tem.ag.setCROPTOPT( cohort[pichrt].croptopt );

  tem.soil.setEETMX( cohort[pichrt].eetmx );

  tem.ag.fertflag = cohort[pichrt].fertflag;                              

  tem.ag.setFORMPROD10C( cohort[pichrt].formPROD10.carbon );
  tem.ag.setFORMPROD10N( cohort[pichrt].formPROD10.nitrogen );

  tem.ag.setFORMPROD100C( cohort[pichrt].formPROD100.carbon );
  tem.ag.setFORMPROD100N( cohort[pichrt].formPROD100.nitrogen );

  tem.veg.setFPREVOZONE( cohort[pichrt].fprevozone );
  
  tem.ag.setFRF( cohort[pichrt].FRF );

  for( dm = 0; dm < CYCLE; ++dm )
  {
    tem.ag.setINITPROD1C( cohort[pichrt].initPROD1[dm].carbon,
                          dm );

    tem.ag.setINITPROD1N( cohort[pichrt].initPROD1[dm].nitrogen,
                          dm ); 
  }
  
  for( i = 0; i < 10; ++i )
  {
    tem.ag.setINITPROD10C( cohort[pichrt].initPROD10[i].carbon, i );
    tem.ag.setINITPROD10N( cohort[pichrt].initPROD10[i].nitrogen, i );
  }
    
  for( i = 0; i < 100; ++i )
  {
    tem.ag.setINITPROD100C( cohort[pichrt].initPROD100[i].carbon, i );
    tem.ag.setINITPROD100N( cohort[pichrt].initPROD100[i].nitrogen, i );
  }

  tem.ag.irrgflag = cohort[pichrt].irrgflag;                              
  
  tem.microbe.setKD( cohort[pichrt].kd );

  tem.ag.setNATPRVEETMX( cohort[pichrt].natprveetmx );

  tem.ag.setNATPRVLEAFMX( cohort[pichrt].natprvleafmx );

  tem.ag.setNATPRVPETMX( cohort[pichrt].natprvpetmx );

  tem.ag.setNATSEEDC( cohort[pichrt].natseedC );

  tem.ag.setNATSEEDSTRN( cohort[pichrt].natseedSTRN );

  tem.ag.setNATSEEDSTON( cohort[pichrt].natseedSTON );

  tem.ag.setNATSOIL( cohort[pichrt].natsoil );

  tem.ag.setNATTOPT( cohort[pichrt].nattopt );

  tem.ag.setNATYREET( cohort[pichrt].natyreet );

  tem.ag.setNATYRPET( cohort[pichrt].natyrpet );

  for( dlyr = 0; dlyr < NLVL; ++dlyr )
  {
    tem.microbe.nem.setANH4IN( cohort[pichrt].NEManh4in[dlyr], 
                               dlyr ); 
  
    tem.microbe.nem.setANO3IN( cohort[pichrt].NEMano3in[dlyr],
                               dlyr );

    tem.microbe.nem.setDPHUMIN( cohort[pichrt].NEMdphumin[dlyr],
                                dlyr ); 

    tem.microbe.nem.setOCIN( cohort[pichrt].NEMocin[dlyr],
                             dlyr );

    tem.microbe.nem.setRCLIN( cohort[pichrt].NEMrclin[dlyr],
                              dlyr ); 
  
    tem.microbe.nem.setRCRIN( cohort[pichrt].NEMrcrin[dlyr],
                              dlyr );
  
    tem.microbe.nem.setRCVLIN( cohort[pichrt].NEMrcvlin[dlyr],
                               dlyr );
  }
  
  tem.microbe.nem.setNONSOLC( cohort[pichrt].NEMnsolc );

  //tem.microbe.nem.setTOPDENS( cohort[pichrt].NEMtopdens );
  
  //tem.microbe.nem.setTOPKSAT( cohort[pichrt].NEMtopksat );
  
  //tem.microbe.nem.setTOPPOR( cohort[pichrt].NEMtoppor );

  tem.veg.setNEWLEAFMX( cohort[pichrt].newleafmx );

  tem.veg.setNEWTOPT( cohort[pichrt].newtopt );

  tem.ag.setNRETENT( cohort[pichrt].nretent );

  tem.ag.setNSRETENT( cohort[pichrt].nsretent );

  tem.ag.setNVRETENT( cohort[pichrt].nvretent );

  tem.atms.setPETMX( cohort[pichrt].petmx );

  tem.atms.setPREV2TAIR( cohort[pichrt].prev2tair );

  tem.atms.setPREVCO2( cohort[pichrt].prevco2 );

  tem.ag.setPREVCROPRESIDUEC( cohort[pichrt].prevCropResidue.carbon );
  tem.ag.setPREVCROPRESIDUEN( cohort[pichrt].prevCropResidue.nitrogen );

  tem.ag.setPREVPROD1C( cohort[pichrt].prevPROD1.carbon );
  tem.ag.setPREVPROD1N( cohort[pichrt].prevPROD1.nitrogen );

  tem.ag.setPREVPROD10C( cohort[pichrt].prevPROD10.carbon );
  tem.ag.setPREVPROD10N( cohort[pichrt].prevPROD10.nitrogen );

  tem.ag.setPREVPROD100C( cohort[pichrt].prevPROD100.carbon );
  tem.ag.setPREVPROD100N( cohort[pichrt].prevPROD100.nitrogen );

  tem.soil.setPREVSPACK( cohort[pichrt].prevspack );

  tem.atms.setPREVTAIR( cohort[pichrt].prevtair );

  tem.veg.setPREVUNRMLEAF( cohort[pichrt].prevunrmleaf );

  tem.ag.setPROD10PAR( cohort[pichrt].prod10par ); 

  tem.ag.setPROD100PAR( cohort[pichrt].prod100par ); 

  tem.ag.setPRODUCTYEAR( cohort[pichrt].productYear );
  
  tem.ag.setPRVCROPNPP( cohort[pichrt].prvcropnpp );

  tem.soil.setPRVEETMX( cohort[pichrt].prveetmx );

  tem.veg.setPRVLEAFMX( cohort[pichrt].prvleafmx );

  tem.atms.setPRVPETMX( cohort[pichrt].prvpetmx );

  tem.ag.setSCONVERT( cohort[pichrt].sconvert ); 
  
  tem.ag.setSCONVRTFLXC( cohort[pichrt].sconvrtflx.carbon );
  tem.ag.setSCONVRTFLXN( cohort[pichrt].sconvrtflx.nitrogen );

  tem.ag.setSLASHC( cohort[pichrt].slash.carbon );
  tem.ag.setSLASHN( cohort[pichrt].slash.nitrogen );

  tem.ag.setSLASHPAR( cohort[pichrt].slashpar ); 
   
  tem.ag.tillflag = cohort[pichrt].tillflag;                           

  tem.veg.setTOPT( cohort[pichrt].topt );

  tem.ag.setVCONVERT( cohort[pichrt].vconvert ); 

  tem.ag.setVCONVRTFLXC( cohort[pichrt].vconvrtflx.carbon );
  tem.ag.setVCONVRTFLXN( cohort[pichrt].vconvrtflx.nitrogen );

  tem.ag.setVRESPAR( cohort[pichrt].vrespar ); 

  tem.veg.yrltrc = cohort[pichrt].yrltrc;
  tem.veg.yrltrn = cohort[pichrt].yrltrn;  	
	
};

/* *************************************************************
************************************************************** */


/* *************************************************************
************************************************************** */

void MITelmnt43::initializeCohortTEMState( const int& pichrt )
{
  int dlyr;
  int dm;
  int i;

  for( i = 0; i < MAXSTATE; ++i )
  {
    cohort[pichrt].y[i] = MISSING;
    cohort[pichrt].prevy[i] = MISSING;
  }

  cohort[pichrt].aggrowdd = MISSING;

  cohort[pichrt].agkd = MISSING;

  cohort[pichrt].c2n = MISSING;
  
  cohort[pichrt].cneven = MISSING;
  
  cohort[pichrt].convrtflx.carbon = MISSING;
  cohort[pichrt].convrtflx.nitrogen = MISSING;

  cohort[pichrt].cropprveetmx = MISSING;

  cohort[pichrt].cropprvleafmx = MISSING;

  cohort[pichrt].cropprvpetmx = MISSING;

  cohort[pichrt].cropResidue.carbon = MISSING;
  cohort[pichrt].cropResidue.nitrogen = MISSING;

  cohort[pichrt].croptopt = MISSING;

  cohort[pichrt].eetmx = MISSING;
                           
  cohort[pichrt].formPROD10.carbon = MISSING;
  cohort[pichrt].formPROD10.nitrogen = MISSING;
  
  cohort[pichrt].formPROD100.carbon = MISSING;
  cohort[pichrt].formPROD100.nitrogen = MISSING;

  cohort[pichrt].fprevozone = MISSING;
     
  for( dm = 0; dm < CYCLE; ++dm )
  {
    cohort[pichrt].initPROD1[dm].carbon = MISSING;
    cohort[pichrt].initPROD1[dm].nitrogen = MISSING;
  }
  
  for( i = 0; i < 10; ++i )
  {
    cohort[pichrt].initPROD10[i].carbon = MISSING; 
    cohort[pichrt].initPROD10[i].nitrogen = MISSING;
  }
    
  for( i = 0; i < 100; ++i )
  {
    cohort[pichrt].initPROD100[i].carbon = MISSING;
    cohort[pichrt].initPROD100[i].nitrogen = MISSING;
  }                            
  
  cohort[pichrt].kd = MISSING;

  cohort[pichrt].MDMnpp = MISSING;

  cohort[pichrt].natprveetmx = MISSING;

  cohort[pichrt].natprvleafmx = MISSING;

  cohort[pichrt].natprvpetmx = MISSING;

  cohort[pichrt].natseedC = MISSING;

  cohort[pichrt].natseedSTRN = MISSING;

  cohort[pichrt].natseedSTON = MISSING;

  cohort[pichrt].natsoil = MISSING;

  cohort[pichrt].nattopt = MISSING;

  cohort[pichrt].natyreet = MISSING;

  cohort[pichrt].natyrpet= MISSING;

  for( dlyr = 0; dlyr < NLVL; ++dlyr )
  {
    cohort[pichrt].NEManh4in[dlyr] = MISSING; 
  
    cohort[pichrt].NEMano3in[dlyr] = MISSING;

    cohort[pichrt].NEMdphumin[dlyr] = MISSING; 

    cohort[pichrt].NEMocin[dlyr] = MISSING;

    cohort[pichrt].NEMrclin[dlyr] = MISSING; 
  
    cohort[pichrt].NEMrcrin[dlyr] = MISSING;
  
    cohort[pichrt].NEMrcvlin[dlyr] = MISSING;
  }
  
  cohort[pichrt].NEMnsolc = MISSING;

  // cohort[pichrt].NEMtopdens prescribed from soil layer file
  
  // cohort[pichrt].NEMtopksat prescribed from soil layer file
  
  // cohort[pichrt].NEMtoppor prescribed from soil layer file

  cohort[pichrt].newleafmx = MISSING;

  cohort[pichrt].newtopt = MISSING;

  cohort[pichrt].nretent = MISSING;

  cohort[pichrt].nsretent = MISSING;

  cohort[pichrt].nvretent = MISSING;

  cohort[pichrt].petmx = MISSING;

  cohort[pichrt].prev2tair = MISSING;

  cohort[pichrt].prevco2 = MISSING;

  cohort[pichrt].prevCropResidue.carbon = MISSING;
  cohort[pichrt].prevCropResidue.nitrogen = MISSING;
  
  cohort[pichrt].prevPROD1.carbon = MISSING; 
  cohort[pichrt].prevPROD1.nitrogen = MISSING;
  
  cohort[pichrt].prevPROD10.carbon = MISSING; 
  cohort[pichrt].prevPROD10.nitrogen = MISSING;
  
  cohort[pichrt].prevPROD100.carbon = MISSING;
  cohort[pichrt].prevPROD100.nitrogen = MISSING;

  cohort[pichrt].prevspack = MISSING;

  cohort[pichrt].prevtair = MISSING;
  
  cohort[pichrt].prevunrmleaf = MISSING;

  cohort[pichrt].productYear = -99;
  
  cohort[pichrt].prvcropnpp = MISSING;

  cohort[pichrt].prveetmx = MISSING;

  cohort[pichrt].prvleafmx = MISSING;

  cohort[pichrt].prvpetmx = MISSING;

  cohort[pichrt].qc = -99;

  cohort[pichrt].sconvrtflx.carbon = MISSING;
  cohort[pichrt].sconvrtflx.nitrogen = MISSING;

  cohort[pichrt].slash.carbon = MISSING;
  cohort[pichrt].slash.nitrogen = MISSING;                       

  cohort[pichrt].topt = MISSING;

  cohort[pichrt].tqc = -99;

  cohort[pichrt].vconvrtflx.carbon = MISSING;
  cohort[pichrt].vconvrtflx.nitrogen = MISSING; 

  cohort[pichrt].yrltrc = MISSING;
  cohort[pichrt].yrltrn = MISSING;  	
	
};

/* *************************************************************
************************************************************** */


/* *************************************************************
************************************************************* */

void MITelmnt43::outputTEMmonth( const int& pchrt,
                                 const int& pdm )
{

  // Ecosystem carbon pools determined in integrator

  output[tem.I_VEGC][pchrt][pdm] = tem.getY( tem.I_VEGC );

  output[tem.I_SOLC][pchrt][pdm] = tem.getY( tem.I_SOLC );

  // Ecosystem nitrogen pools determined in integrator

  output[tem.I_STRN][pchrt][pdm] = tem.getY( tem.I_STRN );

  output[tem.I_STON][pchrt][pdm] = tem.getY( tem.I_STON );

  output[tem.I_SOLN][pchrt][pdm] = tem.getY( tem.I_SOLN );
      
  output[tem.I_AVLN][pchrt][pdm] = tem.getY( tem.I_AVLN );

 
  // Ecosystem water pools determined in integrator

  output[tem.I_AVLW][pchrt][pdm] = tem.soil.getAVLH2O();

  output[tem.I_SM][pchrt][pdm] = tem.soil.getMOIST();

  output[tem.I_VSM][pchrt][pdm] = tem.soil.getVSM();

  output[tem.I_PCTP][pchrt][pdm] = tem.soil.getPCTP();


  // Monthly phenology determined in integrator

  output[tem.I_UNRMLF][pchrt][pdm] = tem.getY( tem.I_UNRMLF );

  output[tem.I_LEAF][pchrt][pdm] = tem.getY( tem.I_LEAF );

  output[tem.I_LAI][pchrt][pdm] = tem.getY( tem.I_LAI );

  output[tem.I_FPC][pchrt][pdm] = tem.getY( tem.I_FPC );

  // Monthly carbon fluxes in ecosystems determined in integrator

  output[tem.I_INGPP][pchrt][pdm] = tem.getY( tem.I_INGPP );

  output[tem.I_GPP][pchrt][pdm] = tem.getY( tem.I_GPP );

  output[tem.I_FOZONE][pchrt][pdm] = tem.getY( tem.I_FOZONE );

  output[tem.I_FINDOZONE][pchrt][pdm] = tem.getY( tem.I_FINDOZONE );

  output[tem.I_INNPP][pchrt][pdm] = tem.getY( tem.I_INNPP );

  output[tem.I_NPP][pchrt][pdm] = tem.getY( tem.I_NPP );

  output[tem.I_GPR][pchrt][pdm] = tem.getY( tem.I_GPR );

  output[tem.I_RVMNT][pchrt][pdm] = tem.getY( tem.I_RVMNT );

  output[tem.I_RVGRW][pchrt][pdm] = tem.getY( tem.I_RVGRW );

  output[tem.I_LTRC][pchrt][pdm] = tem.getY( tem.I_LTRC );

  output[tem.I_RH][pchrt][pdm] = tem.getY( tem.I_RH );


  // Monthly nitrogen fluxes in ecosystems determined in 
  //   integrator

  output[tem.I_NINP][pchrt][pdm] = tem.getY( tem.I_NINP );

  output[tem.I_AGFRTN][pchrt][pdm] = tem.getY( tem.I_AGFRTN );

  output[tem.I_INNUP][pchrt][pdm] = tem.getY( tem.I_INNUP );

  output[tem.I_VNUP][pchrt][pdm] = tem.getY( tem.I_VNUP );

  output[tem.I_VSUP][pchrt][pdm] = tem.getY( tem.I_VSUP );

  output[tem.I_VLUP][pchrt][pdm] = tem.getY( tem.I_VLUP );

  output[tem.I_VNMBL][pchrt][pdm] = tem.getY( tem.I_VNMBL );

  output[tem.I_VNRSRB][pchrt][pdm] = tem.getY( tem.I_VNRSRB );

  output[tem.I_LTRN][pchrt][pdm] = tem.getY( tem.I_LTRN );

  output[tem.I_MNUP][pchrt][pdm] = tem.getY( tem.I_MNUP );

  output[tem.I_NMIN][pchrt][pdm] = tem.getY( tem.I_NMIN );

  output[tem.I_NLST][pchrt][pdm] = tem.getY( tem.I_NLST );


  // Monthly water fluxes in ecosystems

  output[tem.I_AGIRRIG][pchrt][pdm] = ZERO;

  output[tem.I_INEET][pchrt][pdm] = tem.soil.getINEET();

  output[tem.I_EET][pchrt][pdm] = tem.soil.getEET();


  // Other ecosystem carbon pools

  output[tem.I_TOTEC][pchrt][pdm] = tem.ag.getTOTEC();

  output[tem.I_TOTC][pchrt][pdm] = tem.getTOTALC();

  // Other ecosystem nitrogen pools

  output[tem.I_VEGN][pchrt][pdm] = tem.veg.getVEGN();


  // Other ecosystem water pools
  
  output[tem.I_SNWPCK][pchrt][pdm] = tem.soil.getSNOWPACK();

  // Other monthly carbon fluxes in ecosystems

  output[tem.I_NEP][pchrt][pdm] = tem.getNEP();

  output[tem.I_NCE][pchrt][pdm] = tem.getNCE();

  output[tem.I_CH4EMS][pchrt][pdm] = tem.soil.getCH4EMISS();

  output[tem.I_CH4CSMP][pchrt][pdm] = tem.soil.getCH4CONSUMP();

  output[tem.I_CH4FLX][pchrt][pdm] = tem.soil.getCH4FLUX();

  output[tem.I_CO2NFLX][pchrt][pdm] = tem.soil.getCO2NFLUX();

  output[tem.I_CO2DNFLX][pchrt][pdm] = tem.soil.getCO2DNFLUX();

  output[tem.I_N2OFLX][pchrt][pdm] = tem.soil.getN2OFLUX();

  output[tem.I_N2ONFLX][pchrt][pdm] = tem.soil.getN2ONFLUX();

  output[tem.I_N2ODNFLX][pchrt][pdm] = tem.soil.getN2ODNFLUX();

  output[tem.I_N2FLX][pchrt][pdm] = tem.soil.getN2FLUX();


  // Other monthly water fluxes in ecosystems

  output[tem.I_PET][pchrt][pdm] = tem.atms.getPET();

  
  // Carbon in Human product pools

  output[tem.I_AGPRDC][pchrt][pdm] = tem.ag.getPROD1C();

  output[tem.I_PROD10C][pchrt][pdm] = tem.ag.getPROD10C();

  output[tem.I_PROD100C][pchrt][pdm] = tem.ag.getPROD100C();

  output[tem.I_TOTPRDC][pchrt][pdm] = tem.ag.getTOTPRODC();

  // Carbon in crop residue pool

  output[tem.I_RESIDC][pchrt][pdm] = tem.ag.getCROPRESIDUEC();

  output[tem.I_AGSTUBC][pchrt][pdm] = tem.ag.getSTUBBLEC();

  // Nitrogen in Human product pools

  output[tem.I_AGPRDN][pchrt][pdm] = tem.ag.getPROD1N();

  output[tem.I_PROD10N][pchrt][pdm] = tem.ag.getPROD10N();

  output[tem.I_PROD100N][pchrt][pdm] = tem.ag.getPROD100N();

  output[tem.I_TOTPRDN][pchrt][pdm] = tem.ag.getTOTPRODN();

  // Nitrogen in crop residue pool

  output[tem.I_RESIDN][pchrt][pdm] = tem.ag.getCROPRESIDUEN();

  output[tem.I_AGSTUBN][pchrt][pdm] = tem.ag.getSTUBBLEN();

  // Monthly carbon fluxes associated with
  //  agricultural conversion

  output[tem.I_CNVRTC][pchrt][pdm] = tem.ag.getCONVRTFLXC();

  output[tem.I_VCNVRTC][pchrt][pdm] = tem.ag.getVCONVRTFLXC();

  output[tem.I_SCNVRTC][pchrt][pdm] = tem.ag.getSCONVRTFLXC();

  output[tem.I_SLASHC][pchrt][pdm] = tem.ag.getSLASHC();

  output[tem.I_CFLX][pchrt][pdm] = tem.ag.getCFLUX();

  // Monthly nitrogen fluxes associated with
  //  agricultural conversion

  output[tem.I_CNVRTN][pchrt][pdm] = tem.ag.getCONVRTFLXN();

  output[tem.I_VCNVRTN][pchrt][pdm] = tem.ag.getVCONVRTFLXN();

  output[tem.I_SCNVRTN][pchrt][pdm] = tem.ag.getSCONVRTFLXN();

  output[tem.I_SLASHN][pchrt][pdm] = tem.ag.getSLASHN();

  output[tem.I_NRETNT][pchrt][pdm] = tem.ag.getNRETENT();

  output[tem.I_NVRTNT][pchrt][pdm] = tem.ag.getNVRETENT();

  output[tem.I_NSRTNT][pchrt][pdm] = tem.ag.getNSRETENT();

  // Monthly carbon and nitrogen fluxes from agricultural
  //   ecosystems

  output[tem.I_AGFPRDC][pchrt][pdm] = tem.ag.getCROPPRODC();
  output[tem.I_AGFPRDN][pchrt][pdm] = tem.ag.getCROPPRODN();

  output[tem.I_FRESIDC][pchrt][pdm] = tem.ag.getFORMCROPRESIDUEC();
  output[tem.I_FRESIDN][pchrt][pdm] = tem.ag.getFORMCROPRESIDUEN();

  output[tem.I_AGPRDFC][pchrt][pdm] = tem.ag.getPROD1DECAYC();
  output[tem.I_AGPRDFN][pchrt][pdm] = tem.ag.getPROD1DECAYN();

  output[tem.I_RESIDFC][pchrt][pdm] = tem.ag.getCROPRESIDUEFLXC();
  output[tem.I_RESIDFN][pchrt][pdm] = tem.ag.getCROPRESIDUEFLXN();


  // Monthly carbon and nitrogen fluxes from products

  output[tem.I_PRDF10C][pchrt][pdm] = tem.ag.getFORMPROD10C();
  output[tem.I_PRDF10N][pchrt][pdm] = tem.ag.getFORMPROD10N();

  output[tem.I_PRD10FC][pchrt][pdm] = tem.ag.getPROD10DECAYC();
  output[tem.I_PRD10FN][pchrt][pdm] = tem.ag.getPROD10DECAYN();

  output[tem.I_PRDF100C][pchrt][pdm] = tem.ag.getFORMPROD100C();
  output[tem.I_PRDF100N][pchrt][pdm] = tem.ag.getFORMPROD100N();

  output[tem.I_PRD100FC][pchrt][pdm] = tem.ag.getPROD100DECAYC();
  output[tem.I_PRD100FN][pchrt][pdm] = tem.ag.getPROD100DECAYN();

  output[tem.I_TOTFPRDC][pchrt][pdm] = tem.ag.getFORMTOTPRODC();
  output[tem.I_TOTFPRDN][pchrt][pdm] = tem.ag.getFORMTOTPRODN();

  output[tem.I_TOTPRDFC][pchrt][pdm] = tem.ag.getTOTPRODDECAYC();
  output[tem.I_TOTPRDFN][pchrt][pdm] = tem.ag.getTOTPRODDECAYN();

  //  Output agricultural area-specific vs natural area-specific
  //    results
  
  if( 0 == tem.ag.state )
  {
    output[tem.I_CROPC][pchrt][pdm] = ZERO;
    output[tem.I_NATVEGC][pchrt][pdm] = tem.getY( tem.I_VEGC );

    output[tem.I_CROPN][pchrt][pdm] = ZERO;
    output[tem.I_NATVEGN][pchrt][pdm] = tem.veg.getVEGN();

    output[tem.I_CSTRN][pchrt][pdm] = ZERO;
    output[tem.I_NATSTRN][pchrt][pdm] = ZERO;

    output[tem.I_CSTON][pchrt][pdm] = ZERO;
    output[tem.I_NATSTON][pchrt][pdm] = tem.getY( tem.I_STON );

    output[tem.I_CROPULF][pchrt][pdm] = ZERO;
    output[tem.I_NATULF][pchrt][pdm] = tem.getY( tem.I_UNRMLF );

    output[tem.I_CROPLEAF][pchrt][pdm] = ZERO;
    output[tem.I_NATLEAF][pchrt][pdm] = tem.getY( tem.I_LEAF );

    output[tem.I_CROPLAI][pchrt][pdm] = ZERO;
    output[tem.I_NATLAI][pchrt][pdm] = tem.getY( tem.I_LAI );

    output[tem.I_CROPFPC][pchrt][pdm] = ZERO;
    output[tem.I_NATFPC][pchrt][pdm] = tem.getY( tem.I_FPC );

    output[tem.I_AGINGPP][pchrt][pdm] = ZERO;
    output[tem.I_NATINGPP][pchrt][pdm] = tem.getY( tem.I_INGPP );

    output[tem.I_AGGPP][pchrt][pdm] = ZERO;
    output[tem.I_NATGPP][pchrt][pdm] = tem.getY( tem.I_GPP );

    output[tem.I_AGINNPP][pchrt][pdm] = ZERO;
    output[tem.I_NATINNPP][pchrt][pdm] = tem.getY( tem.I_INNPP );

    output[tem.I_AGNPP][pchrt][pdm] = ZERO;
    output[tem.I_NATNPP][pchrt][pdm] = tem.getY( tem.I_NPP );

    output[tem.I_AGGPR][pchrt][pdm] = ZERO;
    output[tem.I_NATGPR][pchrt][pdm] = tem.getY( tem.I_GPR );

    output[tem.I_AGRVMNT][pchrt][pdm] = ZERO;
    output[tem.I_NATRVMNT][pchrt][pdm] = tem.getY( tem.I_RVMNT );

    output[tem.I_AGRVGRW][pchrt][pdm] = ZERO;
    output[tem.I_NATRVGRW][pchrt][pdm] = tem.getY( tem.I_RVGRW );

    output[tem.I_AGLTRC][pchrt][pdm] = ZERO;
    output[tem.I_NATLTRC][pchrt][pdm] = tem.getY( tem.I_RVGRW );

    output[tem.I_AGINNUP][pchrt][pdm] = ZERO;
    output[tem.I_NATINNUP][pchrt][pdm] = tem.getY( tem.I_INNUP );

    output[tem.I_AGVNUP][pchrt][pdm] = ZERO;
    output[tem.I_NATVNUP][pchrt][pdm] = tem.getY( tem.I_VNUP );

    output[tem.I_AGVSUP][pchrt][pdm] = ZERO;
    output[tem.I_NATVSUP][pchrt][pdm] = tem.getY( tem.I_VSUP );

    output[tem.I_AGVLUP][pchrt][pdm] = ZERO;
    output[tem.I_NATVLUP][pchrt][pdm] = tem.getY( tem.I_VLUP );

    output[tem.I_AGVNMBL][pchrt][pdm] = ZERO;
    output[tem.I_NATVNMBL][pchrt][pdm] = tem.getY( tem.I_VNMBL );

    output[tem.I_AGVNRSRB][pchrt][pdm] = ZERO;
    output[tem.I_NVNRSRB][pchrt][pdm] = tem.getY( tem.I_VNRSRB );

    output[tem.I_AGLTRN][pchrt][pdm] = ZERO;
    output[tem.I_NATLTRN][pchrt][pdm] = tem.getY( tem.I_LTRN );
  }
  else 
  {
    output[tem.I_CROPC][pchrt][pdm] = tem.getY( tem.I_VEGC );
    output[tem.I_NATVEGC][pchrt][pdm] = ZERO;

    output[tem.I_CROPN][pchrt][pdm] = tem.veg.getVEGN();
    output[tem.I_NATVEGN][pchrt][pdm] = ZERO;

    output[tem.I_CSTRN][pchrt][pdm] = tem.getY( tem.I_STRN );
    output[tem.I_NATSTRN][pchrt][pdm] = ZERO;

    output[tem.I_CSTON][pchrt][pdm] = tem.getY( tem.I_STON );
    output[tem.I_NATSTON][pchrt][pdm] = ZERO;

    output[tem.I_CROPULF][pchrt][pdm] = tem.getY( tem.I_UNRMLF );
    output[tem.I_NATULF][pchrt][pdm] = ZERO;

    output[tem.I_CROPLEAF][pchrt][pdm] = tem.getY( tem.I_LEAF );
    output[tem.I_NATLEAF][pchrt][pdm] = ZERO;

    output[tem.I_CROPLAI][pchrt][pdm] = tem.getY( tem.I_LAI );
    output[tem.I_NATLAI][pchrt][pdm] = ZERO;

    output[tem.I_CROPFPC][pchrt][pdm] = tem.getY( tem.I_FPC );
    output[tem.I_NATFPC][pchrt][pdm] = ZERO;

    output[tem.I_AGINGPP][pchrt][pdm] = tem.getY( tem.I_INGPP );
    output[tem.I_NATINGPP][pchrt][pdm] = ZERO;

    output[tem.I_AGGPP][pchrt][pdm] = tem.getY( tem.I_GPP );
    output[tem.I_NATGPP][pchrt][pdm] = ZERO;

    output[tem.I_AGINNPP][pchrt][pdm] = tem.getY( tem.I_INNPP );
    output[tem.I_NATINNPP][pchrt][pdm] = ZERO;

    output[tem.I_AGNPP][pchrt][pdm] = tem.getY( tem.I_NPP );
    output[tem.I_NATNPP][pchrt][pdm] = ZERO;

    output[tem.I_AGGPR][pchrt][pdm] = tem.getY( tem.I_GPR );
    output[tem.I_NATGPR][pchrt][pdm] = ZERO;

    output[tem.I_AGRVMNT][pchrt][pdm] = tem.getY( tem.I_RVMNT );
    output[tem.I_NATRVMNT][pchrt][pdm] = ZERO;

    output[tem.I_AGRVGRW][pchrt][pdm] = tem.getY( tem.I_RVGRW );
    output[tem.I_NATRVGRW][pchrt][pdm] = ZERO;

    output[tem.I_AGLTRC][pchrt][pdm] = tem.getY( tem.I_LTRC );
    output[tem.I_NATLTRC][pchrt][pdm] = ZERO;

    output[tem.I_AGINNUP][pchrt][pdm] = tem.getY( tem.I_INNUP );
    output[tem.I_NATINNUP][pchrt][pdm] = ZERO;

    output[tem.I_AGVNUP][pchrt][pdm] = tem.getY( tem.I_VNUP );
    output[tem.I_NATVNUP][pchrt][pdm] = ZERO;

    output[tem.I_AGVSUP][pchrt][pdm] = tem.getY( tem.I_VSUP );
    output[tem.I_NATVSUP][pchrt][pdm] = ZERO;

    output[tem.I_AGVLUP][pchrt][pdm] = tem.getY( tem.I_VLUP );
    output[tem.I_NATVLUP][pchrt][pdm] = ZERO;

    output[tem.I_AGVNMBL][pchrt][pdm] = tem.getY( tem.I_VNMBL );
    output[tem.I_NATVNMBL][pchrt][pdm] = ZERO;

    output[tem.I_AGVNRSRB][pchrt][pdm] = tem.getY( tem.I_VNRSRB );
    output[tem.I_NVNRSRB][pchrt][pdm] = ZERO;

    output[tem.I_AGLTRN][pchrt][pdm] = tem.getY( tem.I_LTRN );
    output[tem.I_NATLTRN][pchrt][pdm] = ZERO;
  }

  // Climate variables
  
  output[tem.I_NIRR][pchrt][pdm] = tem.atms.getPAR(); 
  output[tem.I_PAR][pchrt][pdm] = tem.atms.getPAR(); 
  output[tem.I_TAIR][pchrt][pdm] = tem.atms.getTAIR(); 
  output[tem.I_PREC][pchrt][pdm] = tem.atms.getPREC(); 
  output[tem.I_SRFRUN][pchrt][pdm] = tem.soil.getSURFRUN(); 
  output[tem.I_DRAIN][pchrt][pdm] = tem.soil.getDRAINAGE();   
  output[tem.I_CO2][pchrt][pdm] = tem.atms.getCO2();
  output[tem.I_AOT40][pchrt][pdm] = tem.atms.getAOT40();  

};

/* *************************************************************
************************************************************* */


/* *************************************************************
************************************************************** */

void MITelmnt43::readCohortState( ifstream& ifstate, 
                                  const int& pichrt )
{
  int i;
  int dlyr;
  int dm;
  float dumflt;
  int dumint;
  
  ifstate >> dumflt;    // Longitude of element
  ifstate >> dumflt;    // Latitude of element

  ifstate >> dumint;   // ichrt+1
  
  ifstate >> cohort[pichrt].srcCohort;
  ifstate >> cohort[pichrt].chrtarea;
  ifstate >> cohort[pichrt].potveg;
  ifstate >> cohort[pichrt].currentveg;
  ifstate >> cohort[pichrt].subtype;
  ifstate >> cohort[pichrt].cmnt;

 	
  for( i = 0; i < MAXSTATE; ++i )
  {
    ifstate >> cohort[pichrt].y[i];
    ifstate >> cohort[pichrt].prevy[i];
  }

  ifstate >> cohort[pichrt].agcmnt;

  ifstate >> cohort[pichrt].aggrowdd;

  ifstate >> cohort[pichrt].agkd;

  ifstate >> cohort[pichrt].agprvstate;

  ifstate >> cohort[pichrt].agstate;

  ifstate >> cohort[pichrt].c2n;
  
  ifstate >> cohort[pichrt].cneven;

  ifstate >> cohort[pichrt].convrtflx.carbon;
  ifstate >> cohort[pichrt].convrtflx.nitrogen;

  ifstate >> cohort[pichrt].cropprveetmx;

  ifstate >> cohort[pichrt].cropprvleafmx;

  ifstate >> cohort[pichrt].cropprvpetmx;

  ifstate >> cohort[pichrt].cropResidue.carbon;
  ifstate >> cohort[pichrt].cropResidue.nitrogen;

  ifstate >> cohort[pichrt].croptopt;

  ifstate >> cohort[pichrt].eetmx;

  ifstate >> cohort[pichrt].fertflag;                              

  ifstate >> cohort[pichrt].formPROD10.carbon;
  ifstate >> cohort[pichrt].formPROD10.nitrogen;

  ifstate >> cohort[pichrt].formPROD100.carbon;
  ifstate >> cohort[pichrt].formPROD100.nitrogen;

  ifstate >> cohort[pichrt].fprevozone;

  ifstate >> cohort[pichrt].FRF;

  for( dm = 0; dm < CYCLE; ++dm )
  {  
    ifstate >> cohort[pichrt].initPROD1[dm].carbon;
    ifstate >> cohort[pichrt].initPROD1[dm].nitrogen;
  }
  
  for( i = 0; i < 10; ++i )
  {
    ifstate >> cohort[pichrt].initPROD10[i].carbon;
    ifstate >> cohort[pichrt].initPROD10[i].nitrogen;
  }
    
  for( i = 0; i < 100; ++i )
  {
    ifstate >> cohort[pichrt].initPROD100[i].carbon;
    ifstate >> cohort[pichrt].initPROD100[i].nitrogen;
  }

  ifstate >> cohort[pichrt].irrgflag;                              
  
  ifstate >> cohort[pichrt].kd;

  ifstate >> cohort[pichrt].MDMnpp;

  ifstate >> cohort[pichrt].natprveetmx;

  ifstate >> cohort[pichrt].natprvleafmx;

  ifstate >> cohort[pichrt].natprvpetmx;

  ifstate >> cohort[pichrt].natseedC;

  ifstate >> cohort[pichrt].natseedSTRN;

  ifstate >> cohort[pichrt].natseedSTON;

  ifstate >> cohort[pichrt].natsoil;

  ifstate >> cohort[pichrt].nattopt;

  ifstate >> cohort[pichrt].natyreet;

  ifstate >> cohort[pichrt].natyrpet;

  for( dlyr = 0; dlyr < NLVL; ++dlyr )
  {
    ifstate >> cohort[pichrt].NEManh4in[dlyr]; 
  
    ifstate >> cohort[pichrt].NEMano3in[dlyr];

    ifstate >> cohort[pichrt].NEMdphumin[dlyr]; 

    ifstate >> cohort[pichrt].NEMocin[dlyr];

    ifstate >> cohort[pichrt].NEMrclin[dlyr]; 
  
    ifstate >> cohort[pichrt].NEMrcrin[dlyr];
  
    ifstate >> cohort[pichrt].NEMrcvlin[dlyr];
  }

  ifstate >> cohort[pichrt].NEMnsolc;
  
  //ifstate >> cohort[pichrt].NEMtopdens;
  
  //ifstate >> cohort[pichrt].NEMtopksat;
  
  //ifstate >> cohort[pichrt].NEMtoppor;

  ifstate >> cohort[pichrt].newleafmx;

  ifstate >> cohort[pichrt].newtopt;

  ifstate >> cohort[pichrt].nretent;

  ifstate >> cohort[pichrt].nsretent;

  ifstate >> cohort[pichrt].nvretent;

  ifstate >> cohort[pichrt].petmx;

  ifstate >> cohort[pichrt].prev2tair;

  ifstate >> cohort[pichrt].prevco2;

  ifstate >> cohort[pichrt].prevCropResidue.carbon;
  ifstate >> cohort[pichrt].prevCropResidue.nitrogen;

  ifstate >> cohort[pichrt].prevPROD1.carbon;
  ifstate >> cohort[pichrt].prevPROD1.nitrogen;

  ifstate >> cohort[pichrt].prevPROD10.carbon;
  ifstate >> cohort[pichrt].prevPROD10.nitrogen;

  ifstate >> cohort[pichrt].prevPROD100.carbon;
  ifstate >> cohort[pichrt].prevPROD100.nitrogen;

  ifstate >> cohort[pichrt].prevspack;

  ifstate >> cohort[pichrt].prevtair;

  ifstate >> cohort[pichrt].prevunrmleaf;
  
  ifstate >> cohort[pichrt].prod10par; 

  ifstate >> cohort[pichrt].prod100par; 

  ifstate >> cohort[pichrt].productYear;

  ifstate >> cohort[pichrt].prvchrtarea;
  
  ifstate >> cohort[pichrt].prvcropnpp;

  ifstate >> cohort[pichrt].prveetmx;

  ifstate >> cohort[pichrt].prvleafmx;

  ifstate >> cohort[pichrt].prvpetmx;

  ifstate >> cohort[pichrt].qc;

  ifstate >> cohort[pichrt].sconvert; 
  
  ifstate >> cohort[pichrt].sconvrtflx.carbon;
  ifstate >> cohort[pichrt].sconvrtflx.nitrogen;

  ifstate >> cohort[pichrt].slash.carbon;
  ifstate >> cohort[pichrt].slash.nitrogen;

  ifstate >> cohort[pichrt].slashpar; 

  ifstate >> cohort[pichrt].tillflag;                           

  ifstate >> cohort[pichrt].topt;

  ifstate >> cohort[pichrt].tqc;

  ifstate >> cohort[pichrt].vconvert; 

  ifstate >> cohort[pichrt].vconvrtflx.carbon;
  ifstate >> cohort[pichrt].vconvrtflx.nitrogen;

  ifstate >> cohort[pichrt].vrespar; 

  ifstate >> cohort[pichrt].yrltrc;
  ifstate >> cohort[pichrt].yrltrn;

  ifstate.seekg( 0, ios::cur );
    	
};

/* *************************************************************
************************************************************** */


/* *************************************************************
************************************************************** */
     
void MITelmnt43::saveTEMCohortState( const int& pichrt )
{
  int dlyr;
  int dm;
  int i;
  
  
  cohort[pichrt].potveg = tem.veg.getPOTVEG();

  cohort[pichrt].currentveg = tem.veg.getCURRENTVEG();

  cohort[pichrt].subtype = tem.veg.getSUBTYPE();

  cohort[pichrt].cmnt = tem.veg.cmnt;

  for( i = 0; i < MAXSTATE; ++i )
  {
    cohort[pichrt].y[i] = tem.getY( i );
    cohort[pichrt].prevy[i] = tem.getPREVY( i );
  }

  cohort[pichrt].agcmnt = tem.ag.cmnt;

  cohort[pichrt].aggrowdd = tem.ag.getGROWDD();

  cohort[pichrt].agkd = tem.ag.getKD();

  cohort[pichrt].agprvstate = tem.ag.prvstate;

  cohort[pichrt].agstate = tem.ag.state;

  cohort[pichrt].c2n = tem.veg.getC2N();
  
  cohort[pichrt].cneven = tem.veg.getCNEVEN();

  cohort[pichrt].convrtflx.carbon = tem.ag.getCONVRTFLXC();
  cohort[pichrt].convrtflx.nitrogen = tem.ag.getCONVRTFLXN();

  cohort[pichrt].cropprveetmx = tem.ag.getCROPPRVEETMX();

  cohort[pichrt].cropprvleafmx = tem.ag.getCROPPRVLEAFMX();

  cohort[pichrt].cropprvpetmx = tem.ag.getCROPPRVPETMX();

  cohort[pichrt].cropResidue.carbon = tem.ag.getCROPRESIDUEC();
  cohort[pichrt].cropResidue.nitrogen = tem.ag.getCROPRESIDUEN();

  cohort[pichrt].croptopt = tem.ag.getCROPTOPT();

  cohort[pichrt].eetmx = tem.soil.getEETMX();

  cohort[pichrt].fertflag = tem.ag.fertflag;                              

  cohort[pichrt].formPROD10.carbon = tem.ag.getFORMPROD10C();
  cohort[pichrt].formPROD10.nitrogen = tem.ag.getFORMPROD10N();

  cohort[pichrt].formPROD100.carbon = tem.ag.getFORMPROD100C();
  cohort[pichrt].formPROD100.nitrogen = tem.ag.getFORMPROD100N();

  cohort[pichrt].fprevozone = tem.veg.getFPREVOZONE();

  cohort[pichrt].FRF = tem.ag.getFRF();

  for( dm = 0; dm < CYCLE; ++dm )
  {  
    cohort[pichrt].initPROD1[dm].carbon = tem.ag.getINITPROD1C( dm );
    cohort[pichrt].initPROD1[dm].nitrogen = tem.ag.getINITPROD1N( dm );
  }
  
  for( i = 0; i < 10; ++i )
  {
    cohort[pichrt].initPROD10[i].carbon = tem.ag.getINITPROD10C( i );
    cohort[pichrt].initPROD10[i].nitrogen = tem.ag.getINITPROD10N( i );
  }
    
  for( i = 0; i < 100; ++i )
  {
    cohort[pichrt].initPROD100[i].carbon = tem.ag.getINITPROD100C( i );
    cohort[pichrt].initPROD100[i].nitrogen = tem.ag.getINITPROD100N( i );
  }

  cohort[pichrt].irrgflag = tem.ag.irrgflag;                              
  
  cohort[pichrt].kd = tem.microbe.getKD();

  cohort[pichrt].MDMnpp = tem.getY( tem.I_NPP );

  cohort[pichrt].natprveetmx = tem.ag.getNATPRVEETMX();

  cohort[pichrt].natprvleafmx = tem.ag.getNATPRVLEAFMX();

  cohort[pichrt].natprvpetmx = tem.ag.getNATPRVPETMX();

  cohort[pichrt].natseedC = tem.ag.getNATSEEDC();

  cohort[pichrt].natseedSTRN = tem.ag.getNATSEEDSTRN();

  cohort[pichrt].natseedSTON = tem.ag.getNATSEEDSTON();

  cohort[pichrt].natsoil = tem.ag.getNATSOIL();

  cohort[pichrt].nattopt = tem.ag.getNATTOPT();

  cohort[pichrt].natyreet = tem.ag.getNATYREET();

  cohort[pichrt].natyrpet = tem.ag.getNATYRPET();

  for( dlyr = 0; dlyr < NLVL; ++dlyr )
  {
    cohort[pichrt].NEManh4in[dlyr] = tem.microbe.nem.getANH4IN( dlyr ); 
  
    cohort[pichrt].NEMano3in[dlyr] = tem.microbe.nem.getANO3IN( dlyr );

    cohort[pichrt].NEMdphumin[dlyr] = tem.microbe.nem.getDPHUMIN( dlyr ); 

    cohort[pichrt].NEMocin[dlyr] = tem.microbe.nem.getOCIN( dlyr );

    cohort[pichrt].NEMrclin[dlyr] = tem.microbe.nem.getRCLIN( dlyr ); 
  
    cohort[pichrt].NEMrcrin[dlyr] = tem.microbe.nem.getRCRIN( dlyr );
  
    cohort[pichrt].NEMrcvlin[dlyr] = tem.microbe.nem.getRCVLIN( dlyr );
  }
  
  cohort[pichrt].NEMnsolc = tem.microbe.nem.getNONSOLC();

  //cohort[pichrt].NEMtopdens = tem.microbe.nem.getTOPDENS();
  
  //cohort[pichrt].NEMtopksat = tem.microbe.nem.getTOPKSAT();
  
  //cohort[pichrt].NEMtoppor = tem.microbe.nem.getTOPPOR();

  cohort[pichrt].newleafmx = tem.veg.getNEWLEAFMX();

  cohort[pichrt].newtopt = tem.veg.getNEWTOPT();

  cohort[pichrt].nretent = tem.ag.getNRETENT();

  cohort[pichrt].nsretent = tem.ag.getNSRETENT();

  cohort[pichrt].nvretent = tem.ag.getNVRETENT();

  cohort[pichrt].petmx = tem.atms.getPETMX();

  cohort[pichrt].prev2tair = tem.atms.getPREV2TAIR();

  cohort[pichrt].prevco2 = tem.atms.getPREVCO2();

  cohort[pichrt].prevCropResidue.carbon = tem.ag.getPREVCROPRESIDUEC();
  cohort[pichrt].prevCropResidue.nitrogen = tem.ag.getPREVCROPRESIDUEN();

  cohort[pichrt].prevPROD1.carbon = tem.ag.getPREVPROD1C();
  cohort[pichrt].prevPROD1.nitrogen = tem.ag.getPREVPROD1N();

  cohort[pichrt].prevPROD10.carbon = tem.ag.getPREVPROD10C();
  cohort[pichrt].prevPROD10.nitrogen = tem.ag.getPREVPROD10N();

  cohort[pichrt].prevPROD100.carbon = tem.ag.getPREVPROD100C();
  cohort[pichrt].prevPROD100.nitrogen = tem.ag.getPREVPROD100N();

  cohort[pichrt].prevspack = tem.soil.getPREVSPACK();

  cohort[pichrt].prevtair = tem.atms.getPREVTAIR();

  cohort[pichrt].prevunrmleaf = tem.veg.getPREVUNRMLEAF();
  
  cohort[pichrt].prod10par = tem.ag.getPROD10PAR(); 

  cohort[pichrt].prod100par = tem.ag.getPROD100PAR(); 

  cohort[pichrt].productYear = tem.ag.getPRODUCTYEAR();
  
  cohort[pichrt].prvcropnpp = tem.ag.getPRVCROPNPP();

  cohort[pichrt].prveetmx = tem.soil.getPRVEETMX();

  cohort[pichrt].prvleafmx = tem.veg.getPRVLEAFMX();

  cohort[pichrt].prvpetmx = tem.atms.getPRVPETMX();

  cohort[pichrt].sconvert = tem.ag.getSCONVERT(); 
  
  cohort[pichrt].sconvrtflx.carbon = tem.ag.getSCONVRTFLXC();
  cohort[pichrt].sconvrtflx.nitrogen = tem.ag.getSCONVRTFLXN();

  cohort[pichrt].slash.carbon = tem.ag.getSLASHC();
  cohort[pichrt].slash.nitrogen = tem.ag.getSLASHN();

  cohort[pichrt].slashpar = tem.ag.getSLASHPAR(); 
 
  cohort[pichrt].tillflag = tem.ag.tillflag;                           

  cohort[pichrt].topt = tem.veg.getTOPT();

  cohort[pichrt].vconvert = tem.ag.getVCONVERT(); 

  cohort[pichrt].vconvrtflx.carbon = tem.ag.getVCONVRTFLXC();
  cohort[pichrt].vconvrtflx.nitrogen = tem.ag.getVCONVRTFLXN();

  cohort[pichrt].vrespar = tem.ag.getVRESPAR(); 

  cohort[pichrt].yrltrc = tem.veg.yrltrc;
  cohort[pichrt].yrltrn = tem.veg.yrltrn;

};

/* *************************************************************
************************************************************** */


/* *************************************************************
************************************************************** */

void MITelmnt43::setCohortTEMState( const MITElmntCohort43& firstchrt,
                                    MITElmntCohort43& targetchrt )
{
  int dlyr;
  int dm;
  int i;
  
  
  for( i = 0; i < MAXSTATE; ++i )
  {
    targetchrt.y[i] = firstchrt.y[i];
    targetchrt.prevy[i] = firstchrt.prevy[i];
  }

  targetchrt.aggrowdd = firstchrt.aggrowdd;

  targetchrt.agkd = firstchrt.agkd;

  targetchrt.c2n = firstchrt.c2n;
  
  targetchrt.cneven = firstchrt.cneven;

  targetchrt.convrtflx.carbon = firstchrt.convrtflx.carbon;
  targetchrt.convrtflx.nitrogen = firstchrt.convrtflx.nitrogen;

  targetchrt.cropprveetmx = firstchrt.cropprveetmx;

  targetchrt.cropprvleafmx = firstchrt.cropprvleafmx;

  targetchrt.cropprvpetmx = firstchrt.cropprvpetmx;

  targetchrt.cropResidue.carbon = firstchrt.cropResidue.carbon;
  targetchrt.cropResidue.nitrogen = firstchrt.cropResidue.nitrogen;

  targetchrt.croptopt = firstchrt.croptopt;

  targetchrt.eetmx = firstchrt.eetmx;

  targetchrt.formPROD10.carbon = firstchrt.formPROD10.carbon;
  targetchrt.formPROD10.nitrogen = firstchrt.formPROD10.nitrogen;

  targetchrt.formPROD100.carbon = firstchrt.formPROD100.carbon;
  targetchrt.formPROD100.nitrogen = firstchrt.formPROD100.nitrogen;

  targetchrt.fprevozone = firstchrt.fprevozone;

  for( dm = 0; dm < CYCLE; ++dm )
  {  
    targetchrt.initPROD1[dm].carbon = firstchrt.initPROD1[dm].carbon;
    targetchrt.initPROD1[dm].nitrogen = firstchrt.initPROD1[dm].nitrogen;
  }
  
  for( i = 0; i < 10; ++i )
  {
    targetchrt.initPROD10[i].carbon = firstchrt.initPROD10[i].carbon;
    targetchrt.initPROD10[i].nitrogen = firstchrt.initPROD10[i].nitrogen;
  }
    
  for( i = 0; i < 100; ++i )
  {
    targetchrt.initPROD100[i].carbon = firstchrt.initPROD100[i].carbon;
    targetchrt.initPROD100[i].nitrogen = firstchrt.initPROD100[i].nitrogen;
  }
 
  targetchrt.kd = firstchrt.kd;

  targetchrt.MDMnpp = firstchrt.MDMnpp;

  targetchrt.natprveetmx = firstchrt.natprveetmx;

  targetchrt.natprvleafmx = firstchrt.natprvleafmx;

  targetchrt.natprvpetmx = firstchrt.natprvpetmx;

  targetchrt.natseedC = firstchrt.natseedC;

  targetchrt.natseedSTRN = firstchrt.natseedSTRN;

  targetchrt.natseedSTON = firstchrt.natseedSTON;

  targetchrt.natsoil = firstchrt.natsoil;

  targetchrt.nattopt = firstchrt.nattopt;

  targetchrt.natyreet = firstchrt.natyreet;

  targetchrt.natyrpet = firstchrt.natyrpet;

  for( dlyr = 0; dlyr < NLVL; ++dlyr )
  {
    targetchrt.NEManh4in[dlyr] = firstchrt.NEManh4in[dlyr]; 
  
    targetchrt.NEMano3in[dlyr] = firstchrt.NEMano3in[dlyr];

    targetchrt.NEMdphumin[dlyr] = firstchrt.NEMdphumin[dlyr]; 

    targetchrt.NEMocin[dlyr] = firstchrt.NEMocin[dlyr];

    targetchrt.NEMrclin[dlyr] = firstchrt.NEMrclin[dlyr]; 
  
    targetchrt.NEMrcrin[dlyr] = firstchrt.NEMrcrin[dlyr];
  
    targetchrt.NEMrcvlin[dlyr] = firstchrt.NEMrcvlin[dlyr];
  }
  
  targetchrt.NEMnsolc = firstchrt.NEMnsolc;

  //targetchrt.NEMtopdens = firstchrt.NEMtopdens;
  
  //targetchrt.NEMtopksat = firstchrt.NEMtopksat;
  
  //targetchrt.NEMtoppor = firstchrt.NEMtoppor;

  targetchrt.newleafmx =  firstchrt.newleafmx;

  targetchrt.newtopt = firstchrt.newtopt;

  targetchrt.nretent = firstchrt.nretent;

  targetchrt.nsretent = firstchrt.nsretent;

  targetchrt.nvretent = firstchrt.nvretent;

  targetchrt.petmx = firstchrt.petmx;

  targetchrt.prev2tair = firstchrt.prev2tair;

  targetchrt.prevco2 = firstchrt.prevco2;

  targetchrt.prevCropResidue.carbon = firstchrt.prevCropResidue.carbon;
  targetchrt.prevCropResidue.nitrogen = firstchrt.prevCropResidue.nitrogen;

  targetchrt.prevPROD1.carbon = firstchrt.prevPROD1.carbon;
  targetchrt.prevPROD1.nitrogen = firstchrt.prevPROD1.nitrogen;

  targetchrt.prevPROD10.carbon = firstchrt.prevPROD10.carbon;
  targetchrt.prevPROD10.nitrogen = firstchrt.prevPROD10.nitrogen;

  targetchrt.prevPROD100.carbon = firstchrt.prevPROD100.carbon;
  targetchrt.prevPROD100.nitrogen = firstchrt.prevPROD100.nitrogen;

  targetchrt.prevspack = firstchrt.prevspack;

  targetchrt.prevtair = firstchrt.prevtair;
  
  targetchrt.prevunrmleaf = firstchrt.prevunrmleaf;

  targetchrt.productYear = firstchrt.productYear;
  
  targetchrt.prvcropnpp = firstchrt.prvcropnpp;

  targetchrt.prveetmx = firstchrt.prveetmx;

  targetchrt.prvleafmx = firstchrt.prvleafmx;

  targetchrt.prvpetmx = firstchrt.prvpetmx;

  targetchrt.qc = firstchrt.qc;

  targetchrt.sconvrtflx.carbon = firstchrt.sconvrtflx.carbon;
  targetchrt.sconvrtflx.nitrogen = firstchrt.sconvrtflx.nitrogen;

  targetchrt.slash.carbon = firstchrt.slash.carbon;
  targetchrt.slash.nitrogen = firstchrt.slash.nitrogen;

  targetchrt.topt = firstchrt.topt;

  targetchrt.tqc = firstchrt.tqc;

  targetchrt.vconvrtflx.carbon = firstchrt.vconvrtflx.carbon;
  targetchrt.vconvrtflx.nitrogen = firstchrt.vconvrtflx.nitrogen;

  targetchrt.yrltrc = firstchrt.yrltrc;
  targetchrt.yrltrn = firstchrt.yrltrn;

};

/* *************************************************************
************************************************************** */


/* **************************************************************
************************************************************** */

int MITelmnt43::setGIStopography( ofstream& rflog1,
                                  int& ftlerr,
                                  FILE* fstxt,
                                  FILE* fslayer,
                                  FILE* felev )
{

  int dlyr;
  int gisend;

  MITSoildata43 fao;
  MITSoilLayerdata43 clmsoil;
  Elevdata43 elv;

  gisend = fao.getdel( fstxt );
 
  if( -1 == gisend )
  {
    rflog1 << "Ran out of Soil texture data" << endl << endl;
 
    exit( -1 );
  }
  
  ftlerr = coregerr( rflog1,
                     "Climate",
                     col,
                     row,
                     "TEXTURE",
                     fao.col,
                     fao.row );

  tem.soil.setPCTSAND( fao.pctsand );
  tem.soil.setPCTSILT( fao.pctsilt );
  tem.soil.setPCTCLAY( fao.pctclay );
  tem.soil.setWSOIL( fao.wsoil );
  tem.soil.setPH( fao.pH );
  

  if( 1 == tem.ch4flag || 1 == tem.n2oflag )
  {
    for ( dlyr = 0; dlyr < CLMNLAYERS; ++dlyr )
    {
      gisend = clmsoil.getdel( fslayer );

      if( -1 == gisend )
      {
        rflog1 << "Ran out of Soil layer data" << endl << endl;
 
        exit( -1 );
      }
    
      tem.soil.layerThick[dlyr] = clmsoil.thickness;
      
      tem.soil.porosity[dlyr] = clmsoil.porosity;
      
      tem.soil.density[dlyr] = (1.0 - tem.soil.porosity[dlyr]) * 2.7000; 
    
      tem.soil.Ksat[dlyr] = clmsoil.Ksat;
    }


    // Determine effective soil porosity of the top 30 cm
  
    //cohort[0].NEMtoppor = (1.75/29.0) * tem.soil.porosity[0]
                          //+ (2.75/29.0) * tem.soil.porosity[1]
                          //+ (4.5/29.0) * tem.soil.porosity[2]
                          //+ (7.5/29.0) * tem.soil.porosity[3]
                          //+ (12.5/29.0) * tem.soil.porosity[4];

    // Determine effective soil density of the top 30 cm
  
    //cohort[0].NEMtopdens = (1.75/29.0) * tem.soil.density[0]
                           //+ (2.75/29.0) * tem.soil.density[1]
                           //+ (4.5/29.0) * tem.soil.density[2]
                           //+ (7.5/29.0) * tem.soil.density[3]
                           //+ (12.5/29.0) * tem.soil.density[4];

    // Determine effective saturated soil hydraulic conductivity 
    //   of the top 30 cm
  
    //cohort[0].NEMtopksat = (1.75/29.0) * tem.soil.Ksat[0]
                           //+ (2.75/29.0) * tem.soil.Ksat[1]
                           //+ (4.5/29.0) * tem.soil.Ksat[2]
                           //+ (7.5/29.0) * tem.soil.Ksat[3]
                           //+ (12.5/29.0) * tem.soil.Ksat[4];

    // Determine effective soil porosity of the top 30 cm

    tem.microbe.nem.setTOPPOR( ((1.75/29.0) * tem.soil.porosity[0]
                          + (2.75/29.0) * tem.soil.porosity[1]
                          + (4.5/29.0) * tem.soil.porosity[2]
                          + (7.5/29.0) * tem.soil.porosity[3]
                          + (12.5/29.0) * tem.soil.porosity[4]) );

    // Determine effective soil density of the top 30 cm

    tem.microbe.nem.setTOPDENS( ((1.75/29.0) * tem.soil.density[0]
                           + (2.75/29.0) * tem.soil.density[1]
                           + (4.5/29.0) * tem.soil.density[2]
                           + (7.5/29.0) * tem.soil.density[3]
                           + (12.5/29.0) * tem.soil.density[4]) );

    // Determine effective saturated soil hydraulic conductivity 
    //   of the top 30 cm

    tem.microbe.nem.setTOPKSAT( ((1.75/29.0) * tem.soil.Ksat[0]
                           + (2.75/29.0) * tem.soil.Ksat[1]
                           + (4.5/29.0) * tem.soil.Ksat[2]
                           + (7.5/29.0) * tem.soil.Ksat[3]
                           + (12.5/29.0) * tem.soil.Ksat[4]) );
 
  }
  else 
  {
    //cohort[0].NEMtoppor = ZERO;
    //cohort[0].NEMtopdens = ZERO;
    //cohort[0].NEMtopksat = ZERO;
    tem.microbe.nem.setTOPPOR( ZERO ); 
    tem.microbe.nem.setTOPDENS( ZERO ); 
    tem.microbe.nem.setTOPKSAT( ZERO ); 
  }
  
  gisend = elv.getdel( felev );
  
  if( gisend == -1 )
  {
    rflog1 << "Ran out of Elevation data" << endl << endl;
 
    exit( -1 );
  }
  
  ftlerr = coregerr( rflog1,
                     "Climate",
                     col,
                     row,
                     "ELEV",
                     elv.col,
                     elv.row );

  tem.elev = elv.elev;

  return gisend;

};

/* *************************************************************
************************************************************** */


/* *************************************************************
************************************************************* */

void MITelmnt43::setTEMequilState( ofstream& rflog1,
                                   const int& equil,
                                   const int& totsptime,
                                   const int& pichrt )
{
  const int dyr = 0;
  
  int dm;

//  double totalSoilC;
//  double outn2oflx;

  // Set all TEM state-related variables in cohort to MISSING
  //   (i.e. start with "clean slate" in cohort)
   
  initializeCohortTEMState( pichrt );

  // Assign cohort data to TEM (i.e. transfer information from 
  //   the land cover/land use module to TEM and start with 
  //   "clean slate" for TEM cohort data)
   
  getTEMCohortState( pichrt );
  
  
  cohort[pichrt].qc = ACCEPT;
  cohort[pichrt].tqc = ACCEPT;

  tem.totyr = 0;

  tem.ag.fert1950flag = 0;

  tem.microbe.nem.startflag = 0;
  
  tem.atms.setMXTAIR( mxtair );
  
  tem.atms.yrprec = yrprec;

  for( dm = 0; dm < CYCLE; ++dm )
  {                                
    // Pass climate data for particular month to TEM

    tem.atms.setNIRR( climate[mitclm.I_NIRR][dm] );
    tem.atms.setPAR( climate[mitclm.I_PAR][dm] );
    tem.atms.setTAIR( climate[mitclm.I_TAIR][dm] );
    tem.atms.setPREC( climate[mitclm.I_PREC][dm] );
    tem.soil.setINEET( initAET[pichrt][dm] );
    tem.soil.setMOIST( initSH2O[pichrt][dm] );
    tem.soil.setSNOWPACK( initSNOWPACK[pichrt][dm] );
    tem.soil.setSURFRUN( initSURFRUN[pichrt][dm] );
    tem.soil.setDRAINAGE( initDRAINAGE[pichrt][dm] );
    tem.atms.setCO2( climate[mitclm.I_CO2][dm] );
    tem.atms.setAOT40( climate[mitclm.I_AOT40][dm] );


    // Check TEM climate input for valid data 

    cohort[pichrt].qc = temgisqc( cohort[pichrt].chrtarea,
                                  tem.soil.getPCTSILT(),
                                  tem.soil.getPCTCLAY(),
                                  tem.veg.cmnt,
                                  tem.elev,
                                  tem.atms.getNIRR(),
                                  tem.atms.getPAR(),
                                  tem.atms.getTAIR(),
                                  tem.atms.getMXTAIR(),
                                  tem.atms.yrprec,
                                  tem.atms.getPREC(),
                                  tem.soil.getINEET(),
                                  tem.soil.getMOIST(),
                                  tem.soil.getSNOWPACK(),
                                  tem.soil.getSURFRUN(),
                                  tem.soil.getDRAINAGE(),
                                  tem.atms.getCO2(),
                                  tem.atms.getAOT40() );


    if( cohort[pichrt].qc != ACCEPT ) 
    { 
      rflog1 << "temgisqc = " << cohort[pichrt].qc;
      rflog1 << " during month " << (dm+1) << endl;
      break; 
    }
      
    // Determine initial values for tem.atms.prvpetmx, 
    //   tem.atms.prveetmx and and tem.veg.topt based on
    //   long-term mean climate

    tem.setEquilEvap( tem.atms.getNIRR(), 
                      tem.atms.getTAIR(), 
                      dm );
  }

    
  // Check TEM parameters for specific vegetation types

  if( ACCEPT == cohort[pichrt].qc ) 
  { 
    cohort[pichrt].qc = tem.ecdqc( tem.veg.cmnt ); 

    if( cohort[pichrt].qc != ACCEPT )
    {
      // Note: If a TEM parameter is invalid, 
      //   cohort[pichrt].qc will have a value greater than 
      //   100
      	
      rflog1 << "temecdqc = " << cohort[pichrt].qc << endl;
    }
  }

  if( cohort[pichrt].qc != ACCEPT )
  {
    // If environmental conditions are too extreme for the 
    //   existence of vegetation (e.g., no precipitation or 
    //   constant freezing air temperatures), assign zero to 
    //   all TEM variables if the plant community is anything
    //   besides ice and open water; and all TEM parameters 
    //   are valid (i.e. cohort[pichrt].qc < 100 )

    if( (cohort[pichrt].qc < 100
         && tem.veg.cmnt > 1 
         && (mxtair < -1.0))
         || cohort[pichrt].chrtarea < 1 )
    {
      // Set tqc flag to assign zero to all TEM variables 
      //   during simulation
      	
      cohort[pichrt].tqc = TQCZEROFLAG; 
    }
    else { cohort[pichrt].tqc = REJECT; }

    // Set missing values to telmnt[0].output

    setTEMmiss( dyr,
                equil,
                totsptime,
                pichrt  );
  }
  else // "cohort[pichrt].qc == ACCEPT"
  {

/* *************************************************************
                   Start Equilibrium Conditions
************************************************************* */

    // Determine soil properties of element based on 
    //   soil texture
    
    tem.soil.xtext( tem.veg.cmnt, 
                    tem.soil.getPCTSILT(), 
                    tem.soil.getPCTCLAY() );

    
    // Initialize tem.atms.prevco2

    tem.atms.setPREVCO2( tem.atms.getCO2LEVEL() );


    // Initialize TEM parameters based on element's 
    //   (i.e. grid cell) vegetation type, soil texture
    //   and atmospheric CO2 concentration
      
    tem.setELMNTecd( tem.veg.cmnt, tem.soil.getPSIPLUSC() );

    tem.setEquilC2N( tem.veg.cmnt, 
                     tem.atms.getPREVCO2() );


    // Assume potential vegetation when determining 
    //   equilibrium conditions

    tem.ag.state = 0;
    tem.ag.prvstate = 0;

    tem.ag.tillflag = 0;
    tem.ag.fertflag = 0;
    tem.ag.irrgflag = 0;


    // Initialize agricultural growing degree days to zero

    tem.ag.setGROWDD( ZERO );
          
    // "While" loop to allow adaptive integrator tolerance 
    //   (i.e. tem.tol) to be reduced if chaotic behavior 
    //   occurs

    tem.qualcon[dyr] = 0;


    // Try up to "tem.maxnrun" times to equilibrate TEM.  If
    //   TEM does not equilibrate within "tem.runsize" 
    //   iterations, decrease tem.tol by an order of magnitude
    //   and try again
     
    tem.nattempt = 0;
    tem.tol = tem.inittol;
    tem.baseline = tem.initbase;
    tem.initFlag = 0;
        
    while( tem.nattempt < tem.maxnrun 
           && 0 == tem.initFlag )
    {               
      tem.nattempt = equilibrateTEM( pichrt, tem.tol );
	                               
      if( tem.nattempt < tem.maxnrun 
          && 0 == tem.initFlag ) 
      { 
      	tem.tol /= 10.0; 
      }
    }

    // Update summary variables for initial agricultural 
    //   state of cohort at end of equilibrium portion 
    //   of the TEM simulation

    tem.ag.setNATSEEDC( ZERO );
    tem.ag.setNATSEEDSTRN( ZERO );
    tem.ag.setNATSEEDSTON( ZERO );
    tem.ag.setCROPPRVLEAFMX( 1.0 );
    tem.ag.setCROPTOPT( tem.veg.getTOPT() );
    tem.ag.setCROPPRVPETMX( tem.atms.getPRVPETMX() );
    tem.ag.setCROPPRVEETMX( tem.soil.getPRVEETMX() );
    tem.ag.setPRVCROPNPP( ZERO );


    // Save quality control information about the simulation 
    //   conditions when the equilibrium portion ended
    //   (i.e. did the carbon and nitrogen fluxes really come 
    //         to equilibrium or was the run terminated after
    //         running chaotically for a specified maximum
    //         number of years?) 

    tem.qualcon[dyr] += (tem.nattempt + 1);
    
    // If simulation is part of a transient simulation, reset
    //   tem.totyr to represent an actual year rather than 
    //   the number of iterations required to reach equilibrum
    
    if( 0 == equil )
    {
      tem.totyr = tem.startyr - totsptime - 1;
      ttotyr[dyr] = tem.totyr;
      
      cohort[pichrt].tqc = transqc( tem.maxyears, 
	                            tem.totyr, 
	                            output[tem.I_VEGC][pichrt] );
    }
    else { ttotyr[dyr] = tem.totyr; }
  } // End of "cohort.qc == ACCEPT"

  // Save TEM state of cohort to telmnt[0].cohort

//  cout << "End of equil TEM cmnt = " << tem.veg.cmnt << endl;
  saveTEMCohortState( pichrt );

};

/* *************************************************************
************************************************************** */


/* *************************************************************
************************************************************** */

void MITelmnt43::setTEMmiss( const int& pdyr,
                             const int& equil,
                             const int& totsptime,
                             const int& pichrt )
{
  int dm;
  int i;
  
  if( 0 == equil )
  {
    ttotyr[pdyr] = tem.startyr 
                   - totsptime - 1 
                   + (pdyr * tem.diffyr);
  }
  else
  {
    ttotyr[pdyr] = -999;
  }

  tem.totyr = ttotyr[pdyr];

  if( TQCZEROFLAG == cohort[pichrt].tqc )
  {
    if( 1 == equil ) { ttotyr[pdyr] = 1; }

    // Assign zero to all TEM state variables
      
    for( i = 0; i < MAXSTATE; ++i )
    {
      tem.setY( ZERO, i );
      tem.setPREVY(ZERO, i );
    }

    for( i = MAXSTATE; i < NUMEQ; ++i )
    {
      tem.setY( ZERO, i );
    }
      
    // Assign zero to all TEM ouput variables
      
    for(i = 0; i < NUMTEM; ++i )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        output[i][pichrt][dm] = ZERO;
      }
    }
  }
  else
  {
    // Assign missing values to grid cells that are covered by ice or open
    // water, or where TEM did not converge on a solution

    for( i = 0; i < MAXSTATE; ++i )
    {
      tem.setY( MISSING, i );
      tem.setPREVY( MISSING, i );
    }

    for( i = MAXSTATE; i < NUMEQ; ++i )
    {
      tem.setY( MISSING, i );
    }
      
    for(i = 0; i < NUMTEM; ++i )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        output[i][pichrt][dm] = MISSING;
      }
    }
  }

};

/* **************************************************************
************************************************************** */


/* **************************************************************
************************************************************** */

int MITelmnt43::temgisqc( const long& subarea,
                          const double& pctsilt,
                          const double& pctclay,
		          const int& cmnt,
                          const double& elev,
                          const double& nirr,
                          const double& par,
		          const double& tair,
                          const double& mxtair,
                          const double& yrprec,
                          const double& prec,
                          const double& eet,
                          const double& sh2o,
                          const double& spack,
                          const double& srfrun,
                          const double& drain,
                          const double& co2,
                          const double& aot40 )


{
  int qc;

  qc = ACCEPT;

  if( subarea < 1 ) { return qc = 1; }
  if( pctsilt < ZERO ) { return qc = 2; }
  if( pctclay < ZERO ) { return qc = 3; }
  if( cmnt < 1 || cmnt > NUMVEG ) { return qc = 4; }
  if( elev <= -999.0 ) { return qc = 5;}

  if( nirr <= -1.0 ) { return qc = 6; }
  if( par <= -1.0 ) { return qc = 7; }
  if( tair <= -99.0 ) { return qc = 8; }
  if( mxtair < -1.0 ) { return qc = 9; }
  if( yrprec <= ZERO ) { return qc = 10; }
  if( prec <= -1.0 ) { return qc = 11; }
  if( eet <= -1.0 ) { return qc = 12; }
  if( sh2o <= -1.0 ) { return qc = 13; }
  if( spack <= -1.0 ) { return qc = 14; }
  if( srfrun <= -1.0 ) { return qc = 15; }
  if( drain <= -1.0 ) { return qc = 16; }
  if( co2 <= -1.0 ) { return qc = 17; }
  if( aot40 <= -1.0 ) { return qc = 18; }

  return qc;

};

/* *************************************************************
************************************************************** */


/* *************************************************************
************************************************************** */

void MITelmnt43::temwritepred( ofstream fout[NUMTEM],
                               const vector<string>& predname,
                               const int& pdyr,
                               const int& pichrt,
                               const int& ntempred )
{
  // Covert cal/cm2/day to W/m2 (4.186 Joules / calorie)
  const double  cal2Watts = 0.4845;

  // Units conversion from grams to milligrams
  const double GRAMS2MG = 1000.0;
   
  // Units conversion from proportion to percent
  const double PROP2PCT = 100.0;
  
  int i;
  int dm;
  Temdata43 tempred;


  for( i = 0; i < ntempred; ++i )
  {
    // ************** Carbon stocks in ecosystems  *************


    if( predname.at( i ) == tem.predstr.at( tem.I_VEGC ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_VEGC][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_SOLC ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_SOLC][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_TOTEC ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_TOTEC][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_TOTC ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_TOTC][pichrt][dm];
      }
    }


    // *************** Nitrogen stocks in ecosystems ***********

    else if( predname.at( i ) == tem.predstr.at( tem.I_STRN ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_STRN][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_STON ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_STON][pichrt][dm] * GRAMS2MG;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_SOLN ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_SOLN][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_AVLN ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_AVLN][pichrt][dm] * GRAMS2MG;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_VEGN ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_VEGN][pichrt][dm];
      }
    }


    // *****************Water stocks in ecosystems *************

    else if( predname.at( i ) == tem.predstr.at( tem.I_AVLW ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_AVLW][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_SM ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_SM][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_VSM ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_VSM][pichrt][dm] * PROP2PCT; 
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_PCTP ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_PCTP][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_SNWPCK ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_SNWPCK][pichrt][dm];
      }
    }


   // ******************** Phenology ***************************


    else if( predname.at( i ) == tem.predstr.at( tem.I_UNRMLF ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_UNRMLF][pichrt][dm] * PROP2PCT;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_LEAF ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_LEAF][pichrt][dm] * PROP2PCT;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_LAI ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_LAI][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_FPC ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_FPC][pichrt][dm] * PROP2PCT;
      }
    }


    // *************** Carbon fluxes in ecosystems *************


    else if( predname.at( i ) == tem.predstr.at( tem.I_INGPP ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_INGPP][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_GPP ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_GPP][pichrt][dm];
      }
    }

    // *********************** Ozone Effects *******************

    else if( predname.at( i ) == tem.predstr.at( tem.I_FOZONE ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_FOZONE][pichrt][dm] * PROP2PCT;  

      }
    }
    else if( predname.at( i ) == tem.predstr.at( tem.I_FINDOZONE ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_FINDOZONE][pichrt][dm] * PROP2PCT;  

      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_INNPP ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_INNPP][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_NPP ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_NPP][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_GPR ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_GPR][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_RVMNT ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_RVMNT][pichrt][dm];  
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_RVGRW ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_RVGRW][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_LTRC ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_LTRC][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_RH ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_RH][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_NEP ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_NEP][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_NCE ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_NCE][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_CH4EMS ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_CH4EMS][pichrt][dm] * GRAMS2MG;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_CH4CSMP ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_CH4CSMP][pichrt][dm] * GRAMS2MG;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_CH4FLX ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_CH4FLX][pichrt][dm] * GRAMS2MG;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_CO2NFLX ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_CO2NFLX][pichrt][dm] * GRAMS2MG;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_CO2DNFLX ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_CO2DNFLX][pichrt][dm] * GRAMS2MG;
      }
    }

    // ************** Nitrogen fluxes in ecosystems ************


    else if( predname.at( i ) == tem.predstr.at( tem.I_NINP ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_NINP][pichrt][dm] * GRAMS2MG;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_AGFRTN ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_AGFRTN][pichrt][dm] * GRAMS2MG; 
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_INNUP ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_INNUP][pichrt][dm] * GRAMS2MG;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_VNUP ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_VNUP][pichrt][dm] * GRAMS2MG;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_VSUP ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_VSUP][pichrt][dm] * GRAMS2MG;  
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_VLUP ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_VLUP][pichrt][dm] * GRAMS2MG;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_VNMBL ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_VNMBL][pichrt][dm] * GRAMS2MG;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_VNRSRB ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_VNRSRB][pichrt][dm] * GRAMS2MG;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_LTRN ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_LTRN][pichrt][dm] * GRAMS2MG;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_MNUP ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_MNUP][pichrt][dm] * GRAMS2MG;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_NMIN ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_NMIN][pichrt][dm] * GRAMS2MG;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_NLST ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_NLST][pichrt][dm] * GRAMS2MG;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_NOFLX ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_NOFLX][pichrt][dm] * GRAMS2MG;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_N2OFLX ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_N2OFLX][pichrt][dm] * GRAMS2MG;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_N2ONFLX ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_N2ONFLX][pichrt][dm] * GRAMS2MG;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_N2ODNFLX ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_N2ODNFLX][pichrt][dm] * GRAMS2MG;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_N2FLX ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_N2FLX][pichrt][dm] * GRAMS2MG;
      }
    }

    // *****************Water fluxes in ecosystems *************

    else if( predname.at( i ) == tem.predstr.at( tem.I_AGIRRIG ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_AGIRRIG][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_INEET ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_INEET][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_EET ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_EET][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_PET ) )
    {
      for ( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_PET][pichrt][dm];
      }
    }


// ************** Carbon stocks in products ********************


    else if( predname.at( i ) == tem.predstr.at( tem.I_AGPRDC ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_AGPRDC][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_PROD10C ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_PROD10C][pichrt][dm];      
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_PROD100C ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_PROD100C][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_TOTPRDC ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_TOTPRDC][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_RESIDC ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_RESIDC][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_AGSTUBC ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_AGSTUBC][pichrt][dm];
      }
    }

    // ************** Nitrogen stocks in products **************


    else if( predname.at( i ) == tem.predstr.at( tem.I_AGPRDN ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_AGPRDN][pichrt][dm] * GRAMS2MG;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_PROD10N ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_PROD10N][pichrt][dm] * GRAMS2MG;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_PROD100N ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_PROD100N][pichrt][dm] * GRAMS2MG;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_TOTPRDN ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_TOTPRDN][pichrt][dm] * GRAMS2MG; 
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_RESIDN ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_RESIDN][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_AGSTUBN ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_AGSTUBN][pichrt][dm];  
      }
    }


    // *** Carbon fluxes during agricultural conversion ********


    else if( predname.at( i ) == tem.predstr.at( tem.I_CNVRTC ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_CNVRTC][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_VCNVRTC ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_VCNVRTC][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_SCNVRTC ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_SCNVRTC][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_SLASHC ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_SLASHC][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_CFLX ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_CFLX][pichrt][dm];
      }
    }


    // *** Nitrogen fluxes during agricultural conversion ******


    else if( predname.at( i ) == tem.predstr.at( tem.I_CNVRTN ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_CNVRTN][pichrt][dm] * GRAMS2MG;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_VCNVRTN ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_VCNVRTN][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_SCNVRTN ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_SCNVRTN][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_SLASHN ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
      	 tempred.mon[dm] = output[tem.I_SLASHN][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_NRETNT ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_NRETNT][pichrt][dm]; 
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_NVRTNT ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_NVRTNT][pichrt][dm]; 
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_NSRTNT ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_NSRTNT][pichrt][dm];
      }
    }


    // ************** Carbon fluxes to/from products ***********

    else if( predname.at( i ) == tem.predstr.at( tem.I_AGFPRDC ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_AGFPRDC][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_PRDF10C ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_PRDF10C][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_PRDF100C ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_PRDF100C][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_TOTFPRDC ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_TOTFPRDC][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_FRESIDC ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_FRESIDC][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_AGPRDFC ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_AGPRDFC][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_PRD10FC ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_PRD10FC][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_PRD100FC ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_PRD100FC][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_TOTPRDFC ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_TOTPRDFC][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_RESIDFC ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_RESIDFC][pichrt][dm];
      }
    }

    // ************** Nitrogen fluxes to/from products *********


    else if( predname.at( i ) == tem.predstr.at( tem.I_AGFPRDN ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_AGFPRDN][pichrt][dm] * GRAMS2MG;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_PRDF10N ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_PRDF10N][pichrt][dm] * GRAMS2MG;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_PRDF100N ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_PRDF100N][pichrt][dm] * GRAMS2MG;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_TOTFPRDN ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_TOTFPRDN][pichrt][dm] * GRAMS2MG;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_FRESIDN ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_FRESIDN][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_AGPRDFN ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_AGPRDFN][pichrt][dm] * GRAMS2MG;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_PRD10FN ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_PRD10FN][pichrt][dm] * GRAMS2MG;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_PRD100FN ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_PRD100FN][pichrt][dm] * GRAMS2MG;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_TOTPRDFN ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_TOTPRDFN][pichrt][dm] * GRAMS2MG;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_RESIDFN ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_RESIDFN][pichrt][dm];
      }
    }

    // ************** Carbon stocks in crops   *****************


    else if( predname.at( i ) == tem.predstr.at( tem.I_CROPC ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_CROPC][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_NATVEGC ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_NATVEGC][pichrt][dm];
      }
    }


    // ************** Nitrogen stocks in crops *****************


    else if( predname.at( i ) == tem.predstr.at( tem.I_CROPN ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_CROPN][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_NATVEGN ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_NATVEGN][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_CSTRN ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_CSTRN][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_NATSTRN ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_NATSTRN][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_CSTON ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_CSTON][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_NATSTON ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_NATSTON][pichrt][dm];
      }
    }

    // ******************** Crop Phenology *********************


    else if( predname.at( i ) == tem.predstr.at( tem.I_CROPULF ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_CROPULF][pichrt][dm] * PROP2PCT;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_NATULF ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_NATULF][pichrt][dm] * PROP2PCT;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_CROPLEAF ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_CROPLEAF][pichrt][dm] * PROP2PCT;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_NATLEAF ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_NATLEAF][pichrt][dm] * PROP2PCT;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_CROPLAI ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_CROPLAI][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_NATLAI ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_NATLAI][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_CROPFPC ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_CROPFPC][pichrt][dm] * PROP2PCT;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_NATFPC ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_NATFPC][pichrt][dm] * PROP2PCT;
      }
    }

    // ************** Carbon fluxes in croplands ***************

    else if( predname.at( i ) == tem.predstr.at( tem.I_AGINGPP ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_AGINGPP][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_NATINGPP ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_NATINGPP][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_AGGPP ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_AGGPP][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_NATGPP ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_NATGPP][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_AGINNPP ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_AGINNPP][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_NATINNPP ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_NATINNPP][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_AGNPP ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_AGNPP][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_NATNPP ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_NATNPP][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_AGGPR ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_AGGPR][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_NATGPR ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_NATGPR][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_AGRVMNT ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_AGRVMNT][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_NATRVMNT ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_NATRVMNT][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_AGRVGRW ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_AGRVGRW][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_NATRVGRW ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_NATRVGRW][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_AGLTRC ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_AGLTRC][pichrt][dm]; 
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_NATLTRC ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_NATLTRC][pichrt][dm];
      }
    }

    // ************** Nitrogen fluxes in croplands *************


    else if( predname.at( i ) == tem.predstr.at( tem.I_AGINNUP ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_AGINNUP][pichrt][dm] * GRAMS2MG;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_NATINNUP ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_NATINNUP][pichrt][dm] * GRAMS2MG;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_AGVNUP ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_AGVNUP][pichrt][dm] * GRAMS2MG;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_NATVNUP ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_NATVNUP][pichrt][dm] * GRAMS2MG;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_AGVSUP ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_AGVSUP][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_NATVSUP ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_NATVSUP][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_AGVLUP ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_AGVLUP][pichrt][dm];  
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_NATVLUP ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_NATVLUP][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_AGVNMBL ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_AGVNMBL][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_NATVNMBL ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_NATVNMBL][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_AGVNRSRB ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_AGVNRSRB][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_NVNRSRB ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_NVNRSRB][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_AGLTRN ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_AGLTRN][pichrt][dm] * GRAMS2MG; 
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_NATLTRN ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_NATLTRN][pichrt][dm] * GRAMS2MG;
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_NIRR ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_NIRR][pichrt][dm] * cal2Watts; 
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_PAR ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_PAR][pichrt][dm] * cal2Watts; 
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_TAIR ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_TAIR][pichrt][dm]; 
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_PREC ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_PREC][pichrt][dm]; 
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_SRFRUN ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_SRFRUN][pichrt][dm]; 
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_DRAIN ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_DRAIN][pichrt][dm];   
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_CO2 ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_CO2][pichrt][dm];
      }
    }

    else if( predname.at( i ) == tem.predstr.at( tem.I_AOT40 ) )
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = output[tem.I_AOT40][pichrt][dm];  
      }
    }

    else
    {
      for( dm = 0; dm < CYCLE; ++dm )
      {
        tempred.mon[dm] = MISSING;
      }
    }

    // Write output data to files

    if( predname.at( i ) == tem.predstr.at( tem.I_VSM ) 
        || predname.at( i ) == tem.predstr.at( tem.I_PCTP ) 
        || predname.at( i ) == tem.predstr.at( tem.I_LEAF ) )
    {
      tempred.poutdel( fout[i],
                       col,
                       row,
                       predname.at( i ),
                       (pichrt+1),
                       tem.veg.getPOTVEG(),
                       tem.veg.getCURRENTVEG(),
                       tem.veg.getSUBTYPE(),
                       tem.veg.cmnt,
                       (PROP2PCT * tem.soil.getPSIPLUSC()),
                       tem.qualcon[pdyr],
                       carea,
                       subarea,
                       ttotyr[pdyr],
                       tempred.mon,
                       region );
    }
    else
    {
      tempred.outdel( fout[i],
                      col,
                      row,
                      predname.at( i ),
                      (pichrt+1),
                      tem.veg.getPOTVEG(),
                      tem.veg.getCURRENTVEG(),
                      tem.veg.getSUBTYPE(),
                      tem.veg.cmnt,
                      (PROP2PCT * tem.soil.getPSIPLUSC()),
                      tem.qualcon[pdyr],
                      carea,
                      cohort[pichrt].chrtarea,
                      ttotyr[pdyr],
                      tempred.mon,
                      region );
    }
  }

};

/* *************************************************************
************************************************************* */


/* *************************************************************
************************************************************* */

int MITelmnt43::transqc( int& maxyears,
                         int& totyr,
                         double plantc[CYCLE] )
{

  int dm;
  int qc;
  double sumcarbon = ZERO;
  qc = ACCEPT;

  if( totyr < 0 || totyr >= maxyears ) { return qc = 30; }
  for( dm = 0; dm < CYCLE; ++dm ) { sumcarbon += plantc[dm]; }
  if( sumcarbon <= 0.1 ) { return qc = TQCZEROFLAG; }

  return qc;

};

/* *************************************************************
************************************************************* */


/* *************************************************************
************************************************************* */

void MITelmnt43::updateTEMmonth( const int& equil,
                                 const int& totsptime,
                                 const int& pdyr,
                                 const int& pdm,
                                 const int& pichrt )
{  
  // Pass cohort characteristics information to TEM

  getTEMCohortState( pichrt );


  if( ACCEPT == cohort[pichrt].qc 
      && ACCEPT == cohort[pichrt].tqc )
  {
    tem.baseline = 0;
    tem.wrtyr = -99;

    tem.totyr = tem.startyr 
                - totsptime - 1 
                + (pdyr * tem.diffyr);


    // Allow optimum N fertilization of crops after 1950

    if( 1 == tem.ag.fertflag && tem.totyr >= 1950 )
    {
      tem.ag.fert1950flag = 1;
    }
    else
    {
      tem.ag.fert1950flag = 0;
    }


    // Run the Terrestrial Ecosystem Model (TEM) under 
    //   transient conditions

    wrtyr = tem.monthlyTransient( pdyr, 
                                  pdm, 
                                  tem.tol );
   
      
    // Save TEM output to telmnt[0].output

    outputTEMmonth( pichrt, pdm );

    ttotyr[pdyr] = tem.totyr;
  } // End of qc == ACCEPT and tqc = ACCEPT
  else
  {
    if( (CYCLE-1) == pdm )
    {
      // Set missing values to telmnt[0].output

      setTEMmiss( pdyr,
                  equil,
                  totsptime,
                  pichrt );
    }
  }  

  // Save TEM state for cohort
  
  saveTEMCohortState( pichrt );

};

/* *************************************************************
************************************************************* */


/* *************************************************************
************************************************************** */

void MITelmnt43::writeCohortState( ofstream& ofstate,
                                   const int& pichrt )
{
  int i;
  int dlyr;
  int dm;

  ofstate << col << " ";    
  ofstate << row << " ";
  ofstate << (pichrt+1) << " ";
  
  ofstate << cohort[pichrt].srcCohort << " ";
  ofstate << cohort[pichrt].chrtarea << " ";
  ofstate << cohort[pichrt].potveg << " ";
  ofstate << cohort[pichrt].currentveg << " ";
  ofstate << cohort[pichrt].subtype << " ";
  ofstate << cohort[pichrt].cmnt << " ";

 	
  for( i = 0; i < MAXSTATE; ++i )
  {
    ofstate << cohort[pichrt].y[i] << " ";
    ofstate << cohort[pichrt].prevy[i] << " ";
  }

  ofstate << cohort[pichrt].agcmnt << " ";

  ofstate << cohort[pichrt].aggrowdd << " ";

  ofstate << cohort[pichrt].agkd << " ";

  ofstate << cohort[pichrt].agprvstate << " ";

  ofstate << cohort[pichrt].agstate << " ";

  ofstate << cohort[pichrt].c2n << " ";
  
  ofstate << cohort[pichrt].cneven << " ";

  ofstate << cohort[pichrt].convrtflx.carbon << " ";
  ofstate << cohort[pichrt].convrtflx.nitrogen << " ";

  ofstate << cohort[pichrt].cropprveetmx << " ";

  ofstate << cohort[pichrt].cropprvleafmx << " ";

  ofstate << cohort[pichrt].cropprvpetmx << " ";

  ofstate << cohort[pichrt].cropResidue.carbon << " ";
  ofstate << cohort[pichrt].cropResidue.nitrogen << " ";

  ofstate << cohort[pichrt].croptopt << " ";

  ofstate << cohort[pichrt].eetmx << " ";

  ofstate << cohort[pichrt].fertflag << " ";                              

  ofstate << cohort[pichrt].formPROD10.carbon << " ";
  ofstate << cohort[pichrt].formPROD10.nitrogen << " ";

  ofstate << cohort[pichrt].formPROD100.carbon << " ";
  ofstate << cohort[pichrt].formPROD100.nitrogen << " ";

  ofstate << cohort[pichrt].fprevozone << " ";

  ofstate << cohort[pichrt].FRF << " ";

  for( dm = 0; dm < CYCLE; ++dm )
  {  
    ofstate << cohort[pichrt].initPROD1[dm].carbon << " ";
    ofstate << cohort[pichrt].initPROD1[dm].nitrogen << " ";
  }
  
  for( i = 0; i < 10; ++i )
  {
    ofstate << cohort[pichrt].initPROD10[i].carbon << " ";
    ofstate << cohort[pichrt].initPROD10[i].nitrogen << " ";
  }
    
  for( i = 0; i < 100; ++i )
  {
    ofstate << cohort[pichrt].initPROD100[i].carbon << " ";
    ofstate << cohort[pichrt].initPROD100[i].nitrogen << " ";
  }

  ofstate << cohort[pichrt].irrgflag << " ";                              
  
  ofstate << cohort[pichrt].kd << " ";

  ofstate << cohort[pichrt].MDMnpp << " ";

  ofstate << cohort[pichrt].natprveetmx << " ";

  ofstate << cohort[pichrt].natprvleafmx << " ";

  ofstate << cohort[pichrt].natprvpetmx << " ";

  ofstate << cohort[pichrt].natseedC << " ";

  ofstate << cohort[pichrt].natseedSTRN << " ";

  ofstate << cohort[pichrt].natseedSTON << " ";

  ofstate << cohort[pichrt].natsoil << " ";

  ofstate << cohort[pichrt].nattopt << " ";

  ofstate << cohort[pichrt].natyreet << " ";

  ofstate << cohort[pichrt].natyrpet << " ";

  for( dlyr = 0; dlyr < NLVL; ++dlyr )
  {
    ofstate <<  cohort[pichrt].NEManh4in[dlyr] << " "; 
  
    ofstate << cohort[pichrt].NEMano3in[dlyr] << " ";

    ofstate << cohort[pichrt].NEMdphumin[dlyr] << " "; 

    ofstate << cohort[pichrt].NEMocin[dlyr] << " ";

    ofstate << cohort[pichrt].NEMrclin[dlyr] << " "; 
  
    ofstate << cohort[pichrt].NEMrcrin[dlyr] << " ";
  
    ofstate << cohort[pichrt].NEMrcvlin[dlyr] << " ";
  }
  
  ofstate << cohort[pichrt].NEMnsolc << " ";

  //ofstate << cohort[pichrt].NEMtopdens << " ";
  
  //ofstate << cohort[pichrt].NEMtopksat << " ";
  
  //ofstate << cohort[pichrt].NEMtoppor << " ";

  ofstate << cohort[pichrt].newleafmx << " ";

  ofstate << cohort[pichrt].newtopt << " ";

  ofstate << cohort[pichrt].nretent << " ";

  ofstate << cohort[pichrt].nsretent << " ";

  ofstate << cohort[pichrt].nvretent << " ";

  ofstate << cohort[pichrt].petmx << " ";

  ofstate << cohort[pichrt].prev2tair << " ";

  ofstate << cohort[pichrt].prevco2 << " ";

  ofstate << cohort[pichrt].prevCropResidue.carbon << " ";
  ofstate << cohort[pichrt].prevCropResidue.nitrogen << " ";

  ofstate << cohort[pichrt].prevPROD1.carbon << " ";
  ofstate << cohort[pichrt].prevPROD1.nitrogen << " ";

  ofstate << cohort[pichrt].prevPROD10.carbon << " ";
  ofstate << cohort[pichrt].prevPROD10.nitrogen << " ";

  ofstate << cohort[pichrt].prevPROD100.carbon << " ";
  ofstate << cohort[pichrt].prevPROD100.nitrogen << " ";

  ofstate << cohort[pichrt].prevspack << " ";

  ofstate << cohort[pichrt].prevtair << " ";

  ofstate << cohort[pichrt].prevunrmleaf << " ";
  
  ofstate << cohort[pichrt].prod10par << " "; 

  ofstate << cohort[pichrt].prod100par << " "; 

  ofstate << cohort[pichrt].productYear << " ";

  ofstate << cohort[pichrt].prvchrtarea << " ";
  
  ofstate << cohort[pichrt].prvcropnpp << " ";

  ofstate << cohort[pichrt].prveetmx << " ";

  ofstate << cohort[pichrt].prvleafmx << " ";

  ofstate << cohort[pichrt].prvpetmx << " ";

  ofstate << cohort[pichrt].qc << " ";

  ofstate << cohort[pichrt].sconvert << " "; 
  
  ofstate << cohort[pichrt].sconvrtflx.carbon << " ";
  ofstate << cohort[pichrt].sconvrtflx.nitrogen << " ";
  
  ofstate << cohort[pichrt].slash.carbon << " ";
  ofstate << cohort[pichrt].slash.nitrogen << " ";

  ofstate << cohort[pichrt].slashpar << " "; 
   
  ofstate << cohort[pichrt].tillflag << " ";                           

  ofstate << cohort[pichrt].topt << " ";

  ofstate << cohort[pichrt].tqc << " ";

  ofstate << cohort[pichrt].vconvert << " "; 

  ofstate << cohort[pichrt].vconvrtflx.carbon << " ";
  ofstate << cohort[pichrt].vconvrtflx.nitrogen << " ";

  ofstate << cohort[pichrt].vrespar << " "; 

  ofstate << cohort[pichrt].yrltrc << " ";
  ofstate << cohort[pichrt].yrltrn << " ";
  ofstate << endl;
  
};

/* *************************************************************
************************************************************* */


/* *************************************************************
************************************************************* */

void MITelmnt43::writecflx( ofstream& ofile, 
                            string varname, 
                            const int& dyr,
                            const int& year, 
                            MITdata43 cflx[MAXRTIME][MXMITNLAT] )
{

  int dm;
  int dmlat;
  string mname;

  ofile.setf( ios::fixed, ios::floatfield );
  ofile.setf( ios::showpoint );
  ofile.precision(0);


  for( dm = 0; dm < CYCLE; ++dm )
  {
    switch( dm )
    {
      case 0:  mname = "JAN"; break;
      case 1:  mname = "FEB"; break;
      case 2:  mname = "MAR"; break;
      case 3:  mname = "APR"; break;
      case 4:  mname = "MAY"; break;
      case 5:  mname = "JUN"; break;
      case 6:  mname = "JUL"; break;
      case 7:  mname = "AUG"; break;
      case 8:  mname = "SEP"; break;
      case 9:  mname = "OCT"; break;
      case 10: mname = "NOV"; break;
      case 11: mname = "DEC"; break;
    }
    ofile << year << "  ";
    ofile << mname << "  ";
    ofile << varname << endl;;
    for ( dmlat = 0; dmlat < (MXMITNLAT/2); ++dmlat )
    {
      ofile << "  " << setprecision( 2 ) << setw( 8 );
      ofile << cflx[dyr][dm].latband[dmlat];
    }
    ofile << endl;

    for ( dmlat = (MXMITNLAT/2); dmlat < MXMITNLAT; ++dmlat )
    {
      ofile << "  " << setprecision( 2 ) << setw( 8 );
      ofile << cflx[dyr][dm].latband[dmlat];
    }
    ofile << endl;
  }

};