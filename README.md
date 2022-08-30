# STM32-RC-Car
An RC car with control options of touch screen UI and bluetooth using phone app. The car also contains a sensor on the front to prevent crashses and notify the driver. It is driven with 4 wheel drive and turns by running each side at different speeds.

## Click Image For Video
[![Phone and rc car](https://i.imgur.com/Le5kn6v.jpg)](https://drive.google.com/file/d/1S7qBVHiXvQqwKeX7BY0ZH7Ovznsy3utL/view?usp=sharing "RC Car Video")

## Build Info
STM32F746NG MCU on the 32F746GDISCOVERY board with LCD and capacitive touch screen. Driving the motors is an L298N H-Bridge with each side connected to the same output, HC-06 Bluetooth Module communicating over UART and infrared avoid sensor detecting obstacles.
<p align="center">
<img src="https://i.imgur.com/8wmnQek.jpg" height="400"/>
</p>
