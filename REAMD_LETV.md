![](/images/letv-freepad/freepad_letv_12.png)

Some time ago, digging TaoBao, I've found an interesting miniature ZigBee remote control keypad with eight buttons.
As this device is powered by a CC2530 chip, an idea appeared in my head to flash it with a great [freepad](https://github.com/diyruz/freepad/) firmware, which was used in my previous remote controls.

UPD: This keypad has appeared on [Aliexpress](https://aliexpress.ru/item/1005001962088229.html?&sku_id=12000018238549831)

Keypad has 8 buttons, two-colored led, and is powered by CR2032 lithium cell.

![](/images/letv-freepad/freepad_letv_13.png)
![](/images/letv-freepad/freepad_letv_2.png)


It is quite easy to disassemble this device. The case is held by plastic latches, and two screws hid beneath the white ring.

![](/images/letv-freepad/freepad_letv_5.png)

A totally disassembled keypad looks like this:

![](/images/letv-freepad/freepad_letv.png)

![](/images/letv-freepad/freepad_letv_3.png)

There are membrane buttons, held by a scotch duct-tape on the PCB.
The scotch-tape hides firmware flashing pads, so you need to remove it or cut a window to have access.

![](/images/letv-freepad/freepad_letv_1.png)

As you can see, contacts are located in a pretty chaotic order. I don't wish to solder wires and that's why I decided to make a flashing adapter.

![](/images/letv-freepad/freepad_letv_6.png)

![](/images/letv-freepad/freepad_letv_7.png)

![](/images/letv-freepad/freepad_letv_8.png)

The only thing you need to do is to upload the modified by [@xyzroe](https://t.me/xyzroe) version of freepad [firmware](https://github.com/diyruz/freepad/) with any 
situated flashing [method](https://zigbee.wiki/books/%D0%BF%D1%80%D0%BE%D1%88%D0%B8%D0%B2%D0%BA%D0%B8/page/%D0%BF%D1%80%D0%BE%D1%88%D0%B8%D0%B2%D0%BA%D0%B0-cc2531cc2530).
Zigbee2mqtt converter file is included in the package archive.

Here is how the device attached to the [SLS Gateway](https://slsys.github.io/Gateway/README_rus.html) looks like:

![](/images/letv-freepad/freepad_letv_9.png)
![](/images/letv-freepad/freepad_letv_11.png)

[Original post by Jager](https://modkam.ru/?p=1791)

### Links
 - [DXF-file](https://www.dropbox.com/s/y3p7kqxyiqmid25/LeTV_pogopin.dxf?dl=0)  
   for DIY flashing adapter.
   

 - [Freepad firmware releases](https://github.com/diyruz/freepad/releases/latest)  
   choose firmware hex file with __LeTV__ suffix.  


 - [Flash instruction (in Russian)](https://zigbee.wiki/books/%D0%BF%D1%80%D0%BE%D1%88%D0%B8%D0%B2%D0%BA%D0%B8/page/%D0%BF%D1%80%D0%BE%D1%88%D0%B8%D0%B2%D0%BA%D0%B0-cc2531cc2530)  
   In Russian for now, but links and pictures are not less useful.