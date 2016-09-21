#include <avr/interrupt.h>
volatile int timer2_overflow_counters = 0;     // Contador de overflows.
int ports_leds [] = {2,3,4,5,6,7,8,9};   // Array con puertos usados.
int ports_gnd [] = {A3,A2,A1,A0};        // Array con pueras usados como tierras de los display.
volatile int times [] = {0,0,0,0};       // Array donde se almacenará la cuenta del tiempo.
int my_display [10] [7] = {         // Matriz con diferentes combianciones para formar los números en el display.
    {1,1,1,1,1,1,0},  // 0
    {0,1,1,0,0,0,0},  // 1
    {1,1,0,1,1,0,1},  // 2
    {1,1,1,1,0,0,1},  // 3
    {0,1,1,0,0,1,1},  // 4
    {1,0,1,1,0,1,1},  // 5
    {1,0,1,1,1,1,1},  // 6
    {1,1,1,0,0,0,0},  // 7
    {1,1,1,1,1,1,1},  // 8
    {1,1,1,0,0,1,1},  // 9
  };
  
int dis;                          // Controlador de que display toca refrescar.
int level = 1;                    // Controlador de oscilacion pin 13.

////////////////////////////////
//// Rutina de Interrupción ////
////////////////////////////////

ISR(TIMER2_OVF_vect){
  if(timer2_overflow_counters < 78){
    timer2_overflow_counters ++;
    if (timer2_overflow_counters == 77)   TCNT2 = 230; // Ponemos en el registro TCNT2 el valor de la cuenta del útimo overflow.
  } else {
    TCNT2 = 1;
    timer2_overflow_counters = 0;
    change_port_osci();
    clock_update();  // Actualizamos reloj.     
    refresher();     // Refrescamos los display.
  } 
}
  
void setup(){
  
  pinMode(13,OUTPUT);      // Inicializamos pin 13 para medir con el osciloscopio.
  digitalWrite(13,LOW);
  
  dis= 0;
  for (int i = 0; i < 8; i++){
    pinMode(ports_leds[i], OUTPUT);      // Inicializar los puertos de salida a nivel bajo.
    digitalWrite(ports_leds[i],LOW);
  }  
  
   for (int i = 0; i < 4; i++){
     pinMode(ports_gnd[i], OUTPUT);      // Inicializar los puertos de salida a nivel bajo.
     digitalWrite(ports_gnd[i],HIGH);
  } 
  int X_secomd, Y_second, X_milisecond, Y_milisecond = 0;
 // Inicialización de registros del timer.
  TCNT2 = 1;  // Comienzo de la cuenta.
  TCCR2A |= (1<<(WGM21)) | (1<<(WGM20));      // Modo de funcionamiento.
  TCCR2B &= ~(1<<(CS22)) & (~(1<<(CS20)));    // Selección de preescaler.
  TCCR2B |=  (1<<(CS21)) ;
  TIMSK2 |= (1<<(TOIE2));      // Mascara de interrupciones.
  interrupts();      // Ponemos en marcha las interrupciones.
}


///////////////////////////////
////    Bloque de LOOP     ////
///////////////////////////////

void loop(){   
}

///////////////////////////////////////////////////////////
////    Rutina que actualiza la cuenta hacia atras     ////
///////////////////////////////////////////////////////////
  
  void clock_update(){
    if(times[3] < 9){
    times[3]++;
  }else{
    times[3]=0;
    if(times[2]<9){
      times[2]++;
    }else{
       times[2]=0;
       if(times[1]<9){
         times[1]++;
       }else{
          times[1]=0;
          if(times[0]<9){
            times[0]++;
          }else{
           times[0]=0;
          times[1]=0;
         times[2]=0;
         times[3]=0;
          }
       }}}   
  }
  
  ///////////////////////////////////////////
  ////    Rutina que controla pin 13     ////
  ///////////////////////////////////////////

  void change_port_osci(){  // Función encargadad de cambiar el valor de la señal del pin 13
    if ( level == 1 ){
      digitalWrite(13,HIGH);
      level = 0;
    } else {
      digitalWrite(13,LOW);  
      level = 1;
    }
  }

///////////////////////////////////////////////
////    Rutina que refresca los display    ////
///////////////////////////////////////////////

void refresher(){
  switch(dis){
    case 0:
      show(times[3]);
      digitalWrite(ports_gnd[3], LOW);
      digitalWrite(9,LOW);
      digitalWrite(ports_gnd[0],HIGH);
      digitalWrite(ports_gnd[1],HIGH);     
      digitalWrite(ports_gnd[2],HIGH);
      dis = 1;
      break;
    case 1:
      show(times[2]);
      digitalWrite(ports_gnd[2], LOW);
      digitalWrite(9,LOW);
      digitalWrite(ports_gnd[0],HIGH);
      digitalWrite(ports_gnd[1],HIGH);     
      digitalWrite(ports_gnd[3],HIGH); 
      dis = 2;
      break;
    case 2:
      show(times[1]);
      digitalWrite(ports_gnd[1], LOW);
      digitalWrite(9,HIGH);
      digitalWrite(ports_gnd[0],HIGH);
      digitalWrite(ports_gnd[3],HIGH);     
      digitalWrite(ports_gnd[2],HIGH); 
      dis = 3;
      break;
    case 3:
      show(times[0]);
      digitalWrite(ports_gnd[0], LOW);
      digitalWrite(9,LOW);
      digitalWrite(ports_gnd[30],HIGH);
      digitalWrite(ports_gnd[1],HIGH);     
      digitalWrite(ports_gnd[2],HIGH); 
      dis = 0;
      break; 
    }
}
  
  ///////////////////////////////////////////////////////
  ////    Rutina de para mostrar valor de display    ////
  ///////////////////////////////////////////////////////

  void show (int Val){
    for(int j = 0; j<7; j++ ){
      digitalWrite(ports_leds[j],my_display[Val][j]);
    } 
  }

