![BusyBox Logo](BusyBox.svg)

# Availability Displays

***

## Description
BusyBox is a system for displaying availability of a person/room in a WFH (work from home) environment. This project differs from other solutions because this project is manually controlled and self-contained - instead of being triggered by calendar appointments or a scheduling system, it is set by a button press. Using a wireless mesh protocol, one server device can trigger several client viewers to display whether the person is free or busy.

## Features
* Low Cost
* Expandable (Multiple Client Viewers)
* Manually Controllable

***

## Different Versions Available

### Mesh Version

This is the standard implementation of BusyBox. This setup requires at least one server and at least one client to work.

*NOTE: The list of possible devices with prebuilt source code will expand once the new devices I ordered arrive. As for now, these devices were chosen since they were what I had on hand.

* ### Client
  * Hardware (One of any of the following for each client instance)
    * LilyGo T-Display
  * Clients (also referred to as viewers) are the device that displays the busy or free message. All the clients used in this project are ESP32 devices with built-in screens (for simplicity). These devices do not offer any control over the BusyBox network status - they are just used to view status.

* ### Server
  * Hardware (One of any of the following for each server instance)
    * ESP32 Dev Board
  * Servers are how the BusyBox network status is set; they also receive confirmation messages from each individual viewer on the network confirming they are displaying the correct status. Some devices have built-in screens and some do not. For the ones without a screen, any relevant messages relating to BusyBox's operation is delivered via serial output.

### Standalone Version
* Hardware
  * LilyGo T-Display
* This is the simplest version and only requires one device. The device does not use a wireless mesh to cycle between busy and free. Instead, it uses the 2 buttons onboard the dev board to switch between busy and free.
