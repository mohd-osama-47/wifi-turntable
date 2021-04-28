# WiFi-Turntable
<p align="center">
  <img src="/media/title.png/" alt="Thumbnail">
</p>
A NoceMCU12E Based wifi turntable with an interactive site!

## Introduction
A turntable that uses a NodeMCU as a webserver that hosts an interactive site, all to control a stepper motor that rotates a platform a certain amount of steps, pauses a bit, and then triggers a Bluetooth shutter that takes a picture of the object of interest. The resultant photos are then used for photogrammetry reasons!

## Table of Contents
- [WiFi-Turntable](#wifi-turntable)
  * [Introduction](#introduction)
  * [Table of Contents](#table-of-contents)
  * [Components Used](#components-used)
  * [How it works](#how-it-works)
  * [Circuit Schematic](#circuit-schematic)
  * [Physical Design](#physical-design)
  * [Full Demo](#full-demo)
  * [Acknowledgments](#acknowledgments)
  * [Future Work](#future-work)


## Components Used
For this project, the used ocmponents are:
<ul>
  <li>NodeMCU Microcontroller</li>
  <li>28BYJ-48 5V Stepper Motor (+ULN2003 controller)</li>
  <li>An LED and a 220-ohm resistor (I went with a blue one since that is all I had)</li>
  <li>One of those Bluetooth shutters that are dirt cheap (used as a wireless shutter for taking pictures)</li>
  <li>A PN2222A NPN transistor (used  to trigger the BT Shutter)</li>
  <li>An old USB phone charger (used as the power supply for the whole project)</li>
  <li>A bunch of wires and solder</li>
</ul>

## How it works
The micrcontroller acts as an access point (through the use of the ESPASynchWebServer library) and hosts a site (accessable by entering the IP:<b>10.10.10.1</b> after connecting to the ESP network <b>"Turntable"</b>) that acts as the main interface for the whole project. The user can specify an even number of phots between 2 and 200. After the amount is specified (using a slider), the "Start!" button is pressed and the motor starts its movement. Depeneding on the number of photos taken, the motor travels an amount calculated in the code and then stops, which triggers the BT shutter to take a picture on a phone/device that is connected and is acting as the main camera. The process is repeated until either the user pauses (using the "Pause!" button), the user stops (by pressing "Pause!" twice or "Start!" once), or if the process is completed. The following is how the site looks like under use:

<p align="center">
  <img src="/media/demo.gif/" alt="GIF of the UI">
</p>
The new version adds the option of constant rotation (clockwise or counter-clockwise) which also changes the rotation direction of the main photogrammetry mode.

<p align="center">
  <img src="/media/new_ui.jpg/" alt="screenshot of the new UI">
</p>

## Circuit Schematic
The circuit is rather basic, a bluetooth shutter is used here to trigger a phone acting as a camera to take pictures. A transistor is soldered on the buttons and is connected to the NodeMCU so that the button can be triggered. Once the user specifies the amount of pictures, the steps are calculated in the controller and the motor is turned accordingly. The following is a very simplified overview of what tahe circuit is like:
<p align="center">
  <img src="/media/basic_overview.png/" alt="Basic Circuit Overview">
</p>
When desgning the PCB, it was decided that all the main components (aside from the external shutter) will be integrated into one circuit. Essentially, the ULN2003 breakout is recreated (without the LEDs) into the circuit with a switch for controlling the supply and motor power separately. As for the shutter, the dimensions were measured as accurately as possible and a silk screen cutout was added so that the shutter can easily be mounted into the PCB. The following is the circuit schematic in more detail:
<p align="center">
  <img src="/media/circuit_schematic.jpg/" alt="Detailed Circuit Schematic">
</p>
the final result is rather small, with the dimensions being 62.23mm x 67.18mm. And here is the final PCB (the actual PCB is printed in black):
<p align="center">
  <img src="/media/pcb.jpg/" alt="Final PCB View">
</p>

## Physical Design
The 3D model is based on AAScan, which is credited at the bottom. Their design uses a 5:1 gear ration so the code reflects that when it comes to the amount of steps needed for each picture to be taken.

## Full Demo
The following shows how the UI is used to initiaite the table to take 2 photos of a cube. The user slider can go up to 200 pictures but for showcase purposes I just went with 2 pictures. Ofcourse, a phone connected to the BT shutter is also needed to take a picture when the table stops rotating so that different pictures are taken.
<p align="center">
  <img src="/media/table_demo.gif/" alt="GIF of the phyiscal table">
</p>

## Future Work
I would like to work on the following set of features in the future, but so far the basic core features of a photogtammetry turntable is met with good results.
<ul>
 <li><s>Create a <b>Constant Turning Mode</b> that turns the turntable into a neat countertop decoration item!</s><ul>
   <li>New version allows for CW or CCW rotation which also controls the main mode's rotation direction in a rather easy to use manner!</li> 
 </ul></li>
 <li>Add a form of animation to the site that shows shutter progress in a more "cool" manner, this one is rather ambigious, dont you think?</li>
 <li><s>Create a PCB for the whole project for better space managment, plus its cool to have it all in a "professional" look, right?</s><ul>
   <li>New circuit and PCB has a built in motor driver and fits in a rather small footprint, physical testing is next on the list!</li> 
 </ul></li>
</ul>

## Acknowledgments
In no particular order, the following resources offored a great deal of technical info that was critical to the success of this project:
<ul>
  <li><a href="https://randomnerdtutorials.com/" target="_blank">Random Nerd Tutorials:</a> Their tutorials on ESPs as webservers and other cool stuff were rather helpful!</li>
  <li><a href="https://hackaday.io/Bribro12" target="_blank">Brian Brocken:</a> His Arduino-based turntable was an aspiration for this project</li>
 <li><a href="https://codepen.io/juanbrujo" target="_blank">Jorge Epuñan:</a> Got the insperation for the slider from his CodePen page</li>
 <li><a href="https://www.thingiverse.com/thing:4167615" target="_blank">QLRO's AAScan:</a> Used a slightly modified version of their Solidworks model for the turnttable</li>
</ul>

Please give credit to "WiFi Turntable" and the resources used in the project in case you do use it, Thank you for reading! 
