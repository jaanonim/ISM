<template>
  <div>
    <div v-for="item in block.value" :key="item.field">
      <ValueField
        :addres="block.addres"
        :item="item"
        :value="data[item.field]"
      />
    </div>
  </div>
</template>

<script>
import ValueField from "./ValueField.vue";
export default {
  props: ["data", "block"],
  components: {
    ValueField,
  },
};
</script>

<style>
</style>