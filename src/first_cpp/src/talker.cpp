#include <atomic>
#include <memory>
#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>

class Talker : public rclcpp::Node
{
 public:
  Talker() : Node("cpp_talker")
  {
    pub_ = this->create_publisher<std_msgs::msg::String>("chatter", 10);

    timer_ = this->create_wall_timer(std::chrono::seconds(1), std::bind(&Talker::timer_callback, this));
  }

 private:
  void timer_callback()
  {
    auto msg = std_msgs::msg::String();
    msg.data = "Hello from C++ publisher_" + std::to_string(count_++);

    RCLCPP_INFO(this->get_logger(), "Publishing: '%s'", msg.data.c_str());
    pub_->publish(msg);
  }

  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_;
  rclcpp::TimerBase::SharedPtr timer_;
  std::atomic_int count_ = 1;
};

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<Talker>());
  rclcpp::shutdown();
  return 0;
}