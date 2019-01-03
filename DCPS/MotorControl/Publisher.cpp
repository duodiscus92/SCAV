/* adapted from OpenDDS by J. Ehrlich */
#include "ace/Get_Opt.h"
#include "ace/OS_NS_unistd.h"
#include "ace/OS_NS_stdlib.h"
#include "ace/OS_NS_sys_stat.h"
#include "ace/Time_Value.h"
#include "ace/streams.h"

#include "AbstractionLayer.h"
#include "ApplicationLevel.h"

#include <string>

#define MAX_SPEED 10
#define MIN_SPEED -5
#define MAX_ANGLE +10
#define MIN_ANGLE -10

int ACE_TMAIN (int argc, ACE_TCHAR *argv[])
{
  /// return status of main
  int status = 0;

  // Initialize DDS before parsing the command line parameters.
  // This is done so the DDS command line parameters do not go through the
  // application's parameter processing.
  AbstractionLayer abs_layer;

  if ( abs_layer.init_DDS(argc, argv, IS_PUBLISHER) ) {

    /// Overide the default name of this node.  The default is the hostname.
    ACE_TString node_name;

    // parse commmand line options
    ACE_Get_Opt get_opts (argc, argv, ACE_TEXT("n:"));
    int optionIterator;
    while ((optionIterator = get_opts ()) != EOF) {
      switch (optionIterator) {

            case 'n': /* n specifies this nodes name */
              node_name = get_opts.opt_arg ();
              break;

            case '?':
              ACE_ERROR_RETURN(
                (LM_ERROR,
                  ACE_TEXT("usage:  -n <node name>\n")),
                0);
              break;
            default: /* no parameters */
              break;
      }
    }

    // check that the node name is set
    if (0 == ACE_OS::strcmp(node_name.c_str(), ACE_TEXT(""))) {
      ACE_TCHAR local_hostname[1024];
      if ( 0 == ACE_OS::hostname(local_hostname, 1023))
        node_name = local_hostname;
      else {
        ACE_ERROR((LM_ERROR, ACE_TEXT("Failed to retreive the hostname.\n") ));
        status = -1;
      }
    }

    // Only proceed if there have been no errors
    if (status == 0) {

      // Create the application level (which connects itself to the abstraction layer)
      ApplicationLevel app_level(&abs_layer, node_name, false, false);

      // Publish initial speed (in dm/h, range 0 to +140 or -10) and steering heading (in degrees, range -30 to +30)
      // Note : speed and heading  are two temporarily variable for test prurpose only
      app_level.send_vehicle_heading(0);
      app_level.send_vehicle_speed(0); 

      // Wait up to 20 seconds to be sure that published data was received by subsrcibers
      //ACE_OS::sleep(20);

      // Loop sleeping and sending new speed and steering angle
      for (int cnt = 1; cnt <= 10; cnt++) {
	int speed, heading;
        heading = 10+cnt;
	speed = 100+cnt;
	// publsih speed and steering angle
	app_level.send_vehicle_heading(heading);
      	app_level.send_vehicle_speed(speed); 
	cout << "Publishing Sample : " << cnt << " speed (dm/h) : " << speed << " steering heading (°) : " << heading << endl;
	// wait a while
        ACE_OS::sleep(1);
      }

      // inform the subscriber that publication has finished
      app_level.motor_publisher_finished();
      app_level.steering_publisher_finished();

      // sleep waiting for other nodes to finish
      ACE_OS::sleep(30);
    }

    // Clean up DDS before finishing.
    abs_layer.shutdown_DDS();

  }
  else {
    ACE_ERROR((LM_ERROR,
      ACE_TEXT("ERROR: Initialization of DDS failed!\n")));
    ACE_ERROR((LM_ERROR,
      ACE_TEXT("usage: -n <node name> \n")));

    status = -1;
  }

  return status;
}
