// Source from DistributedContent example adapted by J. Ehrlich
#ifndef ABSTRACTIONLAYER_H_
#define ABSTRACTIONLAYER_H_

#include "MotorC.h"
#include "dds/DdsDcpsInfoUtilsC.h"
#include <dds/DCPS/Service_Participant.h>
#include <dds/DCPS/PublisherImpl.h>
#include <dds/DCPS/SubscriberImpl.h>
#include <dds/DCPS/transport/tcp/TcpInst.h>

#include "MotorTypeSupportImpl.h"

enum mode {IS_PUBLISHER, IS_SUBSCRIBER};

// Forward Declarations
class ApplicationLevel;


/** Variables that will be consistent across all the nodes in the same
    DDS Domain. **/

/// Id of the DDS domain that all the transactions will take place in.
/// This id needs to be in the domain_ids file that is given to the DCPSInfoRepo application
const int DOMAINID = 111;



/**
 * @class AbstractionLayer
 *
 * @brief Bridges the DDS and Application Level functionality.
 *
 */
class AbstractionLayer
{
public:
  AbstractionLayer();
  virtual ~AbstractionLayer();

  /**
   * Initialize the DDS system.
   *
   * @param argc    - number of command line arguments
   * @param *argv[] - command line arguments
   *
   * @return true if the DDS system was intialized correctly,
   *         false otherwise.
   */
  bool init_DDS(int& argc, ACE_TCHAR *argv[], enum mode current_mode);

  /*
   * Set the ignore mode (motor or steering)
   *
   *  @param motor steering
   */
  void set_ignoremode(bool ignore_motor, bool ignore_steering);

  /**
   * Shutdown the DDS System
   */
  bool shutdown_DDS();

  /**
   * Attach application level code to the abstraction layer.
   *
   * @param app
   */
  void attach_application(ApplicationLevel* app);

  /**
   * Handle the reception of a Steering object.
   * This method is called by the DDS system code.
   * This method will try to call the Application Level code.
   *
   * @param steering - Steering received by the DDS subscriber.
   */
  void receive_steering(const Actuators::Steering& steering);
  /**
   * Handle the sending of a Steering object.
   * This method is called by the Application Level code.
   * This method will try to call the DDS system code.
   *
   * @param steering - Steering to be sent by the DDS publisher
   *
   * @return
   */
  bool send_steering(const Actuators::Steering& steering);

  /**
   * Handle the reception of a Motors object.
   * This method is called by the DDS system code.
   * This method will try to call the Application Level code.
   *
   * @param motor - Motor received by the DDS subscriber.
   */
  void receive_motor(const Actuators::Motor& motor);
  /**
   * Handle the sending of a Motor object.
   * This method is called by the Application Level code.
   * This method will try to call the DDS system code.
   *
   * @param motor - Motor to be sent by the DDS publisher
   *
   * @return
   */
  bool send_motor(const Actuators::Motor& motor);


private:
  /** References to DDS System Code **/

  /// The publishers DDS transport
  OpenDDS::DCPS::TransportImpl_rch        pub_tcp_impl_;
  /// The subscribers DDS transport
  OpenDDS::DCPS::TransportImpl_rch        sub_tcp_impl_;

  /// Factory for creating Domain Participants
  ::DDS::DomainParticipantFactory_var dpf_;
  /// The Domain Participant reference
  ::DDS::DomainParticipant_var	dp_;
  /// The Topics that will be published and subscribed to
  ::DDS::Topic_var                    steering_topic_, motor_topic_;

  /// The publisher reference
  ::DDS::Publisher_var		pub_;
  /// The Data Writer reference (does the actual publishing)
  ::DDS::DataWriter_var		steering_dw_, motor_dw_;
  /// Implementation of the Data writer (needed for sending the Steerings)
  Actuators::SteeringDataWriter_var	steering_writer_;
  /// Implementation of the Data writer (needed for sending the Motor)
  Actuators::MotorDataWriter_var	motor_writer_;
  /// Instance handle for publishing (one per key published)
  ::DDS::InstanceHandle_t	steering_handle_, motor_handle_;

  /// The subscriber reference
  ::DDS::Subscriber_var         sub_;
  /// Listener for alerting that data has been published
  ::DDS::DataReaderListener_var 	steering_listener_;
  ::DDS::DataReaderListener_var  	motor_listener_;
  /// Data Reader for receiving the published data
  ::DDS::DataReader_var         steering_dr_, motor_dr_;


  /// The reference to the Application Level code
  ApplicationLevel* application_;

  /// The ignore mode flags;
  bool ignore_motor, ignore_steering;
};

#endif
