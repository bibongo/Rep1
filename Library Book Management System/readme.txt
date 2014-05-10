The application is builded in Python 2.7 language with Python wx library for GUI support and Python mysql connector.

Python wx library can be installed on debian linux using 
>> sudo apt-get install python-wxgtk2.6

Python mysql-connector can be installed as
>> sudo apt-get install mysql-connector-python

Running the code:
The code is written in one single file named “libapp.py”. 
It can be executed by giving three command line arguments: 
•	The host IP address which has the mysql-server running 
•	The user name having alter privileges on library database
•	The user's password for the same
Example:
>> python libapp.py 127.0.0.1 admin secretkey
