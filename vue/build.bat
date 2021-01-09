docker buildx build --platform linux/amd64,linux/arm -t jaanonim/ism-rasberry --push .
docker run -d --name flask-vue -e "PORT=8765" -p 8000:8765 web:latest