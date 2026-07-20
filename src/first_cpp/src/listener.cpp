#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

// using std::placeholders::_1;

class Listener : public rclcpp::Node
{
 public:
  Listener() : Node("cpp_listener")
  {
    // sub_ = this->create_subscription<std_msgs::msg::String>("chatter", 10, std::bind(&Listener::callback, this, _1));
    // 使用 lambda 替换 std::bind, 但回调函数还是独立的
    sub_ = this->create_subscription<std_msgs::msg::String>("chatter", 10,
                                                            [this](const std_msgs::msg::String::SharedPtr& msg) {
                                                              this->callback(msg);  // lambda 里调用成员函数
                                                            });
  }

 private:
  void callback(const std_msgs::msg::String::SharedPtr &msg)
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