#include "ApplicationLevel.h"
#include "AbstractionLayer.h"

#include "../Config/Config_Scav.h"

#include "ace/streams.h"
#include "ace/OS_NS_stdlib.h"
#include "ace/OS_NS_sys_stat.h"

ApplicationLevel::ApplicationLevel(AbstractionLayer*  abstract,
                                   const ACE_TString& nodename,
				   const bool  ignore_steering,
				   const bool  ignore_motor)
: abstraction_layer_(abstract)
, nodename_(nodename)
, ignore_steering_(ignore_steering)
, ignore_motor_(ignore_motor)

{
  if (0 != abstraction_layer_)
  {
    abstraction_layer_->attach_application(this);
  }
  steering_inprogress = motor_inprogress = true;

  //read the configuration file
  Config_Scav myconfig;
  myconfig.ImportConfigFile(ACE_TEXT("Config_MotorControl.ini"));

  cout << "ApplicationLevel launched by a publisher or a subscriber" << endl;
}


ApplicationLevel::~ApplicationLevel()
{
}

Actuators::Steering received_steering_;
// call by the Abstraction Layer
void ApplicationLevel::receive_steering(const Actuators::Steering& steering)
{
   //received_steering_ = steering;
   if(steering.steering_orientation == Actuators::STEERING_END){
     steering_inprogress =  false;
     cout << "ApplicationLevel ===> Steering publication ended" << endl;
     return;
   }

   //cout << "ApplicationLevel ===> Steering angle : " << ((int)steering.steering_angle  * ANGLE_SCALE_PARAM + ANGLE_SHIFT_PARAM)
   cout << "ApplicationLevel ===> Steering angle : " << (int)steering.steering_angle
	//<< " Steering orientation : " << steering.steering_orientation
	<< " Steering count : " << steering.steering_count
	<< endl;
}

// call by the application 
void ApplicationLevel::receive_vehicle_heading (int& vehicle_heading)
{
   vehicle_heading = received_steering_.steering_angle * ANGLE_SCALE_PARAM + ANGLE_SHIFT_PARAM;
}

// call by the application
Actuators::Steering steering_ = {Actuators::STEERING_LEFT,0,99,0};
void ApplicationLevel::send_vehicle_heading (const int& vehicle_heading)
{
   steering_.steering_angle = (vehicle_heading - ANGLE_SHIFT_PARAM)/ANGLE_SCALE_PARAM;
   abstraction_layer_->send_steering(steering_);
   steering_.steering_count++;
}

Actuators::Motor received_motor_;
// call by the Abstraction Layer
void ApplicationLevel::receive_motor(const Actuators::Motor& motor)
{
   //received_motor_ = motor;
   if(motor.motor_direction == Actuators::MOTOR_END){
     motor_inprogress =  false;
     cout << "ApplicationLevel ===> Motor publication ended" << endl;
     return;
   }

   //cout << "ApplicationLevel ===> Motor speed : " << ((int)motor.motor_speed * SPEED_SCALE_PARAM + SPEED_SHIFT_PARAM)
   cout << "ApplicationLevel ===> Motor speed : " << (int)motor.motor_speed
	//<< " Motor direction : " << motor.motor_direction
	<< " Motor count : " << motor.motor_count
	<< endl;
}

// call by the application 
void ApplicationLevel::receive_vehicle_speed (int& vehicle_speed)
{
   vehicle_speed = received_motor_.motor_speed * SPEED_SCALE_PARAM + SPEED_SHIFT_PARAM;
}

// call by the application
Actuators::Motor motor_ = {Actuators::MOTOR_FORWARD,0,98,0};
void ApplicationLevel::send_vehicle_speed (const int& vehicle_speed)
{
   motor_.motor_speed = (vehicle_speed - SPEED_SHIFT_PARAM)/SPEED_SCALE_PARAM;
   abstraction_layer_->send_motor(motor_);
   motor_.motor_count++;
}

//call by the publisher
void ApplicationLevel::steering_publisher_finished()
{
  motor_.motor_direction = Actuators::MOTOR_END;
  abstraction_layer_->send_motor(motor_);
  cout << "ApplicationLayer ===> Publisher stops sending steering" << endl;
  //steering_inprogress = false;
}
//call by the subscriber application
bool ApplicationLevel::steering_publisher_inprogress()
{
  return steering_inprogress;
}

//call by the publisher
void ApplicationLevel::motor_publisher_finished()
{
  steering_.steering_orientation = Actuators::STEERING_END;
  abstraction_layer_->send_steering(steering_);
  cout << "ApplicationLayer ===> Publisher stops sending motor" << endl;
  //motor_inprogress = false;
}
//call by the subscriber application
bool ApplicationLevel::motor_publisher_inprogress()
{
  return motor_inprogress;
}

