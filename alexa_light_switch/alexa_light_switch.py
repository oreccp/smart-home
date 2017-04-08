import logging

from random import randint

from flask import Flask, render_template

from flask_ask import Ask, statement, question, session
import RPi.GPIO as GPIO
# Using board mode, so the sensor is on GPIO4, pin 7
CONTROL_PIN = 4
GPIO.setmode(GPIO.BCM)  # set board mode to Broadcom
GPIO.setup(CONTROL_PIN, GPIO.OUT)  # set up pin 17
toggle = False

app = Flask(__name__)

ask = Ask(app, "/")

logging.getLogger("flask_ask").setLevel(logging.DEBUG)


@ask.launch

def new_game():
    welcome_msg = render_template('welcome')
    return question(welcome_msg)


@ask.intent("TurnOn")
def turn_on():
    GPIO.output(CONTROL_PIN, True)
    msg = render_template('turn_on')
    return statement(msg)


@ask.intent("TurnOff")
def turn_off():
    GPIO.output(CONTROL_PIN, False)
    msg = render_template('turn_off')
    return statement(msg)


if __name__ == '__main__':
    try:
        app.run(debug=True)
    except KeyboardInterrupt:
        GPIO.cleanup()
