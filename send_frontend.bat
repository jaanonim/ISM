ssh pi@192.168.1.150 -p 777 ./send_frontend
cd C:\Data\Programowanie\ISM\ism_vue
call npm run build
scp -r -P 777 C:\Data\Programowanie\ISM\ism_vue\dist pi@192.168.1.150:~/ism/
cd ..
