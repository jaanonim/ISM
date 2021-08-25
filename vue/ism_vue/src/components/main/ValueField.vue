<template>
  <div>
    {{ item.name }}

    <div
      class="value valueBg"
      v-if="
        item.type === 'string' || item.type === 'hour' || item.type === 'number'
      "
    >
      {{ value }}
    </div>

    <div class="value valueBg" v-if="item.type === 'vcc'">{{ value }} V</div>

    <div class="value valueBg" v-if="item.type === 'temp'">
      {{ value }} &deg;C
    </div>

    <div class="value" v-if="item.type === 'action'">
      <button @click="action">{{ item.field }}</button>
    </div>

    <div class="value" v-if="item.type === 'redirection'">
      <button @click="newPage(item.addres)">
        {{ item.field }}
      </button>
    </div>

    <div class="value" v-if="item.type === 'bool'">
      <label class="switch">
        <input type="checkbox" :checked="value" @click="checkbox" />
        <span class="slider"></span>
      </label>
    </div>

    <div class="obj value" v-if="item.type === 'object'">
      <div v-for="item2 in item.value" :key="item2.field">
        <ValueField
          :addres="addres"
          :item="item2"
          :value="value[item2.field]"
        />
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: "ValueField",
  props: ["item", "value", "addres"],
  methods: {
    async checkbox() {
      this.value = !this.value;
      var obj = {};
      obj[this.item.action] = this.value;
      console.log(obj);
      this.$socket.emit("set", { name: this.addres, payload: obj });
    },
    async action() {
      var obj = {};
      obj[this.item.action] = true;
      this.$socket.emit("set", { name: this.addres, payload: obj });
    },
    newPage(v) {
      window.open(v, "_blank");
    },
  },
};
</script>

<style>
.value {
  display: inline-block;
  padding: 0.3rem;
  margin: 0.2rem;
  vertical-align: middle;
}

.value button {
  margin: 0 !important;
}

.valueBg {
  background-color: var(--detali2-color);
  border-radius: 5px;
}

.obj {
  margin: 10px;
  background-color: var(--presed-color);
  border-radius: 10px;
  padding: 0.5rem;
}

.switch {
  position: relative;
  display: inline-block;
  width: 2.5rem;
  height: 1.5rem;
  z-index: 0;
}

.switch input {
  opacity: 0;
  width: 0;
  height: 0;
}

.slider {
  position: absolute;
  cursor: pointer;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  background-color: red;
  -webkit-transition: 0.4s;
  transition: 0.4s;
  border-radius: 1rem;
  z-index: 0;
}

.slider:before {
  position: absolute;
  content: "";
  height: 1.4rem;
  width: 1.4rem;
  left: 0.05rem;
  bottom: 0.05rem;
  background-color: #fff;
  -webkit-transition: 0.4s;
  transition: 0.4s;
  border-radius: 50%;
  z-index: 0;
}

input:checked + .slider {
  background-color: green;
}

input:checked + .slider:before {
  -webkit-transform: translateX(1rem);
  -ms-transform: translateX(1rem);
  transform: translateX(1rem);
}
</style>