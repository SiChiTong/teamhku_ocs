#include "teamhku_ocs/teamhku_ocs.h"
#include <pluginlib/class_list_macros.h>
#include <QStringList>
#include <iostream>
#include <unistd.h>

#include <QThread>

namespace teamhku {

TeamHKUOCS::TeamHKUOCS()
  : rqt_gui_cpp::Plugin()
  , widget_(0)
{
  // Constructor is called first before initPlugin function, needless to say.

  // give QObjects reasonable names
  setObjectName("TeamHKUOCS");
}

void TeamHKUOCS::initPlugin(qt_gui_cpp::PluginContext& context)
{
  // access standalone command line arguments
  QStringList argv = context.argv();
  // create QWidget
  widget_ = new QWidget();
  drone_ = new DJIDrone(nh_);
  // extend the widget with all attributes and children from UI file
  ui_.setupUi(widget_);
  // add widget to the user interface
  context.addWidget(widget_);

  ui_.accelerationXLineEdit->setReadOnly(true);
  ui_.accelerationYLineEdit->setReadOnly(true);
  ui_.accelerationZLineEdit->setReadOnly(true);

  ui_.gPSLatitudeLineEdit->setReadOnly(true);
  ui_.gPSLongitudeLineEdit->setReadOnly(true);
  ui_.gPSAltitudeLineEdit->setReadOnly(true);
  ui_.gPSHeightLineEdit->setReadOnly(true);
  ui_.gPSHealthLineEdit->setReadOnly(true);

  ui_.velocityXLineEdit->setReadOnly(true);
  ui_.velocityYLineEdit->setReadOnly(true);
  ui_.velocityZLineEdit->setReadOnly(true);

  ui_.localXLineEdit->setReadOnly(true);
  ui_.localYLineEdit->setReadOnly(true);
  ui_.localZLineEdit->setReadOnly(true);

  ui_.batteryProgressBar->setMinimum(0);
  ui_.batteryProgressBar->setMaximum(100);
  ui_.batteryProgressBar->setTextVisible(true);
  ui_.batteryProgressBar->setValue(drone_->power_status.percentage);  ui_.batteryProgressBar->setValue(drone_->power_status.percentage);  ui_.batteryProgressBar->show();
  ui_.batteryProgressBar->show();

  connect(ui_.take_off_button_, SIGNAL (clicked()),this, SLOT (TakeOff()));
  connect(ui_.request_control_button_, SIGNAL (clicked()),this, SLOT (RequestControl()));
  connect(ui_.land_button_, SIGNAL (clicked()),this, SLOT (Land()));
  connect(this, SIGNAL (FlightStatusChanged()), this, SLOT(DisplayFlightStatus()));

  //spin_thread = new boost::thread(&TeamHKUOCS::SpinThread, (TeamHKUOCS*)this);
  ui_update_thread = new boost::thread(&TeamHKUOCS::UIUpdateThread, (TeamHKUOCS*)this);
}

void TeamHKUOCS::shutdownPlugin()
{
  ros::shutdown();
  spin_thread->join();
  ui_update_thread->join();
  // TODO unregister all publishers here
}

void TeamHKUOCS::saveSettings(qt_gui_cpp::Settings& plugin_settings, qt_gui_cpp::Settings& instance_settings) const
{
  // TODO save intrinsic configuration, usually using:
  // instance_settings.setValue(k, v)
}

void TeamHKUOCS::restoreSettings(const qt_gui_cpp::Settings& plugin_settings, const qt_gui_cpp::Settings& instance_settings)
{
  // TODO restore intrinsic configuration, usually using:
  // v = instance_settings.value(k)
}

void TeamHKUOCS::SpinThread()
{
  ros::spin();
}

void TeamHKUOCS::UIUpdateThread()
{
  bool status_change = false;
  while(ros::ok())
  {
    if (status_change)
    {
      emit FlightStatusChanged();
    }
    status_change = false;
    if (ui_.accelerationXLineEdit->text() != (QString::number(drone_->acceleration.ax)))
    {
      status_change = true;
      continue;
    }
    if (ui_.accelerationYLineEdit->text() != (QString::number(drone_->acceleration.ay)))
    {
      status_change = true;
      continue;
    }
    if (ui_.accelerationZLineEdit->text() != (QString::number(drone_->acceleration.az)))
    {
      status_change = true;
      continue;
    }
    if (ui_.gPSLatitudeLineEdit->text() != (QString::number(drone_->global_position.latitude)))
    {
      status_change = true;
      continue;
    }
    if (ui_.gPSLongitudeLineEdit->text() != (QString::number(drone_->global_position.longitude)))
    {
      status_change = true;
      continue;
    }
    if (ui_.gPSAltitudeLineEdit->text() != (QString::number(drone_->global_position.altitude)))
    {
      status_change = true;
      continue;
    }
    if (ui_.gPSHeightLineEdit->text() != (QString::number(drone_->global_position.height)))
    {
      status_change = true;
      continue;
    }
    if (ui_.gPSHealthLineEdit->text() != (QString::number(drone_->global_position.health)))
    {
      status_change = true;
      continue;
    }
    if (ui_.velocityXLineEdit->text() != (QString::number(drone_->velocity.vx)))
    {
      status_change = true;
      continue;
    }
    if (ui_.velocityYLineEdit->text() != (QString::number(drone_->velocity.vy)))
    {
      status_change = true;
      continue;
    }
    if (ui_.velocityZLineEdit->text() != (QString::number(drone_->velocity.vz)))
    {
      status_change = true;
      continue;
    }
    if (ui_.localXLineEdit->text() != (QString::number(drone_->local_position.x)))
    {
      status_change = true;
      continue;
    }
    if (ui_.localYLineEdit->text() != (QString::number(drone_->local_position.y)))
    {
      status_change = true;
      continue;
    }
    if (ui_.localZLineEdit->text() != (QString::number(drone_->local_position.z)))
    {
      status_change = true;
      continue;
    }
    if (ui_.batteryProgressBar->value() != (drone_->power_status.percentage))
    {
      status_change = true;
      continue;
    }
    if (ui_.flightStatusLineEdit->text() != (QString::fromStdString(flight_status_arr_[drone_->flight_status])))
    {
      status_change = true;
      continue;
    }
    if (ui_.controlModeLineEdit->text() != (QString::fromStdString(control_status_arr_[drone_->flight_control_info.control_mode])))
    {
      status_change = true;
      continue;
    }

  }
  
}



void TeamHKUOCS::DisplayFlightStatus()
{
  ui_.accelerationXLineEdit->setText(QString::number(drone_->acceleration.ax));
  ui_.accelerationYLineEdit->setText(QString::number(drone_->acceleration.ay));
  ui_.accelerationZLineEdit->setText(QString::number(drone_->acceleration.az));
  ui_.gPSLatitudeLineEdit->setText(QString::number(drone_->global_position.latitude));
  ui_.gPSLongitudeLineEdit->setText(QString::number(drone_->global_position.longitude));
  ui_.gPSAltitudeLineEdit->setText(QString::number(drone_->global_position.altitude));
  ui_.gPSHeightLineEdit->setText(QString::number(drone_->global_position.height));
  ui_.gPSHealthLineEdit->setText(QString::number(drone_->global_position.health));

  ui_.velocityXLineEdit->setText(QString::number(drone_->velocity.vx));
  ui_.velocityYLineEdit->setText(QString::number(drone_->velocity.vy));
  ui_.velocityZLineEdit->setText(QString::number(drone_->velocity.vz));

  ui_.localXLineEdit->setText(QString::number(drone_->local_position.x));
  ui_.localYLineEdit->setText(QString::number(drone_->local_position.y));
  ui_.localZLineEdit->setText(QString::number(drone_->local_position.z));

  ui_.batteryProgressBar->setValue(drone_->power_status.percentage);

  ui_.flightStatusLineEdit->setText(QString::fromStdString(flight_status_arr_[drone_->flight_status]));

  ui_.controlModeLineEdit->setText(QString::fromStdString(control_status_arr_[drone_->flight_control_info.control_mode]));
  // std::cout << drone_->flight_control_info.control_mode << std::endl;
  //TODO: set a flight status array
  // std::String flight_status_str;
  // switch(drone_-> flight_status)
  // {
  //   case 1: 
  //     flight_status_str = "Ground Standby";
  //     break;
  // }
  // ui_.flightStatusLineEdit->setText(QString::number(drone_->acceleration.az));
}

void TeamHKUOCS::RequestControl()
{
  drone_->request_sdk_permission_control();
}

void TeamHKUOCS::TakeOff()
{
  drone_->takeoff();
}
void TeamHKUOCS::Land()
{
  drone_->landing();
}
/*bool hasConfiguration() const
{
  return true;
}

void triggerConfiguration()
{
  // Usually used to open a dialog to offer the user a set of configuration
}*/



} // namespace
PLUGINLIB_DECLARE_CLASS(teamhku, TeamHKUOCS, teamhku::TeamHKUOCS, rqt_gui_cpp::Plugin)
