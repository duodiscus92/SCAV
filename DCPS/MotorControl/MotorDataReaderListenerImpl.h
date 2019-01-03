/*
 *
 * Adapted by J. Ehrlich
 * Distributed under the OpenDDS License.
 * See: http://www.opendds.org/license.html
 */

#ifndef MOTOR_DATAREADER_LISTENER_IMPL_H
#define MOTOR_DATAREADER_LISTENER_IMPL_H

#include <ace/Global_Macros.h>

#include <dds/DdsDcpsSubscriptionC.h>
#include <dds/DCPS/LocalObject.h>
#include <dds/DCPS/Definitions.h>

class AbstractionLayer;

class MotorDataReaderListenerImpl
  : public virtual OpenDDS::DCPS::LocalObject<DDS::DataReaderListener> {
public:
  MotorDataReaderListenerImpl(AbstractionLayer* monitor);

  virtual ~MotorDataReaderListenerImpl();

  virtual void on_requested_deadline_missed(
    DDS::DataReader_ptr reader,
    const DDS::RequestedDeadlineMissedStatus& status);

  virtual void on_requested_incompatible_qos(
    DDS::DataReader_ptr reader,
    const DDS::RequestedIncompatibleQosStatus& status);

  virtual void on_sample_rejected(
    DDS::DataReader_ptr reader,
    const DDS::SampleRejectedStatus& status);

  virtual void on_liveliness_changed(
    DDS::DataReader_ptr reader,
    const DDS::LivelinessChangedStatus& status);

  virtual void on_data_available(
    DDS::DataReader_ptr reader);

  virtual void on_subscription_matched(
    DDS::DataReader_ptr reader,
    const DDS::SubscriptionMatchedStatus& status);

  virtual void on_sample_lost(
    DDS::DataReader_ptr reader,
    const DDS::SampleLostStatus& status);

private:

  // Pointer to the change monitor.  This class does not take ownership.
  AbstractionLayer* change_monitor_;
};

#endif /* Motor_DATAREADER_LISTENER_IMPL_H */
