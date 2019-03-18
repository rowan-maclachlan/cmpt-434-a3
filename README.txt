Rowan Maclachlan
rdm695 11165820
Monday March 18th 2019
CMPT 434 Eager

Assignment 3

(1) TESTING

(2) DESIGN

(3) LIMITATIONS

(4) PART B

(1) TESTING
   
    There are two executable files: `sensor_node` and `logger`.

    Their use is as follows:
    `./sensor_node <id> <payload> <D> <listening_port> <logger_hostname> <logger_port>`
    `./logger <port_number> <T> <number of sensors>` 

    The logger should be started first.  It will wait for incoming connections
    from sensor nodes.  The `port_number` should be the port number that the
    logger will use to listen for connections.  `T` is the transmission range.
    The number of sensors used should be equal to the number of sensors launched
    if the simulation is to run correctly.

    The sensor nodes can be started after.  Their IDs should be unique, and
    between the values 0 and `number_of_sensors - 1`. `D` is the distance a
    sensor moves every tick.  The `listening_port` is which port the sensor will
    listen for incoming connections from other sensors on.  The
    `logger_hostname` and `logger_port` must correspond to the hostname of the
    logger and the port number argument provided.

    To run these processes on the same system, commands such as
    `./logger 32000 100 10`
    and
    `for i in {1..10}; do; ./sensor $i HELLO 50 3300$i 127.0.0.1 32000 &; done`
    could be used.

    A sensor stops running after successfully delivering its packet to the
    logger, or if it fails to connect to the logger.
    The logger will run until it has received every sensor's packet.

(2) DESIGN

    see DESIGN.txt

(3) LIMITATIONS
    
    see DESIGN.txt

(4) PART B
    
    submission for part B can be found in the file ./p2/a3.pdf


