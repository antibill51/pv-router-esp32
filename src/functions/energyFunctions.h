#ifndef ENERGY_FUNCTIONS
  #define ENERGY_FUNCTIONS

  #include "../config/enums.h"
  #include "../config/config.h"

  extern DisplayValues gDisplayValues;
  extern Config config; 
  extern Logs logging;

  // ***********************************
  // ** recherche du point 0. temps 20 ms max ... 
  // ***********************************
  unsigned long startMillis,stopMillis;
  int middle_debug ; 



  // ***********************************
  // ** boucle de syncho temps
  // ***********************************

  void rt_loop (unsigned long startmicro, unsigned long timer )
  {

  while ( micros() <= ( startmicro + timer ) )  /// a vérifier si < permetrait encore d'affiner la mesure 
      {
      delayMicroseconds(3);  // J'ai lu qu'en dessous de 3, les délais ne sont pas respecté.
      }

  
  }

  #ifndef HARDWARE_MOD
  void front() {
    int Watchdog=0 ;  
    gDisplayValues.porteuse = true; 
    int margin = 0; /// 0 marge de bruit de fond  en lecture ( si pb ) 
    // recherche demi ondulation
    while ( analogRead(ADC_PORTEUSE) > margin  ) { 
      delayMicroseconds (3);
      Watchdog++;
      if ( Watchdog > 2500  ) {  Serial.print("Attention pas de porteuse, alimentation 12v AC ou pont redresseur débranché ? "); gDisplayValues.porteuse = false; break; } 

    }

    Watchdog=0 ; 
    // recherche d'un 0
    
    while ( analogRead(ADC_PORTEUSE) == margin  ) {
    delayMicroseconds (3);  
      Watchdog++;
      if ( Watchdog > 2500  ) {  Serial.print("Attention pas de porteuse, alimentation 12v AC ou pont redresseur débranché ou inversée ? "); gDisplayValues.porteuse = false ; break;}  
    }
      
  }




    // ***********************************
    // recherche d'un niveau d'injection
    // ***********************************
    // int middle = 0;
    // int middle_count = 0; 
    int start = 0 ;


    const int nbmesure = 72 ; /// nombre de mesure par ondulation
    const int nombre_cycle  = 4 ; /// nombre de cycle pour affiner la mesure
    const int freqmesure = nbmesure*(nombre_cycle+1) ;  // nombre total de mesures
    int tableau[freqmesure]; // mesure ADC Ampères
    int porteuse[freqmesure]; // mesure ADC Volts

    //float constante_voltage = 4.33; 


    void injection2(){ 

    int temp_read ; 
    // int temp_porteuse ;
      double sigma_read = 0 ; 
      //double voltage = 0; // test de calcul de voltage
      //String porteuse = "" ;
      int zero = 0; 
      double positive = 0 ; 
      //int zero_count = 0; 
      int loop = 0;  int inter=0;
      
      int wait_time = 277 ; //  --> 1/50Hz -> /2 -> 10ms --> 18 mesures --> 555 us 
      unsigned long startMillis,stopMillis;



    front();  ///synchro porteuse.
    delay (15);
    startMillis = micros();   // 0ms 

    ///// construction du tableau de mesures  /////
    while (loop < freqmesure ) {
        tableau[loop] =  analogRead(ADC_INPUT);
        porteuse[loop] =  analogRead(ADC_PORTEUSE);
        sigma_read += tableau[loop]; 
        // voltage += porteuse[loop]; // test de calcul de voltage
        
    /*    if (temp_porteuse == 0 ) { // 2eme 1/2 mesure  10ms 
          zero += temp_read * temp_read ; 
          zero_count ++; 
          injection = 1 ;
        }
        else {  // 1ere 1/2 mesure  10ms 
          if ( injection == 1 ) { injection =2 ; break ; }
          positive += temp_read * temp_read ;
          inter=loop;
        }
    */
    /*  if ( loop < 37 ) {
        positive += temp_read * temp_read ;
        inter=loop;
      }
      else { 
          zero += temp_read * temp_read ; 
          zero_count ++; 
      //   injection = 1 ;
      }
    */
    
      loop ++; 
    
      rt_loop( startMillis, wait_time*loop ) ; // attend le prochain top temps de mesure ( 277us * loop )
      
      }
      ///// fin  construction du tableau de mesures  /////

    // 20ms * nombre de cycles -> total 72 mesures * nb cycles
    stopMillis = micros();  

    // début des calculs 

    sigma_read = ( sigma_read / ( loop +1 ) ) ;  /// calcul du sigma ( la valeur moyenne ), ne marche bien que dans le cas d'une ondulation symétrique. A voir si nouvelle méthode de calcul. ( valeur théorique : 2047 -> vcc/2)
    // voltage = ( voltage / (loop + 1 )) ; // test de calcul de voltage
    int start=0; 
    int end=0; // utile pour calcul half automatique
    // int voltage_read = int(voltage/constante_voltage) ; // test de calcul de voltage ... pas de réel variation 

    //// test de calcul de voltage
    /*Serial.println("voltage");
    Serial.println(voltage);
    Serial.println(voltage_read);*/



    for(int i = 0; i < loop; i++)
    {
      if ( start == 0 ) {  if ( porteuse[i] > 0 ) { 
        start = i ; 
        //if ( voltage_read < porteuse[i] ) {voltage_read = porteuse[i] ; }
        //Serial.println(String(tableau[i])); }
        }   
      //else { 
      //Serial.println(String(tableau[i])); 
      } 

    }
    ///Serial.println (voltage_read);


    




    for (int j = 0 ; j < nombre_cycle   ; j++) 
    {
      #ifndef HALF_AUTO
        int phi = config.cosphi ;
        if (phi > start ) { half = start ; } // renommage phi en half, plus parlant a mon sens. Peut être modifier également dans l'interface web?

      #else
        for(int i =(0 +(j*nbmesure)); i < (nbmesure+(j*nbmesure)); i++) {
          if ( porteuse[i] !=0  && start == 0 ) {start =( i - (j*nbmesure)) ;} // possibilité d'ajouter à start une valeur (exemple 1) a définir dans l'interface web
          if ( porteuse[i] ==0 && start != 0 && end == 0 ) {end = (i -(j*nbmesure)) ;}  // possibilité de soustraire à stop une valeur (exemple 1) a définir dans l'interface web
        }
        half = (nbmesure/2) - ( end - start );   
      #endif

      for(int i = (start - half ) ; i < ( start + nbmesure - half )  ; i++)
      {
        if ( i <  ( start + ( nbmesure / 2 ) - half ) ) { // Je n'utilise plus cette méthode de calcul (HARDWARE_MOD) mais je me demande si half ne devrait pas lui aussi être divié par 2? 
        temp_read = (tableau[i+(j*nbmesure)]-sigma_read) ; 
        positive += temp_read  ; 
            if ( tableau[i+(j*nbmesure)] < 5 ) { zero ++ ;}    //détection des zeros ( prise débranchée )
        }
        else {
        temp_read = (sigma_read-tableau[i+(j*nbmesure)]) ; 
        positive += temp_read ; 
            if ( tableau[i+(j*nbmesure)] < 5 ) { zero ++ ;}  //détection des zeros ( prise débranchée )
        }
      }
    }


    //Serial.println(int(positive/10000*voltage_read)) ;// test de calcul de voltage



    //positive = ( positive / ( config.facteur * nombre_cycle * 230 / config.voltage ) ) + config.offset ; 
    positive = ( ( positive * config.voltage ) / ( config.facteur * nombre_cycle * 230 ) ) + config.offset ; 


    if ( zero > 75 ) { 
      if (logging.sct) { logging.start += "--> SCT013 Prob not connected  ?\r\n" ; logging.sct = false; }
    }
    //logging.start += "zero detected : " + String(zero) +   "\r\n" ;


    bool nolog =false; 
    if (nolog) {
    Serial.println("fin tableau");
    Serial.println("temp");
    Serial.println(wait_time*loop);
    Serial.println(stopMillis-startMillis);
    Serial.println("inter");
    Serial.println(inter*wait_time);
    Serial.println("middle");
    Serial.println(loop);
    Serial.println(sigma_read);
    Serial.println(start);
    Serial.println(int(positive)) ;
    }
    middle_debug= sigma_read; 

    //zero = sqrt(zero / float(zero_count)); 
    // positive = sqrt(positive / float( loop - zero_count )) ;

    gDisplayValues.watt = int(( positive )) ; 
    if ( config.polarity == true ) { gDisplayValues.watt = - gDisplayValues.watt ; }

    }

  #else
    void frontmod(){
      int Watchdog = 0;
          int temp_value0 = analogRead(ADC_MIDDLE);  //Mean value. Should be at 3.3v/2
          value0 = value0 + PHASECAL * (temp_value0 - value0); // Lissage des valeurs

          startV = analogRead(ADC_PORTEUSE);

      while ( startV< (value0*0.95 )) { 
          delayMicroseconds (5);
          startV = analogRead(ADC_PORTEUSE);
          Watchdog++;
          if ( Watchdog > 2000  ) {  Serial.print("Attention pas de porteuse, alimentation 12v AC ou pont redresseur débranché ? "); gDisplayValues.porteuse = false; break; } 

        }
        startMillis = micros();   // 0ms 
        while(1)                                   //the while loop...
        {
          // if ((startV < (ADC_COUNTS*0.50)) && (startV > (ADC_COUNTS*0.45))) 
          if ((startV < value0) && (startV > (value0*0.95))) 

          {
        //   logging.start +="porteuse OK \r\n" ;

            gDisplayValues.porteuse = true; 
            break;  //check its within range
          }
          startV = analogRead(ADC_PORTEUSE);                    //using the voltage waveform

          if ((millis()-startMillis)>timeout) {Serial.print("Attention pas milieu trouvé, alimentation 12v AC ou pont redresseur débranché ? ");  break;}
        }
    }

    void injection3(){ 
    double temp_read;
    double temp_tension;
    unsigned int crossCount = 0;                             //Used to measure number of times threshold is crossed.
    unsigned int numberOfSamples = 0;                        //This is now incremented
    const int nombre_cycle  = 4 ; /// nombre de cycle pour affiner la mesure

    int wait_time = 277 ; //  --> 1/50Hz -> /2 -> 10ms --> 18 mesures --> 555 us 
    unsigned long startMillis;


    bool lastVCross; 
    bool checkVCross;                  //Used to measure number of times threshold is crossed.

    Irms = 0;
    Vrms = 0;
    PW = 0;

    frontmod();

    startMillis = micros();   // 0ms 
    while ((crossCount <= (nombre_cycle*2))) // && ((millis()-startMillis)<timeout))
    {
      numberOfSamples++;                       //Count number of times looped.
    ///// construction du tableau de mesures  /////

        temp_read =  analogRead(ADC_INPUT) - value0;
        temp_tension = analogRead(ADC_PORTEUSE) - value0;
        
        if (crossCount>0) // On oublie volontairement le 1er cycle
        {
          temp_read = temp_read / config.facteur ; 
          Irms += sq(temp_read);

          temp_tension = temp_tension / config.voltage;
          Vrms += sq(temp_tension); // carré des mesures

          PW += temp_tension * temp_read;
        }

        lastVCross = checkVCross;
        if (temp_tension >= 0) {
          checkVCross = true;}
        else {checkVCross = false;}
        if (numberOfSamples==1) lastVCross = checkVCross;
        
        if (lastVCross != checkVCross) {
          crossCount++;
          if (crossCount ==1) {
            numberOfSamples = 0;
            startMillis = micros();   // 0ms 
          }
        }
        rt_loop( startMillis, (wait_time * numberOfSamples) ) ; // attend le prochain top temps de mesure ( 277us * loop )
      }
      ///// fin  construction du tableau de mesures  /////

    Vrms = Vrms / numberOfSamples;
    Vrms = sqrt(Vrms);
    Vrms = VrmsOLD + PHASECAL * (Vrms - VrmsOLD); // Lissage des valeurs
    VrmsOLD = Vrms;

    Irms = Irms / numberOfSamples;
    Irms = sqrt(Irms);

    PVA = floor(Vrms * Irms);

    PW =  (PW/numberOfSamples) + config.offset ; 

    PowerFactor = floor(100 * abs(PW) / PVA) / 100;

    // positive = PW ; 

    middle_debug= value0; 

    gDisplayValues.watt = int( PW ) ; 
    if ( config.polarity == true ) { gDisplayValues.watt = - gDisplayValues.watt ; }

    }
  #endif

int half = 0 ;
double Vrms = 0;
double Irms = 0;

void injection3(){ 

int temp_read ; 
double sigma_read = 0 ; 

int zero = 0; 
double positive = 0 ; 

int loop = 0; int inter=0;

int wait_time = 277 ; //  --> 1/50Hz -> /2 -> 10ms --> 18 mesures --> 555 us 
unsigned long startMillis,stopMillis;

// int nbmesure=72 ;  /// nombre de mesure par ondulation
// int nombre_cycle = 4 ;  
// int freqmesure = nbmesure*(nombre_cycle+1) ;  // nombre total de mesures


front();  ///synchro porteuse.
delay (15);
startMillis = micros();   // 0ms 

///// construction du tableau de mesures  /////
while (loop < freqmesure ) {
    tableau[loop] =  analogRead(ADC_INPUT);
    porteuse[loop] =  analogRead(ADC_PORTEUSE);
    sigma_read += tableau[loop]; 
 
   loop ++; 
 
   rt_loop( startMillis, wait_time*loop ) ; // attend le prochain top temps de mesure ( 277us * loop )
   
  }
  ///// fin  construction du tableau de mesures  /////

// 20ms * nombre de cycles -> total 72 mesures * nb cycles
stopMillis = micros();  

// début des calculs 

sigma_read = ( sigma_read / ( loop +1 ) ) ;  /// calcul du sigma ( la valeur moyenne ), ne marche bien que dans le cas d'une ondulation symétrique. A voir si nouvelle méthode de calcul. ( valeur théorique : 2047 -> vcc/2)

int nbV = 0;
start=0;
int end=0;
int courant = 0;
int tension = 0;


for (int j = 0 ; j < nombre_cycle ; j++) 
{
  for(int i =(0 +(j*nbmesure)); i < (nbmesure+(j*nbmesure)); i++) {
    if ( porteuse[i] !=0  && start == 0 ) {start =( i - (j*nbmesure)) +1 ;}
    if ( porteuse[i] ==0 && start != 0 && end == 0 ) {end = (i -2 -(j*nbmesure)) ;}  
  }
  half = 36 - ( end - start ); 
  if (half > start ) { half = start ; }

  for(int i = (start - half ) ; i < ( start + nbmesure - half )  ; i++)
  {
    if ( i <  ( start + ( nbmesure / 2 ) - half ) ) {
    temp_read = (tableau[i+(j*nbmesure)]-sigma_read) / config.facteur ; 
    courant += sq(temp_read);
    positive += temp_read  ; 
        if ( tableau[i+(j*nbmesure)] < 5 ) { zero ++ ;}    //détection des zeros ( prise débranchée )
    }
    else {
    temp_read = (sigma_read-tableau[i+(j*nbmesure)]) / config.facteur ;
    courant += sq(temp_read); 
    positive += temp_read ; 
        if ( tableau[i+(j*nbmesure)] < 5 ) { zero ++ ;}  //détection des zeros ( prise débranchée )
    }
    tension += (porteuse[i+(j*nbmesure)]/ config.voltage);
    Vrms += sq(porteuse[i+(j*nbmesure)]/ config.voltage); // carré des mesures
    nbV ++;
  }
  start=0;
  end=0;
}

positive = positive / nbV;
courant = courant / nbV;


Vrms = Vrms / nbV;
Vrms = sqrt(Vrms);
Vrms = VrmsOLD + PHASECAL * (Vrms - VrmsOLD); // Lissage des valeurs
VrmsOLD = Vrms;

Irms = sqrt(courant);

PVA = floor(Vrms * Irms);

PW =  (( positive * Vrms) + config.offset) ; 

PowerFactor = floor(100 * abs(PW) / PVA) / 100;
// logging.start +="Intensité-->" + String(positive) + "A. \r\n" ;
positive = PW ; 
if ( zero > 75 ) { 
  if (logging.sct) { logging.start += "--> SCT013 Prob not connected  ?\r\n" ; logging.sct = false; }
}
//  logging.start +="Vrms-->" + String(Vrms) +"V.  Irms-->" + String(Irms) + "A.  ApparentPower--> " + String(PVA) + "VA.  PF--> " + String(PowerFactor) + "  PW--> " + String(PW) + "\r\n" ;


bool nolog =false; 
if (nolog) {
Serial.println("fin tableau");
Serial.println("temp");
Serial.println(wait_time*loop);
Serial.println(stopMillis-startMillis);
Serial.println("inter");
Serial.println(inter*wait_time);
Serial.println("middle");
Serial.println(loop);
Serial.println(sigma_read);
Serial.println(start);
Serial.println(int(positive)) ;
}
middle_debug= sigma_read; 

gDisplayValues.watt = int( positive ) ; 
if ( config.polarity == true ) { gDisplayValues.watt = - gDisplayValues.watt ; }




}





void injection4(){ 

// int temp_read ; 
int zero = 0; 
double positive = 0; 


//int zero_count = 0; 
int loop = 0;  int inter=0;

int wait_time = 277 ; //  --> 1/50Hz -> /2 -> 10ms --> 18 mesures --> 555 us 
// unsigned long startMillis,stopMillis;
// int injection = 0; 
// nbmesure=100 ;  /// nombre de mesure par ondulation
// nombre_cycle = 4 ;  
// freqmesure = nbmesure*(nombre_cycle+1) ;  // nombre total de mesures


front();  ///synchro porteuse.
int Watchdog=0 ;  
startMillis = micros();   // 0ms 
//rt_loop( startMillis, (wait_time * 71) ) ; // attend le prochain top temps de mesure ( 277us * loop )

///// construction du tableau de mesures  /////
// while (millis() - startMillis < 21) {  
while (loop < freqmesure ) { 
    tableau[loop] =  analogRead(ADC_INPUT);
    porteuse[loop] =  analogRead(ADC_PORTEUSE);
    sigma_read += tableau[loop]; 
    
    loop ++; 
    rt_loop( startMillis, wait_time*(loop) ) ; // attend le prochain top temps de mesure ( 277us * loop )

    if (loop==1 && porteuse[1] == 0) {
      sigma_read -= tableau[0];
      tableau[0] = tableau[1];
      loop = 1;
      Watchdog++ ;  
      startMillis = micros();   // reset du timer 
    }   
    if ( Watchdog > nbmesure  ) {  Serial.print("Attention pas de porteuse, alimentation 12v AC ou pont redresseur débranché ? "); gDisplayValues.porteuse = false; break; } 
  }
  ///// fin  construction du tableau de mesures  /////

// 20ms * nombre de cycles -> total 72 mesures * nb cycles
stopMillis = micros();  

// début des calculs 

sigma_read = ( sigma_read / (loop ) ) ;  /// calcul du sigma ( la valeur moyenne ), ne marche bien que dans le cas d'une ondulation symétrique. A voir si nouvelle méthode de calcul. ( valeur théorique : 2047 -> vcc/2)
double tension = 0;
double courant = 0;
start=0;

int end=0;
Vrms = 0;
Irms = 0;
PW = 0;

for (int j = 0 ; j < nombre_cycle   ; j++) 
{
  // logging.start += "j : " + String(j) +   "\r\n" ;

  for(int i = (0 +(j*nbmesure)); i < (nbmesure+(j*nbmesure)); i++) {
    tableau[i] = (tableau[i] - sigma_read);
    if ( porteuse[i] !=0  && start == 0 ) {start =(i +1) ;}
    if ( porteuse[i] ==0 && start != 0 && end == 0 ) {end = (i -2) ;}  
    if (start != 0 && i<start) {porteuse[i]=0;}

    // if ( porteuse[i] == 0 && end == 0 ) {end = (i -1) ;}  
    if  (end != 0 && i > (end +4 ) && ((i-end-4)-1) >= 0) {  porteuse[i] = -1 * porteuse[((i-end-4)-1)]; }
    //   if (porteuse[(end+1-(i-end))] != 0 ) {porteuse[i] = -1 * porteuse[(end+1-(i-end))];}
    // }

    if (end != 0 && i>((end * 2) + 4)) {porteuse[i]=0;}

    if ( porteuse[i] !=  0 ) {

      tension = (porteuse[i] /config.voltage) ;
      Vrms += sq(tension);

      courant = (tableau[i]/config.facteur);
      Irms += sq(courant);

      PW += courant;
    }
  }

  start=0;
  end=0;
}

  Vrms = Vrms / nombre_cycle;  //square of voltage
  Vrms = sqrt(Vrms);  //RMS voltage
  Irms = Irms / nombre_cycle;  //square of current
  Irms = sqrt(Irms);  // RMS current
  PW = PW / nombre_cycle;
 
   Vrms = VrmsOLD + PHASECAL * (Vrms - VrmsOLD); // Lissage des valeurs
   VrmsOLD = Vrms;

  if ( config.polarity == true ) { PW = - PW ; }
  PVA = floor(Vrms * Irms);
  PowerFactor = floor(100 * fabs(PW) / PVA) / 100;
  PW = floor(PW );


positive = PW + config.offset ; 


if ( zero > 75 ) { 
  if (logging.sct) { logging.start += "--> SCT013 Prob not connected  ?\r\n" ; logging.sct = false; }
}
//logging.start += "zero detected : " + String(zero) +   "\r\n" ;
 logging.start +="Vrms-->" + String(Vrms) +"V.  Irms-->" + String(Irms) + "A.  ApparentPower--> " + String(PVA) + "VA.  PF--> " + String(PowerFactor) + "  PW--> " + String(PW) + "\r\n" ;


bool nolog =false; 
if (nolog) {
Serial.println("fin tableau");
Serial.println("temp");
Serial.println(wait_time*loop);
Serial.println(stopMillis-startMillis);
Serial.println("inter");
Serial.println(inter*wait_time);
Serial.println("middle");
Serial.println(loop);
Serial.println(sigma_read);
Serial.println(start);
Serial.println(int(positive)) ;
}
middle_debug= sigma_read; 



gDisplayValues.watt = int(( positive )) ; 


}








void injection5(){ 

// int temp_read ; 
// int temp_porteuse ;
//double voltage = 0; // test de calcul de voltage
//String porteuse = "" ;
int zero = 0; 
double positive = 0; 


//int zero_count = 0; 
int loop = 0;  int inter=0;

int wait_time = 277 ; //  --> 1/50Hz -> /2 -> 10ms --> 18 mesures --> 555 us 
// unsigned long startMillis,stopMillis;
// int injection = 0; 
// nbmesure=100 ;  /// nombre de mesure par ondulation
// nombre_cycle = 4 ;  
// freqmesure = nbmesure*(nombre_cycle+1) ;  // nombre total de mesures

 int Watchdog=0 ;  
  gDisplayValues.porteuse = true; 
  int margin = 0; /// 0 marge de bruit de fond  en lecture ( si pb ) 
  // recherche demi ondulation
  while ( analogRead(ADC_PORTEUSE) > margin  ) { 
  	delayMicroseconds (3);
    Watchdog++;
    if ( Watchdog > 2500  ) {  Serial.print("Attention pas de porteuse, alimentation 12v AC ou pont redresseur débranché ? "); gDisplayValues.porteuse = false; break; } 

	}

  Watchdog=0 ; 
  // recherche d'un 0
  
  while ( analogRead(ADC_PORTEUSE) == margin  ) {
  delayMicroseconds (3);  
  //startMillis = micros();   // 0ms 
    Watchdog++;
    if ( Watchdog > 2500  ) {  Serial.print("Attention pas de porteuse, alimentation 12v AC ou pont redresseur débranché ou inversée ? "); gDisplayValues.porteuse = false ; break;}  
	}


// front();  ///synchro porteuse.
//delay (24);
Watchdog=0 ;  
startMillis = micros();   // 0ms 
//rt_loop( startMillis, (wait_time * 71) ) ; // attend le prochain top temps de mesure ( 277us * loop )

///// construction du tableau de mesures  /////
loop = 0;
while (millis() - startMillis < 10001) {  
    tableau[loop] =  analogRead(ADC_INPUT);
    porteuse[loop] =  analogRead(ADC_PORTEUSE);
    sigma_read += tableau[loop]; 
    
    loop ++; 
    // if (loop==1 && porteuse[1] == 0) {
    //   sigma_read -= tableau[0];
    //   tableau[0] = tableau[1];
    //   loop = 1;
    //   Watchdog++ ;  
    //   startMillis = micros();   // reset du timer 
    // }   
    // if ( Watchdog > nbmesure  ) {  Serial.print("Attention pas de porteuse, alimentation 12v AC ou pont redresseur débranché ? "); gDisplayValues.porteuse = false; break; } 
  }
  ///// fin  construction du tableau de mesures  /////

// 20ms * nombre de cycles -> total 72 mesures * nb cycles
stopMillis = micros();  

// début des calculs 

sigma_read = ( sigma_read / (loop +1) ) ;  /// calcul du sigma ( la valeur moyenne ), ne marche bien que dans le cas d'une ondulation symétrique. A voir si nouvelle méthode de calcul. ( valeur théorique : 2047 -> vcc/2)
double tension = 0;
double courant = 0;
start=0;
half = ((loop + 1)/2); 

// int end=0;
Vrms = 0;
Irms = 0;
PW = 0;
  for(int i = 0; i < loop; i++) {
    if ( i < half ) {
    tableau[i] = (tableau[i]-sigma_read) ; 
        if ( tableau[i] < 5 ) { zero ++ ;}    //détection des zeros ( prise débranchée )
    }
    else {
    tableau[i] = (sigma_read-tableau[i]) ; 
        if ( tableau[i] < 5 ) { zero ++ ;}  //détection des zeros ( prise débranchée )
    }

    tension = (10/config.voltage) * porteuse[i];
    Vrms += sq(tension);

    courant = (10/config.facteur) * tableau[i];
    Irms += sq(courant);

    PW += tension * courant;

// for (int j = 0 ; j < nombre_cycle   ; j++) 
// {
//   // logging.start += "j : " + String(j) +   "\r\n" ;

  // for(int i = (0 +(j*nbmesure)); i < (nbmesure+(j*nbmesure)); i++) {
  //   tableau[i] = (tableau[i] - sigma_read);
  //   if ( porteuse[i] == 0 && end == 0 ) {end = (i -1) ;}  
  //   if  (end != 0 && i > (end +7 ) && (end+1-(i-end-7)) >= 0) {  porteuse[i] = -1 * porteuse[(end+1-(i-end-7))]; }
  //   //   if (porteuse[(end+1-(i-end))] != 0 ) {porteuse[i] = -1 * porteuse[(end+1-(i-end))];}
  //   // }

  //   if (end != 0 && i>((end * 2) + 7)) {porteuse[i]=0;}

  //   if ( porteuse[i] !=  0 ) {

  //     tension = (10/config.voltage) * porteuse[i];
  //     Vrms += sq(tension);

  //     courant = (10/config.facteur) * tableau[i];
  //     Irms += sq(courant);

  //     PW += tension * courant;
  //   }
  }

  // half = (nbmesure/2) - ( end - start ); 
  // if (half > start ) { half = start ; }

  // for(int i = (start - half ) ; i < ( start + nbmesure - half )  ; i++)
  // {
  //   if ( i <  ( start + ( nbmesure / 2 ) - half ) ) {
  //   temp_read = (tableau[i+(j*nbmesure)]-sigma_read) ; 
  //   positive += temp_read ; 
  //       if ( tableau[i+(j*nbmesure)] < 5 ) { zero ++ ;}    //détection des zeros ( prise débranchée )
  //   }
  //   else {
  //   temp_read = (sigma_read-tableau[i+(j*nbmesure)]) ; 
  //   positive += temp_read ; 
  //       if ( tableau[i+(j*nbmesure)] < 5 ) { zero ++ ;}  //détection des zeros ( prise débranchée )
  //   }
  //   Irms += sq(temp_read) ;  // carré des mesures

  // }
  // for(int i = start ; i < end  ; i++)
  // {
  //   tension += porteuse[i+(j*nbmesure)];
  //   Vrms += sq(porteuse[i+(j*nbmesure)]); // carré des mesures
  //   nbV++;
  // }
  // start=0;
  // end=0;
// }

  Vrms = Vrms/(loop +1);  //square of voltage
  Vrms = sqrt(Vrms);  //RMS voltage
  Irms = Irms/(loop +1) ;  //square of current
  Irms = sqrt(Irms);  // RMS current
  PW = PW/(loop +1);
//  PVA = floor(Vrms * Irms);
 // PowerFactor = floor(100 * PW / PVA) / 100;
//  positive = (10/config.facteur) * positive;
//  positive = positive / freqmesure;

// //
// tension = tension * (freqmesure/ nbV) ;
// tension = (10/config.voltage) * tension;

// tension = tension / freqmesure; 
// tension = tensionOLD + PHASECAL * ( tension - tensionOLD);// Lissage des valeurs
// tensionOLD = tension;
// Vrms = Vrms *2;
// // nbV = nbmesure * nombre_cycle;

//   Vrms = Vrms / freqmesure;
//   Vrms = (10/config.voltage) * sqrt(Vrms);
 
   Vrms = VrmsOLD + PHASECAL * (Vrms - VrmsOLD); // Lissage des valeurs
   VrmsOLD = Vrms;
//   Irms = Irms / freqmesure;
//   Irms = (10/config.facteur) * sqrt(Irms);

  //PW = tension * positive ;
 //logging.start += String(tension) + "*" +String(positive) + "\r\n" ;
  if ( config.polarity == true ) { PW = - PW ; }
  PVA = floor(Vrms * Irms);
  PowerFactor = floor(100 * fabs(PW) / PVA) / 100;
  PW = floor(PW );


//Serial.println(int(positive/10000*voltage_read)) ;// test de calcul de voltage

// positive_debug = PVA ; 
positive = PW + config.offset ; 


if ( zero > 75 ) { 
  if (logging.sct) { logging.start += "--> SCT013 Prob not connected  ?\r\n" ; logging.sct = false; }
}
//logging.start += "zero detected : " + String(zero) +   "\r\n" ;
 logging.start +="Vrms-->" + String(Vrms) +"V.  Irms-->" + String(Irms) + "A.  ApparentPower--> " + String(PVA) + "VA.  PF--> " + String(PowerFactor) + "  PW--> " + String(PW) + "\r\n" ;


bool nolog =false; 
if (nolog) {
Serial.println("fin tableau");
Serial.println("temp");
Serial.println(wait_time*loop);
Serial.println(stopMillis-startMillis);
Serial.println("inter");
Serial.println(inter*wait_time);
Serial.println("middle");
Serial.println(loop);
Serial.println(sigma_read);
Serial.println(start);
Serial.println(int(positive)) ;
}
middle_debug= sigma_read; 

//zero = sqrt(zero / float(zero_count)); 
// positive = sqrt(positive / float( loop - zero_count )) ;

gDisplayValues.watt = int(( positive )) ; 




}

#endif