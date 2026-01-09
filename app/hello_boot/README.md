## Hardware Section
Once you have sucessfully generated the binary hello_boot.bin file by running "make" in parent directory then we are
ready to move ahead.  
You can use any FTDI chip for UART monitoring. CAUTION: Set the jumper to 3.3V or you will end up frying the SoC!  
Do the following connections, always GND first!

```
          USB-TTL Adapter
   +---------------------------+
   |  VCC  GND  TX  RX         |
   |  o     o   o   o          |
   +---------------------------+
     |     |   |   |
     |     |   |   |
     |     |   |   |
     |     |   |   |
     |     |   |   |
     |     |   |   |
     |     |   |   +--------> TH1250 TX0
     |     |   +------------> TH1250 RX0
     +     +----------------> TH1250 GND
     +
(VCC optional, only if device needs 3.3V or 5V power)
```

In my case, I have a spare ESP32-NodeMCU Dev board which I will be using as my UART monitor. I have uploaded the .ino script
here, if anyone is interested (basic stuff O_o).  

## Software Section
I would recommend using any Serial Monitor like minicom or picocom or screen. In this case, it is minicom.  

Once the connections are done, plug in the USB serial monitor and run the following command to open the COM port and establish communication  

```
minicom -D /dev/ttyUSB0 -b 115200
```

### Now fire up the SoC!  
When at the U-boot prompt, press <kbd>Ctrl</kbd> + <kbd>C</kbd> to interrupt. It will show "Light LPI4A#"  
Execute ```loady 0x200000``` , this will start the ymodem protocol to transfer the file directly to the TH1520 at address
0x200000  
The Serial Monitor will send ```C C C``` at regular interval, this is normal and expected.  
Now press <kbd>Ctrt</kbd>+<kbd>A</kbd> <kbd>S</kbd> -> ```ymodem``` then navigate to "hello_boot.bin" and send it to the TH1520.  
Execute ```go 0x200000```  
  
### DONE!!  
  
  
Please refer to the uploaded image for reference.  
![hello_boot](reference.png)
