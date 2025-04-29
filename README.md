# ESP32 Button Server

A simple ESP32 project that starts a web server when a physical button is pressed. The project includes a web portal that can be hosted on GitHub Pages or Netlify for remote monitoring and control.

## Features

- Start a web server with a physical button press
- Modern web interface for remote monitoring
- Real-time status updates using Server-Sent Events (SSE)
- LED status indicators
- Simple API endpoints for status monitoring

## Hardware Requirements

- ESP32 development board (e.g., ESP32 DevKit, NodeMCU-32S)
- A push button
- Resistor (10kΩ) - optional, as we use the internal pull-up resistor
- Breadboard and jumper wires

## Wiring

1. Connect one side of the button to GPIO4
2. Connect the other side of the button to GND
3. The built-in LED is connected to GPIO2 (no external wiring needed)

## Software Setup

### 1. PlatformIO Setup

This project is designed to be built with PlatformIO. Make sure you have PlatformIO installed in your IDE.

### 2. WiFi Configuration

Edit the `src/wifi_credentials.h` file with your WiFi credentials:

```cpp
const char* ssid = "YourWiFiSSID";
const char* password = "YourWiFiPassword";
```

### 3. Deploy the Web Portal

#### GitHub Pages Deployment

1. Create a new repository on GitHub
2. Upload the contents of the `web-portal` directory to your repository
3. Go to repository Settings > Pages
4. Set the source to your main branch and the folder to `/` (root)
5. Your web portal will be available at `https://your-username.github.io/repository-name/`

#### Netlify Deployment

1. Create a Netlify account if you don't have one
2. Create a new site from the Netlify dashboard
3. Drag and drop the `web-portal` directory to the upload area, or connect to your GitHub repository
4. Your web portal will be deployed to a Netlify URL (e.g., `https://your-site-name.netlify.app`)

## Usage

### Local Usage

1. Press the physical button connected to GPIO4
2. The server will start and the LED will blink 3 times
3. Access the web server at the ESP32's IP address shown in the serial monitor

### Remote Monitoring with the Web Portal

1. Open your deployed web portal (GitHub Pages or Netlify)
2. Enter the IP address of your ESP32 (must be on a network accessible to your device)
3. Click "Connect" to establish a connection
4. View real-time status updates and events from your ESP32

## API Endpoints

- `/` - Home page showing server status
- `/api/status` - JSON endpoint returning server status and uptime
- `/api/ip` - Returns the current IP address of the ESP32
- `/events` - Server-Sent Events endpoint for real-time updates

## Making Your ESP32 Accessible from the Internet

For remote access outside your local network, you have several options:

1. **Port Forwarding** (not recommended): Configure your router to forward a port to your ESP32
2. **VPN**: Set up a VPN server on your network and connect to it when away
3. **Cloud IoT Platform**: Use a service like AWS IoT, Azure IoT, or Google Cloud IoT
4. **Reverse Proxy Service**: Use a service like ngrok or Serveo to create a tunnel

## Customization

- Change `BUTTON_PIN` and `LED_PIN` in `main.cpp` to match your hardware setup
- Modify the web server endpoints in the `setupServer()` function to add more functionality
- Customize the web portal appearance by editing the CSS file

## Troubleshooting

- If the LED doesn't blink 2 times during startup, check your WiFi credentials
- If the button doesn't work, verify the wiring and pin configuration
- Check the serial monitor for debugging information at 115200 baud
- If the web portal can't connect to the ESP32, ensure both devices are on the same network
- For CORS issues, make sure the ESP32 is correctly sending CORS headers 