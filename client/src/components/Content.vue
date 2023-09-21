<template>
  <div class="conteinter" :class="{withNavbar: isNavBar}">
    <main>
      <MainComponent :page="page" :blocks="blocks"></MainComponent>
    </main>
  </div>
</template>

<script>
import Main from "./Main.vue";

export default {
  props: ["isNavBar", "blocks", "page"],
  components: {
    MainComponent: Main
  },
};
</script>

<style>
.conteinter {
  padding-top: 3rem;
  width: auto;
  height: auto;
}

@media only screen and (min-width: 800px) {
  .withNavbar {
    padding-left: 350px;
  }
}

main {
  padding: 2rem;
}

.conteinter button {
  padding: 10px;
  margin: 1rem;
  background-color: var(--detali2-color);
  color: var(--text-color);
  border-radius: 5px;
  border: 0px;
}

.conteinter button:hover {
  background-color: var(--presed2-color);
  transform: scale(1.05);
}
</style>