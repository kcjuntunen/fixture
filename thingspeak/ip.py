import socket
import fcntl
import struct
import smtplib
import json

def send_email(subj, ip_addr):
    print "Attempting to send email"
    try:
        sender = config_data["sender"]
        receivers = ['rvansluyters@amstore.com', 'juntunen.kc@gmail.com']
        message = """From: Raspberry Pi <no_real_email@nobody.com>
To: Ryan <rvansluyters@amstore.com>, K. C. <juntunen.kc@gmail.com>
Subject: """

        message += subj + "\n\n"
        message += "Pi started. IP: " + ip_addr + "\n"
        smtpo = smtplib.SMTP(config_data["smtp_server"])
        smtpo.sendmail(sender, receivers, message)
        print "Sent email"
    except Exception, e:
        print "Failure sending email. %s" % e

def get_ip_address(ifname):
  s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
  return socket.inet_ntoa(fcntl.ioctl(
    s.fileno(),
    0x8915,  #SIOCGIFADDR
    struct.pack('256s', ifname[:15]))[20:24])

def broadcast_ip():
  global config_data
  with open('/etc/thingspeak.json') as data_file:
    config_data = json.load(data_file)

  send_email(config_data["unit"] + " booted.", get_ip_address('wlan0'))
  

if __name__ == "__main__":
  global config_data
  with open('/etc/thingspeak.json') as data_file:
    config_data = json.load(data_file)

  send_email(config_data["unit"] + " booted.", get_ip_address('wlan0'))
