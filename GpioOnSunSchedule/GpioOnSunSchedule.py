from astral import Astral
import datetime
from time import sleep
from sys import exit
try:
    import RPi.GPIO as GPIO
except RuntimeError:
    exit("Error importing RPi.GPIO! Try to run it as root!")

class GpioOnSunSchedule:
    def __init__(self, city, gpio_pin, value_during_sun=GPIO.LOW, debug=False):
        self.astral = Astral()
        self.astral.solar_depression = 'civil'
        try:
            self.city = self.astral[city]
        except KeyError as err:
            exit('%s is not in Astral database. Try your capital city.' % city)
        self.gpio_pin = gpio_pin
        self.value_during_sun = value_during_sun
        GPIO.setmode(GPIO.BOARD)
        self.interval = 60 # seconds
        self.debug = debug
    def run(self):
        current_value = self.value_during_sun
        if self.debug:
            print("Setup GPIO pin {0} to OUT with initial value {1}.", self.gpio_pin, current_value)
        GPIO.setup(self.gpio_pin, GPIO.OUT, initial=current_value)
        while True:
            # loop forever
            sun = self.city.sun(date=datetime.date.today(), local=True)
            sunrise, sunset = sun['sunrise'], sun['sunset']
            now = datetime.datetime.now(tz=sunrise.tzinfo)
            if self.debug:
                print("Sunrise at {0}", sunrise)
                print("Sunset at {0}", sunset)
                print("Now is {0}", now)
            if now > sunrise and now < sunset:
                # sun is in the sky
                if self.debug:
                    print("Sun is in the sky! GPIO pin #{1} is {0}.", current_value, self.gpio_pin)
                if current_value != self.value_during_sun:
                    current_value = self.value_during_sun
                    GPIO.output(self.gpio_pin, current_value)
                    if self.debug:
                        print("Switch GPIO pin #{0} to {1}.", self.gpio_pin, current_value)
            else:
                # sun is down
                if self.debug:
                    print("Sun is down! GPIO pin #{0} is {1}.", self.gpio_pin, current_value)
                if current_value == self.value_during_sun:
                    current_value = not self.value_during_sun
                    GPIO.output(self.gpio_pin, current_value)
                    if self.debug:
                        print("Switch GPIO pin #{0} to {1}.", self.gpio_pin, current_value)
            if self.debug:
                print("Sleep {0} seconds...", self.interval)
            sleep(self.interval)
