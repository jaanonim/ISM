import { io } from "socket.io-client";

import Vue from "vue";
import App from "./App.vue";
import VueSocketIO from "vue-socket.io";
import config from "./config.js";

console.log(config);

Vue.use(
    new VueSocketIO({
        debug: config.debug,
        connection: io(config.socketUrl, {
            auth: { token: localStorage.token },
        }),
        vuex: {
            actionPrefix: "SOCKET_",
            mutationPrefix: "SOCKET_",
        },
    })
);

new Vue({
    render: (h) => h(App),
}).$mount("#app");
