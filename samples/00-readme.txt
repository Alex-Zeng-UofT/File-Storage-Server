## Sample Servers

All sample servers take an optional second argument (not required in the
assignment; I forgot, LOL): Initial serial number, so you can test starting with
a huge many-digit serial number instead of always starting from 0.  Example:
port 31415, initial serial number 19873163:

```
/path/to/server-foo 31415 19873163
```

server-normal: "Normal" and correct behaviour, but upon special user names, it
can send invalid serial numbers:

* "none": No serial number at all. EOF right away.
* "nan": Serial number has letters.
* "long": A 13-digit number.
* "noLF": No newline after (correct) serial number. EOF right after.

server-tartaglia: Technically correct behaviour. But guess what it does!


## Sample Clients

All sample clients do not check validity of the user name or the file name you
provide at the command line.  (Your client is not required to check either,
don't worry!)  You should exploit this to manufacture header errors, and test
your server for its header validation logic.  Example:

```
/path/to/client-foo 127.0.0.1 31415 kawaii9413^_^ /bin/ls
```

client-normal: "Normal" and correct behaviour.

client-tartaglia: Technically correct behaviour. But guess what it does!

client-shrinkflation: It claims a bigger file size (n*2+1 to be exact).  And
after sending the file, it exits (and closes the socket) without bothering to
wait for the serial number.  (Recall what your server should do to the saved
file in this case.)
