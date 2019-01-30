#include "RpiCtrl.h"

RpiCtrl::RpiCtrl()
{
  rpi_init_done = false;
}

RpiCtrl::~RpiCtrl(){}

int RpiCtrl::rpi_init(void)
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

int RpiCtrl:: rpi_init_as_root_temporarily(void)
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

void RpiCtrl::rpi_pwm_init(void)
{
   /* ... unprivileged operations ... */
   //fprintf(stderr, "Entering into rpi_pwm_init\n");
#ifdef RASPBERRY_PI
   pinMode (1, PWM_OUTPUT);
   pwmSetMode (PWM_MODE_MS);
   pwmSetClock (192);
   pwmSetRange (2000);
#else
   fprintf(stderr, "rpi_pwm_init ==> Nothing done : running on non-raspberry platform\n");
#endif
   //§fprintf(stderr, "Exiting rpi_pwm_init\n");
}

void RpiCtrl::rpi_pwm_set(char c)
{
   /* ... unprivileged operations ... */
   //fprintf(stderr, "Entering into rpi_pwm_set\n");
#ifdef RASPBERRY_PI
   pwmWrite (1, c);
#else
   fprintf(stderr, "rpi_pwm_set ==> Nothing done : running on non-raspberry platform\n");
#endif
   //fprintf(stderr, "Exiting rpi_pwm_set\n");
}

// test
#ifdef STAND_ALONE
int main(int argc, char *argv[])
{
  RpiCtrl MyRpi;

  MyRpi.rpi_init();
  MyRpi.rpi_pwm_init();
  MyRpi.rpi_pwm_set(atoi(argv[1]));
  return 0;
}
#endif
