//#define _GNU_SOURCE

#ifndef RPICTRL_H
#define RPICTRL_H

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#ifdef RASPBERRY_PI
#include <wiringPi.h>
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

class RpiCtrl
{

public:
   RpiCtrl();
   ~RpiCtrl();

   // initialize wiringPi must be run with root privilege
   int rpi_init_as_root_temporarily(void);
   int rpi_init(void);
   // initialize PWM parameters for servo control
   void rpi_pwm_init(void);
   // ser a value for PWM in the range 60-240, 150 is the middle
   void rpi_pwm_set(char c);

private:
   // mark that WiringPi was already initialised
   int rpi_init_done;
};
#endif
