#include "teamhku_ocs/teamhku_ocs.h"
#include <pluginlib/class_list_macros.h>
#include <QStringList>

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
  connect(ui_.take_off_button_, SIGNAL (clicked()),this, SLOT (takeoff()));
  connect(ui_.request_control_button_, SIGNAL (clicked()),this, SLOT (request_control()));

  drone_ = new DJIDrone(nh_);
}

void TeamHKUOCS::shutdownPlugin()
{
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

void TeamHKUOCS::request_control()
{
  drone_->request_sdk_permission_control();
}

void TeamHKUOCS::takeoff()
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
