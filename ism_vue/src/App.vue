<template>
  <div id="app" :class="theam">
    <TopBar @changePage="changePage" />
    <LeftBar
      @hide="hide"
      @changePage="changePage"
      @changeTheam="changeTheam"
      :isNavBar="isNavBar"
      :blocks="blocks"
    />
    <Burger @hide="hide" :isNavBar="isNavBar" />
    <Content :isNavBar="isNavBar" :blocks="blocks" :page="page" />
  </div>
</template>

<script>
import LeftBar from "./components/nav/LeftBar.vue";
import Burger from "./components/nav/Burger.vue";
import TopBar from "./components/nav/TopBar.vue";
import Content from "./components/Content.vue";

import json from "./data.json";

export default {
  name: "App",
  components: {
    LeftBar,
    Burger,
    TopBar,
    Content
  },
  data() {
    return {
      isNavBar: false,
      page: 0,
      theam: localStorage.getItem("theme"),
      blocks: json
    };
  },
  mounted: function() {
    if (this.theam == null) {
      this.theam = "dark";
      localStorage.setItem("theme", "dark");
    }
  },
  methods: {
    hide() {
      this.isNavBar = !this.isNavBar;
    },
    changePage: function(i) {
      this.page = i;
    },
    changeTheam() {
      if (this.theam === "dark") {
        this.theam = "ligth";
        localStorage.setItem("theme", "ligth");
      } else {
        this.theam = "dark";
        localStorage.setItem("theme", "dark");
      }
    }
  }
};
</script>

<style>
html,
body {
  min-height: 100vh;
  margin: 0;
}

#app {
  min-height: 100vh;
  width: 100%;
  color: var(--text-color);
  background-color: var(--bg-color);
}

:root {
  font-size: 16px;
  font-family: var(--font);
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;

  --font: "Avenir", Helvetica, Arial, sans-serif;
}

.dark {
  --text-color: #fff;
  --bg-color: #333;
  --main-color: #272727;
  --detali-color: #1c1c1c;
  --presed-color: #151515;
  --detali2-color: #111111;
  --presed2-color: #0a0a0a;
}

.ligth {
  --text-color: #000;
  --bg-color: #fff;
  --main-color: #e7e7e7;
  --detali-color: #eee;
  --presed-color: #d5d5d5;
  --detali2-color: #ddd;
  --presed2-color: #ccc;
}

</style>