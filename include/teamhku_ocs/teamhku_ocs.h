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
  // Comment in to signal that the plugin has a way to configure it
  // bool hasConfiguration() const;
  // void triggerConfiguration();
protected Q_SLOTS:
	void DisplayFlightStatus();

signals:
	void FlightStatusChanged();

protected: 
	void SpinThread();
	void UIUpdateThread();

private:
  Ui::MyPluginWidget ui_;
  QWidget* widget_;
  DJIDrone* drone_;
  ros::NodeHandle nh_;
  boost::thread* spin_thread;
  boost::thread* ui_update_thread;
  double pre_ax_;
};
}  // namespace teamhku


#endif