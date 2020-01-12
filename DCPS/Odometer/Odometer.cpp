#include "Odometer.h"

volatile int odometer_left_counter = 0;
volatile int odometer_right_counter = 0;

Odometer::Odometer()
{
   odometer_left_counter = 0;
   odometer_right_counter = 0;
}

Odometer::~Odometer() {}

int Odometer::rpi_init(void)
{
    if(rpi_init_done){
    	fprintf(stderr, "WiringPi library already initialized on this ECU - call to rpi_init() ignored\n");
        return 0;
    }

    fprintf(stderr, "starting privileged operation : initialazing wiringPi\n");
#ifdef RASPBERRY_PI
    wiringPiSetup () ;
#else
   fprintf(stderr, "rpi_Init ==> Nothing done : running on non-raspberry platform\n");
#endif
   rpi_init_done = true;
   return 0;
}

int Odometer::rpi_init_as_root_temporarily(void)
{
    uid_t ruid, euid, suid; /* Real, Effective, Saved user ID */
    gid_t rgid, egid, sgid; /* Real, Effective, Saved group ID */
    int uerr, gerr;

    if(rpi_init_done){
    	fprintf(stderr, "WiringPi library already initialized on this ECU - call to rpi_init() ignored\n");
        return 0;
    }

    if (getresuid(&ruid, &euid, &suid) == -1) {
        fprintf(stderr, "Cannot obtain user identity: %m.\n");
        return EXIT_FAILURE;
    }
    if (getresgid(&rgid, &egid, &sgid) == -1) {
        fprintf(stderr, "Cannot obtain group identity: %m.\n");
        return EXIT_FAILURE;
    }
    if (ruid != (uid_t)TARGET_UID && ruid < (uid_t)MIN_UID) {
        fprintf(stderr, "Invalid user.\n");
        return EXIT_FAILURE;
    }
    if (rgid != (gid_t)TARGET_UID && rgid < (gid_t)MIN_GID) {
        fprintf(stderr, "Invalid group.\n");
        return EXIT_FAILURE;
    }

    /* Switch to target user. setuid bit handles this, but doing it again does no harm. */
    if (seteuid((uid_t)TARGET_UID) == -1) {
        fprintf(stderr, "Insufficient user privileges.\n");
        return EXIT_FAILURE;
    }

    /* Switch to target group. setgid bit handles this, but doing it again does no harm.
     * If TARGET_UID == 0, we need no setgid bit, as root has the privilege. */
    if (setegid((gid_t)TARGET_GID) == -1) {
        fprintf(stderr, "Insufficient group privileges.\n");
        return EXIT_FAILURE;
    }

    fprintf(stderr, "starting privileged operation : initialazing wiringPi\n");
#ifdef RASPBERRY_PI
    wiringPiSetup () ;
#else
   fprintf(stderr, "rpi_Init ==> Nothing done : running on non-raspberry platform\n");
#endif
    /* Drop privileges. */
    gerr = 0;
    if (setresgid(rgid, rgid, rgid) == -1) {
        gerr = errno;
        if (!gerr)
            gerr = EINVAL;
    }
    uerr = 0;
    if (setresuid(ruid, ruid, ruid) == -1) {
        uerr = errno;
        if (!uerr)
            uerr = EINVAL;
    }
    if (uerr || gerr) {
        if (uerr)
            fprintf(stderr, "Cannot drop user privileges: %s.\n", strerror(uerr));
        if (gerr)
            fprintf(stderr, "Cannot drop group privileges: %s.\n", strerror(gerr));
        return EXIT_FAILURE;
    }
    rpi_init_done = true;
    return 0;
}

void /*Odometer::*/OdometerLeftInterruptHandler(void)
{
   odometer_left_counter++;
}

void /*Odometer::*/OdometerRightInterruptHandler(void)
{
   odometer_right_counter++;
}

// initialize/deintialize odometer
void Odometer::rpi_odometer_init(void)
{
   odometer_left_counter = 0;
   odometer_right_counter = 0;
   //ste the odometer pin as an input
   pinMode(ODOMETER_LEFT_PIN,  INPUT);
   pinMode(ODOMETER_RIGHT_PIN, INPUT);
   //set the pull-up resistor to 3v3
   pullUpDnControl(ODOMETER_LEFT_PIN,  PUD_UP);
   pullUpDnControl(ODOMETER_RIGHT_PIN, PUD_UP);
   // install interrupt handler
   wiringPiISR (ODOMETER_LEFT_PIN,  INT_EDGE_RISING, OdometerLeftInterruptHandler) ;
   wiringPiISR (ODOMETER_RIGHT_PIN, INT_EDGE_RISING, OdometerRightInterruptHandler) ;
}

void Odometer::rpi_odometer_deinit(void)
{
   //remove the pull-up resistor
   pullUpDnControl(ODOMETER_LEFT_PIN,  PUD_OFF);
   pullUpDnControl(ODOMETER_RIGHT_PIN, PUD_OFF);
   // deinstall interrupt handler (not sure that it is correct)
   wiringPiISR (ODOMETER_LEFT_PIN,  INT_EDGE_RISING, NULL) ;
   wiringPiISR (ODOMETER_RIGHT_PIN, INT_EDGE_RISING, NULL) ;
}

// get the current left odometer value
int Odometer::rpi_odometer_left_get(void)
{
   return odometer_left_counter;
}

// get the current left distance (in mm)
double Odometer::rpi_distance_left_get(void)
{
   return odometer_left_counter * ODO_ACCURACY;
}

// get the current right odometer value
int Odometer::rpi_odometer_right_get(void)
{
   return odometer_right_counter;
}

// get the current right distance (in mm)
double Odometer::rpi_distance_right_get(void)
{
   return odometer_right_counter * ODO_ACCURACY;
}

// clear odometer value
void Odometer::rpi_odometer_clear(void)
{
   odometer_left_counter =  0;
   odometer_right_counter = 0;
}

#ifdef ODOMETER_STAND_ALONE
int main (int argc, char ** argv)
{
   //int odometer_left_counter_previous=0, odometer_left_counter_last, distance_left_previous, distance_left_last, speed_left;
   //int odometer_right_counter_previous=0, odometer_right_counter_last, distance_right_previous, distance_right_last, speed_right;
   double distance_previous, distance_last;
   struct timespec tempo;


   Odometer myodometer;

   myodometer.rpi_init();
   myodometer.rpi_odometer_init();
/*   for(int i = 0; i <1000 ; i++ ){
     while( (odometer_left_counter_last = myodometer.rpi_odometer_left_get()) ==  odometer_left_counter_previous);
     fprintf(stderr, "Odometer left: %d\tDistance left (dm): %d\t", odometer_left_counter_last, myodometer.rpi_distance_left_get()/100);
     odometer_left_counter_previous = odometer_left_counter_last;
     while( (odometer_right_counter_last = myodometer.rpi_odometer_right_get()) ==  odometer_right_counter_previous);
     fprintf(stderr, "Odometer right: %d\tDistance right (dm): %d\n", odometer_right_counter_last, myodometer.rpi_distance_right_get()/100);
     odometer_right_counter_previous = odometer_right_counter_last;
   }*/

/*   distance_left_previous = 0; distance_right_previous = 0;
   for(int i = 0; i < 100; i++){
     fprintf(stderr, "Odometer left: %d\tDistance left (dm): %d\tSpeed left (dm/s): %d\t", 
	myodometer.rpi_odometer_left_get(), (distance_left_last=myodometer.rpi_distance_left_get()/100), (speed_left = distance_left_last - distance_left_previous));
     fprintf(stderr, "Odometer right: %d\tDistance right (dm): %d\tSpeed right (dm/s): %d\tSPEED (dm/s):%d\n", 
	myodometer.rpi_odometer_right_get(),(distance_right_last=myodometer.rpi_distance_right_get()/100), (speed_right = distance_right_last - distance_right_previous), 
	(speed_left + speed_right)/2);
     distance_left_previous =  distance_left_last;
     distance_right_previous = distance_right_last;
     sleep(1);
   }*/

   // set sleep time to one second
   tempo.tv_nsec = 0;
   tempo.tv_sec =1;

   distance_previous = 0;
   for(int i = 0; i < 100; i++){
     fprintf(stderr, "Odometer : %d\tDistance (dm): %4.1f\tSpeed(dm/s): %4.1f\n",
	(myodometer.rpi_odometer_left_get() +  myodometer.rpi_odometer_right_get())/2,
	(distance_last = (myodometer.rpi_distance_left_get() + myodometer.rpi_distance_right_get())/200),
        distance_last - distance_previous);
     distance_previous =  distance_last;
     nanosleep(&tempo, NULL);
     //sleep(1);
   }

   myodometer.rpi_odometer_deinit();
}
#endif
