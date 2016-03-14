DAY, NIGHT = 1, 2

def check_time(time_to_check, on_time, off_time):
    if on_time > off_time:
        if time_to_check > on_time or time_to_check < off_time:
            return NIGHT, True
    elif on_time < off_time:
        if time_to_check > on_time and time_to_check < off_time:
            return DAY, True
    elif time_to_check == on_time:
        return None, True

    return None, False

  

    # now = datetime.datetime.now().time()
    # on_time = datetime.time(14, 15)
    # off_time = datetime.time(21, 30)
    # when, matching = check_time(now, on_time, off_time)
    # should_I_send = False
    # if matching:
    #     if when == DAY:
    #         print "Daytime; time to send email."
    #         should_I_send = True
    #     elif when == NIGHT:
    #         print "Not a good time for email."
    #     else:
    #         print "WTH?"
    # else:
    #     print now
    #     print when

    # if (float(line[4]) < -700) and should_I_send:
    #     send_email()
