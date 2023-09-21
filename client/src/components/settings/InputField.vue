<template>
  <div class="inputField">
    {{model.name}}
    <input
      v-if="model.type==='hour'"
      type="time"
      v-model="input"
      :style="color"
      required="required"
    />
    <input
      v-if="model.type==='number'"
      type="number"
      v-model="input"
      :style="color"
      required="required"
    />
    <div v-if="model.type==='object'" class="inputObj">
      <InputField
        v-for="item in inputs"
        :key="item.field"
        :model="item"
        :color="color"
        :data="data[item.field]"
        ref="item"
      ></InputField>
    </div>
  </div>
</template>

<script>
export default {
  props: ["model", "color", "data"],
  name: "InputField",
  data() {
    return {
      input: "",
      inputs: []
    };
  },
  mounted: function() {
    if (this.model.type === "object") {
      this.model.value.forEach(element => {
        if (!(element.formField === undefined) || element.type === "object") {
          this.inputs.push(element);
        }
      });
    } else {
      this.input = this.data;
    }
  },
  methods: {
    GetValue() {
      if (this.model.type === "object") {
        var some = "";
        for (let i = 0; i < this.inputs.length; i++) {
          some += this.$refs.item[i].GetValue() + ", ";
        }
        return some.substring(0, some.length-2);
      } else {
        if(this.model.type==="number")
        {
          return "\""+this.model.formField + "\" : " + this.input;
        }
        else{
          return "\""+this.model.formField + "\" : \"" + this.input + "\"";
        }
      }
    }
  }
};
</script>

<style>
.inputField {
  text-align: center;
  line-height: 2rem;
}
input {
  color: var(--text-color);
  border: 0px;
  border-bottom: 2px solid var(--text-color);
  background-color: transparent;
  text-align: center;
  height: 1.5rem;
  font-size: 1.01rem;
  margin: 0.5rem 1rem;
  font-family: var(--font);
}
input[type="time"] {
  width: 6rem;
}
input[type="number"] {
  width: 5rem;
}
.inputObj {
  background-color: var(--presed-color);
  border-radius: 10px;
  display: inline-block;
  padding: 0.3rem;
  margin: 0.2rem;
  vertical-align: middle;
}
input:hover {
  border-color: var(--border-color);
}
input:focus {
  border-color: var(--border-color);
  outline: 0;
}
input::-webkit-inner-spin-button,
input::-webkit-calendar-picker-indicator {
  display: none;
  -webkit-appearance: none;
}
</style>