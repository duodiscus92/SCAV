/* adapted from OpenDDS by J. Ehrlich */
#include <string>
#include <unistd.h>
#include "ace/Get_Opt.h"
#include "ace/OS_NS_unistd.h"
#include "ace/OS_NS_stdlib.h"
#include "ace/OS_NS_sys_stat.h"
#include "ace/Time_Value.h"
#include "ace/streams.h"

#include "AbstractionLayer.h"
#include "ApplicationLevel.h"

#include "../Config/Config_Scav.h"
#include "../ScenarioReader/ScenarioReader.h"

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
  AbstractionLayer abs_layer(false, false);

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
      ApplicationLevel app_level(&abs_layer, node_name, false, false, NULL);
      cout << "Publisher ==> Launching Application Level" << endl;

      // Publish initial speed (in dm/h, range 0 to +140 or -10) and steering heading (in degrees, range -30 to +30)
      // Note : speed and heading  are two temporarily variable for test prurpose only
      app_level.send_vehicle_heading(0);
      app_level.send_vehicle_speed(0); 

      // Read a scenario in a config file
      char Scenario[200]; int Iteration;
      Config_Scav myconfig;
      myconfig.ImportConfigFile(ACE_TEXT("Config_MotorControl.ini"));
      myconfig.GetStringFromConfigFile(ACE_TEXT("scenarios"), ACE_TEXT("huit"), Scenario);
      myconfig.GetIntFromConfigFile(ACE_TEXT("iteration"), ACE_TEXT("itnum"), &Iteration);

      //Publish the scenario
      ScenarioReader myreader;
      int cnt=1, pentduration; useconds_t pdecduration; float duration, heading, speed;
      for(int i = 0; i < Iteration; i++){
      myreader.ScenarioParse(Scenario);
      while(!myreader.ScenarioGetItem(&duration, &heading, &speed) ) {
	// publsih speed and steering angle
	app_level.send_vehicle_heading((int)heading);
      	app_level.send_vehicle_speed((int)speed); 
	cout << "Publishing Sample : " << cnt++ << " speed (dm/h) : " << (int)speed << " steering heading (°) : " << (int)heading << " duration (s) : " << duration << endl;
	// wait a duration
        pentduration = duration; //take the integer part of duration
	pdecduration = (duration - pentduration) * 1000000L; // take the decimal part of duration and convert in microsec
        //cout << "Publisher debug  ===> pentduration : " << pentduration << " pdecduration : " << pdecduration << endl; 
        ACE_OS::sleep(pentduration);  //sleep X seconds
        usleep(pdecduration); //sleep Y mucroseconds
      }
      }
 
     /*
      // Loop sleeping and sending new speed and steering angle
      for (int cnt = -10; cnt <= 10; cnt++) {
	int speed, heading;
        heading = cnt;
	speed = cnt*0.5;
	// publsih speed and steering angle
	app_level.send_vehicle_heading(heading);
      	app_level.send_vehicle_speed(speed); 
	cout << "Publishing Sample : " << cnt << " speed (dm/h) : " << speed << " steering heading (°) : " << heading << endl;
	// wait a while
        ACE_OS::sleep(1);
      }
      */

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
