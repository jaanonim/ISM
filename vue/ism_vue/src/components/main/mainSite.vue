<template>
  <div>
    <div v-for="item in block.value" :key="item.field">
      <ValueField :ip="block.ip" :altip="block.altip" :port="block.port" :item="item" :value="data[item.field]" :token="token" />
    </div>
  </div>
</template>

<script>
import ValueField from "./ValueField.vue";
export default {
  props: ["data", "block","token"],
  components: {
    ValueField
  }
};
</script>

<style>
</style>