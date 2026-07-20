#include <memory>
#include <rclcpp/rclcpp.hpp>

// 自定义节点。
// ROS2 中的绝大多数功能（Topic、Service、Action 等）都运行在 Node 上。
class FirstNode : public rclcpp::Node
{
 public:
  // 调用基类构造函数，并指定节点名称为 "first_node"
  FirstNode() : Node("first_node")
  {
    // ROS2 日志输出接口。
    // get_logger() 返回当前节点的日志器(Logger)。
    RCLCPP_INFO(this->get_logger(), "Hello ROS2 from C++! 我的第一个节点");
  }
};

int main(int argc, char **argv)
{
  // 初始化 ROS2 运行时环境。
  // 必须在创建任何 ROS2 对象之前调用。
  rclcpp::init(argc, argv);

  // 创建节点对象。
  auto node = std::make_shared<FirstNode>();

  // 启动事件循环(Event Loop)。
  //
  // spin() 会持续处理 ROS2 事件，例如：
  //   - Topic 消息到达
  //   - Service 请求到达
  //   - Action 反馈
  //   - Timer 超时
  //
  // 当前示例没有任何事件，因此程序会一直阻塞在这里等待。
  rclcpp::spin(node);

  // 关闭 ROS2 运行时。
  // 通常在程序退出前调用。
  rclcpp::shutdown();

  return 0;
}