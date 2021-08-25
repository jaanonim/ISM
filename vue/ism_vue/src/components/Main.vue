<template>
  <div>
    <button @click="update" class="update">Update</button>
    <button @click="login" class="update">Zaloguj</button>
    <div class="blocks">
      <Block
        @popup="popup"
        :page="page"
        :block="this.blocks[0]"
        :isInactive="isInactive"
        :token="token"
        ref="block1"
      />
      <Block
        @popup="popup"
        :page="page"
        :block="this.blocks[1]"
        :isInactive="isInactive"
        :token="token"
        ref="block2"
      />
      <Block
        @popup="popup"
        :page="page"
        :block="this.blocks[2]"
        :isInactive="isInactive"
        :token="token"
        ref="block3"
        v-if="page === 0"
      />
      <localSettings
        @popup="popup"
        @setInactive="setInactive"
        v-if="page === 1"
      ></localSettings>
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
import localSettings from "./settings/localSettings.vue";

export default {
  name: "Main",
  props: ["blocks", "page"],
  data() {
    return {
      inactiveTime: localStorage.getItem("inactive"),

      popupText: "",
      popupColor: "rgba(0,0,0,0)",
      active: false,

      isInactive: false,
      userActivityThrottlerTimeout: null,
      userActivityTimeout: null,

      token: localStorage.token,
      isLogin: false,
    };
  },
  components: {
    Block,
    localSettings,
    popup,
    login,
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
            window.location.origin + this.blocks[2].endpoint + "/auth",
            {
              username: v.username,
              password: v.password,
            },
            { headers: { "Content-Type": "application/json" } }
          )
          .then((response) => {
            this.token = response.data["access_token"];
            localStorage.token = this.token;
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
      }, 2000);
      this.update();
      return;
    },
    popup(v) {
      this.active = true;
      if (v === "") {
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
    setInactive(v) {
      this.inactiveTime = v;
    },

    resetUserActivityTimeout() {
      clearTimeout(this.userActivityTimeout);
      this.userActivityTimeout = setTimeout(() => {
        this.inactiveUserAction();
      }, this.inactiveTime * 1000);
    },
    activateActivityTracker() {
      window.addEventListener("mousemove", this.userActivityThrottler);
      window.addEventListener("mousemove", this.resetUserActivityTimeout);
      window.addEventListener("scroll", this.resetUserActivityTimeout);
      window.addEventListener("keydown", this.resetUserActivityTimeout);
      window.addEventListener("resize", this.resetUserActivityTimeout);
    },
    userActivityThrottler() {
      if (!this.userActivityThrottlerTimeout) {
        this.userActivityThrottlerTimeout = setTimeout(() => {
          this.resetUserActivityTimeout();

          clearTimeout(this.userActivityThrottlerTimeout);
          this.userActivityThrottlerTimeout = null;
        }, 10000);
      }
    },
    inactiveUserAction() {
      this.isInactive = true;
      this.active = true;

      this.popupText =
        "Wykryto brak aktywności! Proszę odświerzyć strone, aby kontynuować...";
      this.popupColor = "rgba(255,0,0,0.75)";
    },
  },
  mounted: function () {
    if (this.inactiveTime == null) {
      this.inactiveTime = 600;
      localStorage.setItem("inactive", 600);
    }
    this.update();
  },
  beforeMount() {
    this.activateActivityTracker();
  },
  beforeDestroy() {
    window.removeEventListener("mousemove", this.userActivityThrottler);
    window.removeEventListener("scroll", this.userActivityThrottler);
    window.removeEventListener("keydown", this.userActivityThrottler);
    window.removeEventListener("resize", this.userActivityThrottler);

    clearTimeout(this.userActivityTimeout);
    clearTimeout(this.userActivityThrottlerTimeout);
  },
};
</script>

<style>
.blocks {
  display: flex;
  flex-direction: row;
  flex-wrap: wrap;
}
</style>