/* *************************************************************
MITDATA44C.CPP - object to read and write CO2, cloudiness, 
                 temperature and precipitation data from the 
                 2-D MIT L-O climate model

Modifications:

20060128 - DWK created by modifying mitdata436.cpp 
20060128 - DWK added include mitdata437.h and standard includes
20060128 - DWK changed char mname[4] to string mname in 
           get() and writeclm()
20080130 - DWK changed include from mitdata437.h to mitdata44a.h  
20080130 - DWK changed MITdata43:: to MITdata44::
20110707 - DWK changed include mitdata44a.h to mitdata44c.h
20150428 - DWK changed include from mitdata44c.h to 
           mitdata44d1.h
         
****************************************************************
************************************************************* */

#include<cstdio>

  using std::fscanf;
  using std::FILE;

#include<iostream>

  using std::ios;
  using std::endl;

#include<fstream>

  using std::ifstream;
  using std::ofstream;

#include<iomanip>

  using std::setprecision;
  using std::setw;

#include<string>

  using std::string;
  

#include "mitdata44d1.h"


/* *************************************************************
************************************************************* */

MITdata44::MITdata44( void ) { };


/* *************************************************************
************************************************************* */

void MITdata44::get( ifstream& ifile )
{
  int dmlat;

  string mname;

  ifile >> year;

  ifile >> mname;

  if ( "JAN" == mname ) { mon = 0; }
  if ( "FEB" == mname ) { mon = 1; }
  if ( "MAR" == mname ) { mon = 2; }
  if ( "APR" == mname ) { mon = 3; }
  if ( "MAY" == mname ) { mon = 4; }
  if ( "JUN" == mname ) { mon = 5; }
  if ( "JUL" == mname ) { mon = 6; }
  if ( "AUG" == mname ) { mon = 7; }
  if ( "SEP" == mname ) { mon = 8; }
  if ( "OCT" == mname ) { mon = 9; }
  if ( "NOV" == mname ) { mon = 10; }
  if ( "DEC" == mname ) { mon = 11; }
  
  ifile >> varname;

  for( dmlat = 0; dmlat < MXMITNLAT; ++dmlat )
  {
    ifile >> latband[dmlat];
  }

  ifile.seekg( 0, ios::cur );

};

/* *************************************************************
************************************************************* */


/* *************************************************************
************************************************************* */

void MITdata44::writeclm( ofstream& ofile, 
                          string varname, 
                          const int& dm, 
                          const MITdata44& clm )
{
  int dmlat;

  string mname;

  ofile.setf( ios::fixed,ios::floatfield );
  ofile.setf( ios::showpoint );
  ofile.precision( 0 );

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

  ofile << clm.year << "  ";

  ofile << mname << "  ";

  ofile << varname << endl;
  
  for( dmlat = 0; dmlat < (MXMITNLAT/2); ++dmlat )
  {
    ofile << "  " << setprecision( 2 ) << setw( 8 ) << clm.latband[dmlat];
  }

  ofile << endl;

  for( dmlat = (MXMITNLAT/2); dmlat < MXMITNLAT; ++dmlat )
  {
    ofile << "  " << setprecision( 2 ) << setw( 8 ) << clm.latband[dmlat];
  }

  ofile << endl;

};
