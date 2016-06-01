#include "teamhku_ocs/teamhku_ocs.h"
#include <pluginlib/class_list_macros.h>
#include <QStringList>
#include <iostream>
#include <unistd.h>

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

  pre_ax_ = 0;
  connect(ui_.take_off_button_, SIGNAL (clicked()),this, SLOT (TakeOff()));
  connect(ui_.request_control_button_, SIGNAL (clicked()),this, SLOT (RequestControl()));

  connect(this, SIGNAL (FlightStatusChanged()), this, SLOT(DisplayFlightStatus()));



  drone_ = new DJIDrone(nh_);
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
  while(ros::ok())
  {
    emit FlightStatusChanged();

  }
  //std::cout << "hello world" << std::endl;
  // double acc_x  = drone_->acceleration.ax;
  // std::cout << drone_->acceleration.ax << std::endl;
  // ui_.accelerationXLineEdit->insert(QString::number(acc_x));
  // while(ros::ok())
  // {
  //   std::cout << "1" << std::endl;
  // }
  // double acc_x = 0; 
  // while(1)
  // {
  //   ui_.accelerationXLineEdit->setText(QString::number(0));
  //   acc_x = acc_x + 1;
  //   sleep(1);
  // }
  
  // std::cout << drone_->acceleration.ax << std::endl;
  // ui_.accelerationXLineEdit->insert(QString::number(acc_x));
  // while(ros::ok())
  // {
  //   //acc_x = drone_->acceleration.ax;
  //   acc_x = acc_x + 1;
  //   ui_.accelerationXLineEdit->setText(QString::number(acc_x));
  //   //std::cout << acc_x << std::endl;
  // }
  
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
