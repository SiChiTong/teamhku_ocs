#ifndef TEAMHKU_OCS_H
#define TEAMHKU_OCS_H

#include <rqt_gui_cpp/plugin.h>
#include <teamhku_ocs/ui_teamhku_ocs.h>
#include <QWidget>

#include <dji_sdk/dji_drone.h>
#include <actionlib/client/simple_action_client.h>
#include <actionlib/client/terminal_state.h>
#include <cstdlib>
#include <boost/thread.hpp>
#include <QStringList>
#include <QColor>
#include <QDir>
#include <QUrl>
#include <ros/ros.h>
#include <std_msgs/Bool.h>

#include <string>
namespace teamhku {
class TeamHKUOCS
  : public rqt_gui_cpp::Plugin
{
  Q_OBJECT
public:
  TeamHKUOCS();
  virtual void initPlugin(qt_gui_cpp::PluginContext& context);
  virtual void shutdownPlugin();
  virtual void saveSettings(qt_gui_cpp::Settings& plugin_settings, qt_gui_cpp::Settings& instance_settings) const;
  virtual void restoreSettings(const qt_gui_cpp::Settings& plugin_settings, const qt_gui_cpp::Settings& instance_settings);

public Q_SLOTS:
  void TakeOff();
  void RequestControl();
  void ReleaseControl();
  void Land();
  void E_Handler();
  void GoHome();
  void LocalNavigation();
  void GlobalNavigation();
  void CopyLocal();
  void CopyGlobal();
  void RosbagRecord();
  void RosbagRecordStop();
  void TakePicture();
  void StartVideo();
  void StopVideo();
  void MoveGimbal();
  void ResetGimbal();
  void StartMission();
  void PauseMission();
  void ResumeMission();
  void CancelMission();
  void SmartDemo();
  void StartGimbalTrack();
  void StopGimbalTrack();
  void StartPositionTrack();
  void StopPositionTrack();
  void Arm();
  void Disarm();
  void VelocityControlStart();
  void VelocityControlStop();


  // Comment in to signal that the plugin has a way to configure it
  // bool hasConfiguration() const;
  // void triggerConfiguration();
protected Q_SLOTS:
	void DisplayFlightStatus();
	void DisplayPowerStatus();
  void ChangeButton();


signals:
	void FlightStatusChanged();
  void UILogicChanged();

protected: 
	void SpinThread();
	void UIUpdateThread();
  void RecordThread();
  void VelocityControlThread();
	
private:
  Ui::MyPluginWidget ui_;
  QWidget* widget_;
  DJIDrone* drone_;
  DJI::onboardSDK::VirtualRC* VRC_;
  ros::NodeHandle nh_;
  boost::thread* spin_thread;
  boost::thread* ui_update_thread;
  boost::thread* ui_record_thread;
  boost::thread* velocity_control_thread;
  const std::string flight_status_arr_[6] = {"", "Ground Standby", "Taking Off", "Sky Standby", "Landing", "Finishing Landing"};
  const std::string control_status_arr_[3] = {"RC", "Mobile SDK", "Onboard SDK"};
  uint32_t channel_data[16] = {0};
  void ShowMessage(QString msg, QColor color= Qt::black);
  ros::Publisher gimbal_track_pub_;
  ros::Publisher position_track_pub_;
  std_msgs::Bool msg;
  bool velocity_control_on;

};
}  // namespace teamhku


#endif