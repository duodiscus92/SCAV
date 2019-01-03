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

  if ( abs_layer.init_DDS(argc, argv, IS_SUBSCRIBER) ) {

    /// Overide the default name of this node.  The default is the hostname.
    ACE_TString node_name;
    bool ignore_motor, ignore_steering;
    ignore_motor = ignore_steering = false;

    // parse commmand line options
    ACE_Get_Opt get_opts (argc, argv, ACE_TEXT("n:ms"));
    int optionIterator;
    while ((optionIterator = get_opts ()) != EOF) {
      switch (optionIterator) {
            case 'n': /* n specifies this nodes name */
              node_name = get_opts.opt_arg ();
              break;

	    case 'm': /* m motor ignored */
	      ignore_motor = true;
	      cout << "Subscriber ===> Entering in ignore motor mode" << endl;
	      break;

	    case 's': /* s steering ignored */
	      ignore_steering = true;
	      cout << "Subscriber ===> Entering in ignore steering mode" << endl;
	      break;

            case '?':
              ACE_ERROR_RETURN(
                (LM_ERROR,
                  ACE_TEXT("usage:  -n <node name> -m ignore motor mode -s ignore steering mode, -m and -s not allowed together\n")),
                0);
              break;
            default: /* no parameters */
              break;
      }
    }
    // test if both option -m and -s are provided (not allowed) 
    if(ignore_motor && ignore_steering){
	cout << "Subscriber ===> Error : -m and -s options not allowed together" << endl;
              ACE_ERROR_RETURN(
                (LM_ERROR,
                  ACE_TEXT("usage:  -n <node name> -m subscibe to motor only, ignore steering -s subscribe to steering only, ignore motot, -m and -s not allowed together\n")),
                0);
    }

    // check that the node name is set
    if (0 == ACE_OS::strcmp(node_name.c_str(), ACE_TEXT(""))) {
      ACE_TCHAR local_hostname[1024];
      if ( 0 == ACE_OS::hostname(local_hostname, 1023)){
	cerr << "Subscriber error message ===> Hostname is: " <<  ACE_OS::hostname(local_hostname, 1023) << endl;
        node_name = local_hostname;
      }
      else {
        ACE_ERROR((LM_ERROR, ACE_TEXT("Failed to retreive the hostname.\n") ));
        status = -1;
      }
    }

    // Only proceed if there have been no errors
    if (status == 0) {

      // Create the application level (which connects itself to the abstraction layer)
      ApplicationLevel app_level(&abs_layer, node_name, ignore_steering, ignore_motor);
      cout << "Subscriber ==> Launching Application Level" << endl;

      // wait for publication until the pubisher has finished
      while ( app_level.steering_publisher_inprogress() || app_level.motor_publisher_inprogress() ) 
        ACE_OS::sleep(1);
      cout << "Subscriber ==> Shutting down because warned that publisher finished" << endl ;

      // sleep waiting for other nodes to finish
      //ACE_OS::sleep(30);
    }

    // Clean up DDS before finishing.
    abs_layer.shutdown_DDS();

  }
  else {
    ACE_ERROR((LM_ERROR,
      ACE_TEXT("ERROR: Initialization of DDS failed!\n")));
    ACE_ERROR((LM_ERROR,
      ACE_TEXT("usage: -n <node name>\n")));

    status = -1;
  }

  return status;
}
