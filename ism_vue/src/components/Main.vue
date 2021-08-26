<template>
  <div>
    <button @click="update" class="update">Update</button>
    <button @click="login" class="update">Zaloguj</button>
    <div class="status_block">
      Połączenie z serwerem:
      <div class="status" :class="{ on: isConnected }"></div>
    </div>
    <div class="blocks">
      <Block
        @popup="popup"
        :page="page"
        :block="this.blocks[0]"
        :isInactive="isInactive"
        ref="block1"
      />
      <Block
        @popup="popup"
        :page="page"
        :block="this.blocks[1]"
        :isInactive="isInactive"
        ref="block2"
      />
      <Block
        @popup="popup"
        :page="page"
        :block="this.blocks[2]"
        :isInactive="isInactive"
        ref="block3"
        v-if="page === 0"
      />
    </div>
    <popup :text="popupText" :style="popupColorF"></popup>
    <login @loginDone="loginDone" v-if="isLogin"></login>
  </div>
</template>

<script>
import axios from "axios";
import popup from "./popup.vue";
import Block from "./Block.vue";
import login from "./login.vue";

export default {
  name: "Main",
  props: ["blocks", "page"],
  data() {
    return {
      popupText: "",
      popupColor: "rgba(0,0,0,0)",
      active: false,

      isLogin: false,
      isConnected: false,
    };
  },
  components: {
    Block,
    popup,
    login,
  },
  sockets: {
    connect: function () {
      console.log("socket connected");
      this.isConnected = true;
    },
    disconnect: function () {
      console.log("socket disconnected");
      this.isConnected = false;
    },
    set: function (data) {
      console.log("set", data);
      if (data.payload == null) {
        this.popup("");
      } else {
        this.popup(data.name + ": " + data.payload.error);
      }
    },
  },
  computed: {
    popupColorF: function () {
      var s = "hidden";
      if (this.active) {
        s = "visible";
      }
      return {
        "--popup-color": this.popupColor,
        visibility: s,
      };
    },
  },
  mounted: function () {
    this.update();
  },
  methods: {
    update() {
      this.$refs.block1.getData();
      this.$refs.block2.getData();
      this.$refs.block3.getData();
    },
    login() {
      this.isLogin = true;
    },
    async loginDone(v) {
      this.isLogin = false;
      this.active = true;
      this.popupText = "Logowanie ...";
      this.popupColor = "rgba(40,125,255,0.75)";
      try {
        await axios
          .post(
            window.location.origin + "/api/auth",
            {
              username: v.username,
              password: v.password,
            },
            { headers: { "Content-Type": "application/json" } }
          )
          .then((response) => {
            localStorage.token = response.data["access_token"];
          });
      } catch (e) {
        this.active = true;
        this.popupText = e;
        this.popupColor = "rgba(255,0,0,0.75)";
        setTimeout(() => {
          this.active = false;
        }, 2000);
        return;
      }
      this.active = true;
      this.popupText = "Zalogowao!";
      this.popupColor = "rgba(0,255,0,0.75)";
      setTimeout(() => {
        this.active = false;
        window.history.go();
      }, 1000);
      this.update();
      return;
    },
    popup(v) {
      this.active = true;
      if (v == "") {
        this.popupText = "Zapisano!";
        this.popupColor = "rgba(0,255,0,0.75)";
      } else {
        this.popupText = v;
        this.popupColor = "rgba(255,0,0,0.75)";
      }
      setTimeout(() => {
        this.active = false;
      }, 2000);
    },
  },
};
</script>

<style>
.blocks {
  display: flex;
  flex-direction: row;
  flex-wrap: wrap;
}
.status_block {
  display: inline-block;
  margin: 0.2rem;
  vertical-align: middle;
  margin: 10px;
  background-color: var(--presed-color);
  border-radius: 5px;
  padding: 0.55rem 1rem;
}
</style>