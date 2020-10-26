# SpaceMaker-Bot

![Front view of SpaceMaker](https://github.com/desmondcheongzx/SpaceMaker-Bot/blob/main/images/front.jpg)

SpaceMaker Bot is a concept designed to encourage tacit social distancing. During the current pandemic, everyone is constantly inundated with messages to maintain their distance. However, this system could use improvements. After months of hearing the same message, people get fatigued. Additionally, there is a natural impulse for humans to move closer, because that's the nature and pattern of human interaction.

A good method to encourage people to prevent people from getting too close should also rely on innate patterns of social interaction. One innate instinct is to avoid moving objects, or to step away from objects that are steadily moving towards you. This idea underlies the movement patterns of SpaceMaker.

Additionally, we rely on form to encourage certain behaviours. Traffic cones are possibly universally used to control the movement of people and objects. We leverage this authority of form in order to enhance SpaceMaker's ability to create distance.

![Tag team](https://github.com/desmondcheongzx/SpaceMaker-Bot/blob/main/videos/spacemaker_tag_team.gif)

## Known bugs

Unfortunately, at present, no SpaceMaker bot has a working program anymore. I was prototyping when I started to receive the following error upon uploads:
```
avrdude: stk500_recv(): programmer is not responding
avrdude: stk500_getsync() attempt 1 of 10: not in sync: resp=0x00
...
```

The error persists on both Linux and Windows machines and I can't seem to resolve it. Hence the following report is based on work done until the point of failure. I would certainly like to continue working on this and hopefully resolve it asap.

## Hardware

### Sensors and Actuators

The SpaceMaker bot relies on two ultrasound sensors, placed roughly along the same axis, to measure the distances to the closest objects in front and behind it. It also uses two motors to achieve forwards, backwards, and rotation movements.

### Mounts

In order to mount the rear ultrasound, we make the following lasercut to serve as a holder for the ultrasound.

![Lasercut diagram for ultrasound mount](https://github.com/desmondcheongzx/SpaceMaker-Bot/blob/main/images/ultrasound_holders.PNG)
![Ultrasound holder image 1](https://github.com/desmondcheongzx/SpaceMaker-Bot/blob/main/images/ultrasound1.jpg)
![Ultrasound holder image 2](https://github.com/desmondcheongzx/SpaceMaker-Bot/blob/main/images/ultrasound2.jpg)
![Ultrasound holder image 3](https://github.com/desmondcheongzx/SpaceMaker-Bot/blob/main/images/ultrasound3.jpg)

Additionally, in order to support larger traffic cones on the SpaceMaker, we created several cardboard mounts that fit different heights of traffic cones. The cardboard mounts roughly follow the illustrator schematic for lasercutting, with the appropriate modifications to accomodate various heights.

![Lasercut diagram for mounts](https://github.com/desmondcheongzx/SpaceMaker-Bot/blob/main/images/mounts.PNG)
![All mounts](https://github.com/desmondcheongzx/SpaceMaker-Bot/blob/main/images/all_mounts.jpg)
![Mount image 1](https://github.com/desmondcheongzx/SpaceMaker-Bot/blob/main/images/mount1.jpg)
![Mount image 2](https://github.com/desmondcheongzx/SpaceMaker-Bot/blob/main/images/mount2.jpg)
![Mount image 3](https://github.com/desmondcheongzx/SpaceMaker-Bot/blob/main/images/mount3.jpg)

We tried playing around with the form of SpaceMaker by using traffic cones of larger and larger sizes. Unfortunately, while 12-inch traffic cones started to reach the point where Spacemaker was properly covered up, we found that the motors were unable to keep up with the load.

![Large cone movement](https://github.com/desmondcheongzx/SpaceMaker-Bot/blob/main/videos/spacemaker_large_cone.gif)

## Software

The following sections describe various movement patterns employed by SpaceMaker, as well as demo videos to give a sense of how these patterns might play out.

### Tacit Space Maker

In Tacit Space Maker mode, the SpaceMaker rotates until it finds two people standing too close too each other. It then repeatedly moves back and forth between the two people to encourage them to move apart.

![Tacit Space Maker Mode](https://github.com/desmondcheongzx/SpaceMaker-Bot/blob/main/videos/spacemaker_tacit_space_maker.gif)

### Gantry

In Gantry mode, the SpaceMaker waits until someone crosses its path (perhaps at the entrace to a room or building). It then moves to the position that the person occupied, and waits there for three seconds before moving back to its original waiting position. This is meant to discourage people from following too closely behind someone else.

![Gantry Mode](https://github.com/desmondcheongzx/SpaceMaker-Bot/blob/main/videos/spacemaker_gantry.gif)

### Equidistance mode

In equidistance mode, the SpaceMaker tries to stay as far away from all objects that surround it. The idea is that one SpaceMaker maintains equidistance, but a population of SpaceMakers create social distance, as people moving and interacting in a space are tacitly encouraged to navigate around the robots.

Implementing this with the current base was technically challenging for two reasons:

1. Bidirectional movement restriction

2. Bidirectional distance sensing

These restrictions mean that we can only read and adjust for the distance along one axis at any one point in time. In order to maintain equidistance in all directions, SpaceMaker rotates ~1 quadrant every few seconds. By repeated sampling of random axes, SpaceMaker can maintain equidistance on average.

![Equidistance Mode](https://github.com/desmondcheongzx/SpaceMaker-Bot/blob/main/videos/spacemaker_equidistance.gif)
