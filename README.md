# LogNController Product Design
#### Client Name: Cooper Scarbrough-

## Project Requirements
- The board will be powered using a 120V or 220V AC input and should have adequate regulation on board for required inputs and outputs.
- The system should use an ESP32 as the main controller. There is sufficient space for the PCB so choosing a development kit is also an option but not advised.
- The system will need following inputs:
    - Oxygen sensor input (oxygen meter to 1 decimal (19.5mg/l))
    - pH sensor input (pH sensor to 1 decimal (7.2))
    - 2 x Analog inputs (5V)
- The system will have following outputs
    - 3 x 12V DC Pumps (5A each)
    - 1 x Air pump (110/120VAC 15A)
    - 1 x Air pump (110/120VAC 3A)
    - 1 x AC Motor (120VAC 9A)
    - 1 x UV Light (120VAC ~5A)
- The system must also feature a display (3. 4”x3” **or** 3”x2”) connected to ESP32 that is interactive via buttons. Below are the buttons and required functionality of each button:
    - LCD Home Page needs to show:
        - Script name
        - pH
        - Oxygen content
        - Duration
    - System Buttons:
        - ON Button
        - OFF Button
    - LCD Buttons
        - Scroll up
        - Scroll Down
        - Select
        - Back / Home
        - Start
        - Stop

- Example of button:
![image](https://github.com/eSpark-Consultants/CooperScarbrough-LogNController/assets/76909080/75eee1ca-a897-4805-a291-8567a369fa0c)

### System Diagram
![image](https://github.com/eSpark-Consultants/CooperScarbrough-LogNController/assets/76909080/544034c5-9355-4f07-bbbc-2e4d017d4973)
