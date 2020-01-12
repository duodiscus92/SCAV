#ifndef ODOMETER_H
#define ODOMETER_H

#include <unistd.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#ifdef RASPBERRY_PI
#include <wiringPi.h>
#define ODOMETER_RIGHT_PIN 4
#define ODOMETER_LEFT_PIN 5
#endif

#ifndef TARGET_UID
#define TARGET_UID 0
#endif

#ifndef TARGET_GID
#define TARGET_GID 0
#endif

#ifndef MIN_UID
#define MIN_UID 500
#endif

#ifndef MIN_GID
#define MIN_GID 500
#endif

//  Wheel circumference in mm
#define GIRTH 353
#define ODO_ACCURACY ((double)353/8)

class Odometer
{

public:
   Odometer();
   ~Odometer();

   // Interrupt handler (called on rising edge of odometer pulse)
   //void OdometerInterruptHandler(void);

   // initialize wiringPi must be run with root privilege
   int rpi_init_as_root_temporarily(void);
   int rpi_init(void);

   // initialize/deintialize odometer
   void rpi_odometer_init(void);
   void rpi_odometer_deinit(void);

   // get the current odometer value
   int rpi_odometer_left_get(void);
   int rpi_odometer_right_get(void);

   // get the current distance (in mm)
   double rpi_distance_left_get(void);
   double rpi_distance_right_get(void);

   // clear odometer value
   void rpi_odometer_clear(void);

private:
   // odometer counter
   //int odometer_counter;
   // mark that WiringPi was already initialised
   int rpi_init_done = false;
};
#endif

