ssh pi@192.168.0.26 -p 777 ./send_frontend
cd E:\Mateusz\dodatki\PROGRAMOWANIE\ISM\ism_vue
call npm run build
scp -r -P 777 E:\Mateusz\dodatki\PROGRAMOWANIE\ISM\ism_vue\dist pi@192.168.0.26:~/ism/
cd ..
