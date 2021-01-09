<template>
  <div class="local">
    <h3>Ustawienia Lokalne:</h3>Odświerzaj dane co:
    <input type="number" v-model="update" />s.
    <br />Maksymany czas nieaktywności:
    <input type="number" v-model="inactive" />s.
    <br />
    <button @click="save">Zapisz</button>
  </div>
</template>

<script>
export default {
  data() {
    return {
      update: localStorage.getItem("update"),
      inactive: localStorage.getItem("inactive")
    };
  },
  methods: {
    save() {
      this.$emit("setUpdate", this.update);
      this.$emit("setInactive", this.inactive);
      localStorage.setItem("update", this.update);
      localStorage.setItem("inactive", this.inactive);
      this.$emit("popup", "");
    }
  },
  mounted: function() {
    if (this.update == null) {
      this.update = 60;
      localStorage.setItem("update", 60);
    }
    if (this.inactive == null) {
      this.inactive = 600;
      localStorage.setItem("inactive",600);
    }
  }
};
</script>

<style>
.local h3 {
  padding: 1rem 0 2rem 0;
}

.local {
  max-width: 500px;
  background-color: var(--detali-color);
  border-radius: 15px;
  padding: 10px 25px ;
  margin: 20px;
  text-align: center;
  border: 2px solid var(--text-color);

  --border-color: var(--presed2-color)
}
</style>