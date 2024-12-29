# SMART GREEN-HOUSE USING IOT TECHNOLOGY
Vườn thông minh

# Đặt vấn đề
Trong nông nghiệp hiện đại, nhà kính được xem là mô hình quan trọng giúp điều khiển môi trường trồng trọt như nhiệt độ, độ ẩm, ánh sáng, và lượng nước. Tuy nhiên, việc quản lý nhà kính truyền thống thường tốn nhiều thời gian và công sức. Do đó, đề tài này sẽ xây dựng một hệ thống nhà kính thông minh sử dụng công nghệ IoT nhằm đáp ứng các mục tiêu sau:
* Giám sát và phân tích các yếu tố môi trường trong nhà kính như là  nhiệt độ, độ ẩm, độ ẩm đất và cường độ ánh sáng.
* Tự động điều khiển thiết bị như quạt, máy bơm, và hệ thống đèn LED dựa trên ngưỡng được cài đặt.
* Kiểm soát thời gian và điều khiển thủ công các thiết bị theo yêu cầu người dùng.
* Tối ưu hóa các tài nguyên sử dụng và giao diện người dùng thông minh.

# Linh kiện sử dụng
ESP32 development board
Cảm biến:
* Cảm biến nhiệt độ và độ ẩm DHT11
* Cảm biến độ ẩm đất
* Cảm biến cường độ ánh sáng quang trở LM393

Thiết bị điều khiển:
* Máy bơm nước mini
* Quạt thông gió
* Đèn LED chiếu sáng 

Các linh kiện, thiết bị khác:
* Relay
* Trở 1K

# Phần mềm sử dụng
* Phần mềm PlatformIO/Arduino để viết mã.
* Phần mềm HiveMQ để xử lý và phân phối các thông điệp giữa các client sử dụng giao thức MQTT.
* Node-RED, Dashboard để theo dõi các yếu tố môi trường và điều khiển các thiết bị từ xa.

# Sơ đồ khối của hệ thống
![Sơ đồ khối](https://github.com/user-attachments/assets/5b97dbcc-2cfe-4c71-8c79-80cd02437e17)

# NodeRed Flow
![NodeRed_Flow](https://github.com/user-attachments/assets/147a03db-7649-4278-b3ae-995b93f706be)

# NodeRed Dashboard
![Dash_Board](https://github.com/user-attachments/assets/264885c9-dbba-4fca-9d3a-580291778ae9)

# Video Demo: Automatic Mode 
https://github.com/user-attachments/assets/cb775be7-99c1-46e7-8651-5dd842b575f6

# Video Demo: Manual Mode
https://github.com/user-attachments/assets/6a1af4f4-e0e5-44c1-b736-2ed9c98a2c2a

# Kết luận
Đề tài "Ứng dụng công nghệ IoT trong nhà kính" đã ứng dụng công nghệ IoT để hoàn thiện thành công một hệ thống giám sát và điều khiển các yếu tố môi trường trong nhà kính. Sau đây là những kết quả mà tôi đã làm được từ đề tài này:
* Sử dụng các cảm biến để thu thập dữ liệu về nhiệt độ, độ ẩm không khí, độ ẩm đất và cường độ ánh sáng trong không gian nhà kính 
* Sử dụng giao thức MQTT để giao tiếp giữa người dùng và thiết bị
* Xây dựng giao diện người dùng thông minh, tiện lợi bằng Node-red với các tính năng như theo dõi thông số môi trường, điều khiển tự động và thủ công các thiết bị, theo dõi thời gian hoạt động và điều chỉnh ngưỡng hoạt động của chúng. 

Trong tương lai, hệ thống có thể được cải tiến và mở rộng thêm các tính năng như:
* Điều khiển và giám sát lượng khí CO2 trong không khí.
* Giám sát nguồn dinh dưỡng trong môi trường đất.
* Theo dõi và phát hiện sâu bệnh cho cây trồng.
* Tích hợp các công nghệ AI để dự đoán các yếu tố môi trưòng.