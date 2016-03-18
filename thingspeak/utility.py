import datetime

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

def ok_to_send(day_start, day_end):
    now = datetime.datetime.now().time()
    dstart = str.split(day_start, ":")
    dend = str.split(day_end, ":")
    on_time = datetime.time(int(dstart[0]), int(dstart[1]))
    off_time = datetime.time(int(dend[0]), int(dend[1]))
    when, matching = check_time(now, on_time, off_time)
    should_I_send = False
    if matching:
        if when == DAY:
            return True
        elif when == NIGHT:
            return False
        else:
            return False
    else:
        return False
