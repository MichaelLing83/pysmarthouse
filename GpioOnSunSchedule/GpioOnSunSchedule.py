#!/usr/bin/env python3
"A class to keep GPIO pin level according to sunlight calculation."

import datetime
import signal
from time import sleep
from sys import exit
import atexit
import argparse
from astral import Astral
try:
    import RPi.GPIO as GPIO
except RuntimeError:
    exit("Error importing RPi.GPIO! Try to run it as root!")

class GpioOnSunSchedule:
    def __init__(self, city, gpio_pin, value_during_sun=False, default_value=True, interval=60, debug=False):
        '''
        Initialize a GpioOnSunSchedule object, with
            @city(String):  name of your city
            @gpio_pin(Integer): GPIO output pin number on Raspberry Pi board (by board numbering)
            @value_during_sun:  GPIO.LOW or GPIO.HIGH (False or True). Value of given pin when sun is in sky.
            @default_value: GPIO.LOW or GPIO.HIGH (False or True). Default value when GPIO pin is initialized. This can be used to avoid
                            jitter (e.g. for low-triggering relays)
            @interval(Integer): time interval in seconds between two adjacent check.
            @debug(Boolean):    whether to print debug information
        '''
        # Initialize Astral for sunrise/sunset time calculation
        self.astral = Astral()
        self.astral.solar_depression = 'civil'
        try:
            self.city = self.astral[city]
        except KeyError as err:
            exit('%s is not in Astral database. Try your capital city.' % city)
        # Initialize GPIO
        GPIO.setwarnings(False)
        GPIO.setmode(GPIO.BOARD)
        # Save parameters for later usage
        self.gpio_pin = gpio_pin
        self.value_during_sun = value_during_sun
        self.default_value = default_value
        self.interval = interval # seconds
        self.debug = debug
    def __run(self):
        '''
        'run' forever and set given GPIO pin to suitable value according to whether sun is up or not.
        '''
        current_value = self.default_value
        if self.debug:
            print("Setup GPIO pin {0} to OUT with initial value {1}.".format(self.gpio_pin, current_value))
        GPIO.setup(self.gpio_pin, GPIO.OUT, initial=current_value)
        while True:
            # loop forever
            sun = self.city.sun(date=datetime.date.today(), local=True)
            sunrise, sunset = sun['sunrise'], sun['sunset']
            now = datetime.datetime.now(tz=sunrise.tzinfo)
            if self.debug:
                print("Today is {0}".format(sun))
                print("Sunrise at {0}".format(sunrise))
                print("Sunset at {0}".format(sunset))
                print("Now is {0}".format(now))
            if now > sunrise + datetime.timedelta(hours=0.5) and now < sunset - datetime.timedelta(hours=0.5):
                # sun is in the sky
                if self.debug:
                    print("Sun is in the sky! GPIO pin #{0} is {1}.".format(self.gpio_pin, current_value))
                if current_value != self.value_during_sun:
                    current_value = self.value_during_sun
                    GPIO.output(self.gpio_pin, current_value)
                    if self.debug:
                        print("Switch GPIO pin #{0} to {1}.".format(self.gpio_pin, current_value))
            else:
                # sun is down
                if self.debug:
                    print("Sun is down! GPIO pin #{0} is {1}.".format(self.gpio_pin, current_value))
                if current_value == self.value_during_sun:
                    current_value = not self.value_during_sun
                    GPIO.output(self.gpio_pin, current_value)
                    if self.debug:
                        print("Switch GPIO pin #{0} to {1}.".format(self.gpio_pin, current_value))
            if self.debug:
                print("Sleep {0} seconds...".format(self.interval))
            sleep(self.interval)
    def run(self):
        try:
            self.__run()
        except KeyboardInterrupt:
            pass
        finally:
            self.clean()
    def clean(self):
        GPIO.cleanup()

def handler(signum, frame):
    o = frame.f_globals['gpioOnSunSchedule']
    o.clean()

signal.signal(signal.SIGTERM, handler)

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("--debug", help="Enable debug output", action="store_true")
    parser.add_argument("-c", "--city", help="Nearest capital city name", type=str, default="Stockholm")
    parser.add_argument("-p", "--pin", help="GPIO pin number (BOARD mode) for output", type=int, required=True)
    parser.add_argument("-s", "--sun_value", help="GPIO pin output during sun is up", type=bool, default=True)
    parser.add_argument("-d", "--default_value", help="GPIO pin default output value", type=bool, default=True)
    parser.add_argument("-i", "--interval", help="Sleep time before wake up again to change GPIO output", type=int, default=300)
    args = parser.parse_args()
    if args.debug:
        print("city is {0}".format(args.city))
        print("GPIO pin is #{0}".format(args.pin))
        print("sun_value is {0}".format(args.sun_value))
        print("default_value is {0}".format(args.default_value))
    gpioOnSunSchedule = GpioOnSunSchedule(args.city,
                                            args.pin,
                                            value_during_sun=args.sun_value,
                                            default_value=args.default_value,
                                            interval=args.interval,
                                            debug=args.debug)
    gpioOnSunSchedule.run()
