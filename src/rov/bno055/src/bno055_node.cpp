#include <chrono>

#include <rclcpp/rclcpp.hpp>

#include "rov_interfaces/msg/bno055_data.hpp"

#include "bno055/CSMBNO055.hpp"

using namespace std::chrono_literals;

// create cursed functions only available in this translation unit
namespace {
    rov_interfaces::msg::VectorD toVectorDMSG(const csmutil::Vector3d vector) {
        auto toret = rov_interfaces::msg::VectorD();
        toret.i = vector.getI();
        toret.j = vector.getJ();
        toret.k = vector.getK();
        return toret;
    }
    rov_interfaces::msg::QuaternionD toQuatDMSG(const csmutil::Quaterniond quat) {
        auto toret = rov_interfaces::msg::QuaternionD();
        toret.w = quat.getW();
        toret.i = quat.getI();
        toret.j = quat.getJ();
        toret.k = quat.getK();
        return toret;
    }
}

// its a bno055 using i2c, its a ros2 node :o
class BNO055_Node : public rclcpp::Node {
public:
    BNO055_Node() : Node("bno055_node"), bno(-1, BNO055_ADDRESS_A) {
        this->bno_publisher = this->create_publisher<rov_interfaces::msg::BNO055Data>("bno055_data", 10);

        this->create_wall_timer(101ms, std::bind(&BNO055_Node::bno_callback, this));
    }

private:
    // Create and send new BNO055 Data msg
    void bno_callback() {
        auto msg = rov_interfaces::msg::BNO055Data();
        msg.accelerometer = toVectorDMSG(this->bno.getVector(Vector_Type::ACCELEROMETER));
        msg.euler = toVectorDMSG(this->bno.getVector(Vector_Type::EULER));
        msg.gravity = toVectorDMSG(this->bno.getVector(Vector_Type::GRAVITY));
        msg.gyroscope = toVectorDMSG(this->bno.getVector(Vector_Type::GYROSCOPE));
        msg.linearaccel = toVectorDMSG(this->bno.getVector(Vector_Type::LINEARACCEL));
        msg.magnetometer = toVectorDMSG(this->bno.getVector(Vector_Type::MAGNETOMETER));
        msg.orientation = toQuatDMSG(this->bno.getQuat());
        msg.temp = this->bno.getTemp();
        this->bno_publisher->publish(msg);
    }

    BNO055 bno;
    std::shared_ptr<rclcpp::Publisher<rov_interfaces::msg::BNO055Data>> bno_publisher;
};

int main(int argc, char ** argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<BNO055_Node>());
    rclcpp::shutdown();
    return 0;
}