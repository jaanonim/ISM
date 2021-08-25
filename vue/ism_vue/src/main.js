import Vue from 'vue'
import App from './App.vue'
import './registerServiceWorker'
import VueSocketIO from 'vue-socket.io'
import SocketIO from 'socket.io-client'

Vue.config.productionTip = false
Vue.use(new VueSocketIO({
  debug: true,
  connection: SocketIO('http://192.168.0.27:5000'), 
  vuex: {
    actionPrefix: "SOCKET_",
    mutationPrefix: "SOCKET_"
  }
})
);

new Vue({
  render: h => h(App),
}).$mount('#app')
