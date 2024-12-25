# Midterm_Project_Auto_Pump_Control
Hệ thống điều khiển máy bơm mini tự động 

# Đặt vấn đề
Việc giám sát và điều khiển độ ẩm đất đóng vai trò quan trọng trong nông nghiệp, giúp cây trồng phát triển tốt hơn và tiết kiệm tài nguyên nước. Tuy nhiên, các giải pháp truyền thống thường yêu cầu nhiều công sức và thời gian theo dõi thủ công. Vì vậy, cần một hệ thống tự động và thông minh để giải quyết vấn đề này một cách hiệu quả.

# Mục tiêu
* Xây dựng một hệ thống tự động giám sát và điều khiển độ ẩm đất sử dụng vi điều khiển ESP32.
* Thu thập và gửi dữ liệu độ ẩm đất qua giao thức MQTT để người dùng có thể theo dõi từ xa.
* Điều khiển module relay để bật/tắt hệ thống tưới tiêu tự động dựa trên ngưỡng độ ẩm được cấu hình trước.
* Bật tắt relay thông qua dashboard.
  
# Linh kiện sử dụng
* ESP32
* Cảm biến độ ẩm đất
* Relay 5V
* Máy bơm mini

# Phần mềm và giao thức sử dụng
* PlatformIO/Arduino để viết mã.
* Giao thức MQTT để trao đổi dữ liệu qua broker online.
* Node-RED, Dashboard để theo dõi từ xa và điều khiển động cơ mini.

# Sơ đồ khối của hệ thống
![sơ đồ khối (2)](https://github.com/user-attachments/assets/be43e172-1f26-4196-b851-d8e02e7523f3)

# NodeRed Flow
![NodeRed_Flow](https://github.com/user-attachments/assets/ae5a0940-2421-46ee-bc9a-ed498afd365f)

# NodeRed Dashboard
![Dash_Board](https://github.com/user-attachments/assets/41c1cd8f-fe7f-4309-ac0c-136385f4529a)

# Video Demo: Automatic Watering
https://github.com/user-attachments/assets/7c6da52c-57bc-4cbf-86b1-595a82b452cd

# Video Demo: Control Pump
https://github.com/user-attachments/assets/486ec3d1-0670-410c-a375-10d16074d479
