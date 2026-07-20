// action 服务器示例：接收目标角度，模拟转动过程，并提供反馈和结果
// 官方案例地址: https://docs.ros.org/en/humble/Tutorials/Intermediate/Writing-an-Action-Server-Client/Cpp.html

#include <memory>
#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <robot_interfaces/action/turn_angle.hpp>
#include <thread>

using TurnAngle = robot_interfaces::action::TurnAngle;
using GoalHandleTurnAngle = rclcpp_action::ServerGoalHandle<TurnAngle>;
namespace
{

class TurnAngleServer : public rclcpp::Node
{
 public:
  TurnAngleServer() : Node("turn_angle_server")
  {
    server_ = rclcpp_action::create_server<TurnAngle>(
      this, "turn_angle",

      std::bind(&TurnAngleServer::handle_goal, this, std::placeholders::_1, std::placeholders::_2),

      std::bind(&TurnAngleServer::handle_cancel, this, std::placeholders::_1),

      std::bind(&TurnAngleServer::handle_accepted, this, std::placeholders::_1));
  }

 private:
  rclcpp_action::Server<TurnAngle>::SharedPtr server_;

  // ========== 1. 处理 Goal ==========
  rclcpp_action::GoalResponse handle_goal(const rclcpp_action::GoalUUID &,
                                          const std::shared_ptr<const TurnAngle::Goal> &goal)
  {
    RCLCPP_INFO(get_logger(), "Receive Goal: %.1f", goal->target_angle);
    return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
  }

  // ========== 2. 处理 Cancel ==========
  rclcpp_action::CancelResponse handle_cancel(const std::shared_ptr<GoalHandleTurnAngle> &)
  {
    RCLCPP_INFO(get_logger(), "Cancel Request Received");
    return rclcpp_action::CancelResponse::ACCEPT;
  }

  // ========== 3. 接受 Goal 后执行 ==========
  void handle_accepted(const std::shared_ptr<GoalHandleTurnAngle> &goal_handle)
  {
    RCLCPP_INFO(get_logger(), "Goal Accepted");

    std::thread([this, goal_handle]() { execute(goal_handle); }).detach();
  }

  // ========== 4. 真正执行逻辑 ==========
  void execute(const std::shared_ptr<GoalHandleTurnAngle> &goal_handle)
  {
    RCLCPP_INFO(get_logger(), "Start executing goal...");

    const auto goal = goal_handle->get_goal();
    double target = goal->target_angle;

    auto feedback = std::make_shared<TurnAngle::Feedback>();
    auto result = std::make_shared<TurnAngle::Result>();

    double current = 0.0;

    rclcpp::Rate rate(5);  // 5Hz

    while (rclcpp::ok() && current < target)
    {
      if (goal_handle->is_canceling())
      {
        result->success = false;
        goal_handle->canceled(result);
        RCLCPP_WARN(get_logger(), "Goal Canceled");
        return;
      }

      current += 10.0;
      feedback->current_angle = current;

      goal_handle->publish_feedback(feedback);

      RCLCPP_INFO(get_logger(), "Current angle: %.1f", current);

      rate.sleep();
    }

    result->success = true;
    goal_handle->succeed(result);

    RCLCPP_INFO(get_logger(), "Goal Succeeded");
  }
};

}  // namespace
int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);

  rclcpp::spin(std::make_shared<TurnAngleServer>());

  rclcpp::shutdown();

  return 0;
}