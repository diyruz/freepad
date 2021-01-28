![](/images/orig-freepad/v1/14.jpg)  

This is a kind of universal remote control, which displays the most frequently used functions in a particular room. This can be lighting control, multimedia equipment, various UD scenarios, etc.

This remote control will live in the kitchen, the icons show that it will be possible to control the light, hood, music, set the timer.

The device is based on a module from EBYTE E18-MS1-PCB or E18-MS1PA1-PCB if an increased communication range is required.
![](/images/orig-freepad/v2/1.jpg)  
The advantages of this module are cheapness and ultra-low consumption, 0.4 μA in deep sleep mode.

The scheme uses a matrix of 20 buttons. The remote control is powered by one CR2032 element if a 2.5mW module is used, and from two elements via an LDO when using a 100mW module. There is also a LED and a buzzer on the diagram to indicate both the actual operation of the buttons and the display of service modes.  
![](/images/orig-freepad/v1/0.png)  

The dimensions of the device are determined by buttons 12x12x7.3 mm with transparent covers, I must say on AliExpress, their choice is very limited and this is practically the only option. There is a version with contacts for surface mounting. , but you can also use such as in the picture, it is enough to "bite off" the legs flush with the body.  
![](/images/orig-freepad/v1/1.jpg)  

The board was spread in DipTrace
![](/images/orig-freepad/v1/2.jpg)

Virtual result
![](/images/orig-freepad/v1/3.jpg)  
![](/images/orig-freepad/v1/4.jpg)  
Satisfied with the result, Jager place an order for the boards.

While the boards are being manufactured, Jager engaged in the case,
![](/images/orig-freepad/v1/5.jpg)  
![](/images/orig-freepad/v1/6.png)  

Then print on a 3D printer. For fastening to a metal surface, magnets 9 x 1.5 mm are provided, with an effort they snap into place. Fastening the board with M3x5 screws.
![](/images/orig-freepad/v1/7.jpg)  

The upper part can also be printed, but it is much more aesthetically pleasing to cut out with a laser from plexiglass. Light transmittance 2%, enough to see the glow of the LED and hide the insides, 3mm thick.  Held in place by friction.  
![](/images/orig-freepad/v1/8.jpg)  

Printed pictograms for buttons  
![](/images/orig-freepad/v1/9.jpg)  

The boards are ready  
![](/images/orig-freepad/v1/10.jpg)  

a little soldering  
![](/images/orig-freepad/v1/11.jpg)  

and can be assembled for size estimation,
![](/images/orig-freepad/v1/12.jpg)  
![](/images/orig-freepad/v1/13.jpg)  

in comparison with the IKEA TRÅDFRI remote control
![](/images/orig-freepad/v1/14.jpg)  

element AA
![](/images/orig-freepad/v1/15.jpg)  

Variant of fastening to a metal surface  
![](/images/orig-freepad/v1/16.jpg)

The original firmware was written by Vladislav @DJONvl, support in the ioBroker driver was provided by Kirov Ilya @goofyk, for which special thanks to them.  
![](/images/orig-freepad/v1/17.jpg)  

Consumption corresponds to the one declared by the manufacturer, which will allow the remote control to work from one CR2032 element from 1 to 2 years.  
![](/images/orig-freepad/v1/18.jpg)  

The firmware implements both light and sound indications of pressing. There is a network presence control (if the device is out of the network, pressing the buttons is not confirmed by light and sound). Work on the functionality of the firmware continues, possibly binding, and other options will appear.

@anonymass has written a new open-source firmware. The firmware added a lot of new options, binding, multi-click, etc. Significantly improved stability and speed.

@Madjestik58 made his version of the case using other buttons, 12x12 5mm high. It turned out great!  

![](/images/orig-freepad/v1/19.jpg)  

### What's button mapping?
![](/images/orig-freepad/v1/20.jpg) 

### Files to reproduce
* [Gerbers, STL, DXF](https://github.com/diyruz/freepad/hardware/v2) by [Jager](https://t.me/Jager_f)  

* [Another housing](https://www.thingiverse.com/thing:4256330) by [@Madjestik58](https://t.me/Madjestik58)  

* [NEW firmware](https://github.com/diyruz/freepad/releases) by [@anonymass](https://t.me/anonymass)  
choose firmware hex file  __without LeTV__ suffix  

* [OLD original firmware hex](https://github.com/diyruz/freepad/hardware/v1/maxPW_key17setup_DIYRuZ_KEYPAD20.hex) by @DJONvl

[Original post by Jager](https://modkam.ru/?p=1114)  
