//PPM8Outs
//this programm will put out 8 ch PPM signals
//by Francisco Carabaza ene 2016.
// PPM out Digital pin 10
// 8 input A0-A8

//////////////////////CONFIGURATION///////////////////////////////
#define chanel_number 8  //set the number of chanels
#define default_servo_value 1500  //set the default servo value
#define PPM_FrLen 22500  //set the PPM frame length in microseconds (1ms = 1000µs) 8 canales 22500.
#define PPM_PulseLen 300  //set the pulse length
#define onState 1  //set polarity of the pulses: 1 is positive, 0 is negative
#define sigPin 10  //set PPM signal output pin on the arduino
//////////////////////////////////////////////////////////////////


/*this array holds the servo values for the ppm signal
 change theese values in your code (usually servo values move between 1000 and 2000)*/
int ppm[chanel_number];

void setup(){  
  
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
  
  //initiallize default ppm value
  for(int i=0; i<chanel_number; i++){
    ppm[i]= default_servo_value;
  }

  pinMode(sigPin, OUTPUT);
  digitalWrite(sigPin, !onState);  //set the PPM signal pin to the default state (off)
  
  cli();
  TCCR1A = 0; // set entire TCCR1 register to 0
  TCCR1B = 0;
  
  OCR1A = 100;  // compare match register, change this
  TCCR1B |= (1 << WGM12);  // turn on CTC mode
  TCCR1B |= (1 << CS11);  // 8 prescaler: 0,5 microseconds at 16mhz
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
  sei();
}

// the loop routine runs over and over again forever:
void loop(){
  
  // read the inputa on analog pins
  int Analog0 = analogRead(A0);
  int Analog1 = analogRead(A1);
  int Analog2 = analogRead(A2);
  int Analog3 = analogRead(A3);
  int Analog4 = analogRead(A4);
  int Analog5 = analogRead(A5);
  int Analog6 = analogRead(A6);
  int Analog7 = analogRead(A7);

//  FlexSensorValue = FlexSensorValue * 5 - 1700 ;
//  Axis1Value = Axis1Value * 3 -182 ;
//  Axis2Value = Axis2Value * 3 -182 ;
  
  Analog0 = map(Analog0, 0, 1023, 980, 2020);
  Analog1 = map(Analog1, 0, 1023, 980, 2020);
  Analog2 = map(Analog2, 0, 1023, 980, 2020);
  Analog3 = map(Analog3, 0, 1023, 980, 2020);
  Analog4 = map(Analog4, 0, 1023, 980, 2020);
  Analog5 = map(Analog5, 0, 1023, 980, 2020);
  Analog6 = map(Analog6, 0, 1023, 980, 2020);
  Analog7 = map(Analog7, 0, 1023, 980, 2020);

  // print out mapped values:
  Serial.print(Analog0);
  Serial.print(", ");
  Serial.print(Analog1);
  Serial.print(", ");
  Serial.print(Analog2);
  Serial.print(", ");
  Serial.print(Analog3);
  Serial.print(", ");
  Serial.print(Analog4);
  Serial.print(", ");
  Serial.print(Analog5);
  Serial.print(", ");
  Serial.print(Analog6);
  Serial.print(", ");
  Serial.print(Analog7);
  Serial.println(", ");

  
  ppm[0] = Analog0;
  ppm[1] = Analog1;
  ppm[2] = Analog2;
  ppm[3] = Analog3;
  ppm[4] = Analog4;
  ppm[5] = Analog5;
  ppm[6] = Analog6;
  ppm[7] = Analog7;
  delay(10);     // delay in between reads for stability
    
}

ISR(TIMER1_COMPA_vect){  //leave this alone
  static boolean state = true;
  
  TCNT1 = 0;
  
  if(state) {  //start pulse
    digitalWrite(sigPin, onState);
    OCR1A = PPM_PulseLen * 2;
    state = false;
  }
  else{  //end pulse and calculate when to start the next pulse
    static byte cur_chan_numb;
    static unsigned int calc_rest;
  
    digitalWrite(sigPin, !onState);
    state = true;

    if(cur_chan_numb >= chanel_number){
      cur_chan_numb = 0;
      calc_rest = calc_rest + PPM_PulseLen;// 
      OCR1A = (PPM_FrLen - calc_rest) * 2;
      calc_rest = 0;
    }
    else{
      OCR1A = (ppm[cur_chan_numb] - PPM_PulseLen) * 2;
      calc_rest = calc_rest + ppm[cur_chan_numb];
      cur_chan_numb++;
    }     
  }
}
