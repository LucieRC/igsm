c   These two common block are use to pass data from climate model to
C        TEM and back
      INTEGER no3, ndperm, ncoh, nhor, mnlay, mnlay1
      PARAMETER (no3=8,ndperm=31,ncoh=35,nhor=24,mnlay=6,mnlay1=10)
#if ( defined CLM35 )
      COMMON/wetfrac/wetfrac_data
      CHARACTER *256 wetfrac_data
#endif

      COMMON/climate4tem/ co24tem, o34tem, temp4tem, dtem4tem, sws4tem,
     &   pre4tem, strmdur, qstrm, 
#if ( defined CLM35 )
     &   pet, 
#endif

     &   aet, sh2o1m, sh2o2m, swe, 
     &   sfr, drn,
     &   daytsoil, daysho, rsh2o
      REAL*8 co24tem(jm0)
      REAL*8 o34tem(no3,jm0)
      REAL*8 temp4tem(jm0)
      REAL*8 dtem4tem(ndperm,jm0)
      REAL*8 sws4tem(jm0)
      REAL*8 pre4tem(jm0)
      REAL*8 strmdur(ndperm,jm0)
      REAL*8 qstrm(ndperm,jm0)
#if ( defined CLM35 )
      REAL*8 pet(ncoh,jm0)
#endif
      REAL*8 aet(ncoh,jm0)
      REAL*8 sh2o1m(ncoh,jm0)
      REAL*8 sh2o2m(ncoh,jm0)
      REAL*8 swe(ncoh,jm0)
      REAL*8 sfr(ncoh,jm0)
      REAL*8 drn(ncoh,jm0)
      REAL*8 daytsoil(ndperm,ncoh,jm0,mnlay1)
      REAL*8 daysho(ndperm,ncoh,jm0,mnlay1)
      REAL*8 rsh2o(nhor,ndperm,ncoh,jm0,mnlay)

#if ( defined CLM35 )
      COMMON/vegcoverclm/ vegfrac
      REAL*8 vegfrac(ncoh,jm0)

      COMMON/landcover4tem/ cellarea, cohortarea, landarea,
     &  Incohortarea, pctclay4tem, pctsand4tem, pctsilt4te,
     &  soilph4tem, ksat4tem, por4tem, zthick4tem
     & ,fixedLC4TEM
C      cellarea and landfrac are calculated by climate2tem
C      during first call
C      vegfrac  are calculated by clm
C      Incohortarea  are initialCohortArea from temstate file
      logical fixedLC4TEM
      REAL*8 cellarea(jm0)
      INTEGER*8 cohortarea(ncoh,jm0)
      INTEGER*8 landarea(jm0)
      INTEGER*8 Incohortarea(ncoh,jm0)
      REAL*8 pctclay4tem(jm0)
      REAL*8 pctsand4tem(jm0)
      REAL*8 pctsilt4te(jm0)
      REAL*8 soilph4tem(jm0)
      REAL*8 ksat4tem(mnlay1,jm0)
      REAL*8 por4tem(mnlay1,jm0)
      REAL*8 zthick4tem(mnlay1,jm0)
      
      REAL*8 croptotal(jm0)
      REAL*8 landfrac(jm0)
      CHARACTER *256 flsotem,fllatem
      CHARACTER *10 xx,yy,zz
      REAL*8 lattem(jm0)
      REAL*8 lontem(jm0)
      INTEGER*8 areatem(jm0),wsoiltem(jm0)
      REAL*8 claytem(jm0)
      REAL*8 sandtem(jm0)
      REAL*8 silttem(jm0)
      REAL*8 phtem(jm0)
      REAL*8 ksattem(mnlay1,jm0)
      REAL*8 portem(mnlay1,jm0)
      REAL*8 zthicktem(mnlay1,jm0)

#endif


      COMMON/upt4chem/ temco2, temch4, temn2o
      REAL*8 temco2(jm0)
      REAL*8 temch4(jm0)
      REAL*8 temn2o(jm0)

      COMMON/temtake/ adupt, antemnep, temnep
      REAL*8 adupt
      REAL*8 antemnep(jm0)
      REAL*8 temnep(12,jm0)

      COMMON/ozon4tem/ obso3, o3datadir, CLIMO3
      REAL*8 obso3(no3,jm0,12)
      CHARACTER *256 o3datadir
      LOGICAL CLIMO3
C
