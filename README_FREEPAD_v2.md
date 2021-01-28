![](/images/orig-freepad/v2/19.jpg)  

The first version of the Zigbee remote control is good, but this option is not suitable when mobility is needed. For this reason, a second version appeared, in which Jager tried to make a more compact version.

Since 20 buttons are not always and not always needed, Jager had to come up with a board design that allows you to assemble 3 versions of remotes, for 20, 12, and 8 buttons. The following is a process with pictures and short explanations.

The circuit is almost identical to the first version
![](/images/orig-freepad/v2/0.jpg)  

using the Zigbee module from EBYTE E18-MS1-PCB
![](/images/orig-freepad/v2/1.jpg)  

compact buttons 3x4x2 mm
![](/images/orig-freepad/v2/2.jpg)  

The board is wired in DiTrace  
![](/images/orig-freepad/v2/3.jpg)  
Note that the contact pads for the buttons are not designed for 4 pin buttons, as it might seem, these are options for a different offset from the center, due to the design of the case.

Perforation is clearly visible on the renders, in these places unnecessary buttons are “broken off”. The very idea of ​​separating the excess is certainly not new, but those restrictions are imposed on the flight of thought. manufacturer's conditions. Instead of cutouts, Jager had to make holes with a diameter of 0.3 mm.
![](/images/orig-freepad/v2/4.jpg)  

Boards ordered in JLCPCB
![](/images/orig-freepad/v2/5.jpg)  

Minimum soldering  
![](/images/orig-freepad/v2/6.jpg)  

Made a variant of the case for eight buttons in the form of a keychain. 
![](/images/orig-freepad/v2/7.jpg)  
![](/images/orig-freepad/v2/8.jpg)  
![](/images/orig-freepad/v2/9.jpg)  
![](/images/orig-freepad/v2/10.jpg)  

The general idea is this, the insert on top of the board allows you to distribute the force over a larger area
![](/images/orig-freepad/v2/11.jpg)

Printed on 3d printer  
![](/images/orig-freepad/v2/12.jpg)

First tried on  
![](/images/orig-freepad/v2/13.jpg)  

The keyboard
![](/images/orig-freepad/v2/14.jpg)

Different colors  
![](/images/orig-freepad/v2/15.jpg)

Lamination will protect the paper keyboard

![](/images/orig-freepad/v2/16.jpg)  

Remains only to cut and try  
![](/images/orig-freepad/v2/17.jpg)  
![](/images/orig-freepad/v2/18.jpg)

In scale  
![](/images/orig-freepad/v2/19.jpg)  
![](/images/orig-freepad/v2/20.jpg)  

Compared to first version  
![](/images/orig-freepad/v2/21.jpg)  

From the software point of view, the consoles are identical.

@anonymass has written a open source firmware. The firmware has a lot of  options, binding, multi-click, etc. High stability and speed.

@nick7zmail repeated the construction, making the original keyboard
![](/images/orig-freepad/v2/22.jpg)  

but he did not stop there and made various housing options, for 8, 12, and 20 buttons.
![](/images/orig-freepad/v2/23.jpg)  

@Madjestik58 designed and manufactured his version of the 4 button remote all necessary files for manufacturing are included.
![](/images/orig-freepad/v2/24.png)  


Another version of the case for the keypad from obihoernchen
![](/images/orig-freepad/v2/25.png)  

### What's button mapping?
![](/images/orig-freepad/v2/zigbee_keypad22.png)

### Files to reproduce
* [Gerbers and STL for 8 button](https://github.com/diyruz/freepad/hardware/v2) by [Jager](https://t.me/Jager_f)  
* [EasyEDA and STL for 4 button](https://github.com/diyruz/freepad/hardware/4_buttons) by [@Madjestik58](https://t.me/Madjestik58)  
* [Housing for 8, 12 and 20 buttons](https://www.thingiverse.com/thing:4161582) by [@nick7zmail](https://connect.smartliving.ru/profile/225)  
* [Housing for 20 buttons](https://www.thingiverse.com/thing:4668986) by [obihoernchen](https://www.thingiverse.com/obihoernchen)  
* [Firmware](https://github.com/diyruz/freepad/releases) by [@anonymass](https://t.me/anonymass)  
choose firmware hex file __without LeTV__ suffix

[Original post by Jager](https://modkam.ru/?p=1264)  
