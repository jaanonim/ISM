ssh pi@192.168.1.150 -p 777 ./send_backend
scp -P 777 C:\Data\Programowanie\ISM\server\app.py pi@192.168.1.150:~/ism/server/app.py
scp -P 777 C:\Data\Programowanie\ISM\server\server.py pi@192.168.1.150:~/ism/server/server.py
scp -P 777 C:\Data\Programowanie\ISM\server\data.py pi@192.168.1.150:~/ism/server/data.py
scp -P 777 C:\Data\Programowanie\ISM\server\user.py pi@192.168.1.150:~/ism/server/user.py
scp -P 777 C:\Data\Programowanie\ISM\server\requirements.txt pi@192.168.1.150:~/ism/server/requirements.txt
