/* **************************************************************
*****************************************************************
TIGSMSOIL44D1.H - object describing characteristics of soil used
	               by the Terrestrial Ecosystem Model (TEM)

Modifications:

20060126 - DWK created by modifying tsoil50b5.h 
20060126 - DWK changed include from temconts51.hpp to 
           temconsts43.hpp
20060126 - DWK changed include from tprocessXML51.h to
           tprocessXML437.h
20060126 - DWK deleted include qsoiltemp50b5.h
20060126 - DWK changed class Tsoil50 to class Tsoil43
20060126 - DWK changed inheritance from ProcessXML50 to 
           ProcessXML43
20060126 - DWK deleted public function double 
           updateActiveLayerRootZ(), inline double 
           getACTLAYER(), inline double getDST10(), inline 
           double getNEXTDST10(), inline double getPREVDST10()
           and inline double getTSOIL()
20060126 - DWK deleted  public Soilthermal50 stm, int stmflg,
           double yrdst10 and double yrtsoil                                                                            
20060126 - DWK deleted private double activeLayer, double dst10,
           double nextdst10, double prevdst10 and double tsoil
20060129 - DWK added public inline getVSM() and setVSM()
20060320 - DWK added private double surfrun and double drainage
20080130 - DWK changed include from temconsts43.hpp to
           temigsnconsts44a.hpp
20080130 - DWK changed include from tprocessXML447.h to
           tigsmprocessXML44a.h
20080130 - DWK changed ProcessXML43 to ProcessXML44
20110707 - DWK added inline functions getORGC(), setORGC(), 
           getORGN() and setORGN()
20110707 - DWK changed include from temigsmconsts44a.hpp to
           temigsmconsts44c.hpp
20110707 - DWK changed include from tigsmprocessXML44a.h to
           tigsmprocessXML44c.h
20140604 - DWK made all "yr" variables private
20150429 - DWK changed include temigsmconsts44c.hpp to
           temigsmconstants.hpp
20150429 - DWK changed include tigsmprocessXML44c.h to
           tigsmprocessXML44d1.h                      
****************************************************************
************************************************************* */

#ifndef TIGSMSOIL44D_H
#define TIGSMSOIL44D_H

// Tsoil44 uses the global constant MAXCMNT
#include "temigsmconstants.hpp"

#include "tigsmprocessXML44d1.h"

#include "bioms423.hpp"      // Tsoil44 uses Biomass class


class Tsoil44 : public ProcessXML44 
{

  public:
    
     Tsoil44( void );

/* **************************************************************
		 Public Functions
************************************************************** */

     void getecd ( ofstream& rflog1 );

     void getecd ( const string& ecd );

     void getrootz( ofstream& rflog1 );

     void getrootz( const string& ecd );

     void lake( const double& tair,
                const double& prec,
                double& rain,
                double& snowfall,
                const double& pet,
                double& eet );
    
     void resetMonthlyFluxes( void );
     
     void resetYrFluxes( void );

     void setKH2O( const double& vsm,
                   const int& moistlim );

     void showecd( void );

     double snowmelt( const double& elev,
                      const double& tair,
                      const double& prevtair,
                      const double& snowpack );
   
     void updateHydrology( const double& elev,
                           const double& tair,
                           const double& prevtair,
                           const double& prev2tair,
                           const double& rain,
                           const double& pet,
                           const double& sh2o,
                           const double& rgrndh2o,
                           const double& sgrndh2o,
                           const int& irrgflag,
                           double& irrigate,
                           const int& pdm );

     void updateNLosses( const int& pdcmnt,
                         const double& h2oloss,
                         const double& availn, 
                         const double& soilh2o );
                                   
     void updateRootZ( const int& pdcmnt );

     void xtext( const int& pdcmnt, 
                 const double& pctsilt, 
                 const double& pctclay );


     // "Get" and "Set" private variables and parameters
     
     // availn *************************************************
     
     inline double getAVLN( void ) { return availn; }

     inline void setAVLN( const double& pavln ) 
     { 
       availn = pavln; 
     }


     // avlh2o *************************************************
     
     inline double getAVLH2O( void ) { return avlh2o; }


     // awcapmm ************************************************
     
     inline double getAWCAPMM( void ) { return awcapmm; }

     inline void setAWCAPMM( const double& pawcapmm ) 
     { 
       awcapmm = pawcapmm; 
     }
   

     // drainage ***********************************************
     
     inline double getDRAINAGE( void ) { return drainage; }

     inline void setDRAINAGE( const double& pdrainage ) 
     { 
       drainage = pdrainage; 
     }


     // eet ****************************************************
     
     inline double getEET( void ) { return eet; }

     inline void setEET( const double& peet ) 
     { 
       eet = peet; 
     }


     // eetmx **************************************************
     
     inline double getEETMX( void ) { return eetmx; }

     inline void setEETMX( const double& peetmx ) 
     { 
       eetmx = peetmx; 
     }


     // fldcap *************************************************
     
     inline double getFLDCAP( void ) { return fldcap; }

    
     // fldcapa ************************************************
     
     inline double getFLDCAPA( void ) { return fldcapa; }

     inline void setFLDCAPA( const double& pfldcapa ) 
     { 
       fldcapa = pfldcapa; 
     }


     // fldcapb ************************************************
     
     inline double getFLDCAPB( void ) { return fldcapb; }

     inline void setFLDCAPB( const double& pfldcapb ) 
     { 
       fldcapb = pfldcapb; 
     }


     // h2oyld *************************************************
     
     inline double getH2OYLD( void ) { return h2oyld; }

     inline void setH2OYLD( const double& ph2oyld ) 
     { 
       h2oyld = ph2oyld; 
     }

     
     // ineet **************************************************
     
/*     inline double getINEET( void ) { return ineet; }

     inline void setINEET( const double& pineet ) 
     { 
       ineet = pineet; 
     }

*/
     // kh2o ***************************************************
     
     inline double getKH2O( void ) { return kh2o; }


     // minrootz ***********************************************
     
     inline double getMINROOTZ( const int& pcmnt ) 
     { 
       return minrootz[pcmnt]; 
     }

     inline void setMINROOTZ( const double& pminrootz,
                              const int& pcmnt ) 
     { 
       minrootz[pcmnt] = pminrootz; 
     }


     // moist **************************************************
     
     inline double getMOIST( void ) { return moist; }

     inline void setMOIST( const double& psh2o ) 
     { 
       moist = psh2o; 
     }


     // ninput *************************************************
     
     inline double getNINPUT( void ) { return ninput; }

     inline void setNINPUT( const double& pninput ) 
     { 
       ninput = pninput; 
     }


     // nloss **************************************************
     
     inline double getNLOSS( const int& pcmnt ) 
     { 
       return nloss[pcmnt]; 
     }

     inline void setNLOSS( const double& pnloss,
                           const int& pcmnt ) 
     { 
       nloss[pcmnt] = pnloss; 
     }


     // nlost **************************************************
     
     inline double getNLOST( void ) { return nlost; }

     inline void setNLOST( const double& pnlst ) 
     { 
       nlost = pnlst; 
     }


     // org.carbon *********************************************
     
     inline double getORGC( void ) { return org.carbon; }

     inline void setORGC( const double& porgc ) 
     { 
       org.carbon = porgc; 
     }

     // org.nitrogen *********************************************
     
     inline double getORGN( void ) { return org.nitrogen; }

     inline void setORGN( const double& porgn ) 
     { 
       org.nitrogen = porgn; 
     }

     // pctclay ************************************************
     
     inline double getPCTCLAY( void ) { return pctclay; }

     inline void setPCTCLAY( const double& ppctclay ) 
     { 
       pctclay = ppctclay; 
     }


     // pcfldcap ***********************************************
     
     inline double getPCTFLDCAP( void ) { return pcfldcap; }

     inline void setPCTFLDCAP( const double& ppctfldcap ) 
     { 
       pcfldcap = ppctfldcap; 
     }

     
     // pctp ***************************************************
     
     inline double getPCTP( void ) { return pctp; }

     inline void setPCTP( const double& ppctp ) 
     { 
       pctp = ppctp; 
     }


     // pctpora ************************************************
     
     inline double getPCTPORA( void ) { return pctpora; }

     inline void setPCTPORA( const double& ppctpora ) 
     { 
       pctpora = ppctpora; 
     }


     // pctporb ************************************************
     
     inline double getPCTPORB( void ) { return pctporb; }

     inline void setPCTPORB( const double& ppctporb ) 
     { 
       pctporb = ppctporb; 
     }


     // pctsand ************************************************
     
     inline double getPCTSAND( void ) { return pctsand; }

     inline void setPCTSAND( const double& ppctsand ) 
     { 
       pctsand = ppctsand; 
     }


     // pctsilt ************************************************
     
     inline double getPCTSILT( void ) { return pctsilt; }

     inline void setPCTSILT( const double& ppctsilt ) 
     { 
       pctsilt = ppctsilt; 
     }


     // prevspack **********************************************
     
     inline double getPREVSPACK( void ) { return prevspack; }

     inline void setPREVSPACK( const double& pprvspack ) 
     { 
       prevspack = pprvspack; 
     }


     // prveetmx ***********************************************
     
     inline double getPRVEETMX( void ) { return prveetmx; }

     inline void setPRVEETMX( const double& pprveetmx ) 
     { 
       prveetmx = pprveetmx; 
     }


     // psiplusc ***********************************************
     
     inline double getPSIPLUSC( void ) { return psiplusc; }

     inline void setPSIPLUSC( const double& ppsiplusc ) 
     { 
       psiplusc = ppsiplusc; 
     }


     // rootz **************************************************
     
     inline double getROOTZ( void ) { return rootz; }

     inline void setROOTZ( const double& prootz ) 
     { 
       rootz = prootz; 
     }


     // rootza *************************************************
     
     inline double getROOTZA( const int& pcmnt ) 
     { 
       return rootza[pcmnt]; 
     }

     inline void setROOTZA( const double& prootza,
                            const int& pcmnt ) 
     { 
       rootza[pcmnt] = prootza; 
     }


     // rootzb *************************************************
     
     inline double getROOTZB( const int& pcmnt ) 
     { 
       return rootzb[pcmnt]; 
     }

     inline void setROOTZB( const double& prootzb,
                            const int& pcmnt ) 
     { 
       rootzb[pcmnt] = prootzb; 
     }


     // rootzc *************************************************
     
     inline double getROOTZC( const int& pcmnt ) 
     { 
       return rootzc[pcmnt]; 
     }

     inline void setROOTZC( const double& prootzc,
                            const int& pcmnt ) 
     { 
       rootzc[pcmnt] = prootzc; 
     }


     // rperc **************************************************
     
     inline double getRPERC( void ) { return rperc; }


     // rrun ***************************************************
     
     inline double getRRUN( void ) { return rrun; }
 
  
     // snowinf ************************************************
     
     inline double getSNOWINF( void ) { return snowinf; }

     inline void setSNOWINF( const double& psnwinf ) 
     { 
       snowinf = psnwinf; 
     }

     // snowpack ***********************************************
     
     inline double getSNOWPACK( void ) { return snowpack; }

     inline void setSNOWPACK( const double& psnwpck ) 
     { 
       snowpack = psnwpck; 
     }


     // sperc **************************************************
     
     inline double getSPERC( void ) { return sperc; }

     // srun ***************************************************
     
     inline double getSRUN( void ) { return srun; }


     // surfrun ************************************************
     
     inline double getSURFRUN( void ) { return surfrun; }

     inline void setSURFRUN( const double& psurfrun ) 
     { 
       surfrun = psurfrun; 
     }


     // totpor *************************************************
     
     inline double getTOTPOR( void ) { return totpor; }

     inline void setTOTPOR( const double& ptotpor ) 
     { 
       totpor = ptotpor; 
     }


     // vsm *************************************************
     
     inline double getVSM( void ) { return vsm; }
     
     inline void setVSM( const double& pvsm ) 
     { 
       vsm = pvsm; 
     }


     // wiltpt *************************************************
     
     inline double getWILTPT( void ) { return wiltpt; }
     
     inline void setWILTPT( const double& pwiltpt ) 
     { 
       wiltpt = pwiltpt; 
     }


     // wiltpta ************************************************
     
     inline double getWILTPTA( void ) { return wiltpta; }

     inline void setWILTPTA( const double& pwiltpta ) 
     { 
       wiltpta = pwiltpta; 
     }


     // wiltptb ************************************************
     
     inline double getWILTPTB( void ) { return wiltptb; }

     inline void setWILTPTB( const double& pwiltptb ) 
     { 
       wiltptb = pwiltptb; 
     }


     // wsoil **************************************************
     
     inline int getWSOIL( void ) { return wsoil; }

     inline void setWSOIL( const int& pwsoil ) 
     { 
       wsoil = pwsoil; 
     }    


     // yravlh2o ************************************************
/*     
     inline double getYRAVLH2O( void ) { return yravlh2o; }

     inline void setYRAVLH2O( const double& pyravlh2o ) 
     { 
       yravlh2o = pyravlh2o; 
     }

     inline void updateYRAVLH2O( const double& pavlh2o ) 
     { 
       yravlh2o += pavlh2o; 
     }
*/

     // yravln ************************************************
     
     inline double getYRAVLN( void ) { return yravln; }

     inline void setYRAVLN( const double& pyravln ) 
     { 
       yravln = pyravln; 
     }

     inline void updateYRAVLN( const double& pavln ) 
     { 
       yravln += pavln; 
     }


     // yrc2n ************************************************
     
     inline double getYRC2N( void ) { return yrc2n; }

     inline void setYRC2N( const double& pyrc2n ) 
     { 
       yrc2n = pyrc2n; 
     }

     inline void updateYRC2N( const double& pc2n ) 
     { 
       yrc2n += pc2n; 
     }


     // yreet **********************************************
     
     inline double getYREET( void ) { return yreet; }

     inline void setYREET( const double& pyreet ) 
     { 
       yreet = pyreet; 
     }

     inline void updateYREET( const double& peet ) 
     { 
       yreet += peet; 
     }


     // yrh2oyld **********************************************
/*     
     inline double getYRH2OYIELD( void ) { return yrh2oyld; }

     inline void setYRH2OYIELD( const double& pyrh2oyld ) 
     { 
       yrh2oyld = pyrh2oyld; 
     }

     inline void updateYRH2OYIELD( const double& ph2oyld ) 
     { 
       yrh2oyld += ph2oyld; 
     }
*/

     // yrineet **********************************************
/*     
     inline double getYRINEET( void ) { return yrineet; }

     inline void setYRINEET( const double& pyrineet ) 
     { 
       yrineet = pyrineet; 
     }

     inline void updateYRINEET( const double& pineet ) 
     { 
       yrineet += pineet; 
     }
*/

     // yrnin **************************************************
     
     inline double getYRNINPUT( void ) { return yrnin; }

     inline void setYRNINPUT( const double& pyrnin ) 
     { 
       yrnin = pyrnin; 
     }

     inline void updateYRNINPUT( const double& pnin ) 
     { 
       yrnin += pnin; 
     }


     // yrnlost **************************************************
     
     inline double getYRNLOST( void ) { return yrnlost; }

     inline void setYRNLOST( const double& pyrnlost ) 
     { 
       yrnlost = pyrnlost; 
     }

     inline void updateYRNLOST( const double& pnlost ) 
     { 
       yrnlost += pnlost; 
     }
 

     // yrorgc **********************************************
     
     inline double getYRORGC( void ) { return yrorgc; }

     inline void setYRORGC( const double& pyrorgc ) 
     { 
       yrorgc = pyrorgc; 
     }

     inline void updateYRORGC( const double& porgc ) 
     { 
       yrorgc += porgc; 
     }


     // yrorgn **********************************************
     
     inline double getYRORGN( void ) { return yrorgn; }

     inline void setYRORGN( const double& pyrorgn ) 
     { 
       yrorgn = pyrorgn; 
     }

     inline void updateYRORGN( const double& porgn ) 
     { 
       yrorgn += porgn; 
     }


     // yrpctp **********************************************
 /*    
     inline double getYRPCTP( void ) { return yrpctp; }

     inline void setYRPCTP( const double& pyrpctp ) 
     { 
       yrpctp = pyrpctp; 
     }

     inline void updateYRPCTP( const double& ppctp ) 
     { 
       yrpctp += ppctp; 
     }
*/

     // yrrgrndh2o **********************************************
 /*    
     inline double getYRRGRNDH2O( void ) { return yrrgrndh2o; }

     inline void setYRRGRNDH2O( const double& pyrrgrndh2o ) 
     { 
       yrrgrndh2o = pyrrgrndh2o; 
     }

     inline void updateYRRGRNDH2O( const double& prgrndh2o ) 
     { 
       yrrgrndh2o += prgrndh2o; 
     }
*/

     // yrrperc **********************************************
 /*    
     inline double getYRRPERC( void ) { return yrrperc; }

     inline void setYRRPERC( const double& pyrrperc ) 
     { 
       yrrperc = pyrrperc; 
     }

     inline void updateYRRPERC( const double& prperc ) 
     { 
       yrrperc += prperc; 
     }
*/

     // yrrrun **********************************************
/*     
     inline double getYRRRUN( void ) { return yrrrun; }

     inline void setYRRRUN( const double& pyrrrun ) 
     { 
       yrrrun = pyrrrun; 
     }

     inline void updateYRRRUN( const double& prrun ) 
     { 
       yrrrun += prrun; 
     }
*/

     // yrsgrndh2o **********************************************
/*     
     inline double getYRSGRNDH2O( void ) { return yrsgrndh2o; }

     inline void setYRSGRNDH2O( const double& pyrsgrndh2o ) 
     { 
       yrsgrndh2o = pyrsgrndh2o; 
     }

     inline void updateYRSGRNDH2O( const double& psgrndh2o ) 
     { 
       yrsgrndh2o += psgrndh2o; 
     }
*/

     // yrsmoist **********************************************
/*     
     inline double getYRSMOIST( void ) { return yrsmoist; }

     inline void setYRSMOIST( const double& pyrsmoist ) 
     { 
       yrsmoist = pyrsmoist; 
     }

     inline void updateYRSMOIST( const double& psmoist ) 
     { 
       yrsmoist += psmoist; 
     }
*/

     // yrsnowinf **********************************************
/*     
     inline double getYRSNOWINF( void ) { return yrsnowinf; }

     inline void setYRSNOWINF( const double& pyrsnowinf ) 
     { 
       yrsnowinf = pyrsnowinf; 
     }

     inline void updateYRSNOWINF( const double& psnowinf ) 
     { 
       yrsnowinf += psnowinf; 
     }
*/

     // yrsnowpack **********************************************
/*     
     inline double getYRSNOWPACK( void ) { return yrsnowpack; }

     inline void setYRSNOWPACK( const double& pyrsnowpack ) 
     { 
       yrsnowpack = pyrsnowpack; 
     }

     inline void updateYRSNOWPACK( const double& psnowpack ) 
     { 
       yrsnowpack += psnowpack; 
     }
*/

     // yrsperc **********************************************
/*     
     inline double getYRSPERC( void ) { return yrsperc; }

     inline void setYRSPERC( const double& pyrsperc ) 
     { 
       yrsperc = pyrsperc; 
     }

     inline void updateYRSPERC( const double& psperc ) 
     { 
       yrsperc += psperc; 
     }
*/

     // yrsrun **********************************************
/*     
     inline double getYRSRUN( void ) { return yrsrun; }

     inline void setYRSRUN( const double& pyrsrun ) 
     { 
       yrsrun = pyrsrun; 
     }

     inline void updateYRSRUN( const double& psrun ) 
     { 
       yrsrun += psrun; 
     }
*/

     // yrvsm **********************************************
/*     
     inline double getYRVSM( void ) { return yrvsm; }

     inline void setYRVSM( const double& pyrvsm ) 
     { 
       yrvsm = pyrvsm; 
     }

     inline void updateYRVSM( const double& pvsm ) 
     { 
       yrvsm += pvsm; 
     }
*/   
   
   private:
   
/* *************************************************************
		                   Private Functions
************************************************************* */

     void percol( const double& rain,                  
                  const double& avlh2o );

     double rrunoff( const double& rgrndh2o );

     double srunoff( const double& elev,
                     const double& tair,
                     const double& prevtair,
                     const double& prev2tair,
		     const double& sgrndh2o );

     /* Estimated "actual" evapotransipiration (i.e. EET) as 
        described in Vorosmarty et al. (1989) Global 
        Biogeochemical Cycles 3: 241-265.  */

     double xeet( const double& rain,
                  const double& pet,
                  const double& avlh2o,
                  const int& pdm );


/* **************************************************************
		 Private Variables
************************************************************** */
     
     // Available inorganic nitrogen (g N / sq. meter)
     double availn;

     // Available water (mm)
     double avlh2o;  

     // Available water capacity (mm)
     double awcapmm;        

     // Monthly drainage (mm / month)
     double drainage;

     // Monthly estimated actual Evapotranspiration (mm / month)
     double eet;

     // Maximum EET of current year (mm / month)
     double eetmx;

     // Volume of water at field capacity (mm)
     double fldcap;         

      // Water yield (mm / month)
     double h2oyld;  

     // Initial Estimated Actual Evapotranspiration (mm / month)
//     double ineet;

     // Relative hydraulic conductivity through soil profile
     double kh2o;
     
     // Mean annual volumetric soil moisture (%)
     double meanvsm;

     // Soil moisture (mm)
     double moist;   

     double ndays[CYCLE];

     // Total nitrogen input to soils
     double ninput;    // (g N / (sq. meter * month)) 

     // Total nitrogen lost from soils
     double nlost;     // (g N / (sq. meter * month))

      // Reactive soil organic matter
     Biomass org;      //  (g C or g N / sq. meter)

     // Soil moisture as %field capacity
     double pcfc;    

     // Percent clay in soil texture
     double pctclay;        

     // Soil moisture as %total pore space
     double pctp;    

     // Percent sand in soil texture
     double pctsand;        
     
     // Percent silt in soil texture
     double pctsilt;

     // Previous month's snow pack
     double prevspack;

     // Maximum EET of previous year (mm / month)
     double prveetmx;
             
     // Proportion silt and clay in soil texture
     double psiplusc;       

     // Rain runoff storage (mm / month)
     double rgrndh2o;

     // Rain percolation (excess)
     double rperc;   // (mm / month)

     // Rain Runoff (mm / month)
     double rrun;

     // Snowmelt runoff storage (mm)
     double sgrndh2o;

     // Snow melt infiltration (mm / month)
     double snowinf; 

     // Snowpack (mm)
     double snowpack;

     // Snow melt percolation (excess)
     double sperc;   // (mm / month)

     // Snow runoff
     double srun;    // (mm / month)

     // Surface runoff (mm / month)
     double surfrun;

     // Soil texture (categorical data)
     int text;              

     // volume of total pore space (mm)
     double totpor;         

     // Volumetric soil moisture (as %rooting depth)
     double vsm;     
 
     // Volume of water at wilting point (mm)
     double wiltpt;         

     // wetland soil type designation (categorical data)
     int wsoil;

     // Annual sum of avlh2o
     double yravlh2o;

     // Annual sum of availn
     double yravln;

     // Ratio of soil reactive organic carbon to 
     //   soil reactive organic nitrogen
     double yrc2n;

     // Annual estimated actual evapotranspiration (mm / year)
     double yreet;

     // Annual sum of h2oyld (mm / year)
     double yrh2oyld;

     // Annual initial estimated actual evapotranspiration
     //   (mm / year)
 //    double yrineet;

     // Annual sum of ninput
     double yrnin;     // (g N / (sq. meter * year))

     // Annual sum of nlost
     double yrnlost;   // (g N / (sq. meter * year))

     // Annual sum of org.carbon 
     double yrorgc;
     
     // Annual sum of org.nitrogen 
     double yrorgn;

     // Annual sum of pctp
     double yrpctp;

     // Annual sum of rgrdnh2o
     double yrrgrndh2o;

     // Annual sum of rperc
     double yrrperc;   // (mm / year)

     // Annual sum of rrun
     double yrrrun;  // (mm / year)

     // Annual sum of sgrndh2o
     double yrsgrndh2o;

     // Annual sum of moist
     double yrsmoist;

     // Annual sum of snowinf
     double yrsnowinf;      // (mm / year)

     // Annual sum of snowpack
     double yrsnowpack;

     // Annual sum of sperc
     double yrsperc; // (mm / year)

     // Annual sum of srun
     double yrsrun;  // (mm / year)

     // Annual sum of vsm
     double yrvsm;


/* *************************************************************
		 Private Parameters
************************************************************* */

     // Field capacity (%soil volume)

     double fldcapa;
     double fldcapb;
     double pcfldcap;
       

     // Proportion of available nitrogen lost from soil
     //   (g N / (square meter))
     
     double nloss[MAXCMNT];


     // Porosity of soil (%soil volume)

     double pctpor;
     double pctpora;
     double pctporb;


     // Effective rooting depth (m)

     double minrootz[MAXCMNT];
     double rootz;
     double rootza[MAXCMNT];
     double rootzb[MAXCMNT];
     double rootzc[MAXCMNT];


     // Wilting point (%soil volume)

     double pcwiltpt;
     double wiltpta;
     double wiltptb;
     
};

#endif
