#include "teamhku_ocs/teamhku_ocs.h"
#include <pluginlib/class_list_macros.h>
#include <iostream>
#include <unistd.h>
#include <cmath>
#include <time.h>

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

  ui_.MsgWin->setReadOnly(true);

  ui_.batteryProgressBar->setMinimum(0);
  ui_.batteryProgressBar->setMaximum(100);
  ui_.batteryProgressBar->setTextVisible(true);
  ui_.batteryProgressBar->setValue(drone_->power_status.percentage);  
  ui_.batteryProgressBar->setValue(drone_->power_status.percentage);  
  ui_.batteryProgressBar->show();
  ui_.batteryProgressBar->show();

  // disable unuseful button
  ui_.release_control_button_->setEnabled(false);
  ui_.take_off_button_->setEnabled(false);
  ui_.land_button_->setEnabled(false);
  ui_.go_home_button_->setEnabled(false);
  ui_.move_gimbal_button_->setEnabled(false);
  ui_.local_navigation_button_->setEnabled(false);

  // set color for the emergency button
  ui_.estop_button_->setAutoFillBackground(true);
  ui_.estop_button_->setStyleSheet("border-image: url("+QUrl::fromLocalFile(QFileInfo("catkin_ws/src/teamhku_ocs/src/teamhku_ocs/large-red-circle.png").absoluteFilePath()).toString().remove(0,7)+") 15 15 15 15; border-radius: 45px;");
  ui_.estop_button_->show();

  connect(ui_.take_off_button_, SIGNAL (clicked()),this, SLOT (TakeOff()));
  connect(ui_.request_control_button_, SIGNAL (clicked()),this, SLOT (RequestControl()));
  connect(ui_.release_control_button_, SIGNAL (clicked()),this, SLOT (ReleaseControl()));
  connect(ui_.land_button_, SIGNAL (clicked()),this, SLOT (Land()));
  connect(ui_.estop_button_, SIGNAL (clicked()),this, SLOT (E_Handler()));
  connect(ui_.go_home_button_, SIGNAL (clicked()),this, SLOT (GoHome()));
  connect(ui_.local_navigation_button_, SIGNAL (clicked()), this, SLOT (LocalNavigation()));
  connect(ui_.global_navigation_button_, SIGNAL (clicked()), this, SLOT (GlobalNavigation()));
  connect(ui_.rosbag_button_, SIGNAL (clicked()), this, SLOT (RosbagRecord()));
  connect(ui_.rosstop_button_, SIGNAL (clicked()), this, SLOT (RosbagRecordStop()));
  connect(ui_.take_picture_button_, SIGNAL (clicked()), this, SLOT (TakePicture()));
  connect(ui_.start_video_button_, SIGNAL (clicked()), this, SLOT (StartVideo()));
  connect(ui_.stop_video_button_, SIGNAL (clicked()), this, SLOT (StopVideo()));
  connect(this, SIGNAL (FlightStatusChanged()), this, SLOT(DisplayFlightStatus()));
  connect(this, SIGNAL (UILogicChanged()), this, SLOT(ChangeButton()));


  //spin_thread = new boost::thread(&TeamHKUOCS::SpinThread, (TeamHKUOCS*)this);
  ui_update_thread = new boost::thread(&TeamHKUOCS::UIUpdateThread, (TeamHKUOCS*)this);
}

void TeamHKUOCS::shutdownPlugin()
{
  ros::shutdown();
  spin_thread->join();
  ui_update_thread->join();
  ui_record_thread->join();
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
  bool UI_change = false;
  QString prev_acc = QString::number(0);
  int prev_battery = 0, prev_control = -1, prev_status = 0;
  while(ros::ok())
  {
    status_change = false;
    UI_change = false;
    if (prev_acc != (QString::number(drone_->acceleration.ax)))
    {
      status_change = true;
      prev_acc = QString::number(drone_->acceleration.ax);
    }
    if (ui_.flightStatusLineEdit->text() != (QString::fromStdString(flight_status_arr_[drone_->flight_status])))
    {
      status_change = true;
    }
    if (prev_battery != drone_->power_status.percentage)
    {
      status_change = true;
      prev_battery = drone_->power_status.percentage;
    }

    if (prev_control != drone_->flight_control_info.cur_ctrl_dev_in_navi_mode)
    {
      UI_change = true;
      prev_control = drone_->flight_control_info.cur_ctrl_dev_in_navi_mode;
    }
    if (prev_status != drone_->flight_status)
    {
      UI_change = true;
      prev_status = drone_->flight_status;
    }

    if (status_change)
    {
      emit FlightStatusChanged();
    }
    if (UI_change)
    {
      emit UILogicChanged();
    }
    sleep(0.01);
  }
  
}


void TeamHKUOCS::DisplayFlightStatus()
{
  ui_.accelerationXLineEdit->setText(QString::number(round(drone_->acceleration.ax*10)/10));
  ui_.accelerationYLineEdit->setText(QString::number(round(drone_->acceleration.ay*10)/10));
  ui_.accelerationZLineEdit->setText(QString::number(round(drone_->acceleration.az*10)/10));
  ui_.gPSLatitudeLineEdit->setText(QString::number(drone_->global_position.latitude));
  ui_.gPSLongitudeLineEdit->setText(QString::number(drone_->global_position.longitude));
  ui_.gPSAltitudeLineEdit->setText(QString::number(drone_->global_position.altitude));
  ui_.gPSHeightLineEdit->setText(QString::number(drone_->global_position.height));
  ui_.gPSHealthLineEdit->setText(QString::number(drone_->global_position.health));

  ui_.velocityXLineEdit->setText(QString::number(round(drone_->velocity.vx*10)/10));
  ui_.velocityYLineEdit->setText(QString::number(round(drone_->velocity.vy*10)/10));
  ui_.velocityZLineEdit->setText(QString::number(round(drone_->velocity.vz*10)/10));

  ui_.localXLineEdit->setText(QString::number(round(drone_->local_position.x*100)/100));
  ui_.localYLineEdit->setText(QString::number(round(drone_->local_position.y*100)/100));
  ui_.localZLineEdit->setText(QString::number(round(drone_->local_position.z*100)/100));

  ui_.batteryProgressBar->setValue(drone_->power_status.percentage);

  ui_.flightStatusLineEdit->setText(QString::fromStdString(flight_status_arr_[drone_->flight_status]));

  
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

void TeamHKUOCS::ChangeButton()
{
  if (drone_->flight_control_info.cur_ctrl_dev_in_navi_mode == 2)
  {
    ui_.release_control_button_->setEnabled(true);
    ui_.request_control_button_->setEnabled(false);
  }
  else
  {
    ui_.release_control_button_->setEnabled(false);
    ui_.request_control_button_->setEnabled(true);
  }

  if (ui_.release_control_button_->isEnabled())
  {
    if (drone_->flight_status == 1)
    {
      ui_.take_off_button_->setEnabled(true);
      ui_.land_button_->setEnabled(false);
      ui_.go_home_button_->setEnabled(false);
      ui_.move_gimbal_button_->setEnabled(false);
      ui_.local_navigation_button_->setEnabled(false);
      ui_.global_navigation_button_->setEnabled(false);
    }
    else if (drone_->flight_status == 3)
    {
      ui_.take_off_button_->setEnabled(false);
      ui_.land_button_->setEnabled(true);
      ui_.go_home_button_->setEnabled(true);
      ui_.move_gimbal_button_->setEnabled(true);
      ui_.local_navigation_button_->setEnabled(true);
      ui_.global_navigation_button_->setEnabled(true);
    }
    else
    {
      ui_.take_off_button_->setEnabled(false);
      ui_.land_button_->setEnabled(false);
      ui_.go_home_button_->setEnabled(false);
      ui_.move_gimbal_button_->setEnabled(false);
      ui_.local_navigation_button_->setEnabled(false);
      ui_.global_navigation_button_->setEnabled(false);
    }
  }
  else
  {
    ui_.take_off_button_->setEnabled(false);
    ui_.land_button_->setEnabled(false);
    ui_.go_home_button_->setEnabled(false);
    ui_.move_gimbal_button_->setEnabled(false);
    ui_.local_navigation_button_->setEnabled(false);
    ui_.global_navigation_button_->setEnabled(false);
  }

  ui_.controlModeLineEdit->setText(QString::fromStdString(control_status_arr_[drone_->flight_control_info.cur_ctrl_dev_in_navi_mode]));
}

void TeamHKUOCS::RequestControl()
{
  drone_->request_sdk_permission_control();
}

void TeamHKUOCS::ReleaseControl()
{
  drone_->release_sdk_permission_control();
}

void TeamHKUOCS::TakeOff()
{
  drone_->takeoff();
}

void TeamHKUOCS::Land()
{
  drone_->landing();
}

void TeamHKUOCS::E_Handler()
{
  drone_->request_sdk_permission_control();
  drone_->local_position_navigation_cancel_all_goals();
  drone_->global_position_navigation_cancel_all_goals();
  drone_->waypoint_navigation_cancel_all_goals();
  drone_->mission_cancel();
  drone_->velocity_control(0, 0, 0, 0, 0);
  drone_->release_sdk_permission_control();
  
}

void TeamHKUOCS::GoHome()
{
  drone_->gohome();
}

void TeamHKUOCS::LocalNavigation()
{
  drone_->local_position_navigation_send_request(ui_.localXLineEdit_2->text().toDouble(), ui_.localYLineEdit_2->text().toDouble(), ui_.localZLineEdit_2->text().toDouble());
}

void TeamHKUOCS::GlobalNavigation()
{
  drone_->global_position_navigation_send_request(ui_.globalXLineEdit_->text().toDouble(), ui_.globalYLineEdit_->text().toDouble(), ui_.globalZLineEdit_->text().toDouble());
}


void TeamHKUOCS::RecordThread()
{
  std::string date;
  std::time_t nowtime;
  struct tm *local;
  nowtime = time(NULL);
  local=localtime(&nowtime);
  date = "rosbag record -a -O ~/bagfiles/" + std::to_string(local->tm_year+1900) + "-" + (local->tm_mon < 9 ? "0"+std::to_string(local->tm_mon+1) : std::to_string(local->tm_mon+1)) + "-" + (local->tm_mday < 10 ? "0"+std::to_string(local->tm_mday) : std::to_string(local->tm_mday)) + "-" + (local->tm_hour < 10 ? "0"+std::to_string(local->tm_hour) : std::to_string(local->tm_hour)) + "-" + (local->tm_min < 10 ? "0"+std::to_string(local->tm_min) : std::to_string(local->tm_min)) + "-" + (local->tm_sec < 10 ? "0"+std::to_string(local->tm_sec) : std::to_string(local->tm_sec)) + ".bag";
  std::cout << date;
  system(date.c_str());
}


void TeamHKUOCS::RosbagRecord()
{
  ui_record_thread = new boost::thread(&TeamHKUOCS::RecordThread, (TeamHKUOCS*)this);
}

void TeamHKUOCS::RosbagRecordStop()
{
  system("rosnode kill `rosnode list | grep '^/record'`");
}

void TeamHKUOCS::TakePicture()
{
  drone_->take_picture();
}

void TeamHKUOCS::StartVideo()
{
  drone_->start_video();
}

void TeamHKUOCS::StopVideo()
{
  drone_->stop_video();
}

void TeamHKUOCS::ShowMessage(QString msg, QColor color)
{
  ui_.MsgWin->moveCursor(QTextCursor::Start, QTextCursor::MoveAnchor);
  ui_.MsgWin->setTextColor(color);
  ui_.MsgWin->append(msg);

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
