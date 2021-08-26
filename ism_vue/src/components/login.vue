<template>
  <div class="loginParent">
    <div class="login">
      <h3>Zaloguj się</h3>
      <form>
        <input v-model="username" class="logininput" type="text" placeholder="username" />
        <input v-model="password" class="logininput" type="password" placeholder="password" />
        <button @click="log" class="loginbutton">Zaloguj</button>
      </form>
    </div>
  </div>
</template>

<script>
export default {
  data() {
    return {
      username: "",
      password: "",
    };
  },
  methods: {
    log() {
      this.$emit("loginDone", {
        username: this.username,
        password: this.password,
      });
    },
  },
};
</script>

<style>
.loginParent {
  position: fixed;
  left: 0;
  top: 0;
  padding-top: 200px;
  height: 100%;
  width: 100vw;
  z-index: 1999;
  background-color: rgba(0, 0, 0, 0.5);
}
.login {
  margin-left: auto;
  margin-right: auto;
  text-align: center;
  line-height: 2rem;
  background-color: var(--presed-color);
  width: 300px;
  padding: 0.5rem;
  border: 2px #ff0000 solid;
  border-radius: 10px;
  -webkit-box-shadow: -10px 10px 15px 0px rgba(0, 0, 0, 0.8);
  -moz-box-shadow: -10px 10px 15px 0px rgba(0, 0, 0, 0.8);
  box-shadow: -10px 10px 15px 0px rgba(0, 0, 0, 0.8);
}

.logininput::placeholder {
  color: var(--text-color);
  opacity: 0.2;
}

.logininput {
  letter-spacing: 0.1rem;
}

.loginbutton {
  font-size: 1.1rem;
  width: 10rem;
  border: 1px var(--text-color) solid !important;
}
</style>