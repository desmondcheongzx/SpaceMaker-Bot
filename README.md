# SpaceMaker-Bot

Demo video: https://vimeo.com/482537350

## Concept

The SpaceMaker bot started out as a concept to tacitly encourage social distancing. During the currant pandemic, everyone is constantly inundated with messages to maintain their distance. However, this system could use improvements. After months of hearing the same message, people get fatigued. Additionally, there is a natural impulse for humans to move closer, because that's the nature and pattern of human interaction.

So the idea was that instead of relying on explicit messages to maintain social distance, another possibility was to rely on innate patterns of social interaction. One innate instinct is to avoid moving objects, or to step away from objects that are steadily moving towards you. Another instinct is to follow certain authoritative forms such as traffic cones. Traffic cones are possibly universally used around the world to delimit spaces and control the movement of people and objects, and so we rely on this form to tacitly encourage social distancing.

However, as can be seen from the demo video, this idea has evolved beyond encouraging social distancing, to rethinking how we can automate the delimination of space. This provides us with a rich possibility of applications ranging from traffic control (such as for [reversible lanes](https://en.wikipedia.org/wiki/Reversible_lane)), or human movement control in both public and private spaces.

![Tag team](https://github.com/desmondcheongzx/SpaceMaker-Bot/blob/main/videos/spacemaker_tag_team.gif)


## Iteration #1 - Moving the robot and initial designs
### Iteration #1.1 Adding a second ultrasound sensor
From the very first sketch it was apparent that keeping track of distances in multiple directions would be extremely helpful for maintaining equidistance. In terms of wiring another ultrasound sensor to the Arduino, this wasn't any different from setting up the first ultrasound sensor. The question was how should this sensor be mounted?

The first ultrasound sensor had a convenient dock at the front of the mobile chasis, however no such dock existed for a second ultrasound. As such, we laser cut a holder for this rear ultrasound.

<img src="https://github.com/desmondcheongzx/SpaceMaker-Bot/blob/main/images/ultrasound_holders.PNG" height="250"/>
<img src="https://github.com/desmondcheongzx/SpaceMaker-Bot/blob/main/images/ultrasound1.jpg" height="250"/>
<img src="https://github.com/desmondcheongzx/SpaceMaker-Bot/blob/main/images/ultrasound2.jpg" height="250"/>
<img src="https://github.com/desmondcheongzx/SpaceMaker-Bot/blob/main/images/ultrasound3.jpg" height="250"/>

### Iteration #1.2 Reliable ultrasound distance sensing
One drawback of the ultrasound sensors we were using is that they're really only accurate most of the time within a certain distance. Thankfully this is familiar territory in machine learning and probabilistic methods where we have algorithms that are only probably approximately correct.

Let's say that at some timestep _t_ we have a true distance _x_ that produces noisy ultrasound readings _y_ ~ Normal(_x_, _some variance_). These seem like valid assumptions to make, and if our readings do indeed follow this distribution, then we can take advantage of the Law of Large Numbers to estimate _x_ by taking the average of all the ultrasounds readings _y_ at time _t_. As the number of readings we take increases, the variance of our average decreases and we converge to the true distance _x_. However, we don't want to carry out too many readings because each reading takes awhile to complete, which slows down our robot's reaction time. Thankfully, if our assumptions are valid, the Central Limit Theorem kicks in very fast when we sample from a normal distribution, and so a handful of readings are more than sufficient. Our improved ultrasound sensor performs as follows.
```
int get_distance(int cur_trig, int cur_echo) {
  int sum = 0;
  int n_trials = 15;
  // fire the ultrasound multiple times and take the minimum reading
  for (int a = 0; a < n_trials; ++a) {
    int distance = fire_ultrasound(cur_trig, cur_echo);
    sum += distance;
  }
  return distance / n_trials;
}
```
We could have also explored using a hidden markov model with the true distance as the latent variable, and the ultrasound reading as the observed variable. Our true distance is allowed to change from state to state, but every true distance is correlated with its previous true distance. Moreover, ultrasound readings are correlated with the true distance at the point in time. We could even combine this method with the averaging method above. These dependencies should give us a model that's resilient to noisy ultrasound readings. It would be interesting to see how accurate we could make our ultrasound readings using computation alone, but that's an experiment for another time.

## Iteration #2 - Traffic cones and playing with form
small cone -> Medium cone ->
## Iteration #3 - Solving movement issues with more power and H-bridges

### Iteration #3.1 - Safety concerns
As we added more power, I started thinking about ways to control power such as with a switch. The initial reasoning was to preserve battery charge so that I could maximise the voltage that SpaceMaker was receiving. But around the time that I switched to a 9.6V 2400mAh NiMH battery pack, my circuits started to smoke, and I melted through wires on multiple occassions.

[insert pic of melted wire]

This was now no longer a question of power conservation, but a question of safety. So now before testing any circuits with new battery packs, we made sure to first solder on a switch between the battery pack and the H-bridge. This gave us an emergency power cut-off every time something went wrong. The next step was to prevent things from going wrong in the first place.

[insert pic of switch]

Going through our notes on the burnt wires, I noticed that it was always the same wire that melted: a thin wire that connected the ground of the H-bridge to the ground of the Arduino board. I spent some time thinking about how I could remove this wire from the circuit, but it was a necessary connection for the Arduino board to control the relays and PWMs in the H-bridge. I roped in my housemate who's majoring in engineering, but neither of us could figure out what was going wrong in the circuit. So in the end we decided that if the wire was melting because there was too much current flowing through it, we simply had to increase the resistance of the wire. Playing around, we found that a 330 ohm resistor was sufficient to prevent further mishaps, while still allowing sufficient current to flow through.

[insert pic of resistor]

Finally, there was still further electrical interference between the Arduino board and the H-bridge. Realising that this was the cause of various unexpected behaviours and short-circuits took some time, but once I realised it, separating the two circuit boards with a non-conducting material was easy enough and sufficient to solve the issue.

## Iteration #4 - Going wireless

Following the previous iteration, we finally get reliable movement from SpaceMaker and can now move on to some other interesting questions. At the start of this project, I was curious about how a population of SpaceMakers might change the pattern of human movement and interaction within a space. I was also curious about what communication between robots and a central controller could offer in terms of applications. These ideas necessitated some form of communication between SpaceMakers and/or a command centre (which could be another SpaceMaker). So it was finally time to jump into Bluetooth communication.

I'd worked with a Python Bluetooth library before so I didn't expect too many challenges. But since I got my Bluetooth modules at the cheapest price point possible, I found myself with two modules and no documentation to work with. Thankfully there were many tutorials, YouTube videos, and fora available on the HC-05 module. Within a week we managed to set up a Bluetooth connection between our Arduino and a laptop or smartphone.

[insert wireless gif]

This was also the stage where we were pretty familiar with the whole system we were working with, so many other tweaks took place. For example, adjusting the length of spacers used to improve the balance provided by ball bearings; removing the default wires to the mobile chasis and simplifying the wiring; adding a platform above the circuit boards to protect the wiring as well as provide a stable base for holding the battery packs and lose components such as the Bluetooth module and switches.

### Iteration #4.1 On-the-fly pulse-width modulation
As our prototype became more fleshed out and the deadline for the final video drew nearer, we had to start testing out SpaceMaker in different terrains. However, outdoor terrains were challenging because slopes and debris would often cause SpaceMaker to stall. Thankfully, we'd just spent weeks scaling up the amount of power available to our DC motors, so this could easily be solved by increasing the pulse-width modulation (PWM) of each motor according to their performance.

To improve the convenience of fine-tuning the PWM as well as to add the ability to adjust the PWM as we went from terrain to terrain, I added a switch to the Bluetooth input loop. Now, in addition to movement commands, we could simply send numeric characters to SpaceMaker via Bluetooth in order to adjust the power being sent to each motor. It's a little crude, but it worked for our circumstances.
```
void check_bt() {
  if (EEBlue.available()) {
    char bt_val = EEBlue.read();
    switch(bt_val) {
      ...
      case '2':
        l_power += 25;
        analogWrite(pwm_l, l_power);
        break;
      case '1':
        l_power -= 5;
        analogWrite(pwm_l, l_power);
        break;
      case '9':
        r_power += 25;
        analogWrite(pwm_r, r_power);
        break;
      case '8':
        r_power -= 5;
        analogWrite(pwm_r, r_power);
        break;
    }
  }
}
```

## Creating the demo video
At this point, we had a working prototype and it was time to create a demo video. Throughout the prototyping process, I had imagined SpaceMaker to be a social distancing robot as originally envisioned. However, as we worked on the prototype and the various technical challenges came to light, it became clear that we would need a lot more technical progress (most notably, the ability to use multiple SpaceMakers) before some of these ideas become feasible. I also wasn't convinced that social distancing was the right use case for this robot, and slowly lost interest in that idea.

Then, after many showers spent thinking about this, I noticed that there were many traffic cones next to each apartment in the place where I live. This prompted the question of whether SpaceMaker could be used to expand the range of use cases for existing traffic cones, which eventually led to the idea of proximity-based traffic cones in parking lots. This would enable users such as homeowners/passholders/handicapped drivers to reserve parking spaces.

I also drew inspiration from one of my colleague's projects for a [smart sign](https://hiloni25.tumblr.com/post/635750285256572928/smart-sign). I thought that it would be interesting to make a mobile version of the smart sign, which would be a natural extension of SpaceMaker's current design because traffic cones already serve as a form of warning, and we had also established a wireless connection between SpaceMaker and smartphones.

Finally, there was the actual filming of these ideas. I found this to be a very productive process in terms of ideation. When explaining the concept of the video to my film crew (my housemates), I needed to pinpoint the ideas that I wanted to convey and the scenes we would need to achieve this. At the same time, the team pointed out where some ideas didn't make much sense, or where they could be improved or better presented. In the process the concepts being presented became much more refined compared to the very rough story board I had prepared (see below). This goes to show how we slowly become overfamiliar with a concept while we work, which makes us blind to various shortcomings. In times like these, outside perspectives are imperative to improve the design and communication of an idea.
## Future directions
As the semester came to a close, we had to start wrapping up our projects. However, if there were one truism that we've learnt from this course, it would be that every iteration informs the next iteration. True enough, after making my final prototype and hearing my colleagues' critiques, I'm left with many ideas to try out. Someday, if I have the itch to work on a hardware project again, I might well implement some of these improvements. But for now here are my thoughts.

### Form
Throughout the development of SpaceMaker, there has consistent feedback on how its form could be experimented with or improved. In particular, the size of the traffic cone that SpaceMaker uses could make a big difference in how its read. Previously we faced various constraints with the size of the cones we could use with SpaceMaker because heavy loads made the robot stall. However, now that the motor issues have been resolved, we're freed up to experiment.

What would happen if SpaceMaker was completely enveloped in a traffic cone? In this case, people would either expect SpaceMaker to be an ordinary traffic cone; or, when they've had enough interactions with SpaceMaker, they might start assuming that every traffic cone around them has the potential for movement. To try out this idea, we would either have to purchase a large enough traffic cone (a full 28" cone perhaps?), or construct such a cone. We could then use one of our earlier mounting systems to attach this cone to SpaceMaker, then attach its ultrasound sensors to the exterior surface of the cone, or drill holes for the ultrasound sensors.

### Multiple robots + collective behaviour
One of the original goals for this project was to have multiple SpaceMaker bots interacting and communicating with each other. It eventually became clear that there were other pressing issues to work on before this would be possible, so this idea was shelved for the time being. However, I believe this is still an interesting problem, and one rich with possibilities. Thinking through some of the ideas in [collective animal behaviour](https://www.goodreads.com/book/show/9856806-collective-animal-behavior), a mobile Arduino with a distance sensor should be more than capable of producing rich collective behaviour patterns. We wouldn't even need to enable Bluetooth communication between each SpaceMaker unit. It would be interesting to explore some theoretical questions in this area, as well as translate them to some productive application.

### Mechanical improvements
Watching SpaceMaker move, it's immediately noticeable that it moves in fits and starts. Moreover, because it brakes abruptly, it is prone to falling over when moving on a slope. As it is, the mechanical issues currently limit the potential use cases for SpaceMaker. Even for the use cases presented in the demo video, multiple takes had to be filmed because SpaceMaker would occasionally move erratically or tip itself over. However, some colleagues mentioned that this motion was endearing and I do agree that I would like some of this character to remain in future iterations. A natural solution to this dilemma would be to solve the mechanical issues that SpaceMaker currently faces, then add human-like movements afterwards. Some potential ideas are discussed below.

1. __Smoother pulse-width modulation.__ One reason for the jerky movements is that we currently only have one power setting for movement. This means that starting and stopping motions are all-or-nothing and thus abrupt. If we instead slowly ramped up the power and then tapered it off as a movement starts and stops, then SpaceMaker would also move more smoothly.

2. __Add a gearbox.__ Even if we could taper off the power fed to our motors as a movement ends, the start of a movement still poses a challenge. DC motors, under load and in a high friction environment, require a lot of current to avoid stalling. This means that slowly ramping up power wouldn't produce a smooth start because the robot wouldn't even be able to start moving until a threshold is reached. And once that threshold is reached, we start moving abruptly and quickly. To solve this, we can use a gearbox with a high gear ratio when a movement begins. This ensures that we can smoothly generate sufficient torque to move even at slow starting speeds. 

3. __Larger wheels.__ Currently, due to the small diameter of the wheels, cracks and bumps cause SpaceMaker to stumble and even stall. However, larger wheels can only be installed in tandem with adding a gearbox, otherwise we wouldn't be able to generate sufficient torque.

4. __Improved weight distribution.__ To improve the overall balance of SpaceMaker, we can either increase the diameter of the base, or shift its centre of gravity downwards. We could lower its centre of gravity by shifting heavier components such as the two battery packs down onto the base chasis. Another option would be to add ballast to the robot.

## Final remarks
Coming from a software background, I was caught completely off gaurd by the complexities involved in working with hardware. Instead of only working with ideas and code, there were questions of design, material, weight, circuits, power, and all sorts of other physical considerations to take note of. In light of all this complexity, it was very important to move back-and-forth, side-to-side, from rough sketches to higher and higher resolution prototypes. This has been a rich lesson in ideation and prototyping and I hope to carry this experience into the many projects and discussions to come in life.

[insert pixar picture]

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
