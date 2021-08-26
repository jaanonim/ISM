<template>
  <header class="bar" @click="changePage(0)">
    <p>I.S.M.</p>
  </header>
</template>

<script>
export default {
  methods: {
    changePage(i) {
      this.$emit("changePage", i);
    }
  }
};
</script>

<style>
.bar {
  position: fixed;
  top: 0;
  left: 0;
  background-color: var(--detali-color);
  text-align: center;
  width: 100vw;
  font-size: 1.5rem;
  letter-spacing: 5px;
  z-index: 10;
  cursor: pointer;
}

.bar p {
  padding: 0.5rem;
  margin: 0;
}
</style>