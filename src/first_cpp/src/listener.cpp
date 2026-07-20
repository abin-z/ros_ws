#include <memory>
#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>

class Listener : public rclcpp::Node
{
 public:
  Listener() : Node("cpp_listener")
  {
    sub_ = this->create_subscription<std_msgs::msg::String>(
      "chatter", 10, [this](const std_msgs::msg::String::ConstSharedPtr &msg) { this->callback(msg); });
  }

 private:
  void callback(const std_msgs::msg::String::ConstSharedPtr &msg)
  {
    RCLCPP_INFO(this->get_logger(), "C++ received: '%s'", msg->data.c_str());
  }

  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub_;
};

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<Listener>());
  rclcpp::shutdown();
  return 0;
}