// Source from DistributedContent example adapted by J. Ehrlich
#include "AbstractionLayer.h"
#include "MotorDataReaderListenerImpl.h"
#include "SteeringDataReaderListenerImpl.h"

#include "ace/streams.h"

#include <dds/DCPS/Marked_Default_Qos.h>
#include <dds/DCPS/DomainParticipantImpl.h>
#include <dds/DCPS/BuiltInTopicUtils.h>
#include <dds/DCPS/WaitSet.h>
//#include "MotorListener.h"
#include "ApplicationLevel.h"

AbstractionLayer::AbstractionLayer()
: steering_handle_(0)
, motor_handle_(0)
, application_(0)
, ignore_motor(false)
, ignore_steering(false)
{
}


AbstractionLayer::~AbstractionLayer()
{
}


bool
AbstractionLayer::init_DDS(int& argc, ACE_TCHAR *argv[], enum mode current_mode)
{
  // Initialize the Participant Factory
  dpf_ = TheParticipantFactoryWithArgs (argc, argv);
  if (CORBA::is_nil (dpf_.in ()) ) {
    ACE_ERROR((LM_ERROR,
      ACE_TEXT("ERROR - Create participant factory failed.\n") ));
    return false;
  }


  // Create participant
  dp_ = dpf_->create_participant (DOMAINID,
                                  PARTICIPANT_QOS_DEFAULT,
                                  //DDS::DomainParticipantListener::_nil (),
				  0,
                                  ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);
  if (CORBA::is_nil (dp_.in ()) ) {
    ACE_ERROR((LM_ERROR,
      ACE_TEXT("ERROR - Create participant failed.\n") ));
    return false;
  }

/*************************************************************************/
/**************************** TOPIC SECTION ******************************/
/*************************************************************************/
  // Create the Motor topic for datawriter and datareader
  Actuators::MotorTypeSupportImpl* motor_dt =
                new Actuators::MotorTypeSupportImpl();
  motor_dt->register_type (dp_.in (),
                              "Actuators::Motor");
   motor_topic_ = dp_->create_topic ("motor_topic", // topic name
                               "Actuators::Motor", // topic type
                               TOPIC_QOS_DEFAULT,
                               //DDS::TopicListener::_nil (),
			       0,
                               ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);
  if (CORBA::is_nil (motor_topic_.in ()) ) {
    ACE_ERROR((LM_ERROR,
      ACE_TEXT("ERROR - Create motor topic failed.\n") ));
    return false;
  }

  // Create the Steering topic for datawriter and datareader
  Actuators::SteeringTypeSupportImpl* steering_dt =
                new Actuators::SteeringTypeSupportImpl();
  steering_dt->register_type (dp_.in (),
                              "Actuators::Steering");
   steering_topic_ = dp_->create_topic ("steering_topic", // topic name
                               "Actuators::Steering", // topic type
                               TOPIC_QOS_DEFAULT,
                               //DDS::TopicListener::_nil (),
			       0,
                               ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);
  if (CORBA::is_nil (steering_topic_.in ()) ) {
    ACE_ERROR((LM_ERROR,
      ACE_TEXT("ERROR - Create steering topic failed.\n") ));
    return false;
  }


/*************************************************************************/
/************************ PUBLISHER SECTION ******************************/
/*************************************************************************/
if(current_mode ==  IS_PUBLISHER){
  cout << "AbstractionLayer ===> I am a publisher, nice to meet you." << endl;
  // Create publisher
  pub_ = dp_->create_publisher (PUBLISHER_QOS_DEFAULT,
                                //DDS::PublisherListener::_nil (),
				0,
                                ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);
  if (CORBA::is_nil (pub_.in ()) ) {
    ACE_ERROR((LM_ERROR,
      ACE_TEXT("ERROR - Create publisher failed.\n") ));
    return false;
  }

  // Create the datawriter for steering
  steering_dw_ = pub_->create_datawriter (steering_topic_.in (),
                                 DATAWRITER_QOS_DEFAULT,
                                 //DDS::DataWriterListener::_nil (),
				 0,
                                 ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);
  if (CORBA::is_nil (steering_dw_.in ()) ) {
    ACE_ERROR((LM_ERROR,
      ACE_TEXT("ERROR - Create data writer for steering failed.\n") ));
    return false;
  }

  // Narrow down the data writer to the SteeringDataWriter
  steering_writer_ = Actuators::SteeringDataWriter::_narrow (steering_dw_.in());
  if (CORBA::is_nil (steering_writer_.in ()) ) {
    ACE_ERROR((LM_ERROR,
      ACE_TEXT("ERROR - Narrow of steering data writer failed.\n") ));
    return false;
  }

  // Create the datawriter for motor
  motor_dw_ = pub_->create_datawriter (motor_topic_.in (),
                                 DATAWRITER_QOS_DEFAULT,
                                 //DDS::DataWriterListener::_nil (),
				 0,
                                 ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);
  if (CORBA::is_nil (motor_dw_.in ()) ) {
    ACE_ERROR((LM_ERROR,
      ACE_TEXT("ERROR - Create data writer for steering failed.\n") ));
    return false;
  }

  // Narrow down the data writer to the MotorDataWriter
  motor_writer_ = Actuators::MotorDataWriter::_narrow (motor_dw_.in());
  if (CORBA::is_nil (motor_writer_.in ()) ) {
    ACE_ERROR((LM_ERROR,
      ACE_TEXT("ERROR - Narrow of motor data writer failed.\n") ));
    return false;
  }

  // Block until Subscriber is available
  DDS::StatusCondition_var condition = motor_dw_->get_statuscondition();
  condition->set_enabled_statuses(DDS::PUBLICATION_MATCHED_STATUS);
  
  DDS::WaitSet_var ws = new DDS::WaitSet;
  ws->attach_condition(condition);

  while (true) {
    DDS::PublicationMatchedStatus matches;
    if (motor_dw_->get_publication_matched_status(matches) != ::DDS::RETCODE_OK) {
      ACE_ERROR_RETURN((LM_ERROR,
                          ACE_TEXT("ERROR: %N:%l: main() -")
                          ACE_TEXT(" get_publication_matched_status failed!\n")),
                         -1);
    }

    if (matches.current_count >= 1) {
      break;
    }

    DDS::ConditionSeq conditions;
    DDS::Duration_t timeout = { 60, 0 };
    if (ws->wait(conditions, timeout) != DDS::RETCODE_OK) {
      ACE_ERROR_RETURN((LM_ERROR,
                          ACE_TEXT("ERROR: %N:%l: main() -")
                          ACE_TEXT(" wait failed!\n")),
                         -1);
    }
  }

  ws->detach_condition(condition);

}
/*************************************************************************/
/************************ SUBSCRIBER SECTION ******************************/
/*************************************************************************/
else if(current_mode == IS_SUBSCRIBER) {
  cout << "AbstractionLayer ===> I am a" 
       << (ignore_motor ? " non-motor but" : " motor" )
       << (ignore_steering ? " but non-steering" : "steering") 
       << " subscriber, nice to meet you." << endl;
  // Create the subscriber
  sub_ = dp_->create_subscriber(SUBSCRIBER_QOS_DEFAULT,
                                //DDS::SubscriberListener::_nil(),
				0,
                                ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);
  if (CORBA::is_nil (sub_.in ()) ) {
    ACE_ERROR((LM_ERROR,
      ACE_TEXT("ERROR - Create subscriber failed.\n") ));
    return false;
  }


  // create motor listener and data reader only if this topic is subsrcibed
  if(ignore_motor == false) {
    // Create the listener for motor datareader
    motor_listener_ = new MotorDataReaderListenerImpl(this);

    // Create the datareader for motor
    motor_dr_ = sub_->create_datareader (motor_topic_.in (),
                                 DATAREADER_QOS_DEFAULT,
                                 motor_listener_.in (),
                                 ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);
    if (CORBA::is_nil (motor_dr_.in ()) ) {
      ACE_ERROR((LM_ERROR, "ERROR - Create motor data reader failed.\n"));
      return false;
    }
  }

  // create steering listener and data reader only if this topic is subsrcibed
  if(ignore_steering == false) {
    // Create the listener for steering datareader
    steering_listener_ = new SteeringDataReaderListenerImpl(this);

    // Create the datareader for steering
    steering_dr_ = sub_->create_datareader (steering_topic_.in (),
                                 DATAREADER_QOS_DEFAULT,
                                 steering_listener_.in (),
                                 ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);
    if (CORBA::is_nil (steering_dr_.in ()) ) {
      ACE_ERROR((LM_ERROR, "ERROR - Create steering data reader failed.\n"));
      return false;
    }
  }
}
else{
  cout << "Abstraction Layer ==> Error : mode (publisher or subscriber) was not specified" << endl;
  return false;
}
  return true;
}

// called byt ApplicationLayer
void
AbstractionLayer::set_ignoremode(bool ignore_motor, bool ignore_steering)
{
  AbstractionLayer::ignore_motor = ignore_motor;
  AbstractionLayer::ignore_steering = ignore_steering;
}

// called by the steering listener
void 
AbstractionLayer::receive_steering(const Actuators::Steering& steering)
{
  // Pass the steering to the application if it is defined.
  if (0 != application_)
    application_->receive_steering(steering);
  else
    ACE_DEBUG((LM_DEBUG,
      "Warning: AbstractionLayer received steering while no application defined.\n"));
}

bool 
AbstractionLayer::send_steering(const Actuators::Steering& steering)
{
  DDS::ReturnCode_t error = steering_writer_->write(steering, DDS::HANDLE_NIL);
  return (error == DDS::RETCODE_OK);
}

// called by the motor listener
void 
AbstractionLayer::receive_motor(const Actuators::Motor& motor)
{
  // Pass the motor to the application if it is defined.
  if (0 != application_)
    application_->receive_motor(motor);
  else
    ACE_DEBUG((LM_DEBUG,
      "Warning: AbstractionLayer received motor while no application defined.\n"));
}

bool 
AbstractionLayer::send_motor(const Actuators::Motor& motor)
{
  DDS::ReturnCode_t error = motor_writer_->write(motor, DDS::HANDLE_NIL);
  return (error == DDS::RETCODE_OK);
}
bool
AbstractionLayer::shutdown_DDS()
{
  // Block until Publisher completes
  /*
  DDS::StatusCondition_var condition = steering_dr_->get_statuscondition();
  condition->set_enabled_statuses(DDS::SUBSCRIPTION_MATCHED_STATUS);

  DDS::WaitSet_var ws = new DDS::WaitSet;
  ws->attach_condition(condition);

  while (true) {
    DDS::SubscriptionMatchedStatus matches;
    if (steering_dr_->get_subscription_matched_status(matches) != DDS::RETCODE_OK) {
      ACE_ERROR_RETURN((LM_ERROR,
                        ACE_TEXT("ERROR: %N:%l: main() -")
                        ACE_TEXT(" get_subscription_matched_status failed!\n")), -1);
    }

    if (matches.current_count == 0 && matches.total_count > 0) {
      break;
    }

    DDS::ConditionSeq conditions;
    DDS::Duration_t timeout = { 60, 0 };
    if (ws->wait(conditions, timeout) != DDS::RETCODE_OK) {
      ACE_ERROR_RETURN((LM_ERROR,
                        ACE_TEXT("ERROR: %N:%l: main() -")
                        ACE_TEXT(" wait failed!\n")), -1);
    }
  }

  ws->detach_condition(condition);
  */

  // Clean up all the entities in the participant
  cout << "AbstractionLayer ===> Clean up all the entities in the participant" << endl;
  dp_->delete_contained_entities ();

  // Clean up the participant
  cout << "AbstractionLayer ===> Clean up the participant" << endl;
  dpf_->delete_participant (dp_.in ());

  // Clean up the transport

  // Clean up DDS
  cout << "AbstractionLayer ===> Clean up DDS" << endl;
  TheServiceParticipant->shutdown ();

  return true;
}

void
AbstractionLayer::attach_application(ApplicationLevel* app)
{
  application_ = app;
}
