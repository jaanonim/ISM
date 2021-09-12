ssh pi@192.168.0.26 -p 777 ./send_backend
scp -P 777 E:\Mateusz\dodatki\PROGRAMOWANIE\ISM\server\app.py pi@192.168.0.26:~/ism/server/app.py
scp -P 777 E:\Mateusz\dodatki\PROGRAMOWANIE\ISM\server\server.py pi@192.168.0.26:~/ism/server/server.py
scp -P 777 E:\Mateusz\dodatki\PROGRAMOWANIE\ISM\server\user.py pi@192.168.0.26:~/ism/server/user.py
scp -P 777 E:\Mateusz\dodatki\PROGRAMOWANIE\ISM\server\requirements.txt pi@192.168.0.26:~/ism/server/requirements.txt
pause