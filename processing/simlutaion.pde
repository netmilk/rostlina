// Learning Processing
// Daniel Shiffman
// http://www.learningprocessing.com

// Example 19-8: Reading from serial port

import processing.serial.*;

int last_X = 0;
int last_Y = 0;

int X = 0;
int Y = 0;

int iteration = 0;
int current_iteration = 0;

Serial port; // The serial port object

void setup() {
  size(500, 500);
  smooth();

  println(Serial.list());
  port = new Serial(this, Serial.list()[0], 9600); 
  port.bufferUntil('\n');
  
  //PFont font;
  //font = createFont("Helvetica", 32);//loadFont("LetterGothicStd-32.vlw");

}

void draw() {
  //print iteration number if changes
  if(! (iteration == current_iteration)){
    current_iteration = iteration;
    //color c = color(random(255),random(255),random(255));
    color c = color(0,0,0);
    stroke(c);
    fill(c);
    text(iteration, X/10,Y/10);
  }
  
  // print point on coordinates
  point(X/10, Y/10);
}

// Called whenever there is something available to read
void serialEvent(Serial port) {

  String inString = port.readStringUntil('\n');
  // For debugging
  String[] x_y = split(trim(inString), ":");
  String X_string = x_y[0];
  String Y_string = x_y[1];
  String iteration_string = x_y[2];

  X = int(X_string);
  Y = int(Y_string);

  iteration = int(iteration_string);
  
  
  println("Raw:" + inString);
  println("X:" + X);
  println("Y:" + Y);
  println("I:" + iteration);
  
}

