from astral import Astral
import datetime
from time import sleep
from sys import exit
try:
    import RPi.GPIO as GPIO
except RuntimeError:
    exit("Error importing RPi.GPIO! Try to run it as root!")

class GpioOnSunSchedule:
    def __init__(self, city, gpio_pin, value_during_sun=GPIO.LOW, default_value=GPIO.LOW, interval=60, debug=False):
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
        GPIO.setmode(GPIO.BOARD)
        # Save parameters for later usage
        self.gpio_pin = gpio_pin
        self.value_during_sun = value_during_sun
        self.default_value = default_value
        self.interval = interval # seconds
        self.debug = debug
    def run(self):
        '''
        'run' forever and set given GPIO pin to suitable value according to whether sun is up or not.
        '''
        current_value = self.default_value
        if self.debug:
            print("Setup GPIO pin {0} to OUT with initial value {1}.", self.gpio_pin, current_value)
        GPIO.setup(self.gpio_pin, GPIO.OUT, initial=current_value)
        while True:
            # loop forever
            sun = self.city.sun(date=datetime.date.today(), local=True)
            sunrise, sunset = sun['sunrise'], sun['sunset']
            now = datetime.datetime.now(tz=sunrise.tzinfo)
            if self.debug:
                print("Today is {0}", sun)
                print("Sunrise at {0}", sunrise)
                print("Sunset at {0}", sunset)
                print("Now is {0}", now)
            if now > sunrise and now < sunset:
                # sun is in the sky
                if self.debug:
                    print("Sun is in the sky! GPIO pin #{0} is {1}.", self.gpio_pin, current_value)
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

if __name__ == '__main__':
    kitchen_window_light = GpioOnSunSchedule('Stockholm', 10, value_during_sun=GPIO.LOW, default_value=GPIO.HIGH, interval=30, debug=True)
    kitchen_window_light.run()
