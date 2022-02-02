ssh pi@192.168.0.26 -p 777 ./send_frontend
cd C:\Data\Programowanie\ISM
call npm run build
scp -r -P 777 C:\Data\Programowanie\ISM\ism_vue\dist pi@192.168.0.26:~/ism/
cd ..
