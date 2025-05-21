# Arduino---Projects
Personal Arduino Uno 3 Projects 

### Temperature and Humidity Display with Unit Toggle & Graphing

**Components Used:**
- Arduino Uno R3  
- DHT11 Temperature & Humidity Sensor  
- LCD Display with I2C Module (16x2)  
- Pushbutton  
- Breadboard  
- Jumper Wires  

**Description:**  
This project reads real-time temperature and humidity data using the DHT11 sensor and displays it on a 16x2 LCD via I2C. A single pushbutton allows the user to toggle between Celsius and Fahrenheit readings and a Bar graph. The display also shows system uptime in seconds. Required code above!
![Image](images/IMG_8675.jpg)



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
