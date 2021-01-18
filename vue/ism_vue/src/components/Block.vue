<template>
  <section class="Block" :style="boxColor">
    <div class="left">
      <div class="leftBorder">
        <div class="space">
          <h3>{{ block.title }}</h3>
          <br />Status:
          <div class="status" :class="{ on: status, loading: loading }"></div>
          <br />
          <button @click="getData">Sych</button>
          <p style="color: #ff0000">{{ error }}</p>
        </div>
      </div>
    </div>
    <div class="rigth">
      <div class="space">
        <p v-if="!isSet">Dane nie są dostepne.</p>
        <div v-else>
          <mainSite
            v-if="page === 0"
            :block="block"
            :data="data"
            :token="token"
          ></mainSite>
          <TimerSettings
            v-if="page == 1"
            :block="block"
            :data="data"
            :token="token"
            @popup="popup"
            @update="getData"
          ></TimerSettings>
        </div>
      </div>
    </div>
    <div style="clear: both"></div>
  </section>
</template>

<script>
import axios from "axios";
import mainSite from "./main/mainSite.vue";
import TimerSettings from "./settings/TimerSettings.vue";

export default {
  props: ["block", "page", "isInactive", "token"],
  components: {
    TimerSettings,
    mainSite,
  },
  data() {
    return {
      time: 0,
      updateTime: localStorage.getItem("update"),
      status: false,
      loading: false,
      isSet: false,
      data: undefined,
      error: "",
    };
  },
  mounted: function () {
    this.update();
    if (this.updateTime == null) {
      this.updateTime = 60;
      localStorage.setItem("update", 60);
    }
  },
  computed: {
    boxColor: function () {
      return {
        "border-color": this.block.color,
      };
    },
  },
  methods: {
    async getData() {
      this.time = 0;
      this.loading = true;
      try {
        const res = await axios
          .get(window.location.origin + this.block.endpoint + "/api", {
            headers: { Authorization: "JWT " + this.token },
          })
          .then((response) => (this.info = response.data))
          .finally(() => (this.loading = false));

        this.data = res;
        this.error = "";
        this.isSet = true;
        this.status = true;
      } catch (e) {
        this.error = e;
        this.status = false;
        this.isSet = false;
        this.loading = false;
      }
    },
    update() {
      if (this.isInactive) {
        return;
      }
      if (this.time >= this.updateTime) {
        this.getData();
      }
      this.time += 1;
      setTimeout(() => {
        this.update();
      }, 1000);
    },
    popup(v) {
      this.$emit("popup", v);
    },
  },
};
</script>

<style>
.Block {
  max-width: 500px;
  min-height: 300px;
  background-color: var(--detali-color);
  border-radius: 15px;
  padding: 10px;
  margin: 20px;
  text-align: center;
  border: 2px solid;
}

.left {
  line-height: 1.9rem;
}

.leftBorder {
  width: 100%;
  height: 100%;
}

@media only screen and (min-width: 601px) {
  .Block {
    display: flex;
  }

  .rigth {
    width: 60%;
    height: 100%;
  }

  .left {
    width: 40%;
    height: 100%;
  }

  .leftBorder {
    border-right: 2px dotted var(--text-color);
  }
}

@media only screen and (max-width: 600px) {
  .Block {
    display: block;
  }

  .rigth {
    width: 100%;
    height: auto;
  }

  .left {
    width: 100%;
    height: auto;
  }

  .leftBorder {
    border-bottom: 2px dotted var(--text-color);
  }
}

.space {
  padding: 1rem 0.5rem 1rem 0.5rem;
}

h3 {
  padding: 0.5rem;
  margin: 0;
}

.status {
  height: 10px;
  width: 10px;
  border-radius: 5px;
  display: inline-block;
  background-color: red;
  margin-left: 10px;
}

.on {
  background-color: green;
}

.loading {
  background-color: #00d9ff;
}
</style>