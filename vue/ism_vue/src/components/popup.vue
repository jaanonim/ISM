<template>
  <div class="popupParent">
    <div class="popup">{{text}}</div>
  </div>
</template>

<script>
export default {
  props: ["text"]
};
</script>

<style>
.popupParent {
  position: fixed;
  left: 0;
  top: 200px;
  width: 100vw;
  z-index: 2000;
}
.popup {
  margin-left: auto;
  margin-right: auto;
  text-align: center;
  background-color: var(--popup-color);
  width: 300px;
  padding: 0.5rem;
  border-radius: 5px;
  -webkit-box-shadow: -10px 10px 15px 0px rgba(0, 0, 0, 0.8);
  -moz-box-shadow: -10px 10px 15px 0px rgba(0, 0, 0, 0.8);
  box-shadow: -10px 10px 15px 0px rgba(0, 0, 0, 0.8);
}
</style>