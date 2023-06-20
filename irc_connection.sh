#!/usr/bin/expect

set serverhost "localhost"
set port "4000"
set password "123"

spawn irssi
send "/connect $serverhost $port $password\r"
interact
