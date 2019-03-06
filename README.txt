Rowan Maclachlan
rdm695 11165820
Sunday January 27th 2019
CMPT 434 Eager

Assignment 2

(1) TESTING

(2) DESIGN

(3) LIMITATIONS

(1) TESTING
    There are two executable files.  In the directory sender_dir one can find
    the executable 'sender-go-back-n' and 'sender-selective-repeat' for parts 1
    and 2 of the assignment.  The same goes for the directory listener_dir.

    The applications can be run as such:

    `./listener-go-back-n 10` to run the listener program with a 10% chance of dropping an
    ACK.  The message buffer size should be the same size as that given to the
    sender.  Run `./listener-selective-repeat` with an additional argument for
    the max 'r' value.

    `./sender-go-back-n <ip> 32000 10 8` OR
    `./sender-selective-repeat <ip> 32000 10 8` to run the sender program
    with an 10 second timeout and a window size of 8.  The port number used by
    the listener is 32000, and the ip given to the sender should be the hostname of the
    listener.


(2) DESIGN

    TIME-OUTS
        In the case of the go-back-n sliding window part 1 protocol,
        Time-outs are implementing using the stdlib "time" functions.  This
        function simply returns the number of seconds since 1970.  When a new
        message is sent, the time it was sent it recorded.  When the difference
        between the current time and the time the message was sent is greater
        than the user-provided timeout, a timeout error is raised and the
        message queue is re-sent.
        When the expected message acknowledgement is received, the timeout is
        reset and we begin waiting on the next expected acknowledgement.
        In the case of the selective-repeat part 2 protocol, message timeout
        only triggers a resend of the timed-out message, rather than the full
        frame.

    MESSAGE-BUFFERS
        In both implemented protocols, message buffers are implemented with a
        circular array in the file src/msg_queue.c.  This file contains all
        necessary API calls for the src/sender.c and src/listener.c files.
        Tests for this API are contained in the file src/test.c.
        For the go-back-n protocol, the listener has no message buffer.  The
        sender maintains a buffer which has a maximum size of the provided
        window size argument.
        In the selective-repeat protocol, the listener has a message buffer.
        Received messages are stored in sequence and certain message_queue.c API
        calls handle manipulating the array-backed list to remove the correct
        messages from the list and send the correct acknowledgement messages
        back to the sender.

    NON-BLOCKING IO
        Input is managed with the select function call.  When the message buffer
        is full, the sender will not accept additional messages through stdin.

(3) LIMITATIONS
    Individual messages do not have specific timeout constructs associated with
    them.  Instead, the timeout is reset every time the correct acknowledgement
    is received by the sender.  In practice, this means that some messages will
    have longer absolute timeouts than what was specified by the user.  The
    design of the message buffer used by the receiver with the selective-repeat
    protocol was overly complicated.  This is because the message queue used by
    the sender was recycled for the message buffer used by the listener.  A
    better, simpler approach would have been a simple array.
