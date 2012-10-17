/*

merim tisknu na serial port
*/


// Define the number of samples to keep track of.  The higher the number,
// the more the readings will be smoothed, but the slower the output will
// respond to the input.  Using a constant rather than a normal variable lets
// use this value to determine the size of the readings array.

// when true, do not move motors, only mock movements
boolean simulate_motors = false;

// set motor speed - delay between motor steps 
// lower number increase speed
int motor_step_delay = 1;


// when true it will send send absolute coordinates of motots to serial line
// M2:2270
// M1:1419
// M1:1420
// M1:1421
// ..
boolean debug_motor_steps = true;


// when true it will mock EEG input data with randomly generated values
boolean simulate_eeg = false;

// set all readings to 500
boolean simulate_flat_eeg = true;
int flat_eeg_value = 500;

// send target position before each motor stepping iteration eg:
// 3464:9077:1:2:1 (X:Y:iteration:column:row)
boolean debug_print = true;

// print iteration counter will be sent with coordinates
int print_iteration = 0;
int print_row_position = 1;
int print_column_position = 1;

// offset between graps (columns) in row
int column_offset = 250;

// map for row each row column counts
// ommit first item (we have no zero count of rows)
int row_column_counts[] = {0,2,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};

// max rows to print (can't be higher than row_coumn_counts array length)
int max_rows = 4;

// set graph length
// 6 is minimum, because we draw line at beggining and at the end of graph 
// optimum is 500
const int numReadings = 500; 

// number of readings to compute avarage
// recomended for real operation is 10
// set it to 1 for faster simulation
const int numOkno = 1;



//*********************************motor

int Step = 10; /// motor 2 P pravy
int Dir = 9;

int StepL = 7; /// motor 1 L levy
int DirL = 6;



int absM1 = 2000;
int absM2 = 2000;



int cteni[numOkno];
int readings[numReadings];      // the readings from the analog input // zmena
int index = 0;                  // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average

int inputPin = A0;

void setup()
{
  // initialize serial communication with computer:
  Serial.begin(9600);                   
  
  // initialize all the readings to 0: 
  for (int thisReading = 0; thisReading < numReadings; thisReading++){
    readings[thisReading] = 0;   
  }
  
  for (int thisOkno = 0; thisOkno < numOkno; thisOkno++){
    cteni[thisOkno] = 0;  
  }
  
  // deklarace motoru
  pinMode(StepL, OUTPUT);     
  pinMode(DirL, OUTPUT); 
  
  digitalWrite(DirL, HIGH); //// odviji...backward
  
  pinMode(Step, OUTPUT);     
  pinMode(Dir, OUTPUT); 
  
  digitalWrite(Dir, HIGH); //// naviji... forward

//********************************

    
}





void loop() {
  
  // EEG READING
  if(!simulate_eeg == true){
    delay(500);
  }
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {    
    
    for (int thisOkno = 0; thisOkno< numOkno; thisOkno++) {    
      // subtract the last reading:
      total = total - cteni[index];         
      // read from the sensor:
      if(simulate_eeg == true) {
        cteni[index] = random(0, 1023);   
      } else {
        cteni[index] = analogRead(inputPin);  
      }
  
      // add the reading to the total:
      total = total + cteni[index];       
  
      // advance to the next position in the array:  
      index = index + 1;                    

      // if we're at the end of the array...
      if (index >= numOkno){              
        // ...wrap around to the beginning: 
        index = 0;                           
      }
      // calculate the average:
      average = total / numOkno;         
      if(!simulate_eeg == true){
        delay(1);        // delay in between reads for stability
      }
    }
    readings[thisReading] = average;
  }

  if(simulate_flat_eeg == true){
    // initialize all the readings to 500: 
    for (int thisReading = 0; thisReading < numReadings; thisReading++){
      readings[thisReading] = flat_eeg_value;   
    }
  }



  //force to draw line at beginning of each graph
  readings[0] = 0;
  readings[1] = 500;
  readings[2] = 0;

  //force to draw line at end of each graph  
  readings[numReadings -2] = 0;
  readings[numReadings - 1] = 500;
  readings[numReadings] = 0;

  if(!simulate_eeg == true){ 
    delay(500);
  }

  // PRINT
  
  // iterate print count
  print_iteration++;
  int last_reading = 0;
  for (int thisReading = 1; thisReading <= numReadings; thisReading++) {    
    last_reading = thisReading;
    
    // relative X coordinates to achieve
    int X = (readings[thisReading - 1] - readings[thisReading]);
    
    // Y coordinates will increment by 1
    krokM2(0); // krok motoru2 dopredu
         
    X = abs(X);
    if ((readings[thisReading - 1] - readings[thisReading]) > 0) { 
      for(int a = 0; a < X; a++) {
        krokM1(0); // krok motoru1 vpred
      }
    } else {   
      for(int a = 0; a < X; a++) {
        krokM1(1); // krok motoru1 zpet
      }
    }

    if(debug_print == true){
      //int targetAbsM1 = absM1 + X;
      Serial.println(String(absM1)+":"+String(absM2)+":"+print_iteration+":"+print_column_position+":"+print_row_position);
    }
    
  }  // end for cyklu tisku

  
  //for(int a = 0; a < end_offset ; a++) {
  //  krokM1(0);   // motor1 zpatky
  //}

  for(int a = 0; a < numReadings ; a++) {
    krokM2(1);   // motor2 zpatky
  }

  
  int current_row_column_count = row_column_counts[print_row_position]; 
  if (print_column_position == current_row_column_count) {  
    // if we are on end of line, jump to next row (do enter :)
  
    //return to first column (rewind with width of all advanced columns)
    int move_steps = (current_row_column_count * column_offset) - column_offset;
    for(int a = 0; a < move_steps ; a++) {
      krokM1(1);   // motor1 zpatky
    }
    
    print_column_position = 1;

    if(max_rows == print_row_position) {
      for(int a = 0; a < (numReadings * (print_row_position - 1)) ; a++) {  
        krokM2(1);   // motor2 zpatky
      }
      print_row_position = 1;
    } else {
      //advance to new row
      for(int a = 0; a < numReadings ; a++) {  
        krokM2(0);   // motor2 zpatky
      }
      print_row_position++;
    }
  } else {
    // if we will print new column in row, return to new column start position 
    int move_steps = column_offset ;
    // (column_offset + print_column_position * column_offset)
    for(int a = 0; a < move_steps  ; a++) {
      krokM1(0);   // motor1 zpatky
    }
    print_column_position++;
  }
} // end voidu loop



void krokM1(boolean dir)   
 { 
   if(! simulate_motors == true) {
     digitalWrite(Dir, dir);  
  
     digitalWrite(Step, HIGH);  // krok
     delay(motor_step_delay); 
     digitalWrite(Step, LOW);   
     delay(motor_step_delay);                 // krok
   }

   //increment absolute position
   if(dir){
     absM1--;
   } else { 
     absM1++;
   }
   if(debug_motor_steps == true){
     Serial.println("M1:"+String(absM1));
   }
 }
 
void krokM2(boolean dir) 
 {
   if(! simulate_motors == true) {
     digitalWrite(DirL, dir); 
     digitalWrite(StepL, HIGH);   //krok
     delay(motor_step_delay);               
     digitalWrite(StepL, LOW);    
     delay(motor_step_delay);                   //krok
   }
   
   // increment absolute position 
   if(dir){
      absM2--;
    } else {
      absM2++;
    }
   if( debug_motor_steps == true) {
     Serial.println("M2:"+String(absM2));
   }
 }

