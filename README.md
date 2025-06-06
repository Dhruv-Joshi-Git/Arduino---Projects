# Arduino---Projects
Personal Arduino Uno 3 Projects 

### Arduino Uno Smart Car Build : June 2025 

**Description:**  
This project implements the code for an IR-controlled robotic vehicle capable of multiple modes of operation, such as line tracking, obstacle avoidance, hand guided navigation, and remote-controlled movement. The vehicle uses various sensors and actuators to achieve autonomous and manual control.

**Components Used:**
- Arduino Uno R3
- Motor Driver Module (L9110 )
- IR Receiver (compatible with the IRremote library)
- IR Remote Control (
- Ultrasonic Sensor (HC-SR04)
- Line Tracking Module
- IR Obstacle Sensors (2 sensors for left and right detection)
- DC Motors (2 or 4, depending on your vehicle design
- Universal Wheel 
- Power Source (9v battery)
- Breadboard
- Jumper Wires
- Chassis (for assembling the vehicle)

**Features:**
... Finishing soon

### Temperature and Humidity Display with Unit Toggle & Graphing : May 2025 

**Components Used:**
- Arduino Uno R3  
- DHT11 Temperature & Humidity Sensor  
- LCD Display with I2C Module (16x2)  
- Pushbutton  
- Breadboard  
- Jumper Wires  

**Description:**  
This project reads real-time temperature and humidity data using the DHT11 sensor and displays it on a 16x2 LCD via I2C. A single pushbutton allows the user to toggle between Celsius and Fahrenheit readings and a Bar graph. The display also shows system uptime in seconds. Required code above!
![Image](images/IMG_8676.JPG)



**Features:**
- Real-time measurement of temperature and humidity  
- Toggle between °C and °F using a single button  
- Multiple display modes (data view, graph)  
- Simple bar-graph visualization on LCD  
- Uptime display (in seconds)  
- Reliable button handling with software debouncing  

**Skills/Concepts Demonstrated:**
- Interfacing sensors (DHT11)  
- LCD control via I2C  
- Digital input with internal pull-up  
- Debouncing and button state management  
- Mode toggling with limited inputs  
- Data visualization with character-based LCDs  
