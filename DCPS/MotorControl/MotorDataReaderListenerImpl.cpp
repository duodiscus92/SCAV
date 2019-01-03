/*
 *
 * Adapted by J. Ehrlich
 * Distributed under the OpenDDS License.
 * See: http://www.opendds.org/license.html
 */

#include <ace/Log_Msg.h>
#include <ace/OS_NS_stdlib.h>

#include "MotorDataReaderListenerImpl.h"
#include "MotorTypeSupportC.h"
#include "MotorTypeSupportImpl.h"
#include "AbstractionLayer.h"

#include <iostream>

MotorDataReaderListenerImpl::MotorDataReaderListenerImpl(AbstractionLayer* monitor)
: change_monitor_(monitor)
{
}


MotorDataReaderListenerImpl::~MotorDataReaderListenerImpl()
{
}

void
MotorDataReaderListenerImpl::on_requested_deadline_missed(
  DDS::DataReader_ptr /*reader*/,
  const DDS::RequestedDeadlineMissedStatus& /*status*/)
{
}

void
MotorDataReaderListenerImpl::on_requested_incompatible_qos(
  DDS::DataReader_ptr /*reader*/,
  const DDS::RequestedIncompatibleQosStatus& /*status*/)
{
}

void
MotorDataReaderListenerImpl::on_sample_rejected(
  DDS::DataReader_ptr /*reader*/,
  const DDS::SampleRejectedStatus& /*status*/)
{
}

void
MotorDataReaderListenerImpl::on_liveliness_changed(
  DDS::DataReader_ptr /*reader*/,
  const DDS::LivelinessChangedStatus& /*status*/)
{
}

void
MotorDataReaderListenerImpl::on_data_available(DDS::DataReader_ptr reader)
{
  try {
    Actuators::MotorDataReader_var reader_i =
      Actuators::MotorDataReader::_narrow(reader);

    if (!reader_i) {
      ACE_ERROR((LM_ERROR,
               ACE_TEXT("ERROR: %N:%l: on_data_available() -")
               ACE_TEXT(" _narrow failed!\n")));
      ACE_OS::exit(-1);
    }

    Actuators::Motor motor;
    DDS::SampleInfo info;

    DDS::ReturnCode_t error = reader_i->take_next_sample(motor, info);
    if (error == DDS::RETCODE_OK) {
      if (0 != change_monitor_)
        change_monitor_->receive_motor(motor);
      else
        ACE_DEBUG((LM_DEBUG,
          "FileInfoListener::on_data_available() no change_monitor_ defined\n"));
    } else if (error == DDS::RETCODE_NO_DATA) {
        ACE_ERROR((LM_ERROR,
          "ERROR: FileInfoListener::on_data_available() received DDS::RETCODE_NO_DATA!"));
      } else {
        ACE_ERROR((LM_ERROR,
          "ERROR: FileInfoListener::on_data_available() read Message: Error: %d\n",
          error));
      }
    } catch (CORBA::Exception&) {
      ACE_ERROR((LM_ERROR,
        "ERROR: FileInfoListener::on_data_available() Exception caught in read\n"));
  }// end try


  /*if (error == DDS::RETCODE_OK) {
    std::cout << "SampleInfo.sample_rank = " << info.sample_rank << std::endl;
    std::cout << "SampleInfo.instance_state = " << info.instance_state << std::endl;

    if (info.valid_data) {
      std::cout << "Message: direction    = " << motor.motor_direction.in() << std::endl
                << "         key = " << motor.motor_key   << std::endl
                << "         rpm       = " << motor.rpm.in()    << std::endl
                << "         count      = " << motor.count        << std::endl;

    }

  } else {
    ACE_ERROR((LM_ERROR,
               ACE_TEXT("ERROR: %N:%l: on_data_available() -")
               ACE_TEXT(" take_next_sample failed!\n")));
  }*/
}

void
MotorDataReaderListenerImpl::on_subscription_matched(
  DDS::DataReader_ptr /*reader*/,
  const DDS::SubscriptionMatchedStatus& /*status*/)
{
}

void
MotorDataReaderListenerImpl::on_sample_lost(
  DDS::DataReader_ptr /*reader*/,
  const DDS::SampleLostStatus& /*status*/)
{
}
