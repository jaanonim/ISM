<template>
  <div>
    <InputField
      v-for="item in inputs"
      :key="item.field"
      :model="item"
      :color="boxColor"
      :data="data[item.field]"
      ref="item"
    ></InputField>
    <button @click="save">Zapisz</button>
  </div>
</template>

<script>
import InputField from "./InputField.vue";

export default {
  props: ["block", "data"],
  components: {
    InputField,
  },
  computed: {
    boxColor: function () {
      return {
        "--border-color": this.block.color,
      };
    },
    inputs() {
      const a = [];
      this.block.value.forEach((element) => {
        if (!(element.formField === undefined) || element.type === "object") {
          a.push(element);
        }
      });
      return a;
    },
  },
  methods: {
    async save() {
      var request = "{";
      for (let i = 0; i < this.inputs.length; i++) {
        request += this.$refs.item[i].GetValue() + ", ";
      }
      request = request.substring(0, request.length - 2);
      request += "}";
      console.log({ name: this.block.addres, payload: JSON.parse(request) });
      this.$socket.emit("set", {
        name: this.block.addres,
        payload: JSON.parse(request),
      });
    },
  },
};
</script>

<style>
.timeSettings {
  text-align: center;
  background-color: var(--main-color);
  border-radius: 10px;
  margin: 20px;
  padding: 0.5rem;
  width: 500px;
  border: 2px solid var(--detali-color);
}

h5 {
  margin: 0.2rem 0 1rem 0;
  font-size: 0.7rem;
}
</style>