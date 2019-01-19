// Source from DistributedContent example adapted by J. Ehrlich
#ifndef APPLICATIONLEVEL_H_
#define APPLICATIONLEVEL_H_
#define ANGLE_SCALE_PARAM 0.3333333333
#define ANGLE_SHIFT_PARAM -50
#define SPEED_SCALE_PARAM 0.1111111111
#define SPEED_SHIFT_PARAM -16.66666666

#include "MotorC.h"

#include <ace/String_Base.h>

/// Forward Declarations
class AbstractionLayer;


/**
 * @class ApplicationLevel
 *
 * @brief Provides the actual control of the steering and the motor.
 *
 */
class ApplicationLevel
{
public:
  /**
   * Constructor
   *
   * @param abstract - porinter to the Abstraction Layer
   * @param nodename - name of this application
   * @return
   */
  ApplicationLevel(AbstractionLayer*  abstract,
                   const ACE_TString& nodename,
		   const bool ignore_steering, 
		   const bool ignore_motor);

  virtual ~ApplicationLevel();

  /**
   * Receive a published Steering
   *
   * @param steering - the Steering
   */
  void receive_steering (const Actuators::Steering& steering);

  /**
   * Receive a published steering angle
   *
   * @param vehicle_heading - the Vehicle heading
   */
  void receive_vehicle_heading (int& vehicle_heading);

  /**
   * Published a Steering
   *
   * @param vehicle_heading - the Vehicle heading
   */
  void send_vehicle_heading (const int& vehicle_heading);


  /**
   * Receive a published Motor
   *
   * @param motor - the Motor
   */
  void receive_motor (const Actuators::Motor& motor);

  /**
   * Receive a published speed
   *
   * @param vehicle_speed - the Vehicle Speed
   */
  void receive_vehicle_speed (int& vehicle_peed);

  /**
   * Published a Speed
   *
   * @param vehicle_speed - the Vehicle Speed
   */
  void send_vehicle_speed (const int& vehicle_speed);

   /**
    * Publication in progress
    *
    */
  bool steering_publisher_inprogress();

   /**
    * Publisher finished
    *
    */
  void steering_publisher_finished();

   /**
    * Publication in progress
    *
    */
  bool motor_publisher_inprogress();

   /**
    * Publisher finished
    *
    */
  void motor_publisher_finished();


private:
  /// Abstraction Layer that obscures the sending / receiving
  /// on the DDS system.
  AbstractionLayer* abstraction_layer_;
  /// Name of this node
  ACE_TString       nodename_;
  /// Publication in progress
  bool steering_inprogress, motor_inprogress;
  // Ignore flags
  bool ignore_motor_, ignore_steering_;

};

#endif
