#include <Gamer.h>
 
#include <SoftwareSerial.h>
 
Gamer gamer;

 
int currentX = 5;
int currentY = 4;
int velocity[2] = {
  -1,-1};
int blocks[8][8];
int paddleX = 2;
int counter = 0;
int origXV=-1;
int origYV=-1;
int score = 0;
 
//for showScore()
byte numbers[10][2][8];
byte n1[8],n1b[8],n2[8],n2b[8],n3[8],n3b[8],n4[8],n4b[8],n5[8],n5b[8],n6[8],n6b[8],n7[8],n7b[8],n8[8],n8b[8],n9[8],n9b[8],n0[8],n0b[8];
 
byte frames[2][8]; //declare at top of code
void setupFun() { //run this at the start
  frames[0][0] = B00000000;
  frames[0][1] = B01100110;
  frames[0][2] = B01100110;
  frames[0][3] = B00000000;
  frames[0][4] = B01000010;
  frames[0][5] = B00111100;
  frames[0][6] = B00000000;
  frames[0][7] = B00000000;
  frames[1][0] = B00000000;
  frames[1][1] = B01100110;
  frames[1][2] = B01100110;
  frames[1][3] = B00000000;
  frames[1][4] = B00000000;
  frames[1][5] = B00111100;
  frames[1][6] = B01000010;
  frames[1][7] = B00000000;
}
 
void setup() {
  //let's seed our PRNG with micros,
  //far from perfect but at least subsequent games should 
  //probably be different.
  randomSeed(micros());
  gamer.begin();
  setupNums();
  startGame(true);
}
 
void startGame(boolean resetIt) {
  for(int x=0;x<8;x++) {
    for(int y=0;y<4;y++) {
      blocks[x][y] = 1;
    }
  }
  if(!resetIt) {
    score++;
    gamer.clear();
    gamer.printImage(frames[0]);
    delay(500);
  } 
  else score=0;
  //randomize the start position and direction (in the x sense anyhow)
  currentX=random(1,6);
  velocity[0]=random(1,2)?-1:1;
  currentY=6;
  velocity[1]=-1;
}
 
void loop() {


  if(gamer.isHeld(LEFT)&&paddleX>0){
    paddleX--;
  } 
  else if(gamer.isHeld(RIGHT)&&paddleX<4) {
    paddleX++;
  }

  physics();
  currentX = currentX+velocity[0];
  currentY = currentY+velocity[1];
  
  redrawScreen();
  
  gamer.updateDisplay();
  if(currentY==7) { //if out of play, lose
    for(int b=0;b<4;b++) {
      gamer.clear();
      delay(150);
      gamer.display[currentX][currentY]=HIGH;
      gamer.updateDisplay();
      delay(150);
    }
    if(score==0){
      gamer.clear();
      gamer.printImage(frames[1]);
    } 
    else if(score<10){
      gamer.clear();
      showScore(0,score);
    } 
    else {
      int dig2 = score % 10;  //split score into two digits (eg 10 -> 1 and 0)
      int dig1 = (score-(score%10))/10;
      gamer.clear();
      showScore(dig1,dig2);
    }
    delay(500);
    startGame(true);
  }
  boolean finished = true;
  for(int x=0;x<8;x++) {
    for(int y=0;y<4;y++) {
      if(blocks[x][y]==HIGH) finished=false;
    }
  }
  if(finished) {
    startGame(false);
  }
  delay(150);
}

void redrawScreen() {
  //blank the screen
  for(int x=0;x<8;x++) {
    for(int y=0;y<8;y++) {
        gamer.display[x][y] = LOW;
    }
  }
    //draw paddle
  for(int a=0;a<4;a++) {
    if(paddleX+a<8) {
      gamer.display[paddleX+a][7]=HIGH;
    }
  }
  //draw the wall
  for(int x=0;x<8;x++) {
    for(int y=0;y<4;y++) {
      blocks[x][y] == 0 ? gamer.display[x][y] = LOW : gamer.display[x][y] = HIGH;
    }
  }  
  //draw the ball
  gamer.display[currentX][currentY] = HIGH;
  gamer.updateDisplay();
}

void physics() {

  boolean collision = true;

  //we basically have to iterate until we don't have a potential collision,
  //changing the 'velocity' on collision means we have to also check that the 
  //new velocity won't result in a collision, so we loop until there's no 
  //potential collision 
  
  while (collision) {
    collision = false;
    int adjustedX = currentX+velocity[0];
    int adjustedY = currentY+velocity[1];
    if(adjustedX > 7 || adjustedX < 0) {
      velocity[0] = -velocity[0];
      collision = true;
    }
    if(adjustedY> 7 || adjustedY < 0) {
      velocity[1] = -velocity[1];
      collision = true;
    }
  
    adjustedX = currentX+velocity[0];
    adjustedY = currentY+velocity[1];
   
    //check for block collisions, we want to make sure we're not going to check out of bounds array indices
    if(adjustedX >= 0 && adjustedX < 8 && gamer.display[adjustedX][currentY] == HIGH) {
       if(currentY < 5) blocks[adjustedX][currentY] = LOW;
        velocity[0] = -velocity[0];
        collision = true;
      } else if(adjustedY >= 0 && adjustedY < 8 && gamer.display[currentX][adjustedY] == HIGH) {
        if(adjustedY < 5) blocks[currentX][adjustedY] = LOW;
        velocity[1] = - velocity[1];
        collision = true;
      } else if(adjustedX >= 0 && adjustedX < 8 && adjustedY >=0 && adjustedY < 8 && gamer.display[adjustedX][adjustedY] == HIGH) {
        if(adjustedY < 5) blocks[adjustedX][adjustedY] = LOW;
        velocity[1] = -velocity[1];
        velocity[0] = -velocity[0];
        collision = true;
      }
  }
}

void showScore(int dig1,int dig2) {
  byte result[8];
  for(int p=0;p<8;p++) {
    result[p]=numbers[dig1][0][p]^numbers[dig2][1][p];
  }
  gamer.printImage(result);
}
 
void setupNums() {
  n1[0] = B10000000;
  n1[1] = B10000000;
  n1[2] = B10000000;
  n1[3] = B10000000;
  n1[4] = B10000000;
  n1[5] = B10000000;
  n1[6] = B10000000;
  n1[7] = B10000000;
 
  n1b[0] = B00000100;
  n1b[1] = B00000100;
  n1b[2] = B00000100;
  n1b[3] = B00000100;
  n1b[4] = B00000100;
  n1b[5] = B00000100;
  n1b[6] = B00000100;
  n1b[7] = B00000100;
 
  n2[0] = B11100000;
  n2[1] = B00100000;
  n2[2] = B00100000;
  n2[3] = B11100000;
  n2[4] = B10000000;
  n2[5] = B10000000;
  n2[6] = B10000000;
  n2[7] = B11100000;
 
  n2b[0] = B00000111;
  n2b[1] = B00000001;
  n2b[2] = B00000001;
  n2b[3] = B00000111;
  n2b[4] = B00000100;
  n2b[5] = B00000100;
  n2b[6] = B00000100;
  n2b[7] = B00000111;
 
  n3[0] = B11100000;
  n3[1] = B00100000;
  n3[2] = B00100000;
  n3[3] = B01100000;
  n3[4] = B00100000;
  n3[5] = B00100000;
  n3[6] = B00100000;
  n3[7] = B11100000;
 
  n3b[0] = B00000111;
  n3b[1] = B00000001;
  n3b[2] = B00000001;
  n3b[3] = B00000011;
  n3b[4] = B00000001;
  n3b[5] = B00000001;
  n3b[6] = B00000001;
  n3b[7] = B00000111;
 
  n4[0] = B10100000;
  n4[1] = B10100000;
  n4[2] = B10100000;
  n4[3] = B11100000;
  n4[4] = B00100000;
  n4[5] = B00100000;
  n4[6] = B00100000;
  n4[7] = B00100000;
 
  n4b[0] = B00000101;
  n4b[1] = B00000101;
  n4b[2] = B00000101;
  n4b[3] = B00000111;
  n4b[4] = B00000001;
  n4b[5] = B00000001;
  n4b[6] = B00000001;
  n4b[7] = B00000001;
 
  n5[0] = B11100000;
  n5[1] = B10000000;
  n5[2] = B10000000;
  n5[3] = B11100000;
  n5[4] = B00100000;
  n5[5] = B00100000;
  n5[6] = B00100000;
  n5[7] = B11100000;
 
  n5b[0] = B00000111;
  n5b[1] = B00000100;
  n5b[2] = B00000100;
  n5b[3] = B00000111;
  n5b[4] = B00000001;
  n5b[5] = B00000001;
  n5b[6] = B00000001;
  n5b[7] = B00000111;
 
  n6[0] = B11100000;
  n6[1] = B10000000;
  n6[2] = B10000000;
  n6[3] = B11100000;
  n6[4] = B10100000;
  n6[5] = B10100000;
  n6[6] = B10100000;
  n6[7] = B11100000;
 
  n6b[0] = B00000111;
  n6b[1] = B00000100;
  n6b[2] = B00000100;
  n6b[3] = B00000111;
  n6b[4] = B00000101;
  n6b[5] = B00000101;
  n6b[6] = B00000101;
  n6b[7] = B00000111;
 
  n7[0] = B11100000;
  n7[1] = B00100000;
  n7[2] = B00100000;
  n7[3] = B00100000;
  n7[4] = B00100000;
  n7[5] = B00100000;
  n7[6] = B00100000;
  n7[7] = B00100000;
 
  n7b[0] = B00000111;
  n7b[1] = B00000001;
  n7b[2] = B00000001;
  n7b[3] = B00000001;
  n7b[4] = B00000001;
  n7b[5] = B00000001;
  n7b[6] = B00000001;
  n7b[7] = B00000001;
 
  n8[0] = B11100000;
  n8[1] = B10100000;
  n8[2] = B10100000;
  n8[3] = B11100000;
  n8[4] = B10100000;
  n8[5] = B10100000;
  n8[6] = B10100000;
  n8[7] = B11100000;
 
  n8b[0] = B00000111;
  n8b[1] = B00000101;
  n8b[2] = B00000101;
  n8b[3] = B00000111;
  n8b[4] = B00000101;
  n8b[5] = B00000101;
  n8b[6] = B00000101;
  n8b[7] = B00000111;
 
  n9[0] = B11100000;
  n9[1] = B10100000;
  n9[2] = B10100000;
  n9[3] = B11100000;
  n9[4] = B00100000;
  n9[5] = B00100000;
  n9[6] = B00100000;
  n9[7] = B11100000;
 
  n9b[0] = B00000111;
  n9b[1] = B00000101;
  n9b[2] = B00000101;
  n9b[3] = B00000111;
  n9b[4] = B00000001;
  n9b[5] = B00000001;
  n9b[6] = B00000001;
  n9b[7] = B00000111;
 
  n0[0] = B11100000;
  n0[1] = B10100000;
  n0[2] = B10100000;
  n0[3] = B10100000;
  n0[4] = B10100000;
  n0[5] = B10100000;
  n0[6] = B10100000;
  n0[7] = B11100000;
 
  n0b[0] = B00000111;
  n0b[1] = B00000101;
  n0b[2] = B00000101;
  n0b[3] = B00000101;
  n0b[4] = B00000101;
  n0b[5] = B00000101;
  n0b[6] = B00000101;
  n0b[7] = B00000111;
 
  for(int x=0;x<8;x++) {
    numbers[0][0][x] = n0[x];
    numbers[1][0][x] = n1[x];
    numbers[2][0][x] = n2[x];
    numbers[3][0][x] = n3[x];
    numbers[4][0][x] = n4[x];
    numbers[5][0][x] = n5[x];
    numbers[6][0][x] = n6[x];
    numbers[7][0][x] = n7[x];
    numbers[8][0][x] = n8[x];
    numbers[9][0][x] = n9[x];
    numbers[0][1][x] = n0b[x];
    numbers[1][1][x] = n1b[x];
    numbers[2][1][x] = n2b[x];
    numbers[3][1][x] = n3b[x];
    numbers[4][1][x] = n4b[x];
    numbers[5][1][x] = n5b[x];
    numbers[6][1][x] = n6b[x];
    numbers[7][1][x] = n7b[x];
    numbers[8][1][x] = n8b[x];
    numbers[9][1][x] = n9b[x];
  }
}
