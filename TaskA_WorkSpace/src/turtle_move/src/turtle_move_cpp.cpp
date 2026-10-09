#include <rclcpp/rclcpp.hpp>
#include "geometry_msgs/msg/twist.hpp"
#include "turtlesim/msg/pose.hpp"
#include "turtlesim/srv/set_pen.hpp"
#include <thread>
#include <chrono>

using namespace std::chrono_literals;

class TurtleMove : public rclcpp::Node
{
private:
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
    // rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr subscription_;
    rclcpp::Client<turtlesim::srv::SetPen>::SharedPtr client_;

public:
    TurtleMove() : Node("turtle_move_node")
    {
        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);
        client_ = this->create_client<turtlesim::srv::SetPen>("/turtle1/set_pen");
        // subscription_ = this->create_subscription<turtlesim::msg::Pose>("/turtle1/pose",10,std::bind(&TurtleMove::on_pose_received,this,std::placeholders::_1));
    }

    void send_request(int *nums)
    {
        auto request = std::make_shared<turtlesim::srv::SetPen::Request>();
        request->r = nums[0];
        request->g = nums[1];
        request->b = nums[2];
        request->width = nums[3];
        request->off = nums[4];
        // RCLCPP_INFO(this->get_logger(),"请求发送");
        auto future = this->client_->async_send_request(request);
        rclcpp::spin_until_future_complete(this->get_node_base_interface(), future);
        // RCLCPP_INFO(this->get_logger(),"回应返回");
    }

    void draw_line()
    {
        auto msg = geometry_msgs::msg::Twist();
        for (int i = 0; i < 10; i++)
        {
            msg.linear.x = 0;
            publisher_->publish(msg);
            RCLCPP_INFO(this->get_logger(), "publish %d", i);
            std::this_thread::sleep_for(100ms);
        }
        msg.linear.x = 1.0;
        publisher_->publish(msg);
        // RCLCPP_INFO(this->get_logger(), "publish1");
        std::this_thread::sleep_for(1000ms);
        msg.linear.x = 0.0;
        publisher_->publish(msg);
        // RCLCPP_INFO(this->get_logger(), "publish0");
    }

    void wait_for_stability()
    {
        auto msg = geometry_msgs::msg::Twist();
        for (int i = 0; i < 10; i++)
        {
            msg.linear.x = 0;
            publisher_->publish(msg);
            // RCLCPP_INFO(this->get_logger(), "publish %d", i);
            std::this_thread::sleep_for(100ms);
        }
    }

    void move(int length)
    {
        auto msg = geometry_msgs::msg::Twist();
        if (length > 0)
        {
            msg.linear.x = 1.0 * length;
        }
        else if (length < 0)
        {
            length = -length;
            msg.linear.x = -1.0 * length;
        }
        else
        {
            RCLCPP_ERROR(this->get_logger(), "错误");
        }
        publisher_->publish(msg);
        std::this_thread::sleep_for(2000ms);
        msg.linear.x = 0.0;
        publisher_->publish(msg);
        // RCLCPP_INFO(this->get_logger(),"stop");
    }

    void turn(int length)
    {
        auto msg = geometry_msgs::msg::Twist();
        if (length > 0)
        {
            msg.angular.z = length * 90 / 180.0 * 3.1415926;
        }
        else if (length < 0)
        {
            msg.angular.z = 3.1415926 * length * 90 / 180.0;
            length = -length;
        }
        else
        {
            RCLCPP_ERROR(this->get_logger(), "错误");
        }
        publisher_->publish(msg);
        std::this_thread::sleep_for(2000ms);
        msg.angular.z = 0.0;
        publisher_->publish(msg);
        // RCLCPP_INFO(this->get_logger(),"stop");
    }

    void draw_5(int length, int num)
    {
        wait_for_stability();
        turn(2);
        RCLCPP_INFO(this->get_logger(), "Drawing digit 5,num %d,step %d", num, 1);
        move(length);
        turn(1);
        RCLCPP_INFO(this->get_logger(), "Drawing digit 5,num %d,step %d", num, 2);
        move(length);
        turn(1);
        RCLCPP_INFO(this->get_logger(), "Drawing digit 5,num %d,step %d", num, 3);
        move(length);
        turn(-1);
        RCLCPP_INFO(this->get_logger(), "Drawing digit 5,num %d,step %d", num, 4);
        move(length);
        turn(-1);
        RCLCPP_INFO(this->get_logger(), "Drawing digit 5,num %d,step %d", num, 5);
        move(length);
    }

    void draw_gumu(int length)
    {
        wait_for_stability();
        turn(-1);
        move(length * 2);
        turn(-1);
        move(length * 3);
    }
};

int main(int args, char **argv)
{
    int nums1[5] = {255, 0, 0, 2, 1};
    int nums2[5] = {255, 183, 179, 2, 0};
    rclcpp::init(args, argv);
    auto node = std::make_shared<TurtleMove>();
    node->draw_5(2, 1);
    node->send_request(nums1);
    node->draw_gumu(2);
    node->send_request(nums2);
    node->draw_5(2, 2);
    rclcpp::spin(node);
    rclcpp::shutdown();
}
