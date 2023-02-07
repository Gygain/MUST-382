int currVals[] = {0, 0, 0}; //values - r, g, b 
int nextVals[] = {0, 0, 0}; //values - r, g, b
int pins[] = {6, 5, 3}; //pins - r, g, b
int delayTime = 10;

// Configure R, G, B pins to output
void setup() {
  pinMode(pins[0], OUTPUT);
  pinMode(pins[1], OUTPUT);
  pinMode(pins[2], OUTPUT);
}


// Iterates through an input string, using its char values to set RGB values on an LED
void loop() {

  // Input string - adding numbers and special chars widens the range of values for RGB
  String str = "Th3 Quick Br0wn Fox Jump5! 0ver t#e 1azy dog :("; 

  // Beginning at the start, iterate through the input string, getting three contiguous 
  // chars from the input string and setting them as the RGB values of the LED
  for(int i = 0; i < str.length()-3; i++) {
    nextVals[0] = str.charAt(i); 
    nextVals[1] = str.charAt(i+1);
    nextVals[2] = str.charAt(i+2);

    blendColors();
  }
  
}

// Blends between current and next color
void blendColors() {

  // Print next text section to console
  Serial.begin(9600);
  for(int i = 0; i < 3; i++) {
    Serial.write(nextVals[i]);
  }
  Serial.println();

  // For each pin, increment or decrement the current value until it reaches the next 
  // value divided by 2 (Dividing by 2 makes it easier to distinguish colors on LED)
  for(int i = 0; i < 3; i++) {
    while(currVals[i] != nextVals[i] / 2) { 
      if(currVals[i] < nextVals[i] / 2) {
        currVals[i]++;
      } else {
        currVals[i]--;
      }

      analogWrite(pins[i], currVals[i]);
      delay(delayTime);
    }
  }

}


