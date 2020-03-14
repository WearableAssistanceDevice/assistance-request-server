# assistance-request-server

## Setup
Download SEGGER Embedded Studio [here](https://www.segger.com/products/development-tools/embedded-studio/)

Download the nRF5 SDK [here](https://www.nordicsemi.com/Software-and-tools/Software/nRF5-SDK)

Clone this repo into the folder `${NRF_SDK_DIR}/projects/server/`,  where `NRF_SDK_DIR` is the nRF5 SDK folder.

## Usage
The server will advertise itself and wait for a connection by the wearable device. When a device connects and the server finds the Assistance Request Service on the device, the server will read the value of the assistance request characteristic. If the value is `true`, the LED indicated by `ASSISTANCE_REQUEST_LED` will light up. Pressing the button indicated by `ASSISTANCE_REQUEST_ACK_BUTTON` will turn off the LED.

The software for the wearable device can be found here: https://github.com/WearableAssistanceDevice/assistance-device
