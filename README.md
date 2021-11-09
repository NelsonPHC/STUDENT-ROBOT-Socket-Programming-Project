# STUDENT-ROBOT-Socket-Programming-Project

Basic Programming part - Codes in student.cc (STUDENT)
### Step 1 and 2
Create a socket ListenSocket and connect to the ROBOT TCP Port 3310.  
![image](https://user-images.githubusercontent.com/92118991/140870762-22f4f886-1f57-4143-a59a-9b0845413ca3.png)

Send 10 char student ID string via ListenSocket.  
![image](https://user-images.githubusercontent.com/92118991/140870789-93784fed-df43-408d-85ed-d7cb04a6edf0.png)

### Step 3
Receive a 5 char string “ddddd” from ROBOT and change it to int type.  
![image](https://user-images.githubusercontent.com/92118991/140870807-eac9f184-ffab-4fde-b291-a8adf35c3dc9.png)

Create a TCP socket s_2 at port ddddd to accept a new connection.  
![image](https://user-images.githubusercontent.com/92118991/140870849-b5ad2dec-3fb2-46fe-80a6-212ca9c92d08.png)

A new socket s2 is returned upon accepting the connection.  
![image](https://user-images.githubusercontent.com/92118991/140870868-d7c82436-e419-47d7-8326-2394c9cf1f42.png)

### Step 4
Receive “fffff,eeeee.”, decode it, and change it to type int.  
![image](https://user-images.githubusercontent.com/92118991/140870956-8b04cd62-8ea9-4cae-aa5b-27a37cd9abb8.png)

Send num to ROBOT and receive the char string xxx with length num*10 using s3 on port eeeee.  
![image](https://user-images.githubusercontent.com/92118991/140870963-614f887c-5cfa-46e9-acd9-5ecd7b14c965.png)


## Step 5
Send back the string xxx to the ROBOT at UDP port fffff.  
![image](https://user-images.githubusercontent.com/92118991/140870988-a8352256-e410-4e52-a3fe-dd8ef1bd1cc5.png)

## Experiment part

From the relation between the receiver buffer size and the system throughput as shown below, we can see that system throughput is improved as we increase the receive buffer size on STUDENT from 2500 bytes to 10000 bytes. This is because when transmission rate is high relative to the receive buffer size, the receiver will throw away packets once the receive buffer is full. When we increase the size of the receive buffer, more packets can be received by the receiver before the receive buffer is full. Therefore, the system throughput increases as the receive buffer increases in our experiment.  
![image](https://user-images.githubusercontent.com/92118991/140871005-3e46f50c-0b9a-43e0-ad8e-a954ca40eb47.png)

 
X-axis: receive buffer size  
Y-axis: System throughput  

If the network is a high-speed one (the transmission rate is high), the thing that will limit the throughput is the receive buffer size as we discussed above.
If the network is a low-speed one, it means the transmission rate is low. With low transmission rate, the system throughput still increases as small receive buffer size increases in the beginning. However, after the receive buffer size reaches a certain size, the system throughput will remain the same (we will see a flat curve up to a certain receive buffer size). This is because the transmission rate is low relative to the receive buffer size. No matter how we further increase the receive buffer size, the number of bytes being received (and thus the system throughput) remains the same.
Therefore, if the network is a low-speed one (the transmission rate is low), the thing that will limit the throughput is the transmission rate.
