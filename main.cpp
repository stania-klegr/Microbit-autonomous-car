//################################
//I commented out some code and set a couple routes in the onButtonA method
//so I didnt have to use the remote. Also commented out some code at
//the start of main (setting the startHeading stuff).
//So I just press the A button its calibrates if not done yet then calls
//checkDir() which I just filled out the other directions which is working
//as far as getting the directions correct.
//################################

//CAR

#include "MicroBit.h"
MicroBit uBit;

uint8_t radioGroup = 223;

// function declaration
void turn_left_intersection();
void go_straight_intersection();
void turn_right_intersection();
void turn_left_no_reverse();
void turn_left_no_reverse_intersection();
void turn_left();
void go_streight();
void turn_right_no_reverse();
void turn_right();
void turn_right_slow();
void stop();
void exitPark();
int getDistance();
void crash();

void checkDir();

uint8_t routes[10][20] = {

    {3},                              //pause     0
    {2, 1, 3, 4, 0, 1, 1, 1, 2, 2, 5},//s to a    1
    {2, 2, 0, 3, 4, 0, 1, 1, 2, 2, 5},//s to b    2
    {2, 2, 1, 0, 3, 4, 0, 1, 2, 2, 5},//s to c    3
    {2, 2, 1, 1, 0, 3, 4, 0, 2, 2, 5},//s to d    4
    {2, 2, 1, 1, 1, 0, 3, 4, 1, 2, 5},//s to e    5
    {2, 2, 2, 3, 4, 2, 1, 1, 2, 2, 5},//s to f    6
    {2, 2, 1, 2, 3, 4, 2, 1, 2, 2, 5},//s to g    7
    {2, 2, 1, 1, 2, 3, 4, 2, 2, 2, 5},//s to h    8
};


int dirChecks[10][20] = {//general idea is that these are going to corespond the the routes table and we will do a check after
    {0},                           //pause     0
    {1, 1, 1, 2, 2, 2, 2, 3, 0, 0},//s to a    1    ///the numbers are 0-north 2-south etc, revitive to the cars start orintation
    {1, 1, 3, 2, 2, 2, 2, 3, 0, 0},//s to b    2
    {1, 1, 3, 2, 2, 2, 2, 3, 0, 0},//s to c    3
    {1, 1, 3, 2, 2, 2, 2, 3, 0, 0},//s to d    4
    {1, 1, 3, 2, 2, 2, 2, 3, 0, 0},//s to e    5
    {1, 2, 3, 3, 2, 2, 2, 3, 0, 0},//s to f    6
    {1, 2, 2, 3, 3, 2, 2, 3, 0, 0},//s to g    7
    {1, 2, 2, 2, 3, 1, 2, 3, 0, 0},//s to h    8
};
int currentDir = 0;//the current direction to check

char command[10];
int selected = -1;
int cInstruction = 0;

//int speed = 120;
int speed = 105;
int slowSpeed = 25;
int turnAdjust = -50;

int sleepTime = 250;

int toRun[10];
int writeIndex = 0;

int currentIndex = 0;
uint8_t rxBuffer[16];
int startHeading = 0;
bool prevIntersection = false;

int lostCounterWW = 0;

//Compass Stuff
CompassCalibration calVal;
int compassX = 0;
int compassY = 0;
int compassZ = 0;

void onData(MicroBitEvent)
{
    uBit.radio.datagram.recv(rxBuffer, 16);

    toRun[writeIndex] = rxBuffer[0];
    writeIndex++;

    uBit.display.scroll(rxBuffer[0]);
}
//Calibrate the uBit, store compass calibration data and send over radio to baseStation
 void onButtonA(MicroBitEvent)
 {
    uBit.display.print("A");
    if(uBit.compass.isCalibrated() == 0){ //uBit NOT calibrated
        uBit.compass.calibrate();
    }

    calVal = uBit.compass.getCalibration();


    // Save the calibration data to global variables
    // compassX = calVal.centre.x;
    // compassY = calVal.centre.y;
    // compassZ = calVal.centre.z;
    // uBit.display.scroll(compassX);
    // uBit.display.scroll(compassY);
    // uBit.display.scroll(compassZ);

    //After calibrating give the user a chance to place uBit the on road
    uBit.sleep(5000);
    startHeading = uBit.compass.heading();
     //startHeading = 350;
    uBit.display.scroll(startHeading);
    uBit.display.scroll("R");
    //  uBit.sleep(3000);
    //
    // toRun[writeIndex] = 7;
    // writeIndex++;
    // toRun[writeIndex] = 8;
    // writeIndex++;
 }

//Set compass calibration from data saved in global variables
//compassX, compassY and compassZ
//These have to be set by pressing the A button first then you
//can press the B button and set compass calibration values
 void onButtonB(MicroBitEvent)
 {
     uBit.display.print("B");

    CompassCalibration calVal3;
    // calVal3.centre.x = compassX;
    // calVal3.centre.y = compassY;
    // calVal3.centre.z = compassZ;
    calVal3.centre.x = 130158;
    calVal3.centre.y = 28086;
    calVal3.centre.z = -214820;
    calVal3.scale.x = 1024;
    calVal3.scale.y = 1024;
    calVal3.scale.z = 1024;
    calVal3.radius = 0;

    uBit.compass.setCalibration(calVal3);
    uBit.display.scroll("BF");
 }

int main() {

    toRun[0] = -1;

    // Initialise the micro:bit runtime.
    uBit.init();
	uBit.radio.enable();
    uBit.radio.setGroup(radioGroup);
    uBit.radio.setTransmitPower(7);

    //Setup a handler to run when data is received.
    uBit.messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, onData);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_CLICK, onButtonA);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_BUTTON_EVT_CLICK, onButtonB);

    //wait untill we recive a route before starting
    while(toRun[0] == -1)
    {
        uBit.sleep(100);
    }


    startHeading = uBit.compass.heading();
    uBit.sleep(5000);
    startHeading = uBit.compass.heading();
    uBit.display.print(startHeading);
    uBit.sleep(5000);

    // checkDir();

    selected = toRun[0];
    currentIndex++;

    int loopcount = 0;
    int dist = 0;
    while(1==1)
    {
        //runs on every 5th time around
        if(loopcount==1) {
            uBit.display.image.setPixelValue(2, 2, 255);
            dist = getDistance();
            if(dist < 7 && dist != 0)//check that we are not too close to anything
                crash();
            uBit.display.image.setPixelValue(2, 2, 0);
        }
        loopcount++;
        loopcount = loopcount % 20;//this sets the frequency that we check for blockages


        if(uBit.io.P13.getDigitalValue())
        {
            if(uBit.io.P14.getDigitalValue())//double white
            {
                if(lostCounterWW > 100)//if we have been lost in white space too long
                    crash();
                lostCounterWW++;

                turn_left_no_reverse();
                prevIntersection = false;
            }
            else //left white right black
            {
                lostCounterWW = 0;
                if(prevIntersection){

                    prevIntersection = false;

                    if(routes[selected][cInstruction] == 0){//turn left
                        turn_left_intersection();
                    }
                    else if(routes[selected][cInstruction] == 1){//go straight
                        go_straight_intersection();
                    }
                    else if(routes[selected][cInstruction] == 2){//turn right
                        turn_right_intersection();
                    }
                    else if(routes[selected][cInstruction] == 3){//pause
                        stop();
                        checkDir();//CHECK DIRECTION
                        uBit.sleep(4000);
                        cInstruction++;
                    }
                    else if(routes[selected][cInstruction] == 4){//exitPark
                        exitPark();
                    }
                    else if(routes[selected][cInstruction] == 5){//go to route
                        go_straight_intersection();
                        cInstruction = 0;

                        selected = toRun[currentIndex];
                        currentIndex++;

                        // selected++;

                        uBit.display.print(selected);
                    }
                }
                else{
                    //checkDir();
                    prevIntersection = true;
                }
            }
        }
        else
        {
            prevIntersection = false;
            lostCounterWW = 0;

            if(uBit.io.P14.getDigitalValue()) //left black right white
            {
                go_streight();
            }
            else //double black
            {
                turn_right_no_reverse();
            }
        }
    }
    release_fiber();
}

void go_straight_intersection() {

    go_streight();
    uBit.display.image.setPixelValue(4, 4, 255);
    uBit.sleep(500);
    uBit.display.image.setPixelValue(4, 4, 0);

    cInstruction++;
    checkDir();
}

void turn_left_intersection() {
    while (!(uBit.io.P13.getDigitalValue() && uBit.io.P14.getDigitalValue())) {
         turn_left_no_reverse_intersection();
    }
    while (uBit.io.P13.getDigitalValue() && uBit.io.P14.getDigitalValue()) {
        turn_left_no_reverse_intersection();
    }

    go_streight();
    uBit.display.image.setPixelValue(4, 4, 255);
    uBit.sleep(250);
    uBit.display.image.setPixelValue(4, 4, 0);
    cInstruction++;//finished the instruction

    checkDir();
}

void turn_right_intersection() {
    // run the motors for a bit to make sure that we are clear of the line
    while(1){
        turn_right_no_reverse();
        if((!uBit.io.P13.getDigitalValue()) && (!uBit.io.P14.getDigitalValue()))
        break;
    }
    cInstruction++;
    checkDir(); //Check uBit turned rougly into the right direction
}

void exitPark() {
    while (!(uBit.io.P13.getDigitalValue() && uBit.io.P14.getDigitalValue())) {
        turn_left_no_reverse_intersection();
    }
    cInstruction++;
}

void turn_left_no_reverse() {

    command[0] = 0; // Which motor (0, 1)
    command[1] = 1; // Direction (0, 1)
    command[2] = speed / 2; //speed
    uBit.i2c.write(0x10 << 1, command, 3);

    command[0] = 2; // Which motor (0, 1)
    command[1] = 0; // Direction (0, 1)
    command[2] = speed; //speed
    uBit.i2c.write(0x10 << 1, command, 3);

    uBit.sleep(20);
}

void turn_left_no_reverse_intersection() {

    command[0] = 0; // Which motor (0, 1)
    command[1] = 1; // Direction (0, 1)
    command[2] = 0; //speed
    uBit.i2c.write(0x10 << 1, command, 3);

    command[0] = 2; // Which motor (0, 1)
    command[1] = 0; // Direction (0, 1)
    command[2] = speed; //speed
    uBit.i2c.write(0x10 << 1, command, 3);

    uBit.sleep(20);
}

void turn_right_no_reverse() {

    // Operate a motor
    command[0] = 0; // Which motor (0, 1)
    command[1] = 0; // Direction (0, 1)
    command[2] = speed; // Speed
    uBit.i2c.write(0x10 << 1, command, 3);

    command[0] = 2; // Which motor (0, 1)
    command[1] = 1; // Direction (0, 1)
    command[2] = speed / 2; // Speed
    uBit.i2c.write(0x10 << 1, command, 3);

    uBit.sleep(20);
}
void turn_left() {

    command[0] = 0; // Which motor (0, 1)
    command[1] = 1; // Direction (0, 1)
    command[2] = speed; //speed
    uBit.i2c.write(0x10 << 1, command, 3);

    command[0] = 2; // Which motor (0, 1)
    command[1] = 0; // Direction (0, 1)
    command[2] = speed; //speed
    uBit.i2c.write(0x10 << 1, command, 3);

    uBit.sleep(20);
}

void go_streight() {

    //both on

    // Operate a motor
    command[0] = 0; // Which motor (0, 1)
    command[1] = 0; // Direction (0, 1)
    command[2] = speed; // Speed
    uBit.i2c.write(0x10 << 1, command, 3);

    // Operate a motor
    command[0] = 2; // Which motor (0, 1)
    command[1] = 0; // Direction (0, 1)
    command[2] = speed; // Speed
    uBit.i2c.write(0x10 << 1, command, 3);

    uBit.sleep(20);

}
void turn_right() {
    // Operate a motor
    command[0] = 0; // Which motor (0, 1)
    command[1] = 0; // Direction (0, 1)
    command[2] = speed; // Speed
    uBit.i2c.write(0x10 << 1, command, 3);

    // Operate a motor
    command[0] = 2; // Which motor (0, 1)
    command[1] = 1; // Direction (0, 1)
    command[2] = speed; // Speed
    uBit.i2c.write(0x10 << 1, command, 3);

    uBit.sleep(20);
}
void stop(){
    command[0] = 0; // Which motor (0, 1)
    command[2] = 0; //speed
    uBit.i2c.write(0x10 << 1, command, 3);

    command[0] = 2; // Which motor (0, 1)
    command[2] = 0; //speed
    uBit.i2c.write(0x10 << 1, command, 3);

    uBit.sleep(20);
}

//returns the time in micros that it took for the pulseHigh to come in
int pulseIn(MicroBitPin* pin, int maxDuration = 250000) {

    if (!pin) return 0;

    int pulse = 1;
    uint64_t tick =  system_timer_current_time_us();
    uint64_t maxd = (uint64_t)maxDuration;
    while(pin->getDigitalValue() != pulse) {//while we haven't seen a pulse
    if(system_timer_current_time_us() - tick > maxd)
    return 0;
}

uint64_t start =  system_timer_current_time_us();
while(pin->getDigitalValue() == pulse) {
    if(system_timer_current_time_us() - tick > maxd)
    return 0;
}
uint64_t end =  system_timer_current_time_us();
return end - start;
}

//return distance in centimeters
int getDistance(){

    uBit.display.image.setPixelValue(1, 0, 255);

    //send the pulse
    uBit.io.P1.setPull(PullNone);
    uBit.io.P1.setDigitalValue(0);
    wait_us(2);
    uBit.io.P1.setDigitalValue(1);
    wait_us(10);
    uBit.io.P1.setDigitalValue(0);
    uBit.io.P1.setPull(PullUp);

    uBit.display.image.setPixelValue(1, 0, 0);

    //read the returning pulse
    return pulseIn(&uBit.io.P2) / 42;
}
//pretty self explanitory
void crash(){
    stop();
    while (1) {
        const uint8_t sad_face_arr[] {
            0, 1, 0, 1, 0,
            0, 1, 0, 1, 0,
            0, 0, 0, 0, 0,
            0, 1, 1, 1, 0,
            1, 0, 0, 0, 1, };
        MicroBitImage sad_face(5,5,sad_face_arr);
        uBit.display.print(sad_face);
    }
}

void checkDir(){
    uBit.sleep(280);
    stop();
    uBit.display.print("M");
    uBit.sleep(500);

    int m = uBit.compass.heading();
    uBit.sleep(150);
    int n = uBit.compass.heading();

    if(abs(m - n) > 20){

        uBit.display.scroll("blah");
        uBit.sleep(500);
    }



    int bearing = (m + n) / 2;


    uBit.display.scroll(bearing);

    if(dirChecks[selected][currentDir] == 0){//check we are facing north
        uBit.display.print("0");
        uBit.sleep(200);

        int x = (270 + startHeading) % 360;
        int y = (90 + startHeading) % 360;

        if(x > y)
        {
            y += 360;
            bearing += 360;
        }

        if(!(bearing > x && bearing < y))
        {
            uBit.display.scroll("Not north");
            uBit.sleep(1000);
            crash();
        }
    }
    else if(dirChecks[selected][currentDir] == 1){//check we are facing east
        uBit.display.print("1");
        uBit.sleep(200);

        int x = (1 + startHeading) % 360;
        int y = (180 + startHeading) % 360;

        if(x > y)
        {
            y += 360;
            //bearing += 360;
        }

        if(!(bearing > x && bearing < y))
        {
            uBit.display.scroll("Not east");
            uBit.sleep(1000);
            crash();
        }
    }
    else if(dirChecks[selected][currentDir] == 2){//check we are facing south
        uBit.display.print("2");
        uBit.sleep(200);

        int x = (90 + startHeading) % 360;
        int y = (270 + startHeading) % 360;

        if(x > y)
        {
            y += 360;
            bearing += 360;
        }

        if(!(bearing > x && bearing < y))
        {
            uBit.display.scroll("Not south");
            uBit.sleep(1000);
            crash();
        }
    }
    else if(dirChecks[selected][currentDir] == 3){//check we are facing west
        uBit.display.print("3");
        uBit.sleep(200);

        int x = (180 + startHeading) % 360;
        int y = (359 + startHeading) % 360;

        if(x > y)
        {
            y += 360;
            bearing += 360;
        }

        if(!(bearing > x && bearing < y))
        {
            uBit.display.scroll("Not west");
            uBit.sleep(1000);
            crash();
        }
    }
    currentDir++;
}
