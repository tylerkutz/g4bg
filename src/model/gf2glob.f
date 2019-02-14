!Reference:  L.W.Whitlow, SLAC-Report-357,                                      
!            Ph.D. Thesis, Stanford University,                                 
!            March 1990.                                                        
!For details see file HELP.DOCUMENT.                                            
                                                                                
!Program contains 145 lines of Fortran code, of 72 characters each, with        
!no subroutines.  Program requires File E.14 as input.                          
                                                                                
                                                                                
      SUBROUTINE gF2GLOB(X,Q2,Target,MODEL,F2)        
                                                                                
! Returns F2 and related quantities from the either the LAMBDA12 model          
! (MODEL=12), or the OMEGA9 model (MODEL=9), both of 27Jan90.                   
!                                                                               
! F2 Deuterium is for average nucleon. i.e. approximately (F2p +F2n)/2          
!                                                                               
! Further, program returns uncertainty in F2 based on both statistics           
! (ST) and systematic effects due to our choice of models (SY).                 
! Also, program calculates the slope d[F2]/d[logQ2] plus the statistical        
! uncertainty in this slope.                                                    
!                                                                               
! Best model is LAMBDA12.  SY is estimated to be the difference between         
! the two models.                                                               
!                                                                               
! Errors due to overall normalization are not included in program output        
! and they are:  +/- 2.1% for hydrogen and +/- 1.7% for deuterium.              
! Systematic errors due to radiative corrections are shown in Reference         
! to be very kinematic independent, and are everywhere <.5%, and thus,          
! are ignored by this routine (also see documentation to dFRC in file           
! HELP.DOCUMENT).                                                               
!                                                                               
! Coefficients and correlation matrix elements are from File                    
! E.13 F1990.MATRICES, dated 27Jan90.                                           
                                                                                
      IMPLICIT NONE                                                             
      LOGICAL GOODFIT,FIRST                                             
      REAL*8   X, Q2, F2,ST,SY,SLOPE,DSLOPE                               
      REAL*8   XPP,XP,Y,POLY,F2B,POL1,STB,Q,QTH,DQ,F2TH,QUAD,SCB,F2L      
      REAL*8   BINDING,STL                                                
      INTEGER MODEL, I, J, K                                                    
      CHARACTER*1 TARGET                                                        
      REAL*8    B(2,9),L(2,9,9)                      ! OMEGA9 variables         
      REAL*8    C(2,12),M(2,12,12)                   ! LAMBDA12 variable        
      REAL*8 V(9),Z(12),U(12),LIN                                               
                             
      FIRST = .true.                                                   
                                                                                
! Model #9  27 Jan 90.                            !  HYDROGEN Ci            
      DATA (B(1,J),J=1,9)/                        
     >   0.7338659870D0,  11.0245522588D0,   2.6185804129D0,                    
     >   4.0956321483D0,   0.1206495422D0,   1.9714128709D0,                    
     >   3.8893348719D0, -14.0507358314D0,   8.8080576075D0/
!                                                 !  HYDROGEN MijDiDj
      DATA ((L(1,J,K),K=1,9),J=1,9)/              
     >  0.0006676790D0,   0.0088218048D0,  -0.0007305188D0,                     
     > -0.0015980319D0,   0.0000814499D0,   0.0022889591D0,                     
     > -0.0153597481D0,   0.0257681937D0,  -0.0129827203D0,                     
     >  0.0088218048D0,   0.4084284036D0,   0.0479735629D0,                     
     >  0.0472083864D0,   0.0007306896D0,  -0.0267770531D0,                     
     >  0.0663676188D0,  -0.1319505427D0,   0.1028644511D0,                     
     > -0.0007305188D0,   0.0479735629D0,   0.0141871362D0,                     
     >  0.0188269696D0,  -0.0000772884D0,  -0.0209539831D0,                     
     >  0.1024234116D0,  -0.1688799776D0,   0.0910043198D0,                     
     > -0.0015980319D0,   0.0472083864D0,   0.0188269696D0,                     
     >  0.0264316633D0,  -0.0001541384D0,  -0.0321703747D0,                     
     >  0.1590906780D0,  -0.2577418883D0,   0.1356424745D0,                     
     >  0.0000814499D0,   0.0007306896D0,  -0.0000772884D0,                     
     > -0.0001541384D0,   0.0021536048D0,  -0.0190110257D0,                     
     >  0.0585567801D0,  -0.0758507669D0,   0.0352107941D0,                     
     >  0.0022889591D0,  -0.0267770531D0,  -0.0209539831D0,                     
     > -0.0321703747D0,  -0.0190110257D0,   0.2220310596D0,                     
     > -0.7858318126D0,   1.0974127015D0,  -0.5309260823D0,                     
     > -0.0153597481D0,   0.0663676188D0,   0.1024234116D0,                     
     >  0.1590906780D0,   0.0585567801D0,  -0.7858318126D0,                     
     >  2.9565217889D0,  -4.2563361422D0,   2.0922424569D0,                     
     >  0.0257681937D0,  -0.1319505427D0,  -0.1688799776D0,                     
     > -0.2577418883D0,  -0.0758507669D0,   1.0974127015D0,                     
     > -4.2563361422D0,   6.2376383315D0,  -3.1028661049D0,                     
     > -0.0129827203D0,   0.1028644511D0,   0.0910043198D0,                     
     >  0.1356424745D0,   0.0352107941D0,  -0.5309260823D0,                     
     >  2.0922424569D0,  -3.1028661049D0,   1.5586723492D0/                     
                                                                                
      DATA (B(2,J),J=1,9) /                       !  Deuterium Ci               
     >  0.6087459014D0,   8.4283440045D0,   1.8643042857D0,                     
     >  3.1298831009D0,   0.1952690820D0,   0.8207482504D0,                     
     >  3.2808011387D0,  -8.2972794804D0,   4.4892920417D0/                     
                                                                                
      DATA ((L(2,J,K),K=1,9),J=1,9)/              !  Deuterium MijDiDj          
     >  0.0004823134D0,   0.0055128707D0,  -0.0003158223D0,                     
     > -0.0008664550D0,   0.0000058824D0,   0.0013253049D0,                     
     > -0.0072791640D0,   0.0109300741D0,  -0.0049461930D0,                     
     >  0.0055128707D0,   0.2107333442D0,   0.0259720298D0,                     
     >  0.0248189032D0,   0.0007144468D0,  -0.0145424906D0,                     
     >  0.0405570442D0,  -0.0721227448D0,   0.0486265355D0,                     
     > -0.0003158223D0,   0.0259720298D0,   0.0068492388D0,                     
     >  0.0088813426D0,   0.0001809208D0,  -0.0091545289D0,                     
     >  0.0388897684D0,  -0.0588631696D0,   0.0295266467D0,                     
     > -0.0008664550D0,   0.0248189032D0,   0.0088813426D0,                     
     >  0.0124007760D0,   0.0002241085D0,  -0.0138537368D0,                     
     >  0.0599295961D0,  -0.0889074149D0,   0.0432637631D0,                     
     >  0.0000058824D0,   0.0007144468D0,   0.0001809208D0,                     
     >  0.0002241085D0,   0.0010114008D0,  -0.0090339302D0,                     
     >  0.0277972497D0,  -0.0356355323D0,   0.0162553516D0,                     
     >  0.0013253049D0,  -0.0145424906D0,  -0.0091545289D0,                     
     > -0.0138537368D0,  -0.0090339302D0,   0.0957852750D0,                     
     > -0.3188133729D0,   0.4239206981D0,  -0.1961729663D0,                     
     > -0.0072791640D0,   0.0405570442D0,   0.0388897684D0,                     
     >  0.0599295961D0,   0.0277972497D0,  -0.3188133729D0,                     
     >  1.1017824091D0,  -1.4925639539D0,   0.6968068686D0,                     
     >  0.0109300741D0,  -0.0721227448D0,  -0.0588631696D0,                     
     > -0.0889074149D0,  -0.0356355323D0,   0.4239206981D0,                     
     > -1.4925639539D0,   2.0479986415D0,  -0.9652124406D0,                     
     > -0.0049461930D0,   0.0486265355D0,   0.0295266467D0,                     
     >  0.0432637631D0,   0.0162553516D0,  -0.1961729663D0,                     
     >  0.6968068686D0,  -0.9652124406D0,   0.4591313874D0/                     
                                                                                
!    MODEL #12:    27Jan90.                                                     
      DATA (C(1,J),J=1,12)/                !     HYDROGEN Ci                    
     >  1.4168453160D0,  -0.1076464631D0,   1.4864087376D0,                     
     > -5.9785594887D0,   3.5240257602D0,  -0.0106079410D0,                     
     > -0.6190282831D0,   1.3852434724D0,   0.2695209475D0,                     
     > -2.1790402676D0,   4.7223977551D0,  -4.3633393929D0/                     
      DATA ((M(1,J,K),K=1,12),J=1,6)/     !     HYDROGEN MijDiDj               
     >  0.0014961921D0,  -0.0114525491D0,   0.0302843702D0,                     
     > -0.0334635318D0,   0.0132208899D0,   0.0000371728D0,                     
     > -0.0004173300D0,   0.0007986253D0,   0.0000132630D0,                     
     > -0.0000712621D0,  -0.0001056593D0,   0.0004288772D0,                     
     > -0.0114525491D0,   0.0967765603D0,  -0.2740561190D0,                     
     >  0.3184559770D0,  -0.1307971364D0,  -0.0011246012D0,                     
     >  0.0095305519D0,  -0.0155069847D0,  -0.0010495929D0,                     
     >  0.0090797755D0,  -0.0200963251D0,   0.0116773587D0,                     
     >  0.0302843702D0,  -0.2740561190D0,   0.8159191015D0,                     
     > -0.9844443599D0,   0.4163716693D0,   0.0049245087D0,                     
     > -0.0379185977D0,   0.0567662659D0,   0.0051689160D0,                     
     > -0.0439817571D0,   0.0995835938D0,  -0.0638367188D0,                     
     > -0.0334635318D0,   0.3184559770D0,  -0.9844443599D0,                     
     >  1.2221697276D0,  -0.5286404057D0,  -0.0072551971D0,                     
     >  0.0521650844D0,  -0.0735924860D0,  -0.0082081518D0,                     
     >  0.0683850387D0,  -0.1551044074D0,   0.1026791211D0,                     
     >  0.0132208899D0,  -0.1307971364D0,   0.4163716693D0,                     
     > -0.5286404057D0,   0.2327515525D0,   0.0034606631D0,                     
     > -0.0235526467D0,   0.0317074158D0,   0.0041807175D0,                     
     > -0.0342135427D0,   0.0775630764D0,  -0.0522714782D0,                     
     >  0.0000371728D0,  -0.0011246012D0,   0.0049245087D0,                     
     > -0.0072551971D0,   0.0034606631D0,   0.0006331410D0,                     
     > -0.0035750486D0,   0.0043493144D0,   0.0005207326D0,                     
     > -0.0035419381D0,   0.0068329087D0,  -0.0038428417D0/                     
        DATA ((M(1,J,K),K=1,12),J=7,12)/             ! hydrogen
     > -0.0004173300D0,   0.0095305519D0,  -0.0379185977D0,                     
     >  0.0521650844D0,  -0.0235526467D0,  -0.0035750486D0,                     
     >  0.0234071623D0,  -0.0312734982D0,  -0.0029088270D0,                     
     >  0.0220336426D0,  -0.0446325428D0,   0.0252355182D0,                     
     >  0.0007986253D0,  -0.0155069847D0,   0.0567662659D0,                     
     > -0.0735924860D0,   0.0317074158D0,   0.0043493144D0,                     
     > -0.0312734982D0,   0.0455043874D0,   0.0034940236D0,                     
     > -0.0283748709D0,   0.0601210472D0,  -0.0342674110D0,                     
     >  0.0000132630D0,  -0.0010495929D0,   0.0051689160D0,                     
     > -0.0082081518D0,   0.0041807175D0,   0.0005207326D0,                     
     > -0.0029088270D0,   0.0034940236D0,   0.0007624603D0,                     
     > -0.0058108049D0,   0.0129263887D0,  -0.0087097278D0,                     
     > -0.0000712621D0,   0.0090797755D0,  -0.0439817571D0,                     
     >  0.0683850387D0,  -0.0342135427D0,  -0.0035419381D0,                     
     >  0.0220336426D0,  -0.0283748709D0,  -0.0058108049D0,                     
     >  0.0487297250D0,  -0.1154000355D0,   0.0812897233D0,                     
     > -0.0001056593D0,  -0.0200963251D0,   0.0995835938D0,                     
     > -0.1551044074D0,   0.0775630764D0,   0.0068329087D0,                     
     > -0.0446325428D0,   0.0601210472D0,   0.0129263887D0,                     
     > -0.1154000355D0,   0.2885784358D0,  -0.2128276155D0,                     
     >  0.0004288772D0,   0.0116773587D0,  -0.0638367188D0,                     
     >  0.1026791211D0,  -0.0522714782D0,  -0.0038428417D0,                     
     >  0.0252355182D0,  -0.0342674110D0,  -0.0087097278D0,                     
     >  0.0812897233D0,  -0.2128276155D0,   0.1642123699D0/                     
                                                                                
      DATA (C(2,J),J=1,12)/                !     DEUTERIUM Ci                   
     >  0.9483220437D0,  -0.1153382195D0,   1.8614034534D0,                     
     > -4.7333791157D0,   2.3483754563D0,  -0.0651156444D0,                     
     > -0.2243092198D0,   1.0850340284D0,   0.2125643792D0,                     
     > -1.6872146840D0,   3.4085883231D0,  -3.2545701111D0/                     
      DATA ((M(2,J,K),K=1,12),J=1,6)/     !     DEUTERIUM MijDiDj              
     >  0.0007144431D0,  -0.0055332437D0,   0.0148345485D0,                     
     > -0.0166543296D0,   0.0066913067D0,  -0.0000063353D0,                     
     > -0.0000313908D0,   0.0001476921D0,  -0.0000519937D0,                     
     >  0.0004518877D0,  -0.0011993941D0,   0.0010410232D0,                     
     > -0.0055332437D0,   0.0464241060D0,  -0.1316100281D0,                     
     >  0.1539289430D0,  -0.0638038463D0,  -0.0004724619D0,                     
     >  0.0037853638D0,  -0.0060936945D0,  -0.0000911765D0,                     
     >  0.0007345446D0,  -0.0009520769D0,  -0.0006845386D0,                     
     >  0.0148345485D0,  -0.1316100281D0,   0.3889562708D0,                     
     > -0.4695521254D0,   0.1995114383D0,   0.0024459109D0,                     
     > -0.0172286634D0,   0.0247997549D0,   0.0014795243D0,                     
     > -0.0120036957D0,   0.0259982755D0,  -0.0151245338D0,                     
     > -0.0166543296D0,   0.1539289430D0,  -0.4695521254D0,                     
     >  0.5810365405D0,  -0.2518031702D0,  -0.0037864499D0,                     
     >  0.0248168137D0,  -0.0334709127D0,  -0.0029341015D0,                     
     >  0.0235187814D0,  -0.0525907667D0,   0.0342155275D0,                     
     >  0.0066913067D0,  -0.0638038463D0,   0.1995114383D0,                     
     > -0.2518031702D0,   0.1108885979D0,   0.0018333157D0,                     
     > -0.0113882800D0,   0.0146376694D0,   0.0016469653D0,                     
     > -0.0130947155D0,   0.0297048474D0,  -0.0201812916D0,                     
     > -0.0000063353D0,  -0.0004724619D0,   0.0024459109D0,                     
     > -0.0037864499D0,   0.0018333157D0,   0.0005976780D0,                     
     > -0.0033294157D0,   0.0040280997D0,   0.0004270733D0,                     
     > -0.0027573603D0,   0.0049156906D0,  -0.0024136903D0/                     
        DATA ((M(2,J,K),K=1,12),J=7,12)/             ! deuterium
     > -0.0000313908D0,   0.0037853638D0,  -0.0172286634D0,                     
     >  0.0248168137D0,  -0.0113882800D0,  -0.0033294157D0,                     
     >  0.0207148104D0,  -0.0268964589D0,  -0.0023283682D0,                     
     >  0.0162308979D0,  -0.0297645179D0,   0.0142701075D0,                     
     >  0.0001476921D0,  -0.0060936945D0,   0.0247997549D0,                     
     > -0.0334709127D0,   0.0146376694D0,   0.0040280997D0,                     
     > -0.0268964589D0,   0.0372995011D0,   0.0027664597D0,                     
     > -0.0203157999D0,   0.0385356275D0,  -0.0183131702D0,                     
     > -0.0000519937D0,  -0.0000911765D0,   0.0014795243D0,                     
     > -0.0029341015D0,   0.0016469653D0,   0.0004270733D0,                     
     > -0.0023283682D0,   0.0027664597D0,   0.0005581515D0,                     
     > -0.0041387256D0,   0.0089984380D0,  -0.0059280886D0,                     
     >  0.0004518877D0,   0.0007345446D0,  -0.0120036957D0,                     
     >  0.0235187814D0,  -0.0130947155D0,  -0.0027573603D0,                     
     >  0.0162308979D0,  -0.0203157999D0,  -0.0041387256D0,                     
     >  0.0334835563D0,  -0.0777433187D0,   0.0540437564D0,                     
     > -0.0011993941D0,  -0.0009520769D0,   0.0259982755D0,                     
     > -0.0525907667D0,   0.0297048474D0,   0.0049156906D0,                     
     > -0.0297645179D0,   0.0385356275D0,   0.0089984380D0,                     
     > -0.0777433187D0,   0.1924237194D0,  -0.1418467794D0,                     
     >  0.0010410232D0,  -0.0006845386D0,  -0.0151245338D0,                     
     >  0.0342155275D0,  -0.0201812916D0,  -0.0024136903D0,                     
     >  0.0142701075D0,  -0.0183131702D0,  -0.0059280886D0,                     
     >  0.0540437564D0,  -0.1418467794D0,   0.1109342554/                       
      !----------------------------------------------------------------         
      !----------------------------------------------------------------         
                                                                                
                                                                                                                          
      i = 1                                                                     
      IF (TARGET.EQ.'D') i = 2                                                  
      BINDING = 1./(1.-EXP(-MIN(20.0,7.7*(1./X+.93828**2/Q2-1.))))               
      IF (i.EQ.1) BINDING = 1.                                                  
                                                                                
      !OMEGA9 MODEL FIRST:                                                      
           XPP  = (Q2+B(i,1))/(Q2/X+B(i,2))                                     
           XP   = (Q2+B(i,3))/(Q2/X+B(i,4))                                     
           Y    = 1.-XP                                                         
           POLY = B(i,5)*Y**3+B(i,6)*Y**4+B(i,7)*Y**5+B(i,8)*Y**6+              
     >            B(i,9)*Y**7                                                   
           F2B  = X/XPP*BINDING*POLY                                            
          !-----------------------------------------------------------          
          !V(k) is the derivative of F_2 with respect to parameter k.           
           V(1) = -F2B/XPP/(Q2/X+B(i,2))                                        
           V(2) =  F2B/XPP/(Q2/X+B(i,2))**2*(Q2+B(i,1))                         
           POL1 =  3.*B(i,5)*Y**2+4.*B(i,6)*Y**3+5.*B(i,7)*Y**4+                
     >             6.*B(i,8)*Y**5+7.*B(i,9)*Y**6                                
           V(3) = -F2B*POL1/POLY/(Q2/X+B(i,4))                                  
           V(4) =  F2B*POL1/POLY/(Q2/X+B(i,4))**2*(Q2+B(i,3))                   
           DO 10 j = 5,9                                                        
10         V(j) =  F2B/POLY*Y**(j-2)                                            
           STB = 0.                                                             
           DO 11 j = 1,9                                                        
           DO 11 k = 1,9                                                        
11         STB = STB + L(i,j,k)*V(j)*V(k)                                       
           STB = SQRT(STB)*BINDING                                              
                                                                                
      !LAMBDA12 MODEL NEXT:                                                     
           Y    = 1.-X                                                          
           q    = LOG(Q2)                                                       
           qth  = .2+3.2*X                                                      
           dq   = q-qth                                                         
           F2th = C(i,1)*Y**3+C(i,2)*Y**4+C(i,3)*Y**5+                          
     >            C(i,4)*Y**6+C(i,5)*Y**7                                       
           QUAD = (C(i,6)+C(i,7)*X+C(i,8)*X**2)*dq**2                           
           LIN  = (C(i,9)+C(i,10)*X+C(i,11)*X**2+C(i,12)*X**3)*dq               
           IF (q.GT.qth) QUAD = 0.                                              
           SCB  = (1.+LIN+QUAD)                                                 
           F2L  = F2th*SCB*BINDING                                              
          !-----------------------------------------------------------          
          !Z(k) is the derivative of F_2 with respect to parameter k.           
           DO 20 j = 1,5                                                        
20         Z(j) = SCB*Y**(j+2)                                                  
           Z(6) = 0.                                                            
           IF (q.LT.qth) Z(6) = F2th*dq**2                                      
           Z(7) = Z(6)*X                                                        
           Z(8) = Z(6)*X**2                                                     
           DO 21 j = 9,12                                                       
21         Z(j) = F2th*X**(j-9)*dq                                              
           STL = 0.                                                             
           DO 22 j = 1,12                                                       
           DO 22 k = 1,12                                                       
22         STL = STL + M(i,j,k)*Z(j)*Z(k)                                       
           STL = SQRT(STL)*BINDING                                              
                                                                                
          !U(k) is the derivative of slope with respect to parameter k.         
           SLOPE= F2th*LIN/dq*BINDING                                           
           DO 30 j = 1,5                                                        
30         U(j) = LIN/dq*Y**(j+2)                                               
           DO 31 j = 6,8                                                        
31         U(j) = 0.                                                            
           DO 32 j = 9,12                                                       
32         U(j) = Z(j)/dq                                                       
           DSLOPE = 0.                                                          
           DO 33 j = 1,12                                                       
           DO 33 k = 1,12                                                       
33         DSLOPE = DSLOPE + M(i,j,k)*U(j)*U(k)                                 
           DSLOPE = SQRT(DSLOPE)                                                
      !----------------------------------------------------------------         
                                                                                
      F2 = 0.                                                                   
      ST = 0.                                                                   
      IF (MODEL.EQ. 9) THEN                                                     
           F2 = F2B                                                             
           ST = STB                                                             
      ELSEIF (MODEL.EQ.12) THEN                                                 
           F2 = F2L                                                             
           ST = STL                                                             
      ELSE                                                                      
           WRITE(*,'('' F1990: OOPS! MODEL.NE.9.AND.MODEL.NE.12'')')            
      ENDIF                                                                     
      SY = ABS(F2B-F2L)                                                         
                                                                                
      GOODFIT = .TRUE.                                                          
      !The following cuts define the region of applicability of F1990.          
      !In order they are:                                                       
      !     [radiative corrections convergence criteria in Q2] .and.            
      !     [radiative corrections convergence criteria in x]  .and.            
      !     [stay out of resonance region, W2.ge.3.0]          .and.            
      !     [limitation imposed by maximum beam energy].                        
      IF ((Q2.LT..566).OR.(X.LT..062).OR.(X.LT.Q2/(2.*.93828*21.))              
     >   .OR.(X.GT.1./((3.-.93828**2)/Q2+1.)))     THEN                         
                                                                                
c         WRITE(*,'('' GF2GLOB WARNING[F1990]: OUTSIDE RECOMMENDED RANGE.'')')          
c        print *, "Q2 = ", Q2, " X = ", X
          GOODFIT=.FALSE.                                                       
      ENDIF                                                                     
                                                                                
      RETURN                                                                    
      END                                                                       





