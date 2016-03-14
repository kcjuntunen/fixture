import sqlite3 as sql
import sys, datetime
# CREATE TABLE IF NOT EXISTS snapshot_log (id INTEGER PRIMARY KEY ASC, timestamp, obstruction, reed_switch1, reed_switch2, light_level, humidity, temperature, pressure, light_threshold);

class sqlite_writer:
  def __init__(self, db_filename = "./amstore.db"):
    self.db_filename = db_filename

    try:
      self.conn = sql.connect(db_filename)
    except sql.Error, e:
      print "Error %s:" % e.args[0]
      sys.exit(1)
      
    try:
      if not len(self.show_tables()):        
        self.create_table()        
    except sql.Error, e:
      print "Error %s:" % e.args[0]
      sys.exit(1)

  def __del__(self):
    self.conn.close()
    
  def get_db_version(self):
    with self.conn:
      cur = self.conn.cursor()
      cur.execute('SELECT SQLITE_VERSION()')    
      data = cur.fetchone()
      return "SQLite version: %s" % data

  def show_tables(self):
    with self.conn:
      cur = self.conn.cursor()
      cur.execute('select name from sqlite_master where type = \'table\';')
      return cur.fetchall()

  def print_tables(self):
    print "------------------\nTables\n------------------"
    with self.conn:
      cur = self.conn.cursor()
      cnt = 0
      for x in self.show_tables():
        cnt += 1
        print ("{0}.) {1}".format(cnt, x[0]))
        
  def create_table(self):
    with self.conn:
      cur = self.conn.cursor()
      cur.execute('CREATE TABLE IF NOT EXISTS snapshot_log (id INTEGER PRIMARY KEY ASC, timestamp, obstruction, reed_switch1, reed_switch2, light_level, humidity, temperature, pressure, light_threshold);')
    
  def insert_data(self, obstruct, reed1, reed2, ll, hum, temp, press, thresh):    
    with self.conn:
      cur = self.conn.cursor()
      sql = "INSERT INTO snapshot_log (timestamp, obstruction, reed_switch1, reed_switch2, light_level, humidity, temperature, pressure, light_threshold) VALUES (datetime(\'now\'), {0}, {1}, {2}, {3}, {4}, {5}, {6}, {7});".format(obstruct, reed1, reed2, ll, hum, temp, press, thresh)
      cur.execute(sql)


  def insert_data_old(self, obstruct, reed1, reed2, ll, hum, temp):    
    with self.conn:
      cur = self.conn.cursor()
      sql = "INSERT INTO snapshot_log (timestamp, obstruction, reed_switch1, reed_switch2, light_level, humidity, temperature) VALUES (datetime(\'now\'), {0}, {1}, {2}, {3}, {4}, {5});".format(obstruct, reed1, reed2, ll, hum, temp)

      #, " + obstruct + ", " + reed1 + "\', \'" + reed2 + "\', \'" + ll + "\', \'" + hum + "\', \'" + temp + "\';"

      try:
        cur.execute(sql)
      except:
        print ("Couldn't insert\nsql={0}".format(sql))

  def insert_ser_line(self, line):
    data = str.split(line, ":")
    if len(data) < 8:
      print "%s doesn't have enough fields." % line
    else:
      self.insert_data(data[1], data[2], data[3], data[4],
                       data[5], data[6], data[7], data[8])

  def insert_ser_line_old(self, line):
    data = str.split(line, ":")
    if len(data) < 6:
      print "%s doesn't have enough fields." % line
    else:
      try:
        self.insert_data_old(data[1], data[2], data[3],
                             data[4], data[5], data[6])
      except:
        print "Error inserting data."

if __name__ == "__main__":
  db = sqlite_writer('1.db')
  print db.get_db_version()
  db.print_tables()
