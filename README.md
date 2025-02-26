# USBFatFSTest

This is a working example of reading and writing files from the external USB on the Daisy Patch SM. 

External USB drive is connected to pins A8 and A9. 

Notes: 

- The file reference has to be defined globally so that it goes in the heap. 
- You must register the class once connected. In this example, it's done in the USBConnectCallback. 
