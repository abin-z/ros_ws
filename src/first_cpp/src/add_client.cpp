#include <chrono>
#include <example_interfaces/srv/add_two_ints.hpp>
#include <memory>
#include <rclcpp/rclcpp.hpp>

using namespace std::chrono_literals;

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);

  auto node = std::make_shared<rclcpp::Node>("add_client");

  auto client = node->create_client<example_interfaces::srv::AddTwoInts>("add_two_ints");

  // 等待服务上线
  while (!client->wait_for_service(1s))
  {
    RCLCPP_INFO(node->get_logger(), "Waiting for add_two_ints service...");
  }

  auto request = std::make_shared<example_interfaces::srv::AddTwoInts::Request>();

  request->a = 15;
  request->b = 20;

  RCLCPP_INFO(node->get_logger(), "Sending request: %ld + %ld", request->a, request->b);

  auto future = client->async_send_request(request);

  // 等待结果
  auto result = rclcpp::spin_until_future_complete(node, future);

  if (result == rclcpp::FutureReturnCode::SUCCESS)
  {
    RCLCPP_INFO(node->get_logger(), "Response: %ld", future.get()->sum);
  }
  else
  {
    RCLCPP_ERROR(node->get_logger(), "Failed to call service");
  }

  rclcpp::shutdown();

  return 0;
}