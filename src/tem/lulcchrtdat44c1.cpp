/* *************************************************************
LULCCHRTDAT44C1.CPP - object to read and write the structure of 
                     land use/land cover data from/to files

Modifications:

20110715 - DWK created by modifying lulcdat44a.cpp
20110715 - DWK changed include from lulcdat44a.h to 
           lulcchrtdat44c.h
20110715 - DWK changed Lulcdata44:: to LulcCohortdata44::
20140828 - DWK changed include lulcchrtdat44c.h to 
           lulcchrtdat44c1.h 
20140828 - DWK changed float fracLandArea to long cohortArea

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

#include<string>

  using std::string;
  

#include "lulcchrtdat44c1.h"


LulcCohortdata44::LulcCohortdata44( void )
{

  lulcend = 1;
  lagpos = -99;
  curpos = 0;

};

/* **************************************************************
                    Public Functions
************************************************************** */

/* *************************************************************
************************************************************* */

int LulcCohortdata44::get( ifstream& infile )
{

  lagpos = infile.tellg();

  infile >> col;
  infile >> row;
  infile >> varname;
  infile >> year;
  infile >> icohort;
  infile >> currentveg;
  infile >> cohortArea;
  infile >> region;

  infile.seekg( 0, ios::cur );

  curpos = infile.tellg();

  if( curpos < (lagpos + 10) ) { lulcend = -1; }

  return lulcend;

};

/* *************************************************************
************************************************************* */


/* *************************************************************
************************************************************* */

int LulcCohortdata44::getdel( FILE* infile )
{
  char tmpregion[80];

  char tmpvarname[40];


  lulcend = fscanf( infile, 
                    "%f,%f, %s ,%d,%d,%d,%ld, %s",
                    &col, 
                    &row, 
                    tmpvarname,
                    &year,                   
                    &icohort,
                    &currentveg,
                    &cohortArea,
                    tmpregion );

  
  varname = tmpvarname;

  region = tmpregion;
 
 
  return lulcend;

};

/* *************************************************************
************************************************************* */


/* *************************************************************
************************************************************* */

void LulcCohortdata44::out( ofstream& ofile,  
                            const float& col, 
                            const float& row, 
                            const string& varname,
                            const int& year,
                            const int& icohort,
                            const int& currentveg,
                            const long& cohortArea,
                            const string& region )
{

  ofile.setf( ios::fixed,ios::floatfield );
  ofile.setf( ios::showpoint );
  ofile.precision( 1 );

  ofile << col << ' ';
  ofile << row << ' ';
  ofile << varname << ' ';
  ofile << year << ' ';
  ofile << icohort << ' ';
  ofile << currentveg << ' ';
  ofile << cohortArea << ' ';
  ofile << region;
  ofile << endl;

};

/* *************************************************************
************************************************************* */


/* *************************************************************
************************************************************* */

void LulcCohortdata44::outdel( ofstream& ofile, 
                               const float& col, 
                               const float& row, 
                               const string& varname,
                               const int& year,
                               const int& icohort,
                               const int& currentveg,
                               const long& cohortArea,
                               const string& region )
{

  ofile.setf( ios::fixed,ios::floatfield );
  ofile.setf( ios::showpoint );
  ofile.precision( 1 );

  ofile << col << ",";
  ofile << row << ", ";
  ofile << varname << " ,";
  ofile << year << ",";
  ofile << icohort << ",";
  ofile << currentveg << ",";
  ofile << cohortArea << ", ";
  ofile << region;
  ofile << endl;

};

