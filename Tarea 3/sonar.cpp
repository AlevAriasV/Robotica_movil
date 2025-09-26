#include <chrono>
#include <memory>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <cstring>
#include <sstream>
#include <iostream>

#include "rclcpp/rclcpp.hpp"
#include "visualization_msgs/msg/marker.hpp"

using namespace std::chrono_literals;

class MarkerPublisher : public rclcpp::Node 
{
public:
  MarkerPublisher() : Node("marker_publisher"), count_(0) 
  {
    _publisher = this->create_publisher<visualization_msgs::msg::Marker>("marker_topic", 10);
    configurarPuerto();

    timer_ = this->create_wall_timer(1000ms, std::bind(&MarkerPublisher::publicarMarcador, this));
  }

  ~MarkerPublisher() {
    if (serial_port_ >= 0) {
      close(serial_port_);
    }
  }

private:
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<visualization_msgs::msg::Marker>::SharedPtr _publisher;
  size_t count_;
  int serial_port_ = -1;
  const char* dispositivo_ = "/dev/ttyACM2";
  int velocidad_ = B9600;

  void configurarPuerto() {
    serial_port_ = open(dispositivo_, O_RDWR | O_NOCTTY);
    if (serial_port_ < 0) {
      RCLCPP_ERROR(this->get_logger(), "No se pudo abrir el puerto serial");
      return;
    }

    struct termios tty;
    std::memset(&tty, 0, sizeof(tty));
    tcgetattr(serial_port_, &tty);
    cfsetospeed(&tty, velocidad_);
    cfsetispeed(&tty, velocidad_);
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_oflag &= ~OPOST;
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 10;
    tcsetattr(serial_port_, TCSANOW, &tty);

    RCLCPP_INFO(this->get_logger(), "Puerto serial configurado correctamente");
  }

  float leerPuerto() {
    if (serial_port_ < 0) return -1.0f;

    char read_buf[256];
    std::memset(&read_buf, '\0', sizeof(read_buf));
    int num_bytes = read(serial_port_, &read_buf, sizeof(read_buf) - 1);

    if (num_bytes > 0) {
      std::string dato(read_buf);
      std::stringstream ss(dato);
      float distancia;
      ss >> distancia;
      if (!ss.fail()) {
        return distancia;
      }
    }
    return -1.0f;
  }

  void publicarMarcador() {
    float distancia = leerPuerto();
    if (distancia < 0.0f) {
      RCLCPP_WARN(this->get_logger(), "Lectura inválida del sonar");
      return;
    }

    auto marker = visualization_msgs::msg::Marker();
    marker.header.frame_id = "base_link";
    marker.header.stamp = this->get_clock()->now();

    marker.id = 0;
    marker.type = visualization_msgs::msg::Marker::CUBE;
    marker.action = visualization_msgs::msg::Marker::ADD;

    marker.scale.x = 1.0;
    marker.scale.y = 1.0;
    marker.scale.z = 1.0;

    marker.color.r = 0.0f;
    marker.color.g = 1.0f;
    marker.color.b = 0.0f;
    marker.color.a = 1.0;

    marker.lifetime.sec = 3;

    marker.pose.position.x = distancia / 100.0f; // Convertir cm a metros
    marker.pose.position.y = 0.0;
    marker.pose.position.z = 0.0;

    marker.pose.orientation.x = 0.0;
    marker.pose.orientation.y = 0.0;
    marker.pose.orientation.z = 0.0;
    marker.pose.orientation.w = 1.0;

    _publisher->publish(marker);
  }
};

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MarkerPublisher>());
  rclcpp::shutdown();
  return 0;
}
