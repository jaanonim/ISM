ssh pi@192.168.0.26 -p 777 ./updateVue
scp -r -P 777 E:\Mateusz\dodatki\PROGRAMOWANIE\ISM\vue\ism_vue\dist pi@192.168.0.26:~/ism/
pause