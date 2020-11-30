# SpaceMaker-Bot

## Concept

The SpaceMaker bot started out as a concept to tacitly encourage social distancing. During the currant pandemic, everyone is constantly inundated with messages to maintian their distance. However, this system could use improvements. Perhaps because it's a new concept (I'd not heard anyone talk about "social distancing" prior to 2020 even despite living through the 2003 SARS pandemic), or perhaps because of fatigue, people don't seem to be very good at it. Many people are diligent about maintaining their distance from strangers of course, but when you're in the same space working and communicating with people you're familiar with, it's easy to forget or to slightly bend the rules about maintaining a 6 feet distance with others.

I thought that maybe we could make the act of social distancing a natural one through the use of objects around us. How? My first idea was that you could take robots that maintained equidistance from all the objects around them, then place a bunch of them in a public space. Because the robots would move out of the way, people could still walk through the space. But if we made the robot.......


![Front view of SpaceMaker](https://github.com/desmondcheongzx/SpaceMaker-Bot/blob/main/images/front.jpg)

SpaceMaker Bot is a concept designed to encourage tacit social distancing. During the current pandemic, everyone is constantly inundated with messages to maintain their distance. However, this system could use improvements. After months of hearing the same message, people get fatigued. Additionally, there is a natural impulse for humans to move closer, because that's the nature and pattern of human interaction.

A good method to encourage people to prevent people from getting too close should also rely on innate patterns of social interaction. One innate instinct is to avoid moving objects, or to step away from objects that are steadily moving towards you. This idea underlies the movement patterns of SpaceMaker.

Additionally, we rely on form to encourage certain behaviours. Traffic cones are possibly universally used to control the movement of people and objects. We leverage this authority of form in order to enhance SpaceMaker's ability to create distance.

![Tag team](https://github.com/desmondcheongzx/SpaceMaker-Bot/blob/main/videos/spacemaker_tag_team.gif)


## Iteration #1 - Moving the robot and initial designs
### Iteration #1.1 Adding a second ultrasound sensor
From the very first sketch it was clear to me that keeping track of distances in multiple directions would be extremely helpful for maintaining equidistance.
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
As we added more power, I started thinking about ways to control power. The initial reasoning was to preserve battery charge so that I could maximise the voltage that SpaceMaker was receiving. It was around 

Once I first experienced smoking in my circuits, as well as multiple wires melting and burning me, I knew that this was no longer simply a power conservation issue, but a safety issue. The first step of course, was to solder on a switch like we had done with the smaller batteries. This gave us an emergency power cut-off every time something went wrong. The next step was to prevent things from going wrong in the first place.

I'd noticed that it was always the same wire that melted: a thin wire that connected the ground of the H-bridge to the ground of the Arduino board. I spent some time thinking about how I could remove this wire from the circuit, but it was a necessary connection for the Arduino board to control the relays and PWMs in the H-bridge. I roped in my housemate who's majoring in engineering, but neither of us could figure out what was going wrong in the circuit. So in the end we decided that if the wire was melting because there was too much current flowing through it, we simply had to increase the resistance of the wire. Playing around, we found that a 330 ohm resistor was sufficient to prevent further mishaps, while still allowing sufficient current to flow through.

Finally, there was still further electrical interference between the Arduino board and the H-bridge. Realising that this was the cause of various unexpected behaviours and short-circuits took some time, but once I realised it, separating the two circuit boards with a non-conducting material was sufficient to solve the issue.

## Iteration #4 - Going wireless
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

I've been a programmer for nearly half my life now. I enjoy the activity greatly because there's a rich and ever-growing toolset to develop anything you can imagine. I also find it very soothing because while software can behave erratically, it's a familiar beast that can be tamed by relaxing into the system. Working with hardware this semester was a completely different experience from the rest of my university career.

I'll always fondly recall my childhood playing with Lego Mindstorms, an 

Hardware lessons aside, this has been a rich lesson in ideation and prototyping. I hope to carry this experience into the many projects and discussions to come in the future.
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
