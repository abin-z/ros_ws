// action 客户端示例：发送目标角度，接收反馈和结果
// 官方案例地址: https://docs.ros.org/en/humble/Tutorials/Intermediate/Writing-an-Action-Server-Client/Cpp.html

#include <memory>
#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <robot_interfaces/action/turn_angle.hpp>

using TurnAngle = robot_interfaces::action::TurnAngle;
using GoalHandleTurnAngle = rclcpp_action::ClientGoalHandle<TurnAngle>;

class TurnAngleClient : public rclcpp::Node
{
 public:
  TurnAngleClient() : Node("turn_angle_client")
  {
    client_ = rclcpp_action::create_client<TurnAngle>(this, "turn_angle");

    timer_ = this->create_wall_timer(std::chrono::seconds(2), std::bind(&TurnAngleClient::send_goal, this));
  }

 private:
  rclcpp_action::Client<TurnAngle>::SharedPtr client_;
  rclcpp::TimerBase::SharedPtr timer_;

  bool goal_sent_ = false;

  void send_goal()
  {
    RCLCPP_INFO(get_logger(), "Preparing to send goal...");
    if (goal_sent_) return;

    if (!client_->wait_for_action_server(std::chrono::seconds(5)))
    {
      RCLCPP_ERROR(get_logger(), "Action server not available");
      return;
    }

    auto goal_msg = TurnAngle::Goal();
    goal_msg.target_angle = 90.0;

    RCLCPP_INFO(get_logger(), "Sending goal: 90.0");

    rclcpp_action::Client<TurnAngle>::SendGoalOptions options;

    // 1. 反馈回调（执行过程中）
    options.feedback_callback = [this](const GoalHandleTurnAngle::SharedPtr &,
                                       const std::shared_ptr<const TurnAngle::Feedback> &feedback) {
      RCLCPP_INFO(get_logger(), "Feedback: current_angle = %.1f", feedback->current_angle);
    };

    // 2. 结果回调（最终结果）
    options.result_callback = [this](const GoalHandleTurnAngle::WrappedResult &result) {
      switch (result.code)
      {
      case rclcpp_action::ResultCode::SUCCEEDED:
        RCLCPP_INFO(get_logger(), "Goal succeeded!");
        RCLCPP_INFO(get_logger(), "Result success = %d", result.result->success);
        break;

      case rclcpp_action::ResultCode::ABORTED:
        RCLCPP_ERROR(get_logger(), "Goal aborted");
        break;

      case rclcpp_action::ResultCode::CANCELED:
        RCLCPP_WARN(get_logger(), "Goal canceled");
        break;

      default:
        RCLCPP_ERROR(get_logger(), "Unknown result code");
        break;
      }
    };

    // 3. 发送 goal
    client_->async_send_goal(goal_msg, options);

    goal_sent_ = true;
  }
};

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);

  rclcpp::spin(std::make_shared<TurnAngleClient>());

  rclcpp::shutdown();

  return 0;
}