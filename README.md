# XMoto
very small I2C brushed motor controller

Transcribed text from my german(!) video:
https://youtu.be/MuoPkisycU4

![Screenshot](images/IMG_E0511.JPG)

---

It is so far, last time I designed a board that should run a motor via I2C. Two days ago the board finally arrived and I have to say I am thrilled about the result, but see even here is a small UNPack video that should show you how the boards arrived and in what condition I saw them the first time.


Unpacking


 the circuit boards themselves were packed in a plastic bag, and I also got an extra roll of insulating tape. The boards were glued together on the backside with a normal tape and the first impression of the boards was already overwhelming, because the result looked very clean. 

But of course the devil is always in the details and it would not be the first board that would not work for me immediately. You always have to expect mistakes, and that can drive you crazy sometimes, because you put so much time into the design and probably forgot something.


I also took a motor and checked if the mechanics fit, I have to say the motor just went into the existing hole, but that doesn't matter. The soldering lugs fit exactly into the holes, as you can see on the photo here and the whole thing holds by itself. But I save that for now, because I plan to do some tests with this board.
Price
Now let's get to the most important thing, the price! I paid for the order around 58 Euro (-$7 discount), in there was the delivery, the production and so on, in addition you also have to take into account the customs clearance from DHL, which amounts to this: 
the import sales tax was 9,91 €
the commission for the provision of capital costs 12.50 €, 
and 
the value added tax on the capital provision commission was € 2.38 
So I paid 24.79 at the front door, so let's say 25 €. 
If we add it all up, we come to a price of about 77 euros.


Since these were 10 boards, which are completely assembled, one board costs me 7.70 €. This is actually still possible, I imagined it to be more expensive. 


But you can still influence the price very much by providing more PCBs on a panel and thus get more for our money. Maybe I'll make a video about this, you can watch the workaround I published last week in my video channel. Here a panel will be produced that can be used in the SMD assembly, because easyeda still has a bug there. 
This way we can order panels with a maximum size of 400 x 400 mm, and a lot of XMotos fit on it :). Then the total price per board will also be lower.
Testing


 So what do we test first, if you have such a finished board in front of you, you should test if there is a short circuit between ground and positive, if you have done that, the next step would be to apply a normal voltage of say 7 volts to test if the voltage regulator works, there should be 5 volts at the output and on the oscilloscope it should look relatively straight.


UDPI
 First I try to load a blink sketch on the ATtiny 16/17 with the new UDPI or "Unified Program and Debug Interface". You only need one line to program the MPU, this is done quickly by loading the sketch on an Arduino nano and then connecting pin D6 with the reset line of the atiny via a resistor. I linked the manual in english in the video description, you can read all about it and now we try it by ourselves. 


https://www.elektormagazine.de/news/review-attiny817-xplained-mini/14732
https://www.electronics-lab.com/project/getting-started-with-the-new-attiny-chips-programming-the-microchips-0-series-and-1-series-attiny-with-the-arduino-ide/


Prepare


For this we need three cables first of course we have to connect to ground, the second is connected to 5 Volt plus from the Arduino nano and the third is the UDPI cable which is connected directly to the reset port of the Attiny's on the back of the board.


So we take out the soldering iron and off we go we need a 4-pin connection on the back of the board plus a connection to the UDPI port point, that's it and we can install the appropriate software on our computer according to the instructions. And what can I tell you, it worked right away. It was a nice feeling to see the LED lights and to know that the board works in its basic features. I was very relieved because to repair something on this board is a little bit more difficult, because the components are so small that with my 53 years I would have problems to lay any line and so on but we were lucky, everything works and we can continue now. 

Serial Test
To be able to debug the code later on, it is very important to me that I can test the serial port of the MCU, so in the blink sketch just write some serial output into it and connect the RX and TX lines with an extra UART adapter. Here I already noticed the first mistake, I labeled the lines for RX and TX the wrong way round, this is actually done to show the user where to connect his RX and TX. Since these are crossed, that was unhandy for me, since I needed five minutes to find out that I switched RX and TX. This test also worked immediately and I could see the text "blink" in a serial console. 

 Next we should test the function of the motor chip and the hall sensor, I have come up with this little code, which can be found in my GIT repository, the link is in the video description. 
Engine test
https://www.ti.com/lit/ds/symlink/drv8837.pdf
So let's look at the motor driver DRV 8837, so first we look at the datasheet and find out that we have two inputs that are controlled by PWM. If one input is Hi, the motor turns in one direction, the other Hi then turns in the other direction, the PWM sets the speed of the motor over the duty cycle. So we will add the code so that we set a PWM signal first on one pin and then on the other PIN. The motor once should move in one direction and then in the other direction. We also add some debugging to see what's happening with the motor. 
Hall sensor
The hall sensor on our board is a chinese model, and there is only a chinese datasheet, it's the CC 6201, which is only used as a switch either low or high, and so we can test the hi sensor with this simple code, so that if a magnet is passed by the sensor ... our LED lights up, that should be enough to test this sensor. 
https://draeger-it.blog/arduino-lektion-18-magnetischer-hall-sensor/?cn-reloaded=1
Conclusion 
In general I am very satisfied, also with my work on the PCB ... but what JLCPCB has sent me is a straight "one" with an asterisk. Oh, I forgot about the delivery times, the production took a week and the delivery took only 2 days. Here is the protocol that JLCPCB provides you with. Also nice, you can see small videos from the production, how your boards are made.

This gave me an idea, you could take a picture of the board after production and send it to the customer via mail, so he can check for errors and doesn't have to wait for the board. I wrote it in their forum, let's see if something happens.

Next time I will present XMoto with software and on the Marlin Board, but that will take 2 weeks ... because of private plans. But follow my communitychannel on YouTube, there I write from time to time how far I am and when the next video is coming.

Take care and stay healthy
Your Frank
