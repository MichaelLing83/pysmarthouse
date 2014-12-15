import webiopi
import datetime
import pygal
import db

GPIO = webiopi.GPIO

LIGHT = 17 # GPIO pin using BCM numbering

HOUR_ON  = 8  # Turn Light ON at 08:00
HOUR_OFF = 18 # Turn Light OFF at 18:00

# setup function is automatically called at WebIOPi startup
def setup():
    database = db.DB()
    for i in range(10):
        database.insert(db.ALL_IDS[0], db.ALL_TYPES[0], float(i*2+1), datetime.datetime.now()+datetime.timedelta(seconds=i))
    temperatures = list()
    for id, t, v, ts in database.get_records():
        temperatures.append( (ts, v), )
        #print("{:16s}{:16s}{:8.2f}\t{:10s}".format(id, t, v, str(ts)))
    print(temperatures)
    datey = pygal.DateY(x_label_rotation=20, range=(0, 25), explicit_size=True, width=500, height=400)
    datey.add(db.ALL_TYPES[0], temperatures)
    #datey.add("Visits", [
    #    (datetime.datetime(2013, 1, 2), 300),
    #    (datetime.datetime(2013, 1, 12), 412),
    #    (datetime.datetime(2013, 2, 2), 823),
    #    (datetime.datetime(2013, 2, 22), 672)])
    datey.render_to_file('bar_chart.svg')
    # bar_chart = pygal.Bar()                                            # Then create a bar graph object
    # bar_chart.add('Fibonacci', [0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55])  # Add some values
    # bar_chart.render_to_file('bar_chart.svg')
    # set the GPIO used by the light to output
    GPIO.setFunction(LIGHT, GPIO.OUT)

    # retrieve current datetime
    now = datetime.datetime.now()

    # test if we are between ON time and tun the light ON
    if ((now.hour >= HOUR_ON) and (now.hour < HOUR_OFF)):
        GPIO.digitalWrite(LIGHT, GPIO.HIGH)

# loop function is repeatedly called by WebIOPi 
def loop():
    # retrieve current datetime
    now = datetime.datetime.now()

    # toggle light ON all days at the correct time
    if ((now.hour == HOUR_ON) and (now.minute == 0) and (now.second == 0)):
        if (GPIO.digitalRead(LIGHT) == GPIO.LOW):
            GPIO.digitalWrite(LIGHT, GPIO.HIGH)

    # toggle light OFF
    if ((now.hour == HOUR_OFF) and (now.minute == 0) and (now.second == 0)):
        if (GPIO.digitalRead(LIGHT) == GPIO.HIGH):
            GPIO.digitalWrite(LIGHT, GPIO.LOW)

    # gives CPU some time before looping again
    webiopi.sleep(1)

# destroy function is called at WebIOPi shutdown
def destroy():
    GPIO.digitalWrite(LIGHT, GPIO.LOW)

