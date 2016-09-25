import RPi.GPIO as GPIO
import time
from nrf24 import NRF24

# Helpful links
# https://www.raspberrypi.org/forums/viewtopic.php?f=45&t=85504&p=723185
# https://github.com/jpbarraca/pynrf24

# Sensor parameters
# Using board mode, so the sensor is on GPIO4, pin 7
CONTROL_PIN = 4
# Threshold until we timeout and shut off the light
ON_OFF_THRESHOLD = 10
POLL_PERIOD = 0.1


def main():
    # Setup NRF24 radio
    pipes = [[0xc2, 0xc2, 0xc2, 0xc2, 0xc2], [0xc2, 0xc2, 0xc2, 0xc2, 0xc2]]
    radio = NRF24()
    radio.begin(0, 0, 17)
    radio.setRetries(15, 15)
    radio.setPayloadSize(32)
    radio.setChannel(0x60)
    radio.setDataRate(NRF24.BR_2MBPS)
    radio.setPALevel(NRF24.PA_MAX)
    radio.setAutoAck(True)
    radio.enableDynamicPayloads()
    radio.enableAckPayload()
    radio.openWritingPipe(pipes[0])
    radio.openReadingPipe(1, pipes[0])
    radio.startListening()
    radio.stopListening()
    radio.printDetails()
    radio.startListening()

    GPIO.setmode(GPIO.BCM)  # set board mode to Broadcom
    GPIO.setup(CONTROL_PIN, GPIO.OUT)  # set up pin 17

    try:
        count = 0
        while True:
            pipe = [0]
            # Wait for incoming packet from transmitter
            while not radio.available(pipe):
                time.sleep(POLL_PERIOD)
                count += 1
                if count > ON_OFF_THRESHOLD:
                    # Turn off
                    GPIO.output(CONTROL_PIN, False)
            recv_buffer = []
            radio.read(recv_buffer, radio.getDynamicPayloadSize())
            if recv_buffer[0] == 1:
                count = 0
                GPIO.output(CONTROL_PIN, True)
            else:
                print 'Unknown command'

    except KeyboardInterrupt:
        GPIO.cleanup()

if __name__ == '__main__':
    main()
