<template>
  <div class="sidebar">
    <div @click="closeSidebarPanel" v-if="isNavBar"></div>
    <transition name="slide">
      <div v-if="isNavBar" class="sidebar-panel">
        <ul class="sidebar-panel-nav">
          <li>
            <a @click="changePage(0)">Strona Główna</a>
          </li>
          <li>
            <a @click="newPage(blocks[0])">I.S.M.A.</a>
          </li>
          <li>
            <a @click="newPage(blocks[1])">I.S.M.P.</a>
          </li>
          <li>
            <a @click="changePage(1)">Ustawienia</a>
          </li>
          <li>
            <a @click="changeTheam()">Motyw</a>
          </li>
        </ul>
        <footer class="footer">2020 &copy; I.S.M.</footer>
      </div>
    </transition>
  </div>
</template>

<script>
export default {
  methods: {
    closeSidebarPanel(){
      this.$emit('hide')
    },
    changePage(i)
    {
      this.$emit('changePage',i)
    },
    newPage(v){
      window.open("http://"+v.ip+v.port,"_blank");
    },
    changeTheam(){
      this.$emit("changeTheam")
    }
  },
  props: ['isNavBar','blocks']
};
</script>


<style>
.slide-enter-active,
.slide-leave-active {
  transition: transform 0.2s ease;
}

.slide-enter,
.slide-leave-to {
  transform: translateX(-100%);
  transition: all 150ms ease-in 0s;
}

.sidebar-panel {
  overflow-y: auto;
  background-color: var(--main-color);
  position: fixed;
  left: 0;
  top: 0;
  height: 100vh;
  z-index: 999;
  width: 350px;
}

ul.sidebar-panel-nav {
  margin: 10px;
  padding: 2rem 0 0 0;
  list-style-type: none;
}

ul.sidebar-panel-nav a{
  border-radius: 10px;
  margin: 10px;
  text-align: center;
  padding: 5px;
  color: var(--text-color);
  text-decoration: none;
  font-size: 1.2rem;
  display: block;
  transition: transform 0.2s ease-in-out;
}

ul.sidebar-panel-nav > li > a > ol
{
  padding: 0 !important;
	list-style-type: none;
	padding: 0;
	margin: 0;
	display: none;
}

ul.sidebar-panel-nav a {
  background-color: var(--detali-color);
  cursor: pointer;
}

ul.sidebar-panel-nav ol a
{
  background-color: var(--detali2-color);
}

ul.sidebar-panel-nav > li:hover > a > ol
{
	display: block;
}

ul.sidebar-panel-nav a:hover{
  transform: scale(1.05);
  background-color: var(--presed-color);
}

ul.sidebar-panel-nav ol a:hover
{
  transform: scale(1.05);
	background-color: var(--presed2-color);
}

.footer {
  position: absolute;
  margin: auto 0 0 0;
  bottom: 0;
  height: 1rem;
  width: 100%;
  background-color: var(--presed-color);
  text-align: center;
  padding: 5px 0 5px 0;
  z-index: 100;
  font-size: 12px;
}
</style>